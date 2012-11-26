#include <stdlib.h>
#include <strings.h>
#include "BigRealNumber.h"

static Mant128 unoMant128 = { 0x00000001,
			      0x00000000,
			      0x00000000,
			      0x00000000 };

static Mant128 zeroMant128 = { 0x0000000,
			       0x0000000,
			       0x0000000,
			       000000000 };

static unsigned int EXP_ZERO = 0x00000000;
static unsigned int EXP_UNO  = 0X00007FFF;


void
swapUI(unsigned int* val1, unsigned int* val2) {
  unsigned int tmp = *val1;
  *val1 = *val2;
  *val2 = tmp;
}

int
isZero(unsigned exp, PMant128 oper) {
  return (exp == EXP_ZERO &&
	  equalsMant128(oper, &zeroMant128) == 1) ? 1 : 0;
}

int
isNan(unsigned exp, PMant128 oper) {
  return (exp == EXP_UNO &&
	  equalsMant128(oper, &zeroMant128) == 0) ? 1 : 0;
}

int
isInfy(unsigned exp, PMant128 oper) {
  return (exp == EXP_UNO && 
	  equalsMant128(oper, &zeroMant128) == 1) ? 1 : 0;
}

void
sumarFP128(PFP128 oper1, PFP128 oper2, PFP128 result) {

  unsigned int exp1 = getExpFromPFP128(oper1);
  unsigned int exp2 = getExpFromPFP128(oper2);
  unsigned int tmp;
  unsigned int resultExp;
  unsigned int sig1 = getSignFromPFP128(oper1);
  unsigned int sig2 = getSignFromPFP128(oper2);
  unsigned int resultSig;
  PMant128 pMant1 = newPMant128();
  PMant128 pMant2 = newPMant128();
  PMant128 pResultMant = newPMant128();
  unsigned int diffExp;
  int normalize;
  getMant128FromPFP128(oper1, pMant1, 0);
  getMant128FromPFP128(oper2, pMant2, 0);

  if (isNan(exp1, pMant1)) {
    assemblyResult(sig1, exp1, pMant1, result);
    return;
  }

  if (isNan(exp2, pMant2)) {
    assemblyResult(sig2, exp2, pMant2, result);
    return;
  }

  if (isInfy(exp1, pMant1)) {
    if (isInfy(exp2, pMant2)) {

      if (sig1 == 0 && sig2 == 0) {
	assemblyResult(sig1, exp1, pMant1, result);
	return;
      }

      if (sig1 == 0 && sig2 == 1) {
	assemblyResult(sig1, EXP_UNO, &unoMant128, result);
	return;
      }

      if (sig1 == 1 && sig2 == 0) {
	assemblyResult(sig1, EXP_UNO, &unoMant128, result);
	return;
      }

      if (sig1 == 1 && sig2 == 1) {
	assemblyResult(sig1, exp1, pMant1, result);
	return;
      }
    }
    else {

      assemblyResult(sig1, exp1, pMant1, result);
      return;
    }
  }

  if (isInfy(exp2, pMant2)) {
    assemblyResult(sig2, exp2, pMant2, result);
    return;
  }

  if (isZero(exp1, pMant1)) {
    assemblyResult(sig2, exp2, pMant2, result);
    return;
  }

  if (isZero(exp2, pMant2)) {
    assemblyResult(sig1, exp1, pMant1, result);
    return;
  }

  getMant128FromPFP128(oper1, pMant1, NORMALIZEVALUE);
  getMant128FromPFP128(oper2, pMant2, NORMALIZEVALUE);

  if (exp1 < exp2) {
    PMant128 pTmp;
    swapUI(&sig1, &sig2);
    swapUI(&exp1, &exp2);
    pTmp = pMant1;
    pMant1 = pMant2;
    pMant2 = pTmp;
  }

  diffExp = exp1 - exp2;

  sslPMant128(pMant2, diffExp);

  if (sig1) {
    compl2PMant128(pMant1);
  }

  if (sig2) {
    compl2PMant128(pMant2);
  }

  resultSig = addPMant128(pMant1, pMant2, pResultMant);

  if (resultSig) {
    compl2PMant128(pResultMant);
  }

  normalize = normalizePMant128(pResultMant);

  resultExp = exp1 + normalize;

  if (normalize < 129) {
    unnormalizePMant128(pResultMant);
  }
  else {
    resultExp = 0; // Zero
  }

  assemblyResult(resultSig, resultExp, pResultMant, result);
}

unsigned int
getSignFromPFP128(PFP128 oper) {
  return (oper->rep.flint[3] & SIGMASK) >> 31;
}

unsigned int
getExpFromPFP128(PFP128 oper) {
  return (oper->rep.flint[3] & EXPMASK) >> 16;
}

void
getMant128FromPFP128(PFP128 fp, PMant128 result, unsigned int norm) {
  int i;
  for (i = 0; i < 4; i++) {

    if (i != 3) {
      result->mant[i] = fp->rep.flint[i];
    }
    else {
      result->mant[i] = (fp->rep.flint[i] & MANTMASKHV) | norm;
    }
  }
}

void
getNorMant128FromPFP128(PFP128 fp, PMant128 result) {

  getMant128FromPFP128(fp, result, NORMALIZEVALUE);
}


void
compl2PMant128(PMant128 pmant) {
  PMant128 pResult = newPMant128();
  compl1PMant128(pmant);
  addPMant128(pmant, &unoMant128, pResult);
  bcopy(pResult, pmant, sizeof(unsigned int) * 4);
}

void
compl1PMant128(PMant128 pmant) {
  int i;

  for (i = 0; i < 4; i++) {
    pmant->mant[i] = ~pmant->mant[i];
  }
}

void
printfPMant128(FILE *output, PMant128 pmant) {
  int i;
  for (i = 3; i >= 0; i--) {
    fprintf(output, "%08X", pmant->mant[i]);
  }
}

void
printfPFP128(FILE *output, PFP128 pfp) {
  int i;
  fprintf(output, "0x");
  for (i = 3; i >= 0; i--) {
    fprintf(output, "%08X", pfp->rep.flint[i]);
  }
}

PMant128
newPMant128() {
  return malloc(sizeof(Mant128));
}

/* void */
/* fitMant128(PMant128 pMant, int diff) { */
/*   int i; */
/*   int j; */
/*   unsigned int bitBefore; */

/*   for (j = 0; j <= diff; j++) { */
/*     i = 3; */
/*     bitBefore = 0; */
/*     do { */
/*       unsigned int bitBefore = pMant->mant[i] & LASTBITMASK; */
/*       pMant->mant[i] = bitBefore | pMant->mant[i] >> 1; */
/*     } while (i <= 0); */
/*   } */
/* } */


int
normalizePMant128(PMant128 pMant) {
  int normRet = NONORMALIZEDVALUE;
  unsigned int mask = LASTBITMASK;
  int i;
  int j;
  for (j = 3; j >= 0; j--) {

    for (i = 31, mask = LASTBITMASK;
	 mask != 0 && (mask & pMant->mant[j]) == 0;
	 mask >>= 1, i--) {
    }

    if (mask & pMant->mant[j]) {

      if (j == 3) {

	if (i - 16 >= 0) {
	  normRet = i - 16;
	  ssrPMant128(pMant, normRet);
	}
	else {
	  normRet = 16 - i;
	  sslPMant128(pMant, normRet);
	  normRet = -normRet;
	}
	return normRet;
      }
      else {
	normRet = 16 + (31 - i) + (2-j)*32;
	sslPMant128(pMant, normRet);
	return -normRet;
      }
    }
  }

  return normRet;
}

void
unnormalizePMant128(PMant128 pMant) {

  pMant->mant[3]  = pMant->mant[3] & MANTMASKHV;
}

void
assemblyResult(unsigned int sign, unsigned int exp, PMant128 pMant, PFP128 pfp) {
  int i;
  setZeroPFP128(pfp);
  pfp->rep.flint[3] = (sign ? SIGMASK : 0) | (exp << 16) | (pMant->mant[3] & MANTMASKHV);
  for (i = 2; i >= 0; i--) {
    pfp->rep.flint[i] = pMant->mant[i];
  }
}

void
sslPFP128(PFP128 pfp, int times) {
  int j;
  for (j = 0; j < times; j++) {
    unsigned int bitBefore = 0;
    int i;
    for (i = 0; i < 4; i++) {
      unsigned int tmpBitBefore = pfp->rep.flint[i] & LASTBITMASK;
      pfp->rep.flint[i] << 1 & bitBefore;
      bitBefore = tmpBitBefore;
    }
  }
}

void
ssrPFP128(PFP128 pfp, int times) {
  int j;

  for (j = 0; j < times; j++) {

    unsigned int bitBefore = 0;
    int i;

    for (i = 3; i >= 0; i--) {
      unsigned int tmpBitBefore = pfp->rep.flint[i] & FIRSTBITMASK;
      pfp->rep.flint[i] >> 1 & bitBefore;
      bitBefore = tmpBitBefore;
    }
  }
}

void
sslPMant128(PMant128 pMant, int times) {
  int j;

  for (j = 0; j < times; j++) {

    unsigned int bitBefore = 0;
    int i;

    for (i = 0; i < 4; i++) {
      unsigned int tmpBitBefore = pMant->mant[i] & LASTBITMASK;
      pMant->mant[i] = pMant->mant[i] << 1 & bitBefore;
      bitBefore = tmpBitBefore;
    }
  }
}

void
ssrPMant128(PMant128 pMant, int times) {

  int j;

  for (j = 0; j < times; j++) {

    unsigned int bitBefore = 0;
    int i;

    for (i = 3; i >= 0; i--) {
      unsigned int tmpBitBefore = pMant->mant[i] & FIRSTBITMASK;
      pMant->mant[i] = pMant->mant[i] >> 1 | bitBefore;
      bitBefore = tmpBitBefore;
    }
  }
}


void
setZeroPFP128(PFP128 pfp) {
  bzero(pfp->rep.flint, 16);
}

void
setZeroPMant128(PMant128 pMant) {
  bzero(pMant->mant, 16);
}

int
addPMant128(PMant128 oper1, PMant128 oper2, PMant128 result) {
  int sig;
  unsigned carry = 0;
  int i;

  for (i = 0; i < 4; i++) {
    int firstCarry = 0;
    int secondCarry = 0;
    unsigned tmp = oper1->mant[i] + carry;
    firstCarry = ~(oper1->mant[i] | 0) < carry ? 1 : 0;
    result->mant[i] = tmp + oper2->mant[i];
    secondCarry = ~(tmp | 0) < oper2->mant[i] ? 1 : 0;
    carry = firstCarry | secondCarry;
  }
  return (result->mant[3] & SIGMASK) >> 31;
}

int
equalsMant128(PMant128 oper1, PMant128 oper2) {
  int ret = 1;
  int i;
  for (i = 0; i < 4; ++i) {
    if (oper1->mant[i] != oper2->mant[i]) {
      ret = 0;
      break;
    }
  }
  return ret;
}

int
equalsFPF128(PFP128 oper1, PFP128 oper2) {
  int ret = 1;
  int i;
  for (i = 0; i < 4 && ret; i++) {
    if (oper1->rep.flint[i] != oper2->rep.flint[i]) {
      ret = 0;
    }
  }
  return ret;
}

