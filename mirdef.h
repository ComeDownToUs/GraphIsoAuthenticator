/*
 *   MIRACL compiler/hardware definitions - mirdef.h
 */
#define MR_LITTLE_ENDIAN
#define MIRACL 8
#define mr_utype char
#define MR_IBITS 32
#define MR_LBITS 32
#define mr_unsign32 unsigned int
#define mr_dltype short
#define mr_qltype long
#define MR_NOASM
#define MR_FLASH 52
#define MR_ALWAYS_BINARY
#define MAXBASE ((mr_small)1<<(MIRACL-1))
#define MR_KCM 4
#define MR_BITSINCHAR 8
#define MR_SMALL_AES
