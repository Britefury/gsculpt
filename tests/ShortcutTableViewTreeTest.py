##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.MainApp.Shortcuts.ShortcutEditor import *


def onDelete(window, event, data=None):
	return False

view = ShortcutTableViewTree( ShortcutTable() )
view.getWidget().show()


scroll = gtk.ScrolledWindow()
scroll.set_policy( gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC )
scroll.add_with_viewport( view.getWidget() )
scroll.show()


window = gtk.Window( gtk.WINDOW_TOPLEVEL )
window.connect( 'delete-event', onDelete )
window.add( scroll )
window.set_border_width( 20 )
window.show()
