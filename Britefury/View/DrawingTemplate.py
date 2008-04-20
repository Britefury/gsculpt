##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************

class DrawingTemplate (object):
	def templateDrawingPoint3d(self, ray, bBackfaceCulling):
		"""Template drawing point - 3D
		@ray - the ray to raytrace 		[Segment3d]
		@bBackfaceCulling - cull back faces
		Returns: a tuple of:
			bSuccess		 [bool]						Successful; did we hit anything?
			t			[float] (or None if no hit)			The intersection parameter
			intersection	[Point3] (or None if no hit)			The intersection point
			clippedRay	[Segment3] (or None if no hit)		The clipped ray"""
		return False, None, None, None

