#ifndef MK_FREEVERB_FILTER_HPP
#define MK_FREEVERB_FILTER_HPP

#include <cmath>
#include <algorithm>

// Simple biquad filter for reverb input processing
// Uses same algorithms as liquidsfz for consistency
class Filter
{
public:
    enum Type {
        NONE,
        Lowpass,
        Highpass
    };

    Filter() : type_(NONE), sample_rate_(48000.0f) {
        reset(NONE, 48000.0f);
    }

    void reset(Type type, float sample_rate) {
        type_ = type;
        sample_rate_ = sample_rate;
        // Reset filter state
        x1_ = x2_ = y1_ = y2_ = 0.0f;
        first_ = true;
        last_cutoff_ = 0.0f;
        last_resonance_ = 0.0f;
        update_coefficients();
    }

    void setCutoff(float freq) {
        cutoff_ = std::max(freq, 10.0f);  // Same as liquidsfz minimum
        update_coefficients();
    }

    void setResonance(float res) {
        resonance_ = res;
        update_coefficients();
    }

    float process(float input) {
        if (type_ == NONE) {
            return input;
        }

        float output = b0_ * input + b1_ * x1_ + b2_ * x2_ - a1_ * y1_ - a2_ * y2_;
        
        // Shift delay line
        x2_ = x1_;
        x1_ = input;
        y2_ = y1_;
        y1_ = output;

        return output;
    }

private:
    // Fast dB to factor conversion (same as liquidsfz)
    float fast_db_to_factor(float db) {
        return exp2f(db * 0.166096404744368f);
    }

    void update_coefficients() {
        if (type_ == NONE || cutoff_ <= 0.0f) {
            b0_ = 1.0f; b1_ = b2_ = a1_ = a2_ = 0.0f;
            return;
        }

        // Parameter smoothing (same logic as liquidsfz)
        float cutoff = cutoff_;
        float resonance = resonance_;
        
        if (first_) {
            first_ = false;
        } else if (cutoff == last_cutoff_ && resonance == last_resonance_) {
            // Fast case: no need to redesign if parameters didn't change
            return;
        } else {
            // Parameter smoothing
            const float cutoff_smooth = 1.2f;  // Same as liquidsfz for order 2
            const float reso_smooth = 1.0f;
            
            const float high = cutoff_smooth;
            const float low = 1.0f / high;
            
            cutoff = std::clamp(cutoff, last_cutoff_ * low, last_cutoff_ * high);
            resonance = std::clamp(resonance, last_resonance_ - reso_smooth, last_resonance_ + reso_smooth);
        }
        
        last_cutoff_ = cutoff;
        last_resonance_ = resonance;

        // Same math as liquidsfz: normalize cutoff by sample rate
        float norm_cutoff = std::min(cutoff / sample_rate_, 0.49f);

        // 2 pole design from DAFX 2nd ed., Zoelzer (same as liquidsfz)
        const float k = tanf(M_PI * norm_cutoff);
        const float kk = k * k;
        const float q = fast_db_to_factor(resonance);  // Convert dB to linear (same as liquidsfz)
        const float div_factor = 1.0f / (1.0f + (k + 1.0f / q) * k);

        a1_ = 2.0f * (kk - 1.0f) * div_factor;
        a2_ = (1.0f - k / q + kk) * div_factor;

        if (type_ == Lowpass) {
            b0_ = kk * div_factor;
            b1_ = 2.0f * b0_;
            b2_ = b0_;
        } else if (type_ == Highpass) {
            b0_ = div_factor;
            b1_ = -2.0f * div_factor;
            b2_ = div_factor;
        } else {
            b0_ = 1.0f; b1_ = b2_ = a1_ = a2_ = 0.0f;
        }
    }

    Type type_;
    float sample_rate_;
    float cutoff_ = 1000.0f;
    float resonance_ = 0.0f;  // In dB (same as liquidsfz)

    // Parameter smoothing state (same as liquidsfz)
    bool first_ = true;
    float last_cutoff_ = 0.0f;
    float last_resonance_ = 0.0f;

    // Biquad coefficients
    float b0_ = 1.0f, b1_ = 0.0f, b2_ = 0.0f;
    float a1_ = 0.0f, a2_ = 0.0f;

    // Delay line
    float x1_ = 0.0f, x2_ = 0.0f;
    float y1_ = 0.0f, y2_ = 0.0f;
};

#endif // MK_FREEVERB_FILTER_HPP