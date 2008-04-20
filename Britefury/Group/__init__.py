##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************



def _GSProductModel__getitem__(self, index):
	assert isinstance( index, int )
	l = len( self )
	if index < 0:
		index += l
	assert index >= 0  and  index < l
	return self._getModelClone( index )