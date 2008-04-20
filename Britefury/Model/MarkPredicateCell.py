##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.FileIO.IOXml import IOXmlEnum

from Britefury.Cell.Cell import *

from Britefury.Sheet.Sheet import *

from Britefury.Model.Model import MarkPredicate




# XML IO for MarkPredicate

markPredIOXmlEnum = IOXmlEnum( MarkPredicate, 'MarkPredicate', MarkPredicate.MARK )
markPredIOXmlEnum.value( MarkPredicate.MARK, 'mark' )
markPredIOXmlEnum.value( MarkPredicate.UNMARK, 'unmark' )
markPredIOXmlEnum.value( MarkPredicate.ADD, 'add' )
markPredIOXmlEnum.value( MarkPredicate.FILTER, 'filter' )
markPredIOXmlEnum.value( MarkPredicate.INVERT, 'invert' )

ioXmlReadMarkPredicateProp = markPredIOXmlEnum.propReader
ioXmlReadMarkPredicateNode = markPredIOXmlEnum.nodeReader
ioXmlWriteMarkPredicateProp = markPredIOXmlEnum.propWriter
ioXmlWriteMarkPredicateNode = markPredIOXmlEnum.nodeWriter


# Cell for MarkPredicate

class MarkPredicateCell (Cell):
	valueClass = MarkPredicate
	bAllowNone = False

registerCellType( MarkPredicate, MarkPredicateCell )


