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


def messageDialog(title, parent, message):
	dialog = gtk.Dialog( title=title, parent=parent, flags=gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT, buttons=( gtk.STOCK_OK, gtk.RESPONSE_ACCEPT ) )
	dialog.vbox.pack_start( gtk.Label( message ) )
	dialog.show_all()
	dialog.run()
	dialog.destroy()
