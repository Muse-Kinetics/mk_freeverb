#include "mk_freeverb_presets.h"

namespace ReverbPresets {
    
    // Array of all presets for easy iteration
    const ReverbPreset* ALL_PRESETS[] = {
        &DEFAULT_PRESET,
        &SMALL_ROOM,
        &MEDIUM_HALL,
        &LARGE_CATHEDRAL,
        &PLATE_REVERB,
        &SPRING_REVERB,
        &AMBIENT_WASH,
        &FREEZE_MODE
    };

    // Preset names for UI/debugging
    const char* PRESET_NAMES[] = {
        "Default",
        "Small Room",
        "Medium Hall",
        "Large Cathedral", 
        "Plate Reverb",
        "Spring Reverb",
        "Ambient Wash",
        "Freeze Mode"
    };

    const int NUM_PRESETS = sizeof(ALL_PRESETS) / sizeof(ALL_PRESETS[0]);
}