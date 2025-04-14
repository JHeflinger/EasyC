@echo off
setlocal enabledelayedexpansion

:: total timing
set "t_startTime=%time: =0%"

:: initialize vars for building
set SRC_DIR=include
set INCLUDES=
set SOURCES=
set OBJECTS=
set LIBS=
set LINKS=

:: production build flags
set PROD=
if "%1"=="prod" (
    echo Optimizing for production build...
    set PROD=-O3 -DPROD_BUILD
)

:: create build directory if it does not exist
if NOT exist "build\" (
    mkdir build
)

:: create all build directories if it does not exist
cd build
if NOT exist "cache\" (
    mkdir cache
)
cd cache
if NOT exist "src\" (
    mkdir src
)
cd ..
cd ..

:: set up cache folders
for /d /r %SRC_DIR% %%D in (*) do (
    set SUBPATH=%%D
    set REL=!SUBPATH:%CD%\%SRC_DIR%=!
    set DESTDIR=build\cache\src!REL!
    if not exist "!DESTDIR!" (
        mkdir !DESTDIR!
    )
)

:: get includes
for /r %SRC_DIR% %%d in (.) do (
    set INCLUDES=!INCLUDES! -I"%%d"
)

:: compile obj files
echo Compiling sources...
set SOURCES_UP_TO_DATE="true"
set FOUND_MAIN="false"
set "startTime=%time: =0%"
for /r %SRC_DIR% %%f in (*.c) do (
    set SUBPATH=%%f
    set REL=!SUBPATH:%CD%\%SRC_DIR%=!
    if "%%~nxf" NEQ "main.c" (
        if NOT exist "build\cache\src!REL!.o" (
            set SOURCES_UP_TO_DATE="false"
            echo - [%%~nxf] [33m^(compiling...^)[0m
            gcc -Wall -Wextra -Wno-unused-parameter -c %%f%INCLUDES%%LIBS%%LINKS% -o build\cache\src!REL!.o
            if !ERRORLEVEL! NEQ 0 (
                echo Building source "%%~nxf" [31mFailed[0m with error code !ERRORLEVEL!
                exit /b !ERRORLEVEL!
            )
            echo [1A[0K- [%%~nxf] [32mOK[0m
            copy /y %%f build\cache\src!REL! >nul
        ) else (
            fc %%f "build\cache\src!REL!" >nul
            if !ERRORLEVEL! NEQ 0 (
                set SOURCES_UP_TO_DATE="false"
                echo - [%%~nxf] [33m^(compiling...^)[0m
                gcc -Wall -Wextra -Wno-unused-parameter -c %%f%INCLUDES%%LIBS%%LINKS% -o build\cache\src!REL!.o
                if !ERRORLEVEL! NEQ 0 (
                    echo Building source "%%~nxf" [31mFailed[0m with error code !ERRORLEVEL!
                    exit /b !ERRORLEVEL!
                )
                echo [1A[0K- [%%~nxf] [32mOK[0m
                copy /y %%f build\cache\src!REL! >nul
            )
        )
        set OBJECTS=!OBJECTS! build\cache\src!REL!.o
    ) else (
        set FOUND_MAIN="true"
    )
)
if %SOURCES_UP_TO_DATE%=="true" (
    echo [1A[0KSources are currently [32mup to date[0m
) else (
    set "endTime=%time: =0%"
    set "end=!endTime:%time:~8,1%=%%100)*100+1!"  &  set "start=!startTime:%time:~8,1%=%%100)*100+1!"
    set /A "elap=((((10!end:%time:~2,1%=%%100)*60+1!%%100)-((((10!start:%time:~2,1%=%%100)*60+1!%%100), elap-=(elap>>31)*24*60*60*100"
    set /A "cc=elap%%100+100,elap/=100,ss=elap%%60+100,elap/=60,mm=elap%%60+100,hh=elap/60+100"
    echo [32mFinished[0m compiling sources in !hh:~1!!time:~2,1!!mm:~1!!time:~2,1!!ss:~1!!time:~8,1!!cc:~1!
)

:: compile executable
echo Building executable...
set "startTime=%time: =0%"
gcc -Wall -Wextra -Wno-unused-parameter src/main.c%OBJECTS%%INCLUDES%%LIBS%%LINKS% -o build/a.out.exe %PROD%
if !ERRORLEVEL! NEQ 0 (
    echo Build [31mFailed[0m with error code !ERRORLEVEL!
    exit /b !ERRORLEVEL!
)
set "endTime=%time: =0%"
set "end=!endTime:%time:~8,1%=%%100)*100+1!"  &  set "start=!startTime:%time:~8,1%=%%100)*100+1!"
set /A "elap=((((10!end:%time:~2,1%=%%100)*60+1!%%100)-((((10!start:%time:~2,1%=%%100)*60+1!%%100), elap-=(elap>>31)*24*60*60*100"
set /A "cc=elap%%100+100,elap/=100,ss=elap%%60+100,elap/=60,mm=elap%%60+100,hh=elap/60+100"
echo [32mFinished[0m building executable in %hh:~1%%time:~2,1%%mm:~1%%time:~2,1%%ss:~1%%time:~8,1%%cc:~1%
set "endTime=%time: =0%"
set "end=!endTime:%time:~8,1%=%%100)*100+1!"  &  set "start=!t_startTime:%time:~8,1%=%%100)*100+1!"
set /A "elap=((((10!end:%time:~2,1%=%%100)*60+1!%%100)-((((10!start:%time:~2,1%=%%100)*60+1!%%100), elap-=(elap>>31)*24*60*60*100"
set /A "cc=elap%%100+100,elap/=100,ss=elap%%60+100,elap/=60,mm=elap%%60+100,hh=elap/60+100"
echo [32mFinished[0m total build in %hh:~1%%time:~2,1%%mm:~1%%time:~2,1%%ss:~1%%time:~8,1%%cc:~1%
