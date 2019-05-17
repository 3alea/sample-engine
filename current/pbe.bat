SET TARGET_NAME=aexmath
SET CONFIG=Release
SET SRC_DIR=.\extern\aexmath\lib
SET DEST_DIR=.\bin

:copy
if exist "%DEST_DIR%\%CONFIG%" (
	if exist "%SRC_DIR%\%TARGET_NAME%.dll" echo f | xcopy /f /y "%SRC_DIR%\%TARGET_NAME%.dll" "%DEST_DIR%\%CONFIG%\%TARGET_NAME%.dll"
)

:: Repeat for release, if already done -> end.
if %CONFIG% == Debug goto next_lib
SET CONFIG=Debug
if %TARGET_NAME% == glew32 SET TARGET_NAME=%TARGET_NAME%d
if %TARGET_NAME% == aexmath SET TARGET_NAME=%TARGET_NAME%_d

goto copy

:next_lib

::Repeast for GLEW
if %TARGET_NAME% == aexmath_d (
	SET	CONFIG=Release
	SET TARGET_NAME=glew32
	SET SRC_DIR=.\extern\GL
	goto:copy
)

:end