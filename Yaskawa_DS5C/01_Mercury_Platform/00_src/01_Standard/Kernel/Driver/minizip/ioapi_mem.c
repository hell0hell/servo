/* ioapi_mem.c -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API

   This version of ioapi is designed to access memory rather than files.
   We do use a region of memory to put data in to and take it out of. We do
   not have auto-extending buffers and do not inform anyone else that the
   data has been written. It is really intended for accessing a zip archive
   embedded in an application such that I can write an installer with no
   external files. Creation of archives has not been attempted, although
   parts of the framework are present.

   Based on Unzip ioapi.c version 0.22, May 19th, 2003

   Copyright (C) 1998-2003 Gilles Vollant
             (C) 2003 Justin Fletcher

   This file is under the same license as the Unzip tool it is distributed
   with.
*/


#include "ioapi.h"
#include <string.h>

#ifndef _MALLOC_USE_	/*<CUSTOMIZE>*/
#include "unzMemMng.h"
#endif


static voidpf ZCALLBACK fopen_mem_func OF((voidpf opaque,const char* filename,int mode));
static uLong ZCALLBACK fread_mem_func OF((voidpf opaque,voidpf stream,void* buf,uLong size));
static uLong ZCALLBACK fwrite_mem_func OF((voidpf opaque, voidpf stream,const void* buf, uLong size));
static long ZCALLBACK ftell_mem_func OF((voidpf opaque, voidpf stream));
static long ZCALLBACK fseek_mem_func OF((voidpf opaque, voidpf stream, uLong offset, int origin));
static int ZCALLBACK fclose_mem_func OF((voidpf opaque, voidpf stream));
static int ZCALLBACK ferror_mem_func OF((voidpf opaque, voidpf stream));


typedef struct ourmemory_s {
  char *base; /* Base of the region of memory we're using */
  uLong size; /* Size of the region of memory we're using */
  uLong limit; /* Furthest we've written */
  uLong cur_offset; /* Current offset in the area */
} ourmemory_t;

//static ourmemory_t static_mem_io;


static voidpf ZCALLBACK fopen_mem_func (voidpf opaque, const char* filename, int mode)
{
	fileneame_io_t *file_io = (fileneame_io_t *)filename;
//	ourmemory_t *mem = &static_mem_io;
#ifdef _MALLOC_USE_		/*<CUSTOMIZE>*/
	ourmemory_t *mem = malloc(sizeof(*mem));
#else
	ourmemory_t *mem;

	unzInitAlloc();								/* ÉÅÉÇÉäÉvÅ[ÉãÇèâä˙âª */
	mem = unzMalloc(sizeof(*mem));
#endif
    if (mem==NULL)
      return NULL; /* Can't allocate space, so failed */

    /* Filenames are specified in the form :
     *    <hex base of zip file>+<hex size of zip file>
     * This may not work where memory addresses are longer than the
     * size of an int and therefore may need addressing for 64bit
     * architectures

	 if (sscanf(filename,"%x+%x",&mem->base,&mem->size)!=2)
      return NULL;
     */

//	if(mem->base != 0)
//		return NULL;
	mem->base = file_io->base;
	mem->size = file_io->size;

    if (mode & ZLIB_FILEFUNC_MODE_CREATE)
      mem->limit=0; /* When writing we start with 0 bytes written */
    else
      mem->limit=mem->size;

    mem->cur_offset = 0;

    return mem;
}

static voidpf ZCALLBACK fopen64_mem_func (voidpf opaque, const void* filename, int mode)
{
	return fopen_mem_func(opaque, filename, mode);
}

static uLong ZCALLBACK fread_mem_func (voidpf opaque, voidpf stream, void* buf, uLong size)
{
    ourmemory_t *mem = (ourmemory_t *)stream;

    if (size > mem->size - mem->cur_offset)
      size = mem->size - mem->cur_offset;

    memcpy(buf, mem->base + mem->cur_offset, size);
    mem->cur_offset+=size;

    return size;
}


static uLong ZCALLBACK fwrite_mem_func (voidpf opaque, voidpf stream, const void* buf, uLong size)
{
    ourmemory_t *mem = (ourmemory_t *)stream;

    if (size > mem->size - mem->cur_offset)
      size = mem->size - mem->cur_offset;

    memcpy(mem->base + mem->cur_offset, buf, size);
    mem->cur_offset+=size;
    if (mem->cur_offset > mem->limit)
      mem->limit = mem->cur_offset;

    return size;
}

static long ZCALLBACK ftell_mem_func (voidpf opaque, voidpf stream)
{
    ourmemory_t *mem = (ourmemory_t *)stream;

    return mem->cur_offset;
}
static ZPOS64_T ZCALLBACK ftell64_mem_func (voidpf opaque, voidpf stream)
{
    ourmemory_t *mem = (ourmemory_t *)stream;

    return (ZPOS64_T)mem->cur_offset;	
}

static long ZCALLBACK fseek_mem_func (voidpf opaque, voidpf stream, uLong offset, int origin)
{
    ourmemory_t *mem = (ourmemory_t *)stream;
    uLong new_pos;
    switch (origin)
    {
    case ZLIB_FILEFUNC_SEEK_CUR :
        new_pos = mem->cur_offset + offset;
        break;
    case ZLIB_FILEFUNC_SEEK_END :
        new_pos = mem->limit + offset;
        break;
    case ZLIB_FILEFUNC_SEEK_SET :
        new_pos = offset;
        break;
    default: return -1;
    }

    if (new_pos > mem->size)
      return 1; /* Failed to seek that far */

    if (new_pos > mem->limit)
      memset(mem->base + mem->limit, 0, new_pos - mem->limit);

    mem->cur_offset = new_pos;
    return 0;
}

static long ZCALLBACK fseek64_mem_func (voidpf  opaque, voidpf stream, ZPOS64_T offset, int origin)
{
	return fseek_mem_func(opaque, stream, (uLong)offset, origin);
}


static int ZCALLBACK fclose_mem_func (voidpf opaque, voidpf stream)
{
    ourmemory_t *mem = (ourmemory_t *)stream;

    /* Note that once we've written to the buffer we don't tell anyone
       about it here. Probably the opaque handle could be used to inform
       some other component of how much data was written.

       This, and other aspects of writing through this interface, has
       not been tested.
     */

//	memset(mem, 0, sizeof(ourmemory_t));
#ifdef _MALLOC_USE_		/*<CUSTOMIZE>*/
    free (mem);
#else
    unzFree (mem);
#endif
    return 0;
}

static int ZCALLBACK ferror_mem_func (voidpf opaque, voidpf stream)
{
    /* We never return errors */
    return 0;
}

void fill_fopen_filefunc (zlib_filefunc_def* pzlib_filefunc_def)
{
    pzlib_filefunc_def->zopen_file = fopen_mem_func;
    pzlib_filefunc_def->zread_file = fread_mem_func;
    pzlib_filefunc_def->zwrite_file = fwrite_mem_func;
    pzlib_filefunc_def->ztell_file = ftell_mem_func;
    pzlib_filefunc_def->zseek_file = fseek_mem_func;
    pzlib_filefunc_def->zclose_file = fclose_mem_func;
    pzlib_filefunc_def->zerror_file = ferror_mem_func;
    pzlib_filefunc_def->opaque = NULL;
}

void fill_fopen64_filefunc (zlib_filefunc64_def*  pzlib_filefunc_def)
{
    pzlib_filefunc_def->zopen64_file = fopen64_mem_func;
    pzlib_filefunc_def->zread_file = fread_mem_func;
    pzlib_filefunc_def->zwrite_file = fwrite_mem_func;
    pzlib_filefunc_def->ztell64_file = ftell64_mem_func;
    pzlib_filefunc_def->zseek64_file = fseek64_mem_func;
    pzlib_filefunc_def->zclose_file = fclose_mem_func;
    pzlib_filefunc_def->zerror_file = ferror_mem_func;
    pzlib_filefunc_def->opaque = NULL;
}


voidpf call_zopen64 (const zlib_filefunc64_32_def* pfilefunc,const void*filename,int mode)
{
    if (pfilefunc->zfile_func64.zopen64_file != NULL)
        return (*(pfilefunc->zfile_func64.zopen64_file)) (pfilefunc->zfile_func64.opaque,filename,mode);
    else
    {
        return (*(pfilefunc->zopen32_file))(pfilefunc->zfile_func64.opaque,(const char*)filename,mode);
    }
}

long call_zseek64 (const zlib_filefunc64_32_def* pfilefunc,voidpf filestream, ZPOS64_T offset, int origin)
{
    if (pfilefunc->zfile_func64.zseek64_file != NULL)
        return (*(pfilefunc->zfile_func64.zseek64_file)) (pfilefunc->zfile_func64.opaque,filestream,offset,origin);
    else
    {
        uLong offsetTruncated = (uLong)offset;
        if (offsetTruncated != offset)
            return -1;
        else
            return (*(pfilefunc->zseek32_file))(pfilefunc->zfile_func64.opaque,filestream,offsetTruncated,origin);
    }
}

ZPOS64_T call_ztell64 (const zlib_filefunc64_32_def* pfilefunc,voidpf filestream)
{
    if (pfilefunc->zfile_func64.zseek64_file != NULL)
        return (*(pfilefunc->zfile_func64.ztell64_file)) (pfilefunc->zfile_func64.opaque,filestream);
    else
    {
        uLong tell_uLong = (*(pfilefunc->ztell32_file))(pfilefunc->zfile_func64.opaque,filestream);
        if ((tell_uLong) == ((uLong)-1))
            return (ZPOS64_T)-1;
        else
            return tell_uLong;
    }
}

void fill_zlib_filefunc64_32_def_from_filefunc32(zlib_filefunc64_32_def* p_filefunc64_32,const zlib_filefunc_def* p_filefunc32)
{
    p_filefunc64_32->zfile_func64.zopen64_file = NULL;
    p_filefunc64_32->zopen32_file = p_filefunc32->zopen_file;
    p_filefunc64_32->zfile_func64.zerror_file = p_filefunc32->zerror_file;
    p_filefunc64_32->zfile_func64.zread_file = p_filefunc32->zread_file;
    p_filefunc64_32->zfile_func64.zwrite_file = p_filefunc32->zwrite_file;
    p_filefunc64_32->zfile_func64.ztell64_file = NULL;
    p_filefunc64_32->zfile_func64.zseek64_file = NULL;
    p_filefunc64_32->zfile_func64.zclose_file = p_filefunc32->zclose_file;
    p_filefunc64_32->zfile_func64.zerror_file = p_filefunc32->zerror_file;
    p_filefunc64_32->zfile_func64.opaque = p_filefunc32->opaque;
    p_filefunc64_32->zseek32_file = p_filefunc32->zseek_file;
    p_filefunc64_32->ztell32_file = p_filefunc32->ztell_file;
}


