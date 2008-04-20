##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.UI.UIAction import UIAction

undoAction = UIAction( 'General.UndoRedo.Undo', ( _( 'General' ), _( 'Undo / redo' ), _( 'Undo' ) ), accelerator='<ctrl>z', tooltip=_( 'Undo' ) )
redoAction = UIAction( 'General.UndoRedo.Redo', ( _( 'General' ), _( 'Undo / redo' ), _( 'Redo' ) ), accelerator='<ctrl><shift>z', tooltip=_( 'Redo' ) )
