##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.SheetEdit import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSSceneNodeViewSettings import GSSceneNodeViewSettings

from Britefury.Mesh.Mesh import MeshPreserveNormalSharpness, MeshLiveSubdWireframeMode, MeshPainter
from Britefury.Mesh import MeshPreserveNormalSharpnessCell, MeshLiveSubdWireframeModeCell
from Britefury.Mesh.MeshPreserveNormalSharpnessFieldEdit import *
from Britefury.Mesh.MeshLiveSubdWireframeModeFieldEdit import *


class GSSceneNodeMeshViewSettings (GSSceneNodeViewSettings):
	painterClass = MeshPainter


	bLiveSubdivisionEnabled = Field( bool, False )
	liveSubdivisionIterations = Field( int, 1 )
	liveSubdivisionMaxFaces = Field( int, 81920 )
	liveSubdivisionNormalSharpness = Field( MeshPreserveNormalSharpness, MeshPreserveNormalSharpness.IF_SHARP )
	liveSubdivisionWireframeMode = Field( MeshLiveSubdWireframeMode, MeshLiveSubdWireframeMode.CONTROL )


	def applyToViewSettings(self, viewSettings):
		viewSettings.bMeshLiveSubdivisionEnabled = self.bLiveSubdivisionEnabled
		viewSettings.meshLiveSubdivisionIterations = self.liveSubdivisionIterations
		viewSettings.meshLiveSubdivisionMaxFaces = self.liveSubdivisionMaxFaces
		viewSettings.meshLiveSubdivisionNormalSharpness = self.liveSubdivisionNormalSharpness
		viewSettings.meshLiveSubdivisionWireframeMode = self.liveSubdivisionWireframeMode


	def _o_applyToPainter(self, painter):
		painter.setBackgroundMeshViewSettings( self.bLiveSubdivisionEnabled, self.liveSubdivisionIterations, self.liveSubdivisionMaxFaces,
										self.liveSubdivisionNormalSharpness, self.liveSubdivisionWireframeMode )



class GSSceneNodeMeshViewSettingsEditor (SheetEditor):
	sheet = SheetRefField( GSSceneNodeMeshViewSettings )


	bLiveSubdEnabledCheck = BoolFieldEditCheckWithFn( sheet.bLiveSubdivisionEnabled, _( 'Enabled' ) )
	liveSubdIterationsLabel = FieldLabelWithFn( sheet.liveSubdivisionIterations, _( 'Iterations:' ) )
	liveSubdIterationsEntry = IntFieldEditEntry( sheet.liveSubdivisionIterations, 0, 16, 1, 3 )
	liveSubdMaxFacesLabel = FieldLabelWithFn( sheet.liveSubdivisionMaxFaces, _( 'Max. faces:' ) )
	liveSubdMaxFacesEntry = IntFieldEditEntry( sheet.liveSubdivisionMaxFaces, 1, 1 << 30, 128, 4096 )
	liveSubdNormalSharpnessLabel = FieldLabelWithFn( sheet.liveSubdivisionNormalSharpness, _( 'Normal sharpness:' ) )
	liveSubdNormalSharpnessRadio = MeshPreserveNormalSharpnessFieldEditRadioGroup( sheet.liveSubdivisionNormalSharpness )
	liveSubdWireframeModeLabel = FieldLabelWithFn( sheet.liveSubdivisionWireframeMode, _( 'Wireframe mode:' ) )
	liveSubdWireframeModeRadio = MeshLiveSubdWireframeModeFieldEditRadioGroup( sheet.liveSubdivisionWireframeMode )

	layout = FormLayoutFactory()
	layout.row()  <<  _( 'Mesh view settings' )
	layout.row()
	layout.row()  <<  _( 'Live subdivision' )
	layout.row()  <<  bLiveSubdEnabledCheck.check
	layout.row()  <<  liveSubdIterationsLabel.label  <<  liveSubdIterationsEntry.entry  <<  None  <<  None
	layout.row()  <<  liveSubdMaxFacesLabel.label  <<  liveSubdMaxFacesEntry.entry  <<  None  <<  None
	layout.row()  <<  liveSubdNormalSharpnessLabel.label  <<  liveSubdNormalSharpnessRadio.preserve  <<  liveSubdNormalSharpnessRadio.ifSharp  <<  liveSubdNormalSharpnessRadio.smooth
	layout.row()  <<  liveSubdWireframeModeLabel.label  <<  liveSubdWireframeModeRadio.none  <<  liveSubdWireframeModeRadio.control  <<  liveSubdWireframeModeRadio.full
	layout.finalise()
