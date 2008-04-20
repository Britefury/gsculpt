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
import gobject

from Britefury.Config import GSConfig





g_imagePixbufsByFilename = {}


def getImagePixbuf(filename):
	try:
		return g_imagePixbufsByFilename[filename]
	except KeyError:
		pixbuf = gtk.gdk.pixbuf_new_from_file( filename )
		g_imagePixbufsByFilename[filename] = pixbuf
		return pixbuf


def getGSImagePixbuf(filename):
	return getImagePixbuf( GSConfig.getGSImagePath( filename ) )



def getImageWidget(filename):
	image = gtk.Image()
	image.set_from_pixbuf( getImagePixbuf( filename ) )
	image.show()
	return image


def getGSImageWidget(filename):
	image = gtk.Image()
	image.set_from_pixbuf( getGSImagePixbuf( filename ) )
	image.show()
	return image

