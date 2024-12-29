/*David Shea (Coyote Developments) (GBC) to MIDI converter*/
/*By Will Trowbridge*/
/*Portions based on code by ValleyBell*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define bankSize 16384

FILE* rom, * txt;
long bank;
long offset;
long tablePtrLoc;
long tableOffset;
int i, j;
char outfile[1000000];
int songNum;
long songPtr;
int chanMask;
long bankAmt;
int foundTable = 0;
long firstPtr = 0;
int curInst = 0;
int curVol = 0;
int stopCvt = 0;

unsigned static char* romData;

const char MagicBytes[7] = { 0x3E, 0x80, 0xE0, 0x23, 0xC9, 0x24, 0x00 };

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
static void Write8B(unsigned char* buffer, unsigned int value);
static void WriteBE32(unsigned char* buffer, unsigned long value);
static void WriteBE24(unsigned char* buffer, unsigned long value);
static void WriteBE16(unsigned char* buffer, unsigned int value);
void song2txt(int songNum, long ptr);

/*Convert little-endian pointer to big-endian*/
unsigned short ReadLE16(unsigned char* Data)
{
	return (Data[0] << 0) | (Data[1] << 8);
}

static void Write8B(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = value;
}

static void WriteBE32(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF000000) >> 24;
	buffer[0x01] = (value & 0x00FF0000) >> 16;
	buffer[0x02] = (value & 0x0000FF00) >> 8;
	buffer[0x03] = (value & 0x000000FF) >> 0;

	return;
}

static void WriteBE24(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF0000) >> 16;
	buffer[0x01] = (value & 0x00FF00) >> 8;
	buffer[0x02] = (value & 0x0000FF) >> 0;

	return;
}

static void WriteBE16(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = (value & 0xFF00) >> 8;
	buffer[0x01] = (value & 0x00FF) >> 0;

	return;
}

int main(int args, char* argv[])
{
	printf("David Shea (Coyote Developments) (GBC) to TXT converter\n");
	if (args != 3)
	{
		printf("Usage: DSH2TXT <rom> <bank>\n");
		return -1;
	}
	else
	{
		if ((rom = fopen(argv[1], "rb")) == NULL)
		{
			printf("ERROR: Unable to open file %s!\n", argv[1]);
			exit(1);
		}
		else
		{
			if ((rom = fopen(argv[1], "rb")) == NULL)
			{
				printf("ERROR: Unable to open file %s!\n", argv[1]);
				exit(1);
			}
			else
			{
				bank = strtol(argv[2], NULL, 16);
				if (bank != 1)
				{
					bankAmt = bankSize;
				}
				else
				{
					bankAmt = 0;
				}
				fseek(rom, ((bank - 1) * bankSize), SEEK_SET);
				romData = (unsigned char*)malloc(bankSize);
				fread(romData, 1, bankSize, rom);
				fclose(rom);

				/*Try to search the bank for song table loader*/
				for (i = 0; i < bankSize; i++)
				{
					if ((!memcmp(&romData[i], MagicBytes, 7)) && foundTable != 1)
					{
						tablePtrLoc = bankAmt + i + 7;
						printf("Found song table at address 0x%04x!\n", tablePtrLoc);
						tableOffset = ReadLE16(&romData[i + 7]) + (tablePtrLoc - 2);
						printf("Song table starts at 0x%04x...\n", tableOffset);
						foundTable = 1;
					}
				}

				if (foundTable == 1)
				{
					i = tableOffset - bankAmt;
					songNum = 1;
					firstPtr = ReadLE16(&romData[i]) + tableOffset;
					while ((ReadLE16(&romData[i]) + tableOffset) < 0x8000 && stopCvt == 0)
					{
						if (i < (firstPtr - bankAmt))
						{
							songPtr = ReadLE16(&romData[i]) + tableOffset;
							printf("Song %i: 0x%04X\n", songNum, songPtr);
							song2txt(songNum, songPtr);
							i += 2;
							songNum++;
						}
						else
						{
							stopCvt = 1;
						}

					}
				}
				else
				{
					printf("ERROR: Magic bytes not found!\n");
					exit(1);
				}
				printf("The operation was successfully completed!\n");
				exit(0);
			}
		}
	}
}

/*Convert the song data to TXT*/
void song2txt(int songNum, long ptr)
{
	int numChan = 0;
	int chanPtr = 0;
	long romPos = 0;
	long seqPos = 0;
	int seqEnd = 0;
	int curNote = 0;
	int curNoteLen = 0;
	int curDelay = 0;
	int curTrack = 0;
	int noteSize = 0;
	int restTime = 0;
	int tempo = 0;
	int noteOn = 0;
	int repeat1Times = 0;
	int repeat1Pos = 0;
	int repeat2Times = 0;
	int repeat2Pos = 0;
	unsigned char command[8];
	sprintf(outfile, "song%i.txt", songNum);
	if ((txt = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file song%i.txt!\n", songNum);
		exit(2);
	}
	else
	{
		romPos = ptr - bankAmt;
		numChan = ReadLE16(&romData[romPos]);
		fprintf(txt, "Number of channels: %i\n", numChan);
		romPos += 2;

		for (curTrack == 0; curTrack < numChan; curTrack++)
		{
			chanPtr = ReadLE16(&romData[romPos]);
			fprintf(txt, "Pointer: %01X\n", chanPtr);

			seqPos = ptr + chanPtr - bankAmt;
			seqEnd = 0;
			noteOn = 1;
			while (seqEnd == 0 && seqPos < bankAmt)
			{
				command[0] = romData[seqPos];
				command[1] = romData[seqPos + 1];
				command[2] = romData[seqPos + 2];
				command[3] = romData[seqPos + 3];
				command[4] = romData[seqPos + 4];
				command[5] = romData[seqPos + 5];
				command[6] = romData[seqPos + 6];
				command[7] = romData[seqPos + 7];
				if (command[0] < 0x80)
				{
					if (noteOn == 0)
					{
						curDelay = command[0];
						fprintf(txt, "Delay: %01X\n", curDelay);
						seqPos++;
						noteOn = 1;
					}
					else if (noteOn == 1)
					{
						curNote = command[0];
						fprintf(txt, "Play note: %01X\n", curNote);
						seqPos++;
						noteOn = 0;
					}

				}
				else if (command[0] > 0x80 && noteOn == 0)
				{
					curDelay = command[1];
					fprintf(txt, "Delay (long): %01X, %01X\n", command[0], curDelay);
					seqPos += 2;
					noteOn = 1;
				}

				else if (command[0] == 0x80)
				{
					fprintf(txt, "Turn off channel\n\n");
					seqEnd = 1;
				}

				else if (command[0] == 0x81)
				{
					fprintf(txt, "Turn on channel\n");
					noteOn = 0;
					seqPos++;
				}

				else if (command[0] == 0x82)
				{
					tempo = command[1];
					fprintf(txt, "Set tempo: %01X\n", tempo);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x83)
				{
					fprintf(txt, "Set envelope bits (1)\n");
					noteOn = 0;
					seqPos++;
				}

				else if (command[0] == 0x84)
				{
					fprintf(txt, "Set envelope bits (2)\n");
					noteOn = 0;
					seqPos++;
				}

				else if (command[0] == 0x85)
				{
					fprintf(txt, "Set envelope bits (3)\n");
					noteOn = 0;
					seqPos++;
				}

				else if (command[0] == 0x86)
				{
					fprintf(txt, "Set SFX volume (1)?: %01X\n", command[1]);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x87)
				{
					fprintf(txt, "Set SFX volume (2)?: %01X\n", command[1]);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x88)
				{
					fprintf(txt, "Set SFX volume (3)?: %01X\n", command[1]);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x89)
				{
					fprintf(txt, "Set SFX effect?: %01X\n", command[1]);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] >= 0x8A && command[0] < 0x90)
				{
					fprintf(txt, "Invalid command: %01X\n", command[0]);
					noteOn = 0;
					seqPos++;
				}

				else if (command[0] == 0x90)
				{
					fprintf(txt, "Set envelope size: %01X\n", command[1]);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x91)
				{
					fprintf(txt, "Set pitch bend?: %01X\n", command[1]);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x92)
				{
					fprintf(txt, "Set note speed?: %01X\n", command[1]);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x93)
				{
					fprintf(txt, "Unknown command 93: %01X\n", command[1]);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x94)
				{
					curInst = command[1];
					fprintf(txt, "Set instrument: %01X\n", curInst);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x95)
				{
					fprintf(txt, "Unknown command 95: %01X\n", command[1]);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x96)
				{
					fprintf(txt, "Set tuning: %01X\n", command[1]);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x97)
				{
					fprintf(txt, "Unknown command 97: %01X\n", command[1]);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0x98)
				{
					fprintf(txt, "Turn off noise channel?\n");
					noteOn = 0;
					seqPos++;
				}

				else if (command[0] == 0x99)
				{
					fprintf(txt, "Turn on noise channel?\n");
					noteOn = 0;
					seqPos++;
				}

				else if (command[0] >= 0x9A && command[0] < 0xA0)
				{
					fprintf(txt, "Invalid command: %01X\n", command[0]);
					noteOn = 0;
					seqPos++;
				}

				else if (command[0] >= 0xA0 && command[0] < 0xB0)
				{
					fprintf(txt, "Set volume/envelope settings?: %01X\n", command[0]);
					noteOn = 0;
					seqPos++;
				}

				else if (command[0] == 0xB0)
				{
					repeat2Pos = seqPos;
					repeat2Times = command[1];
					fprintf(txt, "Song loop point: %i times\n", repeat2Times);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0xB1)
				{
					repeat1Pos = seqPos;
					repeat1Times = command[1];
					fprintf(txt, "Repeat point: %i times\n", repeat1Times);
					noteOn = 0;
					seqPos += 2;
				}

				else if (command[0] == 0xB2 || command[0] == 0xB3)
				{
					fprintf(txt, "Invalid command: %01X\n", command[0]);
					noteOn = 0;
					seqPos++;
				}

				else if (command[0] == 0xB4)
				{
					fprintf(txt, "Go to song loop point\n\n");
					noteOn = 0;
					seqEnd = 1;
				}

				else if (command[0] == 0xB5)
				{
					fprintf(txt, "Go to repeat point\n");
					noteOn = 0;
					seqPos++;
				}

				else
				{
					fprintf(txt, "Unknown command: %01X\n", command[0]);
					noteOn = 0;
					seqPos++;
				}

			}
			romPos += 2;
		}
		fclose(txt);
	}
}