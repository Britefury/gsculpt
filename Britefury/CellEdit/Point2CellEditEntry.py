##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Point2

from Britefury.CellEdit.VectorCellEditHelpers import Vector2OrPoint2CellEditEntry


class Point2CellEditEntry (Vector2OrPoint2CellEditEntry):
	"""Point2 cell editor - entry
	@x - the x widget
	@y - the y widget"""

	__valueclass__ = Point2





if __name__ == '__main__':
	from Britefury.Cell.Cell import *
	from Britefury.CellEdit.CellEditLabelWithFn import CellEditLabelWithFn
	from Britefury.UI.TestWindow import *
	from Britefury.UI.FormLayout import *
	from Britefury.I18n import i18n
	from Britefury.Math.MathCellTypes import *

	i18n.initialise()

	owner = CellOwner()
	cell1 = Point2Cell( Point2() )
	cell2 = Point2Cell( Point2() )
	cell1.owner = owner
	cell2.owner = owner
	owner.cellScope['Point2'] = Point2
	owner.cellScope['cell1'] = cell1
	owner.cellScope['cell2'] = cell2


	label1 = CellEditLabelWithFn( 'cell1:' )
	label1.show()
	label1.attachCell( cell1 )

	entry1 = Point2CellEditEntry( -1048576, 1048576, 1, 10, 4 )
	entry1.show()
	entry1.attachCell( cell1 )


	label2 = CellEditLabelWithFn( 'cell2:' )
	label2.show()
	label2.attachCell( cell2 )

	entry2 = Point2CellEditEntry( -1048576, 1048576, 1, 10, 4 )
	entry2.show()
	entry2.attachCell( cell2 )


	layout = FormLayout()
	layout.row()
	layout.row()  <<  label1.label
	layout.row()  <<  'X:'  <<  entry1.x
	layout.row()  <<  'Y:'  <<  entry1.y
	layout.row()
	layout.row()  <<  label2.label
	layout.row()  <<  'X:'  <<  entry2.x
	layout.row()  <<  'Y:'  <<  entry2.y
	layout.finalise()
	layout.getWidget().show()

	runTestWindow( layout.getWidget() )
