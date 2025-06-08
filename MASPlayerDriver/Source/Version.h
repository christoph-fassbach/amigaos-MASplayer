
#ifdef MAS_PRO
#define LIBNAME  "mhimaspro"
#endif
#ifdef MAS_STD
#define LIBNAME  "mhimasstd"
#endif
#define VERSION  1
#define REVISION 5
#define DATE     26.08.2022

#define NUM2STR(a) NUM2STR_SUB(a)
#define NUM2STR_SUB(a) #a

#define IDSTRING NUM2STR(VERSION) "." NUM2STR(REVISION) " (" NUM2STR(DATE) ")"

