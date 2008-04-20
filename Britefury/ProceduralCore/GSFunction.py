##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
class GSFunctionClass (type):
	def __init__(cls, clsName, bases, clsDict):
		super( GSFunctionClass, cls ).__init__( clsName, bases, clsDict )

		if not clsDict.has_key( 'description' )  and  not clsDict.has_key( 'getDescription' ):
			raise TypeError, 'function description not supplied'






class GSFunction (object):
	__metaclass__ = GSFunctionClass
	description = 'Abstract function'


	@classmethod
	def getDescription(cls):
		return cls.description












