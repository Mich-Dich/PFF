@echo off
SET "currentDir=%CD%"

REM Create the bin-int directory if it doesn't exist
IF NOT EXIST "%currentDir%\bin-int" (
    mkdir "%currentDir%\bin-int"
)

REM Change directory to bin-int
cd "%currentDir%\bin-int"

REM Run cmake configure
cmake .. 
IF %ERRORLEVEL% EQU 0 (
    REM Run cmake build only if cmake configure succeeded
    cmake --build .
) ELSE (
    echo CMake configure failed, skipping build step.
)

REM Change back to the original directory
cd "%currentDir%"
