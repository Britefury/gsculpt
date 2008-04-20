##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Point3

from Britefury.CellEdit.VectorCellEditHelpers import Vector3OrPoint3CellEditEntry


class Point3CellEditEntry (Vector3OrPoint3CellEditEntry):
	"""Point3 cell editor - entry
	@x - the x widget
	@y - the y widget
	@z - the z widget"""

	__valueclass__ = Point3





if __name__ == '__main__':
	from Britefury.Cell.Cell import *
	from Britefury.CellEdit.CellEditLabelWithFn import CellEditLabelWithFn
	from Britefury.UI.TestWindow import *
	from Britefury.UI.FormLayout import *
	from Britefury.I18n import i18n
	from Britefury.Math.MathCellTypes import *

	i18n.initialise()

	owner = CellOwner()
	cell1 = Point3Cell( Point3() )
	cell2 = Point3Cell( Point3() )
	cell1.owner = owner
	cell2.owner = owner
	owner.cellScope['Point3'] = Point3
	owner.cellScope['cell1'] = cell1
	owner.cellScope['cell2'] = cell2


	label1 = CellEditLabelWithFn( 'cell1:' )
	label1.show()
	label1.attachCell( cell1 )

	entry1 = Point3CellEditEntry( -1048576, 1048576, 1, 10, 4 )
	entry1.show()
	entry1.attachCell( cell1 )


	label2 = CellEditLabelWithFn( 'cell2:' )
	label2.show()
	label2.attachCell( cell2 )

	entry2 = Point3CellEditEntry( -1048576, 1048576, 1, 10, 4 )
	entry2.show()
	entry2.attachCell( cell2 )


	layout = FormLayout()
	layout.row()
	layout.row()  <<  label1.label
	layout.row()  <<  'X:'  <<  entry1.x
	layout.row()  <<  'Y:'  <<  entry1.y
	layout.row()  <<  'Z:'  <<  entry1.z
	layout.row()
	layout.row()  <<  label2.label
	layout.row()  <<  'X:'  <<  entry2.x
	layout.row()  <<  'Y:'  <<  entry2.y
	layout.row()  <<  'Z:'  <<  entry2.z
	layout.finalise()
	layout.getWidget().show()

	runTestWindow( layout.getWidget() )
