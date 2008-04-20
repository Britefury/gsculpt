##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.NumericCellEditSlider import NumericCellEditSlider


class FloatCellEditSlider (NumericCellEditSlider):
	"""Floating point number cell editor - slider
	@slider - the slider widget"""

	__valueclass__ = float

	def __init__(self, minValue, maxValue, step, page):
		super( FloatCellEditSlider, self ).__init__( minValue, maxValue, step, page )




if __name__ == '__main__':
	from Britefury.Cell.Cell import *
	from Britefury.UI.TestWindow import *
	from Britefury.I18n import i18n

	i18n.initialise()

	cell = FloatCell( 0.0 )

	slider = FloatCellEditSlider( -1048576.0, 1048576.0, 1.0, 10.0 )
	slider.show()
	slider.attachCell( cell )

	runTestWindow( slider.slider )

