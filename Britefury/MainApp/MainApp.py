##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os
import sys

import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.FileIO.IOXml import *

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Config import GSConfig

from Britefury.CommandHistory.CommandHistory import CommandHistory

from Britefury.WorkArea import DrawingArea, ModellerView
from Britefury.WorkArea.DrawingArea import DrawingAreaSharingManager

from Britefury.UI import ComboBox
from Britefury.UI.ComboBox import ComboBox
from Britefury.UI.AcceleratorManager import AcceleratorManager
from Britefury.UI.ToggleButton import ToggleButton
from Britefury.UI.RadioGroup import RadioGroup
from Britefury.UI import GSImage
from Britefury.UI.NameDialog import nameDialog
from Britefury.UI.UIAction import UIAction, UIActionInstanceSignal
from Britefury.UI.MessageDialog import messageDialog
from Britefury.UI.ConfirmOverwriteFileDialog import confirmOverwriteFileDialog

from Britefury.gSculptConfig import gSculptUserConfig
from Britefury.gSculptConfig import gSculptConfig

from Britefury.Event.QueuedEvent import queueEvent

from Britefury.Tool import ToolPanel
from Britefury.Tool.ToolPanel import ToolPanel, ToolPanelPage

from Britefury.Project.GSProject import GSProject
from Britefury.Project.GSProjectCommandTracker import GSProjectCommandTracker

from Britefury.SceneEdit.NodeEditor import NodeEditor
from Britefury.SceneEdit.SceneEditorTools import SceneEditorTools

from Britefury.Texture import TextureTable

from Britefury.View.BackgroundImage import BackgroundImageList
from Britefury.View.BackgroundImageTool import BackgroundImageTool
from Britefury.View.ModelLayerEditor import ModelLayerEditorWindow
from Britefury.View.ModelLayerEditor import ModelLayerEditorWindow
from Britefury.View.ModelLayerBackgroundPainter import ModelLayerBackgroundPainter
from Britefury.View.FocusTool import FocusTool
from Britefury.View.DrawingPlane import DrawingPlaneTool
from Britefury.View.ViewDrawingSettings import *
from Britefury.View.ViewDrawingSettingsEditor import ViewDrawingSettingsEditorWindow

from Britefury.Editor import EditorSettings
from Britefury.Editor import SnapSettingsPopup

from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter
from Britefury.WorkArea.GLResourceGuard import GLResourceGuard
from Britefury.WorkArea.ViewLayoutSingle import ViewLayoutSingle
from Britefury.WorkArea.ViewLayoutQuad import ViewLayoutQuad
from Britefury.WorkArea.ViewLayoutNotebook import ViewLayoutNotebook

from Britefury.extlibs.pyconsole.pyconsole import Console

from Britefury.Model.Model import GSProductModelBlank
from Britefury.Mesh.Mesh import GSProductMesh
from Britefury.Group.Group import GSProductGroup

from Britefury.PrimitiveTools.PrimitiveToolbox import PrimitiveToolbox
from Britefury.MeshTools.MeshToolbox import MeshToolbox
from Britefury.GroupTools.GroupToolbox import GroupToolbox

from Britefury.SceneNodes.SceneModelNode import SceneModelNode

from Britefury.ImportExportFilter import ObjImport, ObjExport
from Britefury.ImportExportFilter import VrmlExport

from Britefury.MainApp.CreateMenu import CreateMenu
from Britefury.MainApp.NewNodeMenu import NewNodeMenu

from Britefury.MainApp.Shortcuts.Shortcut import *
from Britefury.MainApp.Shortcuts.ShortcutEditor import ShortcutTableEditorWindow

from Britefury.MainApp.Plugins.PluginTable import *
from Britefury.MainApp.Plugins.PluginManager import *

from Britefury.MainApp import GeneralActions




_layoutAction = UIAction( 'View.SwitchLayout', ( _( 'View' ), _( 'Switch layout' ) ), accelerator='<ctrl>w', tooltip=_( 'Switch view layout' ) )
_focusAction = UIAction( 'View.Focus', ( _( 'View' ), _( 'Focus' ) ), accelerator='<ctrl>f', tooltip=_( 'Focus viewport' ) )
_drawingPlaneAction = UIAction( 'General.DrawingPlaneToggle', ( _( 'General' ), _( 'Toggle drawing plane' ) ), accelerator='<ctrl>d', tooltip=_( 'Toggle drawing plane' ) )




class ToolboxTable (object):
	def __init__(self):
		self._toolboxes = {}
		self._toolboxesInUse = set()


	def registerToolbox(self, productClass, toolbox):
		self._toolboxes[productClass] = toolbox

	def takeToolbox(self, productClass):
		try:
			toolbox = self._toolboxes[productClass]
		except KeyError:
			return None
		assert toolbox not in self._toolboxesInUse
		self._toolboxesInUse.add( toolbox )
		return toolbox

	def returnToolbox(self, toolbox):
		assert toolbox in self._toolboxesInUse
		self._toolboxesInUse.remove( toolbox )



class BackgroundImagePainter (Viewport3dPainter):
	def __init__(self):
		self._backgroundImageList = None
		self._viewports = []

	def painterAttachedToViewport(self, viewport):
		super( BackgroundImagePainter, self ).painterAttachedToViewport( viewport )
		if self._backgroundImageList is not None:
			self._backgroundImageList.painterAttachedToViewport( viewport )
		self._viewports.append( viewport )


	def painterDetachedFromViewport(self, viewport):
		super( BackgroundImagePainter, self ).painterDetachedFromViewport( viewport )
		if self._backgroundImageList is not None:
			self._backgroundImageList.painterDetachedFromViewport( viewport )
		self._viewports.remove( viewport )


	def evPaint3d(self, viewport, paintLayer):
		super( BackgroundImagePainter, self ).evPaint3d( viewport, paintLayer )
		if self._backgroundImageList is not None:
			self._backgroundImageList.evPaint3d( viewport, paintLayer )


	def _p_setBackgroundImageList(self, imageList):
		if self._backgroundImageList is not None:
			for viewport in self._viewports:
				self._backgroundImageList.painterDetachedFromViewport( viewport )
		self._backgroundImageList = imageList
		if self._backgroundImageList is not None:
			for viewport in self._viewports:
				self._backgroundImageList.painterAttachedToViewport( viewport )


	backgroundImageList = property( None, _p_setBackgroundImageList )



class GSculptScriptEnvironment (object):
	def __init__(self, project, app):
		self._project = project
		self._app = app


	def _p_getProject(self):
		return self._project

	def _p_getApp(self):
		return self._app

	project = property( _p_getProject, doc=_( 'The gSculpt project (a gSculpt.Project.GSProject)' ) )
	app = property( _p_getApp, doc=_( 'The gSculpt app (a gSculpt.MainApp.MainApp)' ) )

	__doc__ = _( """gSculpt Scripting Environment
	GSculptScriptEnvironment(project, app) -> scripting environment with @project used as the gSculpt project, and @app as the app""" )



class MainApp (object):
	destroySignal = ClassSignal()


	def __init__(self, painterFactory, bEnablePluginManager):
		self._project = None
		self._projectFilename = None
		self._commandHistory = CommandHistory()
		self._commandHistory.changedSignal.connect( self._p_onCommandHistoryChanged )
		self._bUnsavedData = False


		self._acceleratorManager = AcceleratorManager()



		self._window = gtk.Window( gtk.WINDOW_TOPLEVEL )
		self._window.set_icon_from_file( GSConfig.gSculptIconPath )
		self._window.connect( 'delete-event', self._p_onDeleteEvent )
		self._window.connect( 'destroy', self._p_onDestroy )
		self._window.set_border_width( 10 )


		sharingManager = DrawingAreaSharingManager()
		sharingManager.initialiseGLResourcesSignal.connect( self._p_onInitialiseGLResouces )
		sharingManager.shutdownGLResourcesSignal.connect( self._p_onShutdownGLResouces )
		self._view = ModellerView.ModellerView()
		self._editorSettings = EditorSettings.EditorSettings()


		self._toolboxTable = ToolboxTable()

		self._backgroundImagePainter = BackgroundImagePainter()

		self._backgroundImagesTool = BackgroundImageTool( self._view, self._editorSettings, self._window, self._commandHistory )
		self._backgroundImagesTool.doneListener = self._p_onBackgroundImagesToolDone

		self._drawingPlaneTool = DrawingPlaneTool( self._view, self._editorSettings, self._window, self._commandHistory )
		self._drawingPlaneTool.doneListener = self._p_onDrawingPlaneToolDone

		self._layerTableEditor = ModelLayerEditorWindow( self._window, self._commandHistory )
		self._layerTableEditor.closeSignal.connect( self._p_onLayerTableEditorClose )


		typeToFactoryAndName = ModellerView.createViewportTypeToFactoryAndNameTable( sharingManager, self._backgroundImagePainter )
		self._singleLayout = ViewLayoutSingle( self._view, typeToFactoryAndName )
		self._quadLayout = ViewLayoutQuad( self._view, typeToFactoryAndName )
		self._notebookLayout = ViewLayoutNotebook( self._view, typeToFactoryAndName, self._acceleratorManager )


		self._layerPainter = ModelLayerBackgroundPainter( painterFactory, self._view )
		self._view.addPainter( self._layerPainter, Viewport3d )


		tooltips = gtk.Tooltips()

		self._view.layout = self._singleLayout
		self._layoutCombo = ComboBox( [ gtk.gdk.Pixbuf ], [ ( self._singleLayout, GSImage.getGSImagePixbuf( 'vp_single.png' ) ),
												( self._quadLayout, GSImage.getGSImagePixbuf( 'vp_quad.png' ) ),
												( self._notebookLayout, GSImage.getGSImagePixbuf( 'vp_notebook.png' ) ) ] )
		self._layoutCombo.choice = self._singleLayout
		self._layoutCombo.choiceSignal.connect( self._p_onLayout )
		self._layoutCombo.getWidget().show()
		tooltips.set_tip( self._layoutCombo.getWidget(), _( 'Viewport layout' ), _( 'Viewport layout' ) )


		self._focusButton = ToggleButton( gsImageFilename='focus_tool.png' )
		self._focusButton.getWidget().show()
		self._focusButton.toggleSignal.connect( self._p_onFocusButton )
		tooltips.set_tip( self._focusButton.getWidget(), _( 'Focus' ), _( 'Focus' ) )


		self._drawingPlaneButton = ToggleButton( gsImageFilename='drawing_plane.png' )
		self._drawingPlaneButton.getWidget().show()
		self._drawingPlaneButton.toggleSignal.connect( self._p_onDrawingPlaneButton )
		tooltips.set_tip( self._drawingPlaneButton.getWidget(), _( 'Drawing plane' ), _( 'Drawing plane' ) )

		self._drawingTemplateButton = ToggleButton( gsImageFilename='drawing_template.png' )
		self._drawingTemplateButton.getWidget().show()
		self._drawingTemplateButton.toggleSignal.connect( self._p_onDrawingTemplateButton )
		tooltips.set_tip( self._drawingTemplateButton.getWidget(), _( 'You can draw onto background models when this is enabled' ),
												_( 'You can draw onto background models when this is enabled' ) )


		self._backgroundImageButton = ToggleButton( gsImageFilename='background_images.png' )
		self._backgroundImageButton.getWidget().show()
		self._backgroundImageButton.toggleSignal.connect( self._p_onBackgroundImageButton )
		tooltips.set_tip( self._backgroundImageButton.getWidget(), _( 'Background images' ), _( 'Background images' ) )

		self._snapSettingsButton = gtk.Button()
		image = GSImage.getGSImageWidget( 'snap_settings.png' )
		self._snapSettingsButton.add( image )
		self._snapSettingsButton.show()
		self._snapSettingsButton.connect( 'clicked', self._p_onSnapSettingsButton )
		tooltips.set_tip( self._snapSettingsButton, _( 'Snap settings' ), _( 'Snap settings' ) )

		self._snapSettingsPopup = SnapSettingsPopup.SnapSettingsPopup()
		self._snapSettingsPopup.attachSnapSettings( self._editorSettings.snapSettings )

		self._layersButton = ToggleButton( gsImageFilename='layers.png' )
		self._layersButton.getWidget().show()
		self._layersButton.toggleSignal.connect( self._p_onBackgroundLayers )
		tooltips.set_tip( self._layersButton.getWidget(), _( 'Layers' ), _( 'Layers' ) )



		self._view.getWidget().set_size_request( 640, 480 )
		self._view.getWidget().show()

		self._resourceGuard = GLResourceGuard( sharingManager )
		self._resourceGuard.getWidget().show()
		self._resourceGuard.setSizeRequest( 18, 18 )

		self._statusBar = gtk.HBox()
		self._statusBar.set_size_request( -1, 20 )
		self._statusBar.pack_end( self._resourceGuard.getWidget(), expand=False, fill=False )
		self._statusBar.show()

		self._viewAndStatusBox = gtk.VBox()
		self._viewAndStatusBox.pack_start( self._view.getWidget(), True, True )
		self._viewAndStatusBox.pack_start( self._statusBar, False, False )
		self._viewAndStatusBox.show()



		self._toolPanel = ToolPanel( self._acceleratorManager )
		self._toolPanel.getWidget().show()


		self._rightToolbar = gtk.VBox()
		self._rightToolbar.set_size_request( 20, -1 )
		self._rightToolbar.show()


		self._workArea = gtk.HBox()
		self._workArea.pack_start( self._viewAndStatusBox, True, True )
		self._workArea.pack_start( self._rightToolbar, False, False )
		self._workArea.show()


		self._panes = gtk.HPaned()
		self._panes.pack1( self._toolPanel.getWidget() )
		self._panes.pack2( self._workArea )
		self._panes.set_position( gSculptUserConfig.userConfig.uiToolPanelWidth )
		self._panes.show()



		self._newNodeMenu = NewNodeMenu()



		def _p_onNew():
			self._newNodeMenu.popup( self._sceneEditor._sceneWindow, self._commandHistory, self._project.scene )


		self._sceneEditor = SceneEditorTools( self._toolPanel, painterFactory, self._view, self._layerPainter, self._editorSettings, self._toolboxTable, self._acceleratorManager, self._window, self._commandHistory )
		self._sceneEditor.newNodeSignal.connect( _p_onNew )


		self._focusTool = FocusTool( self._sceneEditor.nodeEditor, self._view, self._window )
		self._focusTool.doneListener = self._p_onFocusToolDone




		fileMenu = gtk.Menu()
		fileMenuItem = gtk.MenuItem( _( 'File' ) )
		fileMenuItem.set_submenu( fileMenu )

		newItem = gtk.MenuItem( _( 'New' ) )
		newItem.connect( 'activate', self._p_onFileNew )
		fileMenu.append( newItem )

		openItem = gtk.MenuItem( _( 'Open' ) )
		openItem.connect( 'activate', self._p_onFileOpen )
		fileMenu.append( openItem )

		saveItem = gtk.MenuItem( _( 'Save' ) )
		saveItem.connect( 'activate', self._p_onFileSave )
		fileMenu.append( saveItem )

		saveAsItem = gtk.MenuItem( _( 'Save As' ) )
		saveAsItem.connect( 'activate', self._p_onFileSaveAs )
		fileMenu.append( saveAsItem )

		fileMenu.append( gtk.SeparatorMenuItem() )

		importObjItem = gtk.MenuItem( _( 'Import OBJ' ) )
		importObjItem.connect( 'activate', self._p_onImportObj )
		fileMenu.append( importObjItem )

		exportObjItem = gtk.MenuItem( _( 'Export OBJ' ) )
		exportObjItem.connect( 'activate', self._p_onExportObj )
		fileMenu.append( exportObjItem )

		exportVrmlItem = gtk.MenuItem( _( 'Export VRML97' ) )
		exportVrmlItem.connect( 'activate', self._p_onExportVrml )
		fileMenu.append( exportVrmlItem )

		fileMenu.append( gtk.SeparatorMenuItem() )

		quitItem = gtk.MenuItem( _( 'Quit' ) )
		quitItem.connect( 'activate', self._p_onFileQuit )
		fileMenu.append( quitItem )


		editMenu = gtk.Menu()
		editMenuItem = gtk.MenuItem( _( 'Edit' ) )
		editMenuItem.set_submenu( editMenu )

		undoItem = gtk.MenuItem( _( 'Undo' ) )
		undoItem.connect( 'activate', self._p_onEditUndo )
		editMenu.append( undoItem )

		redoItem = gtk.MenuItem( _( 'Redo' ) )
		redoItem.connect( 'activate', self._p_onEditRedo )
		editMenu.append( redoItem )

		editMenu.append( gtk.SeparatorMenuItem() )

		shortcutsItem = gtk.MenuItem( _( 'Shortcuts' ) )
		shortcutsItem.connect( 'activate', self._p_onEditShortcuts )
		editMenu.append( shortcutsItem )

		viewDrawingSettingsItem = gtk.MenuItem( _( 'View drawing settings' ) )
		viewDrawingSettingsItem.connect( 'activate', self._p_onEditViewDrawingSettings )
		editMenu.append( viewDrawingSettingsItem )

		if bEnablePluginManager:
			pluginsItem = gtk.MenuItem( _( 'Manage plugins' ) )
			pluginsItem.connect( 'activate', self._p_onEditPlugins )
			editMenu.append( pluginsItem )


		self._createMenu = CreateMenu( self._sceneEditor, self._toolPanel, self._layerPainter, self._view, self._window, self._commandHistory )
		createMenuItem = gtk.MenuItem( _( 'Create' ) )
		createMenuItem.set_submenu( self._createMenu.menu )



		scriptMenu = gtk.Menu()
		scriptMenuItem = gtk.MenuItem( _( 'Script' ) )
		scriptMenuItem.set_submenu( scriptMenu )

		scriptWindowItem = gtk.MenuItem( _( 'Script window' ) )
		scriptWindowItem.connect( 'activate', self._p_onScriptWindowMenuItem )
		scriptMenu.append( scriptWindowItem )

		runScriptItem = gtk.MenuItem( _( 'Run script' ) )
		runScriptItem.connect( 'activate', self._p_onRunScriptMenuItem )
		scriptMenu.append( runScriptItem )


		self._menubar = gtk.MenuBar()
		self._menubar.append( fileMenuItem )
		self._menubar.append( editMenuItem )
		self._menubar.append( createMenuItem )
		self._menubar.append( scriptMenuItem )
		self._menubar.show_all()


		self._undoAction = GeneralActions.undoAction.instance( UIActionInstanceSignal )
		self._undoAction.actionSignal.connect( self._p_onUndoAction )
		self._undoAction.installAccel( self._acceleratorManager )

		self._redoAction = GeneralActions.redoAction.instance( UIActionInstanceSignal )
		self._redoAction.actionSignal.connect( self._p_onRedoAction )
		self._redoAction.installAccel( self._acceleratorManager )

		self._layoutAction = _layoutAction.instance( UIActionInstanceSignal )
		self._layoutAction.actionSignal.connect( self._p_onLayoutAction )
		self._layoutAction.installAccel( self._acceleratorManager )

		self._focusAction = _focusAction.instance( UIActionInstanceSignal )
		self._focusAction.actionSignal.connect( self._p_onFocusAction )
		self._focusAction.installAccel( self._acceleratorManager )

		self._drawingPlaneAction = _drawingPlaneAction.instance( UIActionInstanceSignal )
		self._drawingPlaneAction.actionSignal.connect( self._p_onDrawingPlaneAction )
		self._drawingPlaneAction.installAccel( self._acceleratorManager )



		self._mainToolbar = gtk.HBox()
		self._mainToolbar.pack_start( self._layoutCombo.getWidget(), False, False )
		self._mainToolbar.pack_start( gtk.VSeparator(), False, False, 5 )
		self._mainToolbar.pack_start( self._focusButton.getWidget(), False, False )
		self._mainToolbar.pack_start( gtk.VSeparator(), False, False, 5 )
		self._mainToolbar.pack_start( self._backgroundImageButton.getWidget(), False, False )
		self._mainToolbar.pack_start( gtk.VSeparator(), False, False, 5 )
		self._mainToolbar.pack_start( self._snapSettingsButton, False, False )
		self._mainToolbar.pack_start( gtk.VSeparator(), False, False, 5 )
		self._mainToolbar.pack_start( self._layersButton.getWidget(), False, False )
		self._mainToolbar.pack_start( gtk.VSeparator(), False, False, 5 )
		self._mainToolbar.pack_start( self._drawingPlaneButton.getWidget(), False, False )
		self._mainToolbar.pack_start( self._drawingTemplateButton.getWidget(), False, False )
		self._mainToolbar.show_all()


		self._windowBox = gtk.VBox()
		self._windowBox.pack_start( self._menubar, False, False )
		self._windowBox.pack_start( self._mainToolbar, False, False )
		self._windowBox.pack_start( self._panes, True, True )
		self._windowBox.show()




		self._window.add( self._windowBox )
		self.setProjectFilename( None );


		self._acceleratorManager.attachToWindow( self._window )




		self._shortcutTable = ShortcutTable()
		self._shortcutEditorWindow = ShortcutTableEditorWindow( self._shortcutTable, self._window )
		readSystemShortcuts( self._shortcutTable )



		self._pluginTable = PluginTable()
		self._pluginManagerWindow = PluginManagerWindow( self._pluginTable, self._window )



		self._viewDrawingSettings = ViewDrawingSettings()
		self._viewDrawingSettingsEditorWindow = ViewDrawingSettingsEditorWindow( self._view, self._window )
		self._viewDrawingSettingsEditorWindow.applySignal.connect( self._p_onViewDrawingSettingsApply )
		readSystemViewDrawingSettings( self._viewDrawingSettings )
		self._viewDrawingSettings.applySettings( self._view )




		scriptBanner = _( "gSculpt %s scripting console (uses pyconsole by Yevgen Muntyan)\nPython %s\nType help(object) for help on an object\nThe gSculpt scripting environment is available via the local variable 'gsc'\n" ) % ( gSculptConfig.gSculptVersionString, sys.version )
		self._scriptEnv = GSculptScriptEnvironment( self._project, self )
		self._scriptConsole = Console( locals = { 'gsc' : self._scriptEnv }, banner=scriptBanner, use_rlcompleter=False )
		self._scriptConsole.connect( 'command', self._p_onScriptPreCommand )
		self._scriptConsole.connect_after( 'command', self._p_onScriptPostCommand )
		self._scriptConsole.show()

		self._scriptScrolledWindow = gtk.ScrolledWindow()
		self._scriptScrolledWindow.set_policy( gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC )
		self._scriptScrolledWindow.add( self._scriptConsole )
		self._scriptScrolledWindow.set_size_request( 640, 480 )
		self._scriptScrolledWindow.show()

		self._scriptWindow = gtk.Window( gtk.WINDOW_TOPLEVEL )
		self._scriptWindow.set_transient_for( self._window )
		self._scriptWindow.add( self._scriptScrolledWindow )
		self._scriptWindow.connect( 'delete-event', self._p_onScriptWindowDelete )
		self._scriptWindow.set_title( _( 'gSculpt Script Window' ) )
		self._bScriptWindowVisible = False



		self._toolPanel.addPage( self._sceneEditor, _( 'Scene' ) )


		self._p_populateToolboxTable( self._toolboxTable )


		self._window.show()



		readSystemPluginTable( self._pluginTable )
		self._pluginTable.loadAll()




	def attachProject(self, project):
		assert self._project is None, 'project already attached'
		self._project = project
		self._scriptEnv._project = project
		self._sceneEditor.attachScene( self._project.scene )
		self._backgroundImagePainter.backgroundImageList = self._project.scene.backgroundImages
		self._backgroundImagesTool.attachImageList( self._project.scene.backgroundImages )
		self._drawingPlaneTool.sheet = self._project.drawingPlane
		self._project.drawingPlane.cells.bEnabled.changedSignal.connect( self._p_onDrawingPlaneEnabledChanged )
		self._view.addPainter( self._project.drawingPlane, Viewport3d )
		self._view.addDrawingTemplate( self._project.drawingPlane, Viewport3d )
		self._layerTableEditor.attachLayerTable( self._project.scene.layerTable )
		self._layerPainter.attachLayerTable( self._project.scene.layerTable )
		self._commandHistory.track( self._project )
		self._createMenu.attachScene( self._project.scene )


	def detachProject(self):
		assert self._project is not None, 'no project attached'
		self._view.removePainter( self._project.drawingPlane, Viewport3d )
		self._view.removeDrawingTemplate( self._project.drawingPlane, Viewport3d )
		if self._backgroundImagesTool.bInstalled:
			self._backgroundImageButton.state = False
			self._backgroundImagesTool.uninstall()
		self._createMenu.detachScene()
		self._commandHistory.stopTracking( self._project )
		self._project.drawingPlane.cells.bEnabled.changedSignal.disconnect( self._p_onDrawingPlaneEnabledChanged )
		self._drawingPlaneTool.sheet = None
		self._backgroundImagesTool.detachImageList()
		self._backgroundImagePainter.backgroundImageList = None
		self._layerPainter.detachLayerTable()
		self._layerTableEditor.detachLayerTable()
		self._sceneEditor.detachScene()
		self._project = None
		self._scriptEnv._project = None


	def _p_onInitialiseGLResouces(self, sharingManager):
		TextureTable.globalTextureTable.load()

	def _p_onShutdownGLResouces(self, sharingManager):
		TextureTable.globalTextureTable.unload()




	def _p_onLayout(self, event, layout):
		self._view.layout = layout


	def setProjectFilename(self, filename):
		self._projectFilename = filename
		if self._projectFilename is None:
			self._window.set_title( _( 'gSculpt %s [Untitled]  (C) copyright Geoffrey French 1999-2007' ) % ( gSculptConfig.gSculptVersionString, ) )
		else:
			self._window.set_title( _( 'gSculpt %s [%s]  (C) copyright Geoffrey French 1999-2007' ) % ( gSculptConfig.gSculptVersionString, os.path.basename( self._projectFilename ) ) )



	def _p_clearCommandHistory(self):
		self._commandHistory.clear()



	def _p_onFileNew(self, widget):
		if self._p_saveUnsavedData( gtk.STOCK_NEW ):
			self._p_clearCommandHistory()
			self.detachProject()
			project = GSProject()
			self.attachProject( project )
			self.setProjectFilename( None )
			self._bUnsavedData = False


	def _p_onFileOpen(self, widget):
		if self._p_saveUnsavedData( gtk.STOCK_OPEN ):
			gpxFilter = gtk.FileFilter()
			gpxFilter.set_name( _( 'gSculpt Procedural XML (*.gpx)' ) )
			gpxFilter.add_pattern( '*.gpx' )

			openDialog = gtk.FileChooserDialog( _( 'Open' ), self._window, gtk.FILE_CHOOSER_ACTION_OPEN,
											( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
			openDialog.set_current_folder( gSculptUserConfig.userConfig.projectPath )
			openDialog.add_filter( gpxFilter )
			openDialog.show()
			response = openDialog.run()
			filename = openDialog.get_filename()
			openDialog.destroy()
			if response == gtk.RESPONSE_OK:
				if filename is not None:
					gSculptUserConfig.userConfig.projectPath = os.path.dirname( filename )
					f = open( filename, 'r' )
					if f is not None:
						doc = InputXmlDocument()
						doc.parseFile( f )
						contentNode = doc.getContentNode()
						if contentNode.isValid():
							self._p_clearCommandHistory()
							self.detachProject()
							project = GSProject()
							contentNode  >>  project
							self.attachProject( project )
							self.setProjectFilename( filename )
							self._bUnsavedData = False


	def _p_onFileSave(self, widget):
		self._p_save()


	def _p_onFileSaveAs(self, widget):
		self._p_saveAs()


	def _p_onImportObj(self, widget):
		dialog = gtk.Dialog( title=_( 'Import OBJ' ), parent=self._window, flags=gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,
						buttons=( gtk.STOCK_OPEN, gtk.RESPONSE_ACCEPT, gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL ) )

		objFilter = gtk.FileFilter()
		objFilter.set_name( _( 'Wavefront object (*.obj)' ) )
		objFilter.add_pattern( '*.obj' )

		fileChooser = gtk.FileChooserWidget( action=gtk.FILE_CHOOSER_ACTION_OPEN )
		fileChooser.set_current_folder( gSculptUserConfig.userConfig.importPath )
		fileChooser.add_filter( objFilter )

		separator = gtk.HSeparator()

		mergeCheck = gtk.CheckButton( _( 'Merge models (import as one scene node)' ) )

		dialogVBox = gtk.VBox( spacing=10 )
		dialogVBox.pack_start( fileChooser, expand=True, fill=True )
		dialogVBox.pack_start( separator, expand=False, fill=False )
		dialogVBox.pack_start( mergeCheck, expand=False, fill=False )
		dialogVBox.show_all()

		dialogBox = gtk.HBox()
		dialogBox.pack_start( dialogVBox, expand=True, fill=True, padding=20 )
		dialogBox.show()

		dialog.vbox.pack_start( dialogBox, expand=True, fill=True, padding=20 )

		dialog.set_size_request( 600, 500 )

		dialog.show()
		response = dialog.run()
		filename = fileChooser.get_filename()
		bMerge = mergeCheck.get_active()
		dialog.destroy()
		if response == gtk.RESPONSE_ACCEPT:
			if filename is not None:
				gSculptUserConfig.userConfig.importPath = os.path.dirname( filename )
				f = open( filename, 'r' )
				if f is not None:
					nodes = ObjImport.importNodes( f, bMerge )
					self._commandHistory.freeze()
					for node in nodes:
						self._project.scene.nodes.append( node )
					self._commandHistory.thaw()



	def _p_export(self, exportDialogTitle, filenameExtension, filterDescription, packExportOptionsFunction, getOptionsFunction, exportNodeFunction, exportNodesFunction):
		exportDialog = gtk.Dialog( title=exportDialogTitle, parent=self._window, flags=gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,
							buttons=( gtk.STOCK_SAVE, gtk.RESPONSE_ACCEPT, gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL ) )

		EXPORT_CURRENT = 0
		EXPORT_VISIBLE = 1
		EXPORT_ALL = 2

		initialDir = gSculptUserConfig.userConfig.exportPath
		initialFilename = 'export' + filenameExtension
		if self._projectFilename is not None:
			initialPath = os.path.splitext( self._projectFilename )[0] + filenameExtension
			if os.path.isabs( initialPath ):
				initialDir, initialFilename = os.path.split( initialPath )
			else:
				initialFilename = os.path.split( initialPath )[1]

		objFilter = gtk.FileFilter()
		objFilter.set_name( filterDescription )
		objFilter.add_pattern( '*' + filenameExtension )

		fileChooser = gtk.FileChooserWidget( action=gtk.FILE_CHOOSER_ACTION_SAVE )
		fileChooser.set_current_name( initialFilename )
		fileChooser.set_current_folder( initialDir )
		fileChooser.add_filter( objFilter )

		separator = gtk.HSeparator()

		exportLabel = gtk.Label( _( 'Export:' ) )
		exportGroup = RadioGroup()
		exportCurrentRadio = exportGroup.addRadioButton( EXPORT_CURRENT, _( 'Current node' ) )
		exportVisibleRadio = exportGroup.addRadioButton( EXPORT_VISIBLE, _( 'Visible nodes' ) )
		exportAllRadio = exportGroup.addRadioButton( EXPORT_ALL, _( 'All nodes' ) )
		exportGroup.currentChoice = EXPORT_VISIBLE
		exportBox = gtk.VBox( spacing=2 )
		exportBox.pack_start( exportLabel, expand=False, fill=False )
		exportBox.pack_start( exportCurrentRadio.getWidget(), expand=False, fill=False )
		exportBox.pack_start( exportVisibleRadio.getWidget(), expand=False, fill=False )
		exportBox.pack_start( exportAllRadio.getWidget(), expand=False, fill=False )
		exportBox.show_all()

		dialogVBox = gtk.VBox( spacing=10 )
		dialogVBox.pack_start( fileChooser, expand=True, fill=True )
		dialogVBox.pack_start( separator, expand=False, fill=False )
		dialogVBox.pack_start( exportBox, expand=False, fill=False )
		packExportOptionsFunction( dialogVBox )
		dialogVBox.show_all()

		dialogBox = gtk.HBox()
		dialogBox.pack_start( dialogVBox, expand=True, fill=True, padding=20 )
		dialogBox.show()

		exportDialog.vbox.pack_start( dialogBox, expand=True, fill=True, padding=20 )

		exportDialog.set_size_request( -1, 600 )

		exportDialog.show()
		response = exportDialog.run()
		if response == gtk.RESPONSE_ACCEPT:
			filename = fileChooser.get_filename()
			exportTargets = exportGroup.currentChoice
			getOptionsFunction()
			exportDialog.destroy()

			bWrite = True

			if os.path.exists( filename ):
				bWrite = confirmOverwriteFileDialog( filename, self._window )


			if bWrite:
				gSculptUserConfig.userConfig.exportPath = os.path.dirname( filename )
				try:
					f = open( filename, 'w' )
				except IOError:
					messageDialog( _( 'Could not open file' ), self._window, _( 'Could not open %s for writing' )  %  ( filename, ) )
				else:
					if exportTargets == EXPORT_CURRENT:
						if self._sceneEditorTools.currentNode is not None:
							exportNodeFunction( f, self._sceneEditorTools.currentNode )
					elif exportTargets == EXPORT_VISIBLE:
						nodes = []
						for layer in self._project.scene.layerTable:
							if layer.bVisible:
								nodes += [ node   for node in layer   if node.bVisible ]
						exportNodesFunction( f, nodes )
					elif exportTargets == EXPORT_ALL:
						exportNodesFunction( f, self._project.scene.nodes )
		else:
			exportDialog.destroy()



	def _p_onExportObj(self, widget):
		triangulateCheck = gtk.CheckButton( _( 'Triangulate faces' ) )
		negativeIndicesCheck = gtk.CheckButton( _( 'Use negative indices' ) )

		options = [ False, False ]

		def packExportOptions(dialogVBox):
			dialogVBox.pack_start( triangulateCheck, expand=False, fill=False )
			dialogVBox.pack_start( negativeIndicesCheck, expand=False, fill=False )

		def getOptions():
			options[0] = triangulateCheck.get_active()
			options[1] = negativeIndicesCheck.get_active()

		def exportNode(f, node):
			ObjExport.exportNode( f, node, options[0], options[1] )

		def exportNodes(f, nodes):
			ObjExport.exportNodes( f, nodes, options[0], options[1] )

		self._p_export( _( 'Export OBJ' ), '.obj', _( 'Wavefront object (*.obj)' ), packExportOptions, getOptions, exportNode, exportNodes )



	def _p_onExportVrml(self, widget):
		triangulateCheck = gtk.CheckButton( _( 'Triangulate faces' ) )

		options = [ False ]

		def packExportOptions(dialogVBox):
			dialogVBox.pack_start( triangulateCheck, expand=False, fill=False )

		def getOptions():
			options[0] = triangulateCheck.get_active()

		def exportNode(f, node):
			VrmlExport.exportNode( f, node, options[0] )

		def exportNodes(f, nodes):
			VrmlExport.exportNodes( f, nodes, options[0] )

		self._p_export( _( 'Export Vrml97' ), '.wrl', _( 'Vrml97 (*.wrl)' ), packExportOptions, getOptions, exportNode, exportNodes )




	def _p_save(self):
		if self._projectFilename is None:
			return self._p_saveAs()
		else:
			self._p_writeFile( self._projectFilename )


	def _p_saveAs(self):
		filename = None
		bFinished = False
		while not bFinished:
			gpxFilter = gtk.FileFilter()
			gpxFilter.set_name( _( 'gSculpt Procedural XML (*.gpx)' ) )
			gpxFilter.add_pattern( '*.gpx' )

			saveAsDialog = gtk.FileChooserDialog( _( 'Save As' ), self._window, gtk.FILE_CHOOSER_ACTION_SAVE,
										( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
			saveAsDialog.add_filter( gpxFilter )
			if self._projectFilename is None:
				saveAsDialog.set_current_folder( gSculptUserConfig.userConfig.projectPath )
			else:
				if os.path.isabs( self._projectFilename ):
					saveAsDialog.set_filename( self._projectFilename )
				else:
					saveAsDialog.set_filename( os.path.join( os.getcwd(), self._projectFilename ) )
			saveAsDialog.show()
			response = saveAsDialog.run()
			filenameFromDialog = saveAsDialog.get_filename()
			saveAsDialog.destroy()
			if response == gtk.RESPONSE_OK:
				if filenameFromDialog is not None:
					if os.path.exists( filenameFromDialog ):
						if confirmOverwriteFileDialog( filenameFromDialog, self._window ):
							filename = filenameFromDialog
							gSculptUserConfig.userConfig.projectPath = os.path.dirname( filename )
							bFinished = True
						else:
							bFinished = False
					else:
						filename = filenameFromDialog
						bFinished = True
				else:
					bFinished = True
			else:
				bFinished = True

		if filename is not None:
			self._p_writeFile( filename )
			return True
		else:
			return False


	def _p_writeFile(self, filename):
		doc = OutputXmlDocument()
		doc.getContentNode()  <<  self._project
		f = open( filename, 'w' )
		if f is not None:
			doc.writeFile( f )
			f.close()
			self.setProjectFilename( filename )
			self._bUnsavedData = False



	def _p_onFileQuit(self, widget):
		if self._p_saveUnsavedData( gtk.STOCK_QUIT ):
			self._window.destroy()



	def _p_onEditUndo(self, widget):
		self._p_undo()

	def _p_onEditRedo(self, widget):
		self._p_redo()

	def _p_onEditShortcuts(self, widget):
		self._shortcutEditorWindow.show()

	def _p_onEditViewDrawingSettings(self, widget):
		self._viewDrawingSettingsEditorWindow.start( self._viewDrawingSettings )

	def _p_onEditPlugins(self, widget):
		self._pluginManagerWindow.show()

	def _p_onViewDrawingSettingsApply(self, viewDrawingSettings):
		self._viewDrawingSettings = copy( viewDrawingSettings )
		self._viewDrawingSettings.applySettings( self._view )
		self._view.postRedraw()

	def _p_onUndoAction(self, action):
		self._p_undo()

	def _p_onRedoAction(self, action):
		self._p_redo()

	def _p_onLayoutAction(self, action):
		if self._layoutCombo.choice is self._singleLayout:
			self._layoutCombo.choice = self._quadLayout
		else:
			self._layoutCombo.choice = self._singleLayout

	def _p_onFocusAction(self, action):
		self._focusButton.state = not self._focusButton.state

	def _p_onDrawingPlaneAction(self, action):
		self._drawingPlaneButton.state = not self._drawingPlaneButton.state

	def _p_undo(self):
		if self._commandHistory.canUndo():
			self._commandHistory.undo()

	def _p_redo(self):
		if self._commandHistory.canRedo():
			self._commandHistory.redo()


	def _p_onFocusButton(self, event, bState):
		if bState:
			self._focusTool.install( self._toolPanel, self._window )
		else:
			self._focusTool.uninstall()

	def _p_onBackgroundImageButton(self, event, bState):
		if event.bUserEvent:
			if bState:
				self._backgroundImagesTool.install( self._toolPanel, self._window )
			else:
				self._backgroundImagesTool.uninstall()

	def _p_onDrawingPlaneButton(self, event, bState):
		if self._project is not None:
			if bState != self._project.drawingPlane.bEnabled:
				self._project.drawingPlane.bEnabled = bState
				self._commandHistory.finishCommand()

		if bState:
			self._drawingPlaneTool.install( self._toolPanel, self._window )
		else:
			self._drawingPlaneTool.uninstall()

	def _p_onDrawingTemplateButton(self, event, bState):
		if event.bUserEvent:
			if bState:
				self._view.addDrawingTemplate( self._layerPainter, Viewport3d )
			else:
				self._view.removeDrawingTemplate( self._layerPainter, Viewport3d )


	def _p_onFocusToolDone(self, tool):
		tool.uninstall()
		self._focusButton.state = False


	def _p_onBackgroundImagesToolDone(self, tool):
		tool.uninstall()
		self._backgroundImageButton.state = False


	def _p_onDrawingPlaneToolDone(self, tool):
		tool.uninstall()


	def _p_onDrawingPlaneEnabledChanged(self):
		queueEvent( self._p_onDrawingPlaneEnabled )


	def _p_onDrawingPlaneEnabled(self):
		if not self._project.drawingPlane.bEnabled:
			self._drawingPlaneTool.uninstall()


	def _p_onSnapSettingsButton(self, widget):
		self._snapSettingsPopup.popup( widget )


	def _p_onBackgroundLayers(self, event, bState):
		if event.bUserEvent:
			if bState:
				x, y, modifiers = gtk.gdk.get_default_root_window().get_pointer()

				self._layerTableEditor.show()
				self._layerTableEditor.getWindow().move( x, y )
			else:
				self._layerTableEditor.hide()


	def _p_onLayerTableEditorClose(self):
		self._layersButton.state = False



	def _p_onDeleteEvent(self, widget, event, data=None):
		return not self._p_saveUnsavedData( gtk.STOCK_QUIT )

	def _p_onDestroy(self, widget, data=None):
		gSculptUserConfig.userConfig.uiToolPanelWidth = self._panes.get_position()
		writeSystemShortcuts( self._shortcutTable )
		writeSystemPluginTable( self._pluginTable )
		writeSystemViewDrawingSettings( self._viewDrawingSettings )
		self.destroySignal.emit()



	def _p_onCommandHistoryChanged(self, commandHistory):
		self._bUnsavedData = True



	def _p_saveUnsavedData(self, confirmStock):
		if self._bUnsavedData:
			dialog = gtk.Dialog( _( 'Confirm' ), self._window, gtk.DIALOG_MODAL, ( confirmStock, 0, gtk.STOCK_SAVE, 1, gtk.STOCK_CANCEL, 2 ) )
			dialog.vbox.pack_start( gtk.Label( _( 'You have not saved your work' ) ) )
			dialog.vbox.pack_start( gtk.Label( _( 'Are you sure?' ) ) )
			dialog.show_all()
			response = dialog.run()
			dialog.destroy()

			if response == 0:
				return True
			elif response == 1:
				return self._p_save()
			elif response == 2:
				return False
			else:
				return False
		else:
			return True




	def _p_populateToolboxTable(self, toolboxTable):
		nodeEditor = self._sceneEditor.nodeEditor

		primToolbox = PrimitiveToolbox( self._acceleratorManager, self._view, nodeEditor, self._editorSettings, self._window, self._commandHistory )
		meshToolbox = MeshToolbox( self._acceleratorManager, self._view, nodeEditor, self._sceneEditor, self._layerPainter, self._toolPanel, self._editorSettings, self._window, self._commandHistory )
		groupToolbox = GroupToolbox( self._acceleratorManager, self._view, nodeEditor, self._editorSettings, self._window, self._commandHistory )
		toolboxTable.registerToolbox( GSProductModelBlank, primToolbox )
		toolboxTable.registerToolbox( GSProductMesh, meshToolbox )
		toolboxTable.registerToolbox( GSProductGroup, groupToolbox )


	def _p_onScriptWindowDelete(self, window, event):
		window.hide()
		self._bScriptWindowVisible = False
		return True

	def _p_onScriptWindowMenuItem(self, widget):
		self._bScriptWindowVisible = not self._bScriptWindowVisible
		if self._bScriptWindowVisible:
			self._scriptWindow.show()
		else:
			self._scriptWindow.hide()


	def _p_onScriptPreCommand(self, console, code):
		self._commandHistory.freeze()

	def _p_onScriptPostCommand(self, console, code):
		self._commandHistory.thaw()



	def _p_onRunScriptMenuItem(self, widget):
		pyFilter = gtk.FileFilter()
		pyFilter.set_name( _( 'Python script (*.py)' ) )
		pyFilter.add_pattern( '*.py' )

		runScriptDialog = gtk.FileChooserDialog( _( 'Run script' ), self._window, gtk.FILE_CHOOSER_ACTION_OPEN,
										( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
		runScriptDialog.set_current_folder( gSculptUserConfig.userConfig.scriptPath )
		runScriptDialog.add_filter( pyFilter )
		runScriptDialog.show()
		response = runScriptDialog.run()
		filename = runScriptDialog.get_filename()
		runScriptDialog.destroy()
		if response == gtk.RESPONSE_OK:
			if filename is not None:
				if os.path.isfile( filename ):
					gSculptUserConfig.userConfig.scriptPath = os.path.dirname( filename )
					securityDialog = gtk.Dialog( _( 'Security warning' ), self._window, gtk.DIALOG_MODAL, ( gtk.STOCK_YES, gtk.RESPONSE_OK, gtk.STOCK_NO, gtk.RESPONSE_CANCEL ) )
					def markupLabel(text):
						label = gtk.Label()
						label.set_markup( text )
						return label
					securityDialog.vbox.pack_start( markupLabel( _( '<b><span foreground="#b00000">Run Script: Security Warning!</span></b>' ) ), padding=10 )
					securityDialog.vbox.pack_start( markupLabel( _( '<span foreground="#b00000">Scripts are not checked for safety!</span>' ) ) )
					securityDialog.vbox.pack_start( markupLabel( _( '<span foreground="#b00000">Scripts containing malicious code may destroy data</span>' ) ) )
					securityDialog.vbox.pack_start( markupLabel( _( '<span foreground="#b00000">and damage your system!</span>' ) ) )
					securityDialog.vbox.pack_start( markupLabel( _( '<span foreground="#b00000">The author(s) of gSculpt CANNOT be held liable for any damage that occurs.</span>' ) ) )
					securityDialog.vbox.pack_start( markupLabel( _( '<span foreground="#b00000">Are you sure you wish to proceed?</span>' ) ), padding=10 )
					securityDialog.show_all()
					response = securityDialog.run()
					securityDialog.destroy()
					if response == gtk.RESPONSE_OK:
						self._commandHistory.freeze()

						scriptBanner = _( "gSculpt %s scripting console (based on PyConsole by Yevgen Muntyan) for script %s\nPython %s\nType help(object) for help on an object\nThe gSculpt scripting environment is available via the local variable 'gsc'\n" ) % ( gSculptConfig.gSculptVersionString, os.path.basename( filename ), sys.version )
						scriptConsole = Console( locals = { 'gsc' : self._scriptEnv }, banner=scriptBanner, use_rlcompleter=False )
						scriptConsole.show()

						scriptScrolledWindow = gtk.ScrolledWindow()
						scriptScrolledWindow.set_policy( gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC )
						scriptScrolledWindow.add( scriptConsole )
						scriptScrolledWindow.set_size_request( 640, 480 )
						scriptScrolledWindow.show()

						def onDelete(window, event):
							scriptWindow.destroy()

						def onOk(button):
							scriptWindow.destroy()

						okButton = gtk.Button( stock=gtk.STOCK_OK )
						okButton.connect( 'clicked', onOk )
						okButton.show()

						buttonBox = gtk.HBox()
						buttonBox.pack_end( okButton, expand=False, fill=True, padding=10 )
						buttonBox.show()

						windowBox = gtk.VBox()
						windowBox.pack_start( scriptScrolledWindow, expand=True, fill=True )
						windowBox.pack_start( buttonBox, expand=False, fill=False )
						windowBox.show()

						scriptWindow = gtk.Window( gtk.WINDOW_TOPLEVEL )
						scriptWindow.set_transient_for( self._window )
						scriptWindow.add( windowBox )
						scriptWindow.connect( 'delete-event', onDelete )
						scriptWindow.set_title( _( 'Running script %s' ) % ( os.path.basename( filename ) ) )
						scriptWindow.show()

						scriptConsole.exec_command( 'execfile( \'%s\', locals() )\n' % ( filename.replace( '\'', '\\\'' ), ) )

						self._commandHistory.thaw()

						# Need to be notified about any commands
						scriptConsole.connect( 'command', self._p_onScriptPreCommand )
						scriptConsole.connect_after( 'command', self._p_onScriptPostCommand )


				else:
					notFileDialog = gtk.Dialog( _( 'Not a file' ), self._window, gtk.DIALOG_MODAL, ( gtk.STOCK_OK, gtk.RESPONSE_OK ) )
					notFileDialog.vbox.pack_start( gtk.Label( _( '%s is not a file.' ) % ( os.path.basename( filename ), ) ), padding=10 )
					notFileDialog.show_all()
					notFileDialog.run()






if __name__ == '__main__':
	from Britefury.SceneEdit import NodePlanAdapter, NodePlanViewList
	from Britefury.SceneEdit.NodePlanAdapter import NodePlanAdapter
	from Britefury.SceneEdit.NodePlanViewList import NodePlanViewList

	from Britefury.ProceduralCore import GSPlan, GSProcedure
	from Britefury.ProceduralCore.GSProcedure import *

	from Britefury.Graphics import Graphics
	from Britefury.Graphics.Graphics import glDrawString3

	from Britefury.Math.Math import Point3, Matrix4

	from Britefury.GL.GL import *

	from Britefury.UI.FormLayout import FormLayoutFactory

	from Britefury.ProceduralTool import GSFunctionTool, GSProcedureTool
	from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool


	from Britefury.Tool.Toolbox import Toolbox
	from Britefury.Tool.Toolkit import Toolkit
	from Britefury.Tool.Toolset import Toolset
	from Britefury.Tool.ToolGroup import ToolGroup

	from Britefury.Painter import Painter
	from Britefury.Painter.Painter import ObjectPainterFactory




	gSculptUserConfig.userConfig.load()


	painterFactory = ObjectPainterFactory.getPainterFactory()

	app = MainApp( painterFactory )




	project = GSProject()
	app.attachProject( project )




	gtk.main()

	gSculptUserConfig.userConfig.save()


