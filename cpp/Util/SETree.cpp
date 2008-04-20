//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef SETREE_CPP__
#define SETREE_CPP__

#include <sstream>

#include <Util/gsassert.h>

#include <Util/SETree.h>





static SENull nullNode;



//
// SENode
//

SENode::~SENode()
{
}


void SENode::operator<<(const char *value)
{
	if ( !isNull() )
	{
		setString( value );
	}
}

void SENode::operator<<(const std::string &value)
{
	if ( !isNull() )
	{
		setString( value );
	}
}

void SENode::operator<<(const bool &value)
{
	if ( !isNull() )
	{
		if ( value )
		{
			setString( "T" );
		}
		else
		{
			setString( "F" );
		}
	}
}

void SENode::operator<<(const int &value)
{
	if ( !isNull() )
	{
		std::ostringstream stream;
		stream << value;
		setString( stream.str() );
	}
}

void SENode::operator<<(const float &value)
{
	if ( !isNull() )
	{
		std::ostringstream stream;
		stream.precision( 8 );
		stream << value;
		setString( stream.str() );
	}
}

void SENode::operator<<(const double &value)
{
	if ( !isNull() )
	{
		std::ostringstream stream;
		stream.precision( 17 );
		stream << value;
		setString( stream.str() );
	}
}


void SENode::operator>>(std::string &value) const
{
	if ( !isNull() )
	{
		value = getString();
	}
}

void SENode::operator>>(bool &value) const
{
	if ( !isNull() )
	{
		value = getString() == "T";
	}
}

void SENode::operator>>(int &value) const
{
	if ( !isNull() )
	{
		value = atoi( getString().c_str() );
	}
}

void SENode::operator>>(float &value) const
{
	if ( !isNull() )
	{
		value = atof( getString().c_str() );
	}
}

void SENode::operator>>(double &value) const
{
	if ( !isNull() )
	{
		value = atof( getString().c_str() );
	}
}





//
// SENull
//

SENull::~SENull()
{
}


SENode * SENull::clone() const
{
	return new SENull();
}


void SENull::write(std::string &buffer, int &lastCR)
{
}

bool SENull::isNull() const
{
	return true;
}

const std::string & SENull::getString() const
{
	static std::string str( "" );
	return str;
}

void SENull::setString(const std::string &str)
{
}

SENode & SENull::operator[](int i)
{
	return nullNode;
}

const SENode & SENull::operator[](int i) const
{
	return nullNode;
}

SENode & SENull::addItem()
{
	return nullNode;
}

SENode & SENull::addList()
{
	return nullNode;
}



//
// SEItem
//

SEItem::SEItem()
{
}

SEItem::SEItem(const SEItem &node)
			: str( node.str )
{
}

SEItem::SEItem(const std::string &str)
			: str( str )
{
}

SEItem::~SEItem()
{
}


SENode * SEItem::clone() const
{
	return new SEItem( *this );
}


void SEItem::write(std::string &buffer, int &lastCR)
{
	if ( str == "" )
	{
		buffer += "\"\"";
	}
	else if ( str.find( " " ) != std::string::npos  ||  str.find( "\t" ) != std::string::npos  ||  str.find( "\n" ) != std::string::npos )
	{
		std::string forBuffer = str;
		size_t index = forBuffer.find( '"' );
		while ( index != std::string::npos )
		{
			forBuffer.replace( index, index + 1, "\\\"" );
			index = forBuffer.find( '"' );
		}
		buffer += forBuffer;
	}
	else
	{
		buffer += str;
	}
}

bool SEItem::isNull() const
{
	return false;
}

const std::string & SEItem::getString() const
{
	return str;
}

void SEItem::setString(const std::string &str)
{
	this->str = str;
}

SENode & SEItem::operator[](int i)
{
	return nullNode;
}

const SENode & SEItem::operator[](int i) const
{
	return nullNode;
}

SENode & SEItem::addItem()
{
	return nullNode;
}

SENode & SEItem::addList()
{
	return nullNode;
}




//
// SEList
//

SEList::SEList()
{
}

SEList::SEList(const SEList &node)
{
	for (int nodeI = 0; nodeI < node.nodes.size(); nodeI++)
	{
		nodes.push_back( node.nodes[nodeI]->clone() );
	}
}

SEList::~SEList()
{
	for (int nodeI = 0; nodeI < nodes.size(); nodeI++)
	{
		delete nodes[nodeI];
	}
}


SENode * SEList::clone() const
{
	return new SEList( *this );
}


void SEList::write(std::string &buffer, int &lastCR)
{
	buffer += "(";
	for (int nodeI = 0; nodeI < nodes.size(); nodeI++)
	{
		if ( nodeI > 0 )
		{
			buffer += " ";
		}
		nodes[nodeI]->write( buffer, lastCR );
	}
	buffer += ")";
	if ( static_cast<int>( buffer.length() ) > ( lastCR + 256 ) )
	{
		buffer += "\n";
		lastCR = buffer.length();
	}
}

bool SEList::isNull() const
{
	return false;
}

const std::string & SEList::getString() const
{
	static std::string empty;
	return empty;
}

void SEList::setString(const std::string &str)
{
}

SENode & SEList::operator[](int i)
{
	if ( i >= 0  &&  i < nodes.size() )
	{
		return *nodes[i];
	}
	else
	{
		return nullNode;
	}
}

const SENode & SEList::operator[](int i) const
{
	if ( i >= 0  &&  i < nodes.size() )
	{
		return *nodes[i];
	}
	else
	{
		return nullNode;
	}
}

SENode & SEList::addItem()
{
	SENode *node = new SEItem();
	nodes.push_back( node );
	return *node;
}

SENode & SEList::addList()
{
	SENode *node = new SEList();
	nodes.push_back( node );
	return *node;
}







//
// SETree
//

SETree::SETree()
{
}

SETree::SETree(const SETree &t)
			: tree( t.tree )
{
}

SETree::~SETree()
{
}


SEList & SETree::getNode()
{
	return tree;
}


void SETree::read(const std::string &buffer)
{
	parseString( buffer );
	gs_assert( hierarchy.size() == 0, "SETree::read(): hierarchy non-empty; contains %d elements\n", hierarchy.size() );
}

std::string SETree::write()
{
	std::string buffer;
	int lastCR = 0;
	tree.write( buffer, lastCR );
	return buffer;
}



bool SETree::evBeginList()
{
	if ( hierarchy.size() == 0 )
	{
		hierarchy.push_back( &tree );
	}
	else
	{
		SEList *top = hierarchy.back();
		SEList *newTop = static_cast<SEList*>( &top->addList() );
		hierarchy.push_back( newTop );
	}

	return true;
}

bool SETree::evEndList()
{
	if ( hierarchy.size() > 0 )
	{
		hierarchy.pop_back();
		return true;
	}
	else
	{
		return false;
	}
}

void SETree::evItem(const std::string &str)
{
	hierarchy.back()->addItem().setString( str );
}




const char * SETree::consumeString(const char *buffer, const char *end)
{
	std::string item;
	// Consume '"'
	buffer++;

	while ( *buffer != '"' )
	{
		if ( *buffer == '\\' )
		{
			// Skip escape character
			buffer++;
		}
		else if ( *buffer == (char)0x00  ||  buffer >= end )
		{
			evItem( item );
			return buffer;
		}
		item += *buffer;
		buffer++;
	}
	buffer++;

	evItem( item );

	return buffer;
}

void SETree::sendItem(std::string &item)
{
	if ( item != "" )
	{
		evItem( item );
		item = "";
	}
}



void SETree::parseString(const std::string &str)
{
	const char *buffer = str.c_str();
	const char *end = buffer + str.length();

	std::string item;

	while ( *buffer != (char)0x00  &&  buffer < end )
	{
		if ( *buffer == '(' )
		{
			sendItem( item );
			if ( !evBeginList() )
			{
				break;
			}
			buffer++;
		}
		else if ( *buffer == ')' )
		{
			sendItem( item );
			if ( !evEndList() )
			{
				break;
			}
			buffer++;
		}
		else if ( *buffer == ' '  ||  *buffer == '\t'  ||  *buffer == '\n' )
		{
			sendItem( item );
			buffer++;
		}
		else if ( *buffer == '"' )
		{
			buffer = consumeString( buffer, end );
			item = "";
		}
		else
		{
			item += *buffer;
			buffer++;
		}
	}
	sendItem( item );
}



#endif
