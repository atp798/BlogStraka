@echo off
echo ***********************************
echo   This program is edited by atp
echo.           @��ѧԺһ��
echo         phone:88557(����)
echo ***********************************
echo. 
echo Now let's begin to clean the virus!
pause

taskkill /f /im iexplore.exe /im ppsap.exe /im ��ȫ��ʿ.exe /t 
if exist C:\Windows\System32\ppsap.exe (del C:\Windows\System32\ppsap.exe & echo ppsap.exe has been cleaned) else echo ppsap.exe missing
if exist C:\Windows\System32\��ȫ��ʿ.exe (del C:\Windows\System32\��ȫ��ʿ.exe & echo ��ȫ��ʿ.exe has been cleaned) else echo ppsap.exe missing
echo ***********************************
echo         Congratulations!
echo    This virus has been cleaned!
echo ***********************************
pause

setlocal enabledelayedexpansion
echo ***********************************
echo  Now start to change the attribute!
echo ***********************************

if "%~1"=="" (
echo ��û�������κδ��̻��ļ����ļ���
echo.
echo ���Ҫ����Ĵ��̻��ļ����ļ���
echo �ϵ��������ļ�ͼ���л��Զ�ִ��
echo.
echo    ��Ҫ˫������!
echo.
pause
)
:loop
if "%~1"=="" goto :eof
if not exist "%~1" (echo ·��������)&pause&shift&goto loop
echo ��ǰ����Ĵ��̻��ļ����ļ���:
echo %~1
echo.
echo ��ѡ��Ҫ���еĲ�����Ȼ�󰴻س�
echo.
echo ��ϵͳ���������ԡ���������������������1
echo ȥϵͳ���������ԡ���������������������2
echo ������������������������������������3
echo.
set /p "c1=ѡ�� (1,2,3) (������Ϊ!c1!,Ĭ��Ϊ2):"
if "!c1!"=="1" (set "d=+") else if "!c1!"=="3" (pause&shift&goto loop) else set "d=-"
set "a=%~a1"
if /i "!a:~0,1!"=="d" (
echo.
echo ����һ���ļ��У���ѡ��Ҫ���еĲ�����Ȼ�󰴻س�
echo.
echo ֻ�Ը��ļ��б�����������������������1
echo ֻ�Ը��ļ����е��ļ��к��ļ�����������2
echo �Ը�Ŀ¼�µ�һ�е��ļ��к��ļ���������3
echo.
set /p "c2=ѡ�� (1,2,3) (������Ϊ!c2!,Ĭ��Ϊ1):"
if "!c2!"=="2" (
pushd "%~1"
for /f "delims=" %%i in ('dir /a /b "%~1"') do attrib.exe !d!s !d!h "%%i"
popd
) else if "!c2!"=="3" (
for /f "delims=" %%i in ('dir /a /b /s "%~1"') do attrib.exe !d!s !d!h "%%i"
) else (
attrib.exe !d!s !d!h "%~1"
)
) else (
attrib.exe !d!s !d!h "%~1"
)
echo ***********************************
echo         Congratulations!
echo All the operations have been done!
echo        Thanks for using !
echo ***********************************
pause&shift&goto loop

