# NootedBlue
A [Lilu](https://github.com/acidanthera/Lilu) plugin that provides support for unsupported Intel iGPUs

The Source Code of this Original Work is licensed under the `Thou Shalt Not Profit License version 1.0`. See [`LICENSE`](https://github.com/richardhbtz/NootedBlue/blob/master/LICENSE)

## Current Progress:
- Haswell: Work is under way on adding Mojave patches from HSWGT1Fixup
- Gen 8: N/A
- Gen 9: N/A
- Gen 9.5: N/A
- Gen 11: N/A

Note: you still have to set `AAPL,ig-platform-id` and you also have to add `-disablegfxfirmware` if you are on Skylake+
Refer to [WhateverGreen's Framebuffer list](https://github.com/acidanthera/WhateverGreen/blob/master/Manual/FAQ.IntelHD.en.md) on what framebuffer to set for your system.

## Compatibility:
- Haswell (10.12+)
- Broadwell (10.14+, work in progress)
- Braswell (10.14+, work in progress)
### These Generations may be worked on in the future:
- Skylake
- Apollo Lake
- Kaby Lake
- Coffee Lake
- Gemini Lake
- Ice Lake
- Jasper/Elkhart Lake
