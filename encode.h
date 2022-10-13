#ifndef ENCODE_H
#define ENCODE_H

#include "types.h"      //Contains User-defined Data Types.

/* 
 * Structure to store information required for Encoding Secret Data file to Source Image file
 * Information about Output and Intermediate data is also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 10
#define MAX_MAGIC_STRING_LENGTH 20

typedef struct _EncodeInfo
{
    /* Source Image file Info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data [MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file [MAX_FILE_SUFFIX];
    char secret_data [MAX_SECRET_BUF_SIZE];
    uint size_secret_file;

    /* Stego Image file Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Magic String Info */
    char magic_string [MAX_MAGIC_STRING_LENGTH];
    uint magic_string_size;

} EncodeInfo;

/* Encoding Function Prototype */

/* Check Operation type */
OperationType check_operation_type (char *argv[]);

/* Read and Validate Encode arguments from argv[] */
Status read_and_validate_encode_args (char *argv[], EncodeInfo *encInfo);

/* Perform the Encoding */
Status do_encoding (EncodeInfo *encInfo);

/* Get File pointers for I/P and O/P files */
Status open_files (EncodeInfo *encInfo);

/* Check Source Image file Capacity */
Status check_capacity (EncodeInfo *encInfo);

/* Get Source Image file size */
uint get_image_size_for_bmp (uint* bits_per_pixel, FILE *fptr_image);

/* Get Secret Data file size */
uint get_file_size (FILE *fptr);

/* Copy '.bmp' Source Image Header */
Status copy_bmp_header (FILE *fptr_src_image, FILE *fptr_dest_image);

/* Encode Magic String */
Status encode_magic_string (const char *magic_string, EncodeInfo *encInfo);

/* Encode Secret Data file Extenstion */
Status encode_secret_file_extn (const char *file_extn, EncodeInfo *encInfo);

/* Encode Secret Data file size */
Status encode_secret_file_size (uint file_size, EncodeInfo *encInfo);

/* Encode Secret file data */
Status encode_secret_file_data (EncodeInfo *encInfo);

/* Encode function: which does the real Encoding */
Status encode_data_to_image (const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a Byte into LSB of Image data array */
Status encode_byte_to_lsb (char data, char *image_buffer);

/* Copy remaining Image bytes from Source to Stego Image after encoding */
Status copy_remaining_img_data (FILE *fptr_src, FILE *fptr_dest);

#endif
