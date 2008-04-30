##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.GL.GL import *

from Britefury.Graphics.Graphics import Region3d

from Britefury.Model.Model import MarkPredicate

from Britefury.View.View import PaintLayer

from Britefury.Manipulator.Manipulator import Manipulator




class MarkRegionManipulator (Manipulator):
	def __init__(self):
		super( MarkRegionManipulator, self ).__init__()

		self._predicateCell = None
		self._regionCell = None
		self._backfaceCullCell = None
		self._bEnabledCell = None

		self._o_setUISensitivity( False )


	def attachPredicateCell(self, predicate):
		self._predicateCell = predicate

	def attachRegionCell(self, region):
		self._regionCell = region
		self._regionCell.changedSignal.connect( self._p_onRegionChanged )

	def attachBackfaceCullCell(self, backfaceCull):
		self._backfaceCullCell = backfaceCull

	def attachEnabledCell(self, enabled):
		self._bEnabledCell = enabled

	def detachCells(self):
		if self._regionCell is not None:
			self._regionCell.changedSignal.disconnect( self._p_onRegionChanged )
		self._predicateCell = None
		self._regionCell = None
		self._backfaceCullCell = None
		self._bEnabledCell = None


	def _o_setUISensitivity(self, bSensitive):
		pass



	def _o_createNewRegion(self, viewport):
		# Region
		if self._regionCell is not None:
			self._regionCell.literalValue = Region3d()

		# Backface culling
		if self._backfaceCullCell is not None:
			self._backfaceCullCell.literalValue = viewport.camera.createBackfaceCull()

		# Enable
		if self._bEnabledCell is not None:
			self._bEnabledCell.literalValue = True


	def _o_handleKeyStatus(self, viewport):
		if self._predicateCell is not None:
			pstat = viewport.pointerStatus

			bShift = pstat.bShiftKey
			bControl = pstat.bControlKey


			if not bShift  and  not bControl:
				pred = MarkPredicate.MARK
			elif bShift  and  not bControl:
				pred = MarkPredicate.UNMARK
			elif not bShift  and  bControl:
				pred = MarkPredicate.ADD
			elif bShift  and  bControl:
				pred = MarkPredicate.FILTER

			self._predicateCell.literalValue = pred


	def _o_paintRegion(self, viewport, paintLayer):
		if paintLayer == PaintLayer.PAINTLAYER_OVERLAY_BLEND:
			if self._regionCell is not None:
				region = self._regionCell.value

				if region is not None:
					glDepthMask( False )
					glColor4d( 0.0, 0.0, 0.3, 0.7 )
					region.paint()
					glDepthMask( True )


	def _p_onRegionChanged(self):
		self._o_viewPostRedraw()
