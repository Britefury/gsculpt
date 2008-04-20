##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import pygtk
pygtk.require( '2.0' )
import gtk



def makeEmptyTestWindow():
	def onWindowDelete(window, event):
		gtk.main_quit()


	window = gtk.Window( gtk.WINDOW_TOPLEVEL )
	window.set_border_width( 25 )
	window.connect( 'delete-event', onWindowDelete )

	window.show()

	return window



def makeTestWindow(contents):
	window = makeEmptyTestWindow()
	window.add( contents )

	return window



def runTestWindow(contents):
	makeTestWindow( contents )
	gtk.main()
