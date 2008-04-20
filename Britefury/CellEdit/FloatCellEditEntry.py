##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.NumericCellEditEntry import NumericCellEditEntry


class FloatCellEditEntry (NumericCellEditEntry):
	"""Floating point number cell editor - entry
	@label - the label
	@entry - the entry widget"""

	__valueclass__ = float

	def __init__(self, minValue, maxValue, step, page, numDigits):
		super( FloatCellEditEntry, self ).__init__( minValue, maxValue, step, page, numDigits )




if __name__ == '__main__':
	from Britefury.Cell.Cell import *
	from Britefury.UI.TestWindow import *
	from Britefury.I18n import i18n

	i18n.initialise()

	cell = FloatCell( 0.0 )

	entry = FloatCellEditEntry( -1048576.0, 1048576.0, 1.0, 10.0, 4 )
	entry.show()
	entry.attachCell( cell )

	box = gtk.HBox( homogeneous=True )
	box.pack_start( entry.entry, True, True )
	box.show()

	runTestWindow( box )

