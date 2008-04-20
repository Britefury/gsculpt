##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Tool.Tool import Tool

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.SheetEdit import *




class SheetEditorToolClass (SheetEditorClass):
	pass






class SheetEditorTool (SheetEditor, Tool):
	"Function tool base class"
	__metaclass__ = SheetEditorToolClass

	pageTitle = 'tool'
	titleText = 'SheetEditorTool'



	def __init__(self, view, editorSettings, parentWindow, commandHistory):
		"Constructor"
		SheetEditor.__init__( self, view, editorSettings, parentWindow, commandHistory )
		Tool.__init__( self, view, parentWindow )

		self._o_packContents( self.getSheetEditorWidget() )

		self.setTitleWidgetText( self.titleText )

		self._pageTitle = self.pageTitle



	def initialiseTool(self):
		pass


	def shutdownTool(self):
		super( SheetEditorTool, self ).shutdownTool()
		self.shutdownSheetEditor()




	def _o_installAccelerators(self, accels):
		super( SheetEditorTool, self )._o_installAccelerators( accels )
		self.sheetEditorInstallAccelerators( accels )


	def _o_uninstallAccelerators(self, accels):
		super( SheetEditorTool, self )._o_uninstallAccelerators( accels )
		self.sheetEditorUninstallAccelerators( accels )



	def _o_installEventHandlers(self):
		super( SheetEditorTool, self )._o_installEventHandlers()
		self.sheetEditorInstallEventHandlers()

	def _o_uninstallEventHandlers(self):
		super( SheetEditorTool, self )._o_uninstallEventHandlers()
		self.sheetEditorUninstallEventHandlers()



