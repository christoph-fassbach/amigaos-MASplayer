#ifndef MHI_MASPRO_H
#define MHI_MASPRO_H
extern APTR    i_MHIAllocDecoder (struct Task *task asm("a0"), ULONG mhisignal asm("d0"));
extern VOID    StartTimer        (struct freetimer *ft);
extern BOOL    FindFreeTimer     (struct freetimer *ft, int preferA);
extern BOOL    TryTimer          (struct freetimer *ft);
extern VOID    i_MHIFreeDecoder  (APTR handle asm("a3"));
extern BOOL    i_MHIQueueBuffer  (struct MPHandle *handle asm("a3"), APTR buffer asm("a0"), ULONG size asm("d0"));
extern APTR    i_MHIGetEmpty     (struct MPHandle *handle asm("a3"));
extern UBYTE   i_MHIGetStatus    (struct MPHandle *handle asm("a3"));
extern VOID    i_MHIPlay         (struct MPHandle *handle asm("a3"));
extern VOID    i_MHIStop         (struct MPHandle *handle asm("a3"));
extern VOID    i_MHIPause        (struct MPHandle *handle asm("a3"));
extern ULONG   i_MHIQuery        (ULONG query asm("d1"));
extern VOID    i_MHISetParam     (struct MPHandle *handle asm("a3"), UWORD param asm("d0"), ULONG value asm("d1"));
#endif
