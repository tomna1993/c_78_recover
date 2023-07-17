#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARS 50
#define MAX_ITERATION 2000

int32_t FindStartOfImage(FILE *input_fp, uint8_t *buffer, int32_t count_bytes);
int32_t ReadUntilEndOfImage(FILE *input_fp, uint8_t *buffer, int32_t count_bytes,
                            int32_t max_read_bytes, FILE *output_fp);

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Usage: <program> <input.raw> <output_folder_path>\n");
    return EXIT_FAILURE;
  }

  char input[MAX_CHARS];
  char output[MAX_CHARS];
  char file_name[MAX_CHARS];

  strcpy_s(input, sizeof(input), argv[1]);
  strcpy_s(output, sizeof(output), argv[2]);

  FILE *input_fp = fopen(input, "rb");

  if(input_fp == NULL)
  {
    printf("Failed to open the file!\n");
    return EXIT_FAILURE;
  }

  const int32_t max_read_bytes = 512;

  uint8_t *buffer = (uint8_t *)malloc(sizeof(buffer) * max_read_bytes);

  if(buffer == NULL)
  {
    printf ("Memory allocation failed!\n");
    return EXIT_FAILURE;
  }


  int16_t img_count = 0;

  while(true)
  {
    int32_t count_bytes = 0;
    
    count_bytes = FindStartOfImage(input_fp, buffer, count_bytes);

    if(count_bytes == EOF)
    {
      printf("Reached end of the file\n");
      break;
    }
    else if (count_bytes == 0)
    {
      printf("Reached max iterations!\n");
      break;
    }
    

    sprintf_s(file_name, MAX_CHARS, "%s/%i.JPEG", output, img_count);

    ++img_count;

    FILE *output_fp = fopen(file_name, "wb");

    if(output_fp == NULL)
    {
      printf("Cannot create file!\n");
      break;
    }

    count_bytes = ReadUntilEndOfImage(input_fp, buffer, count_bytes,
                                      max_read_bytes, output_fp);

    if(count_bytes == EOF)
    {
      printf("Reached end of the file\n");
      fclose(output_fp);
      break;
    }
    
    fclose(output_fp);
  }

  free(buffer);
  buffer = NULL;

  fclose(input_fp);

  return EXIT_SUCCESS;
}

int32_t FindStartOfImage(FILE *input_fp, uint8_t *buffer, int32_t count_bytes)
{
  int32_t iter = 0;
  do
  {
    fread(buffer + count_bytes, sizeof(*buffer), 1, input_fp);

    if(feof(input_fp))
    {
      printf("End of file\n");
      return EOF;
    }

    if (count_bytes == 0 && *buffer == 0xFF)
    {
      ++count_bytes;
    }
    else if(count_bytes == 1 && *(buffer + count_bytes) == 0xD8)
    {
      printf("Start of image\n");
      return count_bytes;
    }
    else if(count_bytes == 1)
    {
      count_bytes = 0;
    }
  } while(++iter < MAX_ITERATION);

  return 0;
}

int32_t ReadUntilEndOfImage(FILE *input_fp, uint8_t *buffer, int32_t count_bytes,
                            int32_t max_read_bytes, FILE *output_fp)
{
  int32_t iter = 0;

  do
  {
    ++count_bytes;

    if(count_bytes >= max_read_bytes)
    {
      printf("Read another 512 bytes\n");

      fwrite(buffer, sizeof(*buffer), count_bytes, output_fp);

      iter = 0;

      count_bytes = 0;
    }

    fread(buffer + count_bytes, sizeof(*buffer), 1, input_fp);

    if(feof(input_fp))
    {
      printf("End of file\n");
      return EOF;
    }

    if(count_bytes >= 1 && 
            *(buffer + count_bytes - 1) == 0xFF && 
            *(buffer + count_bytes) == 0xD9)
    {
      printf("End of image\n");

      fwrite(buffer, sizeof(*buffer), count_bytes, output_fp);

      return EXIT_SUCCESS;
    }
  } while(++iter < MAX_ITERATION);

  return EXIT_FAILURE;
}