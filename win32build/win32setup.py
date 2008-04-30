##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from distutils.core import setup
import py2exe

import os
import sys

from Britefury.gSculptConfig import gSculptConfig

setup(
	name = 'gSculpt',
	description = 'The gSculpt procedural 3D modeller',
	version = gSculptConfig.gSculptVersionString,

	console = [
		{
			'script' : 'gSculpt.py',
			'icon_resources' : [ ( 1, 'win32build\\gSculpt_icon_by_Gary_Rose.ico' ) ]
		}
	],

	options = {
		'py2exe': {
			'packages' : 'encodings',
			'includes' : 'cairo, pango, pangocairo, atk, gobject, gtk.gdkgl',
		}
	}
)