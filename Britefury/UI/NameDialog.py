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



def nameDialog(title, message, prompt, initialValue = '', bModal = True, parentWindow=None,
				buttons = ( gtk.STOCK_OK, gtk.RESPONSE_OK, gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL )):
	def onEntryActivate(entry):
		dialog.response( gtk.RESPONSE_OK )

	dialogFlags = 0
	if bModal:
		dialogFlags |= gtk.DIALOG_MODAL
	if parentWindow is not None:
		dialogFlags |= gtk.DIALOG_DESTROY_WITH_PARENT

	dialog = gtk.Dialog( title, parentWindow, dialogFlags, buttons=buttons)

	messagesBox = gtk.VBox()
	for m in message.split( '\n' ):
		messagesBox.pack_start( gtk.Label( m ) )

	entry = gtk.Entry()
	entry.set_text( initialValue )
	entry.connect( 'activate', onEntryActivate )
	entryBox = gtk.HBox( spacing=10 )
	entryBox.pack_start( gtk.Label( prompt ) )
	entryBox.pack_start( entry )

	contentsVBox = gtk.VBox( spacing=20 )
	contentsVBox.pack_start( messagesBox )
	contentsVBox.pack_start( entryBox )

	paddingHBox = gtk.HBox()
	paddingHBox.pack_start( contentsVBox, padding=30 )

	dialog.vbox.pack_start( paddingHBox, padding=30 )

	dialog.show_all()
	dialog.grab_focus()
	response = dialog.run()
	dialog.destroy()

	if response == gtk.RESPONSE_OK:
		return entry.get_text()
	else:
		return None
