##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Vector3, Matrix4, Quaternion

from Britefury.TransformationTools.ToolTransformation import *
from Britefury.TransformationTools.ToolTransformationBase import *



class ProcRotatedTransformation (ProcTransformationBase):
	description = _( 'Abstract rotated transformation' )

	xformRotation = Field( Quaternion, Quaternion() )



class RotatedTransformationAdjustableInvoker (TransformationBaseAdjustableInvoker):
	functionClass = ProcRotatedTransformation





class ToolRotatedTransformation (ToolTransformationBase):
	sheet = SheetRefField( ProcRotatedTransformation )

	functionClass = ProcRotatedTransformation
	pageTitle = _( 'Rotated Transformation' )
	titleText = _( 'Abstract Rotated Transformation' )
	adjustableInvoker = RotatedTransformationAdjustableInvoker


