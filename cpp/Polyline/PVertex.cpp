//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PVERTEX_CPP__
#define PVERTEX_CPP__

#include <Polyline/PVertex.h>





PVertex::PVertex()
{
	flags.mark = false;
	flags.normalSharp = false;
	flags.textureBreak = false;
}

PVertex::PVertex(const Point3 &pos, bool marked)
{
	position = pos;
	flags.mark = marked;
	flags.normalSharp = false;
	flags.textureBreak = false;
}



void PVertex::read(FILE *f)
{
	position.read( f );
	fread( &flags, sizeof(flags), 1, f );
}

void PVertex::write(FILE *f) const
{
	position.write( f );
	fwrite( &flags, sizeof(flags), 1, f );
}




void PVertex::mark()
{
	flags.mark = true;
}

void PVertex::unmark()
{
	flags.mark = false;
}

void PVertex::setMark(bool marked)
{
	flags.mark = marked;
}

bool PVertex::isMarked() const
{
	return flags.mark;
}



void PVertex::setNormalSharp(bool sharp)
{
	flags.normalSharp = sharp;
}

bool PVertex::isNormalSharp() const
{
	return flags.normalSharp;
}



void PVertex::setTextureBreak(bool texBreak)
{
	flags.textureBreak = texBreak;
}

bool PVertex::isTextureBreak() const
{
	return flags.textureBreak;
}



void PVertex::copyPropertiesFrom(const PVertex &src)
{
	flags.mark = src.flags.mark;
	flags.normalSharp = src.flags.normalSharp;
	flags.textureBreak = src.flags.textureBreak;
}



void PVertex::setPosition(const Point3 &pos)
{
	position = pos;
}

const Point3 &PVertex::getPosition() const
{
	return position;
}



void PVertex::copyFrom(const PVertex &v)
{
	position = v.position;
	positionSave = v.positionSave;
	flags = v.flags;
}


void PVertex::transform(const Matrix4 &m)
{
	position *= m;
}

void PVertex::transform(const Transformation *x)
{
	position = x->transformPoint( position );
}


int PVertex::mark(MarkPredicate predicate, const Region3d *region)
{
	bool alreadyMarked = flags.mark;
	bool inRegion = isInside( region );

	flags.mark = markPredicateIsMarked( predicate, alreadyMarked, inRegion );

	if ( alreadyMarked  &&  !flags.mark )
	{
		return -1;
	}
	else if ( !alreadyMarked  &&  flags.mark )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int PVertex::mark(MarkPredicate predicate)
{
	bool alreadyMarked = flags.mark;

	flags.mark = markPredicateIsMarked( predicate, alreadyMarked, true );

	if ( alreadyMarked  &&  !flags.mark )
	{
		return -1;
	}
	else if ( !alreadyMarked  &&  flags.mark )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool PVertex::isInside(const Region3d *region) const
{
	return region->contains( position );
}



void PVertex::savePosition()
{
	positionSave = position;
}

void PVertex::restorePosition()
{
	position = positionSave;
}


void PVertex::saveMark()
{
	flags.markSave = flags.mark;
}

int PVertex::restoreMark()
{
	bool oldMark = flags.mark;
	flags.mark = flags.markSave;

	if ( oldMark  &&  !flags.mark )
	{
		return -1;
	}
	else if ( !oldMark  &&  flags.mark )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



#endif
