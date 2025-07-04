#ifndef SDI_COMPILER_H
#define SDI_COMPILER_H

/* Includeheader

        Name:           SDI_compiler.h
        Versionstring:  $VER: SDI_compiler.h 1.10 (18.10.2002)
        Author:         SDI
        Distribution:   PD
        Description:    defines to hide compiler stuff

 1.1   25.06.98 : created from data made by Gunter Nikl
 1.2   17.11.99 : added VBCC
 1.3   29.02.00 : fixed VBCC REG define
 1.4   30.03.00 : fixed SAVEDS for VBCC
 1.5   29.07.00 : added #undef statements (needed e.g. for AmiTCP together
        with vbcc)
 1.6   19.05.01 : added STACKEXT and Dice stuff
 1.7   16.06.02 : added MorphOS specials and VARARGS68K
 1.8   21.09.02 : added MorphOS register stuff
 1.9   26.09.02 : added OFFSET macro. Thanks Frank Wille for suggestion
 1.10  18.10.02 : reverted to old MorphOS-method for GCC
*/

/*
** This is PD (Public Domain). This means you can do with it whatever you want
** without any restrictions. I only ask you to tell me improvements, so I may
** fix the main line of this files as well.
**
** To keep confusion level low: When changing this file, please note it in
** above history list and indicate that the change was not made by myself
** (e.g. add your name or nick name).
**
** Dirk Stoecker <stoecker@epost.de>
*/

#ifdef ASM
#undef ASM
#endif
#ifdef REG
#undef REG
#endif
#ifdef LREG
#undef LREG
#endif
#ifdef CONST
#undef CONST
#endif
#ifdef SAVEDS
#undef SAVEDS
#endif
#ifdef INLINE
#undef INLINE
#endif
#ifdef REGARGS
#undef REGARGS
#endif
#ifdef STDARGS
#undef STDARGS
#endif
#ifdef OFFSET
#undef OFFSET
#endif

/* first "exceptions" */

#if defined(__MAXON__)
  #define STDARGS
  #define STACKEXT
  #define REGARGS
  #define SAVEDS
  #define INLINE inline
#elif defined(__VBCC__)
  #define STDARGS
  #define STACKEXT
  #define REGARGS
  #define INLINE
  #define INTERRUPT __amigainterrupt
  #define OFFSET(p,m) __offsetof(struct p,m)
  #if defined(__MORPHOS__)
    #define REG(reg,arg) __reg(MOS__##reg) arg

    /* NOTE: This assumes "quick native mode" when compiling libraries. */
    #define MOS__a0 "r24"
    #define MOS__a1 "r25"
    #define MOS__a2 "r26"
    #define MOS__a3 "r27"
    #define MOS__a4 "r28"
    #define MOS__a5 "r29"
    #define MOS__a6 "r30"
    /* #define MOS__a7 "r31" */
    #define MOS__d0 "r16"
    #define MOS__d1 "r17"
    #define MOS__d2 "r18"
    #define MOS__d3 "r19"
    #define MOS__d4 "r20"
    #define MOS__d5 "r21"
    #define MOS__d6 "r22"
    #define MOS__d7 "r23"

  #else
    #define REG(reg,arg) __reg(#reg) arg
  #endif
#elif defined(__STORM__)
  #define STDARGS
  #define STACKEXT
  #define REGARGS
  #define INLINE inline
#elif defined(__SASC)
  #define ASM(arg) arg __asm
  #define INTERRUPT __interrupt
#elif defined(__GNUC__)
  #define REG(reg,arg) arg __asm(#reg)
  #define LREG(reg,arg) register REG(reg,arg)

  /* Don`t use __stackext for the MorphOS version
     because we anyway don`t have a libnix ppc with stackext
     Also we define a VARARGS68K define here to specify
     functions that should work with that special attribute
     of the MOS gcc compiler for varargs68k handling. */
  #if defined(__MORPHOS__)
    #define STDARGS
    #define STACKEXT
    #define VARARGS68K  __attribute__((varargs68k))
  #endif
#elif defined(_DCC)
  #define REG(reg,arg) __##reg arg
  #define STACKEXT __stkcheck
  #define STDARGS __stkargs
  #define INLINE static
#endif

/* then "common" ones */

#if !defined(ASM)
  #define ASM(arg) arg
#endif
#if !defined(REG)
  #define REG(reg,arg) register __##reg arg
#endif
#if !defined(LREG)
  #define LREG(reg,arg) register arg
#endif
#if !defined(CONST)
  #define CONST const
#endif
#if !defined(SAVEDS)
  #define SAVEDS __saveds
#endif
#if !defined(INLINE)
  #define INLINE static __inline
#endif
#if !defined(REGARGS)
  #define REGARGS __regargs
#endif
#if !defined(STDARGS)
  #define STDARGS __stdargs
#endif
#if !defined(STACKEXT)
  #define STACKEXT __stackext
#endif
#if !defined(VARARGS68K)
  #define VARARGS68K
#endif
#if !defined(OFFSET)
  #define OFFSET(structName, structEntry) \
    ((char *)(&(((struct structName *)0)->structEntry))-(char *)0)
#endif

/* Christoph's extras: */
/**
 * Usage: x = GET_REG(REG_D1)
 */
#if defined(__VBCC__)

__reg("d1") ULONG __GET_REG_D1()="\t";
#define GET_REG(reg) __GET_ ## reg()

#elif defined(__SASC)

#define GET_REG getreg

#define getreg __builtin_getreg
extern long getreg(int);

#define REG_D1 1

#endif

#if defined(__VBCC__)

#undef INLINE
#define INLINE static 

#elif defined(__SASC)
#endif

#endif /* SDI_COMPILER_H */
