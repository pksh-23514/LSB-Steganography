#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

char p_flag;        //To store the Password flag. It is used to check whether a pre-definer or user-defined Magic string shall be encoded in the Image file.

int main(int argc, char* argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    if (argc < 2)       //If the argument count passed in Command Line are improper, print the format in which the arguments shall be passed.
    {
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
        printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
        printf("./lsb_steg: Encoding: ./lsb_steg -pe <.bmp_file> <.text_file> [output file]\n");
        printf("./lsb_steg: Decoding: ./lsb_steg -pd <.bmp_file> [output file]\n");
        return 0;
    }

    OperationType ret1 = check_operation_type (argv);       //To check which Operation needs to be performed based on the Option passed in Command Line.
    if (ret1 == e_encode)                                   //If the return value from the function is to perform Encoding, proceed for Validation and Encoding process.
    {
        Status ret2 = read_and_validate_encode_args (argv, &encInfo);       //To validate all the arguments passed in Command Line and store them in the various Structure members.
        if (ret2 == e_success)                                              //If validation is successful, proceed with the Encoding process.
        {
            Status ret3 = do_encoding (&encInfo);           //To execute the Encoding process of Secret Data file.
            if (ret3 == e_success)                          //If the Encoding process is successfully executed, print a confirmation message.
            {
                printf ("INFO: ##### ENCODING DONE SUCCESSFULLY #####\n");
            }
        }
    }
    else if (ret1 == e_decode)                              //If the return value from the function is to perform Encoding, proceed for Validation and Decoding process.
    {
        Status ret2 = read_and_validate_decode_args (argv, &decInfo);       //To validate all the arguments passed in Command Line and store them in the various Structure members.
        if (ret2 == e_success)                                              //If validation is successful, proceed with the Encoding process.
        {
            Status ret3 = do_decoding (argv, &decInfo);
            if (ret3 == e_success)
            {
                printf ("INFO: ##### DECODING DONE SUCCESSFULLY #####\n");
            }
        }
    }
    else                                                    //If the return value from the function is other than to perform Encoding/Decoding, print an error message.
    {
        printf ("ERROR: Unsupported Operation\n");
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
        printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
        printf("./lsb_steg: Encoding: ./lsb_steg -pe <.bmp_file> <.text_file> [output file]\n");
        printf("./lsb_steg: Decoding: ./lsb_steg -pd <.bmp_file> [output file]\n");
    }

    return 0;
}
