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

#include <proto/dos.h>
#include <proto/exec.h>

#include "mas_mhi.h"
#include "library.h"

#ifdef BASE_GLOBAL

// As declared in mas_mhi.h
struct MHI_LibBase       * MAS_MHI_Base      = 0;
struct ExecBase          * SysBase           = 0;
struct DosLibrary        * DOSBase           = 0;

#endif

/******************************************************************************
 * Library skeleton required library hooks - public function definitions.
 *****************************************************************************/

LONG CustomLibInit( LIBRARY_TYPE * base, struct ExecBase * sysBase ) {

  base->mhi_SysBase = sysBase;

#ifdef BASE_GLOBAL
  SysBase = sysBase;
#endif

  base->mhi_DOSBase =
    ( struct DosLibrary * ) OpenLibrary( "dos.library", 34 );
  if ( !( base->mhi_DOSBase )) {

    return 17; //EOpenDosBase;
  }

#ifdef BASE_GLOBAL
  DOSBase         = base->mhi_DOSBase;
  MAS_MHI_Base    = base;
#endif

  return 0; //ENoError;
}

VOID CustomLibClose( LIBRARY_TYPE * base ) {

#ifndef BASE_GLOBAL
  struct ExecBase *SysBase = base->agb_SysBase;
#endif

  if ( base->mhi_DOSBase ) {

    CloseLibrary(( struct Library *) base->mhi_DOSBase );
  }
}
