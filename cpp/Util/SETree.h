//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef SETREE_H__
#define SETREE_H__

#include <string>

#include <Util/Array.h>




// When writing:
//	Receive a @SENode as a parameter:
//		To write a primitive value:
//			node.addItem()  <<  l-value		(char*, std::string, bool, int, float, or double)
//		To write a composite value:
//			SENode &subNode = node.addList()
//
//
// When reading:
//	Receive a @SENode as a parameter:
//		To read a primitive value from the sub-node at @index:
//			node[index]  >>  variable
//		To read a composite value from the sub-node as @index:
//			SENOde &subNode = node[index]





//
// SENode
//

class GS_DllExport SENode
{
public:
	virtual ~SENode();

	virtual SENode * clone() const = 0;


	virtual void write(std::string &buffer, int &lastCR) = 0;

protected:
	virtual bool isNull() const = 0;
	virtual const std::string & getString() const = 0;
	virtual void setString(const std::string &str) = 0;

public:
	virtual SENode & operator[](int i) = 0;
	virtual const SENode & operator[](int i) const = 0;
	virtual SENode & addItem() = 0;
	virtual SENode & addList() = 0;

	void operator<<(const char *value);
	void operator<<(const std::string &value);
	void operator<<(const bool &value);
	void operator<<(const int &value);
	void operator<<(const float &value);
	void operator<<(const double &value);

	void operator>>(std::string &value) const;
	void operator>>(bool &value) const;
	void operator>>(int &value) const;
	void operator>>(float &value) const;
	void operator>>(double &value) const;

	friend class SEList;
	friend class SETree;
};



//
// SENull
//

class GS_DllExport SENull : public SENode
{
public:
	virtual ~SENull();

	virtual SENode * clone() const;


	virtual void write(std::string &buffer, int &lastCR);

protected:
	virtual bool isNull() const;
	virtual const std::string & getString() const;
	virtual void setString(const std::string &str);

public:
	virtual SENode & operator[](int i);
	virtual const SENode & operator[](int i) const;
	virtual SENode & addItem();
	virtual SENode & addList();
};



//
// SEItem
//

class GS_DllExport SEItem : public SENode
{
private:
	std::string str;


public:
	SEItem();
	SEItem(const SEItem &node);
	SEItem(const std::string &str);
	virtual ~SEItem();

	virtual SENode * clone() const;


	virtual void write(std::string &buffer, int &lastCR);

protected:
	virtual bool isNull() const;
	virtual const std::string & getString() const;
	virtual void setString(const std::string &str);

public:
	virtual SENode & operator[](int i);
	virtual const SENode & operator[](int i) const;
	virtual SENode & addItem();
	virtual SENode & addList();
};



//
// SEList
//

class GS_DllExport SEList : public SENode
{
private:
	Array<SENode*> nodes;


public:
	SEList();
	SEList(const SEList &node);
	virtual ~SEList();

	virtual SENode * clone() const;


	virtual void write(std::string &buffer, int &lastCR);

protected:
	virtual bool isNull() const;
	virtual const std::string & getString() const;
	virtual void setString(const std::string &str);

public:
	virtual SENode & operator[](int i);
	virtual const SENode & operator[](int i) const;
	virtual SENode & addItem();
	virtual SENode & addList();

	friend class SETree;
};




//
// SETree
//

class GS_DllExport SETree
{
private:
	SEList tree;
	Array<SEList*> hierarchy;


public:
	SETree();
	SETree(const SETree &t);
	~SETree();


	SEList & getNode();


	void read(const std::string &buffer);
	std::string write();


private:
	bool evBeginList();
	bool evEndList();
	void evItem(const std::string &str);

	const char * consumeString(const char *buffer, const char *end);
	void sendItem(std::string &item);
	void parseString(const std::string &str);
};




#endif
