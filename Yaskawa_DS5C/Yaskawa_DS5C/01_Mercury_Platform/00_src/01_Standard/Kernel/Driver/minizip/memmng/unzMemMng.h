/* unzip�p���I�������Ǘ��֐� */

#include <stdlib.h>

/* �������v�[�������� */
/* unzMalloc(), unzCalloc(), unzFree() ���g�p����O�Ɏ��s���Ă������� */
void unzInitAlloc( void );

/* malloc()�����݊��֐� */
void *unzMalloc( size_t size );

/* calloc()�����݊��֐� : ����_ �m�ۂ����̈��0�ŏ��������Ȃ� */
void *unzCalloc( size_t nmemb, size_t size );

/* free()�����݊��֐� */
void unzFree( void *ptr );

