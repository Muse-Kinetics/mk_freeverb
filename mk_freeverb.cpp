#include "mk_freeverb.hpp"

mk_freeverb::mk_freeverb()
{
    // Initialize comb filters (only the ones we're using)
    combL[0].setbuffer(bufcombL1, combtuningL1);
    combR[0].setbuffer(bufcombR1, combtuningR1);
    combL[1].setbuffer(bufcombL2, combtuningL2);
    combR[1].setbuffer(bufcombR2, combtuningR2);
    combL[2].setbuffer(bufcombL3, combtuningL3);
    combR[2].setbuffer(bufcombR3, combtuningR3);
    combL[3].setbuffer(bufcombL4, combtuningL4);
    combR[3].setbuffer(bufcombR4, combtuningR4);

#if MK_FREEVERB_NUM_COMBS > 4
    combL[4].setbuffer(bufcombL5, combtuningL5);
    combR[4].setbuffer(bufcombR5, combtuningR5);
    combL[5].setbuffer(bufcombL6, combtuningL6);
    combR[5].setbuffer(bufcombR6, combtuningR6);
    combL[6].setbuffer(bufcombL7, combtuningL7);
    combR[6].setbuffer(bufcombR7, combtuningR7);
    combL[7].setbuffer(bufcombL8, combtuningL8);
    combR[7].setbuffer(bufcombR8, combtuningR8);
#endif

    allpassL[0].setbuffer(bufallpassL1, allpasstuningL1);
    allpassR[0].setbuffer(bufallpassR1, allpasstuningR1);
    allpassL[1].setbuffer(bufallpassL2, allpasstuningL2);
    allpassR[1].setbuffer(bufallpassR2, allpasstuningR2);
    allpassL[2].setbuffer(bufallpassL3, allpasstuningL3);
    allpassR[2].setbuffer(bufallpassR3, allpasstuningR3);
    allpassL[3].setbuffer(bufallpassL4, allpasstuningL4);
    allpassR[3].setbuffer(bufallpassR4, allpasstuningR4);

    allpassL[0].setfeedback(0.5f);
    allpassR[0].setfeedback(0.5f);
    allpassL[1].setfeedback(0.5f);
    allpassR[1].setfeedback(0.5f);
    allpassL[2].setfeedback(0.5f);
    allpassR[2].setfeedback(0.5f);
    allpassL[3].setfeedback(0.5f);
    allpassR[3].setfeedback(0.5f);

    setwet(0.3f);
    setroomsize(0.5f);
    setdry(0.7f);
    setdamp(0.5f);
    setwidth(1.0f);
    setmode(0.0f);

    // Apply default preset for consistent initialization
    apply_preset(ReverbPresets::DEFAULT_PRESET);

#if MK_FREEVERB_ENABLE_INPUT_FILTER
    input_filter.reset(Filter::Type::Lowpass, sampleRate);
    input_filter.setCutoff(8000.f);
    input_filter.setResonance(0.5f);
#endif

#if MK_FREEVERB_ENABLE_PREDELAY
    // Initialize predelay buffers to zero
    for (size_t i = 0; i < MK_FREEVERB_MAX_PREDELAY_SAMPLES; i++) {
        predelayBufferL[i] = 0.0f;
        predelayBufferR[i] = 0.0f;
#if MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE
        predelayBufferL_new[i] = 0.0f;
        predelayBufferR_new[i] = 0.0f;
#endif
    }
    predelaySize = 0;
    predelayWrite = 0;
    predelayRead = 0;
#if MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE
    predelaySize_new = 0;
    predelayWrite_new = 0;
    predelayRead_new = 0;
    fadeSamples = 0;
    fadeCount = 0;
#endif
#endif

    mute();
}

void mk_freeverb::mute()
{
    if (getmode() >= freezemode) return;

    for (int i = 0; i < MK_FREEVERB_NUM_COMBS; i++)
    {
        combL[i].mute();
        combR[i].mute();
    }
    for (int i = 0; i < numallpasses; i++)
    {
        allpassL[i].mute();
        allpassR[i].mute();
    }
}

void mk_freeverb::processreplace(float *inputL, float *inputR, float *outputL, float *outputR, long numsamples, int skip)
{
    if (presetPending)
    {
        apply_preset_internal(pendingPreset);
        presetPending = false;
    }

    while (numsamples-- > 0)
    {
        float inL = *inputL;
        float inR = *inputR;

#if MK_FREEVERB_ENABLE_INPUT_FILTER
        // Apply input filtering if enabled
        inL = input_filter.process(inL);
        inR = input_filter.process(inR);
#endif

        float predelayedL = inL;
        float predelayedR = inR;

#if MK_FREEVERB_ENABLE_PREDELAY
        // Predelay processing
        if (predelaySize > 0)
        {
#if MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE
            // Complex crossfade version (not RT-safe due to dynamic allocation)
            if (fadeCount > 0)
            {
                predelayBufferL[predelayWrite] = inL;
                predelayBufferR[predelayWrite] = inR;

                predelayBufferL_new[predelayWrite_new] = inL;
                predelayBufferR_new[predelayWrite_new] = inR;

                float oldL = predelayBufferL[predelayRead];
                float oldR = predelayBufferR[predelayRead];

                float newL = predelayBufferL_new[predelayRead_new];
                float newR = predelayBufferR_new[predelayRead_new];

                float fade = static_cast<float>(fadeCount) / fadeSamples;
                predelayedL = oldL * fade + newL * (1.0f - fade);
                predelayedR = oldR * fade + newR * (1.0f - fade);

                predelayWrite = (predelayWrite + 1) % predelaySize;
                predelayRead = (predelayRead + 1) % predelaySize;
                predelayWrite_new = (predelayWrite_new + 1) % predelaySize_new;
                predelayRead_new = (predelayRead_new + 1) % predelaySize_new;

                fadeCount--;
                if (fadeCount == 0)
                {
                    // Swap the buffers (RT-safe with static arrays)
                    for (size_t i = 0; i < predelaySize_new; i++) {
                        predelayBufferL[i] = predelayBufferL_new[i];
                        predelayBufferR[i] = predelayBufferR_new[i];
                    }
                    predelaySize = predelaySize_new;
                    predelayWrite = predelayWrite_new;
                    predelayRead = predelayRead_new;
                }
            }
            else
            {
                // Normal predelay operation
                predelayBufferL[predelayWrite] = inL;
                predelayBufferR[predelayWrite] = inR;

                predelayedL = predelayBufferL[predelayRead];
                predelayedR = predelayBufferR[predelayRead];

                predelayWrite = (predelayWrite + 1) % predelaySize;
                predelayRead = (predelayRead + 1) % predelaySize;
            }
#else
            // Simple predelay (RT-safe, no crossfading)
            predelayBufferL[predelayWrite] = inL;
            predelayBufferR[predelayWrite] = inR;

            predelayedL = predelayBufferL[predelayRead];
            predelayedR = predelayBufferR[predelayRead];

            predelayWrite = (predelayWrite + 1) % predelaySize;
            predelayRead = (predelayRead + 1) % predelaySize;
#endif
        }
#endif

        float input = (predelayedL + predelayedR) * gain;

        float outL = 0, outR = 0;
        for (int i = 0; i < MK_FREEVERB_NUM_COMBS; i++)
        {
            outL += combL[i].process(input);
            outR += combR[i].process(input);
        }

        for (int i = 0; i < numallpasses; i++)
        {
            outL = allpassL[i].process(outL);
            outR = allpassR[i].process(outR);
        }

        *outputL = outL * wet1 + outR * wet2 + *inputL * dry;
        *outputR = outR * wet1 + outL * wet2 + *inputR * dry;

        inputL += skip;
        inputR += skip;
        outputL += skip;
        outputR += skip;
    }
}

void mk_freeverb::setroomsize(float value) { roomsize = value; update(); }
float mk_freeverb::getroomsize() { return roomsize; }

void mk_freeverb::setdamp(float value) { damp = value; update(); }
float mk_freeverb::getdamp() { return damp; }

void mk_freeverb::setwet(float value) { wet = value; update(); }
float mk_freeverb::getwet() { return wet; }

void mk_freeverb::setdry(float value) { dry = value; }
float mk_freeverb::getdry() { return dry; }

void mk_freeverb::setwidth(float value) { width = value; update(); }
float mk_freeverb::getwidth() { return width; }

void mk_freeverb::setmode(float value) { mode = value; update(); }
float mk_freeverb::getmode() { return mode; }

#if MK_FREEVERB_ENABLE_PREDELAY
void mk_freeverb::set_predelay(float seconds)
{
    size_t newSize = static_cast<size_t>(sampleRate * seconds);
    
    // Clamp to maximum buffer size for RT safety
    if (newSize > MK_FREEVERB_MAX_PREDELAY_SAMPLES) {
        newSize = MK_FREEVERB_MAX_PREDELAY_SAMPLES;
    }
    
#if MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE
    // Crossfade version (RT-safe with static buffers)
    if (newSize == predelaySize) return;

    // Clear new buffer
    for (size_t i = 0; i < newSize; i++) {
        predelayBufferL_new[i] = 0.0f;
        predelayBufferR_new[i] = 0.0f;
    }
    
    predelaySize_new = newSize;
    predelayWrite_new = 0;
    predelayRead_new = 0;
    fadeSamples = static_cast<int>(0.02f * sampleRate);  // 20ms crossfade
    fadeCount = fadeSamples;
#else
    // Simple immediate change (RT-safe, no crossfading)
    predelaySize = newSize;
    predelayWrite = 0;
    predelayRead = 0;
    
    // Clear the buffer
    for (size_t i = 0; i < predelaySize; i++) {
        predelayBufferL[i] = 0.0f;
        predelayBufferR[i] = 0.0f;
    }
#endif
}
#endif

#if MK_FREEVERB_ENABLE_INPUT_FILTER
void mk_freeverb::set_input_filter(float cutoff, float resonance)
{
    input_filter.setCutoff(cutoff);
    input_filter.setResonance(resonance);
}
#endif

void mk_freeverb::update()
{
    wet1 = wet * (width / 2 + 0.5f);
    wet2 = wet * ((1 - width) / 2);

    if (mode >= freezemode)
    {
        roomsize1 = 1;
        damp1 = 0;
        gain = muted;
    }
    else
    {
        roomsize1 = roomsize;
        damp1 = damp;
        gain = fixedgain;
    }

    for (int i = 0; i < MK_FREEVERB_NUM_COMBS; i++)
    {
        combL[i].setfeedback(roomsize1);
        combR[i].setfeedback(roomsize1);
        combL[i].setdamp(damp1);
        combR[i].setdamp(damp1);
    }
}

void mk_freeverb::queue_preset(float newRoom, float newDamp, float newWet, float newDry,
                            float newWidth, float newMode
#if MK_FREEVERB_ENABLE_PREDELAY
                            , float newPreDelay
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
                            , float newCutoff, float newResonance
#endif
                            )
{
    // Create temporary preset from individual parameters (legacy compatibility)
    ReverbPreset legacyPreset(newRoom, newDamp, newWet, newDry, newWidth, newMode
#if MK_FREEVERB_ENABLE_PREDELAY
                             , newPreDelay
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
                             , newCutoff, newResonance
#endif
                             );
    
    queue_preset(legacyPreset);
}

// Enhanced preset methods
void mk_freeverb::apply_preset(const ReverbPreset& preset)
{
    // Apply immediately (for initialization or non-RT thread)
    apply_preset_internal(preset);
}

void mk_freeverb::queue_preset(const ReverbPreset& preset)
{
    // Queue for RT-safe application in audio thread
    pendingPreset = preset;
    presetPending = true;
}

void mk_freeverb::load_preset_by_index(int index)
{
    if (index >= 0 && index < ReverbPresets::NUM_PRESETS) {
        apply_preset(*ReverbPresets::ALL_PRESETS[index]);
    }
}

void mk_freeverb::apply_preset_internal(const ReverbPreset& preset)
{
    setroomsize(preset.roomsize);
    setdamp(preset.damp);
    setwet(preset.wet);
    setdry(preset.dry);
    setwidth(preset.width);
    setmode(preset.mode);
#if MK_FREEVERB_ENABLE_PREDELAY
    set_predelay(preset.predelay);
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
    set_input_filter(preset.cutoff, preset.resonance);
#endif
}