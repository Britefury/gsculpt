##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO.IOXml import *

from Britefury.Math.Math import Axis, Vector2, Vector3, Point2, Point3, Quaternion, Plane, Colour3f

from Britefury.Cell.Cell import *
from Britefury.Sheet.Sheet import *




## XML IO for enumerated types

axisIOXmlEnum = IOXmlEnum( Axis, 'Axis', Axis.AXIS_Z )
axisIOXmlEnum.value( Axis.AXIS_X, 'x' )
axisIOXmlEnum.value( Axis.AXIS_Y, 'y' )
axisIOXmlEnum.value( Axis.AXIS_Z, 'z' )
axisIOXmlEnum.value( Axis.AXIS_NEGATIVE_X, 'neg_x' )
axisIOXmlEnum.value( Axis.AXIS_NEGATIVE_Y, 'neg_y' )
axisIOXmlEnum.value( Axis.AXIS_NEGATIVE_Z, 'neg_z' )

ioXmlReadAxisProp = axisIOXmlEnum.propReader
ioXmlReadAxisNode = axisIOXmlEnum.nodeReader
ioXmlWriteAxisProp = axisIOXmlEnum.propWriter
ioXmlWriteAxisNode = axisIOXmlEnum.nodeWriter




#//////////////////////////////////////////////////////////////////////////////
#			Cell types
#//////////////////////////////////////////////////////////////////////////////

class AxisCell (Cell):
	valueClass = Axis
	bAllowNone = False


class Vector2Cell (Cell):
	valueClass = Vector2
	bAllowNone = False


class Vector3Cell (Cell):
	valueClass = Vector3
	bAllowNone = False


class Point2Cell (Cell):
	valueClass = Point2
	bAllowNone = False


class Point3Cell (Cell):
	valueClass = Point3
	bAllowNone = False


class QuaternionCell (Cell):
	valueClass = Quaternion
	bAllowNone = False


class PlaneCell (Cell):
	valueClass = Plane
	bAllowNone = False


class Colour3fCell (Cell):
	valueClass = Colour3f
	bAllowNone = False


registerCellType( Axis, AxisCell )
registerCellType( Vector2, Vector2Cell )
registerCellType( Vector3, Vector3Cell )
registerCellType( Point2, Point2Cell )
registerCellType( Point3, Point3Cell )
registerCellType( Quaternion, QuaternionCell )
registerCellType( Plane, PlaneCell )
registerCellType( Colour3f, Colour3fCell )


