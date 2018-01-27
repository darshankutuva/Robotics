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

if EXIST "cleanoutput.txt" (

del /Q cleanoutput.txt

)

for /R "%1\Library" %%G IN (*.cproj) do (
	
	echo cleaning %%G
	
	atmelstudio.exe %%G /build %2 /out cleanoutput.txt
) 

for /R "%1\App" %%G IN (*.cproj) do (
	
	echo cleaning %%G
	
	atmelstudio.exe %%G /build %2 /out cleanoutput.txt
)

goto EOF

:Usage

echo.

echo clean [Source Code Base Path] [Project Configuration to Build - Debug or Release]

echo.

echo Example

echo.

echo clean C:\MySourceTree Debug 

echo.

:EOF