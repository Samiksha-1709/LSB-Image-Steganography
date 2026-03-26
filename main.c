/*
Name = Samiksha Mane 
Batch = ECEP_25023
Project Name = LSB Image Steganography
Description = 
The project hides secret information such as text or data inside a BMP image without changing the visible appearance of the image.
It uses the Least Significant Bit (LSB) technique, where the least significant bits of the image pixels are modified to store the secret data.
The program takes an input .bmp image file (entered by the user) and encodes the secret message into it. 
The encoded image can later be decoded to retrieve the hidden message.
*/


// Include necessary header files
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"

/* Passing arguments through command line arguments */
int main(int argc, char *argv[])
{
    // Function call for check operation type
    // Encoding
    if (check_operation_type(argv) == e_encode)
    {
        printf("\n------------------------------------------------------------------\n");
        printf("\n\tEncoding is Selected\n");
        printf("\n------------------------------------------------------------------\n");
        // Declare structure variable
        EncodeInfo encInfo;
        // Read and validate encode arguments
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            
            printf("Read and validate encode arguments is a success\n");
            printf("\n------------------------------------------------------------------\n");
            printf("\n------------------------Started Encoding--------------------------\n");
            printf("\n------------------------------------------------------------------\n");
            // Function call for encoding
            if (do_encoding(&encInfo) == e_success)
            {
                printf("\n------------------------------------------------------------------\n");
                printf("\n------------------------Completed Encoding------------------------\n");
                printf("\n------------------------------------------------------------------\n");
            }
            else
            {
                //error
                printf("\nError : Failed to Encode\n");
                return e_failure;
            }
        }
        else
        {
            printf("\nError : Read and Validate Encode Arguments is a Failure\n");
            return e_failure;
        }
    }
    // Function call for check operation type
    // Decoding
    else if (check_operation_type(argv) == e_decode)
    {
        printf("\n------------------------------------------------------------------\n");
        printf("\n\tDecoding is Selected\n");
        printf("\n------------------------------------------------------------------\n");
        // Declare structure variables
        DecodeInfo decInfo;
        if (read_and_validate_decode_args(argv, &decInfo) == d_success)
        {
            
            printf("Read and validate decode arguments is a success\n");
            printf("\n------------------------------------------------------------------\n");
            printf("\n-------------------------Started Decoding-------------------------\n");
            printf("\n------------------------------------------------------------------\n");
            // Function call for do decoding
            if (do_decoding(&decInfo) == d_success)
            {
                printf("\n-------------------------Completed Decoding-------------------------\n");
            }
            else
            {
                printf("\nError : Failed to Decode\n");
                return e_failure;
            }
        }
        else
        {
            printf("\nError : Read and Validate Decode Arguments is a Failure\n");
            return e_failure;
        }
    }
    else
    {
        //if user enters invalid options
        printf("Invalid option\nKindly pass for\nEncoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\nDecoding: ./a.out -d stego.bmp decode.txt\n");
    }
    return 0;
}
