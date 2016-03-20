@echo off
::-----------------------------------------------------------------------------
:: Paths
::-----------------------------------------------------------------------------
set SCRIPT_DIR=%~dp0%
set ProjectHome=%SCRIPT_DIR:~0,-1%
set WorkspaceDir=%ProjectHome%\workspace
set ArduinoTools=%ProjectHome%\..\Tools
set ThisProjTools=%ProjectHome%\tools

::-----------------------------------------------------------------------------
:: Load Script Environment / Configuration
::-----------------------------------------------------------------------------
for /f "delims=" %%x in (%ProjectHome%\env.config) do (set "%%x")


::-----------------------------------------------------------------------------
:: Configured Settings
::-----------------------------------------------------------------------------
:: Arduino IDE (see http://arduino.cc/en/Main/OldSoftwareReleases) 
::set ArduinoDownloadUrl=http://arduino.cc/download.php?f=
::set ArduinoVer=1.5.6-r2 ::to be set in env.config

:: Eclipse Arduino Workbench Bundle nightly build (see http://www.baeyens.it/eclipse/download.php)
::set EclipseArduinoDownloadUrl=http://www.baeyens.it/eclipse/download/product
::set EclipseArduinoRel=
::set EclipseArduinoVer=2015-01-13_02-06-58 ::to be set in env.config
::set EclipseArduinoDownloadUrlTail=
:: Bundle Release V2.4: http://eclipse.baeyens.it/download/product/V2.4_win64.2015-12-06_20-24-02.tar.gz?forcedownload
::set EclipseArduinoDownloadUrl=http://eclipse.baeyens.it/download/product
::set EclipseArduinoRel=V2.4_
::set EclipseArduinoVer=2015-12-06_20-24-02
::set EclipseArduinoDownloadUrlTail=?forcedownload

:: Expected Project Location (Eclipse CDT cannot deal with relative paths)
:: set ArduinoProjects=C:\git\arduino-projects ::to be set in env.config
set ExpectedProjectHome=%ArduinoProjects%\%ProjectSubDir%

::-----------------------------------------------------------------------------
:: Get the OS Variant
::-----------------------------------------------------------------------------
IF "%PROCESSOR_ARCHITECTURE%;%PROCESSOR_ARCHITEW6432%"=="x86;" (
  set OsVariant=win32
) ELSE (
  set OsVariant=win64
)

::-----------------------------------------------------------------------------
:: Assert correct path
::-----------------------------------------------------------------------------
if not "%ExpectedProjectHome%"=="%ProjectHome%" (
  msg "%username%" Please install this project %ProjectHome% here: %ExpectedProjectHome% - otherwise you would not be able to compile.
  if not exist "%ArduinoProjects%" (
    md "%ArduinoProjects%"
  )
  goto end
)


::-----------------------------------------------------------------------------
:: Set the tools' paths
::-----------------------------------------------------------------------------
:: Arduino IDE
set ArduinoRevs=%ArduinoTools%\arduino_revs
set CurArduino=%ArduinoRevs%\arduino-%ArduinoVer%

:: Eclipse Arduino Workbench Bundle
set EclipseArduinoRevs=%ArduinoTools%\eclipseArduino_revs
set CurEclipseArduinoVerName=%EclipseArduinoRel%%OsVariant%.%EclipseArduinoVer%
set CurEclipseArduino=%EclipseArduinoRevs%\%CurEclipseArduinoVerName%\eclipseArduino

:: 7Zip
set Archiver=%ThisProjTools%\7za920\7za.exe

:: wget
set Wget=%ThisProjTools%\wget\wget.exe

:: git
if "%OsVariant%"=="win32" (
  set Git="%ProgramFiles%\Git\bin\git.exe"
) else (
  set Git="%ProgramFiles(x86)%\Git\bin\git.exe"
)

::-----------------------------------------------------------------------------
:: Assert untouched .project file
::-----------------------------------------------------------------------------
:: ensure that src/.project has not been changed
set statusResult=
for /f "delims=" %%a in ('%%Git%% status --porcelain %%ProjectHome%%\src\.project') do @set statusResult=%%a
echo "%statusResult%"
if not "%statusResult%"=="" (
  msg "%username%" The file %ProjectHome%\src\.project is already touched. This script shall only be run on a vanilla project just cloned before.
  goto end
)

::-----------------------------------------------------------------------------
:: Prepare org.eclipse.cdt.core.prefs file from template
::-----------------------------------------------------------------------------
:: Eclipse Arduino IDE applies changes to org.eclipse.cdt.core.prefs, copy from template before
copy %ProjectHome%\src\.settings\org.eclipse.cdt.core.prefs-template %ProjectHome%\src\.settings\org.eclipse.cdt.core.prefs

::-----------------------------------------------------------------------------
:: Get the tools
::-----------------------------------------------------------------------------
:: Arduino IDE
if not exist "%CurArduino%" (
  if not exist "%ArduinoRevs%" (
    md "%ArduinoRevs%"
  )
  if not exist "%ArduinoRevs%\arduino-%ArduinoVer%-windows.zip" (
    %Wget% --tries=0 --output-document="%ArduinoRevs%\arduino-%ArduinoVer%-windows.zip" "%ArduinoDownloadUrl%/arduino-%ArduinoVer%-windows.zip"
  )
  %Archiver% x -y -o%ArduinoRevs% %ArduinoRevs%\arduino-%ArduinoVer%-windows.zip
  if %errorlevel% == 0 (
    del %ArduinoRevs%\arduino-%ArduinoVer%-windows.zip
  )
)
:: create softlink (junction) for Arduino IDE in current project tools
rmdir %ThisProjTools%\arduino
mklink /J %ThisProjTools%\arduino %CurArduino%

:: Eclipse Arduino Workbench Bundle
if not exist "%CurEclipseArduino%" (
  if not exist "%EclipseArduinoRevs%" (
    md "%EclipseArduinoRevs%"
  )
  if not exist "%EclipseArduinoRevs%\%OsVariant%.%EclipseArduinoVer%.tar.gz" (
    %Wget% --tries=0 --output-document="%EclipseArduinoRevs%\%CurEclipseArduinoVerName%.tar.gz" "%EclipseArduinoDownloadUrl%/%CurEclipseArduinoVerName%.tar.gz%EclipseArduinoDownloadUrlTail%"
  )
  %Archiver% x -y -o%EclipseArduinoRevs% %EclipseArduinoRevs%\%CurEclipseArduinoVerName%.tar.gz
  %Archiver% x -y -o%EclipseArduinoRevs%\%CurEclipseArduinoVerName% %EclipseArduinoRevs%\%CurEclipseArduinoVerName%.tar
  if %errorlevel% == 0 (
    del %EclipseArduinoRevs%\%CurEclipseArduinoVerName%.tar.gz
	del %EclipseArduinoRevs%\%CurEclipseArduinoVerName%.tar
  )
)
:: create softlink (junction) for Eclipse Arduino Workbench Bundle in current project tools
rmdir %ThisProjTools%\eclipseArduino
mklink /J %ThisProjTools%\eclipseArduino %CurEclipseArduino%

:: unpack Eclipse metadata in workspace
if not exist "%WorkspaceDir%\.metadata" (
  %Archiver% x -y -o%WorkspaceDir% "%WorkspaceDir%\.metadata.zip"
)

::-----------------------------------------------------------------------------
:: Perform Workaround Eclipse Arduino Bug (removed Library links in .project file after first [failing] build)
::-----------------------------------------------------------------------------
:: run first build, would fail
call build.bat
if %errorlevel% == 0 goto end
:: revert src/.project that have been made dirty by the failing build
%Git% checkout -- %ProjectHome%\src\.project

:: run second build
call build.bat
if %errorlevel% == 0 goto end

:error
msg "%username%" An error occured!
::pause

:end
