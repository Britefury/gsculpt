##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Sheet.Sheet import *


class GSSceneNodeViewSettings (Sheet):
	"""Scene node view settings

	Override the @painterClass class variable with the class of the painter to which this settings class applies"""

	painterClass = None


	def applyToViewSettings(self, viewSettings):
		pass

	def applyToPainter(self, painter):
		if isinstance( painter, self.painterClass ):
			self._o_applyToPainter( painter )


	def _o_applyToPainter(self, painter):
		pass

