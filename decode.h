#ifndef DEODE_H
#define DECODE_H

#include "types.h"      //Contains User-defined Data Types.

/* 
 * Structure to store information required for Decoding Secret Data file to Source Image file
 * Information about Output and Intermediate data is also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 10
#define MAX_MAGIC_STRING_LENGTH 20

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char image_data [MAX_IMAGE_BUF_SIZE];

    /* Magic String Info */
    char magic_string [MAX_MAGIC_STRING_LENGTH];
    uint magic_string_size;

    /* Secret File Info */
    char secret_fname [10 + MAX_FILE_SUFFIX];
    FILE *fptr_secret;
    char extn_secret_file [MAX_FILE_SUFFIX];
    char secret_data [MAX_SECRET_BUF_SIZE];
    uint size_secret_file;

} DecodeInfo;

/* Decoding Function Prototype */

/* Read and Validate Decode args from argv */
Status read_and_validate_decode_args (char *argv[], DecodeInfo *decInfo);

/* Perform the Decoding */
Status do_decoding (char *argv[], DecodeInfo *decInfo);

/* Get File pointers for I/P and O/P files */
Status open_files_2 (DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string (DecodeInfo *decInfo);

/* Decode Secret Data file Extenstion */
Status decode_secret_file_extn (DecodeInfo *decInfo);

/* Decode Secret Data file size */
Status decode_secret_file_size (DecodeInfo *decInfo);

/* Decode Secret file data*/
Status decode_secret_file_data (DecodeInfo *decInfo);

/* Decode function: which does the real Decoding */
Status decode_data_from_image (char *data, int size, FILE *fptr_stego_image);

/* Decode a Byte from LSB of Image data array */
Status decode_byte_from_lsb (char* data, char *image_buffer);

#endif
