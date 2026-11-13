#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include <string.h>
#include <stdio.h>

typedef struct _DecodeInfo
{
    /* Source image information used for decoding */
    char *stego_image_fname;       // Stores the encoded image file name
    FILE *fptr_stego_image;        // file pointer for stego file

    /* Secret file information */
    char *secret_fname;            // Stores the output secret file name
    FILE *fptr_secret;              //file pointer for secret file
    char extn_secret_file[5];      // Stores the decoded secret file extension
    char secret_data[100];          // Stores the decoded secret data
    int size_secret_file;           // Stores the decoded secret file size
    int extn_size;                  // Stores the decoded extension size

} DecodeInfo;

/* Validate the command-line arguments required for decoding */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Open the stego image and output secret file */
Status open_decode_files(DecodeInfo *decInfo);

/* Skip the BMP header before starting the decoding process */
Status skip_bmp_header(FILE *fptr_stego_image);

/* Decode and verify the magic string from the stego image */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Extract one byte from the LSBs of image data */
char decode_byte_from_lsb(char *buffer);

/* Decode the size of the secret file extension */
Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo);

/* Decode the secret file extension */
Status decode_secret_file_extension(char *file_extn, DecodeInfo *decInfo);

/* Decode the size of the secret file */
Status decode_secret_file_size(int *file_size, DecodeInfo *decInfo);

/* Decode the actual secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Perform the complete decoding process */
Status do_decoding(DecodeInfo *decInfo);

#endif