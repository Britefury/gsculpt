##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Vector3, Quaternion

from Britefury.UI.UIAction import UIAction

from Britefury.ProceduralTool.ProcToolGroup import ProcToolGroup, ToolGroupActionInstanceProc, ToolGroupActionInstanceSubmenu

from Britefury.TransformationTools.ToolTransformationMove import ProcTransformationMove
from Britefury.TransformationTools.ToolTransformationScale import ProcTransformationScale
from Britefury.TransformationTools.ToolTransformationRotate import ProcTransformationRotate
from Britefury.TransformationTools.ToolTransformationShear import ProcTransformationShear
from Britefury.TransformationTools.ToolTransformationLinearTwist import ProcTransformationLinearTwist
from Britefury.TransformationTools.ToolTransformationRadialTwist import ProcTransformationRadialTwist
from Britefury.TransformationTools.ToolTransformationTaper import ProcTransformationTaper
from Britefury.TransformationTools.ToolTransformationShrink import ProcTransformationShrink
from Britefury.TransformationTools.ToolTransformationDent import ProcTransformationDent
from Britefury.TransformationTools.ToolTransformationBend import ProcTransformationBend

from Britefury.TransformationTools.ToolTransformationBase import TransformationTarget




def _humPathAppend(path, suffix):
	return path + ( suffix, )

def _posAxisActions(path, humanReadablePathTuple, tooltip):
	xAction = UIAction( path + '.X', _humPathAppend( humanReadablePathTuple, 'X' ), 'X', 'X', None, tooltip + ' - X' )
	yAction = UIAction( path + '.Y', _humPathAppend( humanReadablePathTuple, 'Y' ), 'Y', 'Y', None, tooltip + ' - Y' )
	zAction = UIAction( path + '.Z', _humPathAppend( humanReadablePathTuple, 'Z' ), 'Z', 'Z', None, tooltip + ' - Z' )
	return [ xAction, yAction, zAction ]

def _axisActions(path, humanReadablePathTuple, tooltip):
	xAction = UIAction( path + '.+X', _humPathAppend( humanReadablePathTuple, '+X' ), '+X', '+X', None, tooltip + ' - +X' )
	nxAction = UIAction( path + '.-X', _humPathAppend( humanReadablePathTuple, '-X' ), '-X', '-X', None, tooltip + ' - -X' )
	yAction = UIAction( path + '.+Y', _humPathAppend( humanReadablePathTuple, '+Y' ), '+Y', '+Y', None, tooltip + ' - +Y' )
	nyAction = UIAction( path + '.-Y', _humPathAppend( humanReadablePathTuple, '-Y' ), '-Y', '-Y', None, tooltip + ' - -Y' )
	zAction = UIAction( path + '.+Z', _humPathAppend( humanReadablePathTuple, '+Z' ), '+Z', '+Z', None, tooltip + ' - +Z' )
	nzAction = UIAction( path + '.-Z', _humPathAppend( humanReadablePathTuple, '-Z' ), '-Z', '-Z', None, tooltip + ' - -Z' )
	return [ xAction, nxAction, yAction, nyAction, zAction, nzAction ]



_moveAction = UIAction( 'Tools.Deformations.Move', ( _( 'Tools' ), _( 'Deformations' ), _( 'Move' ) ),
			_( 'Move' ), _( 'Move' ), 'w', _( 'Move' ) )
_scaleAction = UIAction( 'Tools.Deformations.Scale', ( _( 'Tools' ), _( 'Deformations' ), _( 'Scale' ) ),
			_( 'Scale' ), _( 'Scale' ), 'e', _( 'Scale' ) )
_rotateAction = UIAction( 'Tools.Deformations.Rotate', ( _( 'Tools' ), _( 'Deformations' ), _( 'Rotate' ) ),
			_( 'Rotate' ), _( 'Rotate' ), 'r', _( 'Rotate' ) )

_shearAction = UIAction( 'Tools.Deformations.Shear', ( _( 'Tools' ), _( 'Deformations' ), _( 'Shear' ), _( 'Popup menu' ) ),
			_( 'Shear' ), _( 'Shear' ), 'h', _( 'Shear' ) )
_shearSubActions = _axisActions( 'Tools.Deformations.Shear', ( _( 'Tools' ), _( 'Deformations' ), _( 'Shear' ) ), _( 'Shear' ) )

_linearTwistAction = UIAction( 'Tools.Deformations.LinearTwist', ( _( 'Tools' ), _( 'Deformations' ), _( 'Linear twist' ), _( 'Popup menu' ) ),
			_( 'L. Twist' ), _( 'Linear twist' ), 't', _( 'Linear twist' ) )
_linearTwistSubActions = _axisActions( 'Tools.Deformations.LinearTwist', ( _( 'Tools' ), _( 'Deformations' ), _( 'Linear twist' ) ), _( 'Linear twist' ) )

_radialTwistAction = UIAction( 'Tools.Deformations.Radial Twist', ( _( 'Tools' ), _( 'Deformations' ), _( 'Radial twist' ), _( 'Popup menu' ) ),
			_( 'R. Twist' ), _( 'Radial twist' ), '<shift>t', _( 'Radial twist' ) )
_radialTwistSubActions = _posAxisActions( 'Tools.Deformations.Radial Twist', ( _( 'Tools' ), _( 'Deformations' ), _( 'Radial twist' ) ), _( 'Radial twist' ) )

_taperAction = UIAction( 'Tools.Deformations.Taper', ( _( 'Tools' ), _( 'Deformations' ), _( 'Taper' ), _( 'Popup menu' ) ),
			_( 'Taper' ), _( 'Taper' ), 'p', _( 'Taper' ) )
_taperSubActions = _axisActions( 'Tools.Deformations.Taper', ( _( 'Tools' ), _( 'Deformations' ), _( 'Taper' ) ), _( 'Taper' ) )

_shrinkAction = UIAction( 'Tools.Deformations.Shrink', ( _( 'Tools' ), _( 'Deformations' ), _( 'Shrink' ), _( 'Popup menu' ) ),
			_( 'Shrink' ), _( 'Shrink' ), 'i', _( 'Shrink' ) )
_shrinkSubActions = _posAxisActions( 'Tools.Deformations.Shrink', ( _( 'Tools' ), _( 'Deformations' ), _( 'Shrink' ) ), _( 'Shrink' ) )

_dentAction = UIAction( 'Tools.Deformations.Dent', ( _( 'Tools' ), _( 'Deformations' ), _( 'Dent' ), _( 'Popup menu' ) ),
			_( 'Dent' ), _( 'Dent' ), 'd', _( 'Dent' ) )
_dentSubActions = _posAxisActions( 'Tools.Deformations.Dent', ( _( 'Tools' ), _( 'Deformations' ), _( 'Dent' ) ), _( 'Dent' ) )

_bendAction = UIAction( 'Tools.Deformations.Bend', ( _( 'Tools' ), _( 'Deformations' ), _( 'Bend' ), _( 'Popup menu' ) ),
			_( 'Bend' ), _( 'Bend' ), 'b', _( 'Bend' ) )
_bendSubActions = _axisActions( 'Tools.Deformations.Bend', ( _( 'Tools' ), _( 'Deformations' ), _( 'Bend' ) ),_( 'Bend' ) )


_xformMenuAction = UIAction( 'Tools.Deformations.DeformationsMenu', ( _( 'Tools' ), _( 'Deformations' ), _( 'Deformations menu' ) ),
			_( 'Deformations' ), _( 'Deformations' ), None, _( 'Deformation tools' ) )


class TransformationToolGroup (ProcToolGroup):
	def __init__(self, nodeEditor, target, bMoveSclRot, bInstallAccelerators):
		""" target - should be of type ToolTransformationBase.TransformationTarget"""
		super( TransformationToolGroup, self ).__init__( nodeEditor )

		self._bInstallAccelerators = bInstallAccelerators
		self._bMoveSclRot = bMoveSclRot

		self._target = target

		if self._bMoveSclRot:
			self._moveAction = _moveAction.instance( ToolGroupActionInstanceProc, self, self._p_procFactory( ProcTransformationMove ) )
			self._scaleAction = _scaleAction.instance( ToolGroupActionInstanceProc, self, self._p_procFactory( ProcTransformationScale ) )
			self._rotateAction = _rotateAction.instance( ToolGroupActionInstanceProc, self, self._p_procFactory( ProcTransformationRotate ) )


		self._shearAction = self._p_axisActionInstance( _shearAction, _shearSubActions, ProcTransformationShear )
		self._linearTwistAction = self._p_axisActionInstance( _linearTwistAction, _linearTwistSubActions, ProcTransformationLinearTwist )
		self._radialTwistAction = self._p_posAxisActionInstance( _radialTwistAction, _radialTwistSubActions, ProcTransformationRadialTwist )
		self._taperAction = self._p_axisActionInstance( _taperAction, _taperSubActions, ProcTransformationTaper )
		self._shrinkAction = self._p_posAxisActionInstance( _shrinkAction, _shrinkSubActions, ProcTransformationShrink )
		self._dentAction = self._p_posAxisActionInstance( _dentAction, _dentSubActions, ProcTransformationDent )
		self._bendAction = self._p_axisActionInstance( _bendAction, _bendSubActions, ProcTransformationBend )


		self._xformMenuAction = _xformMenuAction.instance( ToolGroupActionInstanceSubmenu, self )


		if self._bMoveSclRot:
			self._xformMenuAction.menu.layout.row()  <<  self._moveAction.contextMenuItem( self._xformMenuAction.menu )  <<		\
											self._scaleAction.contextMenuItem( self._xformMenuAction.menu )
			self._xformMenuAction.menu.layout.row()  <<  self._rotateAction.contextMenuItem( self._xformMenuAction.menu )  <<		\
											self._shearAction.contextMenuItem( self._xformMenuAction.menu )
		else:
			self._xformMenuAction.menu.layout.row()  <<  self._shearAction.contextMenuItem( self._xformMenuAction.menu )  <<  None
		self._xformMenuAction.menu.layout.row()  <<  self._linearTwistAction.contextMenuItem( self._xformMenuAction.menu )  <<		\
										self._radialTwistAction.contextMenuItem( self._xformMenuAction.menu )
		self._xformMenuAction.menu.layout.row()  <<  self._taperAction.contextMenuItem( self._xformMenuAction.menu )  <<		\
										self._shrinkAction.contextMenuItem( self._xformMenuAction.menu )
		self._xformMenuAction.menu.layout.row()  <<  self._dentAction.contextMenuItem( self._xformMenuAction.menu )  <<		\
										self._bendAction.contextMenuItem( self._xformMenuAction.menu )



	def _p_posAxisActionInstance(self, menuActionDeclaration, axisActionDeclarations, procClass):
		xActionDecl, yActionDecl, zActionDecl = axisActionDeclarations
		menuAction = menuActionDeclaration.instance( ToolGroupActionInstanceSubmenu, self )
		xAction = xActionDecl.instance( ToolGroupActionInstanceProc, self, self._p_axisProcFactory( procClass, Vector3( 1.0, 0.0, 0.0 ) ) )
		yAction = yActionDecl.instance( ToolGroupActionInstanceProc, self, self._p_axisProcFactory( procClass, Vector3( 0.0, 1.0, 0.0 ) ) )
		zAction = zActionDecl.instance( ToolGroupActionInstanceProc, self, self._p_axisProcFactory( procClass, Vector3( 0.0, 0.0, 1.0 ) ) )
		menuAction.menu.layout.row()  <<  xAction.contextMenuItem( menuAction.menu )
		menuAction.menu.layout.row()  <<  yAction.contextMenuItem( menuAction.menu )
		menuAction.menu.layout.row()  <<  zAction.contextMenuItem( menuAction.menu )
		return menuAction

	def _p_axisActionInstance(self, menuActionDeclaration, axisActionDeclarations, procClass):
		xActionDecl, nxActionDecl, yActionDecl, nyActionDecl, zActionDecl, nzActionDecl = axisActionDeclarations
		menuAction = menuActionDeclaration.instance( ToolGroupActionInstanceSubmenu, self )
		xAction = xActionDecl.instance( ToolGroupActionInstanceProc, self, self._p_axisProcFactory( procClass, Vector3( 1.0, 0.0, 0.0 ) ) )
		nxAction = nxActionDecl.instance( ToolGroupActionInstanceProc, self, self._p_axisProcFactory( procClass, Vector3( -1.0, 0.0, 0.0 ) ) )
		yAction = yActionDecl.instance( ToolGroupActionInstanceProc, self, self._p_axisProcFactory( procClass, Vector3( 0.0, 1.0, 0.0 ) ) )
		nyAction = nyActionDecl.instance( ToolGroupActionInstanceProc, self, self._p_axisProcFactory( procClass, Vector3( 0.0, -1.0, 0.0 ) ) )
		zAction = zActionDecl.instance( ToolGroupActionInstanceProc, self, self._p_axisProcFactory( procClass, Vector3( 0.0, 0.0, 1.0 ) ) )
		nzAction = nzActionDecl.instance( ToolGroupActionInstanceProc, self, self._p_axisProcFactory( procClass, Vector3( 0.0, 0.0, -1.0 ) ) )
		menuAction.menu.layout.row()  <<  xAction.contextMenuItem( menuAction.menu )  <<  nxAction.contextMenuItem( menuAction.menu )
		menuAction.menu.layout.row()  <<  yAction.contextMenuItem( menuAction.menu )  <<  nyAction.contextMenuItem( menuAction.menu )
		menuAction.menu.layout.row()  <<  zAction.contextMenuItem( menuAction.menu )  <<  nzAction.contextMenuItem( menuAction.menu )
		return menuAction



	def _p_procFactory(self, procClass):
		def factory():
			proc = procClass()
			proc.target = self._target
			return proc
		return factory


	def _p_axisProcFactory(self, procClass, axis):
		def factory():
			proc = procClass()
			proc.target = self._target
			proc.xformRotation = Quaternion.axisToAxis( Vector3( 0.0, 0.0, 1.0 ), axis )
			return proc
		return factory



	def layoutContents(self, layout):
		layout.row()  <<  _( 'Transformations/Deformations' )
		if self._bMoveSclRot:
			layout.row()  <<  self._moveAction.button()  <<  self._scaleAction.button()  <<  self._rotateAction.button()
		layout.row()  <<  self._shearAction.button()  <<  None  <<  None
		layout.row()  <<  self._linearTwistAction.button()  <<  self._radialTwistAction.button()  <<  self._taperAction.button()
		layout.row()  <<  self._shrinkAction.button()  <<  self._dentAction.button()  <<  self._bendAction.button()
		layout.row()


	def initContextMenu(self, contextMenu):
		contextMenu.layout.row()  <<  self._xformMenuAction.contextMenuItem( contextMenu )


