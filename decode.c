#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"
#include <stdlib.h>

// Function definition for read and validate decode args
Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Validate source image (.bmp)
    if (argv[2] != NULL)
    {// Check for .bmp extension
        char *src_ext = strrchr(argv[2], '.');
        // Validate extension
        if (src_ext != NULL && strcmp(src_ext, ".bmp") == 0)
        {// Copy source image file name
            decInfo->d_src_image_fname = argv[2];
        }
        else
        {// If extension is not valid
            printf("\nInvalid Source Image Format! Expected a .bmp file.\n");
            return d_failure;
        }
    }
    else
    {// If argv[2] is NULL
        printf("\nError: Source image argument missing!\n");
        return d_failure;
    }

    // Validate optional output text file
    if (argv[3] != NULL)
    {// Check for .txt extension
        char *out_ext = strrchr(argv[3], '.');
        // Validate extension
        if (out_ext != NULL && strcmp(out_ext, ".txt") == 0)
        {// Copy output file name
            decInfo->d_secret_fname = argv[3];
        }
        else
        {// If extension is not valid
            printf("\nInvalid Output File Format! Expected a .txt file.\n");
            return d_failure;
        }
    }
    else
    {
        // Default output file name
        decInfo->d_secret_fname = "decode.txt";
    }

    return d_success;
}


// Function definition for open files for decoding
Status_d open_files_dec(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_d_src_image = fopen(decInfo->d_src_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_d_src_image == NULL)
    {// Error opening source image file
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->d_src_image_fname);
        return d_failure;
    }
    // Dest file
    decInfo->fptr_d_secret = fopen(decInfo->d_secret_fname, "w");
    // Do Error handling
    if (decInfo->fptr_d_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->d_secret_fname);
        return d_failure;
    }
    // If no failure then return d_success
    return d_success;
}

// Function definition for decode magic string
Status_d decode_magic_string(DecodeInfo *decInfo)
{// Set file pointer to byte 54
    fseek(decInfo->fptr_d_src_image, 54, SEEK_SET);
    int i = strlen(MAGIC_STRING);
    // Allocate memory for magic data
    decInfo->magic_data = malloc(strlen(MAGIC_STRING) + 1);
    // Decode data from image
    decode_data_from_image(strlen(MAGIC_STRING), decInfo->fptr_d_src_image, decInfo);
    // Null terminate the string
    decInfo->magic_data[i] = '\0';
    // Compare magic strings
    if (strcmp(decInfo->magic_data, MAGIC_STRING) == 0)
        return d_success;
    else
        return d_failure;
}

// Function definition for decoding data fom image
Status_d decode_data_from_image(int size, FILE *fptr_d_src_image, DecodeInfo *decInfo)
{
    int i;
    char str[8];
    // Loop to decode each byte
    for (i = 0; i < size; i++)
    {// Read 8 bytes from stego image
        fread(str, 8, sizeof(char), fptr_d_src_image);
        // Decode byte from lsb
        decode_byte_from_lsb(&decInfo->magic_data[i], str);
    }
    return d_success;
}

// Function definition for decode byte from lsb
Status_d decode_byte_from_lsb(char *data, char *image_buffer)
{
    int bit = 7;
    unsigned char ch = 0x00;
    // Loop through each bit
    for (int i = 0; i < 8; i++)
    {// Extract lsb and form the byte
        ch = ((image_buffer[i] & 0x01) << bit--) | ch;
    }
    // Store the decoded byte
    *data = ch;
    return d_success;
}

// Function definition for decode file extn size
Status_d decode_file_extn_size(int size, FILE *fptr_d_src_image)
{
    char str[32];
    int length;
    // Read 32 bytes from stego image
    fread(str, 32, sizeof(char), fptr_d_src_image);
    // Decode size from lsb
    decode_size_from_lsb(str, &length);
    if (length == size)
        return d_success;
    else
        return d_failure;
}

// Function definition decode size from lsb
Status_d decode_size_from_lsb(char *buffer, int *size)
{
    int j = 31;
    int num = 0x00;
    // Loop through each bit
    for (int i = 0; i < 32; i++)
    {// Extract lsb and form the size
        num = ((buffer[i] & 0x01) << j--) | num;
    }
    // Store the decoded size
    *size = num;
}

// Function definition for decode secret file extn
Status_d decode_secret_file_extn(char *file_ext, DecodeInfo *decInfo)
{
    file_ext = ".txt";
    int i = strlen(file_ext);
    // Allocate memory for extension
    decInfo->d_extn_secret_file = malloc(i + 1);
    // Decode extension data from image
    decode_extension_data_from_image(strlen(file_ext), decInfo->fptr_d_src_image, decInfo);
    // Null terminate the string
    decInfo->d_extn_secret_file[i] = '\0';
    // Compare file extensions
    if (strcmp(decInfo->d_extn_secret_file, file_ext) == 0)
        return d_success;
    else
        return d_failure;
}

// Function definition decode extension data from image
Status_d decode_extension_data_from_image(int size, FILE *fptr_d_src_image, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {// Read 8 bytes from stego image
        fread(decInfo->d_src_image_fname, 8, 1, fptr_d_src_image);
        // Decode byte from lsb
        decode_byte_from_lsb(&decInfo->d_extn_secret_file[i], decInfo->d_src_image_fname);
    }
    return d_success;
}

// Function definition for decode secret file size
Status_d decode_secret_file_size(int file_size, DecodeInfo *decInfo)
{
    char str[32];
    // Read 32 bytes from stego image
    fread(str, 32, sizeof(char), decInfo->fptr_d_src_image);
    // Decode size from lsb
    decode_size_from_lsb(str, &file_size);
    // Store the decoded secret file size
    decInfo->size_secret_file = file_size;
    return d_success;
}

// Function definition for decode secret file data
Status_d decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    // Loop to decode each byte of secret file data
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        // Read 8 bytes from stego image
        fread(decInfo->d_src_image_fname, 8, sizeof(char), decInfo->fptr_d_src_image);
        // Decode byte from lsb
        decode_byte_from_lsb(&ch, decInfo->d_src_image_fname);
        // Write decoded byte to secret file
        fputc(ch, decInfo->fptr_d_secret);
    }
    return d_success;
}

// Function definition for do decoding
Status_d do_decoding(DecodeInfo *decInfo)
{// Open files
    if (open_files_dec(decInfo) == d_success)
    {
        printf("\nFiles Opened Successfully\n");
        // Decode magic string
        if (decode_magic_string(decInfo) == d_success)
        {
            printf("\nMagic String Decoded Successfully\n");
            // Decode file extension size
            if (decode_file_extn_size(strlen(".txt"), decInfo->fptr_d_src_image) == d_success)
            {
                printf("\nDecoded File Extension Size Succesfully\n");
                // Decode secret file extension
                if (decode_secret_file_extn(decInfo->d_extn_secret_file, decInfo) == d_success)
                {
                    printf("\nDecoded Secret File Extension Succesfully\n");
                    // Decode secret file size
                    if (decode_secret_file_size(decInfo->size_secret_file, decInfo) == d_success)
                    {
                        printf("\nDecoded Secret File Size Successfully\n");
                        // Decode secret file data
                        if (decode_secret_file_data(decInfo) == d_success)
                        {
                            printf("\nDecoded Secret File Data Successfully\n");
                        }
                        else
                        {//error to decode secret file data
                            printf("\nError: Decoding of Secret File Data is a Failure\n");
                        }
                    }
                    else
                    {//error to decode secret file size
                        printf("\nError: Decode of Secret File Size is a Failure\n");
                        return d_failure;
                    }
                }
                else
                {// error to decode secret file extension
                    printf("\nError: Decode of Secret File Extension is a Failure\n");
                    return d_failure;
                }
            }
            else
            {// error to decode file extension size
                printf("\nError: Decode of File Extension Size is a Failure\n");
                return d_failure;
            }
        }
        else
        {// 
            printf("\nError: Decoding of Magic String is a Failure\n");
            return d_failure;
        }
    }
    else
    {// error to open files
        printf("\nError: Open Files is a Failure\n");
        return d_failure;
    }
    return d_success;
}
