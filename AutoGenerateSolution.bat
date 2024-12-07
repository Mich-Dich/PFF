@echo OFF
SETLOCAL EnableExtensions DisableDelayedExpansion
for /F %%a in ('echo prompt $E ^| cmd') do (
  set "ESC=%%a"
)
SETLOCAL EnableDelayedExpansion


echo.
echo ======== Setup windows icon
echo.

echo -------- Setup PFF-Project file icon
python scripts/windows_icon_setup.py

echo.
echo ======== Building PFF (Procedurally Focused Framework)
echo.

echo -------- Compile VS 2022 Solution:
call vendor\premake\premake5.exe vs2022
echo.

echo -------- Compile Result:
if %errorlevel% neq 0 (
  
  echo !ESC![31mBUILD FAILED!ESC![0m the premake script encountered [%errorlevel%] errors
  @echo on
  PAUSE

) else (
  echo !ESC![32mBUILD SUCCESSFULL!ESC![0m
)

@echo on
PAUSE
