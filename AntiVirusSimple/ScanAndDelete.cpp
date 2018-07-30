// AllScanAndDelete.cpp : 定义控制台应用程序的入口点。
//

#include<stdio.h>
#include<TlHelp32.h>
#include"Psapi.h"
#include<string>
#include <stdlib.h>
#include<time.h>

#include <iostream>
#include <fstream>
#include <string.h>
#include <windows.h>
#include "md5.h"
#include "stdafx.h"

using namespace std;

#define MAX_PATH_LENGTH 500

void allScanEXE();
void scanEXE(char * lpPath);
void dealProcess(char * lpPath);
void md5Process(char * lpPath, char * md5String);
bool searchSignature(char * Md5String);
void doFindProcess(char * lpPath);

int counter;

int _tmain(int argc, _TCHAR* argv[])
{
	counter = 0;
//	scanEXE("R");
	allScanEXE();
	cout<<"There are"<<counter<<"files found in total~"<<endl;
	return 0;
	return 0;
}

void scanEXE(char * lpPath)
{
	CHAR szFilePath[MAX_PATH];

		//构造代表子目录和文件夹路径的字符串，使用通配符“*”
		lstrcpy(szFilePath, szPath);
		lstrcat(szFilePath, "\\*");

			WIN32_FIND_DATA FindFileData;
			HANDLE hListFile;
			//WIN32_FILE_ATTRIBUTE_DATA FileAttData;
			bool VirusFlag=false;
	
			//查找第一个文件/目录，获得查找句柄
			hListFile = FindFirstFile(szFilePath,&FindFileData);


	char szFind[MAX_PATH_LENGTH];
	WIN32_FIND_DATA FindFileData;
	strcpy(szFind,lpPath);
	strcat(szFind,"\\*.*");
	HANDLE hFind = ::FindFirstFileA(szFind,&FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
		return;
	while(1)
	{
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(FindFileData.cFileName[0] != '.')
			{
				char szFile[MAX_PATH_LENGTH];
				strcpy(szFile,lpPath);
				strcat(szFile,"\\");
				strcat(szFile,FindFileData.cFileName);
				scanEXE(szFile);
			}
		}
		else
		{
			int len = strlen(FindFileData.cFileName);
			const char *p = (char *)&FindFileData.cFileName[len-3];
			if(_stricmp(p,"exe") == 0)
			{
				char strFileName[MAX_PATH_LENGTH];
				strcpy(strFileName,lpPath);
				strcat(strFileName,"\\");
				strcat(strFileName,FindFileData.cFileName);
				dealProcess(strFileName);
			}
		}
		if(!FindNextFile(hFind,&FindFileData))
			break;
	}
	FindClose(hFind);
}

void allScanEXE()
{
	for(char cLabel='c';cLabel<='z';cLabel++)
	{
		char strRootPath[] = {"c:\\"};
		strRootPath[0] = cLabel;
		if(GetDriveType(strRootPath) == DRIVE_FIXED)
		{
			strRootPath[2] = '\0';
			scanEXE(strRootPath);
		}
	}
}

void dealProcess(char * lpPath)
{
	cout<<lpPath<<endl;
	char Md5String[33];
	bool find = false;
	md5Process(lpPath,Md5String);
	find = searchSignature(Md5String);
	if(find)
	{
		counter++;
		doFindProcess(lpPath);
	}
}

void md5Process(char * lpPath, char * Md5String)
{
	char asc[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	unsigned char * buffer;
	unsigned char digest[16];
	unsigned int len;
	MD5_CTX md5;
	ifstream inFile;
	inFile.open(lpPath,ios::in|ios::binary);
	if(!inFile)
	{
		cout<<"file open failed!"<<endl;
		return;
	}
	else
	{
		inFile.seekg(0,ios::end);
		len = inFile.tellg();
		if(len > 52428800)
		{
			inFile.close();
			return;
		}
		inFile.seekg(0);
		buffer = new unsigned char[len];
		inFile.read((char *)buffer,len);
		inFile.close();
	}
	md5.MD5Update(buffer,len);
	md5.MD5Final(digest);
	for(int j=0;j<16;j++)
	{		
		Md5String[2*j] = asc[digest[j]>>4];
		Md5String[2*j+1] = asc[digest[j]&0x0f];
	}
	Md5String[32] = '\0';
	delete buffer;
}

bool searchSignature(char * Md5String)
{
	char Signature[] = "3F6DDC443BE05FC52A345C820CB1B64A";  //更改此处为病毒签名即可
	if(_stricmp(Signature,Md5String) == 0)
		return true;
	else
		return false;
}

void doFindProcess(char * lpPath)
{
	int yn;
	cout<<"Really delete "<<lpPath<<" ?"<<endl;
	cin>>yn;
	if(yn)
		DeleteFile(lpPath);
	return;
}

