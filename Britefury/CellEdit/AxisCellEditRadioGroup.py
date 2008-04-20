##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Axis

from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup


class AxisCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( AxisCellEditRadioGroup, self ).__init__()
		self.x = self.addChoice( Axis.AXIS_X, 'X' )
		self.y = self.addChoice( Axis.AXIS_Y, 'Y' )
		self.z = self.addChoice( Axis.AXIS_Z, 'Z' )
		self.negativeX = self.addChoice( Axis.AXIS_NEGATIVE_X, '-X' )
		self.negativeY = self.addChoice( Axis.AXIS_NEGATIVE_Y, '-Y' )
		self.negativeZ = self.addChoice( Axis.AXIS_NEGATIVE_Z, '-Z' )







if __name__ == '__main__':
	from Britefury.Cell.Cell import *
	from Britefury.CellEdit.CellEditLabelWithFn import CellEditLabelWithFn
	from Britefury.UI.TestWindow import *
	from Britefury.UI.FormLayout import *
	from Britefury.I18n import i18n

	i18n.initialise()

	owner = CellOwner()
	cell1 = IntCell( 0 )
	cell1.owner = owner
	owner.cellScope['cell1'] = cell1


	label1 = CellEditLabelWithFn( 'cell1:' )
	label1.show()
	label1.attachCell( cell1 )

	radio1 = AxisCellEditRadioGroup()
	radio1.show()
	radio1.attachCell( cell1 )



	layout = FormLayout()
	layout.row()  <<  label1.label
	layout.row()  <<  radio1.x
	layout.row()  <<  radio1.y
	layout.row()  <<  radio1.z
	layout.row()  <<  radio1.negativeX
	layout.row()  <<  radio1.negativeY
	layout.row()  <<  radio1.negativeZ
	layout.finalise()
	layout.getWidget().show()

	runTestWindow( layout.getWidget() )
