@ECHO OFF
REM -- Automates cygwin installation

SETLOCAL

REM -- Change to the directory of the executing batch file
CD %~dp0

REM -- Configure our paths
SET SITE=http://mirrors.kernel.org/sourceware/cygwin/
SET LOCALDIR=%LOCALAPPDATA%/cygwin
SET ROOTDIR=C:/cygwin64

REM -- These are the packages we will install (in addition to the default packages)
SET PACKAGES=mintty,wget,ctags,diffutils,git,git-completion,git-svn,mercurial
SET PACKAGES=%PACKAGES%,gcc-core,make,automake,autoconf,readline,libncursesw-devel,libiconv,zlib-devel,gettext
SET PACKAGES=%PACKAGES%,lua,python,ruby
SET PACKAGES=%PACKAGES%,vim,nano,gcc-g++
REM -- graphics
SET PACKAGES=%PACKAGES%,libnetpbm-devel,libnetpbm10,netpbm
SET PACKAGES=%PACKAGES%,libpng16,libpng16-devel,mingw64-x86_64-libpng

REM --create missing folders and download cygwin installer
ECHO [INFO] Downloading cygwin setup.exe
mkdir C:\Temp\cygwindownload\
Cscript.exe getWebFile1.vbs "https://cygwin.com/setup-x86_64.exe" "C:\Temp\cygwindownload\setup-x86_64.exe"
if exist C:\Temp\cygwindownload\setup-x86_64.exe (
   echo [INFO]: Cygwin setup was downloaded
) else (
   echo [FATAL]: Cygwin setup was not downloaded
   goto end
)

REM -- Do it!
ECHO *** INSTALLING PACKAGES
C:\Temp\cygwindownload\setup-x86_64.exe -q -D -L -d -g -o -s %SITE% -l "%LOCALDIR%" -R "%ROOTDIR%" -C Base -P %PACKAGES%

REM -- Show what we did
ECHO.
ECHO.
ECHO cygwin installation updated
ECHO  - %PACKAGES%
ECHO.

REM -- Install apt-cyg
c:\cygwin64\bin\bash.exe --norc --noprofile -c "cd /bin/; /bin/wget http://rawgit.com/transcode-open/apt-cyg/master/apt-cyg; /bin/chmod +x apt-cyg"

ENDLOCAL

PAUSE
EXIT /B 0
