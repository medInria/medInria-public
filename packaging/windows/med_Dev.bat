@echo off

set script_path=%~dp0
set script_path=%script_path:~0,-1%
set MED_VS_FOUND=

for /d %%a in ("%systemdrive%\Program Files (x86)\Microsoft Visual Studio*") do (
    for /d %%x in ("%%a\*") do (
        if defined MED_VS_FOUND ( @echo off ) else (
            if exist "%%x\Community\VC\Auxiliary\Build\vcvarsall.bat" (
               @call "%%x\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
               @echo off            
               set MED_VS_FOUND=y
            )
        )
    )
)

if defined MED_VS_FOUND ( @echo off ) else (
    for /d %%a in ("%programfiles%\Microsoft Visual Studio*") do (
        for /d %%x in ("%%a\*") do (
            if defined MED_VS_FOUND ( @echo off ) else (
                if exist "%%x\Community\VC\Auxiliary\Build\vcvarsall.bat" (
                   @call "%%x\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
                   @echo off            
                   set MED_VS_FOUND=y
                )
            )
        )
    )
)


set PATH=%script_path%\dependencies\dtk\lib;%script_path%\dependencies\ITK\bin;%script_path%dependencies\ITK\lib;%script_path%\dependencies\TTK\bin;%script_path%dependencies\TTK\lib;%script_path%\dependencies\VTK\bin;%script_path%dependencies\VTK\lib;%script_path%\bin;%script_path%\bin\plugins;%script_path%\bin\plugins_legacy;%PATH%

cd "%script_path%\bin"
start .\medInria.exe"
start devenv.exe 
cmd /k
exit
