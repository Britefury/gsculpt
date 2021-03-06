##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os
import sys

from Britefury.gSculptConfig import gSculptConfig



iconsPath = None
possibleIconPaths = [ 'gsimages', os.path.join( sys.prefix, 'gsimages' ), os.path.join( sys.prefix, 'local', 'gsimages' ), os.path.join( sys.prefix, 'share', 'gSculpt', 'gsimages' ), os.path.join( sys.prefix, 'local', 'share', 'gSculpt', 'gsimages' ) ]

for _iconPath in possibleIconPaths:
	if os.path.isdir( _iconPath ):
		iconsPath = _iconPath
		break

if iconsPath is None:
	print 'Cannot find gsimages path'
	sys.exit()


gSculptIconPath = os.path.join( iconsPath, 'gSculpt_Glass_Master_by_Gary_Rose.png' )
gSculptSplashPath = os.path.join( iconsPath, 'gSculpt_splash_screen_by_Gary_Rose.png' )
gSculptSplashMouseOverPath = os.path.join( iconsPath, 'gSculpt_splash_screen_mouse_over_by_Gary_Rose.png' )


# Files in user's home directory
settingsPath = os.path.expanduser( os.path.join( '~', '.gsculpt' ) )
userConfigFilePath = os.path.join( settingsPath, 'gsculptuserconfig.cfg' )
shortcutsConfigPath = os.path.join( settingsPath, 'gsculptshortcuts.confxml' )
pluginsConfigPath = os.path.join( settingsPath, 'gsculptplugins.confxml' )
viewDrawingSettingsConfigPath = os.path.join( settingsPath, 'gsculptviewdrawingsettings.confxml' )





def getGSImagePath(filename):
	return os.path.join( iconsPath, filename )


def makeSettingsDir():
	if not os.path.exists( settingsPath ):
		os.mkdir( settingsPath )



