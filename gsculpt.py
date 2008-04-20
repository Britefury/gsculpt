##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
#!/usr/bin/env python

import gc
import sys
import getopt

import pygtk
if sys.platform == 'win32':
	# The call to pygtk.require() is not necessary on the win32 platform.
	# Additionally, it does not work when pygtk is bundled by py2exe.

	# Since pygtk.require() is called throughout gSculpt, replace it with a blank function
	def blank(version):
		pass
	pygtk.require = blank
pygtk.require( '2.0' )
import gtk
import gobject



from Britefury.I18n import i18n
i18n.initialise()


from Britefury.Config import GSConfig
from Britefury.gSculptConfig import gSculptConfig, gSculptUserConfig





#
#
#
# PROCESS COMMAND LINE OPTIONS
#
#
#
cmdOptions, cmdArgs = getopt.getopt( sys.argv[1:], 's', [ 'no-splash', 'enable-cell-functions', 'enable-plugin-manager' ] )

bShowSplash = ( '--no-splash', '' )  not in  cmdOptions
bShowFunctionButtons = ( '--enable-cell-functions', '' )  in  cmdOptions
bEnablePluginManager = ( '--enable-plugin-manager', '' )  in  cmdOptions








#
#
#
# VERSION INFO WINDOW
#
#
#

warningText = _( \
"""
<span color="red">Warning:</span>
This is ALPHA release software
The work you produce in this gSculpt version is not guaranteed to work in future versions.
The author accepts no responsibility for lost time, hair, money, or sanity.

<span color="blue">Advice:</span>
- Save regularly
- As new versions of gSculpt may not load models created in previous version, export to OBJ regularly as well.
- The saving code can crash. In this case, the old work file will remain intact (it will not be replaced),
however, you will lose subsequent changes. The only notification that you will receive of a problem is a
traceback appearing in the command shell window.
""" )  +  _( \
"""
<span color="red">gSculpt-0.99.45.1-alpha notes</span>:
Previous versions contained a bug that could result in work that was created in those versions getting poly-souped
when loaded into v0.99.45.1 or newer. If this happens, you can use a previous version of gSculpt (such as
v0.99.45) to export your model as an OBJ. This can be reimported.
""" )  +  _( \
"""
<span color="red">gSculpt-0.99.43-alpha notes</span>:
The pick edge loops tool, pick edge rings tool, and pick edge boundaries tool, have been removed. Their functionality is
now part of the pick edges tool. See the pick edges tool for instructions.
""" )  +  _( \
"""
<span color="red">gSculpt-0.99.35-alpha notes</span>:
This version has introduced some changes to the slide edges tool which could introduce some incompatibilites.
The changes mean that the slide tool can slide edges that could not be slid before. This could result in some pieces
of work created in previous versions that use the slide operation, to load incorrectly. To fix these problems, go through
the plan carefully, and check your slide operations to see if they are doing the right thing. If not, modify them to
correct it. If this causes problems, please go to the gSculpt website at http://gsculpt.sourceforge.net and contact the
author, and attach your project file so that it can be fixed.
""" )


bVersionInfoDisplayed = False


def displayVersionInfo():
	global bVersionInfoDisplayed

	if not bVersionInfoDisplayed:
		dialog = gtk.Dialog( _( 'gSculpt %s Notes' ) % ( gSculptConfig.gSculptVersionString, ), None, gtk.DIALOG_MODAL,
						( gtk.STOCK_OK, gtk.RESPONSE_ACCEPT, gtk.STOCK_QUIT, gtk.RESPONSE_CANCEL ) )
		for line in warningText.strip().split( '\n' ):
			label = gtk.Label()
			label.set_markup( line )
			dialog.vbox.pack_start( label )
		dialog.show_all()
		response = dialog.run()
		dialog.destroy()
		if response == gtk.RESPONSE_CANCEL:
			sys.exit()

		bVersionInfoDisplayed = True









#
#
#
# CHECK VERSIONS
#
#
# Force the user config to load
gSculptUserConfig.userConfig.load()


lastVersionUsed = gSculptUserConfig.userConfig.lastVersionUsed
currentVersion = gSculptConfig.gSculptNumericVersionString

bForceDisplayVersionInfo = currentVersion != lastVersionUsed

gSculptUserConfig.userConfig.lastVersionUsed = currentVersion







#
#
#
# SPLASH SCREEN
#
#
#

if bShowSplash:
	SPLASH_MOUSE_OVER_POS = 268, 275
	SPLASH_MOUSE_OVER_SIZE = 132, 24

	_splashTickCount = 0
	_bSplashDestroyed = False
	_bSplashHighlighted = False



	def _splashTimeout():
		global _bSplashDestroyed, _splashTickCount

		if not _bSplashDestroyed:
			_splashTickCount += 1

			if _splashTickCount >= 20:
				splashWindow.destroy()
				if bForceDisplayVersionInfo:
					displayVersionInfo()
				_bSplashDestroyed = True
				return False
			else:
				return True
		else:
			return False


	def _highlightSplash():
		global _bSplashHighlighted

		if not _bSplashDestroyed:
			if not _bSplashHighlighted:
				splashImage.set_from_pixbuf( splashWithMouseOverPixbuf )
				_bSplashHighlighted = True





	def _unhighlightSplash():
		global _bSplashHighlighted

		if not _bSplashDestroyed:
			if _bSplashHighlighted:
				splashImage.set_from_pixbuf( splashPixbuf )
				_bSplashHighlighted = False




	def _isMouseInVersionButtonBounds(x, y):
		if x < SPLASH_MOUSE_OVER_POS[0]  or  y < SPLASH_MOUSE_OVER_POS[1]:
			return False
		if x - SPLASH_MOUSE_OVER_POS[0]  >  SPLASH_MOUSE_OVER_SIZE[0]  or  y - SPLASH_MOUSE_OVER_POS[1]  >  SPLASH_MOUSE_OVER_SIZE[1]:
			return False
		return True



	def _onSplashClicked(widget, event):
		global _bSplashDestroyed

		if not _bSplashDestroyed:
			x, y, state = event.window.get_pointer()

			splashWindow.destroy()
			_bSplashDestroyed = True

			if _isMouseInVersionButtonBounds( x, y )  or  bForceDisplayVersionInfo:
				displayVersionInfo()




	def _onSplashMotionNotify(widget, event):
		if not _bSplashDestroyed:
			if event.is_hint:
				x, y, state = event.window.get_pointer()
			else:
				x, y = event.x, event.y

			if _isMouseInVersionButtonBounds( x, y ):
				_highlightSplash()
			else:
				_unhighlightSplash()


	def _onSplashEnterNotify(widget, event):
		if not _bSplashDestroyed:
			x, y, state = event.window.get_pointer()

			if _isMouseInVersionButtonBounds( x, y ):
				_highlightSplash()



	def _onSplashLeaveNotify(widget, event):
		if not _bSplashDestroyed:
			_unhighlightSplash()



	splashPixbuf = gtk.gdk.pixbuf_new_from_file( GSConfig.gSculptSplashPath )
	splashMouseOverPixbuf = gtk.gdk.pixbuf_new_from_file( GSConfig.gSculptSplashMouseOverPath )

	splashWithMouseOverPixbuf = splashPixbuf.copy()
	splashMouseOverPixbuf.copy_area( 0, 0, SPLASH_MOUSE_OVER_SIZE[0], SPLASH_MOUSE_OVER_SIZE[1], splashWithMouseOverPixbuf, SPLASH_MOUSE_OVER_POS[0], SPLASH_MOUSE_OVER_POS[1] )

	splashImage = gtk.Image()
	splashImage.set_from_pixbuf( splashPixbuf )
	splashImage.show()


	splashEventBox = gtk.EventBox()
	splashEventBox.add( splashImage )
	splashEventBox.show()

	splashEventBox.connect( 'button-press-event', _onSplashClicked )
	splashEventBox.connect( 'motion-notify-event', _onSplashMotionNotify )
	splashEventBox.connect( 'enter-notify-event', _onSplashEnterNotify )
	splashEventBox.connect( 'leave-notify-event', _onSplashLeaveNotify )

	splashEventBox.add_events( gtk.gdk.BUTTON_PRESS_MASK |
							gtk.gdk.POINTER_MOTION_MASK |
							gtk.gdk.POINTER_MOTION_HINT_MASK |
							gtk.gdk.ENTER_NOTIFY_MASK |
							gtk.gdk.LEAVE_NOTIFY_MASK )

	splashWindow = gtk.Window( gtk.WINDOW_POPUP )
	splashWindow.add( splashEventBox )
	splashWindow.set_position( gtk.WIN_POS_CENTER_ALWAYS )
	splashWindow.show()


	gobject.timeout_add( 500, _splashTimeout )








if sys.platform == 'win32':
	# Initialise the PIL library on the Win32 platform by importing all image file format plugins
	# This is necessary for bundling with py2exe
	import Image
	import ArgImagePlugin
	import BmpImagePlugin
	import BufrStubImagePlugin
	import CurImagePlugin
	import DcxImagePlugin
	import EpsImagePlugin
	import FitsStubImagePlugin
	import FliImagePlugin
	import FpxImagePlugin
	import GbrImagePlugin
	import GifImagePlugin
	import GribStubImagePlugin
	import Hdf5StubImagePlugin
	import IcnsImagePlugin
	import IcoImagePlugin
	import ImImagePlugin
	import ImtImagePlugin
	import IptcImagePlugin
	import JpegImagePlugin
	import McIdasImagePlugin
	import MicImagePlugin
	import MpegImagePlugin
	import MspImagePlugin
	import PalmImagePlugin
	import PcdImagePlugin
	import PcxImagePlugin
	import PdfImagePlugin
	import PixarImagePlugin
	import PngImagePlugin
	import PpmImagePlugin
	import PsdImagePlugin
	import SgiImagePlugin
	import SpiderImagePlugin
	import SunImagePlugin
	import TgaImagePlugin
	import TiffImagePlugin
	import WmfImagePlugin
	import XbmImagePlugin
	import XpmImagePlugin
	import XVThumbImagePlugin
	Image._initialized = 2



from Britefury.FileIO.IOXml import *

from Britefury.Model.Model import GSProductModelBlank

from Britefury.Painter.Painter import ObjectPainterFactory

from Britefury.Project.GSProject import GSProject

from Britefury.MainApp.MainApp import MainApp

from Britefury.Util.Util import FPUPrecision, setFPUPrecision

import signal




bTestSETree = False


if bTestSETree:
	from Britefury.ProceduralCore.Product import GSProduct
	from Britefury.Mesh.Mesh import GSProductMesh, BoxPrimitive, MeshVertexList
	from Britefury.Util.Util import SETree, SEList

	# Create a box
	boxPrim = BoxPrimitive( 1, 1, 1, 100.0, 100.0, 100.0, True, True, True, True, True, True )
	sourceMesh = GSProductMesh()
	sourceMeshVerts = MeshVertexList()
	boxPrim.createBox( sourceMesh, sourceMeshVerts )

	# Create source SETree
	sourceTree = SETree()
	sourceMesh.writeSimple( sourceTree.getNode() )
	sourceText = sourceTree.write()

	# Load back in
	loadedTree = SETree()
	loadedTree.read( sourceText )
	loadedMesh = GSProductMesh()
	loadedMesh.readSimple( loadedTree.getNode() )

	# Write back out
	testTree = SETree()
	loadedMesh.writeSimple( testTree.getNode() )
	testText = testTree.write()

	assert testText == sourceText

	print sourceText
	print 'Data size %d' % ( len( testText ), )

	sys.exit();




from Britefury.CellEdit import CellEditFunctionButton
CellEditFunctionButton.__bShowFunctionButtons__ = bShowFunctionButtons



# Set FPU precision
setFPUPrecision( FPUPrecision.DOUBLE )


def onAppDestroy():
	gtk.main_quit()


def loadFile(filename):
	f = open( filename, 'r' )
	if f is not None:
		doc = InputXmlDocument()
		doc.parseFile( f )
		contentNode = doc.getContentNode()
		if contentNode.isValid():
			contentNode  >>  project
			app.setProjectFilename( filename )



painterFactory = ObjectPainterFactory.getPainterFactory()

app = MainApp( painterFactory, bEnablePluginManager )
app.destroySignal.connect( onAppDestroy )



project = GSProject()

if len( cmdArgs ) >= 1:
	loadFile( cmdArgs[0] )


app.attachProject( project )


gtk.main()

gSculptUserConfig.userConfig.save()

del painterFactory
del app
del project

gc.collect()
print 'Garbage:', gc.garbage
