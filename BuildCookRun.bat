@REM I put this script directly in my project directory, next to the .uproject

@echo off

set PROJECT_DIR=%cd%

set ENGINE_DIR=C:\UnrealEngine\UE_5.1
set PROJECT_NAME=SimpleRacer
set BUILD_CONFIGURATION=Shipping
set ARCHIVE_DIRECTORY=%PROJECT_DIR%\Build_1


"%ENGINE_DIR%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project="%PROJECT_DIR%\%PROJECT_NAME%.uproject" -noP4 -platform=Win64 -clientconfig=%BUILD_CONFIGURATION% -build -cook -stage -pak -allmaps -CrashReporter -archive -archivedirectory="%ARCHIVE_DIRECTORY%"
