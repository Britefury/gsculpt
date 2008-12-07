//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef OBJSTRINGUTILS_CPP__
#define OBJSTRINGUTILS_CPP__

#include <math.h>
#include <assert.h>

#include <ImportExportFilter/ObjImport/ObjStringUtils.h>


#define INV_PWR_10_TABLE_SIZE 309

bool bInversePowersOf10Init = false;
double inversePowersOf10[INV_PWR_10_TABLE_SIZE];

static double * getInversePowersOf10()
{
	if ( !bInversePowersOf10Init )
	{
		double p = 10.0;
		for (int i = 0; i < INV_PWR_10_TABLE_SIZE; i++)
		{
			inversePowersOf10[i] = 1.0 / p;
			p *= 10.0;
		}
		bInversePowersOf10Init = true;
	}

	return inversePowersOf10;
}




int strToInt(const char *data)
{
	bool bNegate = false;
	int x = 0;
	if ( *data == '-' )
	{
		bNegate = true;
		data++;
	}
	else if ( *data == '+' )
	{
		data++;
	}
	while ( *data >= '0'  &&  *data <= '9' )
	{
		x = x * 10  +  ( *data - '0' );
		data++;
	}
	return bNegate  ?  -x  :  x;
}



double strToDouble(const char *data)
{
	bool bNegate = false;
	double x = 0.0;
	
	// Handle leading - or +
	if ( *data == '-' )
	{
		bNegate = true;
		data++;
	}
	else if ( *data == '+' )
	{
		data++;
	}

	// Read a sequence of digits before the decimal point
	while ( *data >= '0'  &&  *data <= '9' )
	{
		x = x * 10.0  +  (double)( *data - '0' );
		data++;
	}

	if ( *data == '.' )
	{
		// Found a decimal point
		data++;

		// Read a sequence of digits after the decimal point
		// Each digit is converted to a double, then multiplied by 10^-position
		// The inverse powers of 10 are stored in an array, obtained from getInversePowersOf10()
		double *invPwr10 = getInversePowersOf10();
		// The inverse powers of 10 array is of size INV_PWR_10_TABLE_SIZE
		// Going beyond this would not make sense, as we would go below the limits of the minimum number that a double can store
		// Compute the last character that we should go to, and stop there if we have to
		const char *end = data + INV_PWR_10_TABLE_SIZE;
		while ( *data >= '0'  &&  *data <= '9'  &&  data != end )
		{
			x += (double)( *data - '0' ) * *invPwr10;
			invPwr10++;
			data++;
		}
	}


	// Read the exponent
	if ( *data == 'e'  ||  *data == 'E' )
	{
		data++;

		int exponent = strToInt( data );

		x *= pow( 10, (double)exponent );
	}

	return bNegate  ?  -x  :  x;
}




void testObjStringUtils()
{
	// test strToInt()
	assert( strToInt( "1" ) == 1 );
	assert( strToInt( "0" ) == 0 );
	assert( strToInt( "9" ) == 9 );
	
	assert( strToInt( "10" ) == 10 );
	assert( strToInt( "00" ) == 0 );
	assert( strToInt( "90" ) == 90 );
	assert( strToInt( "19" ) == 19 );

	assert( strToInt( "+1" ) == 1 );
	assert( strToInt( "+0" ) == 0 );
	assert( strToInt( "+9" ) == 9 );
	
	assert( strToInt( "+10" ) == 10 );
	assert( strToInt( "+00" ) == 0 );
	assert( strToInt( "+90" ) == 90 );
	assert( strToInt( "-19" ) == -19 );

	assert( strToInt( "-1" ) == -1 );
	assert( strToInt( "-0" ) == 0 );
	assert( strToInt( "-9" ) == -9 );
	
	assert( strToInt( "-10" ) == -10 );
	assert( strToInt( "-00" ) == 0 );
	assert( strToInt( "-90" ) == -90 );
	assert( strToInt( "-19" ) == -19 );



	// test strToDouble()
	assert( strToDouble( "2.0" )  ==  2.0 );
	assert( strToDouble( "20.0" )  ==  20.0 );
	assert( strToDouble( "0.0" )  ==  0.0 );
	assert( strToDouble( "2." )  ==  2.0 );
	assert( strToDouble( "20." )  ==  20.0 );
	assert( strToDouble( "0." )  ==  0.0 );
	assert( strToDouble( ".2" )  ==  0.2 );
	assert( strToDouble( ".02" )  ==  0.02 );
	assert( strToDouble( ".0" )  ==  0.0 );

	assert( strToDouble( "+2.0" )  ==  2.0 );
	assert( strToDouble( "+20.0" )  ==  20.0 );
	assert( strToDouble( "+0.0" )  ==  0.0 );
	assert( strToDouble( "+2." )  ==  2.0 );
	assert( strToDouble( "+20." )  ==  20.0 );
	assert( strToDouble( "+0." )  ==  0.0 );
	assert( strToDouble( "+.2" )  ==  0.2 );
	assert( strToDouble( "+.02" )  ==  0.02 );
	assert( strToDouble( "+.0" )  ==  0.0 );
	
	assert( strToDouble( "-2.0" )  ==  -2.0 );
	assert( strToDouble( "-20.0" )  ==  -20.0 );
	assert( strToDouble( "-0.0" )  ==  0.0 );
	assert( strToDouble( "-2." )  ==  -2.0 );
	assert( strToDouble( "-20." )  ==  -20.0 );
	assert( strToDouble( "-0." )  ==  0.0 );
	assert( strToDouble( "-.2" )  ==  -0.2 );
	assert( strToDouble( "-.02" )  ==  -0.02 );
	assert( strToDouble( "-.0" )  ==  0.0 );

	assert( strToDouble( "1.2e12" )  ==  1.2e12 );
	assert( strToDouble( "1.2e-12" )  ==  1.2e-12 );
	assert( strToDouble( "+1.2e12" )  ==  1.2e12 );
	assert( strToDouble( "+1.2e-12" )  ==  1.2e-12 );
	assert( strToDouble( "-1.2e12" )  ==  -1.2e12 );
	assert( strToDouble( "-1.2e-12" )  ==  -1.2e-12 );
}


#endif

