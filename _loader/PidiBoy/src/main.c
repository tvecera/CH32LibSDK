
#include "../include.h"

// files
#define PATHMAX		38	// max. length of path
#define NAMELEN		9	// max. length of filename without extension and with terminating 0
#define FILEWIDTH	10	// width of 1 filename
#define FILEROWS	(TEXTHEIGHT-1) // rows of files (=7)
#define MAXFILES	200	// max files in one directory

sAppInfoHeader App;

// file entry descriptor (10 bytes)
//#define ATTR_DIR	B4	// Directory
typedef struct { // 10 bytes
	u8	attr;		// attributes ATTR_*
	u8	len;		// file name length
	char	name[8];	// file name (without extension and without terminating zero)
} sFileDesc;

// files
char Path[PATHMAX+1]; // current path (with terminating 0)
int PathLen; // length of path
u32 LastMount; // last mount time
Bool Remount; // remount request
sFileDesc FileDesc[MAXFILES]; // file descriptors
int FileNum; // number of files
int FileCur; // index of current file
int FileTop; // index of first visible file
sFile FileF; // search structure
sFileInfo FileI; // search file info

// temporary buffer
#define TEMPBUF	SECT_SIZE	// size must be multiply of FLASH_PAGEPG_SIZE (= 256 B)
ALIGNED char TempBuf[TEMPBUF];

// seek cursor to last name
char LastName[8];
int LastNameLen = 0; // length of last name, 0 = not used
int LastNameTop = 0; // relative offset to top file
u8 LastNameDir; // ATTR_DIR flag of last name
Bool LoaderReq = False; // request to run loader instead application

// display info text
void DispInfo(const char* text)
{
	// clear screen
	PrintClear();

	// prepare length of into text
	int len = StrLen(text);

	// set text color and coordinated
	PrintRow = TEXTHEIGHT/2;
	PrintPos = (TEXTWIDTH - len)/2;

	// display info text
	PrintText(text);
	DispUpdate();
}

// dispay file list (screen should be cleared after loading files)
void DispFileList()
{
	// display directory
	PrintHome();
	PrintInv = 128;
	PrintSpcRep((TEXTWIDTH - PathLen)/2);
	PrintText(Path);
	PrintSpcRep((TEXTWIDTH - PathLen + 1)/2);

	// no files
	if (FileNum == 0)
	{
		PrintText("\n\nNo files");
		DispUpdate();
		return;
	}

	// display files
	int i, j;
	char ch;
	Bool dir;
	sFileDesc* fd = &FileDesc[FileTop];
	PrintRow = 1;
	for (i = 0; i < FILEROWS; i++)
	{
		PrintPos = 0;

		// set normal color
		PrintInv = 0;

		// invalid file
		if (i + FileTop >= FileNum)
		{
			PrintSpcRep(10);
		}
		else
		{
			// check directory
			dir = ((fd->attr & ATTR_DIR) != 0);

			// set cursor color
			if (FileTop+i == FileCur) PrintInv = 128;

			// directory mark '['
			ch = dir ? '[' : ' ';
			PrintChar(ch);

			// decode entry name
			for (j = 0; j < fd->len; j++) PrintChar(fd->name[j]);

			// directory mark ']'
			ch = dir ? ']' : ' ';
			PrintChar(ch);

			// display spaces
			PrintSpcRep(8 - fd->len);
		}

		// increase file
		PrintRow++;
		fd++;
	}
	DispUpdate();
}

// load files (programs with UF2 extension, directory without extension)
void LoadFileList()
{
	// clear file list
	FileNum = 0;
	FileCur = 0;

	// set current directory
	if (!SetDir(Path))
	{
		PrintClear();
		return;
	}

	// open search 
	if (!FindOpen(&FileF, ""))
	{
		PrintClear();
		return;
	}

	// load files
	int inx, i, len;
	sFileDesc* fd = FileDesc;
	char ch;
	char* name;
	Bool dir;
	for (inx = 0; inx < MAXFILES; inx++)
	{
		// find next file
		if (!FindNext(&FileF, &FileI, ATTR_DIR_MASK, "*.*")) break;

		// check directory
		dir = (FileI.attr & ATTR_DIR) != 0;

		// skip directory "."
		len = FileI.namelen;
		name = FileI.name;
		if (dir && (len == 1) && (name[0] == '.')) continue;

		// get attributes
		fd->attr = FileI.attr & ATTR_MASK;

		// copy directory ".."
		if (dir && (len == 2) && (name[0] == '.') && (name[1] == '.'))
		{
			fd->len = len;
			fd->name[0] = '.';
			fd->name[1] = '.';
			fd++;
			FileNum++;
		}
		else
		{
			// skip hidden entry
			if ((FileI.attr & ATTR_HID) != 0) continue;

			// directory
			if (dir)
			{
				// copy directory name (without extension)
				for (i = 0; (i < len) && (i < 8); i++)
				{
					ch = name[i];
					if (ch == '.') break;
					fd->name[i] = ch;
				}

				// directory is valid only if has no extension
				if (i == len)
				{
					fd->len = len;
					fd++;
					FileNum++;
				}
			}

			// file
			else
			{
				// check extension "BIN"
				if ((len > 4) && (name[len-4] == '.') && (name[len-3] == 'B') &&
					 (name[len-2] == 'I') && (name[len-1] == 'N'))
				{
					// copy file name (without extension)
					fd->len = len-4;
					memcpy(fd->name, name, len-4);
					fd++;
					FileNum++;
				}
			}
		}
	}

	// sort files (using bubble sort)
	fd = FileDesc;
	for (inx = 0; inx < FileNum-1;)
	{
		Bool ok = True;

		// directory '..' must be at first place
		if ((fd[1].len == 2) && (fd[1].name[0] == '.') && (fd[1].name[1] == '.')) ok = False;

		// directory must be before the files
		if (((fd[0].attr & ATTR_DIR) == 0) && ((fd[1].attr & ATTR_DIR) != 0)) ok = False;

		// entry of the same group
		if (((fd[0].attr ^ fd[1].attr) & ATTR_DIR) == 0)
		{
			// compare names
			len = fd[0].len;
			if (fd[1].len < fd[0].len) len = fd[1].len;
			for (i = 0; i < len; i++)
			{
				if (fd[0].name[i] != fd[1].name[i]) break;
			}

			if (i < len) // names are different
			{
				if (fd[0].name[i] > fd[1].name[i]) ok = False;
			}
			else // names are equal, check name lengths
			{
				if (fd[1].len < fd[0].len) ok = False;
			}
		}

		// exchange files
		if (!ok)
		{
			ch = fd[0].attr;
			fd[0].attr = fd[1].attr;
			fd[1].attr = ch;

			ch = fd[0].len;
			fd[0].len = fd[1].len;
			fd[1].len = ch;

			for (i = 0; i < 8; i++)
			{
				ch = fd[0].name[i];
				fd[0].name[i] = fd[1].name[i];
				fd[1].name[i] = ch;
			}

			// shift index down
			if (inx > 0)
			{
				inx -= 2;
				fd -= 2;
			}
		}

		// shift index up
		inx++;
		fd++;
	}

	PrintClear();
}

// request to reload current directory
void Reload()
{
	// reset file list
	FileNum = 0; // no entry
	FileCur = 0; // reset cursor
	FileTop = 0; // reset top entry

	// set flag - disk need to be remounted
	Remount = True;
}

// reset to root
void ResetRoot()
{
	// reset path to root
	Path[0] = PATHCHAR;
	Path[1] = 0;
	PathLen = 1;

	// request to reload current directory
	Reload();
}

// set cursor to last name
void SetLastName()
{
	sFileDesc* fd;
	int i;

	// no last name required
	if (LastNameLen == 0) return;

	// search last name in file list
	for (i = FileNum-1; i > 0; i--)
	{
		fd = &FileDesc[i];
		if ((fd->len == LastNameLen) &&
			((fd->attr & ATTR_DIR) == LastNameDir) &&
			(memcmp(LastName, fd->name, LastNameLen) == 0)) break;
	}

	// limit max. top entry
	FileTop = i - LastNameTop;
	if (FileTop < 0) FileTop = 0;
	if (FileTop > i) FileTop = i;
	FileCur = i;

	// limit min. top entry
	i = i - (FILEROWS-1);
	if (i < 0) i = 0;
	if (FileTop < i) FileTop = i;

	// delele last name request
	LastNameLen = 0;
}

// clear program memory
void ClearApp()
{
	// display info
	DispInfo("Erasing...");

	// find end of memory
	const u32* end = Flash_Check(FLASH_BASE+BOOTLOADER_SIZE, FLASHSIZE-BOOTLOADER_SIZE);

	// align to 256 B
	int size = (u32)end - (FLASH_BASE+BOOTLOADER_SIZE);
	size = (size + FLASH_PAGEER_SIZE-1) & ~(FLASH_PAGEER_SIZE-1);

	// erase memory
	Flash_Erase(FLASH_BASE+BOOTLOADER_SIZE, size, 1000);
}

/*
// Loader resident data
typedef struct {
	u16	crc;		// 0x00: CRC-16C CCITT checksum of following 14 bytes of this structure (use Crc16CFast())
	u8	flags;		// 0x02: flags	bit 7: selected file is directory
				//		bit 6: feedback flag from the application - return to the loader
				//		bit 0..5: relative offset of selected file on the page (to get top file of the file list)
	u8	lastnamelen;	// 0x03: length of name of selected file
	u8	lastname[8];	// 0x04: name of last selected file (without terminating 0 and without file extension ".BIN")
	u32	curdirclust;	// 0x0C: cluster of current directory (0 = ROOT)
} sLoaderData;
*/

// save bot laoder data
void SaveBootData()
{
	if ((uint)FileCur < (uint)FileNum)
	{
		sLoaderData* ld = LoaderData; // pointer to boot loader data
		sFileDesc* fd = &FileDesc[FileCur]; // pointer to file descriptor
		ld->flags = ((fd->attr & ATTR_DIR) != 0) ? B7 : 0;
		int i = FileCur - FileTop;	// offset to top file
		if (i < 0) i = 0;
		if (i >= FILEROWS) i = FILEROWS-1;
		ld->flags |= i;			// add relative top file
		ld->lastnamelen = fd->len; // length of name of selected file
		memcpy(ld->lastname, fd->name, 8); // copy name of selected file
		SetDir(Path); // set current path
		ld->curdirclust = CurDirClust; // cluster of current directory
		LoaderData_CRC();		// setup CRC of loader data
	}
}

// load boot loader data
void LoadBootData()
{
	sLoaderData* ld = LoaderData; // pointer to boot loader data
	if (LoaderData_Check()) // check checksum of loader data
	{
		LastNameDir = ((ld->flags & B7) != 0) ? ATTR_DIR : 0; // attribute of the selected file
		LastNameTop = ld->flags & 0x3f; // offset of top file
		LoaderReq = (ld->flags & B6) != 0; // reuest to run loader instead application
		LastNameLen = ld->lastnamelen; // length of name of selected file
		memcpy(LastName, ld->lastname, 8); // copy name of selected file
		if (DiskMounted())
		{
			CurDirClust = ld->curdirclust; // cluster of current directory
			PathLen = GetDir(Path, PATHMAX); // get current directory
		}
		else
		{
			CurDirClust = 0;
			PathLen = 0;
		}
		if (PathLen == 0) ResetRoot();
	}
	else
	{
		LastNameDir = 0;
		LoaderReq = False;
		LastNameLen = 0;
		LastNameTop = 0;
		ResetRoot();
	}
	ld->lastnamelen = 0; // destroy loader data
}

// run application from Flash
//extern "C" 
void __attribute__((noreturn)) GoToApp();

// run application
void RunApp()
{
	int i;

	// save boot data
	SaveBootData();

	// wait for no key pressed
	KeyWaitNoPressed();
	
	// System terminate
	SystemTerm();

	// run application from Flash
	GoToApp();
}

// system setup
void SysSetup()
{
	// clear screen
	PrintClear();

	// battery input from PC4 (A2)
	RCC_PCClkEnable();
	GPIO_Mode(PC4, GPIO_MODE_AIN);

	// ADC1 clock enable
	RCC_ADC1ClkEnable();

	// initialize ADC single conversion mode
	ADC1_InitSingle();

	// measure reference voltage 1.2V
	u32 vref = ADC1_GetSingleMul(8, B16) >> 9; // result value (12+16-9) 19 bits
	vref = (u32)(1200 << 19) / vref;

	// re-initialize ADC single conversion mode
	ADC1_InitSingle();

	// measure battery voltage (input A2)
	u32 vbat = ADC1_GetSingleMul(2, B16) >> 9; // result value (12+16-9) 19 bits
	vbat = (vbat * vref) >> 19;

	// display main voltage
	DecUNum(DecNumBuf, vref, 0);
	PrintText("\nSupply: ");
	PrintText(DecNumBuf);
	PrintText("mV\n");

	// display battery voltage
	DecUNum(DecNumBuf, vbat, 0);
	PrintText("\nBattery: ");
	PrintText(DecNumBuf);
	PrintText("mV\n");

	PrintText("\n\nB=Up-Dir Y=Back");

	// display update
	DispUpdate();
}

int main(void)
{
	int i, j, n, size;
	u32 t;
	sFileDesc* fd;

	// short delay to power-up SD card
	WaitMs(10);

	// display prompt to insert SD card
	PrintClear();
	DispInfo("Insert SD card");

	// initialize remount
	LastMount = Time()-2000000*HCLK_PER_US; // invalidate last mount time = current time - 2 seconds
	Remount = True; // remount request

	// loading files
	if (!DiskMount())
	{
		// load boot loader data
		LoadBootData();

		// cannot mount disk, try to run current application ... not if software reset from application
		if (!LoaderReq && CheckApp()) RunApp();
	}
	else
	{
		FileTop = 0;

		// load boot loader data
		LoadBootData();

		// display info text
		DispInfo("Loading files...");

		// load files
		Remount = False; // clear flag - disk already remounted
		LoadFileList(); // load file list

		// set cursor to last name
		SetLastName();

		// display new directory
		DispFileList();
	}

	while(True)
	{
		// display update
		DispUpdate();

		// check last mount time (interval 1 second)
		t = Time();
		if ((int)(t - LastMount) > 1000000*HCLK_PER_US)
		{
			// save new last mount time
			LastMount = t;

			// disk removed, request to remount disk next time
			if (!DiskMount())
			{
				// clear directories if not done yet
				if (!Remount) // if remount request yet not set
				{
					// reset to root
					ResetRoot();

					// display info text
					DispInfo("Insert SD card");
				}
			}
			else
			{
				// reload directories if disk need to be remounted
				if (Remount)
				{
					// display info text
					DispInfo("Loading files...");

					// load files
					Remount = False; // clear flag - disk already remounted
					LoadFileList(); // load file list

					// set cursor to last name
					SetLastName();

					// display new directory
					DispFileList();
				}
			}
		}

		// keyboard service
		while (True)
		{
			u8 ch = KeyGet();
			if (ch == NOKEY) break;

			// key switch
			switch (ch)
			{
			// Down
			case KEY_DOWN:
				if (FileCur < FileNum-1)
				{
					// increase cursor
					FileCur++;

					// prepare max. top entry
					i = FileCur - (FILEROWS-1);
					if (i < 0) i = 0;

					// limit min. top entry
					if (FileTop < i) FileTop = i;

					// repaint display
					DispFileList();
				}
				break;

			// Right
			case KEY_RIGHT:
				if (FileCur < FileNum-1)
				{
					// max. top entry
					i = FileNum - FILEROWS;
					if (i < 0) i = 0;

					// max. top entry not reached - move page
					if (FileTop < i)
					{
						j = FILEROWS - 1; // page size
						FileTop += j; // shift top file
						if (FileTop > i) // max. top entry exceeded?
						{
							j -= FileTop - i; // reduce page size
							FileTop = i; // reduce top entry
						}
						FileCur += j; // shift cursor
					}

					// max. top entry reached - only shift cursor
					else
						FileCur = FileNum-1;

					// repaint display
					DispFileList();
				}
				break;

			// Up
			case KEY_UP:
				if (FileCur > 0)
				{
					// decrease cursor
					FileCur--;

					// limit top entry
					if (FileTop > FileCur) FileTop = FileCur;

					// repaint display
					DispFileList();
				}
				break;

			// Left
			case KEY_LEFT:
				if (FileCur > 0)
				{
					// min. top entry not reached - move page
					if (FileTop > 0)
					{
						j = FILEROWS - 1; // page size
						FileTop -= j; // shift top file
						if (FileTop < 0) // min. top entry exceeded?
						{
							j += FileTop; // reduce page size
							FileTop = 0; // reduce top entry
						}
						FileCur -= j; // shift cursor
					 }

					// min. top entry reached - only shift cursor
					else
						FileCur = 0;

					// repaint display
					DispFileList();
				}
				break;

			// Enter
			case KEY_A:
				if ((uint)FileCur < (uint)FileNum)
				{
					fd = &FileDesc[FileCur];

					// change directory
					if ((fd->attr & ATTR_DIR) != 0)
					{
						// going to up-directory
						if ((fd->len == 2) && (fd->name[0] == '.') && (fd->name[1] == '.'))
						{
							// delete last directory
							i = PathLen;
							while (Path[PathLen-1] != PATHCHAR) PathLen--;

							// save last name
							LastNameLen = i - PathLen;
							memcpy(LastName, &Path[PathLen], LastNameLen);
							LastNameDir = ATTR_DIR;

							// delete path separator if not root
							if (PathLen > 1) PathLen--;
						}

						// going to sub-directory
						else
						{
							// check path length
							if (PathLen + 1 + fd->len <= PATHMAX)
							{
								// add path separator if not root
								if (PathLen != 1) Path[PathLen++] = PATHCHAR;

								// add path name
								memcpy(&Path[PathLen], fd->name, fd->len);
								PathLen += fd->len;
							}
						}

						// set terminating 0
						Path[PathLen] = 0;

						// request to reload current directory
						FileTop = 0;
						Reload();

						// invalidate last mount time = current time - 2 seconds
						LastMount = Time()-2000000*HCLK_PER_US;
					}
					else
					{
						// set directory
						SetDir(Path);

						// prepare filename of the file
						memcpy(TempBuf, fd->name, fd->len);
						memcpy(&TempBuf[fd->len], ".BIN", 5);

						// open file
						DispInfo("Loading...");
						WaitMs(10);
						if (!FileOpen(&FileF, TempBuf))
						{
							// error
							DispInfo("Cannot open file");
							WaitMs(1000);
							PrintClear();
							DispFileList();
							KeyFlush();
						}
						else
						{
							// read application header
							FileSeek(&FileF, APPINFO_OFF);
							memset(&App, 0, sizeof(sAppInfoHeader));
							FileRead(&FileF, &App, sizeof(sAppInfoHeader));
							WaitMs(10);

							// check file header
							size = FileF.size;
							if (
								(size > FLASHSIZE) ||	// file is too big
								(size < APPINFO_OFF+sizeof(sAppInfoHeader)) ||	// file is too small
								(*(u32*)App.magic1 != APPINFO_MAGIC1) || // "INFO"
								(*(u16*)App.mcu != APPINFO_MCU) ||	// "0X"
								(*(u32*)App.magic2 != APPINFO_MAGIC2)) // "info"
							{
								// invalid file
								DispInfo("Invalid file");
								WaitMs(1000);
								PrintClear();
								DispFileList();
								KeyFlush();
							}
							else
							{
								// clear program memory
								ClearApp();
								WaitMs(10);
								DispInfo("Loading...");
								WaitMs(10);

								// load program
								FileSeek(&FileF, BOOTLOADER_SIZE); // reset file pointer
								size -= BOOTLOADER_SIZE;
								i = FLASH_BASE+BOOTLOADER_SIZE;
								while (size > 0)
								{
									// read block from the file
									n = size;
									if (n > TEMPBUF) n = TEMPBUF;
									FileRead(&FileF, TempBuf, n);

									// write block to the flash
									n = (n + FLASH_PAGEPG_SIZE-1) & ~(FLASH_PAGEPG_SIZE-1);
									Flash_Program(i, (const u32*)TempBuf, n, 1000);

									// shift
									i += TEMPBUF;
									size -= TEMPBUF;
								}

								// check checksum
								if (CheckApp())
								{
									RunApp();
								}

								// error
								DispInfo("Load Error!");
								WaitMs(2000);

								// request to reload current directory
								Reload();

								// invalidate last mount time = current time - 2 seconds
								LastMount = Time()-2000000*HCLK_PER_US;
								KeyFlush();
							}
						}
					}
				}
				break;

			// out of directory
			case KEY_B:

				// system setup
				KeyFlush();
				while ((ch = KeyGet()) == NOKEY)
				{
					SysSetup();
					WaitMs(100);
				}
				PrintClear();

				// repaint display
				DispFileList();

				// not root directory yet
				if ((ch == KEY_B) && (PathLen > 1))
				{
					// delete last directory
					i = PathLen;
					while (Path[PathLen-1] != PATHCHAR) PathLen--;

					// save last name
					LastNameLen = i - PathLen;
					memcpy(LastName, &Path[PathLen], LastNameLen);
					LastNameDir = ATTR_DIR;

					// delete path separator if not root
					if (PathLen > 1) PathLen--;

					// set terminating 0
					Path[PathLen] = 0;

					// request to reload current directory
					FileTop = 0;
					Reload();

					// invalidate last mount time = current time - 2 seconds
					LastMount = Time()-2000000*HCLK_PER_US;
				}
				break;

			// run application in flash
			case KEY_Y:
				if (CheckApp())
				{
					RunApp();
				}
				break;
			}
		}
	}
}
