/****************************************************************************
Description : Encrypted Elf loader for Power PC, JL-086		<S146>
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
                Copyright (c) 2012 All Rights Reserved

Project       : SGDZ-BS63/80, SGDV-MD, SGD7

----------------------------------------------------------------------------
Changes       :
  Name   Date          Description
  ------------------------------------------------------------------------
  I.M.   2012.10.24     created

******************************************************************************/
#include "Basedef.h"
#include "KnlApi.h"
#include "unzip.h"
#include "elf.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpReg.h"
#include "Interrupt.h"
#include "I2cDriver.h"
#include "usb_f.h"	/*<S1A2>*/

/* <S1A2> >>>>> */
/* 外部関数 */
extern void InitCorePeripheral( void );							/* JL086初期化メイン処理				*/
extern void KpxInitUsbMsgIf( LONG DriveID, LONG ComAddr, LONG AxisNumber );
extern void KpxInitMsgInterface( void );						/* メッセージＩＦ初期化処理				*/
extern void KpxUsbMsgManager( void );
extern void KpxMsgManager( void );
extern void MbusFuncSysDlInitWork( void );
extern void MbusFuncSystemDownLoadCheckSum( void );
/* <<<<< <S1A2> */

/* defines*/
/* RAM上のアドレス情報 */
#define STACK_TOP_ADDRESS			0x67FAF500			/* スタックTOPアドレス				*/
#define ALM_SET_ADDRESS			(STACK_TOP_ADDRESS + 0x20)	/* 起動時アラームフラグ設定アドレス				*/

/* EEPROM関連 */
#define EEP_SYS_PASSWD_ADDRESS		0x0078U			/* EEPROMパスワード書き込みアドレス		<S185>		*/
#define EEP_SYS_PASSWD_LENGTH		0x0008U			/* パスワード長(バイト)							*/

/* <S1A2> ボードID関連はHwDeviceIf.c,hにて対応 */
/* ボードID関連 */
//#define BOARD_ID_GPIO_ADDRESS		0xEFFF8500		/* ボードID取得用GPIOアドレス					*/
//#define BOARD_ID_MASK				0x0000000E		/* ボードID部マスクパターン						*/
//#define BOARD_AP					0x00000000		/* アナログパルス								*/
//#define BOARD_M2					0x00000002		/* MII											*/
//#define BOARD_M3					0x00000004		/* MIII											*/

/* サーボID */ /*<S1A2>*/
#define SERVO_ID_AP					0x70
#define SERVO_ID_M2					0x71
#define SERVO_ID_M3					0x72
#define SERVO_ID_W_M3				0x75

/* ランタイム通知用アラーム情報 */
#define ALARM_NONE					0x00000000		/* アラームなし									*/
#define ALARM_A024					0x00000001		/* A.024(初回ブート異常)						*/
#define ALARM_A025					0x00000002		/* A.025(ランタイムサムエラー、ランタイムIFエラー)	*/

/*--------------------------------------------------------------------------------------------------*/
/* Panel Operator LED Codes																			*/
/*																									*/
/* (digit location)        (7seg location)															*/
/*    bit:0																							*/
/*       --					_																		*/
/*    5 |  | 1             |_|																		*/
/*       -- 6              |_|																		*/
/*    4 |  | 2      led[?]: 0																		*/
/*       -- * 7																						*/
/*       3  logic			0:ON / 1:OFF															*/
/*--------------------------------------------------------------------------------------------------*/
#define	LEDCODE_B0		0x01
#define	LEDCODE_B1		0x02
#define	LEDCODE_B2		0x04
#define	LEDCODE_B3		0x08
#define	LEDCODE_B4		0x10
#define	LEDCODE_B5		0x20
#define	LEDCODE_B6		0x40
#define	LEDCODE_B7		0x80
/*--------------------------------------------------------------------------------------------------*/
#define	LEDCODE_ZERO	LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5
#define	LEDCODE_1		LEDCODE_B1|LEDCODE_B2
#define	LEDCODE_2		LEDCODE_B0|LEDCODE_B1|LEDCODE_B3|LEDCODE_B4|LEDCODE_B6
#define	LEDCODE_3		LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B6
#define	LEDCODE_4		LEDCODE_B1|LEDCODE_B2|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_5		LEDCODE_B0|LEDCODE_B2|LEDCODE_B3|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_6		LEDCODE_B0|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_7		LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B5
#define	LEDCODE_8		LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_9		LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_A		LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_B		LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_C		LEDCODE_B0|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5
#define	LEDCODE_D		LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B6
#define	LEDCODE_E		LEDCODE_B0|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_F		LEDCODE_B0|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_L		LEDCODE_B3|LEDCODE_B4|LEDCODE_B5
#define	LEDCODE_MINUS	LEDCODE_B6
#define	LEDCODE_BLANK	0x00U

const UCHAR ledcode[] =
{ /*    0,            1,            2,            3,           4,             5     */
	LEDCODE_ZERO, LEDCODE_1,    LEDCODE_2,    LEDCODE_3,    LEDCODE_4,    LEDCODE_5,
  /*    6,            7             8,            9,            A,            b     */
	LEDCODE_6,    LEDCODE_7,    LEDCODE_8,    LEDCODE_9,    LEDCODE_A,    LEDCODE_B, 
  /*    C,            d,            E,            F                                 */
	LEDCODE_C,    LEDCODE_D,    LEDCODE_E,    LEDCODE_F };

/* 7セグLED出力関連 */
#define LED_SHIFT_AP			16
#define LED_SHIFT_M2			8
#define LED_SHIFT_M3			8
#define LED_SHIFT_W_M3			24			/*<S1A2>*/
//<S1A2>#define REVERSE_BIT_AP			0x00000003
#define REVERSE_BIT_AP			0x000078D3	/* 右端7セグ利用(通常は0xF8D3が反転用パターン) */
#define REVERSE_BIT_M2			0x000000D3
#define REVERSE_BIT_M3			0x000000D3
#define REVERSE_BIT_W_M3		0x00FF0F85	/* 右端7セグ利用(通常は0xF85が反転用パターン) *//*<S1A2>*/

/* アラーム処理用 */
//#define EFW_WDT_WAIT_TIME		2							/*    2ms	*/
#define EFW_WDT_WAIT_TIME		1							/*    1ms	*//*<S190>*/
#define EFW_DISPLAY_TIME		( 350/EFW_WDT_WAIT_TIME)	/*  350ms	*/
#define EFW_BLANK_TIME			( 100/EFW_WDT_WAIT_TIME)	/*  100ms	*/
#define EFW_LONG_BLANK_TIME		(1000/EFW_WDT_WAIT_TIME)	/* 1000ms	*/

typedef enum EFW_ALARM {
	EFW_NO_ALARM = 0,
	EFW_ALM_FL3 = 3,
	EFW_ALM_FL4 = 4,
	EFW_ALM_FL5 = 5,
} EFW_ALARM;

/* 起動フェーズ */
typedef enum BOOT_PHASE {
	BOOT_INITIAL,				/* 初回ブート				*/
	BOOT_RUNTIME,				/* ランタイムブート			*/
	BOOT_MIRROR,				/* ミラーブート				*/
	BOOT_ALARM,					/* アラーム表示				*/
	BOOT_SUCCESS,				/* ランタイム展開正常終了	*/
} BOOT_PHASE;

#define ELF_MAX_PRG_SEGMENTS    16

/* typedefs */

/* ELFヘッダ */
typedef struct ELF_HEADER_T_ {
	struct {
		UCHAR	magic[4];
			#define ELF_MAG0		0x7f
			#define ELF_MAG1		'E'
			#define ELF_MAG2		'L'
			#define ELF_MAG3		'F'
		UCHAR	class;
			#define ELF_CLASS32		1
		UCHAR	format;
			#define ELF_DATA2LSB	1
			#define ELF_DATA2MSB	2
		UCHAR	version;
			#define ELF_VERSION		1
		UCHAR	abi;
		UCHAR	abi_version;
		UCHAR	reserve[7];
	} id;
	INT16	type;
		#define ELF_EXEC_TYPE	2
	INT16	arch;
		#define ELF_PPC			20
		#define ELF_ARM			40
	INT32	version;
	INT32	entry_point;
	INT32	program_header_offset;
	INT32	section_header_offset;
	INT32	flags;
		#define ELF_ARM_EMB		0x05000002
	INT16	elf_header_size;
	INT16	program_header_size;
	INT16	program_header_num;
	INT16	section_header_size;
	INT16	section_header_num;
	INT16	section_name_index;
} ELF_HEADER_T;

#if 0	/* 使用しない */
/* プログラムヘッダ */
typedef struct ELF_PRG_HEADER_T_ {
	INT32	type;
		#define PT_LOAD		1
	UINT32	offset;
	UINT32	virtual_addr;
	UINT32	physical_addr;
	INT32	file_size;
	INT32	memory_size;
	INT32	flags;
	INT32	align;
} ELF_PRG_HEADER_T;
#endif

/* セクションヘッダ */
typedef struct ELF_SCT_HEADER_T_ {
	UINT32	name;
	UINT32	type;
		#define SHT_NULL		0
		#define SHT_PROGBITS	1
		#define SHT_STRTAB		3
		#define SHT_NOTE		7
		#define SHT_NOBITS		8
	UINT32	flags;
		#define SHF_WRITE		0x1
		#define SHF_ALLOC		0x2
		#define SHF_EXECINSTR	0x4
	UINT32	addr;
	UINT32	offset;
	UINT32	size;
	UINT32	link;
	UINT32	info;
	UINT32	addr_align;
	UINT32	ent_size;
} ELF_SCT_HEADER_T;

/* locals */
static UINT8		work_buff[0x400];		/* 読み捨て用バッファ */

USHORT	ServoId4Boot;						/* サーボ機種ID (ブート失敗時に設定する)*//*<S1A2>*/

/****************************************************************************
 ブート時アラームコード設定（ランタイム処理部への通知用）
 ****************************************************************************/
static void EFW_SetAlarmCode( UINT32 AlarmCode )
{
	/* アラームコードを設定する */
	*((UINT32 *)ALM_SET_ADDRESS) = AlarmCode;
	return;
}

/****************************************************************************
 ブート時アラーム処理(無限ループで終了しない)
 ****************************************************************************/
static void EFW_DisplayAlarmCode( UCHAR AlarmCode )
{
	UCHAR			DisplayData[8];			/* 表示データ			*/
	UINT16			DisplayTime[8];			/* 表示時間				*/
	ASIC_HW_IF_STR	*AsicHwIf;				/* ASICアクセス用		*/
	ULONG			OutputData;				/* 出力データ			*/
	ULONG			BoardId;				/* 基板ID				*/
	ULONG			ShiftNum;				/* LEDデータ左シフト数	*/
	ULONG			ReverseBitPattern;		/* 反転ビットパターン	*/
	ULONG			StartTime;				/* 開始時間保持用	<S190>	*/
	INT16			i;						/* ループ用				*/
//<S1A2>	INT16			j;						/* ループ用				*/

	/*--------*/
	/* 初期化 */
	/*--------*/
	/* ASICアドレス設定	*/
	AsicHwIf = (ASIC_HW_IF_STR *)HW_SVASIC_ADDRESS;

	/* 基板ID取得 */
//<S1A2>	BoardId = *((ULONG *)BOARD_ID_GPIO_ADDRESS) & BOARD_ID_MASK;
	BoardId = HALdrv_ReadBoardID();

	/* 基板ID毎に、シフト数と反転ビットパターンを設定する */
	switch( BoardId )
	{
//<S1A2>	case BOARD_AP:	/* アナログ */
	case BOARD_ID_S_AP:	/* アナログ */
		ShiftNum = LED_SHIFT_AP;
		ReverseBitPattern = REVERSE_BIT_AP;
		ServoId4Boot = SERVO_ID_AP;			/*<S1A2>*/
		break;
//<S1A2>	case BOARD_M2:	/* MII		*/
	case BOARD_ID_S_M2:	/* MII		*/
		ShiftNum = LED_SHIFT_M2;
		ReverseBitPattern = REVERSE_BIT_M2;
		ServoId4Boot = SERVO_ID_M2;			/*<S1A2>*/
		break;
//<S1A2>	case BOARD_M3:	/* MIII		*/
	case BOARD_ID_S_M3:	/* MIII		*/
		ShiftNum = LED_SHIFT_M3;
		ReverseBitPattern = REVERSE_BIT_M3;
		ServoId4Boot = SERVO_ID_M3;			/*<S1A2>*/
		break;
/* <S1A2> >>>>> */
	case BOARD_ID_W_M3:	/* MIII		*/
		ShiftNum = LED_SHIFT_W_M3;
		ReverseBitPattern = REVERSE_BIT_W_M3;
		ServoId4Boot = SERVO_ID_W_M3;		/*<S1A2>*/
		break;
/* <<<<< <S1A2> */
	default:		/* その他	*/
		ShiftNum = LED_SHIFT_M2;
		ReverseBitPattern = REVERSE_BIT_M2;
		ServoId4Boot = SERVO_ID_M2;			/*<S1A2>*/
		break;
	}

	/* 表示パターン設定(コード、時間) */
	DisplayData[0] = LEDCODE_F;
	DisplayTime[0] = EFW_DISPLAY_TIME;
	DisplayData[1] = LEDCODE_BLANK;
	DisplayTime[1] = EFW_BLANK_TIME;
	DisplayData[2] = LEDCODE_L;
	DisplayTime[2] = EFW_DISPLAY_TIME;
	DisplayData[3] = LEDCODE_BLANK;
	DisplayTime[3] = EFW_BLANK_TIME;
	DisplayData[4] = LEDCODE_MINUS;
	DisplayTime[4] = EFW_DISPLAY_TIME;
	DisplayData[5] = LEDCODE_BLANK;
	DisplayTime[5] = EFW_BLANK_TIME;
	DisplayData[6] = ledcode[AlarmCode];
	DisplayTime[6] = EFW_DISPLAY_TIME;
	DisplayData[7] = LEDCODE_BLANK;
	DisplayTime[7] = EFW_LONG_BLANK_TIME;

	/* Initialize <S190> */
	InitCorePeripheral();

	/* USB通信I/F初期化 */
	KpxInitUsbMsgIf(0,1,1);			/*<S190>*/
	/* メッセージ処理I/F初期化 */
	KpxInitMsgInterface();			/*<S190>*/

	/* ファームダウンロードワーククリア */	/*<S1A2>*/
	MbusFuncSysDlInitWork();

	/* 無限ループでアラーム表示 */
	for(;;)
	{
		/* 7セグLEDパターン表示処理 */
		for( i = 0; i < 8; i++ )
		{
			/* 出力データ設定 */
			OutputData = ((~DisplayData[i] & 0xFF) << ShiftNum) ^ ReverseBitPattern;
			AsicHwIf->AREG_IOSTAT = OutputData;

			/* 時間待ち＆ウォッチドッグタイマー処理 */
//<S190>			for( j = 0; j < DisplayTime[i]; j++ )
			KlibRstLongTimer( &StartTime );
			while( KlibGetLongTimerMs( StartTime ) < DisplayTime[i] )
			{
				/* USB割り込みポーリング */
				UsbFuncIntIntu2f();
				UsbFuncIntIntu2fepc();
				/* USBコントロール転送処理 */
				UsbFuncTaskControl();
				/* USB通信管理 */
				KpxUsbMsgManager();
				/* メッセージ処理 */
				KpxMsgManager();
				/* ファームウェアダウンロード用サムチェック処理 */
				MbusFuncSystemDownLoadCheckSum();

				/* ウォッチドッグタイマー更新 */
				AsicHwIf->AREG_HINT1 = 0;
				KlibWaitms(1);
//				KlibWaitms( EFW_WDT_WAIT_TIME );
			}
		}
	}
}

/****************************************************************************
 ELF header check
 ****************************************************************************/
static BOOL EFW_IsElfHeaderValid(ELF_HEADER_T *header)
{
	/* ELFヘッダの内容チェック */
	if (header->id.magic[0]!= ELF_MAG0)     return TRUE; /* ELF */
	if (header->id.magic[1]!= ELF_MAG1)     return TRUE; /* ELF */
	if (header->id.magic[2]!= ELF_MAG2)     return TRUE; /* ELF */
	if (header->id.magic[3]!= ELF_MAG3)     return TRUE; /* ELF */
	if (header->id.class   != ELF_CLASS32)  return TRUE; /* ELF32 */
	if (header->id.format  != ELF_DATA2LSB) return TRUE; /* Little endian */
	if (header->id.version != ELF_VERSION)  return TRUE; /* version 1 */
	if (header->type       != ELF_EXEC_TYPE)return TRUE; /* Executable file */
	if (header->version    != ELF_VERSION)  return TRUE; /* version 1 */
	if (header->arch       != ELF_ARM)      return TRUE; /* ARM */
	if (header->flags      != ELF_ARM_EMB)  return TRUE; /* ARM */

    return FALSE;
}

/****************************************************************************
 Load program segment into physical memory
 ****************************************************************************/
static EFW_ERR EFW_LoadProgram(unzFile uf, UINT16 *check_sum)
{
	INT32				rc;								/* 戻り値					*/
	INT32				file_ofs;						/* ファイルオフセット		*/
	INT32				len;							/* 展開サイズ				*/
	INT32				rem;							/* 展開残サイズ				*/
	INT32				i;								/* ループ用					*/
	UINT8				*ptr;							/* バイトアクセス用ポインタ	*/
	UINT8				*end_address;					/* 終了アドレス				*/
	UINT16				sum;							/* サム値計算用ワーク		*/
	ELF_HEADER_T		elf_h;							/* ELFヘッダ				*/
static ELF_SCT_HEADER_T sct_h[ELF_MAX_PRG_SEGMENTS];	/* セクションヘッダ			*/

	/* Get a ELF header */
	rc = unzReadCurrentFile(uf, (void*)&elf_h, sizeof(elf_h));
	if(rc <= 0)
	{
		return EFW_ERR_HEADER;
	}
	file_ofs = rc;

	/* Check ELF header */
	if(FALSE != EFW_IsElfHeaderValid(&elf_h))
	{
		return EFW_ERR_HEADER;
	}

	/* Get Section headers */
	len = elf_h.section_header_size * elf_h.section_header_num;
	rc = unzReadCurrentFile(uf, (void*)sct_h, len);
	if(rc != len)
	{
		return EFW_ERR_PRG_HEADER;
	}
	file_ofs += len;

	/* サム値初期化 */
	sum = 0;

	/* Load Program Segments */
	for( i = 0; i < elf_h.section_header_num; i++ )
	{
		/* セクションヘッダの内容により処理を分ける*/
		switch( sct_h[i].type )
		{
		case SHT_PROGBITS:  /* for code, data */
			if(file_ofs > sct_h[i].offset)
			{/* file read overrun */
				;//return EFW_ERR_PRG_SEGMENT;
			}
			else if(file_ofs < sct_h[i].offset)
			{/* load unnecessary image data into work buffer */
				rem = sct_h[i].offset - file_ofs;
				file_ofs += rem;
				do{
					len = (rem > sizeof(work_buff)) ? sizeof(work_buff) : rem;
					rc = unzReadCurrentFile(uf, (void*)work_buff, len);
					rem -= rc;
				}while((rc > 0) && (rem > 0));
				i--;
			}
			else
			{/* load program segment into physical ram */
				len = sct_h[i].size;
				if(len > 0)
				{
					/* イメージをRAMへ直接展開 */
					rc = unzReadCurrentFile(uf, (void *)sct_h[i].addr, len);
					if(rc != len)
					{
						return EFW_ERR_PRG_SEGMENT;
					}
					file_ofs += len;

					/* 展開されたデータでサム値更新 */
					end_address =(UINT8 *)sct_h[i].addr + len;
					for( ptr = (UINT8 *)sct_h[i].addr; ptr < end_address; ptr++ )
					{
						sum += (UINT16)*ptr;
					}
				}
			}
			break;

#if 1	/* bss の初期化は行わない */
		/* 自ワーク領域等とオーバーラップすることがあり、破壊されるので有効にしないこと*/
		case SHT_NOBITS:    /* for bss */
			if( (sct_h[i].flags == (SHF_WRITE | SHF_ALLOC) ) && (sct_h[i].addr != 0) )
			{   /* need initialize memory */
				if( sct_h[i].addr != STACK_TOP_ADDRESS )	/* スタック */
				{
					end_address = (UINT8 *)sct_h[i].addr + sct_h[i].size;
					for( ptr = (UINT8*)sct_h[i].addr; ptr < end_address; ptr++ )
					{
//						if( ptr >= (UINT8 *)0x6806C000 ) break;
						*ptr = 0;
					}
				}
			}
			break;
#endif

		default:	/* others */
			/* 処理なし */
			break;
		}
	}

	/* Load the rest of file contents */
	do{
		rc = unzReadCurrentFile(uf, (void*)work_buff, sizeof(work_buff));
	}while(rc > 0);

	if(rc < 0)
	{
		return EFW_ERR_OTHER_SECTION;
	}

	/* チェックサム値計算 */
	*check_sum = -sum;

	return EFW_SUCCESS;
}

/****************************************************************************
 Load Encrypted ROM image
 ****************************************************************************/
static EFW_ERR EFW_LoadROMImage(void *romImage, const char *passwd)
{
	unz_global_info		gi;						/* ZIPグローバル情報		*/
	fileneame_io_t		filename_io = { 0 };	/* ZIPファイル I/O情報		*/
	unzFile				uf;						/* ZIPファイルポインタ		*/
	EFW_ERR				err;					/* エラーコード				*/
	FIRM_HEADER_T		*FirmHeader;			/* ファームヘッダへのポインタ		*/
	UINT32				*ZipLocalHeader;		/* ZIPローカルヘッダへのポインタ	*/
	UINT16				check_sum;				/* ロード時チェックサム値	*/

	/*--------------------------*/
	/* ファームヘッダ取得＆解析 */
	/*--------------------------*/
	FirmHeader = (FIRM_HEADER_T *)romImage;

	/* 明らかに異常なファイルサイズをチェック */
	if( (FirmHeader->zip_file_size < sizeof(UINT32))
		|| ((FirmHeader->zip_file_size + sizeof(FIRM_HEADER_T)) > FLS_CODE_AREA_SIZE) )
	{
		return EFW_ERR_OPEN_IMAGE;
	}

	/* ZIPローカルヘッダのシグネチャをチェック */
	ZipLocalHeader = (UINT32 *)(((char *)romImage) + sizeof(FIRM_HEADER_T));
	if( *ZipLocalHeader != ZIP_LOCAL_HEADER_SIGNATURE )
	{
		return EFW_ERR_OPEN_IMAGE;
	}

	/*---------------------*/
	/* ZIPファイルオープン */
	/*---------------------*/
	/* 暗号データ先頭アドレスとデータサイズを設定してオープン */
	/* Open a compressed ROM image file */
	filename_io.base = ((char *)romImage) + sizeof(FIRM_HEADER_T);
	filename_io.size = FirmHeader->zip_file_size;
	uf = unzOpen((const char *) &filename_io);
	if(uf == NULL)
	{
		return EFW_ERR_OPEN_IMAGE;
	}

	/* ZIPファイル情報取得 */
	if(UNZ_OK != unzGetGlobalInfo(uf, &gi))
	{
		unzClose(uf);
		return EFW_ERR_OPEN_IMAGE;
	}

	/*------------------------*/
	/* ELFファイルをRAMに展開 */
	/*------------------------*/
	/* ELFファイルをオープン(パスワード指定) */
	if(UNZ_OK != unzOpenCurrentFilePassword(uf, passwd))
	{
		unzClose(uf);
		return EFW_ERR_OPEN_IMAGE;
	}

	/* ELFファイルを解析して、RAMにデータを展開 */
	/* load program segments */
	err = EFW_LoadProgram(uf, &check_sum);

	/* ELFファイルをクローズ */
	unzCloseCurrentFile(uf);

	/*---------------------*/
	/* ZIPファイルクローズ */
	/*---------------------*/
	unzClose(uf);

	/*----------------*/
	/* サム値チェック */
	/*----------------*/
	if( err == EFW_SUCCESS )
	{
		/* ファームヘッダのチェックサムと、RAM展開時に計算したチェックサムが一致するか */
		if( FirmHeader->check_sum != check_sum )
		{
			err = EFW_ERR_CHECK_SUM;
		}
	}

	return err;
}


/****************************************************************************
 Encrypted ELF image loader
 ****************************************************************************/
EFW_ERR EFW_Loader(void)
{
	EFW_ERR				err;									/* エラーコード		*/
	UINT16              passwd[(EEP_SYS_PASSWD_LENGTH/2) + 1];	/* パスワード格納用	*/
	BOOT_PHASE			BootPhase;								/* ブートフェーズ	*/
	INT32				i;										/* ループ用			*/
	EFW_ALARM			AlarmCode;								/* アラームコード	*/

	/*--------*/
	/* 初期化 */
	/*--------*/
	BootPhase = BOOT_INITIAL;
	AlarmCode = EFW_NO_ALARM;
	KlibInitialize();						/* タイマ利用するため、初期化	*/

	/* ランタイム処理へ通知するアラームは、なし設定(念のため) */
	EFW_SetAlarmCode( ALARM_NONE );

	/*------------*/
	/* 初回ブート */
	/*------------*/
	/* 初回ブートパスワードの1バイト目簡易チェック後、初回ブートを試行する */
	if( *(UINT8 *)FLS_FACTORY_PASSWD_ADDRESS == 0xFF )
	{
		/* ランタイムブートへ */
		BootPhase = BOOT_RUNTIME;
	}
	else
	{
		/* 初回ブート試行 */
		for( i = 0; i < (EEP_SYS_PASSWD_LENGTH/2); i++ )
		{
			passwd[i] = *((UINT16 *)FLS_FACTORY_PASSWD_ADDRESS + i);
		}
		passwd[i] = 0;
		err = EFW_LoadROMImage( (void*)FLS_CODE_EFW_ADDRESS, (const char*)passwd );
		
		switch( err )
		{
		case EFW_SUCCESS:					/* 正常終了									*/
			/* A.024をセットし終了 */
			EFW_SetAlarmCode( ALARM_A024 );
			BootPhase = BOOT_SUCCESS;
			break;
		case EFW_ERR_OPEN_IMAGE:			/* ELFファイルをオープンするまでのエラー	*/
		case EFW_ERR_HEADER:				/* パスワードが異なる場合、このエラーとなる	*/
			/* ランタイムブートへ */
			BootPhase = BOOT_RUNTIME;
			break;
		default:							/* その他									*/
			/* FL-3(FLASHデータ異常)のアラーム表示へ */
			AlarmCode = EFW_ALM_FL3;
			BootPhase = BOOT_ALARM;
			break;
		}
	}

	/*------------------*/
	/* ランタイムブート */
	/*------------------*/
	/* ランタイムブート試行 */
	if( BootPhase == BOOT_RUNTIME )
	{
		/* EEPROMからパスワードを取得 */
		InitInterrupt( );
		I2CdrvInitCpuI2CBusIF( );			/* @@ I2Cバス I/Fの初期化		*/
		for( i = 0; i < (EEP_SYS_PASSWD_LENGTH/2); i++ )
		{
			EepI2Cdrv_ReadEepromWord( EEP_SYS_PASSWD_ADDRESS + (i * 2), &passwd[i], 0 );
		}
		passwd[i] = 0;

		/* ランタイムをRAMに展開 */
		err = EFW_LoadROMImage( (void*)FLS_CODE_EFW_ADDRESS, (const char*)passwd );

		switch( err )
		{
		case EFW_SUCCESS:					/* 正常終了									*/
			/* 終了 */
			BootPhase = BOOT_SUCCESS;
			break;
		default:							/* その他									*/
			/* ミラーブートへ */
			BootPhase = BOOT_MIRROR;
			break;
		}
	}

	/*--------------*/
	/* ミラーブート */
	/*--------------*/
	/* ミラーブート試行 */
	if( BootPhase == BOOT_MIRROR )
	{
		/* ミラーブート試行(ミラーをRAMに展開) */
		err = EFW_LoadROMImage( (void*)FLS_MIRROR_EFW_ADDRESS, (const char*)passwd );

		switch( err )
		{
		case EFW_SUCCESS:					/* 正常終了									*/
			/* A.025セットし終了 */
			EFW_SetAlarmCode( ALARM_A025 );
			BootPhase = BOOT_SUCCESS;
			break;
		case EFW_ERR_OPEN_IMAGE:			/* ELFファイルをオープンするまでのエラー	*/
		case EFW_ERR_HEADER:				/* パスワードが異なる場合、このエラーとなる */
			/* FL-5 : FLASHデータ異常 */
			AlarmCode = EFW_ALM_FL5;
			BootPhase = BOOT_ALARM;
			break;
		default:							/* その他									*/
			/* FL-4 : FLASHデータ異常 */
			AlarmCode = EFW_ALM_FL4;
			BootPhase = BOOT_ALARM;
			break;
		}
	}

	/*--------------*/
	/* アラーム処理 */
	/*--------------*/
	if( BootPhase == BOOT_ALARM )
	{
		/* アラーム処理で無限ループする */
		EFW_DisplayAlarmCode( AlarmCode );
	}

	/*---------------------------*/
	/* BootPhase == BOOT_SUCCESS */
	/*---------------------------*/
	/* 呼び元で通常処理にジャンプする */
	return err;
}
