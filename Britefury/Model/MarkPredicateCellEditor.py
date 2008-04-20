##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Model.Model import MarkPredicate
import Britefury.Model.MarkPredicateCell



# Cell editor for MarkPredicate

class MarkPredicateCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MarkPredicateCellEditRadioGroup, self ).__init__()
		self.mark = self.addChoice( MarkPredicate.MARK, _( 'Select' ) )
		self.unmark = self.addChoice( MarkPredicate.UNMARK, _( 'Unselect' ) )
		self.add = self.addChoice( MarkPredicate.ADD, _( 'Add to selection' ) )
		self.filter = self.addChoice( MarkPredicate.FILTER, _( 'Filter selection' ) )
		self.invert = self.addChoice( MarkPredicate.INVERT, _( 'Invert selection' ) )
