/* unzip用動的メモリ管理関数 */
#include "Basedef.h"
#include "unzMemMng.h"
#include "rt_Memory.h"


/* unzip用メモリプール */
//#define UNZIP_MEM_POOL_SIZE		0x10000		/* メモリプールサイズ 64KB */
#define UNZIP_MEM_POOL_SIZE		0xA800		/* メモリプールサイズ 42KB */
static INT32	unzip_mem_pool[UNZIP_MEM_POOL_SIZE/sizeof(INT32)];

/* メモリプール初期化 */
/* unzMalloc(), unzCalloc(), unzFree() を使用する前に実行しておくこと */
void unzInitAlloc( void )
{
	rt_init_mem( (void*)unzip_mem_pool, sizeof(unzip_mem_pool) );
	/* rt_init_mem()は戻り値があるが、戻り値なしとする */

	return;
}

/* malloc()引数互換関数 */
void *unzMalloc( size_t size )
{
	void	*rc;

	rc = rt_alloc_mem( (void*)unzip_mem_pool, (U32)size );

	return(rc);
}

/* calloc()引数互換関数 : 相違点 確保した領域を0で初期化しない */
void *unzCalloc( size_t nmemb, size_t size )
{
	void	*rc;

	rc = rt_alloc_mem( (void*)unzip_mem_pool, (U32)(nmemb * size) );

	return(rc);
}

/* free()引数互換関数 */
void unzFree( void *ptr )
{
	rt_free_mem( (void*)unzip_mem_pool, ptr );
	/* rt_free_mem()は戻り値があるが、free()は戻り値がないため戻り値なしとする */

	return;
}
