##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Util.SignalSlot import ClassSignal

from Britefury.FileIO.IOXml import ioObjectFactoryRegister

from Britefury.Cell.Cell import *

from Britefury.Sheet.Sheet import *



class TargetList (list):
	def __readxml__(self, xmlNode):
		self[:] = []
		for child in xmlNode:
			target = child.readObject()
			if target is not None:
				self.append( target )

	def __writexml__(self, xmlNode):
		for target in self:
			xmlNode.addChild( 'target' ).writeObject( target )

	def __add__(self, x):
		y = TargetList()
		y[:] = self
		y += x
		return y


ioObjectFactoryRegister( 'TargetList', TargetList )



class TargetListCell (Cell):
	valueClass = TargetList
	bAllowNone = False

registerCellType( TargetList, TargetListCell )





