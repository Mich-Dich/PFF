@echo OFF
SETLOCAL EnableExtensions DisableDelayedExpansion
for /F %%a in ('echo prompt $E ^| cmd') do (
  set "ESC=%%a"
)
SETLOCAL EnableDelayedExpansion


echo.
echo ===============================================  Building PFF (Procedurally Focused Framework)  ===============================================
echo.

echo =================================== Clean previous compile result:
set files_to_delete=.\*.ilk .\*.obj .\*.pdb .\*.exe

echo  deleting               %files_to_delete%
del %files_to_delete%
echo.

echo =================================== Compile Project:
call vendor\premake\premake5.exe vs2022
echo.

echo =================================== Compile Result:
if %errorlevel% neq 0 (
    echo !ESC![31mFAILED!ESC![0m Premake5 encountered errors. !ESC![31mError count [%errorlevel%] Build aborted!ESC![0m
    exit /b %errorlevel%
) else (
    echo !ESC![32mNo errors found!ESC![0m
)

@echo on
PAUSE