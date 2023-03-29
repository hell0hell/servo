/* unzip用動的メモリ管理関数 */

#include <stdlib.h>

/* メモリプール初期化 */
/* unzMalloc(), unzCalloc(), unzFree() を使用する前に実行しておくこと */
void unzInitAlloc( void );

/* malloc()引数互換関数 */
void *unzMalloc( size_t size );

/* calloc()引数互換関数 : 相違点 確保した領域を0で初期化しない */
void *unzCalloc( size_t nmemb, size_t size );

/* free()引数互換関数 */
void unzFree( void *ptr );

