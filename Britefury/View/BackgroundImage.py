##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from OpenGL.GL import *

from Britefury.FileIO.IOXml import ioObjectFactoryRegister

from Britefury.Graphics.Graphics import glMultMatrix

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Math.Math import Point3, Quaternion, Vector2, Matrix4

from Britefury.Sheet.Sheet import *

from Britefury.Texture.TextureRef import TextureRef
from Britefury.Texture.TextureRefCell import TextureRefCell

from Britefury.WorkArea.Viewport3d import Viewport3dPainter

from Britefury.View.View import PaintLayer
from Britefury.View import BackgroundImageCommandTracker




class BackgroundImage (Sheet):
	trackerClass = BackgroundImageCommandTracker.BackgroundImageCommandTracker

	texture = Field( TextureRef, TextureRef() )
	position = Field( Point3, Point3() )
	rotation = Field( Quaternion, Quaternion() )
	size = Field( Vector2, Vector2( 100.0, 100.0 ) )
	brightness = Field( float, 1.0 )
	opacity = Field( float, 1.0 )
	bOverlay = Field( bool, False )
	bVisible = Field( bool, True )
	bBackfaceCull = Field( bool, False )

	textureChangedSignal = ClassSignal()


	def __init__(self):
		super( BackgroundImage, self ).__init__()
		self.cells.texture.changedSignal.connect( self._p_onTextureChanged )



	def paint3d(self, paintLayer):
		if self.bVisible:
			if ( paintLayer == PaintLayer.PAINTLAYER_OVERLAY_BLEND and not self.bOverlay )  or  ( paintLayer == PaintLayer.PAINTLAYER_OVERLAY_BLEND_NODEPTH and self.bOverlay ):
				if self.bBackfaceCull:
					glCullFace( GL_BACK )
					glEnable( GL_CULL_FACE )

				matrix = Matrix4.scale( self.size )  *  Matrix4.rotate( self.rotation )  *  Matrix4.translate( self.position.toVector3() )
				glPushMatrix()
				glMultMatrix( matrix )

				tex = None
				if self.texture is not None:
					tex = self.texture.texture

				if tex is not None:
					glEnable( GL_TEXTURE_2D )
					glBindTexture( GL_TEXTURE_2D, tex.glTextureHandle )

				b = self.brightness

				glColor4d( b, b, b, 1.0 - self.opacity )

				if tex is not None:
					glBegin( GL_QUADS )
				else:
					glBegin( GL_LINE_LOOP )

				if tex is not None:
					glTexCoord2d( 0.0, 0.0 )
				glVertex3d( -0.5, -0.5, 0.0 )

				if tex is not None:
					glTexCoord2d( 1.0, 0.0 )
				glVertex3d( 0.5, -0.5, 0.0 )

				if tex is not None:
					glTexCoord2d( 1.0, 1.0 )
				glVertex3d( 0.5, 0.5, 0.0 )

				if tex is not None:
					glTexCoord2d( 0.0, 1.0 )
				glVertex3d( -0.5, 0.5, 0.0 )

				glEnd()


				if tex is not None:
					glDisable( GL_TEXTURE_2D )

				glPopMatrix()

				if self.bBackfaceCull:
					glDisable( GL_CULL_FACE )


	def setSizeToPreferredSize(self):
		self.size = self.computePreferredSize()


	def computePreferredSize(self):
		if self.texture is not None:
			tex = self.texture.texture

			if tex is not None:
				size = tex.size

				if size is not None:
					if size.x > 0  and  size.y > 0:
						if size.x > size.y:
							return Vector2( 100.0, ( size.y * 100.0 )  /  size.x )
						else:
							return Vector2( ( size.x * 100.0 )  /  size.y, 100.0 )
		return Vector2( 100.0, 100.0 )


	def _p_onTextureChanged(self):
		self.textureChangedSignal.emit( self )




class BackgroundImageList (Viewport3dPainter):
	trackerClass = BackgroundImageCommandTracker.BackgroundImageListCommandTracker

	imageTextureChangedSignal = ClassSignal()
	imageAddedSignal = ClassSignal()
	imageInsertedSignal = ClassSignal()
	imageRemoveSignal = ClassSignal()
	imageRemovedSignal = ClassSignal()

	def __init__(self):
		self._images = []
		self._viewports = []

		self._commandTracker_ = None


	def newImage(self):
		image = BackgroundImage()
		self.appendImage( image )
		return image


	def appendImage(self, image):
		image.textureChangedSignal.connect( self._p_onImageTextureChanged )
		image.cellModifiedSignal.connect( self._p_onImageModified )
		self._images.append( image )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onImageListImageAppended( self, image )
		self.imageAddedSignal.emit( self, image )
		self._p_postRedraw()


	def insertImage(self, index, image):
		image.textureChangedSignal.connect( self._p_onImageTextureChanged )
		image.cellModifiedSignal.connect( self._p_onImageModified )
		self._images.insert( index, image )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onImageListImageInserted( self, index, image )
		self.imageInsertedSignal.emit( self, index, image )
		self._p_postRedraw()


	def removeImage(self, image):
		assert image in self._images
		self.imageRemoveSignal.emit( self, image )
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onImageListImageRemoved( self, image )
		image.cellModifiedSignal.disconnect( self._p_onImageModified )
		image.textureChangedSignal.disconnect( self._p_onImageTextureChanged )
		self._images.remove( image )
		self.imageRemovedSignal.emit( self, image )
		self._p_postRedraw()


	def clear(self):
		while len( self._images ) > 0:
			self.removeImage( self._images[-1] )


	def evPaint3d(self, viewport, paintLayer):
		for image in self._images:
			image.paint3d( paintLayer )


	def __getitem__(self, index):
		return self._images[index]

	def __len__(self):
		return len( self._images )

	def __iter__(self):
		return iter( self._images )

	def __contains__(self, image):
		return image in self._images

	def index(self, image):
		return self._images.index( image )



	def __readxml__(self, xmlNode):
		self.clear()
		for node in xmlNode.childrenNamed( 'image' ):
			image = self.newImage()
			node  >>  image

	def __writexml__(self, xmlNode):
		for image in self._images:
			xmlNode.addChild( 'image' )  <<  image



	def painterAttachedToViewport(self, viewport):
		super( BackgroundImageList, self ).painterAttachedToViewport( viewport )
		self._viewports.append( viewport )


	def painterDetachedFromViewport(self, viewport):
		super( BackgroundImageList, self ).painterDetachedFromViewport( viewport )
		self._viewports.remove( viewport )


	def _p_postRedraw(self):
		for viewport in self._viewports:
			viewport.postRedraw()


	def _p_onImageModified(self, image):
		self._p_postRedraw()

	def _p_onImageTextureChanged(self, image):
		self.imageTextureChangedSignal.emit( self, image )


ioObjectFactoryRegister( 'BackgroundImageList', BackgroundImageList )

