@echo off

if "%1" == "" (

	goto Usage

)

if "%1" == "/?" (

	goto Usage

)

if "%2" == "" (

	goto Usage

)

set path=%PATH%;"C:\Program Files (x86)\Atmel\Atmel Studio 6.2"

if EXIST "rebuildoutput.txt" (

del /Q rebuildoutput.txt

)

for /R "%1\Library" %%G IN (*.cproj) do (
	
	echo re-building %%G
	
	atmelstudio.exe %%G /rebuild %2 /out rebuildoutput.txt
) 

for /R "%1\App" %%G IN (*.cproj) do (
	
	echo re-building %%G
	
	atmelstudio.exe %%G /rebuild %2 /out rebuildoutput.txt
)

goto EOF

:Usage

echo.

echo rebuild [Source Code Base Path] [Project Configuration to Build - Debug or Release]

echo.

echo Example

echo.

echo rebuild C:\MySourceTree Debug 

echo.

:EOF