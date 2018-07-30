// kill48KB.cpp : 定义控制台应用程序的入口点。
/*
一个文件夹病毒的专杀工具，很初级的，我是通过编写这个才入门API的，代码在visual studio 2010，windows7 7600下测试通过，我学写的时候苦于没有可模仿的现成的代码，希望帮助那些也想学习API的人
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

BOOL EnablePriv();  		//进程权限提升函数
BOOL TerminateProcessFromId(DWORD PID);	//进程结束函数
BOOL KillProcess();		//扫描并查找病毒进程返回PID
VOID ShowInformation();	//打印信息
BOOL CleanVirusFile();	//清除病毒文件
BOOL FileCleaninDrive(LPSTR szDrive);	//清除磁盘根目录下的病毒		
BOOL GetDirverInfo(LPSTR szDrive);	//获取驱动器信息
BOOL CleanRegister();		//清除注册表信息
BOOL RepairTheDamage(LPSTR szDrive);	//修复被病毒更改的属性


int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleTitle("48KB-Killer");
	ShowInformation();				//输出信息
	double RunTime[2]={0.0,0.0};
	RunTime[0]=(double)clock()/CLOCKS_PER_SEC;

	EnablePriv();				//提升进程权限
	KillProcess();				//结束进程
	CleanVirusFile();				//清除病毒母本及副本
	CleanRegister();				//清除病毒启动项
	for(int i=0;i<argc-1;i++){
		RepairTheDamage(argv[i+1]); //按传入参数扫描磁盘恢复被病毒修改的属性
	}
	RunTime[1]=(double)clock()/CLOCKS_PER_SEC;
	printf("\n          *****************************************\n");
	printf("\nAll Finished , totally using time : %.3lf seconds\n",RunTime[1]-RunTime[0]);
	printf("Best wishes to you & your computer! #^_^\n");
	printf("\nYou can exit this program now!\n");
	printf("       Or it will exit itself in 60 seconds\n");
	Sleep(60000);					//观察输出信息
	return 0;
}
void ShowInformation(){
	HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);     //获取控制台窗口句柄
	if(hConsole!=INVALID_HANDLE_VALUE){
		SetConsoleTextAttribute(hConsole,FOREGROUND_GREEN|FOREGROUND_INTENSITY);
	}			//设置文字颜色和文字高亮
	printf("          *****************************************\n");
	printf("          *This program was written by Atp        *\n");
	printf("          *                     or QQ:344174246   *\n");
	printf("          *****************************************\n");
	//printf("\n请以管理员身份运行此程序，\n    否则可能无法删除病毒母本!\n");
	printf("\nPlease run this program as a Administrator，\n       Or it may not clean the viruses!\n");
	//printf("具体方法是在右键菜单中点击\"以管理员身份运行\"~\n");
	printf("The way is right click the icon ,\n       then choose\"Run as a Administrator\" from the menu~\n");
	//倒数
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
		Sleep(1000);		// 每毫秒打印一次
	}
	printf("\n          * * * * * * * *  Start !  * * * * * * * *\n");
	printf("          *****************************************\n");
}

BOOL KillProcess(){
	    HANDLE hSnap = INVALID_HANDLE_VALUE;//进程快照句柄
		PROCESSENTRY32 pe;			//进程信息结构   
		BOOL VirusFlag=false;		//标记病毒存在信息
		BOOL bNext;
		pe.dwSize = sizeof(pe);
	    LPSTR Virus[3]={"安全卫士.exe","ppsap.exe","iexplore.exe"};

		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);	//建立一个进程快照
		//获得第一个进程句柄，调用成功返回TRUE，否则返回FALSE
		bNext = Process32First(hSnap,&pe);

		while(bNext){
			if(pe.th32ProcessID==0||pe.th32ProcessID==4){
				bNext = Process32Next(hSnap,&pe);
				continue;
			}//忽略System和System Idle Process进程

			//结束进程部分
			if(stricmp(Virus[0],pe.szExeFile)==0||stricmp(Virus[1],pe.szExeFile)==0||stricmp(Virus[2],pe.szExeFile)==0){   //判断进程名是否为病毒进程名
					if(stricmp(Virus[2],pe.szExeFile)!=0){
						printf("	Find virus's process：%s PID：%d\n",pe.szExeFile,pe.th32ProcessID);//结束进程函数
					}
					if(TerminateProcessFromId(pe.th32ProcessID))
					{
						printf("	Kill process %s successful！^_^!\n",pe.szExeFile);
					}
					else{
						printf("    Kill process %s failed~ -_-#\n",pe.szExeFile);
					}
					VirusFlag=true;
			}
			bNext = Process32Next(hSnap,&pe);
		}
		
		CloseHandle(hSnap); //关闭进程快照
		return VirusFlag;
}

BOOL CleanVirusFile(){
	TCHAR SystemPath[MAX_PATH];			
	ULONGLONG liFileSize;
	GetSystemDirectory(SystemPath,MAX_PATH);
	LPSTR VirusPath[8]={"\\安全卫士.exe","\\ppsap.exe","C:\\Windows\\SysWOW64\\安全卫士.exe","C:\\Windows\\SysWOW64\\ppsap.exe","C:\\WINDOWS\\system32\\安全卫士.exe","C:\\WINDOWS\\system32\\ppsap.exe","C:\\Windows\\System32\\安全卫士.exe","C:\\Windows\\System32\\ppsap.exe"};
	char* Virus[2]={"安全卫士.exe","ppsap.exe"};  //C:\\Windows\\System32\\安全卫士.exe
	WIN32_FILE_ATTRIBUTE_DATA FileData;

	for(int i=0;i<8;i++){
		if(i<2)
		lstrcat(VirusPath[i],SystemPath);

		GetFileAttributesExA(VirusPath[i],GetFileExInfoStandard,&FileData);		//获取文件属性

		liFileSize=FileData.nFileSizeHigh;
		liFileSize <<= sizeof(DWORD)*8;
		liFileSize += FileData.nFileSizeLow;
		
		DWORD a;

		if(liFileSize==49152&&GetBinaryTypeA(VirusPath[i], &a)){
			SetFileAttributes(VirusPath[i],FileData.dwFileAttributes&~(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY));		//设置文件属性
			int j=0;
			i%2==0?j=0:j=1;
			if(!DeleteFileA(VirusPath[i]))      //需要以管理员身份运行
			{
				//printf("File %s delete：\n",Virus[j]);
			}
			else
			{
				printf("    File %s delete success！\n",Virus[j]);
			}
		}
	}

	//*****遍历磁盘寻找最后一个和可移动磁盘
	CHAR szLogicalDriveStrings[BUFSIZE];		
	PCHAR szDrive;
	ZeroMemory(szLogicalDriveStrings,BUFSIZE);//初始化缓冲区
	
	GetLogicalDriveStrings(BUFSIZE - 1,szLogicalDriveStrings);	// 获取逻辑驱动器卷标名
	szDrive = (PCHAR)szLogicalDriveStrings;

	// 循环处理每个卷
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
	uDriveType = GetDriveType(szDrive);			//获取驱动器类型
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

		//构造代表子目录和文件夹路径的字符串，使用通配符“*”
		lstrcpy(szFilePath, szPath);
		lstrcat(szFilePath, "\\*");

			WIN32_FIND_DATA FindFileData;
			HANDLE hListFile;
			//WIN32_FILE_ATTRIBUTE_DATA FileAttData;
			bool VirusFlag=false;
	
			//查找第一个文件/目录，获得查找句柄
			hListFile = FindFirstFile(szFilePath,&FindFileData);
			
			if(hListFile==INVALID_HANDLE_VALUE)//判断句柄
			{
				//printf("错误：%d",GetLastError());
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

					//计算文件大小
					lstrcat(szFullPath,FindFileData.cFileName);
					liFileSize=FindFileData.nFileSizeHigh;
					//高位移动32位
					liFileSize <<= sizeof(DWORD)*8;
					liFileSize += FindFileData.nFileSizeLow;
					if(liFileSize==49152&&GetBinaryTypeA(szFullPath, &a)){			//判断文件大小和类型是否为可执行文件
						SetFileAttributes(szFullPath,FindFileData.dwFileAttributes&~(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY));
						if(DeleteFile(szFullPath)){				//删除文件
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


		//构造代表子目录和文件夹路径的字符串，使用通配符“*”
		lstrcpy(szFilePath, szPath);
		lstrcat(szFilePath, "\\*");

		WIN32_FIND_DATA FindFileData;
		HANDLE hListFile;
		//WIN32_FILE_ATTRIBUTE_DATA FileAttData;
		bool VirusFlag=false;
	
		//查找第一个文件/目录，获得查找句柄
		hListFile = FindFirstFile(szFilePath,&FindFileData);
			
		if(hListFile==INVALID_HANDLE_VALUE)//判断句柄
		{
			//printf("错误：%d",GetLastError());
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

				//修复文件属性：隐藏，系统
				if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN&&FindFileData.dwFileAttributes&FILE_ATTRIBUTE_SYSTEM){		//判断属性是否是系统隐藏
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
		RegOpenKeyExA(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",0,KEY_ALL_ACCESS,&hKey);		//打开注册表项
		if(!RegDeleteValue(hKey,"PPS Accelerator")){		//删除键值
			printf("    The KEY has been repaired\n");
		}
		//else{
		//	printf("the KEY repairing failed\n");
		//}
	return true;
}

BOOL EnablePriv()//提升进程权限
{
    HANDLE hToken;
    if ( OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken) )
    {
	    TOKEN_PRIVILEGES tkp;

	    LookupPrivilegeValue( NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid );//修改进程权限
	    tkp.PrivilegeCount=1;
	    tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	    AdjustTokenPrivileges( hToken,FALSE,&tkp,sizeof tkp,NULL,NULL );//通知系统修改进程权限
	
		return( (GetLastError()==ERROR_SUCCESS) );
	}
}

BOOL TerminateProcessFromId(DWORD PID)	//结束进程函数
{
	BOOL bReturn=FALSE;
	HANDLE hProcess=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,PID);
	if(hProcess!=NULL){		
		bReturn=::TerminateProcess(hProcess,0);
	}
	CloseHandle(hProcess);
	return bReturn;           
}