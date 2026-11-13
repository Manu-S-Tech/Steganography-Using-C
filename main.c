#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
#include <string.h>

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        printf("Insufficient arguments\n");
        return e_failure;
    }
    if (check_operation_type(argv[1]) == e_encode)
    {
        if (argc < 4)
        {
            printf("Insufficient arguments for encoding\n");
            return e_failure;
        }
        printf("encoding is started\n");

        EncodeInfo encInfo;

        // Validate the command-line arguments required for encoding
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("encoding is doing:");

            // Perform the complete encoding process
            if (do_encoding(&encInfo) == e_success)
            {
                printf("encoding is done sucessfully\n");
            }
            else
                printf("encoding is not sucessfully done\n");
        }
        else
            printf("encodinding is failure\n");
    }
    else if (check_operation_type(argv[1]) == e_decode)
    {
        if (argc < 3)
        {
            printf("Insufficient arguments for decoding\n");
            return e_failure;
        }
        printf("decoding is started\n");

        DecodeInfo decInfo;

        // Validate the command-line arguments required for decoding
        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("decoding is going on:");

            // Perform the complete decoding process
            if (do_decoding(&decInfo) == e_success)
            {
                printf("decoding is done sucessfully\n");
            }
            else
                printf("decoding is not sucessfully done\n");
        }
        else
            printf("decoding files are not correct\n");
    }
    else
        printf("given input of doing encoding and decoding is wrong\n");
    return 0;
}

OperationType check_operation_type(char *symbol)
{
    // Check whether the command-line option is for encoding
    if (strcmp(symbol, "-e") == 0)
        return e_encode;

    // Check whether the command-line option is for decoding
    else if (strcmp(symbol, "-d") == 0)
        return e_decode;

    // Return unsupported when an invalid option is given
    return e_unsupported;
}