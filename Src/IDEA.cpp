//
// IDEA.cpp
//
// Fast and dirty implementation of the International Data Encryption Algorithm.
// For non-commercial use (just because it could be better).
//
// There are probably some holes such as:
//	* Data may be swapped out cuz app uses virtual memory.
//	* Not a lot of thought into key generation... no analysis of distribution
//	  of letters/words in the Engligh language.
//	* Small minimum password size.
//	* Possibly will get confused between encrypting and decrypting for some data...
//	  although this is (extremely) unlikely.
//
// Copyright (c) 2000 Tristan Grimmer.
// Please read the accompanying licence file.
//

#ifdef IDEA_WIN
#define VC_EXTRALEAN

#ifndef WINVER																											// Allow use of features specific to Windows 98 or later.
#define WINVER 0x0501																									// Change this to the appropriate value to target Windows 2000 or later.
#endif

#include <afx.h>
#include <afxwin.h>																										// MFC core and standard components.
#else
#include <Windows.h>
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "IDEA.h"


#define HEADER_MAGIC_A 0x4287FAFD
#define HEADER_MAGIC_B 0x11982C0C
#define HEADER_MAGIC_C 0xAB3005B2


// These values must be kept up to date.
#ifndef IDEA_WIN
#ifdef IDEA_DEBUG
#define EXE_SIZE 543744
#else
#define EXE_SIZE 94208
#endif
#else
#ifdef IDEA_DEBUG
#define EXE_SIZE 7575552
#else
#define EXE_SIZE 2042368
#endif
#endif


typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned _int64 uint64;
typedef int int32;


void IdeaRound(uint16& x0, uint16& x1, uint16& x2, uint16& x3, uint16 z0, uint16 z1, uint16 z2, uint16 z3, uint16 z4, uint16 z5);
void ComputeKeyArrays(uint16 ek[56], uint16 dk[56], uint64 key0, uint64 key1);
void OutputTransform(uint16& x0, uint16& x1, uint16& x2, uint16& x3, uint16 z0, uint16 z1, uint16 z2, uint16 z3);
uint32 Iinv(uint32);
uint16 Ineg(uint16);


uint64 EnDeCrypt(uint64 data, uint16 skey[56])
{
	// Divide the data into sub-blocks.
	uint16 x0 = uint16( (data & 0xFFFF000000000000) >> 48 );
	uint16 x1 = uint16( (data & 0x0000FFFF00000000) >> 32 );
	uint16 x2 = uint16( (data & 0x00000000FFFF0000) >> 16 );
	uint16 x3 = uint16( (data & 0x000000000000FFFF) );

	int keyi = 0;
	for (int i = 0; i < 8; i++)
	{
		IdeaRound(x0, x1, x2, x3, skey[keyi + 0], skey[keyi + 1], skey[keyi + 2], skey[keyi + 3], skey[keyi + 4], skey[keyi + 5]);
		keyi += 6;
	}
	OutputTransform(x0, x1, x2, x3, skey[48], skey[49], skey[50], skey[51]);

	// x0 to x3 now contain the cyphertext!!!
	return (uint64(x0) << 48) + (uint64(x1) << 32) + (uint64(x2) << 16) + x3;	
}


void ComputeKeyArrays(uint16 ek[56], uint16 dk[56], uint64 key0, uint64 key1)
{
	// First we compute the Encryption keys...
	for (int s = 0; s < 7; s++)
	{
		ek[s*8 + 0] = uint16( (key0 & 0xFFFF000000000000) >> 48 );
		ek[s*8 + 1] = uint16( (key0 & 0x0000FFFF00000000) >> 32 );
		ek[s*8 + 2] = uint16( (key0 & 0x00000000FFFF0000) >> 16 );
		ek[s*8 + 3] = uint16( (key0 & 0x000000000000FFFF) );
		ek[s*8 + 4] = uint16( (key1 & 0xFFFF000000000000) >> 48 );
		ek[s*8 + 5] = uint16( (key1 & 0x0000FFFF00000000) >> 32 );
		ek[s*8 + 6] = uint16( (key1 & 0x00000000FFFF0000) >> 16 );
		ek[s*8 + 7] = uint16( (key1 & 0x000000000000FFFF) );

		// Rotate key left by 25.
		uint32 a = uint32( (key0 & 0xFFFFFF8000000000) >> 39 );
		uint64 b = (key0 & 0x0000007FFFFFFFFF);
		uint32 c = uint32( (key1 & 0xFFFFFF8000000000) >> 39 );
		uint64 d = (key1 & 0x0000007FFFFFFFFF);
		key0 = (b << 25) + c;
		key1 = (d << 25) + a;
	}

	// Next we compute the Decryption keys...
	int r;
	for (r = 0; r < 9; r++)
	{
		dk[r*6 + 0] = Iinv( ek[ ((8-r) * 6) + 0 ] );
		dk[r*6 + 3] = Iinv( ek[ ((8-r) * 6) + 3 ] );
	}

	for (r = 0; r < 8; r++)
	{
		dk[r*6 + 4] = ek[ ((7-r) * 6) + 4 ];
		dk[r*6 + 5] = ek[ ((7-r) * 6) + 5 ];
	}

	for (r = 1; r < 8; r++)
	{
		dk[r*6 + 1] = Ineg( ek[ ((7-r) * 6) + 8 ] );
		dk[r*6 + 2] = Ineg( ek[ ((7-r) * 6) + 7 ] );
	}

	dk[1] = Ineg( ek[49] );
	dk[2] = Ineg( ek[50] );
	dk[49] = Ineg( ek[1] );
	dk[50] = Ineg( ek[2] );
}


uint32 Iinv(uint32 s)
{
	// Uses Euclid's algorithm...
	int32 n1, n2, q, r, b1, b2, t;

	if (!s)
	{
	  b2 = 0;
	}
	else
	{
		n1 = 65537;
		n2 = s;
		b2 = 1;
		b1 = 0;
		do
		{
			r = (n1 % n2);
			q = (n1 - r) / n2;
			if (!r)
			{
				if (b2 < 0)
					b2 = 65537 + b2;
			}
			else
			{
				n1 = n2;
				n2 = r;
				t = b2;
				b2 = b1 - q*b2;
				b1 = t;
			}
		}
		while (r);
	}
	return b2;
}


uint16 Ineg(uint16 s)
{
	return 0 - s;
}


uint16 Imul(uint16 a, uint16 b)
{
	uint32 p = a * b;
	if (p == 0)
	{
		a = 65537 - a - b;
	}
	else
	{
		a = p >> 16;
		b = p;
		a = b - a;
		if (b < a)
			a = a + 65537;
	}
	return a;
}


void OutputTransform(uint16& x0, uint16& x1, uint16& x2, uint16& x3, uint16 z0, uint16 z1, uint16 z2, uint16 z3)
{
	x0 = Imul(z0, x0);
	uint32 x1orig = x1;
	x1 = x2 + z1;
	x2 = x1orig + z2;
	x3 = Imul(z3, x3);
}


void IdeaRound(uint16& x0, uint16& x1, uint16& x2, uint16& x3, uint16 z0, uint16 z1, uint16 z2, uint16 z3, uint16 z4, uint16 z5)
{
	uint32 a = Imul(x0,z0);
	uint16 b = x1 + z1;
	uint16 c = x2 + z2;
	uint32 d = Imul(x3, z3);

	uint16 e = a ^ c;
	uint16 f = b ^ d;

	uint32 g = Imul(z4, e);
	uint16 h = f + g;

	uint32 i = Imul(h, z5);
	uint16 j = g + i;

	uint32 k = a ^ i;
	uint32 l = b ^ j;
	uint32 m = c ^ i;
	uint32 n = j ^ d;

	x0 = k;
	x1 = m;
	x2 = l;
	x3 = n;
}


char GetCharID(char c)
{
	// Slow but easy to understand...
	char carray[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz`1234567890-=~!@#$%^&*()_+[{]};:'<,>.? ";

	for (int i = 0; i < sizeof(carray); i++)
	{
		if (carray[i] == c)
			return i;
	}
	return -1;
}


void XORKeyBit(uint64& key0, uint64& key1, int bit, bool val)
{
	int value = val ? 1 : 0;

	if (bit < 64)
	{
		uint64 b = uint64(value) << bit;
		key0 ^= b;
	}
	else
	{
		bit -= 64;
		uint64 b = uint64(value) << bit;
		key1 ^= b;
	}
}


void CreateKeyFromPassPhrase(uint64& key0, uint64& key1, const char* phrase)
{
	uint64 bucket = 0;
	int bucketrange = 0;

	key0 = 0;
	key1 = 0;
	int keybit = 0;
	for (int c = 0; c < int(strlen(phrase)); c++)
	{
		// Returns a number from 0 to 91.  We can use first 6 LS bits.  7th is a carry.
		char newid = GetCharID( phrase[c] );
		if (newid == -1)
		{
			printf("Error:  Unhandled character %c in pass phrase.\n", phrase[c]);
			exit(1);
		}
		if (newid > 63)
		{
			// 0 to 27.
			char extra = newid - 64;
			char bits4 = extra & 0x0F;
			for (int i = 0; i < 4; i++)
			{
				bool bit = (bits4 & (1 << i)) ? true : false;
				XORKeyBit(key0, key1, keybit, bit);
				keybit++;
				if (keybit == 128)
					keybit = 0;
			}
		}
		else
		{
			char bits6 = newid & 0x3F;
			for (int i = 0; i < 6; i++)
			{
				bool bit = (bits6 & (1 << i)) ? true : false;
				XORKeyBit(key0, key1, keybit, bit);
				keybit++;
				if (keybit == 128)
					keybit = 0;
			}
		}
	}
}


struct FileHeader
{
	FileHeader(uint32 magica, uint32 magicb, uint32 magicc, uint32 filesize, const char* filename);
	FileHeader()																										{ }
	uint32 MagicA;
	uint32 MagicB;
	uint32 MagicC;
	uint32 FileSize;

	// 32 64bit chunks.
	char FileName[256];
};


FileHeader::FileHeader(uint32 magica, uint32 magicb, uint32 magicc, uint32 filesize, const char* filename) :
	MagicA(magica),
	MagicB(magicb),
	MagicC(magicc),
	FileSize(filesize)
{
	for (int c = 0; c < 256; c++)
		FileName[c] = '\0';

	strcpy(FileName, filename);
}


void PrintUsageAndExit(char* argv0 = NULL)
{
	if (!argv0)
		argv0 = "IDEA";

	printf("\n");
	printf("Encrypt/Decrypt a file using the 128 bit\n");
	printf("International Data Encryption Algorithm (IDEA) in CBC Mode.\n");
	printf("Implementation by T. Grimmer circa 2000.\n");
	printf("\n");
	printf("Usage:  %s <filetoencrypt> <passphrase in quotes>\n", argv0);
	printf("  Encrypts the file with the supplied passphrase.  Creates the encrypted\n");
	printf("  file and tells you it's name.  The encrypted file is a self extracting\n");
	printf("  executable.\n\n");
	printf("Usage:  %s <passphrase in quotes>\n", argv0);
	printf("  Decrypts the self extracting executable file.  re-creates the original file.\n");
	printf("\n");
	exit(1);
}


void EncryptHeaderAndFileCBCMode(FileHeader& header, const char* infile, FILE* ofile, uint16 ek[56], int infilesize)
{
	FILE* ifile = fopen(infile, "rb");
	if (!ifile)
	{
		printf("Error opening file %s for reading.\n", infile);
		PrintUsageAndExit();
	}

	uint64 iv = 0;
	uint64 plain;

	// A total of 34 64bit blocks in header.
	for (int hb = 0; hb < 34; hb++)
	{
		plain = ((uint64*)&header)[hb];
		uint64 data = plain ^ iv;
		uint64 cypher = EnDeCrypt(data, ek);
		fwrite(&cypher, 8, 1, ofile);
		iv = cypher;
	}

	int thisread = 0;
	int numread = 0;
	int numstar = -1;
	int numpercent = -1;

	printf("\n");
	while (thisread = int(fread(&plain, 1, 8, ifile)))
	{
		numread += thisread;
		uint64 data = plain ^ iv;
		uint64 cypher = EnDeCrypt(data, ek);
		
		fwrite(&cypher, 8, 1, ofile);
		iv = cypher;

		float percent = float(numread) / float(infilesize);
		bool doprint = false;

		if (numstar != int(percent * 60.0))
		{
			numstar = int(percent * 60.0);
			doprint = true;
		}

		if (numpercent != int(100.0 * percent))
		{
			numpercent = int(100.0 * percent);
			doprint = true;
		}

		if (doprint)
		{
			printf("[");
			for (int wq = 0; wq < numstar; wq++)
				printf(".");

			printf("] %d%%\r", numpercent);	
		}
	}

	fclose(ifile);
}


void DecryptFileCBCMode(FILE* srcfile, char* destfile, uint16 dk[56], int destsize, uint64 iv)
{
	FILE* dfile = fopen(destfile, "wb");
	if (!dfile)
	{
		printf("Error opening file %s for writing.\n", destfile);
		PrintUsageAndExit();
	}

	uint64 cypher;
	int numwritten = 0;
	int numstar = -1;
	int numpercent = -1;

	printf("\n");
	while (fread(&cypher, 1, 8, srcfile))
	{
		uint64 xored = EnDeCrypt(cypher, dk);
		uint64 plain = iv ^ xored;

		int numtowrite = (destsize - numwritten) > 8 ? 8 : destsize - numwritten;

		numwritten += int(fwrite(&plain, 1, numtowrite, dfile));

		float percent = float(numwritten) / float(destsize);
		bool doprint = false;

		if (numstar != int(percent * 60.0))
		{
			numstar = int(percent * 60.0);
			doprint = true;
		}

		if (numpercent != int(100.0 * percent))
		{
			numpercent = int(100.0 * percent);
			doprint = true;
		}

		if (doprint)
		{
			printf("[");
			for (int wq = 0; wq < numstar; wq++)
				printf(".");

			printf("] %d%%\r", numpercent);
		}

		iv = cypher;
	}
	printf("\nCreated file: %s\n", destfile);
	fclose(dfile);
}


uint64 DecryptHeader(FileHeader& fh, FILE* srcfile, uint16 dk[56])
{
	uint64 iv = 0;
	uint64 cypher;

	// total of 34 64bit blocks in header.
	for (int hb = 0; hb < 34; hb++)
	{
		fread(&cypher, 1, 8, srcfile);
		uint64 xored = EnDeCrypt(cypher, dk);
		uint64 plain = iv ^ xored;

		((uint64*)&fh)[hb] = plain;

		iv = cypher;
	}
	return iv;
}


void MyCopyFile(char* srcfilename, FILE* destfile)
{
	FILE* srcfile = fopen(srcfilename, "rb");
	if (!srcfile)
	{
		printf("Error opening file %s for reading.\n", srcfilename);
		PrintUsageAndExit();
	}

	char r;

	// @todo  Should only copy exe size.
	int i = 0;
	for (i = 0; i < EXE_SIZE; i++)
	{
		fread(&r, 1, 1, srcfile);
		fwrite(&r, 1, 1, destfile);
	}

	fclose(srcfile);
}


void ChooseDestFilename(char dfile[256], const char* basesuggestion)
{
	char base[256];

	#ifndef IDEA_WIN
	if (strchr(basesuggestion, ':') || strchr(basesuggestion, '/') || strchr(basesuggestion, '\\'))
		sprintf(base, "Data");
	else
		sprintf(base, basesuggestion);
	#else
	sprintf(base, basesuggestion);
	#endif

	char poss[256];
	for (int fnum = 0; fnum < 10000; fnum++)
	{
		if (fnum > 0)
			sprintf(poss, "%s%d.exe", base, fnum);
		else
			sprintf(poss, "%s.exe", base);

		WIN32_FIND_DATA finddata;
		HANDLE h = FindFirstFile(poss, &finddata);

		if (h == INVALID_HANDLE_VALUE)
		{
			// All is good... file doesn't exist.
			strcpy(dfile, poss);
			return;
		}
		FindClose(h);
	}
	printf("Unable to find a filename to write to.  Please delete or move data.exe\n");
	exit(1);
}


static char ErrMsg[1024];


const char* GetErrorMessage()
{
	return ErrMsg;
}


bool DoEncrypt(const char* fileName, const char* passPhrase)
{
	uint64 key0, key1;

	CreateKeyFromPassPhrase(key0, key1, passPhrase);

	// Only first 52 are used!!!
	uint16 ekey[56];
	uint16 dkey[56];
	ComputeKeyArrays(ekey, dkey, key0, key1);

	// Make sure file exists.
	WIN32_FIND_DATA finddata;
	HANDLE h = FindFirstFile(fileName, &finddata);

	if (h == INVALID_HANDLE_VALUE)
	{
		sprintf(ErrMsg, "Error: Input file %s doesn't seem to exist!\n", fileName);
		return false;
	}

	FindClose(h);

	// Find the destination file name...
	char dfile[256];
	ChooseDestFilename(dfile, fileName);

	// Copy the current exe to that file.
	char exefile[MAX_PATH+1];
	GetModuleFileName(0, exefile, MAX_PATH);

	FILE* destfile = fopen(dfile, "wb");
	if (!destfile)
	{
		sprintf(ErrMsg, "Error opening file %s for writing.\n", dfile);
		return false;
	}

	// Really only want to copy the first bit...
	MyCopyFile(exefile, destfile);

	// Create header info... The filename gets stripped of any possible full-path info...
	// Just the filename is stored.
	char* lastSlash = strrchr((char*)fileName, '/');
	if (!lastSlash)
		lastSlash = strrchr((char*)fileName, '\\');

	const char* justName = fileName;
	if (lastSlash)
		justName = lastSlash + 1;

	FileHeader fh(HEADER_MAGIC_A, HEADER_MAGIC_B, HEADER_MAGIC_C, finddata.nFileSizeLow, justName);

	EncryptHeaderAndFileCBCMode(fh, fileName, destfile, ekey, finddata.nFileSizeLow);

	fclose(destfile);
	printf("\nCreated file: %s\n", dfile);
	return true;
}


bool DoDecrypt(const char* passPhrase)
{
	uint64 key0, key1;

	CreateKeyFromPassPhrase(key0, key1, passPhrase);

	// Only first 52 are used!!!
	uint16 ekey[56];
	uint16 dkey[56];
	ComputeKeyArrays(ekey, dkey, key0, key1);

	// Decrypt.
	char exefile[MAX_PATH+1];
	GetModuleFileName(0, exefile, MAX_PATH);

	FILE* srcfile = fopen(exefile, "rb");
	if (!srcfile)
	{
		sprintf(ErrMsg, "Error opening file %s for writing.\n", exefile);
		return false;
	}

	fseek(srcfile, EXE_SIZE, SEEK_SET);

	FileHeader fh;
	uint64 nextiv = DecryptHeader(fh, srcfile, dkey);

	// I'm assuming 96 bits is enough to ensure the assumption that the pass phrase was incorrect.
	if ((fh.MagicA != HEADER_MAGIC_A) || (fh.MagicB != HEADER_MAGIC_B) || (fh.MagicC != HEADER_MAGIC_C))
	{
		#ifndef IDEA_WIN
		printf("Pass phrase incorrect!\n");
		for (volatile int pause = 0; pause < 1000000000; pause++);
		fclose(srcfile);
		exit(1);
		#else
		sprintf(ErrMsg, "Pass Phrase Incorrect.\n");
		fclose(srcfile);
		return false;
		#endif
	}

	// Encrypt.
	WIN32_FIND_DATA finddata;
	HANDLE h = FindFirstFile(fh.FileName, &finddata);

	if (h != INVALID_HANDLE_VALUE)
	{
		#ifndef IDEA_WIN
		printf("Error: Output file %s already exists and will not be overwritten!\n", fh.FileName);
		FindClose(h);
		fclose(srcfile);
		exit(1);
		#else
		sprintf(ErrMsg, "Error: Output file %s already exists and will not be overwritten!\n", fh.FileName);
		FindClose(h);
		fclose(srcfile);
		return false;
		#endif
	}

	DecryptFileCBCMode(srcfile, fh.FileName, dkey, fh.FileSize, nextiv);
	fclose(srcfile);
	return true;
}


#ifndef IDEA_WIN
int main(int argc, char* argv[])
{
	if ((argc != 3) && (argc != 2))
	{
		PrintUsageAndExit(argv[0]);
	}

	if (argc == 3)			// We're encrypting!!
	{
		bool ok = DoEncrypt(argv[1], argv[2]);
		if (!ok)
		{
			printf( GetErrorMessage() );
			PrintUsageAndExit(argv[0]);
		}
	}
	else
	{
		bool ok = DoDecrypt(argv[1]);
		if (!ok)
		{
			printf( GetErrorMessage() );
			PrintUsageAndExit(argv[0]);
		}
	}
	return 0;
}
#endif
