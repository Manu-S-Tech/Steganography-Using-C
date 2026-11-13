#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/*
 * Get image size
 * Input: Image file pointer
 * Output: width * height * bytes per pixel
 * Description: In BMP image, width is stored at offset 18
 * and height is stored immediately after width.
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    // Move file pointer to the position where image width is stored
    fseek(fptr_image, 18, SEEK_SET);

    // Read image width from BMP header
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read image height from BMP header
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Calculate image capacity using 3 bytes per pixel
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    long current_positon = ftell(fptr);

    // Move to the end of the file to find its size
    fseek(fptr, 0, SEEK_END);

    // Get the current position, which represents the file size
    current_positon = ftell(fptr);

    // Move the file pointer back to the beginning
    rewind(fptr);

    return (uint)current_positon;
}

/*
 * Get File pointers for input and output files
 * Inputs: Source image, Secret file and Stego image file
 * Output: FILE pointer for the above files
 * Return Value: e_success or e_failure on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Validate the source image extension before storing its file name
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
        encInfo->src_image_fname = argv[2];
    else
        return e_failure;

    // Validate the secret file extension and store its extension
    if (strcmp(strstr(argv[3], "."), ".txt") == 0)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, strstr(argv[3], "."));
    }
    else
        return e_failure;

    // Validate the optional stego image name or use the default name
    if (argv[4] != NULL)
    {
        if (strcmp(strstr(argv[4], "."), ".bmp") == 0)
            encInfo->stego_image_fname = argv[4];
        else
            return e_failure;
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Open the source image for reading
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");

    // Check whether the source image was opened successfully
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",
                encInfo->src_image_fname);

        return e_failure;
    }

    // Open the secret file for reading
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

    // Check whether the secret file was opened successfully
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",
                encInfo->secret_fname);

        return e_failure;
    }

    // Open the stego image for writing encoded data
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");

    // Check whether the stego image was opened successfully
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",
                encInfo->stego_image_fname);

        return e_failure;
    }

    // Return success when all required files are opened
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    // Check whether the image has enough space to hide all required data
    if (encInfo->image_capacity >
        16 + 32 + (strlen(encInfo->extn_secret_file) * 8) +
            32 + (encInfo->size_secret_file * 8))
        return e_success;

    return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // Buffer stores the 54-byte BMP header
    char buffer[54];

    // Move source file pointer to the beginning
    rewind(fptr_src_image);

    // Read the BMP header from the source image
    fread(buffer, 1, 54, fptr_src_image);

    // Copy the BMP header to the stego image
    fwrite(buffer, 1, 54, fptr_dest_image);

    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[8];

    // Process each character of the magic string separately
    for (int i = 0; i < strlen(magic_string); i++)
    {
        // Read 8 image bytes to store one character
        fread(buffer, 1, 8, encInfo->fptr_src_image);

        // Hide one magic string character in the LSBs of image bytes
        encode_byte_to_lsb(magic_string[i], buffer);

        // Write the modified image bytes to the stego image
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }

    // Check whether source and destination pointers are at the same position
    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
        return e_success;

    return e_failure;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char buffer[32];

    // Read 32 image bytes to store the extension size
    fread(buffer, 1, 32, encInfo->fptr_src_image);

    // Hide the extension size in the LSBs of image bytes
    encode_size_to_lsb(size, buffer);

    // Write the modified image bytes to the stego image
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buffer[8];

    // Process each character of the secret file extension
    for (int i = 0; i < strlen(file_extn); i++)
    {
        // Read 8 image bytes to store one extension character
        fread(buffer, 1, 8, encInfo->fptr_src_image);

        // Hide one extension character in the LSBs of image bytes
        encode_byte_to_lsb(file_extn[i], buffer);

        // Write the modified image bytes to the stego image
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];

    // Read 32 image bytes to store the secret file size
    fread(buffer, 1, 32, encInfo->fptr_src_image);

    // Hide the secret file size in the LSBs of image bytes
    encode_size_to_lsb(file_size, buffer);

    // Write the modified image bytes to the stego image
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // Read the complete secret file data into the structure
    fread(encInfo->secret_data, 1,
          encInfo->size_secret_file, encInfo->fptr_secret);

    char buffer[8];

    // Process each byte of the secret file
    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        // Read 8 image bytes to store one secret byte
        fread(buffer, 1, 8, encInfo->fptr_src_image);

        // Hide one secret byte in the LSBs of image bytes
        encode_byte_to_lsb(encInfo->secret_data[i], buffer);

        // Write the modified image bytes to the stego image
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    // Copy all remaining image bytes without modification
    while (fread(&ch, 1, 1, fptr_src) == 1)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }

    // Check whether all remaining data was copied successfully
    if (ftell(fptr_src) == ftell(fptr_dest))
    {
        printf("copying the remaining image data sucessfully\n");
        return e_success;
    }

    return e_failure;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Process all 8 bits of the data byte
    for (int i = 0; i < 8; i++)
    {
        // Extract the LSB of the data byte
        int dat_bit = data & 1;

        // Clear the LSB of the image byte
        image_buffer[i] = image_buffer[i] & (~1);

        // Store the extracted data bit in the image LSB
        image_buffer[i] = image_buffer[i] | dat_bit;

        // Move to the next bit of the data byte
        data >>= 1;
    }

    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    // Process all 32 bits of the integer size
    for (int i = 0; i < 32; i++)
    {
        // Extract one bit from the size value
        int dat_bit = size & 1;

        // Clear the LSB of the image byte
        imageBuffer[i] = imageBuffer[i] & (~1);

        // Store the size bit in the image LSB
        imageBuffer[i] = imageBuffer[i] | dat_bit;

        // Move to the next bit of the size value
        size >>= 1;
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    // Open all required files before starting the encoding process
    if (open_files(encInfo) == e_success)
        printf("opening all filese sucessfully\n");
    else
        return e_failure;

    // Check whether the source image has enough capacity for secret data
    if (check_capacity(encInfo) == e_success)
        printf("printing capacity of file sucessfully\n");
    else
        return e_failure;

    // Copy the original BMP header before modifying image data
    if (copy_bmp_header(encInfo->fptr_src_image,
                        encInfo->fptr_stego_image) == e_success)
        printf("copying bmp header\n");
    else
        return e_failure;

    // Encode the magic string into the image
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
        printf("encoding magic string sucessfully\n");
    else
        return e_failure;

    // Encode the size of the secret file extension
    encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo);

    // Encode the secret file extension
    encode_secret_file_extn(encInfo->extn_secret_file, encInfo);

    // Encode the size of the secret file
    encode_secret_file_size(encInfo->size_secret_file, encInfo);

    // Encode the actual secret file data
    encode_secret_file_data(encInfo);

    // Copy the remaining image data without modification
    if (copy_remaining_img_data(encInfo->fptr_src_image,
                                encInfo->fptr_stego_image) == e_success)
        printf("copying secret data from the secret file sucessfully\n");
    else
        return e_failure;

    return e_success;
}