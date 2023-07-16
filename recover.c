#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARS 50

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf ("Usage: <program> <input.raw> <output_folder_path>\n");
    return EXIT_FAILURE;
  }

  char input[MAX_CHARS];
  char output[MAX_CHARS];

  strcpy_s(input, sizeof(input), argv[1]);
  strcpy_s(output, sizeof(output), argv[2]);



  return EXIT_SUCCESS;
}