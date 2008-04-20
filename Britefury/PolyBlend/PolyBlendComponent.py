##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.PolyBlend.PolyBlend import PolyBlend

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.SheetEdit import *
from Britefury.SheetEdit.FieldEditors import *



class PolyBlendComponent (Sheet):
	collapsePenalty = Field( float, 1.0 )
	twistPenalty = Field( float, 1.0 )
	inversionPenalty = Field( float, 2.0 )
	areaWeighting = Field( float, 0.5 )


	def makePolyBlend(self):
		return PolyBlend( self.collapsePenalty, self.twistPenalty, self.inversionPenalty, self.areaWeighting )




class PolyBlendComponentCell (CompositeCell):
	valueClass = PolyBlendComponent

registerCellType( PolyBlendComponent, PolyBlendComponentCell )





class PolyBlendComponentEditor (SheetEditor):
	sheet = SheetRefField( PolyBlendComponent )


	collapsePenaltyLabel = FieldLabelWithFn( sheet.collapsePenalty, _( 'Collapse penalty:' ) )
	collapsePenaltyEntry = FloatFieldEditEntry( sheet.collapsePenalty, -1024576.0, 1024576.0, 0.01, 0.1, 4 )
	twistPenaltyLabel = FieldLabelWithFn( sheet.twistPenalty, _( 'Twist penalty:' ) )
	twistPenaltyEntry = FloatFieldEditEntry( sheet.twistPenalty, -1024576.0, 1024576.0, 0.01, 0.1, 4 )
	inversionPenaltyLabel = FieldLabelWithFn( sheet.inversionPenalty, _( 'Inversion penalty:' ) )
	inversionPenaltyEntry = FloatFieldEditEntry( sheet.inversionPenalty, -1024576.0, 1024576.0, 0.01, 0.1, 4 )
	areaWeightingLabel = FieldLabelWithFn( sheet.areaWeighting, _( 'Area weighting:' ) )
	areaWeightingEntry = FloatFieldEditEntry( sheet.areaWeighting, -1024576.0, 1024576.0, 0.01, 0.1, 4 )

	layout = FormLayoutFactory()
	layout.row()  <<  collapsePenaltyLabel.label  <<  collapsePenaltyEntry.entry
	layout.row()  <<  twistPenaltyLabel.label  <<  twistPenaltyEntry.entry
	layout.row()  <<  inversionPenaltyLabel.label  <<  inversionPenaltyEntry.entry
	layout.row()  <<  areaWeightingLabel.label  <<  areaWeightingEntry.entry
	layout.finalise()



class PolyBlendComponentFieldEdit (CompositeFieldEditor):
	compositeSheetEditorClass = PolyBlendComponentEditor
