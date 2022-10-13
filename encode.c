#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

extern char p_flag;         //The 'extern' declaration of the global variable defined in 'main.c' file to extend it's Visibility in this file.

/* Function Definitions */

/* Check which Operation is to be Performed
 * Input: Command Line Arguments
 * Output: Operation to be Performed
 * Return Value: e_encode, e_decode or e_unsupported on Invalid options provided by User
 */

OperationType check_operation_type (char *argv[])
{
    if (strncmp (argv [1], "-e", 2) == 0)           //If the Option provided in the Command Line Argument is '-e', it is for Encoding Operation.
    {
        return e_encode;
    }
    else if (strncmp (argv [1], "-d", 2) == 0)      //If the Option provided in the Command Line Argument is '-d', it is for Decoding Operation.
    {
        return e_decode;
    }
    else if (strncmp (argv [1], "-pe", 3) == 0)     //If the Option provided in the Command Line Argument is '-pe', it is for Encoding Operation.
    {
        p_flag = 1;                                 //Update the Password flag to '1'.
        return e_encode;
    }
    else if (strncmp (argv [1], "-pd", 3) == 0)     //If the Option provided in the Command Line Argument is '-pd', it is for Decoding Operation.
    {
        p_flag = 1;                                 //Update the Password flag to '1'.
        return e_decode;
    }
    else                                            //If any other Option is passed, it is an Unsupported Operation.
    {
        return e_unsupported;
    }
}

/* Check if the Read Command Line Arguments are Valid
 * Input: Command Line Arguments and Structure Pointer
 * Output: Source Image file, Secret file and Stego Image file Names and Extensions
 * Return Value: e_success or e_failure on Invalid Arguments passed
 */

Status read_and_validate_encode_args (char *argv[], EncodeInfo *encInfo)
{
    if (argv [2] == NULL)       //The raw Image file name shall be passed as the 2nd Command Line Argument as per the format.
    {
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
        return e_failure;
    }
    else
    {
        encInfo->fptr_src_image = fopen (argv [2], "rb");       //Open the file in Binary Read Mode.
        if (encInfo->fptr_src_image != NULL)
        {
            char str[2];
            fread (str, 1, 2, encInfo->fptr_src_image);         //Read the file Signature. For a '.bmp file', it is "BM".
            if ((strncmp (str, "BM", 2)) == 0)                  //Check if the value read in 'str' is matching with the BMP file Signature "BM".
            {
                encInfo->src_image_fname = argv [2];            //Update the Source Image file Name in the Structure.
            }
            else                                                //If the file Signature is not "BM", it is not a '.bmp' file and no further operation can be performed.
            {
                printf ("ERROR: The file Signature is not matching with that of a '.bmp' file.\n");
                printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
                return e_failure;
            }
        }
        else        //If the file does not open, we cannot read and validate the arguments to the Structure. So, return 'e_failure'.
        {
            printf ("ERROR: Unable to Open the %s file.\n", argv [2]);
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
            return e_failure;
        }
        fclose (encInfo->fptr_src_image);                       //Close the file opened previously.
    }

    if (argv [3] == NULL)       //The Secret data file name shall be passed as the 3rd Command Line Argument as per the format.
    {
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
        return e_failure;
    }
    else
    {
        if ((strstr (argv [3], ".")) != NULL)       //The Secret Data file extension shall not be NULL.
        {
            encInfo->secret_fname = argv [3];                                               //Update the Secret Data file Name in the Structure.
            strncpy (encInfo->extn_secret_file, strstr (argv [3], "."), MAX_FILE_SUFFIX);   //Update the Secret Data file Extension in the Structure.
        }
        else        //If the extension of the Secret Data file is 'NULL', we cannot read and validate the arguments to the Structure.
        {
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
            return e_failure;
        }
    }

    if (argv [4] == NULL)       //The resultant Output file name is an Optional Command Line Argument. If it is not passed, a default name is to be given for the Output file.
    {
        printf ("INFO: Output file not mentioned. Creating steged_img.bmp as default.\n");
        encInfo->stego_image_fname =  "steged_img.bmp";
    }
    else
    {
        if (((strstr (argv [4], ".")) != NULL) && (strncmp ((strstr (argv [4], ".")), ".bmp", 4)) == 0)     //The extension of Stego Image file shall be '.bmp'.
        {
            encInfo->stego_image_fname = argv [4];      //Update the Stego Image file Name in the Structure.
        }
        else        //If the extension of the Output file is other than '.bmp', a default name is to be given for the Output file.
        {
            printf ("INFO: Output file extension mentioned is not '.bmp'. Creating steged_img.bmp as default.\n");
            encInfo->stego_image_fname =  "steged_img.bmp";
        }
    }

    //No e-failure returned above, then return e_success.
    return e_success;
}

/* To do the Encoding of the Secret Data file in the given Source Image file
 * Input: Structure Pointer
 * Output: Stego Image file
 * Return Value: e_success or e_failure on Source image size insufficient or Secret Data file being empty
 */

Status do_encoding (EncodeInfo *encInfo)
{
    printf ("INFO: Opening required files.\n");
    if ((open_files (encInfo)) == e_failure)        //If any of the files passed in the Command Line Arguments fail to open, the process of Encoding will terminate.
    {
        printf ("ERROR: Unable to Open the required files.\n");
        return e_failure;
    }
    else
    {
        printf ("INFO: Opened %s.\nINFO: Opened %s.\nINFO: Opened %s.\n", encInfo->src_image_fname, encInfo->secret_fname, encInfo->stego_image_fname);
        printf ("INFO: Done.\n");
        printf("INFO: ##### ENCODING PROCEDURE STARTED #####\n");

        Status ret4 = check_capacity (encInfo);     //The Image size must be large enough to accomodate the complete Secret Data file along with other miscellaneous details.
        if (ret4 == e_failure)                      //If the Image size is insufficient, the process of Encoding will terminate.
        {
            return e_failure;
        }
        else        //If the Image size is sufficient, the process of Encoding will begin.
        {
            printf ("INFO: Copying Image Header.\n");
            Status ret5;
            ret5 = copy_bmp_header (encInfo->fptr_src_image, encInfo->fptr_stego_image);    //The first 54 bytes of '.bmp' file must be copied to the Stego Image before encoding begins.
            if (ret5 == e_failure)                                                          //If the Header is not copied successfully, the process of Encoding will terminate.
            {
                return e_failure;
            }
            else    //If the Header is copied successfully, print a confirmation message.
            {
                printf ("INFO: Done.\n");
            }

            printf ("INFO: Encoding Magic String Signature.\n");
            if (p_flag == 1)                                                        //If the 'p_flag' is '1', we have to encode the user-defined Magic string.
            {
                printf ("INFO: Please enter the Magic String to be encoded: ");
                scanf ("%19s", encInfo->magic_string);                              //Read the Magic string from the User and store it in the Structure.
                encInfo->magic_string_size = strlen (encInfo->magic_string);        //Store the length of the Magic string in the Structure.
            }
            else                                                                                    //Otherwise, we have to encode the pre-defined (MACRO) Magic string.
            {
                encInfo->magic_string_size = strlen (MAGIC_STRING);                                 //Store the length of the Magic string in the Structure.
                strncpy (encInfo->magic_string, MAGIC_STRING, (encInfo->magic_string_size + 1));    //Read the Magic string from the MACRO and store it in the Structure.
            }
            ret5 = encode_magic_string (encInfo->magic_string, encInfo);    //From the 55th byte, the encoding shall begin with Magic string being the first text to be encoded.
            if (ret5 == e_failure)                                          //If the Magic string is not encoded successfully, the process of Encoding will terminate.
            {
                return e_failure;
            }
            else    //If the Magic string is encoded successfully, print a confirmation message.
            {
                printf ("INFO: Done.\n");
            }

            printf ("INFO: Encoding %s file Extension.\n", encInfo->secret_fname);
            ret5 = encode_secret_file_extn (encInfo->extn_secret_file, encInfo);    //After Magic string, the Secret Data file extension shall be encoded.
            if (ret5 == e_failure)                                                  //If the Secret Data file extension is not encoded successfully, the process of Encoding will terminate.
            {
                return e_failure;
            }
            else    //If the Secret Data file extension is encoded successfully, print a confirmation message.
            {
                printf ("INFO: Done.\n");
            }

            printf ("INFO: Encoding %s file Size.\n", encInfo->secret_fname);
            ret5 = encode_secret_file_size (encInfo->size_secret_file, encInfo);    //Next, the Secret Data file size shall be encoded.
            if (ret5 == e_failure)                                                  //If the Secret Data file size is not encoded successfully, the process of Encoding will terminate.
            {
                return e_failure;
            }
            else    //If the Secret Data file size is encoded successfully, print a confirmation message.
            {
                printf ("INFO: Done.\n");
            }

            printf ("INFO: Encoding %s file Data.\n", encInfo->secret_fname);
            ret5 = encode_secret_file_data (encInfo);       //Finally, the Secret file data shall be encoded.
            if (ret5 == e_failure)                          //If the Secret file data is not encoded successfully, the process of Encoding will terminate.
            {
                return e_failure;
            }
            else    //If the Secret file data is encoded successfully, print a confirmation message.
            {
                printf ("INFO: Done.\n");
            }

            printf ("INFO: Copying Left Over Data.\n");
            ret5 = copy_remaining_img_data (encInfo->fptr_src_image, encInfo->fptr_stego_image);    //The rest of the Data from Source Image file shall be copied as it is to Stego Image file.
            if (ret5 == e_failure)                                                                  //If the rest of the Data is not copied successfully, the process of Encoding will terminate.
            {
                return e_failure;
            }
            else    //If the Copying of data is successfully, print a confirmation message.
            {
                printf ("INFO: Done.\n");
            }
        }
    }

    fclose (encInfo->fptr_src_image);       //To close the Source Image file.
    fclose (encInfo->fptr_secret);          //To close the Secret Data file.
    fclose (encInfo->fptr_stego_image);     //To close the Stego Image file.

    //No e-failure returned above, then return e_success.
    return e_success;
}

/* 
 * Get FILE pointers for I/P and O/P files
 * Inputs: Source Image file, Secret file and Stego Image file
 * Output: FILE pointer for the above files
 * Return Value: e_success or e_failure on file errors
 */

Status open_files (EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");         //To open the Source Image file.
    if (encInfo->fptr_src_image == NULL)                                    //Error handling.
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s.\n", encInfo->src_image_fname);
    	return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");               //To open the Secret Data file.
    if (encInfo->fptr_secret == NULL)                                       //Error handling.
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s.\n", encInfo->secret_fname);
    	return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");     //To open the Stego Image file.
    if (encInfo->fptr_stego_image == NULL)                                  //Error handling.
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s.\n", encInfo->stego_image_fname);
    	return e_failure;
    }

    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Check the Capacity of Source Image file
 * Input: Structure Pointer
 * Output: If Source Image file is large enough to encode the Data in it 
 * Return Value: e_success or e_failure on Invalid Arguments passed
 */

Status check_capacity (EncodeInfo *encInfo)
{
    printf ("INFO: Checking for %s size.\n", encInfo->secret_fname);
    encInfo->size_secret_file = get_file_size (encInfo->fptr_secret);   //To calculate and store the Secret Data file size.
    
    if (encInfo->size_secret_file == 0)                                 //If the Secret Data file is empty, the process of Encoding will terminate.
    {
        printf ("ERROR: Secret Data file is empty.\n");
        return e_failure;
    }
    else    //If the Secret Data file is not empty, print a confirmation message.
    {
        printf ("INFO: Done. Not Empty.\n");
    }

    printf ("INFO: Checking for %s capacity to handle %s.\n", encInfo->src_image_fname , encInfo->secret_fname);

    encInfo->image_capacity = get_image_size_for_bmp (&encInfo->bits_per_pixel, encInfo->fptr_src_image);     //To calculate and store the Source Image file size.

/*Total size to be encoded = Header size + ((Password length + Data Type of return value of strlen() (int)) x 8 +
  ((size of the Secret Data file Extension + Data Type of Secret Data file size Extension (int)) x 8) +
  ((size of the Secret Data file + Data Type of Secret Data file size) x 8)
  The multiplication factor of '8' is due to the fact that for each character of Data to be encoded, it requires 8 Bytes of size in the Image file.
 */
    
    uint total_size = 54 + ((strlen (MAGIC_STRING) + sizeof (int) + sizeof (encInfo->extn_secret_file) + sizeof (int) + encInfo->size_secret_file + sizeof (uint)) * 8);

    if (total_size > encInfo->image_capacity)      //If the Source Image file size is not large enough to accommodate the Total Data to be encoded, the process of Encoding will terminate.
    {
        printf ("ERROR: The %s doesn't have the Capacity to encode %s.\n", encInfo->src_image_fname, encInfo->secret_fname);
        return e_failure;
    }
    else    //If the Source Image file size is large enough to accommodate the Total Data to be encoded, print a confirmation message.
    {
        printf ("INFO: Done. Found OK.\n");
        return e_success;
    }
}

/* Get Source Image file size
 * Input: Source Image file pointer
 * Output: Width * Height * Bytes per Pixel
 * Return Value: (width * height * bytes)
 * Description: In BMP Image, Width is stored in offset 18, Height in offset 22 (both are Integer values) and Bits per Pixel in offset 28 (a Short value)
 */

uint get_image_size_for_bmp (uint* bits_per_pixel, FILE *fptr_image)
{
    uint width, height;
    fseek (fptr_image, 18, SEEK_SET);                                       //Seek to 18th byte of the Source Image file.

    fread (&width, sizeof(int), 1, fptr_image);                             //Read the Width (an int).

    fread (&height, sizeof(int), 1, fptr_image);                            //Read the Height (an int).

    fseek (fptr_image, 28, SEEK_SET);                                       //Seek to 28th byte of the Source Image file.
    
    fread (bits_per_pixel, sizeof (short), 1, fptr_image);                  //Read the number of Bits Per Pixel (a short). So, to get the output in Bytes, we divide by 8.

    return (width * height * ((*bits_per_pixel) / 8));                      //Return the Image capacity.
}

/* Get Secret Data file size
 * Input: Secret Data file pointer
 * Output: Secret Data file size
 * Return Value: size
 */

uint get_file_size (FILE *fptr)
{
    char ch;                                //To store each character being read from the Secret Data file.
    uint size = 0;                          //To store the size of the Secret Data file.
    while ((ch = fgetc (fptr)) != EOF)      //The loop shall run till the End of File character is encountered in the Secret Data file.
    {
        size++;
    }

    fseek (fptr, 0L, SEEK_SET);             //To update the offset to point to the beginning of the Secret Data file.
    return size;                            //Return the size of the Secret Data file.
}

/* Copy the Header from the Source '.bmp' file to the Stego '.bmp' file
 * Input: Source Image and Stego Image file pointers
 * Output: Stego Image with the Header part
 * Return Value: e_success or e_failure
 */

Status copy_bmp_header (FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image, 0L, SEEK_SET);                //To update the offset to point to the beginning of the Source Image file.
    char ch [54];                                       //A buffer to store the first 54 characters being read from the Source Image file.
    int count;

    count = fread (ch, sizeof (char), 54, fptr_src_image);      //To read the first 54 characters of the Source Image file.
    if (count < 54)                                             //Error Handling.
    {
        printf ("ERROR: Unable to read the Header from the Source Image file.\n");
        return e_failure;
    }

    count = fwrite (ch, sizeof (char), 54, fptr_dest_image);    //To write the first 54 characters to the Stego Image file.
    if (count < 54)                                             //Error Handling.    
    {
        printf ("ERROR: Unable to write the Header to the Stego Image file.\n");
        return e_failure;
    }

    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Encode the Magic string into the Stego Image file
 * Input: Magic string, Source Image and Stego Image file pointers
 * Output: Stego Image with the Magic string encoded
 * Return Value: e_success or e_failure
 */

Status encode_magic_string (const char *magic_string, EncodeInfo *encInfo)
{
    int len = strlen (magic_string);        //To find the length of the Magic string being encoded.
    Status ret;    
    void* ptr = &len;
    ret = encode_data_to_image ((const char*) ptr, sizeof (int), encInfo->fptr_src_image, encInfo->fptr_stego_image);   //Function call to encode given Data to Image.
    
    if (ret == e_failure)       //If the encoding of Magic string Size is unsuccessful, the process of Encoding will terminate.
    {
        printf ("ERROR: Unsuccessful in Encoding the Magic string Size.\n");
        return e_failure;
    }
    else    //Otherwise, proceed for the encoding of the Magic string.
    {
        ret = encode_data_to_image (magic_string, len, encInfo->fptr_src_image, encInfo->fptr_stego_image);     //Function call to encode given Data to Image.

        if (ret == e_failure)       //If the encoding of Magic string is unsuccessful, the process of Encoding will terminate.
        {
            printf ("ERROR: Unsuccessful in Encoding the Magic string.\n");
            return e_failure;
        }
    }

    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Encode the Secret Data file Extension and Extension size into the Stego Image file
 * Input: Secret file Extension, Source Image and Stego Image file pointers
 * Output: Stego Image with the Secret Data file Extension and Extension size encoded
 * Return Value: e_success or e_failure
 */

Status encode_secret_file_extn (const char *file_extn, EncodeInfo *encInfo)
{
    int size = strlen (file_extn);          //To find the length of the Secret Data file Extension being encoded.
    void* ptr = &size;
    Status ret1 = encode_data_to_image ((const char*) ptr, sizeof (int), encInfo->fptr_src_image, encInfo->fptr_stego_image);       //Function call to encode given Data to Image.
    
    if (ret1 == e_failure)      //If the encoding of Secret Data file Extension is unsuccessful, the process of Encoding will terminate.
    {
        printf ("ERROR: Unsuccessful in Encoding the Secret Data file extension Size.\n");
        return e_failure;
    }
    else    //Otherwise, proceed for the encoding of the Secret Data file Extension.
    {
        Status ret2 = encode_data_to_image (file_extn, size, encInfo->fptr_src_image, encInfo->fptr_stego_image);       //Function call to encode given Data to Image.
        
        if (ret2 == e_failure)      //If the encoding of Secret Data file Extension is unsuccessful, the process of Encoding will terminate.
        {
            printf ("ERROR: Unsuccessful in Encoding the Secret Data file Extension.\n");
            return e_failure;
        }
    }

    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Encode the Secret Data file Size into the Stego Image file
 * Input: Secret Data file Size, Source Image and Stego Image file pointers
 * Output: Stego Image with the Secret Data file size encoded
 * Return Value: e_success or e_failure
 */

Status encode_secret_file_size (uint file_size, EncodeInfo *encInfo)
{
    void* file_size_ptr = &file_size;
    Status ret = encode_data_to_image ((const char*) file_size_ptr, sizeof (int), encInfo->fptr_src_image, encInfo->fptr_stego_image);      //Function call to encode given Data to Image.
    
    if (ret == e_failure)           //If the encoding of Secret Data file Size is unsuccessful, the process of Encoding will terminate.
    {
        printf ("ERROR: Unsuccessful in Encoding the Secret Data file Size.\n");
        return e_failure;
    }
    
    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Encode the Secret file Data into the Stego Image file
 * Input: Structure pointer
 * Output: Stego Image with the Secret file Data encoded
 * Return Value: e_success or e_failure
 */

Status encode_secret_file_data (EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret, 0L, SEEK_SET);      //To update the offset to point to the beginning of the Secret Data file.

    char ch [encInfo->size_secret_file];
    int count;
    
    count = fread (ch, 1, encInfo->size_secret_file, encInfo->fptr_secret);     //To read and store the Secret file Data into the form of Character Array.
    if (count < encInfo->size_secret_file)                                      //Error Handling.
    {
        printf ("ERROR: Unsuccessful in reading the Secret Data file.\n");
        return e_failure;
    }
    else
    {
        Status ret = encode_data_to_image (ch, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);      //Function call to encode given Data to Image.

        if (ret == e_failure)       //If the encoding of Secret file Data is unsuccessful, the process of Encoding will terminate.
        {
            printf ("ERROR: Unsuccessful in Encoding the Secret Data file.\n");
            return e_failure;
        }
    }

    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Encode the given Data into the Stego Image file
 * Input: A given Character array, Size of the Data in the Character array, Source Image file and Stego Image file pointers
 * Output: Stego Image with the given Data encoded
 * Return Value: e_success or e_failure
 */

Status encode_data_to_image (const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char ch [8];                                        //For each Byte of 'data', we require 8 Bytes of Image file Data.
    int count;
    for (int i = 0; i < size; i++)                      //The loop shall run till all the bytes of 'data' have been encoded in the Stego Image file.
    {
        count = fread (ch, 1, 8, fptr_src_image);       //The 8 bytes of Image file data are read and stored in 'ch'.
        if (count < 8)                                  //Error Handling.
        {
            printf ("ERROR: Unsuccessful in reading 8 Bytes from the Source Image file.\n");
            return e_failure;
        }
        else
        {
            Status ret = encode_byte_to_lsb (data [i], ch);     //Function call to encode given Data to the LSB of the 8 Bytes of the Image file.
            if (ret == e_success)
            {
                count = fwrite (ch, 1, 8, fptr_stego_image);    //The 8 bytes of the encoded Data is written to the Stego Image file stored in 'ch'.
                if (count < 8)                                  //Error Handling.
                {
                    printf ("ERROR: Unsuccessful in writing 8 Bytes to the Stego Image file.\n");
                    return e_failure;
                }
            }
        }
    }

    //No e_failure returned above, then return e_success.
    return e_success;
}

/* Encode a Byte of Data into the 8 Bytes of Stego Image file
 * Input: A given Character and 8 Bytes of Image file
 * Output: Stego Image with the given Byte encoded
 * Return Value: e_success
 */

Status encode_byte_to_lsb (char data, char *image_buffer)
{
    char temp;
    for (int i = ((sizeof (char) * 8) - 1); i >= 0; i--)    //The loop runs from MSB side to LSB side of the 'data' byte.
    {
        image_buffer [7 - i] &= 0XFE;                       //To clear the LSB of the '(7-i)th' Byte (out of the 8 bytes) of the Image Data received.
        temp = ((data & (1 << i)) >> i);                    //To get the 'ith' Bit from the MSB side of the 'data' and shift that Bit to LSB.
        image_buffer [7 - i] |= temp;                       //To set the 'ith' Bit from the 'data' into the LSB of the '(7-i)th' Byte of Image Data.
    }

    return e_success;
}

/* Copy the remaining Data (after Encoding is completed) from the Source Image file to Stego Image file
 * Input: Source Image file and Stego Image file pointers
 * Output: Stego Image
 * Return Value: e_success or e_failure
 */

Status copy_remaining_img_data (FILE *fptr_src, FILE *fptr_dest)
{
    long start = ftell (fptr_src);          //Store the current position to know where the Encoding ends.

    fseek (fptr_src, 0L, SEEK_END);         //To update the offset to point to the end of the Source Image file.
    long end = ftell (fptr_src);            //Store the ending position of the Source Image file.

    fseek (fptr_src, start, SEEK_SET);      //To update the offset to point to the 'start' position of the Source Image file.

    char ch;
    int count;

    while ((ftell (fptr_src)) != end)       //To copy data byte by byte from the Source Image file to Stego Image file.
    {
        count = fread (&ch, 1, 1, fptr_src);        //Read the data from Source Image file.
        if (count != 1)                             //Error Handling.
        {
            printf ("ERROR: Unable to read the Data from the Source Image file.\n");
            return e_failure;
        }
        else
        {
            count = fwrite (&ch, 1, 1, fptr_dest);      //Write the data to Stego Image file.
            if (count != 1)                             //Error Handling.
            {
                printf ("ERROR: Unable to write the Data to the Stego Image file.\n");
                return e_failure;
            }
        }
    }

    //No e_failure returned above, then return e_success.
    return e_success;
}
