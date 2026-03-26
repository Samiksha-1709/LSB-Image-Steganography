// Include necessary header files
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function definition for check operation type for encoding/decoding */
OperationType check_operation_type(char *argv[])
{
    // Check for -e options
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
        // Check for -d options
    if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
    //if user enters invalid options
        return e_unsupported;
}

/* Function Definitions */
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return e_failure;
    }
    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
        return e_failure;
    }
    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }
    // No failure return e_success
    return e_success;
}

// Function definition for read and validate encode args
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Validate source image (.bmp)
    // Check if argv[2] is not NULL
    if (argv[2] != NULL)
    {// Check for .bmp extension
        char *src_ext = strrchr(argv[2], '.');
        // Validate extension
        if (src_ext != NULL && strcmp(src_ext, ".bmp") == 0)
        {
            // Copy source image file name
            encInfo->src_image_fname = argv[2];
        }
        // If extension is not valid
        else
        {
            printf("\nInvalid Source Image Format! Expected a .bmp file.\n");
            return e_failure;
        }
    }
    // If argv[2] is NULL
    else
    {
        printf("\nError: Source image argument missing!\n");
        return e_failure;
    }

    // Validate secret file argument
    if (argv[3] != NULL)
    {// Check for .txt extension
        char *secret_ext = strrchr(argv[3], '.');
        // Validate extension
        if (secret_ext != NULL && strcmp(secret_ext, ".txt") == 0)
        {
            // Copy secret file name
            encInfo->secret_fname = argv[3];

            // Copy extension (skip the dot)
            strncpy(encInfo->extn_secret_file, secret_ext + 1, MAX_FILE_SUFFIX);
            encInfo->extn_secret_file[MAX_FILE_SUFFIX - 1] = '\0'; // safety null terminator
            printf("File extension: %s\n", encInfo->extn_secret_file);
        }
        // If extension is not valid
        else
        {
            printf("\nInvalid Secret File Format! Expected a .txt file.\n");
            return e_failure;
        }
    }
    // If argv[3] is NULL
    else
    {
        printf("\nError: Secret file argument missing!\n");
        return e_failure;
    }

    // Validate optional stego image (.bmp)
    if (argv[4] != NULL)
    {// Check for .bmp extension
        char *stego_ext = strrchr(argv[4], '.');
        // Validate extension
        if (stego_ext != NULL && strcmp(stego_ext, ".bmp") == 0)
        {// Copy stego image file name
            encInfo->stego_image_fname = argv[4];
        }
        // If extension is not valid
        else
        {
            printf("\nInvalid Stego Image Format! Expected a .bmp file.\n");
            return e_failure;
        }
    }
    // If argv[4] is NULL
    else
    {
        // Default output file name
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}

// Function definition for check capacity
Status check_capacity(EncodeInfo *encInfo)
{// Get image capacity
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    // Get secret file size
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    // Check if image has enough capacity to hold secret data
    if (encInfo->image_capacity > ((strlen(MAGIC_STRING)*8+32+strlen(encInfo->extn_secret_file)*8+32+encInfo->size_secret_file)*8))
    // If yes, return success
        return e_success;
    else
    // If no, return failure
        return e_failure;
}


// Function definition for getting file size
long get_file_size(FILE *fptr)
{// Set pointer to end of file
    fseek(fptr, 0, SEEK_END);
    // Return current position of pointer
    return ftell(fptr);
}

// Function definition for copying 1st 54 bytes header file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str[54];

    // Setting pointer to point to 0th position
    fseek(fptr_src_image, 0, SEEK_SET);

    // Reading 54 bytes from beautiful.bmp
    fread(str, 54, 1, fptr_src_image);

    // Writing 54 bytes to str
    fwrite(str, 54, 1, fptr_dest_image);
    return e_success;
}

// Function definition for encoding magic string
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{// Call encode data to image function
    encode_data_to_image(magic_string, 2, encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

// Function definition for Encode data to image
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{// Loop to read each byte from data
    for (int i = 0; i < size; i++)
    {// Read 8 bytes from source image
        fread(encInfo->image_data, 8, 1, fptr_src_image);
        // Encode byte to lsb
        encode_byte_to_lsb(data[i], encInfo->image_data);
        // Write encoded data to stego image
        fwrite(encInfo->image_data, 8, 1, fptr_stego_image);
    }
}

// Function definition for encode byte to lsb
Status encode_byte_to_lsb(char data, char *image_buffer)
{// Loop through each bit of the byte
    unsigned int mask = 0x80, i;
    for (i = 0; i < 8; i++)
    {  // Modify lsb of image buffer byte
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (7 - i));
        // Shift mask to right
        mask = mask >> 1;
    }
}

// Function definition for encode secret file extn size
Status encode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{// Read 32 bytes from source image
    char str[32];
    fread(str, 32, 1, fptr_src_image);
    // Encode size to lsb
    encode_size_to_lsb(size, str);
    // Write encoded data to stego image
    fwrite(str, 32, 1, fptr_stego_image);
    return e_success;
}

// Function definition to encode size to lsb
Status encode_size_to_lsb(int size, char *image_buffer)
{// Loop through each bit of the size
    unsigned int mask = 1 << 31, i;
    for (i = 0; i < 32; i++)
    {// Modify lsb of image buffer byte
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((size & mask) >> (31 - i));
        // Shift mask to right
        mask = mask >> 1;
    }
}

// Function definition to encode secret file extn
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{// Call encode data to image function
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

// Function definition for encode secret file size
Status encode_secret_file_size(int size, EncodeInfo *encInfo)
{// Read 32 bytes from source image
    char str[32];
    fread(str, 32, 1, encInfo->fptr_src_image);
    // Encode size to lsb
    encode_size_to_lsb(size, str);
    // Write encoded data to stego image
    fwrite(str, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

// Function definition for encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{// Set pointer to beginning of secret file
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    char str[encInfo->size_secret_file];
    // Read secret file data
    fread(str, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    // Call encode data to image function
    encode_data_to_image(str, strlen(str), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

// Function definition for copying remaining data as it is
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{// Copy remaining bytes from source to destination
    char ch;
    // Loop until end of file
    while ((fread(&ch, 1, 1, fptr_src)) > 0)
    {// Write byte to destination file
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}

// Function definition for do encoding called in main function
Status do_encoding(EncodeInfo *encInfo)
{// Open files
    if (open_files(encInfo) == e_success)
    {
        printf("\nFiles Opened Successfully\n");
        // Check capacity
        if (check_capacity(encInfo) == e_success)
        {
            printf("\nCheck Capacity is Successfull\n");
            // Copy bmp header
            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                printf("\nBMP header Copied Successfully\n");
                // Encode Magic String
                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("\nMagic String Encoded Successfully\n");
                    // Encode Secret File Extn Size
                    strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname, "."));//get extension with dot
                    //encode secret file extn size
                    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                    {
                        printf("\nEncoded Secret File Extn Size Successfully\n");
                        // Encode Secret File Extn
                        if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            printf("\nEncoded Secret File Extn Successfully\n");
                            // Encode Secret File Size
                            if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                printf("\nEncoded Secret File Size Successfully\n");
                                // Encode Secret File Data
                                if (encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("\nEncoded Secret File Data Successfully\n");
                                    // Copy Remaining Image Data
                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("\nCopied Remaining Data Successfully\n");
                                    }
                                    else
                                    {//error to copy remaining data
                                        printf("\nError : Failed to Copy Remaining Data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {//error to encode secret file data
                                    printf("\nError : Failed to Encode Secret File Data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {//error to encode secret file size
                                printf("\nError : Failed to Encode Secret File Size\n");
                                return e_failure;
                            }
                        }
                        else
                        {//error to encode secret file extn
                            printf("\nError : Failed to Encode Secret File Extn\n");
                            return e_failure;
                        }
                    }
                    else
                    {//error to encode secret file extn size
                        printf("\nError : Failed to Encode Secret File Extn Size\n");
                        return e_failure;
                    }
                }
                else
                {//error to encode magic string
                    printf("\nError : Failed to Encode Magic String\n");
                    return e_failure;
                }
            }
            else
            {//error to copy bmp header
                printf("\nError : Failed to Copy BMP Header\n");
                return e_failure;
            }
        }
        else
        {//error to check capacity
            printf("\nError : Check Capacity is a Failure\n");
            return e_failure;
        }
    }
    else
    {//error to open files
        printf("\nError : Failed to Open Files\n");
        return e_failure;
    }
    // No failure
    return e_success;
}
