//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LOGSCALE_H__
#define LOGSCALE_H__

#include <math.h>

#include <Util/Array.h>





class GS_DllExport LogScale
{
public:
	class GS_DllExport Level
	{
	private:
		const LogScale *ls;
		int major, minor;

	public:
		Level();
		Level(const LogScale *l, int maj, int min);


		Level &operator++();
		Level operator++(int);
		Level &operator+=(int i);
		Level operator+(int i) const;
		Level &operator--();
		Level operator--(int);
		Level &operator-=(int i);
		Level operator-(int i) const;

		double getValue() const;
		double getPower() const;
	};

private:
	double base, oneOverLogBase;
	Array<double> powers;

public:
	LogScale();
	LogScale(double bs, const Array<double> &pwr);
	~LogScale();



	Level getSmaller(double v) const;
	Level getNearest(double v) const;
	Level getLarger(double v) const;

private:
	Level find(double pwr) const;


	friend class LogScale::Level;
};




#endif
