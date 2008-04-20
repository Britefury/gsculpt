##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import math

from Britefury.Math.Math import Vector2, Vector3, Point2, Point3

from Britefury.Cell.Cell import FloatCell



class ManipulatorSnapping (object):
	def __init__(self, abGranularity, aGranularity, bGranularity):
		self.abGranularity = FloatCell( abGranularity )
		self.aGranularity = FloatCell( aGranularity )
		self.bGranularity = FloatCell( bGranularity )



	def snap(self, x, bA, bB):
		granularity = self._p_getGranularity( bA, bB )
		if granularity is not None:
			return self._p_snap( x, granularity )
		else:
			return x



	def snapVector2(self, v, constraints, bA, bB):
		return self._p_snap2( v, constraints, bA, bB, Vector2 )

	def snapVector3(self, v, constraints, bA, bB):
		return self._p_snap3( v, constraints, bA, bB, Vector3 )

	def snapPoint2(self, v, constraints, bA, bB):
		return self._p_snap2( v, constraints, bA, bB, Point2 )

	def snapPoint3(self, v, constraints, bA, bB):
		return self._p_snap3( v, constraints, bA, bB, Point3 )


	def _p_snap2(self, v, constraints, bA, bB, valueType):
		granularity = self._p_getGranularity( bA, bB )
		if granularity is not None:
			if constraints is not None:
				return valueType( self._p_constrainedSnap( v, constraints, granularity, Vector2 ) )
			else:
				return valueType( self._p_snap( v.x, granularity ),  self._p_snap( v.y, granularity ) )
		else:
			return v

	def _p_snap3(self, v, constraints, bA, bB, valueType):
		granularity = self._p_getGranularity( bA, bB )
		if granularity is not None:
			if constraints is not None:
				return valueType( self._p_constrainedSnap( v, constraints, granularity, Vector3 ) )
			else:
				return valueType( self._p_snap( v.x, granularity ),  self._p_snap( v.y, granularity ),  self._p_snap( v.z, granularity ) )
		else:
			return v


	def _p_constrainedSnap(self, v, constraints, granularity, constrainedVectorType):
		if isinstance( constraints, list ):
			a = [ v.dot( constraint )  for constraint in constraints ]
			unconstrained = reduce( lambda v, constraintAndA:  v  -  constraintAndA[0] * constraintAndA[1], zip( constraints, a ), v )
			constrained = reduce( lambda constrained, constraintAndA:  constrained  +  constraintAndA[0] * self._p_snap( constraintAndA[1], granularity ), zip( constraints, a ), constrainedVectorType() )
			return unconstrained + constrained
		else:
			a = v.dot( constraints )
			unconstrained = v  -  constraints * a
			constrained = constraints * self._p_snap( a, granularity )
			return unconstrained + constrained


	def _p_getGranularity(self, bA, bB):
		if bA and not bB:
			return self.aGranularity.value
		elif not bA and bB:
			return self.bGranularity.value
		elif bA and bB:
			return self.abGranularity.value
		else:
			return None

	def _p_snap(self, x, granularity):
		q = math.floor( x / granularity  +  0.5 )
		return q * granularity




if __name__ == '__main__':
	from Britefury.Math.Math import Matrix4, Axis
	snapper = ManipulatorSnapping( 1.0, 1.0, 1.0 )

	def printSnap(value, constraints, snapperFunction):
		snapped = snapperFunction( value, constraints )
		if constraints is None:
			print value, ' ==> ', snapped
		else:
			if isinstance( constraints, list ):
				print value, ' ==> ', snapped, '   [', [ snapped.dot( c )  for c in constraints ], ']'
			else:
				print value, ' ==> ', snapped, '   [', snapped.dot( constraints ), ']'



	rotMatrix = Matrix4.rotate( Vector3( 2.4, -1.4, 6.3 ).getNormalised(), math.radians( 183.2 ) )
	space = [ Vector3( axis ) * rotMatrix   for axis in [ Axis.AXIS_X, Axis.AXIS_Y, Axis.AXIS_Z ] ]
	for v in space:
		print v, v.length()



	snap1 = lambda x, constraints: snapper.snap( x, True, True )
	snapVector3 = lambda x, constraints: snapper.snapVector3( x, constraints, True, True )
	snapPoint3 = lambda x, constraints: snapper.snapPoint3( x, constraints, True, True )


	printSnap( 5.4, None, snap1 )
	printSnap( Vector3( 5.4, 2.2, 3.1 ), None, snapVector3 )
	printSnap( Vector3( 5.4, 2.2, 3.1 ), Vector3( 1.0, 0.0, 0.0 ), snapVector3 )
	printSnap( Vector3( 5.4, 2.2, 3.1 ), space[0], snapVector3 )
	printSnap( Vector3( 5.4, 2.2, 3.1 ), [ space[0], space[1] ], snapVector3 )
	printSnap( Point3( 5.4, 2.2, 3.1 ), [ space[0], space[1] ], snapPoint3 )



