/*
   miniunz.c
   Version 1.1, February 14h, 2010
   sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications of Unzip for Zip64
         Copyright (C) 2007-2008 Even Rouault

         Modifications for Zip64 support on both zip and unzip
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unzip.h"

#include "Basedef.h"
#include "FileDownload.h"


#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)


#ifdef	__ghs__
#pragma ghs section bss=".fixed_buffer"
#endif

static char UnZippedFile[FPRG_FILE_SIZE];	//4Mbyte

#ifdef	__ghs__
#pragma ghs section bss=default
#endif



static int do_extract_currentfile(unzFile uf, const char* password)
{
    char filename_inzip[256];
    char* filename_withoutpath;
    char* p;
    int err=UNZ_OK;
	int extract_size = 0;

    unz_file_info64 file_info;

	err = unzGetCurrentFileInfo64(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
    if (err!=UNZ_OK)
    {
        return err;
    }

    p = filename_withoutpath = filename_inzip;
    while ((*p) != '\0')
    {
        if (((*p)=='/') || ((*p)=='\\'))
            filename_withoutpath = p+1;
        p++;
    }

    if ((*filename_withoutpath)!='\0')
    {
        err = unzOpenCurrentFilePassword(uf,password);
        if (err!=UNZ_OK)
        {
			extract_size=err;
        }
		else
        {
			do
			{
				err = unzReadCurrentFile(uf, 
					(void*)(&UnZippedFile[extract_size]), sizeof(UnZippedFile));
				if (err<0)
				{
					extract_size=err;
					break;
				}
				extract_size+=err;
			} while (err>0);
		}

        unzCloseCurrentFile(uf);
    }

    return extract_size;
}


static int do_extract(unzFile uf, const char* password)
{
    uLong i;
    unz_global_info64 gi;
    int err;

    err = unzGetGlobalInfo64(uf,&gi);
    if (err == UNZ_OK)
	{
		for (i=0;i<gi.number_entry;i++)
		{
			if (do_extract_currentfile(uf, password) != UNZ_OK)
				break;

			if ((i+1)<gi.number_entry)
			{
				err = unzGoToNextFile(uf);
				if (err!=UNZ_OK)
				{
					break;
				}
			}
		}
	}

	return err;
}


int UnzipMemFile(void *base_file_addr, int file_size, const char *password)
{
	fileneame_io_t filename_io = {0};
    int ret_value=0;

	unzFile uf=NULL;

    if (base_file_addr!=NULL)
    {
		filename_io.base = base_file_addr;
		filename_io.size = file_size;

		uf = unzOpen64((const char *)&filename_io);
    }

    if (uf==NULL)
    {
        return 1;
    }

    ret_value = do_extract(uf, password);

    unzClose(uf);

    return ret_value;
}
