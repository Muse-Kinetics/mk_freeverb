#ifndef _mk_freeverb_
#define _mk_freeverb_

#include "comb.hpp"
#include "allpass.hpp"
#include "filter.hpp"
#include "tuning.h"
#include "mk_freeverb_presets.h"

// Real-time safety configuration
#ifndef MK_FREEVERB_ENABLE_PREDELAY
#define MK_FREEVERB_ENABLE_PREDELAY 0  // Disable predelay for RT safety
#endif

#ifndef MK_FREEVERB_ENABLE_INPUT_FILTER
#define MK_FREEVERB_ENABLE_INPUT_FILTER 0  // Disable input filtering for RT safety
#endif

#ifndef MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE
#define MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE 0  // Disable crossfading for RT safety
#endif

#ifndef MK_FREEVERB_NUM_COMBS
#define MK_FREEVERB_NUM_COMBS 4  // Reduce from 8 to 4 for better performance
#endif

// Predelay buffer size in samples (max ~100ms at 48kHz)
#define MK_FREEVERB_MAX_PREDELAY_SAMPLES 4800

class mk_freeverb
{
public:
    mk_freeverb();

    void mute();

    void processreplace(float *inputL, float *inputR, float *outputL, float *outputR, long numsamples, int skip);

    void setroomsize(float value);
    float getroomsize();

    void setdamp(float value);
    float getdamp();

    void setwet(float value);
    float getwet();

    void setdry(float value);
    float getdry();

    void setwidth(float value);
    float getwidth();

    void setmode(float value);
    float getmode();

#if MK_FREEVERB_ENABLE_PREDELAY
    void set_predelay(float seconds);
#endif

#if MK_FREEVERB_ENABLE_INPUT_FILTER
    void set_input_filter(float cutoff, float resonance);
#endif

    void set_sample_rate(float sr) { sampleRate = sr; }

    // Enhanced preset system
    void apply_preset(const ReverbPreset& preset);
    void queue_preset(const ReverbPreset& preset);
    
    // Convenience methods for common presets
    void load_default_preset() { apply_preset(ReverbPresets::DEFAULT_PRESET); }
    void load_preset_by_index(int index);
    
    // Legacy method for backward compatibility (simplified for RT safety)
    void queue_preset(float newRoom, float newDamp, float newWet, float newDry,
                      float newWidth, float newMode
#if MK_FREEVERB_ENABLE_PREDELAY
                      , float newPreDelay
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
                      , float newCutoff, float newResonance
#endif
                      );

private:
    void update();
    void apply_preset_internal(const ReverbPreset& preset);

    float gain;
    float roomsize, roomsize1;
    float damp, damp1;
    float wet, wet1, wet2;
    float dry;
    float width;
    float mode;

    // Comb filters (configurable count)
    comb combL[MK_FREEVERB_NUM_COMBS];
    comb combR[MK_FREEVERB_NUM_COMBS];

    // Allpass filters
    allpass allpassL[numallpasses];
    allpass allpassR[numallpasses];

    // Comb buffers (only create what we need)
    float bufcombL1[combtuningL1];
    float bufcombR1[combtuningR1];
    float bufcombL2[combtuningL2];
    float bufcombR2[combtuningR2];
    float bufcombL3[combtuningL3];
    float bufcombR3[combtuningR3];
    float bufcombL4[combtuningL4];
    float bufcombR4[combtuningR4];
#if MK_FREEVERB_NUM_COMBS > 4
    float bufcombL5[combtuningL5];
    float bufcombR5[combtuningR5];
    float bufcombL6[combtuningL6];
    float bufcombR6[combtuningR6];
    float bufcombL7[combtuningL7];
    float bufcombR7[combtuningR7];
    float bufcombL8[combtuningL8];
    float bufcombR8[combtuningR8];
#endif

    // Allpass buffers
    float bufallpassL1[allpasstuningL1];
    float bufallpassR1[allpasstuningR1];
    float bufallpassL2[allpasstuningL2];
    float bufallpassR2[allpasstuningR2];
    float bufallpassL3[allpasstuningL3];
    float bufallpassR3[allpasstuningR3];
    float bufallpassL4[allpasstuningL4];
    float bufallpassR4[allpasstuningR4];

#if MK_FREEVERB_ENABLE_INPUT_FILTER
    // Input filter (optional for RT safety)
    Filter input_filter;
#endif

#if MK_FREEVERB_ENABLE_PREDELAY
    // Static predelay buffers (RT-safe)
    float predelayBufferL[MK_FREEVERB_MAX_PREDELAY_SAMPLES];
    float predelayBufferR[MK_FREEVERB_MAX_PREDELAY_SAMPLES];
    size_t predelaySize = 0;
    size_t predelayWrite = 0;
    size_t predelayRead = 0;

#if MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE
    // For crossfade - use second static buffer instead of dynamic allocation
    float predelayBufferL_new[MK_FREEVERB_MAX_PREDELAY_SAMPLES];
    float predelayBufferR_new[MK_FREEVERB_MAX_PREDELAY_SAMPLES];
    size_t predelaySize_new = 0;
    size_t predelayWrite_new = 0;
    size_t predelayRead_new = 0;
    int fadeSamples = 0;
    int fadeCount = 0;
#endif
#endif

    // Sample rate for timing
    float sampleRate = 44100.0f;

    // Enhanced preset handling (RT-safe)
    bool presetPending = false;
    ReverbPreset pendingPreset = ReverbPresets::DEFAULT_PRESET;
};

#endif // _mk_freeverb_