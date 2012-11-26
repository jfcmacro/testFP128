#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "BigRealNumber.h"

#define BUFFER_SIZE (sizeof(unsigned int) * 4)

FP128 bigZero  = { { 0x00000000,
		     0x00000000,
		     0x00000000,
		     0x00000000}};

FP128 bigUno   = { { 0x00000000,
		     0x00000000,
		     0x00000000,
		     0x3FFF0000}};

FP128 negBigUno = { { 0x00000000,
		     0x00000000,
		     0x00000000,
		     0xBFFF0000}};

PFP128 pBigZero = &bigZero;

Mant128 mant1 = { 0xFFFFFFFF,
		  0xFFFFFFFF,
		  0x00000000,
		  0x00010000};

Mant128 mant2 = { 0xFFFFFFFF,
		  0x00000000,
		  0x00000000,
		  0x00010000};

Mant128 mant3 = { 0x00000000,
		  0x00000000,
		  0x00000000,
		  0x00000000};

FP128 result;

Mant128 resultMant;

void
usage(char *pgname) {
  fprintf(stderr, "Usage: %s <result file> [1..10]\n", pgname);
  exit(1);
}

int
main(int argc, char *argv[]) {

  FILE *fd;
  char *buffer;
  int nTest = 1;
  FP128 resValue;
  int correctTest = 0;
  int oneTestFile = 0;
  int nOneTestFile = 0;

  buffer = (char *) malloc(BUFFER_SIZE * 3);

  if (!buffer) {
    fprintf(stderr, "Error: can't get more memory cause: %d\n", errno);
    exit(1);
  }

  if (argc > 3 || argc < 2) {
    usage(argv[0]);
  }

  if (argc == 3) {
    oneTestFile = 1;
    nOneTestFile = atoi(argv[2]);
    fprintf(stdout, "Test: %d \n", nOneTestFile);
    if (nOneTestFile > 10 || nOneTestFile < 0) {
      usage(argv[0]);
    }
  }

  if (!(fd = fopen(argv[1], "r"))) {
    fprintf(stderr, "Error: opening %s cause: %d\n", argv[1], errno);
    exit(1);
  }

  while (fread(buffer, BUFFER_SIZE, 3, fd) != 0) {
    PFP128 oper1  = (PFP128) (buffer); ;
    PFP128 oper2  = (PFP128) (buffer + BUFFER_SIZE);
    PFP128 result = (PFP128) (buffer + (BUFFER_SIZE * 2));

    fprintf(stdout, "*** Test %d ***\noper1: ", 
	    (oneTestFile) ? nOneTestFile : nTest);

    printfPFP128(stdout, oper1);
    fprintf(stdout, "\noper2: ");
    printfPFP128(stdout, oper2);
    fprintf(stdout, "\nresult: ");
    printfPFP128(stdout, result);
    fprintf(stdout, "\nexpected result: ");


    setZeroPFP128(&resValue);

    sumarFP128(oper1, oper2, &resValue);
    printfPFP128(stdout, &resValue);

    if (equalsFPF128(result, &resValue)) {
      fprintf(stdout, ". Test has passed\n");
      correctTest++;
    }
    else {
      fprintf(stdout, ". Test has failed\n");
    }

    nTest++;
    if (oneTestFile) {
      break;
    }
  }

  nTest--;
  if (oneTestFile) {
    fprintf(stdout, "\n Summary: Test: 1 Correct: %d Result: %2.2f%\n", 
	    correctTest, ((float) correctTest / 1.0f) * 5.0f); 
  }
  else {
    fprintf(stdout, "\n Summary: Test: %d Correct: %d Result: %2.2f\n", 
	    nTest,
	    correctTest, ((float) correctTest / (float) nTest) * 5.0f);
  }
  return 0;
}
