// kill48KB.cpp : �������̨Ӧ�ó������ڵ㡣
/*
һ���ļ��в�����רɱ���ߣ��ܳ����ģ�����ͨ����д���������API�ģ�������visual studio 2010��windows7 7600�²���ͨ������ѧд��ʱ�����û�п�ģ�µ��ֳɵĴ��룬ϣ��������ЩҲ��ѧϰAPI����
*/
#include "stdafx.h"
#include<windows.h>
#include<stdio.h>
#include<TlHelp32.h>
#include"Psapi.h"
#include<string>
#include <stdlib.h>
#include<time.h>

//#define _WIN32_WINNT 0x0501
#define BUFSIZE MAX_PATH

using namespace std;

BOOL EnablePriv();  		//����Ȩ����������
BOOL TerminateProcessFromId(DWORD PID);	//���̽�������
BOOL KillProcess();		//ɨ�貢���Ҳ������̷���PID
VOID ShowInformation();	//��ӡ��Ϣ
BOOL CleanVirusFile();	//��������ļ�
BOOL FileCleaninDrive(LPSTR szDrive);	//������̸�Ŀ¼�µĲ���		
BOOL GetDirverInfo(LPSTR szDrive);	//��ȡ��������Ϣ
BOOL CleanRegister();		//���ע�����Ϣ
BOOL RepairTheDamage(LPSTR szDrive);	//�޸����������ĵ�����


int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleTitle("48KB-Killer");
	ShowInformation();				//�����Ϣ
	double RunTime[2]={0.0,0.0};
	RunTime[0]=(double)clock()/CLOCKS_PER_SEC;

	EnablePriv();				//��������Ȩ��
	KillProcess();				//��������
	CleanVirusFile();				//�������ĸ��������
	CleanRegister();				//�������������
	for(int i=0;i<argc-1;i++){
		RepairTheDamage(argv[i+1]); //���������ɨ����ָ̻��������޸ĵ�����
	}
	RunTime[1]=(double)clock()/CLOCKS_PER_SEC;
	printf("\n          *****************************************\n");
	printf("\nAll Finished , totally using time : %.3lf seconds\n",RunTime[1]-RunTime[0]);
	printf("Best wishes to you & your computer! #^_^\n");
	printf("\nYou can exit this program now!\n");
	printf("       Or it will exit itself in 60 seconds\n");
	Sleep(60000);					//�۲������Ϣ
	return 0;
}
void ShowInformation(){
	HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);     //��ȡ����̨���ھ��
	if(hConsole!=INVALID_HANDLE_VALUE){
		SetConsoleTextAttribute(hConsole,FOREGROUND_GREEN|FOREGROUND_INTENSITY);
	}			//����������ɫ�����ָ���
	printf("          *****************************************\n");
	printf("          *This program was written by Atp        *\n");
	printf("          *                     or QQ:344174246   *\n");
	printf("          *****************************************\n");
	//printf("\n���Թ���Ա������д˳���\n    ��������޷�ɾ������ĸ��!\n");
	printf("\nPlease run this program as a Administrator��\n       Or it may not clean the viruses!\n");
	//printf("���巽�������Ҽ��˵��е��\"�Թ���Ա�������\"~\n");
	printf("The way is right click the icon ,\n       then choose\"Run as a Administrator\" from the menu~\n");
	//����
	for(int i=0; i<3; i++)
	{
		if(i<2)
			 printf("%u seconds left to run!\n", 3-i);
		else printf("%u second  left to run!\n", 3-i);
		/*if(i==0)
			printf("Press Enter to continue.\n");
		if(!getchar()){
			break;
		}*/
		Sleep(1000);		// ÿ�����ӡһ��
	}
	printf("\n          * * * * * * * *  Start !  * * * * * * * *\n");
	printf("          *****************************************\n");
}

BOOL KillProcess(){
	    HANDLE hSnap = INVALID_HANDLE_VALUE;//���̿��վ��
		PROCESSENTRY32 pe;			//������Ϣ�ṹ   
		BOOL VirusFlag=false;		//��ǲ���������Ϣ
		BOOL bNext;
		pe.dwSize = sizeof(pe);
	    LPSTR Virus[3]={"��ȫ��ʿ.exe","ppsap.exe","iexplore.exe"};

		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);	//����һ�����̿���
		//��õ�һ�����̾�������óɹ�����TRUE�����򷵻�FALSE
		bNext = Process32First(hSnap,&pe);

		while(bNext){
			if(pe.th32ProcessID==0||pe.th32ProcessID==4){
				bNext = Process32Next(hSnap,&pe);
				continue;
			}//����System��System Idle Process����

			//�������̲���
			if(stricmp(Virus[0],pe.szExeFile)==0||stricmp(Virus[1],pe.szExeFile)==0||stricmp(Virus[2],pe.szExeFile)==0){   //�жϽ������Ƿ�Ϊ����������
					if(stricmp(Virus[2],pe.szExeFile)!=0){
						printf("	Find virus's process��%s PID��%d\n",pe.szExeFile,pe.th32ProcessID);//�������̺���
					}
					if(TerminateProcessFromId(pe.th32ProcessID))
					{
						printf("	Kill process %s successful��^_^!\n",pe.szExeFile);
					}
					else{
						printf("    Kill process %s failed~ -_-#\n",pe.szExeFile);
					}
					VirusFlag=true;
			}
			bNext = Process32Next(hSnap,&pe);
		}
		
		CloseHandle(hSnap); //�رս��̿���
		return VirusFlag;
}

BOOL CleanVirusFile(){
	TCHAR SystemPath[MAX_PATH];			
	ULONGLONG liFileSize;
	GetSystemDirectory(SystemPath,MAX_PATH);
	LPSTR VirusPath[8]={"\\��ȫ��ʿ.exe","\\ppsap.exe","C:\\Windows\\SysWOW64\\��ȫ��ʿ.exe","C:\\Windows\\SysWOW64\\ppsap.exe","C:\\WINDOWS\\system32\\��ȫ��ʿ.exe","C:\\WINDOWS\\system32\\ppsap.exe","C:\\Windows\\System32\\��ȫ��ʿ.exe","C:\\Windows\\System32\\ppsap.exe"};
	char* Virus[2]={"��ȫ��ʿ.exe","ppsap.exe"};  //C:\\Windows\\System32\\��ȫ��ʿ.exe
	WIN32_FILE_ATTRIBUTE_DATA FileData;

	for(int i=0;i<8;i++){
		if(i<2)
		lstrcat(VirusPath[i],SystemPath);

		GetFileAttributesExA(VirusPath[i],GetFileExInfoStandard,&FileData);		//��ȡ�ļ�����

		liFileSize=FileData.nFileSizeHigh;
		liFileSize <<= sizeof(DWORD)*8;
		liFileSize += FileData.nFileSizeLow;
		
		DWORD a;

		if(liFileSize==49152&&GetBinaryTypeA(VirusPath[i], &a)){
			SetFileAttributes(VirusPath[i],FileData.dwFileAttributes&~(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY));		//�����ļ�����
			int j=0;
			i%2==0?j=0:j=1;
			if(!DeleteFileA(VirusPath[i]))      //��Ҫ�Թ���Ա�������
			{
				//printf("File %s delete��\n",Virus[j]);
			}
			else
			{
				printf("    File %s delete success��\n",Virus[j]);
			}
		}
	}

	//*****��������Ѱ�����һ���Ϳ��ƶ�����
	CHAR szLogicalDriveStrings[BUFSIZE];		
	PCHAR szDrive;
	ZeroMemory(szLogicalDriveStrings,BUFSIZE);//��ʼ��������
	
	GetLogicalDriveStrings(BUFSIZE - 1,szLogicalDriveStrings);	// ��ȡ�߼������������
	szDrive = (PCHAR)szLogicalDriveStrings;

	// ѭ������ÿ����
	do
	{
		GetDirverInfo(szDrive);
		szDrive += (lstrlen(szDrive)+1);
	}
	while(*szDrive!='\x00');
	return true;
}

BOOL GetDirverInfo(LPSTR szDrive)
{
	UINT uDriveType;			
	uDriveType = GetDriveType(szDrive);			//��ȡ����������
	int FixDrivCount=0;

	switch(uDriveType){
	case DRIVE_REMOVABLE:
		FileCleaninDrive(szDrive);
		break;
	case DRIVE_FIXED:
		FileCleaninDrive(szDrive);
		break;
	case DRIVE_RAMDISK:
		FileCleaninDrive(szDrive);
		break;
	default:
		break;
	}
	return TRUE;
}

BOOL FileCleaninDrive(LPSTR szPath){
		CHAR szFilePath[MAX_PATH];

		//���������Ŀ¼���ļ���·�����ַ�����ʹ��ͨ�����*��
		lstrcpy(szFilePath, szPath);
		lstrcat(szFilePath, "\\*");

			WIN32_FIND_DATA FindFileData;
			HANDLE hListFile;
			//WIN32_FILE_ATTRIBUTE_DATA FileAttData;
			bool VirusFlag=false;
	
			//���ҵ�һ���ļ�/Ŀ¼����ò��Ҿ��
			hListFile = FindFirstFile(szFilePath,&FindFileData);
			
			if(hListFile==INVALID_HANDLE_VALUE)//�жϾ��
			{
				//printf("����%d",GetLastError());
				return 1;
			}
			else
			{		
				do
				{
					ULONGLONG liFileSize;
					CHAR szFullPath[MAX_PATH];
					DWORD a;
					lstrcpy(szFullPath, szPath);
					lstrcat(szFullPath, "\\");	

					//�����ļ���С
					lstrcat(szFullPath,FindFileData.cFileName);
					liFileSize=FindFileData.nFileSizeHigh;
					//��λ�ƶ�32λ
					liFileSize <<= sizeof(DWORD)*8;
					liFileSize += FindFileData.nFileSizeLow;
					if(liFileSize==49152&&GetBinaryTypeA(szFullPath, &a)){			//�ж��ļ���С�������Ƿ�Ϊ��ִ���ļ�
						SetFileAttributes(szFullPath,FindFileData.dwFileAttributes&~(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY));
						if(DeleteFile(szFullPath)){				//ɾ���ļ�
							printf("    File %s Delete Success!\n",FindFileData.cFileName);
						}
						VirusFlag=true;
					}		
				}
				while(FindNextFile(hListFile, &FindFileData));
				if(VirusFlag==true){
					RepairTheDamage(szPath);
				}
			}

	return true;
}

BOOL RepairTheDamage(LPSTR szPath){
		CHAR szFilePath[MAX_PATH];


		//���������Ŀ¼���ļ���·�����ַ�����ʹ��ͨ�����*��
		lstrcpy(szFilePath, szPath);
		lstrcat(szFilePath, "\\*");

		WIN32_FIND_DATA FindFileData;
		HANDLE hListFile;
		//WIN32_FILE_ATTRIBUTE_DATA FileAttData;
		bool VirusFlag=false;
	
		//���ҵ�һ���ļ�/Ŀ¼����ò��Ҿ��
		hListFile = FindFirstFile(szFilePath,&FindFileData);
			
		if(hListFile==INVALID_HANDLE_VALUE)//�жϾ��
		{
			//printf("����%d",GetLastError());
			return 1;
		}
		else
		{		
			do
			{
				CHAR szFullPath[MAX_PATH];
				lstrcpy(szFullPath, szPath);
				lstrcat(szFullPath, "\\");
				lstrcat(szFullPath,FindFileData.cFileName);

				//�޸��ļ����ԣ����أ�ϵͳ
				if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN&&FindFileData.dwFileAttributes&FILE_ATTRIBUTE_SYSTEM){		//�ж������Ƿ���ϵͳ����
					if(SetFileAttributes(szFullPath,FindFileData.dwFileAttributes&~(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY))){
						printf("    The arribuates of file %s has recovered to normal!\n",FindFileData.cFileName);
					}
				}
			}
			while(FindNextFile(hListFile, &FindFileData));
		}
	return true;
}

BOOL CleanRegister(){
		HKEY hKey=NULL;
		RegOpenKeyExA(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",0,KEY_ALL_ACCESS,&hKey);		//��ע�����
		if(!RegDeleteValue(hKey,"PPS Accelerator")){		//ɾ����ֵ
			printf("    The KEY has been repaired\n");
		}
		//else{
		//	printf("the KEY repairing failed\n");
		//}
	return true;
}

BOOL EnablePriv()//��������Ȩ��
{
    HANDLE hToken;
    if ( OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken) )
    {
	    TOKEN_PRIVILEGES tkp;

	    LookupPrivilegeValue( NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid );//�޸Ľ���Ȩ��
	    tkp.PrivilegeCount=1;
	    tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	    AdjustTokenPrivileges( hToken,FALSE,&tkp,sizeof tkp,NULL,NULL );//֪ͨϵͳ�޸Ľ���Ȩ��
	
		return( (GetLastError()==ERROR_SUCCESS) );
	}
}

BOOL TerminateProcessFromId(DWORD PID)	//�������̺���
{
	BOOL bReturn=FALSE;
	HANDLE hProcess=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,PID);
	if(hProcess!=NULL){		
		bReturn=::TerminateProcess(hProcess,0);
	}
	CloseHandle(hProcess);
	return bReturn;           
}