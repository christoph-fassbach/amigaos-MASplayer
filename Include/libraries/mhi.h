/*
 * Licenced under modified BSD licence.
 *
 * Copyright © Paul Qureshi, Thomas Whenzel and Dirk Conrad. All rights
 * reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the authors nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *
 * Or, in English:
 *   - You're free to derive any work you like from this, just don't change
 *     the original source.
 *   - Give credit where credit is due
 *   - Don't fob it off as your own work
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef LIBRARIES_MHI_H
#define LIBRARIES_MHI_H

#include <exec/libraries.h>

struct MHIBase
{
  struct Library base;
};

/* MHI status flags for player */

#define MHIF_PLAYING            0
#define MHIF_STOPPED            1
#define MHIF_OUT_OF_DATA        2
#define MHIF_PAUSED             3

/* MHI queries and returned values */

#define MHIF_UNSUPPORTED        0
#define MHIF_SUPPORTED          1
#define MHIF_FALSE              0
#define MHIF_TRUE               1

#define MHIQ_DECODER_NAME    1000
#define MHIQ_DECODER_VERSION 1001
#define MHIQ_AUTHOR          1002

#define MHIQ_IS_HARDWARE     1010
#define MHIQ_IS_68K          1011
#define MHIQ_IS_PPC          1012

#define MHIQ_CAPABILITIES       0
#define MHIQ_MPEG1              1
#define MHIQ_MPEG2              2
#define MHIQ_MPEG25             3
#define MHIQ_MPEG4              4   /* there is no MPEG3! */

#define MHIQ_LAYER1            10
#define MHIQ_LAYER2            11
#define MHIQ_LAYER3            12

#define MHIQ_VARIABLE_BITRATE  20
#define MHIQ_JOINT_STEREO      21

#define MHIQ_BASS_CONTROL      30
#define MHIQ_TREBLE_CONTROL    31
#define MHIQ_MID_CONTROL       32
#define MHIQ_PREFACTOR_CONTROL 33
#define MHIQ_5_BAND_EQ         34
#define MHIQ_10_BAND_EQ        35

#define MHIQ_VOLUME_CONTROL    40
#define MHIQ_PANNING_CONTROL   41
#define MHIQ_CROSSMIXING       42

/* ********************** */
/* MHI decoder parameters */
/* ********************** */

#define MHIP_VOLUME             0        // 0=muted .. 100=0dB
#define MHIP_PANNING            1        // 0=left .. 50=center .. 100=right
#define MHIP_CROSSMIXING        2        // 0=stereo .. 100=mono
// For 3-band equalizer
#define MHIP_BASS               3        // 0=max.cut .. 50=unity gain .. 100=max.boost
#define MHIP_MID                4        // 0=max.cut .. 50=unity gain .. 100=max.boost
#define MHIP_TREBLE             5        // 0=max.cut .. 50=unity gain .. 100=max.boost
#define MHIP_PREFACTOR          6        // 0=max.cut .. 50=unity gain .. 100=max.boost
// Extension for 5-band equalizer
#define MHIP_MIDBASS            7        // 0=max.cut .. 50=unity gain .. 100=max.boost
#define MHIP_MIDHIGH            8        // 0=max.cut .. 50=unity gain .. 100=max.boost

// Extension for 10-band equalizer
#define MHIP_BAND1              9        // 32 Hz
#define MHIP_BAND2          MHIP_BASS    // 64 Hz
#define MHIP_BAND3             10        // 125 Hz
#define MHIP_BAND4         MHIP_MIDBASS  // 250 Hz
#define MHIP_BAND5             11        // 500 Hz
#define MHIP_BAND6          MHIP_MID     // 1 kHz
#define MHIP_BAND7             12        // 2 kHz
#define MHIP_BAND8         MHIP_MIDHIGH  // 4 kHz
#define MHIP_BAND9             13        // 8 kHz
#define MHIP_BAND10        MHIP_TREBLE   // 16 kHz

#endif
