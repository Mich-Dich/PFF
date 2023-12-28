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
set del_file_ILK=.\*.ilk
set del_file_OBJ=.\*.obj
set del_file_PDB=.\*.pdb
set del_file_EXE=.\*.exe

echo  deleting               %del_file_ILK% %del_file_OBJ% %del_file_PDB% %del_file_EXE% 
del %del_file_ILK%
del %del_file_OBJ%
del %del_file_PDB%
del %del_file_EXE%
echo.

echo =================================== Compile VS 2022 Solution:
call vendor\premake\premake5.exe vs2022
echo.

echo =================================== Compile Result:
if %errorlevel% neq 0 (
    echo !ESC![31mFAILED!ESC![0m Premake5 encountered errors. !ESC![31mError count [%errorlevel%] Build aborted!ESC![0m
) else (
    echo !ESC![32mNo errors found!ESC![0m
)

@echo on
PAUSE