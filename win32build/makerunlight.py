##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os

from Britefury.gSculptConfig import gSculptConfig


open( os.path.join( 'win32build', 'runlight.bat' ), 'w' ).writelines( [ 'light -out gSculpt-%s-win32.msi gSculpt.wixobj C:\Software\Wix\wixui.wixlib -loc C:\Software\Wix\WixUI_en-us.wxl'  %  ( gSculptConfig.gSculptVersionString, ) ] )
