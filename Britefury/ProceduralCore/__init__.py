##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO.IOXml import ioObjectFactoryRegister

from Britefury.Kernel import RTType

from Britefury.ProceduralCore.Product import GSProduct

from Britefury.Util.Util import SETree


def GSProduct__copy__(self):
	return self.clone()


def GSProduct__deepcopy__(self, memo):
	return self.clone()



def GSProduct__readxml__(self, node):
	tree = SETree()
	tree.read( str( node.content ) )
	self.readSimple( tree.getNode() )


def GSProduct__writexml__(self, node):
	tree = SETree()
	self.writeSimple( tree.getNode() )
	node.content = tree.write()



GSProduct.__copy__ = GSProduct__copy__
GSProduct.__deepcopy__ = GSProduct__deepcopy__
GSProduct.__readxml__ = GSProduct__readxml__
GSProduct.__writexml__ = GSProduct__writexml__

ioObjectFactoryRegister( 'GSProduct', GSProduct )


class UnknownProduct (GSProduct):
	pass

ioObjectFactoryRegister( 'UnknownProduct', UnknownProduct )


def getUnknownProductClass():
	return UnknownProduct

