##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import Brush

from Britefury.FileIO.IOXml import *




from Britefury.Brush.Brush import CubicSCurveBrush, FlatBrush, GaussianBrush, LinearBrush, SphericalBrush


#
# COPYING
#

def __primitive__copy__(self):
	return self.__class__( self )

def __primitive__deepcopy__(self, memo):
	return self.__class__( self )


def implementPrimitiveCopyMethods(cls):
	cls.__copy__ = __primitive__copy__
	cls.__deepcopy__ = __primitive__deepcopy__



implementPrimitiveCopyMethods( Brush.CubicSCurveBrush )
implementPrimitiveCopyMethods( Brush.FlatBrush )
implementPrimitiveCopyMethods( Brush.GaussianBrush )
implementPrimitiveCopyMethods( Brush.LinearBrush )
implementPrimitiveCopyMethods( Brush.SphericalBrush )








#
# XML IO
#

def implementXml(typeName, cls, readFunction, writeFunction):
	cls.__readxml__ = readFunction
	cls.__writexml__ = writeFunction
	ioObjectFactoryRegister( typeName, cls )



def Brush__readxml__(self, xmlNode):
	pass

def Brush__writexml__(self, xmlNode):
	pass

implementXml( 'Brush', Brush.Brush, Brush__readxml__, Brush__writexml__ )



def SphericalBoundaryBrush__readxml__(self, xmlNode):
	super( Brush.SphericalBoundaryBrush, self ).__readxml__( xmlNode )
	self.radius = ioXmlReadFloatNode( xmlNode.getChild( 'radius' ), self.radius )

def SphericalBoundaryBrush__writexml__(self, xmlNode):
	super( Brush.SphericalBoundaryBrush, self ).__writexml__( xmlNode )
	ioXmlWriteFloatNode( xmlNode.addChild( 'radius' ), self.radius )

implementXml( 'SphericalBoundaryBrush', Brush.SphericalBoundaryBrush, SphericalBoundaryBrush__readxml__, SphericalBoundaryBrush__writexml__ )




def CubicSCurveBrush__readxml__(self, xmlNode):
	super( Brush.CubicSCurveBrush, self ).__readxml__( xmlNode )

def CubicSCurveBrush__writexml__(self, xmlNode):
	super( Brush.CubicSCurveBrush, self ).__writexml__( xmlNode )

implementXml( 'CubicSCurveBrush', Brush.CubicSCurveBrush, CubicSCurveBrush__readxml__, CubicSCurveBrush__writexml__ )



def FlatBrush__readxml__(self, xmlNode):
	super( Brush.FlatBrush, self ).__readxml__( xmlNode )

def FlatBrush__writexml__(self, xmlNode):
	super( Brush.FlatBrush, self ).__writexml__( xmlNode )

implementXml( 'FlatBrush', Brush.FlatBrush, FlatBrush__readxml__, FlatBrush__writexml__ )



def GaussianBrush__readxml__(self, xmlNode):
	super( Brush.GaussianBrush, self ).__readxml__( xmlNode )
	self.gaussianMinimum = ioXmlReadFloatNode( xmlNode.getChild( 'gaussian_minimum' ), self.radius )

def GaussianBrush__writexml__(self, xmlNode):
	super( Brush.GaussianBrush, self ).__writexml__( xmlNode )
	ioXmlWriteFloatNode( xmlNode.addChild( 'gaussian_minimum' ), self.gaussianMinimum )

implementXml( 'GaussianBrush', Brush.GaussianBrush, GaussianBrush__readxml__, GaussianBrush__writexml__ )



def LinearBrush__readxml__(self, xmlNode):
	super( Brush.LinearBrush, self ).__readxml__( xmlNode )

def LinearBrush__writexml__(self, xmlNode):
	super( Brush.LinearBrush, self ).__writexml__( xmlNode )

implementXml( 'LinearBrush', Brush.LinearBrush, LinearBrush__readxml__, LinearBrush__writexml__ )



def SphericalBrush__readxml__(self, xmlNode):
	super( Brush.SphericalBrush, self ).__readxml__( xmlNode )

def SphericalBrush__writexml__(self, xmlNode):
	super( Brush.SphericalBrush, self ).__writexml__( xmlNode )

implementXml( 'SphericalBrush', Brush.SphericalBrush, SphericalBrush__readxml__, SphericalBrush__writexml__ )

