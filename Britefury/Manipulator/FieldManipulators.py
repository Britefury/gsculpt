##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Point3, Quaternion, Vector3

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.SheetEdit import *

import ScrollManipulator, Point3Manipulator, TranslationManipulator3d, ScaleManipulator3d, RotationManipulator, RealManipulator, ManipulatorSwitch






class FieldManipulatorSnapping (object):
	def __init__(self, attrName):
		super( FieldManipulatorSnapping, self ).__init__()
		self._attrName = attrName

	def getManipulatorSnapping(self, sheetEditContext):
		return getattr( sheetEditContext.editorSettings.snapSettings, self._attrName )


class FieldManipulatorSnappingDistance (FieldManipulatorSnapping):
	def __init__(self):
		super( FieldManipulatorSnappingDistance, self ).__init__( 'distance' )

class FieldManipulatorSnappingFraction (FieldManipulatorSnapping):
	def __init__(self):
		super( FieldManipulatorSnappingFraction, self ).__init__( 'fraction' )

class FieldManipulatorSnappingAngle (FieldManipulatorSnapping):
	def __init__(self):
		super( FieldManipulatorSnappingAngle, self ).__init__( 'angle' )





class FieldManipulator (FieldEditor):
	def __init__(self):
		super( FieldManipulator, self ).__init__()
		self._bEnabled = False

	def enable(self):
		assert not self._f_metaMember_isInitialised(), 'cannot enable; field manipulator has already been initialised'
		self._bEnabled = True

	def disable(self):
		assert not self._f_metaMember_isInitialised(), 'cannot disable; field manipulator has already been initialised'
		self._bEnabled = False


	def _o_getManipulatorSnapping(self, fieldManipulatorSnapping, sheetEditContext):
		if fieldManipulatorSnapping is not None:
			if isinstance( fieldManipulatorSnapping, FieldInterface ):
				return FieldEditor._p_getCell( fieldManipulatorSnapping, sheetEditContext )
			else:
				return fieldManipulatorSnapping.getManipulatorSnapping( sheetEditContext )
		else:
			return None




class FieldManipulatorSwitch (FieldManipulator):
	def __init__(self, fieldManipulators):
		super( FieldManipulatorSwitch, self ).__init__()
		self._fieldManipulators = fieldManipulators
		for fieldManipulator in fieldManipulators:
			self._o_addDependency( fieldManipulator )


	def createElement(self, sheetEditContext):
		manipulators = [ sheetEditContext.sheetEditor._o_getElementForFieldEditor( fieldManip )  for fieldManip in self._fieldManipulators ]
		return ManipulatorSwitch.ManipulatorSwitch( manipulators )

	def initialiseElement(self, element, sheetEditContext):
		super( FieldManipulatorSwitch, self ).initialiseElement( element, sheetEditContext )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		super( FieldManipulatorSwitch, self ).shutdownElement( element, sheetEditContext )
		element.commandHistory = None

	def installElementEventHandlers(self, element, sheetEditContext):
		if self._bEnabled:
			element.manipAttachToView( sheetEditContext.view )

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		if self._bEnabled:
			element.manipDetachFromView()




class FieldManipulatorSwitchRadioGroup (FieldWidgetEditor):
	class ChoiceComponent (FieldEditorComponent):
		def __init__(self, fieldManipSwitchRadioGroup, index):
			super( FieldManipulatorSwitchRadioGroup.ChoiceComponent, self ).__init__()
			self._fieldManipSwitchRadioGroup = fieldManipSwitchRadioGroup
			self._index = index
			self._o_addDependency( fieldManipSwitchRadioGroup )

		def getFieldEditor(self):
			"Get the field editor that owns this field editor component"
			return self._fieldManipSwitchRadioGroup

		def getElementWidget(self, element):
			"""Get the widget of @element that is represented by this factory component
			@element - the UI element"""
			return element.getButtonByIndex( self._index )


	def __init__(self, fieldManipSwitch):
		super( FieldManipulatorSwitchRadioGroup, self ).__init__()
		self._fieldManipSwitch = fieldManipSwitch
		self._o_addDependency( self._fieldManipSwitch )
		self._choices = []



	def addChoice(self, fieldManipulator, action):
		index = len( self._choices )
		self._o_addDependency( fieldManipulator )
		self._choices.append( ( fieldManipulator, action ) )
		return FieldManipulatorSwitchRadioGroup.ChoiceComponent( self, index )


	def createElement(self, sheetEditContext):
		element = ManipulatorSwitch.ManipulatorSwitchRadioGroup()
		# Attach the manipulator switch here rather than in initialiseElement()
		manipSwitch = self._fieldManipSwitch._f_getElementFromInstance( sheetEditContext.sheetEditor )
		element.attachManipulatorSwitch( manipSwitch )
		for fieldManip, action in self._choices:
			manip = fieldManip._f_getElementFromInstance( sheetEditContext.sheetEditor )
			element.addChoice( manip, action )
		return element


	def shutdownElement(self, element, sheetEditContext):
		super( FieldManipulatorSwitchRadioGroup, self ).shutdownElement( element, sheetEditContext )
		element.detachManipulatorSwitch()


	def installElementAccelerators(self, element, accels, sheetEditContext):
		super( FieldManipulatorSwitchRadioGroup, self ).installElementAccelerators( element, accels, sheetEditContext )
		element.installAccelerators( accels )

	def uninstallElementAccelerators(self, element, accels, sheetEditContext):
		super( FieldManipulatorSwitchRadioGroup, self ).uninstallElementAccelerators( element, accels, sheetEditContext )
		element.uninstallAccelerators( accels )






class SingleFieldManipulator (FieldManipulator):
	def __init__(self, field):
		super( SingleFieldManipulator, self ).__init__()
		self._field = field
		self._o_addDependency( field )


	def initialiseElement(self, element, sheetEditContext):
		super( SingleFieldManipulator, self ).initialiseElement( element, sheetEditContext )
		element.attachCell( self._p_getCell( self._field, sheetEditContext ) )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		super( SingleFieldManipulator, self ).shutdownElement( element, sheetEditContext )
		element.detachCell()
		element.commandHistory = None

	def installElementEventHandlers(self, element, sheetEditContext):
		if self._bEnabled:
			element.manipAttachToView( sheetEditContext.view )

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		if self._bEnabled:
			element.manipDetachFromView()




class IntFieldScrollManipulator (SingleFieldManipulator):
	def __init__(self, field, lower, upper, rate=1):
		super( IntFieldScrollManipulator, self ).__init__( field )
		self._lower = lower
		self._upper = upper
		self._rate = rate


	def createElement(self, sheetEditContext):
		return ScrollManipulator.ScrollManipulator( self._lower, self._upper, self._rate )




class Point3FieldManipulator (SingleFieldManipulator):
	def __init__(self, field, snapping=None):
		super( Point3FieldManipulator, self ).__init__( field )
		self._snapping = snapping


	def createElement(self, sheetEditContext):
		return Point3Manipulator.Point3Manipulator( snapping=self._o_getManipulatorSnapping( self._snapping, sheetEditContext ) )





class PositionedFieldManipulator3d (FieldManipulator):
	def __init__(self, positionField=None):
		super( PositionedFieldManipulator3d, self ).__init__()
		self._positionField = positionField
		if positionField is not None:
			self._o_addDependency( positionField )


	def initialiseElement(self, element, sheetEditContext):
		super( PositionedFieldManipulator3d, self ).initialiseElement( element, sheetEditContext )
		if self._positionField is not None:
			element.positionCell = self._p_getCell( self._positionField, sheetEditContext )

	def shutdownElement(self, element, sheetEditContext):
		super( PositionedFieldManipulator3d, self ).shutdownElement( element, sheetEditContext )
		element.positionCell = None



class TransformedFieldManipulator3d (PositionedFieldManipulator3d):
	def __init__(self, positionField=None, spaceRotationField=None):
		super( TransformedFieldManipulator3d, self ).__init__( positionField )
		self._spaceRotationField = spaceRotationField
		if spaceRotationField is not None:
			self._o_addDependency( spaceRotationField )


	def initialiseElement(self, element, sheetEditContext):
		super( TransformedFieldManipulator3d, self ).initialiseElement( element, sheetEditContext )
		if self._spaceRotationField is not None:
			element.spaceRotationCell = self._p_getCell( self._spaceRotationField, sheetEditContext )

	def shutdownElement(self, element, sheetEditContext):
		super( TransformedFieldManipulator3d, self ).shutdownElement( element, sheetEditContext )
		element.spaceRotationCell = None





class PositionedSingleFieldManipulator3d (PositionedFieldManipulator3d):
	def __init__(self, field, positionField=None):
		super( PositionedSingleFieldManipulator3d, self ).__init__( positionField )
		self._field = field
		self._o_addDependency( field )


	def initialiseElement(self, element, sheetEditContext):
		super( PositionedSingleFieldManipulator3d, self ).initialiseElement( element, sheetEditContext )
		element.attachCell( self._p_getCell( self._field, sheetEditContext ) )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		super( PositionedSingleFieldManipulator3d, self ).shutdownElement( element, sheetEditContext )
		element.detachCell()
		element.commandHistory = None

	def installElementEventHandlers(self, element, sheetEditContext):
		if self._bEnabled:
			element.manipAttachToView( sheetEditContext.view )

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		if self._bEnabled:
			element.manipDetachFromView()



class TransformedSingleFieldManipulator3d (TransformedFieldManipulator3d):
	def __init__(self, field, positionField=None, spaceRotationField=None):
		super( TransformedSingleFieldManipulator3d, self ).__init__( positionField, spaceRotationField )
		self._field = field
		self._o_addDependency( field )


	def initialiseElement(self, element, sheetEditContext):
		super( TransformedSingleFieldManipulator3d, self ).initialiseElement( element, sheetEditContext )
		element.attachCell( self._p_getCell( self._field, sheetEditContext ) )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		super( TransformedSingleFieldManipulator3d, self ).shutdownElement( element, sheetEditContext )
		element.detachCell()
		element.commandHistory = None

	def installElementEventHandlers(self, element, sheetEditContext):
		if self._bEnabled:
			element.manipAttachToView( sheetEditContext.view )

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		if self._bEnabled:
			element.manipDetachFromView()




class Translation2FieldManipulator3d (TransformedSingleFieldManipulator3d):
	def __init__(self, field, positionField, spaceRotationField=None, snapping=None):
		super( Translation2FieldManipulator3d, self ).__init__( field, positionField, spaceRotationField )
		self._snapping = snapping


	def createElement(self, sheetEditContext):
		return TranslationManipulator3d.Translation2Manipulator3d( snapping=self._o_getManipulatorSnapping( self._snapping, sheetEditContext ) )





class Translation3FieldManipulator3d (TransformedSingleFieldManipulator3d):
	def __init__(self, field, positionField, spaceRotationField=None, snapping=None):
		super( Translation3FieldManipulator3d, self ).__init__( field, positionField, spaceRotationField )
		self._snapping = snapping


	def createElement(self, sheetEditContext):
		return TranslationManipulator3d.Translation3Manipulator3d( snapping=self._o_getManipulatorSnapping( self._snapping, sheetEditContext ) )




class Scale2FieldManipulator3d (TransformedSingleFieldManipulator3d):
	def __init__(self, field, positionField, spaceRotationField=None, snapping=None):
		super( Scale2FieldManipulator3d, self ).__init__(field,  positionField, spaceRotationField )
		self._snapping = snapping


	def createElement(self, sheetEditContext):
		return ScaleManipulator3d.Scale2Manipulator3d( snapping=self._o_getManipulatorSnapping( self._snapping, sheetEditContext ) )





class Scale3FieldManipulator3d (TransformedSingleFieldManipulator3d):
	def __init__(self, field, positionField, spaceRotationField=None, snapping=None):
		super( Scale3FieldManipulator3d, self ).__init__( field, positionField, spaceRotationField )
		self._snapping = snapping


	def createElement(self, sheetEditContext):
		return ScaleManipulator3d.Scale3Manipulator3d( snapping=self._o_getManipulatorSnapping( self._snapping, sheetEditContext ) )





class RotationFieldManipulator (TransformedSingleFieldManipulator3d):
	def __init__(self, field, positionField, spaceRotationField=None, snapping=None):
		super( RotationFieldManipulator, self ).__init__( field, positionField, spaceRotationField )
		self._snapping = snapping


	def createElement(self, sheetEditContext):
		return RotationManipulator.RotationManipulator( snapping=self._o_getManipulatorSnapping( self._snapping, sheetEditContext ) )





class RealFieldManipulator (object):
	class WorldSpaceScaleFactory (object):
		def __init__(self, focalPointField):
			self._focalPointField = focalPointField

		def createScale(self, sheetEditContext):
			focalPointCell = FieldEditor._p_getCell( self._focalPointField, sheetEditContext )
			return RealManipulator.RealManipulator.WorldSpaceScale( focalPointCell )





class Real1FieldManipulator (SingleFieldManipulator, RealFieldManipulator):
	def __init__(self, field, lower, upper, scale, snapping=None):
		super( Real1FieldManipulator, self ).__init__( field )
		assert scale is not None, 'must supply a scale'
		self._lower = lower
		self._upper = upper
		self._scale = scale
		self._snapping = snapping
		if isinstance( self._scale, FieldBase ):
			self._o_addDependency( self._scale )


	def createElement(self, sheetEditContext):
		scale = self._scale
		if scale is not None:
			if isinstance( scale, RealFieldManipulator.WorldSpaceScaleFactory ):
				scale = scale.createScale( sheetEditContext )
			elif isinstance( scale, FieldInterface ):
				scale = FieldEditor._p_getCell( scale, sheetEditContext )
		return RealManipulator.Real1Manipulator( self._lower, self._upper, scale, snapping=self._o_getManipulatorSnapping( self._snapping, sheetEditContext ) )






class Real2FieldManipulator (FieldManipulator, RealFieldManipulator):
	def __init__(self, xField, xLower, xUpper, xScale, yField, yLower, yUpper, yScale, xSnapping=None, ySnapping=None):
		super( Real2FieldManipulator, self ).__init__()
		assert xScale is not None, 'must supply an x-scale'
		assert yScale is not None, 'must supply an y-scale'
		self._xField = xField
		self._xLower = xLower
		self._xUpper = xUpper
		self._xScale = xScale
		self._yField = yField
		self._yLower = yLower
		self._yUpper = yUpper
		self._yScale = yScale
		self._xSnapping = xSnapping
		self._ySnapping = ySnapping
		self._o_addDependency( xField )
		self._o_addDependency( yField )
		if isinstance( self._xScale, FieldBase ):
			self._o_addDependency( self._xScale )
		if isinstance( self._yScale, FieldBase ):
			self._o_addDependency( self._yScale )


	def createElement(self, sheetEditContext):
		xScale = self._xScale
		yScale = self._yScale
		if xScale is not None:
			if isinstance( xScale, RealFieldManipulator.WorldSpaceScaleFactory ):
				xScale = xScale.createScale( sheetEditContext )
			elif isinstance( xScale, FieldInterface ):
				xScale = FieldEditor._p_getCell( xScale, sheetEditContext )
		if yScale is not None:
			if isinstance( yScale, RealFieldManipulator.WorldSpaceScaleFactory ):
				yScale = yScale.createScale( sheetEditContext )
			elif isinstance( yScale, FieldInterface ):
				yScale = FieldEditor._p_getCell( yScale, sheetEditContext )
		return RealManipulator.Real2Manipulator( self._xLower, self._xUpper, xScale, self._yLower, self._yUpper, yScale,
										self._o_getManipulatorSnapping( self._xSnapping, sheetEditContext ), self._o_getManipulatorSnapping( self._ySnapping, sheetEditContext ) )

	def initialiseElement(self, element, sheetEditContext):
		super( Real2FieldManipulator, self ).initialiseElement( element, sheetEditContext )
		xCell = None
		yCell = None
		if self._xField is not None:
			xCell = self._p_getCell( self._xField, sheetEditContext )
		if self._yField is not None:
			yCell = self._p_getCell( self._yField, sheetEditContext )
		element.attachCells( xCell, yCell )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		super( Real2FieldManipulator, self ).shutdownElement( element, sheetEditContext )
		element.detachCells()
		element.commandHistory = None

	def installElementEventHandlers(self, element, sheetEditContext):
		if self._bEnabled:
			element.manipAttachToView( sheetEditContext.view )

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		if self._bEnabled:
			element.manipDetachFromView()

