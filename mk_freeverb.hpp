#ifndef _mk_freeverb_
#define _mk_freeverb_

#include "comb.hpp"
#include "allpass.hpp"
#include "filter.hpp"
#include "tuning.h"
#include "mk_freeverb_config.h"
#include "mk_freeverb_presets.h"
#include <memory>

class mk_freeverb
{
public:
    mk_freeverb(float sr = 48000.0f);
    
    // Initialize with default preset - call this after construction to avoid static init issues
    void initialize();

    void mute();

    void processreplace(float *inputL, float *inputR, float *outputL, float *outputR, long numsamples, int skip);

    void setRoomSize(float value);
    float getRoomSize();

    void setDamp(float value);
    float getDamp();

    void setWet(float value);
    float getWet();

    void setDry(float value);
    float getDry();

    void setWidth(float value);
    float getWidth();

    void setMode(float value);
    float getMode();

    void setLPCutoff(float cutoff);

    void setPredelay(float seconds);
    float getPredelay();

#if MK_FREEVERB_ENABLE_INPUT_FILTER
    void set_input_filter(float cutoff, float resonance);
#endif

    void set_sample_rate(float sr) 
    { 
        sampleRate = sr; 
#if MK_FREEVERB_ENABLE_INPUT_FILTER
        Filter::Type filter_type = input_filter->getType();
        input_filter->reset(filter_type, sr);
#endif
    }

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
    float roomSize, roomSize1;
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
    std::shared_ptr<Filter> input_filter = nullptr;
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