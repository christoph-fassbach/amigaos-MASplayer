/*
 * This file is part of the mhimas[pro|std].library.
 *
 * mhimas[pro|std].library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, version 3 of the License only.
 *
 * mhimas[pro|std].library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with mhimas[pro|std].library.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAS3507_H
#define MAS3507_H

// TO NEVER BE USED OUTSIDE THE LIBRARY CODE !!!

#include <exec/types.h>

#include "SDI_compiler.h"

ASM( VOID ) SAVEDS SetupParPort(
  REG( a6, struct MHI_LibBase * base ));

ASM( VOID ) SAVEDS SetVolume(
  REG( d0, ULONG l ),
  REG( d1, ULONG r ),
  REG( a6, struct MHI_LibBase * base ));

ASM( VOID ) SAVEDS SetPrefactor(
  REG( d0, UWORD prefac ),
  REG( a6, struct MHI_LibBase * base ));

ASM( VOID ) SAVEDS SetBass(
  REG( d0, UWORD bass ),
  REG( a6, struct MHI_LibBase * base ));

ASM( VOID ) SAVEDS SetTreble(
  REG( d0, UWORD treb ),
  REG( a6, struct MHI_LibBase * base ));

#endif /* MAS3507_H */
