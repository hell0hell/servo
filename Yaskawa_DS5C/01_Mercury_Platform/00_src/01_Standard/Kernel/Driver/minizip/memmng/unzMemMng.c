/* unzip�p���I�������Ǘ��֐� */
#include "Basedef.h"
#include "unzMemMng.h"
#include "rt_Memory.h"


/* unzip�p�������v�[�� */
//#define UNZIP_MEM_POOL_SIZE		0x10000		/* �������v�[���T�C�Y 64KB */
#define UNZIP_MEM_POOL_SIZE		0xA800		/* �������v�[���T�C�Y 42KB */
static INT32	unzip_mem_pool[UNZIP_MEM_POOL_SIZE/sizeof(INT32)];

/* �������v�[�������� */
/* unzMalloc(), unzCalloc(), unzFree() ���g�p����O�Ɏ��s���Ă������� */
void unzInitAlloc( void )
{
	rt_init_mem( (void*)unzip_mem_pool, sizeof(unzip_mem_pool) );
	/* rt_init_mem()�͖߂�l�����邪�A�߂�l�Ȃ��Ƃ��� */

	return;
}

/* malloc()�����݊��֐� */
void *unzMalloc( size_t size )
{
	void	*rc;

	rc = rt_alloc_mem( (void*)unzip_mem_pool, (U32)size );

	return(rc);
}

/* calloc()�����݊��֐� : ����_ �m�ۂ����̈��0�ŏ��������Ȃ� */
void *unzCalloc( size_t nmemb, size_t size )
{
	void	*rc;

	rc = rt_alloc_mem( (void*)unzip_mem_pool, (U32)(nmemb * size) );

	return(rc);
}

/* free()�����݊��֐� */
void unzFree( void *ptr )
{
	rt_free_mem( (void*)unzip_mem_pool, ptr );
	/* rt_free_mem()�͖߂�l�����邪�Afree()�͖߂�l���Ȃ����ߖ߂�l�Ȃ��Ƃ��� */

	return;
}
