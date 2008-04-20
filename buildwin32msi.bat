python win32build\win32setup.py py2exe --exclude=OpenGL
python win32build\finalwin32setup.py
python win32build\makewxs.py win32build\gSculpt.wxs
cd win32build 
candle gSculpt.wxs 
light -out gSculpt.msi gSculpt.wixobj \wix\wixui.wixlib -loc \wix\WixUI_en-us.wxl
cd ..

