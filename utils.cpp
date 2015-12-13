/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

const char *textFileRead( const char *fn )
{
    /* Note: the `fatalError' thing is a bit of a hack, The proper course of
     * action would be to have people check the return values of textFileRead,
     * but lets avoid cluttering the lab-code even further.
     */

    FILE *fp;
    char *content = NULL;
    int count = 0;

    if( fn != NULL ) 
    {
        fp = fopen( fn, "rt" );
        if( fp != NULL ) 
        {
            fseek( fp, 0, SEEK_END );
            count = ftell( fp );
            fseek( fp, 0, SEEK_SET );

            if( count > 0 ) 
            {
                content = new char[count+1];
                count = fread( content, sizeof(char), count, fp );
                content[count] = '\0';
            }
            else
            {
                printf("Error : file empty");
            }

            fclose( fp );
        }
        else
        {
            printf("Error : Cant read file");
        }
    }
    else
    {
        printf("Error : NULL arg");
    }

    return content;
}
