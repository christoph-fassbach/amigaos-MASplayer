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

#ifndef MAS_INTERRUPTS_H
#define MAS_INTERRUPTS_H

// TO NEVER BE USED OUTSIDE THE LIBRARY CODE !!!

#include <exec/types.h>

#include "SDI_compiler.h"

ASM( VOID ) SAVEDS dcr_pro_int( VOID );

#endif /* MAS_INTERRUPTS_H */
