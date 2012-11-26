#ifndef _BIGREALNUMBER_
#define _BIGREALNUMBER_

#include <stdio.h>

typedef struct FloatPointNumber128 {
  union {
    unsigned int flint[4];
    unsigned char flchar[16];
  } rep;
} FP128, *PFP128;

typedef struct Mantisa128 {
  unsigned int mant[4];
} Mant128, *PMant128;

// Operations

void
sumarFP128(PFP128, PFP128, PFP128);

unsigned int
getSignFromPFP128(PFP128);

unsigned int
getExpFromPFP128(PFP128);

void
getNorMant128FromPFP128(PFP128, PMant128);

void
getMant128FromPFP128(PFP128, PMant128, unsigned int);

void
compl2PMant128(PMant128);

void
compl1PMant128(PMant128);

/* void */
/* fitMant128(PMant128, int); */

void
printfPMant128(FILE*, PMant128);

void
printfPFP128(FILE*, PFP128);

PMant128
newPMant128();

int
normalizePMant128(PMant128);

void
unnormalizePMant128(PMant128);

void
assemblyResult(unsigned int, unsigned int, PMant128, PFP128);

void 
sslPFP128(PFP128, int);

void 
ssrPFP128(PFP128, int);

void 
sslPMant128(PMant128, int);

void 
ssrPMant128(PMant128, int);

void
setZeroPFP128(PFP128);

void
setZeroPMant128(PMant128);

int
addPMant128(PMant128, PMant128, PMant128);

int 
equalsFPF128(PFP128, PFP128);
// Constants

#define MANTMASKHV         0x0000FFFF
#define EXPMASK            0x7FFF0000
#define SIGMASK            0x80000000
#define LASTBITMASK        SIGMASK
#define FIRSTBITMASK       0x00000001
#define NORMALIZEVALUE     0x00010000
#define NONORMALIZEDVALUE  4*32+1
#define NUMBERBITSUI       32
#define BIAS               16383

#endif
