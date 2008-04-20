##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.View.View import PaintLayer


class RequestPainterObjectSource (object):
	def requestObjectToPaint(self):
		assert False, 'abstract'





class RequestPainter (object):
	def __init__(self, painterFactory):
		assert painterFactory is not None
		self._painterFactory = painterFactory
		self.painterObjectSource = None

		self._cache_product = None
		self._cache_painter = None



	def paintObject3d(self, viewportSettings, paintLayer, bBackground):
		if self.painterObjectSource is not None:
			if self._p_requestObject():
				if self._cache_painter is not None:
					#if paintLayer != PaintLayer.PAINTLAYER_WIREFRAME:
						self._cache_painter.paintObject3d( viewportSettings, paintLayer, True, bBackground )


	def _p_requestObject(self):
		product = self.painterObjectSource.requestObjectToPaint()


		if product is not None:
			# NOTE: One would initially assume that you would only need to create a new painter when the object *type*
			# changes. This would work, except that since the painter is a C++ object, assigning a product to the painter
			# results in the Python garbage collector considering the painter to be a referrer of the product. This can result
			# in the painter being a referrer of many products, resulting in none of them being destroyed by the garbage
			# collector, preventing their memory from being freed.
			if product is not self._cache_product:
				# Different product; need a new painter
				self._cache_painter = self._painterFactory.createPainterForObject( product )
				if self._cache_painter is not None:
					self._cache_painter.product = product
				self._cache_product = product

			return True
		else:
			self._cache_painter = None
			self._cache_product = None

			return False





