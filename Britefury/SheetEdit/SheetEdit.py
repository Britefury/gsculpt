##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.Cell.Cell import *
from Britefury.Kernel.KMeta import KMetaMember, KClass, KObject
from Britefury.Sheet.Sheet import *
from Britefury.UI.FormLayout import FormLayoutFactory



class SheetEditContext (object):
	def __init__(self, sheetEditor, view, editorSettings, parentWindow, commandHistory):
		super( SheetEditContext, self ).__init__()
		self.sheetEditor = sheetEditor
		self.view = view
		self.editorSettings = editorSettings
		self.parentWindow = parentWindow
		self.commandHistory = commandHistory





class FieldEditor (KMetaMember):
	"Field editor interface"
	def __init__(self, doc=''):
		super( FieldEditor, self ).__init__( doc )
		self._elementAttrName = None



	def _f_metaMember_initMetaMember(self, cls, name):
		"""Initialise a meta-member; called by the metaclass constructor
		@cls - the class that this meta-member is part of
		@name - the attribute name of this meta-member"""
		super( FieldEditor, self )._f_metaMember_initMetaMember( cls, name )
		self._elementAttrName = intern( self._p_computeElementAttrName( name ) )



	def _f_metaMember_initInstance(self, instance, srcInstance=None):
		"""Initialise instance
		@instance - instance to initialise
		@srcInstance - source instance to copy values from"""
		super( FieldEditor, self )._f_metaMember_initInstance( instance, srcInstance )
		element = self.createElement( instance._sheetEditContext )
		setattr( instance, self._elementAttrName, element )



	def __get__(self, obj, objtype = None):
		"""Descriptor - get"""
		if obj is not None:
			# Return the element
			return self._f_getElementFromInstance( obj )
		else:
			# Return the meta-member
			return self

	def __set__(self, obj, value):
		"""Descriptor - set"""
		raise TypeError, 'cannot set sheet field editor element'

	def __delete__(self, obj):
		"""Descriptor - delete"""
		raise TypeError, 'cannot delete sheet field editor element'



	def _f_getElementFromInstance(self, instance):
		"""Get the element corresponding to @self from @instance
		@instance - an instance of @Sheet"""
		return getattr( instance, self._elementAttrName )



	@staticmethod
	def _p_computeElementAttrName(name):
		"""Private - compute the full element attribute name"""
		return '_sheetEditorElement_' + name



	def createElement(self, sheetEditContext):
		"""Create an element
		@sheetEditContext - the context in which this element is being used"""
		assert False, 'abstract'


	def initialiseElement(self, element, sheetEditContext):
		"""Initialise the elemtn
		@element; the element in question
		@sheetEditContext - the context in which this element is being used"""
		pass

	def shutdownElement(self, element, sheetEditContext):
		"""Shutdown the element
		@element; the element in question
		@sheetEditContext - the context in which this element is being used"""
		pass


	def installElementEventHandlers(self, element, sheetEditContext):
		"""Install the event handlers of a previously created element
		@element; the element in question
		@sheetEditContext - the context in which this element is being used"""
		pass

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		"""Uninstall the event handlers of a previously created element
		@element; the element in question
		@sheetEditContext - the context in which this element is being used"""
		pass


	def installElementAccelerators(self, element, accels, sheetEditContext):
		"""Install the accelerators of a previously created element
		@element; the element in question
		@accels - accelerator list
		@sheetEditContext - the context in which this element is being used"""
		pass

	def uninstallElementAccelerators(self, element, accels, sheetEditContext):
		"""Uninstall the accelerators of a previously created element
		@element; the element in question
		@accels - accelerator list
		@sheetEditContext - the context in which this element is being used"""
		pass





	@staticmethod
	def _p_getCell(field, sheetEditContext):
		"""Private - get a variable from a function context given the field
		@field - the field
		@sheetEditContext - the context in which this element is being used"""
		return field._f_getCellFromInstance( sheetEditContext.sheetEditor )







class FieldWidgetEditor (FieldEditor):
	"Widget based field editor interface"
	def __init__(self):
		super( FieldWidgetEditor, self ).__init__()
		self._sensitivityField = None


	def initialiseElement(self, element, sheetEditContext):
		super( FieldWidgetEditor, self ).initialiseElement( element, sheetEditContext )
		# Handle sensitivity
		if self._sensitivityField is not None:
			# Get the cell
			sensitivityCell = self._p_getCell( self._sensitivityField, sheetEditContext )
			# Attach as sensitivity source
			element.setSensitivity( sensitivityCell )

	def shutdownElement(self, element, sheetEditContext):
		super( FieldWidgetEditor, self ).shutdownElement( element, sheetEditContext )
		# Handle sensitivity
		if self._sensitivityField is not None:
			# Detach as sensitivity source
			element.setSensitivity( True )


	def showElement(self, element):
		"""Show a previously created element
		@element; the element to destroy"""
		element.show()

	def hideElement(self, element):
		"""Hide a previously created element
		@element; the element to destroy"""
		element.hide()



	def _p_setSensitivityField(self, field):
		"""Set a field as a source of a variable that determines widget sensitivity"""
		self._sensitivityField = field


	sensitivityField = property( None, _p_setSensitivityField )






class FieldEditorComponent (KMetaMember):
	"""Field editor component
	A UI element can be composed of multiple components. A 3D vector editing component would be composed of components to edit the X, Y, and Z axes.
	A factory component is used to retrieve the component of the UI element created by an element factory."""
	def __init__(self, doc=''):
		super( FieldEditorComponent, self ).__init__( doc )
		self._componentAttrName = None



	def _f_metaMember_initMetaMember(self, cls, name):
		"""Initialise a meta-member; called by the metaclass constructor
		@cls - the class that this meta-member is part of
		@name - the attribute name of this meta-member"""
		super( FieldEditorComponent, self )._f_metaMember_initMetaMember( cls, name )
		self._componentAttrName = intern( self._p_computeComponentAttrName( name ) )



	def _f_metaMember_initInstance(self, instance, srcInstance=None):
		"""Initialise instance
		@instance - instance to initialise
		@srcInstance - source instance to copy values from"""
		super( FieldEditorComponent, self )._f_metaMember_initInstance( instance, srcInstance )
		fieldEditor = self.getFieldEditor()
		element = fieldEditor._f_getElementFromInstance( instance )
		elementWidget = self.getElementWidget( element )
		setattr( instance, self._componentAttrName, elementWidget )



	def __get__(self, obj, objtype = None):
		"""Descriptor - get"""
		if obj is not None:
			# Return the component
			return self._f_getComponentWidgetFromInstance( obj )
		else:
			# Return the meta-member
			return self

	def __set__(self, obj, value):
		"""Descriptor - set"""
		raise TypeError, 'cannot set sheet field editor element component'

	def __delete__(self, obj):
		"""Descriptor - delete"""
		raise TypeError, 'cannot delete sheet field editor element component'



	def _f_getComponentWidgetFromInstance(self, instance):
		"""Get the element corresponding to @self from @instance
		@instance - an instance of @Sheet"""
		return getattr( instance, self._componentAttrName )



	@staticmethod
	def _p_computeComponentAttrName(name):
		"""Private - compute the full element attribute name"""
		return '_sheetEditorElementComponent_' + name



	def getFieldEditor(self):
		"Get the field editor that owns this field editor component"
		assert False, 'abstract'

	def getElementWidget(self, element):
		"""Get the widget of @element that is represented by this factory component
		@element - the UI element"""
		assert False, 'abstract'




class FieldEditorAttributeComponent (FieldEditorComponent):
	"""Field editor attribute component
	Keeps a reference to the field editor, for @getFieldEditor() to return, and a widget attribute name; the name of the attribute in the cell editor that stores the widget represented by
	this component, when given a UI element created by the field editor"""
	def __init__(self, fieldEditor, attributeName, doc=''):
		super( FieldEditorAttributeComponent, self ).__init__( doc )
		"""Constructor
		@fieldEditor - the field editor
		@attributeName - the name of the attribute in the cell editor that stores the widget represented by this component"""
		self._fieldEditor = fieldEditor
		self._attributeName = attributeName
		self._o_addDependency( self._fieldEditor )


	def getFieldEditor(self):
		"Get the field editor that owns this field editor component"
		return self._fieldEditor

	def getElementWidget(self, element):
		"""Get the widget of @element that is represented by this factory component
		@element - the UI element"""
		return getattr( element, self._attributeName )




class BasicFieldWidgetEditor (FieldWidgetEditor):
	__componentnames__ = []

	def __init__(self):
		super( BasicFieldWidgetEditor, self ).__init__()

		for attrName in self.__componentnames__:
			setattr( self, attrName, FieldEditorAttributeComponent( self, attrName ) )





class BasicSingleFieldWidgetEditor (BasicFieldWidgetEditor):
	def __init__(self, field):
		super( BasicSingleFieldWidgetEditor, self ).__init__()
		self._field = field
		self._o_addDependency( field )

	def initialiseElement(self, element, sheetEditContext):
		super( BasicSingleFieldWidgetEditor, self ).initialiseElement( element, sheetEditContext )
		element.attachCell( self._p_getCell( self._field, sheetEditContext ) )

	def shutdownElement(self, element, sheetEditContext):
		super( BasicSingleFieldWidgetEditor, self ).shutdownElement( element, sheetEditContext )
		element.detachCell()






class CompositeFieldEditorClass (type):
	def __init__(cls, clsName, bases, clsDict):
		super( CompositeFieldEditorClass, cls ).__init__( clsName, bases, clsDict )
		if not clsDict.has_key( 'compositeSheetEditorClass' ):
			raise AttributeError, '\'compositeSheetEditorClass\' not specified'



class CompositeFieldEditor (FieldEditor):
	__metaclass__ = CompositeFieldEditorClass

	compositeSheetEditorClass = None

	class CompositeComponent (FieldEditorComponent):
		def __init__(self, fieldEditor):
			"""Constructor
			@fieldEditor - the field editor"""
			self._fieldEditor = fieldEditor


		def getFieldEditor(self):
			"Get the field editor that owns this field editor component"
			return self._fieldEditor

		def getElementWidget(self, element):
			"""Get the widget of @element that is represented by this factory component
			@element - the UI element"""
			return element.getSheetEditorWidget()



	def __init__(self, field):
		super( CompositeFieldEditor, self ).__init__()
		self._field = field
		self.sheetEditor = CompositeFieldEditor.CompositeComponent( self )


	def createElement(self, sheetEditContext):
		return self.compositeSheetEditorClass( sheetEditContext.view, sheetEditContext.editorSettings, sheetEditContext.parentWindow, sheetEditContext.commandHistory )


	def initialiseElement(self, element, sheetEditContext):
		super( CompositeFieldEditor, self ).initialiseElement( element, sheetEditContext )
		cell = self._p_getCell( self._field, sheetEditContext )
		def _getSheet():
			return cell.getImmutableValue()
		#element.sheet = cell.getEvaluator()
		element.cells.sheet.function = _getSheet

	def shutdownElement(self, element, sheetEditContext):
		super( CompositeFieldEditor, self ).shutdownElement( element, sheetEditContext )
		element.sheet = None



	def installElementEventHandlers(self, element, sheetEditContext):
		element.sheetEditorInstallEventHandlers()

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		element.sheetEditorUninstallEventHandlers()


	def installElementAccelerators(self, element, accels, sheetEditContext):
		element.sheetEditorInstallAccelerators( accels )

	def uninstallElementAccelerators(self, element, accels, sheetEditContext):
		element.sheetEditorUninstallAccelerators( accels )








class SheetEditorClass (SheetClass):
	def __init__(cls, clsName, bases, clsDict):
		super( SheetEditorClass, cls ).__init__( clsName, bases, clsDict )

		# Get field editors, layout factory, and data sources from class / class dictionary
		fieldEditors = SheetEditorClass._o_gatherListFromBases( bases, 's_SheetEdit_fieldEditors' )
		fieldEditorComponents = SheetEditorClass._o_gatherListFromBases( bases, 's_SheetEdit_fieldEditorComponents' )
		layoutFactories = SheetEditorClass._o_gatherListFromBases( bases, 's_SheetEdit_layoutFactories' )
		cls.s_SheetEdit_fieldEditors = fieldEditors
		cls.s_SheetEdit_fieldEditorComponents = fieldEditorComponents
		cls.s_SheetEdit_layoutFactories = layoutFactories

		# Handle dictionary contents
		fieldEditors.extend( [ value   for value in clsDict.values()   if isinstance( value, FieldEditor ) ] )
		fieldEditorComponents.extend( [ value   for value in clsDict.values()   if isinstance( value, FieldEditorComponent ) ] )


		fieldEditorsByDep = FieldEditor._f_computeDependencySortedList( fieldEditors )
		cls.s_SheetEdit_fieldEditorsByDep = fieldEditorsByDep


		# Handle layout
		try:
			layout = clsDict['layout']
			if not isinstance( layout, FormLayoutFactory )  and  not callable( layout ):
				raise TypeError, 'layout is not an instance of FormLayoutFactory and is not callable'
			layoutFactories.append( layout )
		except KeyError:
			pass






class SheetEditor (Sheet):
	"Function tool base class"
	__metaclass__ = SheetEditorClass


	sheet = SheetRefField( Sheet )



	def __init__(self, view, editorSettings, parentWindow, commandHistory):
		"Constructor"
		self._view = view
		self._editorSettings = editorSettings
		self._parentWindow = parentWindow
		self._commandHistory = commandHistory
		self._sheetEditContext = self._o_createSheetEditContext()
		self._uiElements = []
		self._bEventHandlersInstalled = False


		super( SheetEditor, self ).__init__()


		self._sheetEditorWidget = gtk.VBox()
		self._sheetEditorWidget.show()


		# Add all elements to the _uiElements array, and show them
		for fieldEditor in self.s_SheetEdit_fieldEditorsByDep:
			element = fieldEditor._f_getElementFromInstance( self )
			self._uiElements.append( element )
			if isinstance( fieldEditor, FieldWidgetEditor ):
				fieldEditor.showElement( element )


		# Layout the UI
		for layoutFactory in self.s_SheetEdit_layoutFactories:
			if isinstance( layoutFactory, FormLayoutFactory ):
				layoutWidget = layoutFactory.makeLayout( self._p_getUIElementWidgetForComponent )
			elif callable( layoutFactory ):
				layoutWidget = layoutFactory( self )
				assert layoutWidget is not None, 'layout factory function must not return None'
			else:
				raise TypeError, 'layout is not an instance of FormLayoutFactory and is not callable'
			layoutWidget.show()
			self._sheetEditorWidget.pack_start( layoutWidget, False, False )


		# Initialise
		for fieldEditor in self.s_SheetEdit_fieldEditorsByDep:
			element = fieldEditor._f_getElementFromInstance( self )
			fieldEditor.initialiseElement( element, self._sheetEditContext )





	def shutdownSheetEditor(self):
		# Shutdown
		for fieldEditor in self.s_SheetEdit_fieldEditorsByDep:
			element = fieldEditor._f_getElementFromInstance( self )
			fieldEditor.shutdownElement( element, self._sheetEditContext )




	def getSheetEditorWidget(self):
		return self._sheetEditorWidget




	def _o_createSheetEditContext(self):
		return SheetEditContext( self, self._view, self._editorSettings, self._parentWindow, self._commandHistory )





	def sheetEditorInstallEventHandlers(self):
		if not self._bEventHandlersInstalled:
			self._bEventHandlersInstalled = True
			if self._view is not None:
				self._p_installEventHandlers()

	def sheetEditorUninstallEventHandlers(self):
		if self._bEventHandlersInstalled:
			self._bEventHandlersInstalled = False
			if self._view is not None:
				self._p_uninstallEventHandlers()


	def sheetEditorInstallAccelerators(self, accels):
		assert self._sheetEditContext is not None, 'no sheet attached'
		for fieldEditor in self.s_SheetEdit_fieldEditorsByDep:
			fieldEditor.installElementAccelerators( fieldEditor._f_getElementFromInstance( self ), accels, self._sheetEditContext )

	def sheetEditorUninstallAccelerators(self, accels):
		assert self._sheetEditContext is not None, 'no sheet attached'
		for fieldEditor in reversed( self.s_SheetEdit_fieldEditorsByDep ):
			fieldEditor.uninstallElementAccelerators( fieldEditor._f_getElementFromInstance( self ), accels, self._sheetEditContext )






	def _p_getUIElementWidgetForComponent(self, factoryComponent):
		"""Private - get the widget given a factory component
		@factoryComponent - the factory component"""
		fieldEditor = factoryComponent.getFieldEditor()
		element = fieldEditor._f_getElementFromInstance( self )
		return factoryComponent.getElementWidget( element )



	def _p_installEventHandlers(self):
		for fieldEditor in self.s_SheetEdit_fieldEditorsByDep:
			fieldEditor.installElementEventHandlers( fieldEditor._f_getElementFromInstance( self ), self._sheetEditContext )

	def _p_uninstallEventHandlers(self):
		for fieldEditor in reversed( self.s_SheetEdit_fieldEditorsByDep ):
			fieldEditor.uninstallElementEventHandlers( fieldEditor._f_getElementFromInstance( self ), self._sheetEditContext )


	def _p_attachView(self, view):
		self._view = view
		self._sheetEditContext.view = view
		if self._bEventHandlersInstalled:
			self._p_installEventHandlers()


	def _p_detachView(self):
		if self._view is not None:
			self._view = None
			self._sheetEditContext.view = None
			if self._bEventHandlersInstalled:
				self._p_uninstallEventHandlers()



	@classmethod
	def _o_getFieldEditors(self):
		return self.s_SheetEdit_fieldEditors



	def _o_getElementForFieldEditor(self, fieldEditor):
		"""Private - get the element corresponding to the supplied field editor
		@fieldEditor - the field editor"""
		return fieldEditor._f_getElementFromInstance( self )


	def _p_getView(self):
		return self._view

	def _p_setView(self, view):
		self._p_detachView()
		self._p_attachView( view )




	view = property( _p_getView, _p_setView )







if __name__ == '__main__':
	print 'To test this module, test one of the field editors, e.g. FloatFieldEditEntry'
