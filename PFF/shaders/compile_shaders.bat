@echo OFF
setlocal enabledelayedexpansion

echo ======== Compiling Shaders

REM Store the current directory in a variable
set "current_dir=%CD%"

REM Define an array of file extensions
set "extensions=.comp .frag .vert"

REM Loop through each file extension
for %%e in (%extensions%) do (
    REM Loop through files with the current extension in the current directory
    for %%f in ("%current_dir%\*%%e") do (

        set "comp_file=%%f"
        set "comp_spv_file=%%f.spv"

        if exist !comp_spv_file! (

            REM Get last modified times of .comp and .comp.spv files
            for %%g in ("!comp_file!") do ( set "comp_file_time=%%~tg" )
            for %%g in ("!comp_spv_file!") do ( set "comp_spv_file_time=%%~tg" )

            if !comp_file_time! GTR !comp_spv_file_time! (
                
                echo      found compiled version is older than file. Recompiling "!comp_file!" now
                @echo on
                "%current_dir%\..\vendor\vulkan-glslc\glslc.exe" "!comp_file!" -o "!comp_spv_file!"
                @echo off

            ) else ( 
                
                echo      found compiled version is up to date for file: "!comp_file!"
            )

        ) else (
            
            echo     no compiled version found. Compiling "!comp_file!" now
            @echo on
            "%current_dir%\..\vendor\vulkan-glslc\glslc.exe" "!comp_file!" -o "!comp_spv_file!"
            @echo off
        )

    )
)
