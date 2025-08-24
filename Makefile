#
# This file is part of the mhimas[pro|std].library.
#
# mhimas[pro|std].library is free software: you can redistribute
# it and/or modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation, version 3 of the License only.
#
# mhimas[pro|std].library is distributed in the hope that it
# will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with mhimas[pro|std].library.
#
# If not, see <http://www.gnu.org/licenses/>.
#

ifeq ($(findstring /home,$(HOME)), /home)
  LINUX         = 1

  NDK           = $(VBCC)/NDK3.2/
  WILDCARD      = *

  MKDIR         = mkdir -p
  MKDIR_SUFFIX  =
  RM            = rm -rf
  RM_SUFFIX     =
  CP            = cp
  RUNNER        = vamos -c .vamosrc
  IGNORE_VAMOS  = -
  LHA           = jlha

  LIB_HOST      = cross

  # Use .netrc to store passwords! 
  # ~/.netrc format:
  # machine <ip> login username password pass
  # Could do --user username:pass instead - but not cool in repo ;)
  INSTALL1      = curl --netrc --upload-file $(LIB_FILE_DEST) ftp://192.168.0.16/sda4/Libs/MHI/$(LIB_NAME).library ; curl --netrc --upload-file $(LIB_FILE_DEST) ftp://192.168.0.14/cf3/Libs/MHI/$(LIB_NAME).library
  INSTALL0      = cp $(LIB_FILE_DEST) ~/Documents/FS-UAE/Shared/MHI/$(LIB_NAME).library
else
  AMIGAOS       = 1
  
  NDK           = NDK_3.2:
  WILDCARD      = \#?

  MKDIR         = makedir
  MKDIR_SUFFIX  = ALL >NIL:
  RM            = delete
  RM_SUFFIX     = ALL >NIL:
  CP            = copy
  RUNNER        =
  IGNORE_VAMOS  =
  LHA           = lha

  LIB_HOST      = native

  INSTALL0      = copy FROM $(LIB_FILE_DEST)	TO	Libs:MHI/$(LIB_NAME).library		CLONE
  INSTALL1      = avail flush >NIL:
endif

DDIR   = target/
BDIR   = build/
SDIR   = src/
HDIR   = header/
ADFDIR = $(DDIR)MASplayer-MHI
ADFIMG = MASplayer-MHI-$(LIB_VERSION).$(LIB_REVISION)-`date +%Y%m%d`.adf
LHAARC = MASplayer-MHI-$(LIB_VERSION).$(LIB_REVISION)-`date +%Y%m%d`.lha

MAS_VERSION    ?= pro
LIB_CPU        ?= 000
LIB_LOG        ?= NO_LOG
# NO_LOG
# SER_LOG
# FILE_LOG
# MEM_LOG
LIB_NAME        = mhimas$(MAS_VERSION)
LIB_FILE        = $(LIB_NAME).library
LIB_VERSION     = 1
LIB_REVISION    = 7
LIB_PREFIX      = _LIB_

LIB_LIBS        =
LIB_FILE_DEST   = $(DDIR)$(LIB_FILE)-$(LIB_CPU)-$(LIB_LOG)

FD_FILE         = $(HDIR)$(LIB_NAME)_lib.fd
PRAGMA_FILE     = $(HDIR)$(LIB_NAME)_pragmas.h

###########################

MHI_INCLUDE     = Include
NDK_ASM_INCLUDE = $(NDK)Include_I
NDK_C_INCLUDE   = $(NDK)Include_H

C_SRCS          = $(wildcard $(SDIR)$(WILDCARD).c)
BDIR_C_SRCS     = $(foreach C_SRC,$(C_SRCS),$(subst $(SDIR),$(BDIR),$(C_SRC)))
BDIR_C_OBJS     = $(BDIR_C_SRCS:%.c=%-$(LIB_CPU)-$(LIB_LOG).o)

A_SRCS          = $(wildcard $(SDIR)$(WILDCARD).asm)
BDIR_A_SRCS     = $(foreach A_SRC,$(A_SRCS),$(subst $(SDIR),$(BDIR),$(A_SRC)))
BDIR_A_OBJS     = $(BDIR_A_SRCS:%.asm=%.o)

LIB_OBJS        = $(BDIR)library-$(LIB_CPU)-$(LIB_LOG).o \
                  $(filter-out %/library-$(LIB_CPU)-$(LIB_LOG).o,$(BDIR_C_OBJS)) \
                  $(filter-out %_interrupt.o,$(BDIR_A_OBJS)) \
                  $(BDIR)mas_$(MAS_VERSION)_interrupt.o

###########################

# activate VBCC builds by adding `USEVBCC=1` to the make call!
ifeq (34,$(USEVBCC))
  ifdef AMIGAOS
    LDIR        = vlibos3:
  endif
  ifdef LINUX
    LDIR        = $(VBCC)/targets/m68k-amigaos/lib/
  endif

  AFLAGS        = -I$(NDK_ASM_INCLUDE) -quiet -phxass -Fhunk           \
                  -m68$(LIB_CPU)                                       \
                  -o
  CFLAGS        = -c -Os -+ -c99 -cpu=68$(LIB_CPU)                     \
                  -I$(MHI_INCLUDE) -I$(HDIR)                           \
                  -D__NOLIBBASE__ -D$(LIB_LOG)                         \
                  -DLIB_FILE=$(LIB_FILE) -DLIB_DATE=__AMIGADATE__      \
                  -DLIB_VERSION=$(LIB_VERSION)                         \
                  -DLIB_REVISION=$(LIB_REVISION) -DLIB_CPU=$(LIB_CPU)  \
                  -DLIB_COMPILER=VBCC -DLIB_HOST=$(LIB_HOST)           \
                  -o
  LFLAGS        = -bamigahunk -x -s -mrel -Cvbcc -Bstatic -nostdlib -o
  LFLAGS1       =
  LFLAGST1      = $(LDIR)startup.o
  LFLAGST2      = $(LDIR)auto.lib $(LDIR)vc.lib
  # add -v below to see what vbcc does under the hood :)
  CC            = vc +kick13
  AS            = vasmm68k_mot
  LD            = vlink 
else ifdef USEVBCC
  ifdef AMIGAOS
    LDIR        = vlibos3:
  endif
  ifdef LINUX
    LDIR        = $(VBCC)/targets/m68k-amigaos/lib/
  endif

  AFLAGS        = -I$(NDK_ASM_INCLUDE) -quiet -phxass -Fhunk           \
                  -m68$(LIB_CPU)                                       \
                  -o
  CFLAGS        = -O2 -c -cpu=68$(LIB_CPU)                             \
                  -I$(MHI_INCLUDE) -I$(HDIR)                           \
                  -D__NOLIBBASE__ -D$(LIB_LOG)                         \
                  -DLIB_FILE=$(LIB_FILE) -DLIB_DATE=__AMIGADATE__      \
                  -DLIB_VERSION=$(LIB_VERSION)                         \
                  -DLIB_REVISION=$(LIB_REVISION) -DLIB_CPU=$(LIB_CPU)  \
                  -DLIB_COMPILER=VBCC -DLIB_HOST=$(LIB_HOST)           \
                  -o
  LFLAGS        = -bamigahunk -x -s -mrel -Cvbcc -Bstatic -nostdlib -o
  LFLAGS1       =
  LFLAGST1      = $(LDIR)startup.o
  LFLAGST2      = $(LDIR)auto.lib $(LDIR)vc.lib
  # add -v below to see what vbcc does under the hood :)
  CC            = vc +aos68k
  AS            = vasmm68k_mot
  LD            = vc +aos68k
else
  AFLAGS        = RESETOPTIONS DEFINE=__NOLIBBASE__ DEFINE=$(LIB_HOST)         \
                  IDIR=$(MHI_INCLUDE) IDIR=NDK_3.2:Include_I IDIR=$(HDIR)      \
                  OBJNAME

  CFLAGS        = RESETOPTIONS                                                 \
                  PARAMETERS=REGISTER DATA=FAR CODE=FAR                        \
                  NOSTACKCHECK NOCHECKABORT NOICONS                            \
                  STRINGMERGE UNSIGNEDCHAR CPU=68$(LIB_CPU)                    \
                  OPTIMIZE OPTIMIZERGLOBAL OPTIMIZERINLINE OPTIMIZERINLOCAL    \
                  OPTIMIZERLOOP OPTIMIZERPEEPHOLE OPTIMIZERSCHEDULE            \
                  IDIR=$(MHI_INCLUDE) IDIR=$(NDK_C_INCLUDE) IDIR=$(HDIR)       \
                  DEF __NOLIBBASE__ DEF $(LIB_LOG)                             \
                  DEF LIB_FILE=$(LIB_FILE) DEF LIB_DATE=__AMIGADATE__          \
                  DEF LIB_VERSION=$(LIB_VERSION)                               \
                  DEF LIB_REVISION=$(LIB_REVISION) DEF LIB_CPU=$(LIB_CPU)      \
                  DEF LIB_COMPILER=SAS/C DEF LIB_HOST=$(LIB_HOST)              \
                  OBJNAME

  LFLAGS        = NOICONS SMALLDATA SMALLCODE STRIPDEBUG \
                  TO 
  LFLAGS1       = FROM
  LFLAGST1      = LIB:c.o
  LFLAGST2      = LIBRARY LIB:sc.lib LIB:amiga.lib
  CC            = $(RUNNER) sc
  AS            = $(RUNNER) sc
  LD            = $(RUNNER) slink
  FD            = $(RUNNER) sc:c/fd2pragma
endif

##################################

.PHONY: all install test support INCLUDES FOLDERS clean release

all: FOLDERS $(LIB_FILE_DEST)

ECHO:
	@echo "C-sources: "$(C_SRCS)
	@echo "ASM-sources: "$(A_SRCS)
	@echo "USEVBCC: "$(USEVBCC)
	@echo "LIB_OBJS: "$(LIB_OBJS)

install: all
	$(INSTALL0)
	$(INSTALL1)

clean-intermediate:
	-$(RM) $(BDIR) $(RM_SUFFIX)

clean: clean-intermediate
	-$(RM) $(DDIR) $(ADFDIR) $(RM_SUFFIX)
	-$(RM) support/$(WILDCARD).o
	-$(RM) support/$(WILDCARD)/$(WILDCARD).o

FOLDERS:
	-@$(MKDIR) $(DDIR) $(MKDIR_SUFFIX)
	-@$(MKDIR) $(BDIR) $(MKDIR_SUFFIX)
	-@$(MKDIR) $(ADFDIR) $(MKDIR_SUFFIX)

$(LIB_FILE_DEST): $(LIB_OBJS)
	$(LD) $(LFLAGS) $@ $(LFLAGS1) $^

###############################################################################

$(BDIR)%-$(LIB_CPU)-$(LIB_LOG).o: $(SDIR)%.c $(HDIR)*
	$(CC) $(CFLAGS) $@ $<

$(BDIR)%.o: $(SDIR)%.asm $(HDIR)*
	$(AS) $(AFLAGS) $@ $<

###############################################################################

support: FOLDERS MHIplay Test

MHIplay:
	$(CC) $(CFLAGS) support/$@/$@.o support/$@/$@.c
	$(LD) $(LFLAGS) target/$@ $(LFLAGS1) $(LFLAGST1) support/$@/$@.o $(LFLAGST2)
	-cp target/$@ ~/Documents/FS-UAE/Shared/MHI/
	-curl --connect-timeout 3 --netrc --upload-file target/$@       ftp://192.168.0.4/cf0/Expansion/MASplayer/

Test:
	$(CC) $(CFLAGS) support/$@/$@.o support/$@/$@.c
	$(LD) $(LFLAGS) target/$@ $(LFLAGS1) $(LFLAGST1) support/$@/$@.o $(LFLAGST2)
	-cp target/$@ ~/Documents/FS-UAE/Shared/MHI/

###############################################################################

RELEASE_API_URL = https://api.github.com/repos/christoph-fassbach/amigaos-MASplayer/releases
UPLOAD_API_URL = https://uploads.github.com/repos/christoph-fassbach/amigaos-MASplayer/releases

release: all-variants package

release-publish: release
	@if [ -z "$(RELEASE_TAG)" ] ; then echo "Please call \"make RELEASE_TAG='release-1.7-RC1' release-publish\"" ; exit 1 ; fi
	git tag "$(RELEASE_TAG)"
	git push --tags
	curl -L --netrc -X POST                       \
		-H "Accept: application/vnd.github+json"    \
		-H "X-GitHub-Api-Version: 2022-11-28"       \
		$(RELEASE_API_URL)                          \
		-d '{ "tag_name":"$(RELEASE_TAG)", "target_commitish":"main", "name":"$(RELEASE_TAG)", "body":"Description of the release", "draft":false, "prerelease":true, "generate_release_notes":false }'
	curl -L --netrc                               \
		-H "Accept: application/vnd.github+json"    \
		-H "X-GitHub-Api-Version: 2022-11-28"       \
		$(RELEASE_API_URL)/tags/$(RELEASE_TAG) | jq -r ".id" > $(BDIR)/TEMP_RELEASE_ID
	echo "$(RELEASE_TAG) found as $$(cat $(BDIR)/TEMP_RELEASE_ID)"
	curl -L --netrc                               \
		-X POST                                     \
		-H "Accept: application/vnd.github+json"    \
		-H "X-GitHub-Api-Version: 2022-11-28"       \
		-H "Content-Type: application/octet-stream" \
		"$(UPLOAD_API_URL)/$$(cat $(BDIR)/TEMP_RELEASE_ID)/assets?name=$(LHAARC)" \
		--data-binary "@$(DDIR)/$(LHAARC)"
	echo $(ADFIMG) > $(BDIR)/TEMP_ADF_FILE_NAME
	curl -L --netrc                               \
		-X POST                                     \
		-H "Accept: application/vnd.github+json"    \
		-H "X-GitHub-Api-Version: 2022-11-28"       \
		-H "Content-Type: application/octet-stream" \
		"$(UPLOAD_API_URL)/$$(cat $(BDIR)/TEMP_RELEASE_ID)/assets?name=$$(cat $(BDIR)/TEMP_ADF_FILE_NAME)" \
		--data-binary "@$(DDIR)/$$(cat $(BDIR)/TEMP_ADF_FILE_NAME)"

release-unpublish: FOLDERS
	@if [ -z "$(RELEASE_TAG)" ] ; then echo "Please call \"make RELEASE_TAG='release-1.7-RC1' release-unpublish\"" ; exit 1 ; fi
	curl -L --netrc                               \
		-H "Accept: application/vnd.github+json"    \
		-H "X-GitHub-Api-Version: 2022-11-28"       \
		$(RELEASE_API_URL)/tags/$(RELEASE_TAG) | jq -r ".id" > $(BDIR)/TEMP_RELEASE_ID
	echo "$(RELEASE_TAG) found as $$(cat $(BDIR)/TEMP_RELEASE_ID)"
	curl -L --netrc                               \
		-X DELETE                                   \
		-H "Accept: application/vnd.github+json"    \
		-H "X-GitHub-Api-Version: 2022-11-28"       \
		$(RELEASE_API_URL)/$$(cat $(BDIR)/TEMP_RELEASE_ID)
	git tag -d $(RELEASE_TAG)
	git push origin :$(RELEASE_TAG)

###############################################################################

package:
	-umount $(ADFDIR)
	rm -rf $(DDIR)/$(ADFIMG)

	adf_floppy_create $(DDIR)/$(ADFIMG) dd
	adf_format -l MASplayerMHI -t 0 -f $(DDIR)/$(ADFIMG)
	fuseadf $(DDIR)/$(ADFIMG) $(ADFDIR)

	mkdir -p $(ADFDIR)/MHI \
	         $(ADFDIR)/Tools       \
	         $(ADFDIR)/Manual

	cp Installer/MHI.info                       $(ADFDIR)/
	cp Installer/Folder.infoV36                 $(ADFDIR)/
	cp $(DDIR)/*\.library*                      $(ADFDIR)/MHI/

	cp Installer/Tools.info                     $(ADFDIR)/
	cp $(DDIR)/MHIplay                          $(ADFDIR)/Tools/
	cp Installer/program.info                   $(ADFDIR)/Tools/MHIplay.info
	cp $(DDIR)/Test                             $(ADFDIR)/Tools/
	cp Installer/program.info                   $(ADFDIR)/Tools/Test.info

	cp Installer/Manual.info                    $(ADFDIR)/
	cp Installer/MASplayer.guide                $(ADFDIR)/Manual/
	cp Installer/MASplayer.guide.info           $(ADFDIR)/Manual/
	cp Docs/Manual/MASplayer.guide              $(ADFDIR)/Manual/MASplayerv39.guide
	sed -e "s#MultiView#Sys:Utilities/WDisplay#"    \
	    -e "s#ilbm/#MASplayer-Manual:ilbm/#"        \
	    Docs/Manual/MASplayer.guide           > $(ADFDIR)/Manual/MASplayerv38.guide
	cp -R Docs/Manual/ilbm                      $(ADFDIR)/Manual
	cp Installer/MASplayer.guide.infoV38        $(ADFDIR)/Manual/

	cp Installer/Disk.info                      $(ADFDIR)/
	cp Installer/GPL.txt.info                   $(ADFDIR)/GPL.txt.info
	cp COPYING                                  $(ADFDIR)/GPL.txt
	cp Installer/LGPL.txt.info                  $(ADFDIR)/LGPL.txt.info
	cp COPYING.LESSER                           $(ADFDIR)/LGPL.txt
	cp mhimasplayer.readme                      $(ADFDIR)/ReadMeFirst.txt
	cp Installer/ReadMeFirst.txt.info           $(ADFDIR)/ReadMeFirst.txt.info

	cp "Installer/MASplayer MHI installer"      $(ADFDIR)/
	cp "Installer/2.0+English.info"             $(ADFDIR)/
	cp "Installer/2.0+Deutsch.info"             $(ADFDIR)/

	cp "Installer/1.3-English"                  $(ADFDIR)/
	cp "Installer/1.3-English.info"             $(ADFDIR)/
	cp "Installer/1.3-Deutsch"                  $(ADFDIR)/
	cp "Installer/1.3-Deutsch.info"             $(ADFDIR)/

	cp Installer/drawer.info                    $(ADFDIR).info
	cd `realpath $(ADFDIR)/..` ; jlha a $(LHAARC) `basename $(ADFDIR)` `basename $(ADFDIR)`.info

	umount $(ADFDIR)
	-cp $(DDIR)/$(ADFIMG)                       ~/Documents/FS-UAE/Shared/MHI/
	-cp $(DDIR)/$(LHAARC)                  ~/Documents/FS-UAE/Shared/MHI/

all-variants:
	make MAS_VERSION=pro LIB_CPU=000 LIB_LOG=NO_LOG
	make MAS_VERSION=pro LIB_CPU=020 LIB_LOG=NO_LOG
	make MAS_VERSION=pro LIB_CPU=040 LIB_LOG=NO_LOG
	make MAS_VERSION=pro LIB_CPU=000 LIB_LOG=SER_LOG
	make MAS_VERSION=pro LIB_CPU=020 LIB_LOG=SER_LOG
	make MAS_VERSION=pro LIB_CPU=040 LIB_LOG=SER_LOG
	make MAS_VERSION=std LIB_CPU=000 LIB_LOG=NO_LOG
	make MAS_VERSION=std LIB_CPU=020 LIB_LOG=NO_LOG
	make MAS_VERSION=std LIB_CPU=040 LIB_LOG=NO_LOG
	make MAS_VERSION=std LIB_CPU=000 LIB_LOG=SER_LOG
	make MAS_VERSION=std LIB_CPU=020 LIB_LOG=SER_LOG
	make MAS_VERSION=std LIB_CPU=040 LIB_LOG=SER_LOG
	make support

