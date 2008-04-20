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





class GtkAccelEntry (gtk.Entry):
        __gsignals__ = {
            'accel_set'  :  ( gobject.SIGNAL_RUN_LAST, gobject.TYPE_NONE, ( str, ) )
        }


	__ignoreKeys = set( [ gtk.keysyms.Shift_L, gtk.keysyms.Shift_R, gtk.keysyms.Control_L, gtk.keysyms.Control_R, gtk.keysyms.Alt_L, gtk.keysyms.Alt_R,
					gtk.keysyms.ISO_Level3_Shift, gtk.keysyms.Super_L, gtk.keysyms.Tab ] )


	def do_key_press_event(self, event):
		if event.keyval in self.__ignoreKeys:
			gtk.Entry.do_key_press_event( self, event )
		else:
			mods = event.state  &  ( gtk.gdk.SHIFT_MASK  |  gtk.gdk.CONTROL_MASK  |  gtk.gdk.MOD1_MASK )

			if event.keyval == gtk.keysyms.BackSpace:
				if self.get_text() != '':
					self._p_set( '', '' )
					return
			self._p_set( gtk.accelerator_name( event.keyval, mods ), gtk.accelerator_get_label( event.keyval, mods ) )


	def _p_set(self, accelName, accelLabel):
		self.set_text( accelLabel )
		self.emit( 'accel_set', accelName )

	def do_tab_pressed(self):
		pass


	def set_accel(self, accelName):
		keyval, mods = gtk.accelerator_parse( accelName )
		self.set_text( gtk.accelerator_get_label( keyval, mods ) )



gobject.type_register( GtkAccelEntry )






if __name__ == '__main__':
	def onDelete(window, event, data=None):
		gtk.main_quit()

	def onAccel(entry, accelName, data=None):
		print 'Accelerator: ', accelName


	entry = GtkAccelEntry()
	entry.connect( 'accel-set', onAccel )
	entry.show()


	window = gtk.Window( gtk.WINDOW_TOPLEVEL )
	window.connect( 'delete-event', onDelete )
	window.add( entry )
	window.set_border_width( 20 )
	window.show()


	gtk.main()