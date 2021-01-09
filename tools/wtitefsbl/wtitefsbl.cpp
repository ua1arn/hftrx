// wtitefsbl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

static int
writeto(FILE * fpdisk, FILE * fpboot, long offset, long length)
{
 if (fseek(fpdisk, offset, SEEK_SET) != 0)
  return 1;

 rewind(fpboot);
 
 int c;
 while (length -- && (c = fgetc(fpboot)) != EOF)
 {
  if (fputc(c, fpdisk) == EOF)
   return 1;
 }
 return fflush(fpdisk);
}

static int
readfrom(FILE * fpdisk, FILE * fpboot, long offset, long size)
{
 if (fseek(fpdisk, offset, SEEK_SET) != 0)
  return 1;

 //rewind(fpboot);
 
 int c;
 while (size -- && (c = fgetc(fpdisk)) != EOF)
 {
  if (fputc(c, fpboot) == EOF)
   return 1;
 }
 return fflush(fpboot);
}

//  if (writeto(fpdisk, fpboot, 0x4400))
//   return 1;
//  if (writeto(fpdisk, fpboot, 0x4400 + (256L * 1024)))

// wtitefsbl image.raw 0x004400 fsbl.stm32
// wtitefsbl image.raw 0x044400 fsbl.stm32
int main(int argc, char* argv[])
{
 if (argc >= 5 && strcmp(argv [4], "-x") == 0)
 {
	  FILE * fpdisk;
   long target;
   FILE * fpboot;
   const long length = argc >= 6 ? strtol(argv [5], NULL, 0) : LONG_MAX;

   fpdisk = fopen(argv [1], "rb");
   target = strtol(argv [2], NULL, 0);
   fpboot = fopen(argv [3], "wb");

   if (fpdisk == NULL || fpboot == NULL)
    return 1;

   if (readfrom(fpdisk, fpboot, target, length))
    return 1;

  fprintf(stderr, "Read succesful\n");
   return 0;
 }
 else if (argc >= 5 && strcmp(argv [4], "-w") == 0)
 {
	  FILE * fpdisk;
   long target;
   FILE * fpboot;
   const long length = argc >= 6 ? strtol(argv [5], NULL, 0) : LONG_MAX;

   fpdisk = fopen(argv [1], "r+b");
   target = strtol(argv [2], NULL, 0);
   fpboot = fopen(argv [3], "rb");

   if (fpdisk == NULL || fpboot == NULL)
    return 1;

   if (writeto(fpdisk, fpboot, target, length))
    return 1;

  fprintf(stderr, "Write succesful\n");
   return 0;
 }
 else
 {
  fprintf(stderr, "Not enough args\n");
  return 1;
 }
 return 0;
}

