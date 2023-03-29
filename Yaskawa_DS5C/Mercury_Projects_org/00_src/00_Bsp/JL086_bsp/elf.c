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
/* �O���֐� */
extern void InitCorePeripheral( void );							/* JL086���������C������				*/
extern void KpxInitUsbMsgIf( LONG DriveID, LONG ComAddr, LONG AxisNumber );
extern void KpxInitMsgInterface( void );						/* ���b�Z�[�W�h�e����������				*/
extern void KpxUsbMsgManager( void );
extern void KpxMsgManager( void );
extern void MbusFuncSysDlInitWork( void );
extern void MbusFuncSystemDownLoadCheckSum( void );
/* <<<<< <S1A2> */

/* defines*/
/* RAM��̃A�h���X��� */
#define STACK_TOP_ADDRESS			0x67FAF500			/* �X�^�b�NTOP�A�h���X				*/
#define ALM_SET_ADDRESS			(STACK_TOP_ADDRESS + 0x20)	/* �N�����A���[���t���O�ݒ�A�h���X				*/

/* EEPROM�֘A */
#define EEP_SYS_PASSWD_ADDRESS		0x0078U			/* EEPROM�p�X���[�h�������݃A�h���X		<S185>		*/
#define EEP_SYS_PASSWD_LENGTH		0x0008U			/* �p�X���[�h��(�o�C�g)							*/

/* <S1A2> �{�[�hID�֘A��HwDeviceIf.c,h�ɂđΉ� */
/* �{�[�hID�֘A */
//#define BOARD_ID_GPIO_ADDRESS		0xEFFF8500		/* �{�[�hID�擾�pGPIO�A�h���X					*/
//#define BOARD_ID_MASK				0x0000000E		/* �{�[�hID���}�X�N�p�^�[��						*/
//#define BOARD_AP					0x00000000		/* �A�i���O�p���X								*/
//#define BOARD_M2					0x00000002		/* MII											*/
//#define BOARD_M3					0x00000004		/* MIII											*/

/* �T�[�{ID */ /*<S1A2>*/
#define SERVO_ID_AP					0x70
#define SERVO_ID_M2					0x71
#define SERVO_ID_M3					0x72
#define SERVO_ID_W_M3				0x75

/* �����^�C���ʒm�p�A���[����� */
#define ALARM_NONE					0x00000000		/* �A���[���Ȃ�									*/
#define ALARM_A024					0x00000001		/* A.024(����u�[�g�ُ�)						*/
#define ALARM_A025					0x00000002		/* A.025(�����^�C���T���G���[�A�����^�C��IF�G���[)	*/

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

/* 7�Z�OLED�o�͊֘A */
#define LED_SHIFT_AP			16
#define LED_SHIFT_M2			8
#define LED_SHIFT_M3			8
#define LED_SHIFT_W_M3			24			/*<S1A2>*/
//<S1A2>#define REVERSE_BIT_AP			0x00000003
#define REVERSE_BIT_AP			0x000078D3	/* �E�[7�Z�O���p(�ʏ��0xF8D3�����]�p�p�^�[��) */
#define REVERSE_BIT_M2			0x000000D3
#define REVERSE_BIT_M3			0x000000D3
#define REVERSE_BIT_W_M3		0x00FF0F85	/* �E�[7�Z�O���p(�ʏ��0xF85�����]�p�p�^�[��) *//*<S1A2>*/

/* �A���[�������p */
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

/* �N���t�F�[�Y */
typedef enum BOOT_PHASE {
	BOOT_INITIAL,				/* ����u�[�g				*/
	BOOT_RUNTIME,				/* �����^�C���u�[�g			*/
	BOOT_MIRROR,				/* �~���[�u�[�g				*/
	BOOT_ALARM,					/* �A���[���\��				*/
	BOOT_SUCCESS,				/* �����^�C���W�J����I��	*/
} BOOT_PHASE;

#define ELF_MAX_PRG_SEGMENTS    16

/* typedefs */

/* ELF�w�b�_ */
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

#if 0	/* �g�p���Ȃ� */
/* �v���O�����w�b�_ */
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

/* �Z�N�V�����w�b�_ */
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
static UINT8		work_buff[0x400];		/* �ǂݎ̂ėp�o�b�t�@ */

USHORT	ServoId4Boot;						/* �T�[�{�@��ID (�u�[�g���s���ɐݒ肷��)*//*<S1A2>*/

/****************************************************************************
 �u�[�g���A���[���R�[�h�ݒ�i�����^�C���������ւ̒ʒm�p�j
 ****************************************************************************/
static void EFW_SetAlarmCode( UINT32 AlarmCode )
{
	/* �A���[���R�[�h��ݒ肷�� */
	*((UINT32 *)ALM_SET_ADDRESS) = AlarmCode;
	return;
}

/****************************************************************************
 �u�[�g���A���[������(�������[�v�ŏI�����Ȃ�)
 ****************************************************************************/
static void EFW_DisplayAlarmCode( UCHAR AlarmCode )
{
	UCHAR			DisplayData[8];			/* �\���f�[�^			*/
	UINT16			DisplayTime[8];			/* �\������				*/
	ASIC_HW_IF_STR	*AsicHwIf;				/* ASIC�A�N�Z�X�p		*/
	ULONG			OutputData;				/* �o�̓f�[�^			*/
	ULONG			BoardId;				/* ���ID				*/
	ULONG			ShiftNum;				/* LED�f�[�^���V�t�g��	*/
	ULONG			ReverseBitPattern;		/* ���]�r�b�g�p�^�[��	*/
	ULONG			StartTime;				/* �J�n���ԕێ��p	<S190>	*/
	INT16			i;						/* ���[�v�p				*/
//<S1A2>	INT16			j;						/* ���[�v�p				*/

	/*--------*/
	/* ������ */
	/*--------*/
	/* ASIC�A�h���X�ݒ�	*/
	AsicHwIf = (ASIC_HW_IF_STR *)HW_SVASIC_ADDRESS;

	/* ���ID�擾 */
//<S1A2>	BoardId = *((ULONG *)BOARD_ID_GPIO_ADDRESS) & BOARD_ID_MASK;
	BoardId = HALdrv_ReadBoardID();

	/* ���ID���ɁA�V�t�g���Ɣ��]�r�b�g�p�^�[����ݒ肷�� */
	switch( BoardId )
	{
//<S1A2>	case BOARD_AP:	/* �A�i���O */
	case BOARD_ID_S_AP:	/* �A�i���O */
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
	default:		/* ���̑�	*/
		ShiftNum = LED_SHIFT_M2;
		ReverseBitPattern = REVERSE_BIT_M2;
		ServoId4Boot = SERVO_ID_M2;			/*<S1A2>*/
		break;
	}

	/* �\���p�^�[���ݒ�(�R�[�h�A����) */
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

	/* USB�ʐMI/F������ */
	KpxInitUsbMsgIf(0,1,1);			/*<S190>*/
	/* ���b�Z�[�W����I/F������ */
	KpxInitMsgInterface();			/*<S190>*/

	/* �t�@�[���_�E�����[�h���[�N�N���A */	/*<S1A2>*/
	MbusFuncSysDlInitWork();

	/* �������[�v�ŃA���[���\�� */
	for(;;)
	{
		/* 7�Z�OLED�p�^�[���\������ */
		for( i = 0; i < 8; i++ )
		{
			/* �o�̓f�[�^�ݒ� */
			OutputData = ((~DisplayData[i] & 0xFF) << ShiftNum) ^ ReverseBitPattern;
			AsicHwIf->AREG_IOSTAT = OutputData;

			/* ���ԑ҂����E�H�b�`�h�b�O�^�C�}�[���� */
//<S190>			for( j = 0; j < DisplayTime[i]; j++ )
			KlibRstLongTimer( &StartTime );
			while( KlibGetLongTimerMs( StartTime ) < DisplayTime[i] )
			{
				/* USB���荞�݃|�[�����O */
				UsbFuncIntIntu2f();
				UsbFuncIntIntu2fepc();
				/* USB�R���g���[���]������ */
				UsbFuncTaskControl();
				/* USB�ʐM�Ǘ� */
				KpxUsbMsgManager();
				/* ���b�Z�[�W���� */
				KpxMsgManager();
				/* �t�@�[���E�F�A�_�E�����[�h�p�T���`�F�b�N���� */
				MbusFuncSystemDownLoadCheckSum();

				/* �E�H�b�`�h�b�O�^�C�}�[�X�V */
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
	/* ELF�w�b�_�̓��e�`�F�b�N */
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
	INT32				rc;								/* �߂�l					*/
	INT32				file_ofs;						/* �t�@�C���I�t�Z�b�g		*/
	INT32				len;							/* �W�J�T�C�Y				*/
	INT32				rem;							/* �W�J�c�T�C�Y				*/
	INT32				i;								/* ���[�v�p					*/
	UINT8				*ptr;							/* �o�C�g�A�N�Z�X�p�|�C���^	*/
	UINT8				*end_address;					/* �I���A�h���X				*/
	UINT16				sum;							/* �T���l�v�Z�p���[�N		*/
	ELF_HEADER_T		elf_h;							/* ELF�w�b�_				*/
static ELF_SCT_HEADER_T sct_h[ELF_MAX_PRG_SEGMENTS];	/* �Z�N�V�����w�b�_			*/

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

	/* �T���l������ */
	sum = 0;

	/* Load Program Segments */
	for( i = 0; i < elf_h.section_header_num; i++ )
	{
		/* �Z�N�V�����w�b�_�̓��e�ɂ�菈���𕪂���*/
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
					/* �C���[�W��RAM�֒��ړW�J */
					rc = unzReadCurrentFile(uf, (void *)sct_h[i].addr, len);
					if(rc != len)
					{
						return EFW_ERR_PRG_SEGMENT;
					}
					file_ofs += len;

					/* �W�J���ꂽ�f�[�^�ŃT���l�X�V */
					end_address =(UINT8 *)sct_h[i].addr + len;
					for( ptr = (UINT8 *)sct_h[i].addr; ptr < end_address; ptr++ )
					{
						sum += (UINT16)*ptr;
					}
				}
			}
			break;

#if 1	/* bss �̏������͍s��Ȃ� */
		/* �����[�N�̈擙�ƃI�[�o�[���b�v���邱�Ƃ�����A�j�󂳂��̂ŗL���ɂ��Ȃ�����*/
		case SHT_NOBITS:    /* for bss */
			if( (sct_h[i].flags == (SHF_WRITE | SHF_ALLOC) ) && (sct_h[i].addr != 0) )
			{   /* need initialize memory */
				if( sct_h[i].addr != STACK_TOP_ADDRESS )	/* �X�^�b�N */
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
			/* �����Ȃ� */
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

	/* �`�F�b�N�T���l�v�Z */
	*check_sum = -sum;

	return EFW_SUCCESS;
}

/****************************************************************************
 Load Encrypted ROM image
 ****************************************************************************/
static EFW_ERR EFW_LoadROMImage(void *romImage, const char *passwd)
{
	unz_global_info		gi;						/* ZIP�O���[�o�����		*/
	fileneame_io_t		filename_io = { 0 };	/* ZIP�t�@�C�� I/O���		*/
	unzFile				uf;						/* ZIP�t�@�C���|�C���^		*/
	EFW_ERR				err;					/* �G���[�R�[�h				*/
	FIRM_HEADER_T		*FirmHeader;			/* �t�@�[���w�b�_�ւ̃|�C���^		*/
	UINT32				*ZipLocalHeader;		/* ZIP���[�J���w�b�_�ւ̃|�C���^	*/
	UINT16				check_sum;				/* ���[�h���`�F�b�N�T���l	*/

	/*--------------------------*/
	/* �t�@�[���w�b�_�擾����� */
	/*--------------------------*/
	FirmHeader = (FIRM_HEADER_T *)romImage;

	/* ���炩�Ɉُ�ȃt�@�C���T�C�Y���`�F�b�N */
	if( (FirmHeader->zip_file_size < sizeof(UINT32))
		|| ((FirmHeader->zip_file_size + sizeof(FIRM_HEADER_T)) > FLS_CODE_AREA_SIZE) )
	{
		return EFW_ERR_OPEN_IMAGE;
	}

	/* ZIP���[�J���w�b�_�̃V�O�l�`�����`�F�b�N */
	ZipLocalHeader = (UINT32 *)(((char *)romImage) + sizeof(FIRM_HEADER_T));
	if( *ZipLocalHeader != ZIP_LOCAL_HEADER_SIGNATURE )
	{
		return EFW_ERR_OPEN_IMAGE;
	}

	/*---------------------*/
	/* ZIP�t�@�C���I�[�v�� */
	/*---------------------*/
	/* �Í��f�[�^�擪�A�h���X�ƃf�[�^�T�C�Y��ݒ肵�ăI�[�v�� */
	/* Open a compressed ROM image file */
	filename_io.base = ((char *)romImage) + sizeof(FIRM_HEADER_T);
	filename_io.size = FirmHeader->zip_file_size;
	uf = unzOpen((const char *) &filename_io);
	if(uf == NULL)
	{
		return EFW_ERR_OPEN_IMAGE;
	}

	/* ZIP�t�@�C�����擾 */
	if(UNZ_OK != unzGetGlobalInfo(uf, &gi))
	{
		unzClose(uf);
		return EFW_ERR_OPEN_IMAGE;
	}

	/*------------------------*/
	/* ELF�t�@�C����RAM�ɓW�J */
	/*------------------------*/
	/* ELF�t�@�C�����I�[�v��(�p�X���[�h�w��) */
	if(UNZ_OK != unzOpenCurrentFilePassword(uf, passwd))
	{
		unzClose(uf);
		return EFW_ERR_OPEN_IMAGE;
	}

	/* ELF�t�@�C������͂��āARAM�Ƀf�[�^��W�J */
	/* load program segments */
	err = EFW_LoadProgram(uf, &check_sum);

	/* ELF�t�@�C�����N���[�Y */
	unzCloseCurrentFile(uf);

	/*---------------------*/
	/* ZIP�t�@�C���N���[�Y */
	/*---------------------*/
	unzClose(uf);

	/*----------------*/
	/* �T���l�`�F�b�N */
	/*----------------*/
	if( err == EFW_SUCCESS )
	{
		/* �t�@�[���w�b�_�̃`�F�b�N�T���ƁARAM�W�J���Ɍv�Z�����`�F�b�N�T������v���邩 */
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
	EFW_ERR				err;									/* �G���[�R�[�h		*/
	UINT16              passwd[(EEP_SYS_PASSWD_LENGTH/2) + 1];	/* �p�X���[�h�i�[�p	*/
	BOOT_PHASE			BootPhase;								/* �u�[�g�t�F�[�Y	*/
	INT32				i;										/* ���[�v�p			*/
	EFW_ALARM			AlarmCode;								/* �A���[���R�[�h	*/

	/*--------*/
	/* ������ */
	/*--------*/
	BootPhase = BOOT_INITIAL;
	AlarmCode = EFW_NO_ALARM;
	KlibInitialize();						/* �^�C�}���p���邽�߁A������	*/

	/* �����^�C�������֒ʒm����A���[���́A�Ȃ��ݒ�(�O�̂���) */
	EFW_SetAlarmCode( ALARM_NONE );

	/*------------*/
	/* ����u�[�g */
	/*------------*/
	/* ����u�[�g�p�X���[�h��1�o�C�g�ڊȈՃ`�F�b�N��A����u�[�g�����s���� */
	if( *(UINT8 *)FLS_FACTORY_PASSWD_ADDRESS == 0xFF )
	{
		/* �����^�C���u�[�g�� */
		BootPhase = BOOT_RUNTIME;
	}
	else
	{
		/* ����u�[�g���s */
		for( i = 0; i < (EEP_SYS_PASSWD_LENGTH/2); i++ )
		{
			passwd[i] = *((UINT16 *)FLS_FACTORY_PASSWD_ADDRESS + i);
		}
		passwd[i] = 0;
		err = EFW_LoadROMImage( (void*)FLS_CODE_EFW_ADDRESS, (const char*)passwd );
		
		switch( err )
		{
		case EFW_SUCCESS:					/* ����I��									*/
			/* A.024���Z�b�g���I�� */
			EFW_SetAlarmCode( ALARM_A024 );
			BootPhase = BOOT_SUCCESS;
			break;
		case EFW_ERR_OPEN_IMAGE:			/* ELF�t�@�C�����I�[�v������܂ł̃G���[	*/
		case EFW_ERR_HEADER:				/* �p�X���[�h���قȂ�ꍇ�A���̃G���[�ƂȂ�	*/
			/* �����^�C���u�[�g�� */
			BootPhase = BOOT_RUNTIME;
			break;
		default:							/* ���̑�									*/
			/* FL-3(FLASH�f�[�^�ُ�)�̃A���[���\���� */
			AlarmCode = EFW_ALM_FL3;
			BootPhase = BOOT_ALARM;
			break;
		}
	}

	/*------------------*/
	/* �����^�C���u�[�g */
	/*------------------*/
	/* �����^�C���u�[�g���s */
	if( BootPhase == BOOT_RUNTIME )
	{
		/* EEPROM����p�X���[�h���擾 */
		InitInterrupt( );
		I2CdrvInitCpuI2CBusIF( );			/* @@ I2C�o�X I/F�̏�����		*/
		for( i = 0; i < (EEP_SYS_PASSWD_LENGTH/2); i++ )
		{
			EepI2Cdrv_ReadEepromWord( EEP_SYS_PASSWD_ADDRESS + (i * 2), &passwd[i], 0 );
		}
		passwd[i] = 0;

		/* �����^�C����RAM�ɓW�J */
		err = EFW_LoadROMImage( (void*)FLS_CODE_EFW_ADDRESS, (const char*)passwd );

		switch( err )
		{
		case EFW_SUCCESS:					/* ����I��									*/
			/* �I�� */
			BootPhase = BOOT_SUCCESS;
			break;
		default:							/* ���̑�									*/
			/* �~���[�u�[�g�� */
			BootPhase = BOOT_MIRROR;
			break;
		}
	}

	/*--------------*/
	/* �~���[�u�[�g */
	/*--------------*/
	/* �~���[�u�[�g���s */
	if( BootPhase == BOOT_MIRROR )
	{
		/* �~���[�u�[�g���s(�~���[��RAM�ɓW�J) */
		err = EFW_LoadROMImage( (void*)FLS_MIRROR_EFW_ADDRESS, (const char*)passwd );

		switch( err )
		{
		case EFW_SUCCESS:					/* ����I��									*/
			/* A.025�Z�b�g���I�� */
			EFW_SetAlarmCode( ALARM_A025 );
			BootPhase = BOOT_SUCCESS;
			break;
		case EFW_ERR_OPEN_IMAGE:			/* ELF�t�@�C�����I�[�v������܂ł̃G���[	*/
		case EFW_ERR_HEADER:				/* �p�X���[�h���قȂ�ꍇ�A���̃G���[�ƂȂ� */
			/* FL-5 : FLASH�f�[�^�ُ� */
			AlarmCode = EFW_ALM_FL5;
			BootPhase = BOOT_ALARM;
			break;
		default:							/* ���̑�									*/
			/* FL-4 : FLASH�f�[�^�ُ� */
			AlarmCode = EFW_ALM_FL4;
			BootPhase = BOOT_ALARM;
			break;
		}
	}

	/*--------------*/
	/* �A���[������ */
	/*--------------*/
	if( BootPhase == BOOT_ALARM )
	{
		/* �A���[�������Ŗ������[�v���� */
		EFW_DisplayAlarmCode( AlarmCode );
	}

	/*---------------------------*/
	/* BootPhase == BOOT_SUCCESS */
	/*---------------------------*/
	/* �Ăь��Œʏ폈���ɃW�����v���� */
	return err;
}
