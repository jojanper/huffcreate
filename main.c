#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "huffman.h"


static int message_rows = 19;

static char *message[] = {
    "\nparameters :\n",
    "h - Huffman code construction method (optional)",
    "[method]:",
    "\t* HUFFMAN_BASIC (default)",
    "\t* HUFFMAN_CONSTRAINED",
    "\t* HUFFMAN_AD_HOC",
    "\t* HUFFMAN_AD_HOC_ORDERED",
    "\t* HUFFMAN_VOORHIS",
    "[length]:",
    "\t* maximum codeword length in bits. This parameter should be",
    "\t  specified only for methods HUFFMAN_CONSTRAINED, HUFFMAN_AD_HOC,",
    "\t  HUFFMAN_AD_HOC_ORDERED and HUFFMAN_VOORHIS. This parameter will",
    "\t  set a constraint to the code construction so that no codeword will",
    "\t  be longer than the specified number of bits.\n",
    "<input_file> - name of the sample data file",
    "<output_file> - name of the output file",
    "\nShort description : ",
    "\nHuffman code construction program that can be used to perform a ",
    "lossless compression.\n",
    "NULL"
};


void
InfoMsg (char *prog_name)
{
  int i;

  fprintf(stderr,
          "\nusage : %s -h [method] [length] <input_file>\n",
          prog_name);

  for(i = 0; i < message_rows; i++)
    fprintf(stderr, "%s\n", message[i]);

  exit(EXIT_FAILURE);
}


void
command_line (int argc, char *argv[], HUFFMAN_METHOD *huffman_method,
              int *code_length, int *ordered, char **in_filename)
{
  char *prog_name;
  int i;

  prog_name = *argv;

  if (argc < 2)
    InfoMsg (prog_name);

  argv++;
  argc--;
  while(argc > 1)
  {
    i = 0;
    if (*argv[i] == '-')
    {
      switch (*(argv[i] + 1))
      {
        case 'h':
          argv++;

          if (strcmp (*argv, "HUFFMAN_BASIC") == 0 ||
              strcmp (*argv, "huffman_basic") == 0)
            *huffman_method = HUFFMAN_BASIC;

          else if(strcmp (*argv, "HUFFMAN_CONSTRAINED") == 0 ||
                  strcmp (*argv, "huffman_constrained") == 0)
            *huffman_method = HUFFMAN_CONSTRAINED;

          else if(strcmp (*argv, "HUFFMAN_AD_HOC") == 0 ||
                  strcmp (*argv, "huffman_ad_hoc") == 0)
            *huffman_method = HUFFMAN_AD_HOC;

          else if(strcmp (*argv, "HUFFMAN_AD_HOC_ORDERED") == 0 ||
                  strcmp (*argv, "huffman_ad_hoc_ordered") == 0)
            *huffman_method = HUFFMAN_AD_HOC_ORDERED;

          else if(strcmp (*argv, "HUFFMAN_VOORHIS") == 0 ||
                  strcmp (*argv, "huffman_voorhis")  == 0)
            *huffman_method = HUFFMAN_VOORHIS;
          else
          {
            fprintf(stderr,
                    "Error in Huffman method selection, '%s' is not valid.\n",
                    *argv);
            InfoMsg (prog_name);
          }

          if(*huffman_method != HUFFMAN_BASIC)
          {
            *code_length = atoi(*++argv);
            argc--;
          }

          argv++;
          argc -= 2;
          break;

        default:

          fprintf (stderr, "unknown option %s\n", argv[i]);
          InfoMsg (prog_name);
      }
    }
    else
      break;
  }

  if (argc != 1)
    InfoMsg (prog_name);

  *in_filename = *argv;

  return;
}


RLE *
read_data(FILE *fp, int *len)
{
  int i, read_samples;
  int run, size;
  float prob;
  RLE *rle_array;
  char szText[1024];

  *len = 0;
  while(fgets (szText, sizeof (szText), fp) != NULL)
    (*len)++;

  rewind(fp);

  rle_array = (RLE *)calloc(*len, sizeof(RLE));
  if(rle_array == NULL)
  {
    fprintf(stderr, "out of memory\n");
    exit(EXIT_FAILURE);
  }

  for(i = 0; i < *len; i++)
  {
    read_samples = fscanf(fp, "%i%i%f", &run, &size, &prob);
    if(read_samples != 3)
    {
      fprintf(stderr, "invalid data on line %i\n", i);
      exit(EXIT_FAILURE);
    }
    rle_array[i].run = run;
    rle_array[i].size = size;
    rle_array[i].prob = rle_array[i].original_prob = prob;
  }

  return (rle_array);
}


void
write_data(FILE *fp, HuffmanCode *hfcode, int len)
{
  int i, k, n;
  float sum;

  sum = 0;

  fprintf(fp, "(length)\t(run,size)\t(codeword)(codeword_in_bits)\n\n");

  for(i=0; i < len; i++)
  {
    fprintf(fp, "%4i\t\t (%2i,%2i)\t%7lu    (",
            hfcode[i].length, hfcode[i].rle.run, hfcode[i].rle.size,
            hfcode[i].codeword);

    for(k = hfcode[i].length, n = 1; k > 0; k--, n++)
      fprintf(stdout, "%i",
              ((hfcode[i].codeword) & (((int)1 << k)-1)) >> (k-1));

    fprintf(fp, ") %f\n", hfcode[i].rle.prob);
    sum += hfcode[i].rle.prob*hfcode[i].length;
  }

  {
    int *knownIndices, prevLen;

    knownIndices = (int *) calloc(len, sizeof(int));

    prevLen = 0;
    for(i=0; i < len; i++)
    {
      unsigned int index, clen, codeword;
      unsigned int index2, clen2, codeword2;

      index = hfcode[i].rle.size;
      clen = hfcode[i].length - prevLen;
      prevLen = hfcode[i].length;
      codeword = hfcode[i].codeword;

      /*fprintf(fp, "{%2i,%2i,%2i}, ", index, clen, codeword);*/

      index |= clen << 8;
      index |= codeword << 12;
      if(i && (i % 8) == 0)
	fprintf(fp, "\n");
      fprintf(fp, "%10i, ", index);
    }
    fprintf(fp, "\n\n");

    for(i=0; i < len; i++)
    {
      if(i && (i % 8) == 0)
	fprintf(fp, "\n");
      fprintf(fp, "{%3i,%3i}, ", hfcode[i].rle.size, hfcode[i].length);
    }
    fprintf(fp, "\n\n");

    for(i=0; i < len; i++)
    {
      int j;

      for(j=0; j < len; j++)
      {
	if(hfcode[j].rle.size == i && knownIndices[i] == 0)
	{
	  knownIndices[i] = 1;
	  if(i && (i % 5) == 0)
	    fprintf(fp, "\n");
	  fprintf(fp, "{%3i,%3i,%6i}, ", hfcode[j].rle.size, hfcode[j].length, hfcode[j].codeword);
	  break;
	}
      }
    }

    free(knownIndices);

    fprintf(fp, "\n");
  }

  fprintf(fp, "\nAverage codeword length %f bits.\n\n", sum);

}


void
main(int argc, char *argv[])
{
  char *in_filename;
  HUFFMAN_METHOD huffman_method = HUFFMAN_BASIC;
  int code_length = -1, len;
  FILE *fp_in;
  RLE *rle;
  HuffmanCode *hfcode;


  /* Process command line options. */
  command_line(argc, argv, &huffman_method, &code_length, NULL, &in_filename);

  fp_in = fopen(in_filename, "r");

  if(fp_in == NULL)
  {
    fprintf(stderr, "Unable to open file %s for reading\n", in_filename);
    exit(EXIT_FAILURE);
  }

  /* Read the symbols and their corresponding probabilities. */
  rle = read_data(fp_in, &len);

  hfcode = (HuffmanCode *)calloc(len, sizeof(HuffmanCode));
  if(hfcode == NULL)
  {
    fprintf(stderr, "out of memory\n");
    exit(EXIT_FAILURE);
  }

  /* Switch to selected Huffman code construction method. */
  switch(huffman_method)
  {
    case HUFFMAN_BASIC:
      Huffman(rle, hfcode, len);
      break;

    case HUFFMAN_CONSTRAINED:
      Huffman_Constrained(rle, hfcode, &len, code_length);
      break;

    case HUFFMAN_AD_HOC:
      Huffman_Ad_Hoc(rle, hfcode, len, code_length, 0);
      break;

    case HUFFMAN_AD_HOC_ORDERED:
      Huffman_Ad_Hoc(rle, hfcode, len, code_length, 1);
      break;

    case HUFFMAN_VOORHIS:
      fprintf(stderr,
              "Sorry, HUFFMAN_VOORHIS method is not implemented yet\n");
      exit(EXIT_FAILURE);
  }

  write_data(stdout, hfcode, len);

  /* Deallocate memory. */
  free(rle);
  free(hfcode);

  /* Close file handles. */
  fclose(fp_in);

  return;
}
