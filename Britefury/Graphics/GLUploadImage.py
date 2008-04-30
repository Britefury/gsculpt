##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.GL.GL import *
from Britefury.GL.GLU import *




def glUploadImage(image, textureHandle):
	width, height = image.size
	mode = image.mode

	if mode == 'P':
		image = image.convert()
		mode = image.mode
	elif mode == 'CMYK'  or  mode == 'YCbCr':
		image = image.convert( 'RGB' )
		mode = image.mode

	format = None
	if mode == 'L':
		format = GL_LUMINANCE
	elif mode == 'LA':
		format = GL_LUMINANCE_ALPHA
	elif mode == 'RGB':
		format = GL_RGB
	elif mode == 'RGBA':
		format = GL_RGBA
	else:
		raise ValueError, 'GL image: pixel format not supported'

	gltype = GL_UNSIGNED_BYTE



	glPixelStorei( GL_PACK_ALIGNMENT, 1 )
	glBindTexture( GL_TEXTURE_2D, textureHandle )
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT )
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT )
	#glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR )
	#glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST )
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST )
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST )

	gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, width, height, format, gltype, image.tostring() )
