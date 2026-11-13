#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"
#include <string.h>

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Validate the stego image extension before storing its file name
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decInfo->stego_image_fname = argv[2]; // store argv[2] in stego_image_fname
    }
    else
    {
        printf("Invalid Decoding image source file format. Please provide a BMP file.\n");
        return e_failure;
    }

    // Validate the optional output secret file extension
    if (argv[3] != NULL)
    {
        if (strcmp(strstr(argv[3], "."), ".txt") == 0)
            decInfo->secret_fname = argv[3]; // store argv[3] in secret_fname
        else
            return e_failure;
    }
    else
        decInfo->secret_fname = "output.txt";

    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    // Open the stego image in binary read mode for decoding
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    // Check whether the stego image was opened successfully
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s",
                decInfo->stego_image_fname);

        return e_failure;
    }

    // Open the output file to store the decoded secret data
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "wb");

    // Check whether the output file was opened successfully
    if (decInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",
                decInfo->secret_fname);

        return e_failure;
    }

    return e_success;
}

Status skip_bmp_header(FILE *fptr_stego_image)
{
    // Skip the 54-byte BMP header before reading hidden data
    fseek(fptr_stego_image, 54, SEEK_SET);

    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char buffer[8];
    char decoded_magic[100];

    int len = strlen(magic_string);

    // Decode each character of the magic string from 8 image bytes
    for (int i = 0; i < len; i++)
    {
        // Read 8 image bytes containing one hidden character
        fread(buffer, 1, 8, decInfo->fptr_stego_image);

        // Extract one character from the LSBs of image bytes
        decoded_magic[i] = decode_byte_from_lsb(buffer);
    }

    decoded_magic[len] = '\0';

    // Compare decoded magic string with the expected magic string
    if (strcmp(decoded_magic, magic_string) == 0)
    {
        return e_success;
    }

    return e_failure;
}

char decode_byte_from_lsb(char *buffer)
{
    char ch = 0;

    // Extract 8 bits from the LSBs and reconstruct one byte
    for (int i = 0; i < 8; i++)
    {
        ch = ch | ((buffer[i] & 1) << i);
    }

    return ch;
}

Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo)
{
    char buffer[32];

    *size = 0;

    // Read 32 image bytes containing the extension size
    fread(buffer, 1, 32, decInfo->fptr_stego_image);

    // Reconstruct the extension size from the image LSBs
    for (int i = 0; i < 32; i++)
    {
        *size = *size | ((buffer[i] & 1) << i);
    }

    return e_success;
}

Status decode_secret_file_extension(char *file_extn, DecodeInfo *decInfo)
{
    char buffer[8];

    // Decode each character of the secret file extension
    for (int i = 0; i < decInfo->extn_size; i++)
    {
        // Read 8 image bytes containing one extension character
        fread(buffer, 1, 8, decInfo->fptr_stego_image);

        // Extract one extension character from the image LSBs
        file_extn[i] = decode_byte_from_lsb(buffer);
    }

    file_extn[decInfo->extn_size] = '\0';

    return e_success;
}

Status decode_secret_file_size(int *file_size, DecodeInfo *decInfo)
{
    char buffer[32];

    *file_size = 0;

    // Read 32 image bytes containing the secret file size
    fread(buffer, 1, 32, decInfo->fptr_stego_image);

    // Reconstruct the secret file size from the image LSBs
    for (int i = 0; i < 32; i++)
    {
        *file_size = *file_size | ((buffer[i] & 1) << i);
    }

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;

    // Decode each byte of the secret file data
    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        // Read 8 image bytes containing one secret byte
        fread(buffer, 1, 8, decInfo->fptr_stego_image);

        // Extract one secret byte from the image LSBs
        ch = decode_byte_from_lsb(buffer);

        // Write the decoded byte into the output secret file
        fwrite(&ch, 1, 1, decInfo->fptr_secret);
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    // Open the stego image and output secret file
    if (open_decode_files(decInfo) == e_success)
        printf("opening all files sucessfully\n");
    else
        return e_failure;

    // Skip the BMP header before decoding hidden information
    if (skip_bmp_header(decInfo->fptr_stego_image) == e_success)
        printf("skipping the bmp header sucessfully\n");
    else
        return e_failure;

    // Decode and verify the magic string
    if (decode_magic_string(MAGIC_STRING, decInfo) == e_success)
        printf("decoding magic string sucessfully\n");
    else
        return e_failure;

    // Decode the size of the secret file extension
    if (decode_secret_file_extn_size(&decInfo->extn_size, decInfo) == e_success)
        printf("decoding secret file extension size has been done\n");
    else
        return e_failure;

    // Decode the secret file extension
    if (decode_secret_file_extension(decInfo->extn_secret_file, decInfo) == e_success)
        printf("decoding secret file extension sucessfully\n");
    else
        return e_failure;

    // Decode the size of the secret file
    if (decode_secret_file_size(&decInfo->size_secret_file, decInfo) == e_success)
        printf("decoding secret file size sucessfully\n");
    else
        return e_failure;

    // Decode the actual secret file data
    if (decode_secret_file_data(decInfo) == e_success)
        printf("decoding secret file data sucessfully\n");
    else
        return e_failure;

    return e_success;
}