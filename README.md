# mk_freeverb

A real-time optimized implementation of the Freeverb algorithm adapted for Muse Kinetics audio products.

## Credits

Based on the original Freeverb algorithm by Jezar at Dreampoint (http://www.dreampoint.co.uk), released into the public domain. The original implementation uses the Schroeder/Moorer reverb model with 8 comb filters and 4 allpass filters per channel.

## Adaptations

This version has been modified for real-time audio processing in Muse Kinetics products:

- **Real-time safe**: Static buffer allocation with no dynamic memory operations
- **Configurable processing**: Compile-time options to disable CPU-intensive features (predelay, crossfade, input filtering)
- **Lightweight**: Optimized for minimal CPU overhead in embedded audio systems
- **Preset system**: Predefined reverb configurations for common use cases
- **MIDI integration**: Direct parameter control via MIDI CC messages

## Configuration

Key optimizations can be controlled via preprocessor defines:

- `MK_FREEVERB_DISABLE_PREDELAY`: Removes predelay processing
- `MK_FREEVERB_DISABLE_CROSSFADE`: Disables wet/dry crossfading  
- `MK_FREEVERB_DISABLE_INPUT_FILTER`: Bypasses input filtering
- `MK_FREEVERB_COMB_COUNT`: Adjusts number of comb filters (default: 4)

## Usage

```cpp
#include "mk_freeverb.hpp"

mk_freeverb reverb;
reverb.setroomsize(0.5f);
reverb.setdamp(0.5f);
reverb.setwidth(1.0f);
reverb.processreplace(input_left, input_right, output_left, output_right, frames);
```

## Performance

Computational overhead approximately 1.1% of 80-voice polyphonic synthesis when configured for real-time operation.