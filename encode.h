#ifndef ENCODE_H
#define ENCODE_H

#include <stdio.h>

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[5];
    char secret_data[100];
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname; // Stores the output stego image file name
    FILE *fptr_stego_image;

} EncodeInfo;

/* Encoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the complete encoding process */
Status do_encoding(EncodeInfo *encInfo);

/* Open source, secret and stego image files */
Status open_files(EncodeInfo *encInfo);

/* Check whether the source image has enough capacity for secret data */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size from BMP header */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get the size of the secret file */
uint get_file_size(FILE *fptr);

/* Copy BMP header from source image to stego image */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Encode the magic string to identify the hidden data */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode the size of the secret file extension */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo);

/* Encode the secret file extension */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode the size of the secret file */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode the secret file data into the image */
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Hide one byte of data into the LSBs of image bytes */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Hide the size value into the LSBs of image bytes */
Status encode_size_to_lsb(int size, char *imageBuffer);

/* Copy the remaining image data without encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif