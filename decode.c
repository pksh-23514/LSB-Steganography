#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

extern char p_flag;         //The 'extern' declaration of the global variable defined in 'main.c' file to extend it's Visibility in this file.

/* Function Definitions */

/* Check if the Read Command Line Arguments are Valid
 * Input: Command Line Arguments and Structure Pointer
 * Output: Stego Image file
 * Return Value: e_success or e_failure on Invalid Arguments passed
 */

Status read_and_validate_decode_args (char *argv[], DecodeInfo *decInfo)
{
    if (argv [2] == NULL)           //The encoded Image file name shall be passed as the 2nd Command Line Argument as per the format.
    {
        printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
        return e_failure;
    }
    else
    {
        decInfo->fptr_stego_image = fopen (argv [2], "rb");     //Open the file in Binary Read Mode.
        if (decInfo->fptr_stego_image != NULL)
        {
            char str [2];
            fread (str, 1, 2, decInfo->fptr_stego_image);       //Read the file Signature. For a '.bmp file', it is "BM".
            if ((strncmp (str, "BM", 2)) == 0)                  //Check if the value read in 'str' is matching with the BMP file Signature "BM".
            {
                decInfo->stego_image_fname =  argv [2];         //Update the Source Image file Name in the Structure.
            }
            else                                                //If the file Signature is not "BM", it is not a '.bmp' file and no further operation can be performed.
            {
                printf ("ERROR: The file Signature is not matching with that of a '.bmp' file.\n");
                printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
                return e_failure;
            }
        }
        else        //If the file does not open, we cannot read and validate the arguments to the Structure. So, return 'e_failure'.
        {
            printf ("ERROR: Unable to open the file: %s.\n", argv [2]);
            return e_failure;
        }
        fclose (decInfo->fptr_stego_image);                     //Close the file opened previously.
    }

    //No e-failure returned above, then return e_success.
    return e_success;
}

/* To do the Decoding of the Secret Data file in the given Stego Image file
 * Input: Command Line Arguments and Structure Pointer
 * Output: Secret Data file
 * Return Value: e_success or e_failure
 */

Status do_decoding (char *argv[], DecodeInfo *decInfo)
{
    printf ("INFO: ##### DECODING PROCEDURE STARTED #####\n");
    printf ("INFO: Opening required files.\n");
    if ((open_files_2 (decInfo)) == e_failure)      //If any of the files passed in the Command Line Arguments fail to open, the process of Decoding will terminate.
    {
        printf ("ERROR: Unable to Open the required files.\n");
        return e_failure;
    }
    else
    {
        printf ("INFO: Opened %s.\n", decInfo->stego_image_fname);
        fseek (decInfo->fptr_stego_image, 54L, SEEK_SET);       //To update the offset to point from where encoding begins i.e. 54th position of the Stego Image file.
        Status ret;

        printf ("INFO: Decoding Magic String Signature.\n");
        ret = decode_magic_string (decInfo);        //From the 55th byte, the decoding shall begin with Magic string being the first text to be decoded.
        if (ret == e_failure)                       //If the Magic string is not decoded successfully, the process of Decoding will terminate.
        {
            return e_failure;
        }
        else        //If the Magic string is decoded successfully, check it with the Original Magic string and print a confirmation message.
        {
            if (p_flag == 1)                                            //If the 'p_flag' is '1', we have to compare with the user-defined Magic string.
            {
                char str[20];
                printf ("INFO: Enter the Magic String encoded: ");
                scanf ("%19s", str);                                    //Read the Magic string from the User and store it in the Structure.
                int len = strlen (str);                                 //Store the length of the Magic string.
                if ((strncmp (decInfo->magic_string, str, 20)) == 0)    //Comparing the user-defined Magic string with the decoded Magic string.
                {
                    printf ("INFO: Done.\n");
                }
                else        //If the Magic strings don't match, the process of Decoding will terminate.
                {
                    printf ("ERROR: Magic String entered is Incorrect.\n");
                    return e_failure;
                }
            }
            else                                                        //Otherwise, we have to compare with the pre-defined Magic string.
            {
                if ((strncmp (decInfo->magic_string, MAGIC_STRING, decInfo->magic_string_size)) == 0)   //Comparing with the pre-defined (MACRO) Magic string.
                {
                    printf ("INFO: Done.\n");
                }
                else        //If the Magic strings don't match, the process of Decoding will terminate.
                {
                    printf ("ERROR: Magic String in the Encoded file is Incorrect or Tampered.\n");
                    return e_failure;
                }
            }
        }

        printf ("INFO: Decoding Output file Extension.\n");
        ret = decode_secret_file_extn (decInfo);    //After Magic string, the Secret Data file extension shall be decoded.
        if (ret == e_failure)                       //If the Secret Data file extension is not decoded successfully, the process of Decoding will terminate.
        {
            return e_failure;
        }
        else    //If the Secret Data file extension is decoded successfully, print a confirmation message.
        {
            printf ("INFO: Done.\n");
        }

        if (argv [3] == NULL)       //The resultant Secret Data file name is an Optional Command Line Argument. If it is not passed, a default name is to be given for the same.
        {
            printf ("INFO: Output file not mentioned. Creating decoded as default file Name.\n");
            strncpy (decInfo->secret_fname, "decoded", 8);
        }
        else        //If the Secret Data file Name has been passed in the Command Line, it shall not have any extension as we are decoding the Extension from Stego Image file.
        {
            if ((strstr (argv [3], ".")) == NULL)       //Checking if the name of the Secret Data file name passed in Command Line doesn't have an extension.
            {
                int len = strlen (argv [3]);                                        //To store the Length of the Extension of the Output file.
                strncpy (decInfo->secret_fname,argv [3], (len + 1));                //Update the Secret Data file Name in the Structure.
            }
            else    //If the extension of the Secret Data file is given, we cannot read and validate the arguments to the Structure and the process of Decoding will terminate.
            {
                printf ("ERROR: Output file extension shall not be mentioned.\n");
                printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
                return e_failure;
            }
        }

        strcat (decInfo->secret_fname, decInfo->extn_secret_file);      //Concatenate the Secret Data file Name with the decoded Extension and store in the Structure.

        decInfo->fptr_secret = fopen (decInfo->secret_fname, "w");      //Open the Secret Data file in Write only Mode.

        printf ("INFO: Opened %s.\n", decInfo->secret_fname);
        printf ("INFO: Done. Opened all required files.\n");
        if (decInfo->fptr_secret == NULL)                               //Error Handling.
        {
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file %s.\n", decInfo->secret_fname);
            return e_failure;
        }

        printf ("INFO: Decoding %s file Size.\n", decInfo->secret_fname);
        ret = decode_secret_file_size (decInfo);    //Next, the Secret Data file size shall be decoded.
        if (ret == e_failure)                       //If the Secret Data file size is not decoded successfully, the process of Decoding will terminate.
        {
            return e_failure;
        }
        else    //If the Secret Data file size is decoded successfully, print a confirmation message.
        {
            printf ("INFO: Done.\n");
        }

        printf ("INFO: Decoding %s file Data.\n", decInfo->secret_fname);
        ret = decode_secret_file_data (decInfo);    //Finally, the Secret file data shall be decoded.
        if (ret == e_failure)                       //If the Secret file data is not decoded successfully, the process of Decoding will terminate.
        {
            return e_failure;
        }
        else    //If the Secret file data is decoded successfully, print a confirmation message.
        {
            printf ("INFO: Done.\n");
        }
    }

    //No e-failure returned above, then return e_success.
    return e_success;
}

/* 
 * Get FILE pointer for I/P file
 * Inputs: Structure Pointer
 * Output: FILE pointer for the Stego Image file
 * Return Value: e_success or e_failure on file errors
 */

Status open_files_2 (DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen (decInfo->stego_image_fname, "r");        //To open the Stego Image file.
    if (decInfo->fptr_stego_image == NULL)                                      //Error Handling.
    {
        perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s.\n", decInfo->stego_image_fname);
    	return e_failure;
    }

    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Decode the Magic string from the Stego Image file
 * Input: Structure Pointer
 * Output: The Magic string
 * Return Value: e_success or e_failure
 */

Status decode_magic_string (DecodeInfo *decInfo)
{
    void* ptr = &decInfo->magic_string_size;     //To find the length of the Magic string encoded and store in the Structure.
    Status ret;

    ret = decode_data_from_image ((char*) ptr, sizeof (int), decInfo->fptr_stego_image);        //Function call to decode the Data from Image.

    if (ret == e_failure)       //If the decoding of Magic string Size is unsuccessful, the process of Decoding will terminate.
    {
        printf ("ERROR: Unsuccessful in Decoding the Magic string size.\n");
    }
    else    //Otherwise, proceed for the decoding of the Magic string.
    {        
        ret = decode_data_from_image (decInfo->magic_string, decInfo->magic_string_size, decInfo->fptr_stego_image);     //Function call to decode the Data from Image.

        if (ret == e_failure)       //If the decoding of Magic string is unsuccessful, the process of Decoding will terminate.
        {
            printf ("ERROR: Unsuccessful in Decoding the Magic string.\n");
            return e_failure;
        }
        decInfo->magic_string [decInfo->magic_string_size] = '\0';      //To add the '\0' (NULL terminating) character at the end of the Magic string decoded.
    }

    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Decode the Secret Data file Extension and Extension size from the Stego Image file
 * Input: Structure Pointer
 * Output: Stego The Secret Data file Extension and Extension size
 * Return Value: e_success or e_failure
 */

Status decode_secret_file_extn (DecodeInfo *decInfo)
{
    int size;           //To store the Secret Data file Extension size.
    void* ptr = &size;
    Status ret = decode_data_from_image ((char*) ptr, sizeof (int), decInfo->fptr_stego_image);     //Function call to decode the Data from Image.

    if (ret == e_failure)       //If the decoding of Secret Data file Extension is unsuccessful, the process of Decoding will terminate.
    {
        printf ("ERROR: Unsuccessful in Decoding the Secret Data file Extension Size.\n");
        return e_failure;
    }
    else    //Otherwise, proceed for the decoding of the Secret Data file Extension.
    {
        Status ret1 = decode_data_from_image (decInfo->extn_secret_file, size, decInfo->fptr_stego_image);      //Function call to decode the Data from Image.
        
        if (ret1 == e_failure)      //If the decoding of Secret Data file Extension is unsuccessful, the process of Decoding will terminate.
        {
            printf ("ERROR: Unsuccessful in Decoding the Secret Data file Extension.\n");
            return e_failure;
        }
        decInfo->extn_secret_file [size] = '\0';        //To add the '\0' (NULL terminating) character at the end of the Secret Data file Extension decoded.
    }

    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Decode the Secret Data file Size from the Stego Image file
 * Input: Structure Pointer
 * Output: Stego The Secret Data file Size
 * Return Value: e_success or e_failure
 */

Status decode_secret_file_size (DecodeInfo *decInfo)
{
    void* ptr = &decInfo->size_secret_file;
    Status ret = decode_data_from_image ((char*) ptr, sizeof (int), decInfo->fptr_stego_image);     //Function call to decode the Data from Image.

    if (ret == e_failure)       //If the decoding of Secret Data file Size is unsuccessful, the process of Decoding will terminate.
    {
        printf ("ERROR: Unsuccessful in Decoding the Secret Data file Size.\n");
        return e_failure;
    }
    
    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Decode the Secret file Data from the Stego Image file
 * Input: Structure pointer
 * Output: The Secret file Data
 * Return Value: e_success or e_failure
 */

Status decode_secret_file_data (DecodeInfo *decInfo)
{
    fseek (decInfo->fptr_secret, 0L, SEEK_SET);             //To update the offset to point to the beginning of the Secret Data file.
    Status ret;
    int count;
    for (int i = 0; i < decInfo->size_secret_file; i++)     //The loop shall run till all the Secret file Data has been decoded and written.
    {
        ret = decode_data_from_image (decInfo->secret_data, sizeof (char), decInfo->fptr_stego_image);      //Function call to decode the Data from Image.

        if (ret == e_failure)       //If the decoding of Secret file Data is unsuccessful, the process of Decoding will terminate.
        {
            printf ("ERROR: Unsuccessful in Decoding the Secret Data file Data.\n");
            return e_failure;
        }
        else        //If the decoding of Secret file Data is successful, write the Data in the Output Secret Data file.
        {
            count = fwrite (decInfo->secret_data, 1, 1, decInfo->fptr_secret);      //To write the Secret file Data byte by byte in the Output Secret Data file.
            if (count != 1)                                                         //Error Handling.
            {
                printf ("ERROR: Unsuccessful in writing the Byte to %s file.\n", decInfo->secret_fname);
                return e_failure;
            }
        }
    }
    
    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Decode the particular Data from the Stego Image file
 * Input: A Character array, Size of the Data in the Character array, and Stego Image file pointers
 * Output: The decoded Data
 * Return Value: e_success or e_failure
 */

Status decode_data_from_image (char *data, int size, FILE *fptr_stego_image)
{
    char ch [8];                        //For each Byte of 'data', we require 8 Bytes of Image file Data.
    int count;
    for (int i = 0; i < size; i++)      //The loop shall run till all the bytes of 'data' have been decoded from the Stego Image file.
    {
        count = fread (ch, 1, 8, fptr_stego_image);     //The 8 bytes of Image file data are read and stored in 'ch'.
        if (count < 8)                                  //Error Handling.
        {
            printf ("ERROR: Unsuccessful in reading 8 Bytes from the Stego Image file.\n");
            return e_failure;
        }
        else
        {
            Status ret = decode_byte_from_lsb (&data [i], ch);      //Function call to decode the Data from the LSB of the 8 Bytes of the Image file.
        }
    }

    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Decode a Byte of Data from the 8 Bytes of Stego Image file
 * Input: A Character pointer and 8 Bytes of Image file
 * Output: The decoded Byte
 * Return Value: e_success
 */

Status decode_byte_from_lsb (char *data, char *image_buffer)
{
    char temp;
    *data = 0;                                                      //To clear all the 8 bits of the 'data' for the decoded bit to be set on the 'data'.
    for (int i = ((sizeof (char) * 8) - 1); i >= 0; i--)            //The loop runs from MSB side to LSB side of the 'data'.
    {
        temp = image_buffer [7-i] & 0x1;                            //To get the LSB of the '(7-i)th' Byte (out of the 8 bytes) of the Image Data received.
        *data |= (temp << i);                                       //To set the 'ith' Bit from MSB side of the 'data' with the LSB of the '(7-i)th' Byte of Image Data.
    }

    return e_success;
}
