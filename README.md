# Origin:

https://aminet.net/driver/audio/mhi_dev.lha

# Legal:

Originally, from Docs/MHI_Developer.guide .

See LICENSE.md.

# History:

V1.0:
  - internal test release
  - basics working

V1.2 Final:
  - volume and tone control working
  - optimised code a little
  - 4 byte memory leak fixed

V1.4:
  - added support for latest MAS Players (thanks Dirk!)
  - fixed MHIQuery() returns for tone control
  - updated to meet MHI 1.1 standard
  - increased interrupt frequency, more load in slow CPUs
    but now supports higher bit rates
  - added panning control

V1.5:
  - updated to meet MHI 1.2 standard
  - fixed driver so that it doesn't trash register A6 any more.
  - recompiled with amiga-gcc.

V1.6:
  - fixed driver so that it doesn't mess with the serial port.

V1.7:
  - Backport to OS 1.3 / v34
  - Port to SAS/C + VBCC + VASM for 68000 compatibility
  - License change - code is now partially under LGPL 3
