##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
"Linked list utility module"


class LinkedListNode (object):
	"Linked list node class"
	def __init__(self):
		self.prev = None
		self.next = None

class LinkedList (object):
	"Linked list class"
	def __init__(self, nodeFunction):
		"""Constructor:
		@nodeFunciton - a function which will retreive the desired LinkedListNode object from the list entries (pointed to by the @prev and @next attributes of LinkedListNode"""
		self.head = None
		self.tail = None
		self._nodeFunction = nodeFunction

	def isEmpty(self):
		"Determine if the list is empty"
		return self.head == None

	def append(self, object):
		"Append object to the list"
		if self.head == None:
			self.head = object

		if self.tail != None:
			self._nodeFunction( self.tail ).next = object

		self._nodeFunction( object ).prev = self.tail
		self.tail = object

	def remove(self, object):
		"Remove object from the list"
		objectNode = self._nodeFunction( object )

		if objectNode.prev != None:
			self._nodeFunction( objectNode.prev ).next = objectNode.next

		if objectNode.next != None:
			self._nodeFunction( objectNode.next ).prev = objectNode.prev

		if self.head == object:
			self.head = objectNode.next

		if self.tail == object:
			self.tail = objectNode.prev

		objectNode.prev = None
		objectNode.next = None
