#ifndef _mk_freeverb_config_
#define _mk_freeverb_config_

// Real-time safety configuration for mk_freeverb
// Include this file before mk_freeverb.hpp to configure the build

// Enable/disable predelay feature
// Predelay adds latency and uses significant memory
// For real-time applications, consider disabling (0)
#ifndef MK_FREEVERB_ENABLE_PREDELAY
#define MK_FREEVERB_ENABLE_PREDELAY 0
#endif

// Enable/disable predelay crossfading
// Crossfading provides smooth predelay time changes but uses more CPU
// and doubles the predelay memory usage (uses 2 static buffers)
// Only relevant if MK_FREEVERB_ENABLE_PREDELAY is enabled
// Now RT-safe with static buffer implementation
#ifndef MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE
#define MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE 0
#endif

// Enable/disable input filtering
// Input filtering adds CPU load per sample
// For real-time applications, consider disabling (0) 
#ifndef MK_FREEVERB_ENABLE_INPUT_FILTER
#define MK_FREEVERB_ENABLE_INPUT_FILTER 1
#endif

// Number of comb filters to use (1-8)
// More combs = higher quality but more CPU
// For real-time: 2-4 combs recommended
// Default original algorithm: 8 combs
#ifndef MK_FREEVERB_NUM_COMBS
#define MK_FREEVERB_NUM_COMBS 4
#endif

// Maximum predelay buffer size in samples
// At 48kHz: 4800 samples = 100ms max predelay
// At 44.1kHz: 4410 samples = 100ms max predelay
// Reduce this to save memory if shorter predelays are acceptable
#ifndef MK_FREEVERB_MAX_PREDELAY_SAMPLES
#define MK_FREEVERB_MAX_PREDELAY_SAMPLES 4800
#endif

// Preset configurations for common use cases:

// Ultra-light configuration for embedded/RT applications
// #define MK_FREEVERB_ENABLE_PREDELAY 0
// #define MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE 0
// #define MK_FREEVERB_ENABLE_INPUT_FILTER 0
// #define MK_FREEVERB_NUM_COMBS 2

// Balanced configuration for desktop RT applications  
// #define MK_FREEVERB_ENABLE_PREDELAY 0
// #define MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE 0
// #define MK_FREEVERB_ENABLE_INPUT_FILTER 0
// #define MK_FREEVERB_NUM_COMBS 4

// High-quality configuration for offline processing
// #define MK_FREEVERB_ENABLE_PREDELAY 1
// #define MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE 1
// #define MK_FREEVERB_ENABLE_INPUT_FILTER 1
// #define MK_FREEVERB_NUM_COMBS 8

// Real-time configuration with predelay but no crossfade
// #define MK_FREEVERB_ENABLE_PREDELAY 1
// #define MK_FREEVERB_ENABLE_PREDELAY_CROSSFADE 0
// #define MK_FREEVERB_ENABLE_INPUT_FILTER 0
// #define MK_FREEVERB_NUM_COMBS 4

#endif // _mk_freeverb_config_