set PYTHONPATH=.
python win32build\win32setup.py py2exe
python win32build\finalwin32setup.py
python win32build\makewxs.py win32build\gSculpt.wxs
python win32build\makerunlight.py
cd win32build 
candle gSculpt.wxs
call runlight.bat
del runlight.bat
cd ..

