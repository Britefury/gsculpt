//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PAINTLAYER_H__
#define PAINTLAYER_H__

enum PaintLayer
{												//	DEPTH TEST			DEPTH WRITE		BLEND
	PAINTLAYER_FOREGROUND,						//	X					X
	PAINTLAYER_TRANSPARENCY,						//										X
	PAINTLAYER_WIREFRAME,							//	X					X
	PAINTLAYER_OVERLAY_BLEND,						//	X					X				X
	PAINTLAYER_OVERLAY_NODEPTH,					//
	PAINTLAYER_OVERLAY_BLEND_NODEPTH				//										X
};


#define PAINTLAYER_INIT						PAINTLAYER_FOREGROUND
#define PAINTLAYER_BACKGROUND_IMAGE		PAINTLAYER_FOREGROUND
#define PAINTLAYER_GRID						PAINTLAYER_FOREGROUND
#define PAINTLAYER_OBJECTS					PAINTLAYER_FOREGROUND


#endif

