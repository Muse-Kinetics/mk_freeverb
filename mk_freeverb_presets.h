#ifndef _mk_freeverb_presets_
#define _mk_freeverb_presets_

// Reverb preset structure
struct ReverbPreset {
    // Core parameters (always available)
    float roomsize;
    float damp;
    float wet;
    float dry;
    float width;
    float mode;
    
#if MK_FREEVERB_ENABLE_PREDELAY
    float predelay;  // in seconds
#endif

#if MK_FREEVERB_ENABLE_INPUT_FILTER
    float cutoff;    // Hz
    float resonance; // 0.0 - 1.0
#endif

    // Constructor for easy initialization
    ReverbPreset(float r, float d, float w, float dr, float wi, float m
#if MK_FREEVERB_ENABLE_PREDELAY
                , float pd = 0.0f
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
                , float cf = 8000.0f, float res = 0.5f
#endif
                )
        : roomsize(r), damp(d), wet(w), dry(dr), width(wi), mode(m)
#if MK_FREEVERB_ENABLE_PREDELAY
        , predelay(pd)
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
        , cutoff(cf), resonance(res)
#endif
    {}
};

// Predefined reverb presets
namespace ReverbPresets {
    
    // Default/initialization preset
    const ReverbPreset DEFAULT_PRESET(
        0.5f,   // roomsize
        0.5f,   // damp
        1.0f,   // wet
        0.0f,   // dry
        1.0f,   // width
        0.0f    // mode
#if MK_FREEVERB_ENABLE_PREDELAY
        , 0.0f  // predelay (no predelay by default)
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
        , 8000.0f, 0.5f  // cutoff, resonance
#endif
    );

    // Small room - tight, controlled reverb
    const ReverbPreset SMALL_ROOM(
        0.3f,   // roomsize
        0.7f,   // damp (more damping for smaller space)
        1.0f,  // wet
        0.0f,  // dry
        0.8f,   // width
        0.0f    // mode
#if MK_FREEVERB_ENABLE_PREDELAY
        , 0.005f  // 5ms predelay
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
        , 6000.0f, 0.3f  // slightly darker for intimate sound
#endif
    );

    // Medium hall - balanced reverb
    const ReverbPreset MEDIUM_HALL(
        0.6f,   // roomsize
        0.4f,   // damp
        1.0f,   // wet
        0.0f,   // dry
        1.0f,   // width
        0.0f    // mode
#if MK_FREEVERB_ENABLE_PREDELAY
        , 0.015f  // 15ms predelay
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
        , 8000.0f, 0.4f
#endif
    );

    // Large cathedral - spacious, long reverb
    const ReverbPreset LARGE_CATHEDRAL(
        0.85f,  // roomsize
        0.2f,   // damp (less damping for longer decay)
        1.0f,   // wet
        0.0f,   // dry
        1.0f,   // width
        0.0f    // mode
#if MK_FREEVERB_ENABLE_PREDELAY
        , 0.035f  // 35ms predelay
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
        , 10000.0f, 0.3f  // brighter for spacious feel
#endif
    );

    // Plate reverb - vintage, metallic character
    const ReverbPreset PLATE_REVERB(
        0.7f,   // roomsize
        0.6f,   // damp
        1.0f,   // wet
        0.0f,   // dry
        0.9f,   // width (slightly narrower)
        0.0f    // mode
#if MK_FREEVERB_ENABLE_PREDELAY
        , 0.002f  // very short predelay
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
        , 12000.0f, 0.6f  // brighter, more resonant
#endif
    );

    // Spring reverb - surf/vintage amp sound
    const ReverbPreset SPRING_REVERB(
        0.4f,   // roomsize
        0.8f,   // damp (characteristic dampening)
        1.0f,   // wet
        0.0f,   // dry
        0.7f,   // width (narrower stereo field)
        0.0f    // mode
#if MK_FREEVERB_ENABLE_PREDELAY
        , 0.001f  // minimal predelay
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
        , 5000.0f, 0.8f  // darker, more resonant
#endif
    );

    // Ambient/pad - lush, washy reverb
    const ReverbPreset AMBIENT_WASH(
        0.9f,   // roomsize
        0.1f,   // damp (very little damping)
        1.0f,   // wet (mostly reverb)
        0.0f,   // dry
        1.0f,   // width
        0.0f    // mode
#if MK_FREEVERB_ENABLE_PREDELAY
        , 0.025f  // 25ms predelay
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
        , 9000.0f, 0.2f  // open and airy
#endif
    );

    // Freeze mode - infinite sustain
    const ReverbPreset FREEZE_MODE(
        1.0f,   // roomsize
        0.0f,   // damp
        1.0f,   // wet
        0.0f,   // dry
        1.0f,   // width
        1.0f    // mode (freeze!)
#if MK_FREEVERB_ENABLE_PREDELAY
        , 0.0f  // no predelay for freeze
#endif
#if MK_FREEVERB_ENABLE_INPUT_FILTER
        , 8000.0f, 0.3f
#endif
    );

    // Array of all presets for easy iteration (defined in mk_freeverb_presets.cpp)
    extern const ReverbPreset* ALL_PRESETS[];

    // Preset names for UI/debugging (defined in mk_freeverb_presets.cpp)
    extern const char* PRESET_NAMES[];

    extern const int NUM_PRESETS;
}

#endif // _mk_freeverb_presets_