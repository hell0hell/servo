#ifndef	_VC_COMPILE_H /* 再読み込み防止用 */
#define	_VC_COMPILE_H /* 再読み込み防止用 */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		VC_Compile.h : VCによりコンパイルするためのスタブ定義										*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.04.20	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "Basedef.h"
#include <malloc.h>

/****************************************************************************************************/
/*	Typedef Definition																				*/
/****************************************************************************************************/
//#define FAR
typedef void*	Address;
typedef LONG	MemoryRegion;
typedef	LONG	Semaphore;
typedef LONG	Task;
typedef void*	IODevice;
typedef LONG	Connection;
typedef void*	Object;
typedef LONG	Error;
typedef LONG	Value;
typedef	int		ssize_t;
typedef void*	Mailbox;
typedef void*   MessageQueue;
typedef void*   LocalMutex;

#define	Success							TRUE
#define	IllegalLength					TRUE
#define	IllegalBlockType				TRUE
#define	__ghs_VirtualMemoryRegionPool	TRUE

/****************************************************************************************************/
/*	Dummy Structure Definition																		*/
/****************************************************************************************************/
typedef struct	Time {
	LONG	Seconds;
	LONG	Fraction;
} Time;

#define DataBuffer		0x1
#define LastBuffer		0x2
typedef struct Buffer {
	LONG BufferType;
	LONG Length;
	LONG Transferred;
	Address TheAddress;
} Buffer;

/****************************************************************************************************/
/*	Dummy Prototype Definition																		*/
/****************************************************************************************************/
VOID	CommonCreateTask( LONG, VOID*, LONG, CHAR*, Task* );
VOID	RunTask( Task );
#define	SetTaskPriority(Task, Value, BOOL)	
static Task CurrentTask(void){ return 0;}	
Error	RequestResource( Object*, const CHAR*, CHAR* );
VOID	Exit( LONG );
VOID	sleep( LONG );
VOID	usleep( LONG );
Error	MapMemory( BOOL, MemoryRegion, MemoryRegion*, VOID*, VOID* );
Error	GetMemoryRegionAddresses( MemoryRegion, VOID*, VOID* );
Error	CreateSemaphore( LONG, Semaphore* );
Error	WaitForSemaphore( Semaphore );
Error	TryToClearSemaphore( Semaphore, VOID* );
Error	LinkSemaphore( Semaphore, Semaphore* );
Error	ReadIODeviceRegister( IODevice a, LONG b, Value *c );
Error	WriteIODeviceRegister( IODevice a, LONG b, Value c );
static Error	WriteIODeviceStatus( IODevice a, LONG b, void *d, Value c ){return 0;}
Error	ClearIODeviceOverruns( IODevice a, Value *b );
#define	WriteBuffersToIODevice( a, b, c, d)		0
#define	ReadBuffersFromIODevice( a, b, c, d)	0

VOID	ReleaseSemaphore( Semaphore );
BOOL	SynchronousReceive( Connection, VOID* );
VOID	consolestring( CHAR* );
VOID	RegisterResource( Object, CHAR*, CHAR* );
SHORT	MP_ResetHINT( IODevice a );
SHORT	MP_EnableM3SSync( IODevice a );
SHORT	MP_DisableM3SSync( IODevice a );
SHORT	MP_SetMHINTTimer( IODevice a, USHORT b );
#define GetClockTime( HighResClock, t)		(t)->Fraction = 1

static Error   AllocateMessageQueueBuffer(MessageQueue queue, void **buffer){*buffer = malloc(100); return 0;}
static Error   CreateMessageQueue(Value numBuffers, Value bufferSize,Value flags, MessageQueue *queue){return 0;}
static Error   FreeMessageQueueBuffer(MessageQueue queue, void **buffer){free(*buffer); return 0;}
static Error   ReceiveOnMessageQueue(MessageQueue queue, void **buffer){return 0;}
static Error   RegisterMessageQueue(MessageQueue queue, const char *name){return 0;}
static Error   SendOnMessageQueue(MessageQueue queue, const void *buffer){return 0;}
static Error   TimedReceiveOnMessageQueue(MessageQueue queue, Time *interval,void **buffer){return 0;}
static Error   TryReceiveOnMessageQueue(MessageQueue queue, void **buffer){return 0;}
static Error   OpenRegisteredMessageQueue(const char *name, MessageQueue *queue){return 0;}

static Error CloseLocalMutex(LocalMutex mutex){return 0;}
static Error CreateLocalMutex(LocalMutex* mutex){return 0;}
static Error ReleaseLocalMutex(LocalMutex mutex){return 0;}
static Error TimedWaitForLocalMutex(LocalMutex mutex, Time *interval){return 0;}
static Error TryToObtainLocalMutex(LocalMutex mutex){return 0;}
static Error WaitForLocalMutex(LocalMutex mutex){return 0;}


#define IODEV_ETHER_SET_DYNAMIC_IP 0
///////////////////////////////////////////////////////////////////
// socket interface
#ifndef FD_SETSIZE
#define FD_SETSIZE      64
#endif /* FD_SETSIZE */
#define INADDR_ANY              0x00000000
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define FIONREAD		127			/* get # bytes to read */
#define FIONBIO			126			/* set/clear non-blocking i/o */
#define FIOASYNC		125			/* set/clear async i/o */

struct in_addr{
      unsigned int s_addr;
};
typedef struct fd_set {
        unsigned fd_count;               /* how many are SET? */
        unsigned fd_array[FD_SETSIZE];   /* an array of SOCKETs */
} fd_set;
struct sockaddr_in {
        short   sin_family;
        unsigned short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
};
typedef struct dip_s {
  int devno;
  int my_ip;
  int df_gw;
  int mask;
} dip_t;
#define FD_SET(fd, set) do { \
    unsigned __i; \
    for (__i = 0; __i < ((fd_set FAR *)(set))->fd_count; __i++) { \
        if (((fd_set FAR *)(set))->fd_array[__i] == (fd)) { \
            break; \
        } \
    } \
    if (__i == ((fd_set FAR *)(set))->fd_count) { \
        if (((fd_set FAR *)(set))->fd_count < FD_SETSIZE) { \
            ((fd_set FAR *)(set))->fd_array[__i] = (fd); \
            ((fd_set FAR *)(set))->fd_count++; \
        } \
    } \
} while(0)

#define FD_ZERO(set) (((fd_set FAR *)(set))->fd_count=0)

#define FD_ISSET(fd, set) 0

/*
 * Structure used in select() call, taken from the BSD file sys/time.h.
 */
struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};

static int     bind(int s, const struct sockaddr *name, int namelen){return 1;}
static int     close(int fd){return 1;}
static int     ioctl(int fd, int Request, char *Data){return 1;}
static ssize_t recvfrom(int s, void *buf, size_t len, int flags,
		struct sockaddr *from, int *fromlen){return 0;}
static int     select(int s, fd_set *rd, fd_set *wr, fd_set *e, struct timeval *t){return 0;}
static int     socket(int domain, int type, int protocol){return 1;}
static ssize_t sendto(int s, const void *msg, size_t len, int flags,
		const struct sockaddr *to, int tolen){return len;}

int     accept(int s, struct sockaddr *addr, int *addrlen);
int     connect(int s, const struct sockaddr *name, int namelen);
int     getsockopt(int s, int level, int optname, void *optval, int *optlen);

int     listen(int s, int backlog);
ssize_t read(int fd, void *buf, size_t nbytes);
ssize_t recv(int s, void *buf, size_t len, int flags);
ssize_t send(int s, const void *msg, size_t len, int flags);
int     setsockopt(int, int, int, const void *, int);
int     shutdown(int s, int how);
int     socketpair(int d, int type, int protocol, int *sv);

#define htons(hostshort)	hostshort
#define ntohs(hostshort)	hostshort
#define htonl(hostlong)		hostlong
#define ntonl(hostlong)		hostlong




#endif /* _VC_COMPILE_H */
/***************************************** end of file **********************************************/
