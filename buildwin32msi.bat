REM python win32build\win32setup.py py2exe --exclude=OpenGL
python win32build\win32setup.py py2exe
python win32build\finalwin32setup.py
REM python win32build\makewxs.py win32build\gSculpt.wxs
REM cd win32build 
REM candle gSculpt.wxs 
REM light -out gSculpt.msi gSculpt.wixobj \wix\wixui.wixlib -loc \wix\WixUI_en-us.wxl
REM cd ..

