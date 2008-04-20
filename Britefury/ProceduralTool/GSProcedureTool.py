##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Util.SignalSlot import ClassSignal, blockSlot, unblockSlot

from Britefury.Painter import Painter
from Britefury.Painter.RequestPainter import RequestPainterObjectSource
from Britefury.Painter.RequestViewportPainter import RequestViewportPainter

from Britefury.Cell.Cell import Cell
from Britefury.Cell.CellEvaluatorWithExternalDependencies import CellEvaluatorWithExternalDependencies

from Britefury.Sheet.Sheet import *

from Britefury.SheetEdit.SheetEdit import FieldEditor

from Britefury.ProceduralCore.GSProcedure import GSProcedure

from Britefury.ProceduralTool.GSFunctionTool import GSFunctionTool, FunctionToolProductView

from Britefury.Manipulator.FieldManipulators import FieldManipulator










class GSProcedureToolFieldManipulator (FieldManipulator):
	"UI element factory interface"
	def attachInputProductCell(self, element, cell):
		"""Attach the input product cell
		@element - the element
		@cell - the product cell"""
		pass

	def detachInputProductCell(self, element, cell):
		"""Attach the input product cell
		@element - the element
		@cell - the product cell"""
		pass


	def attachOutputProductCell(self, element, cell):
		"""Attach the displayed product cell
		@element - the element
		@cell - the product cell"""
		pass

	def detachOutputProductCell(self, element, cell):
		"""Attach the displayed product cell
		@element - the element
		@cell - the product cell"""
		pass




class PlanProcedureInputProductCellEvaluator (CellEvaluatorWithExternalDependencies):
	def __init__(self, ownerCell):
		super( PlanProcedureInputProductCellEvaluator, self ).__init__( ownerCell )
		self._planCell = None
		self._plan = None
		self._procCell = None



	def attachPlanAndProcedureCells(self, planCell, procCell):
		assert self._planCell is None, 'plan and procedure cells already attached'
		self._planCell = planCell
		self._planCell.changedSignal.connect( self._p_onPlanChanged )
		self._plan = None
		self._procCell = procCell
		self._o_changed()


	def detachPlanAndProcedureCells(self):
		assert self._planCell is not None, 'no plan and procedure cells attached'
		self._planCell.changedSignal.disconnect( self._p_onPlanChanged )
		self._planCell= None
		self._plan = None
		self._procCell = None
		self._o_changed()



	def evaluate(self, cell):
		result = None

		proc = self._procCell.getValue()
		self._p_refreshPlan()
		if proc is not None  and  self._plan is not None:
			procIndex = self._plan.index( proc )

			inputProduct = self._plan.computePartialProduct( procIndex - 1 )

			if inputProduct is not None:
				# Check types
				if proc.canOperateOn( inputProduct ):
					result = inputProduct

		unblockSlot( self._p_onInvalidateProduct )

		return result


	def _p_onInvalidateProduct(self, plan, index):
		assert plan is self._plan, '@plan not attached to this'

		# May need to invalidate the cache
		proc = self._procCell.getValue()
		if proc is not None:
			procIndex = self._plan.index( proc )

			if index <= ( procIndex - 1 ):
				blockSlot( self._p_onInvalidateProduct )
				self._o_changed()



	def _p_onPlanChanged(self):
		if self._plan is not None:
			self._plan.invalidateProductSignal.disconnect( self._p_onInvalidateProduct )
			self._plan = None


	def _p_refreshPlan(self):
		if self._plan is None:
			self._plan = self._planCell.getValue()
			if self._plan is not None:
				self._plan.invalidateProductSignal.connect( self._p_onInvalidateProduct )









class ProcedureToolProductView (FunctionToolProductView):
	def __init__(self):
		super( ProcedureToolProductView, self ).__init__()

		self._inputProductCell = Cell()
		self._inputProductEvaluator = PlanProcedureInputProductCellEvaluator( self._inputProductCell )
		self._inputProductCell.evaluator = self._inputProductEvaluator
		self._view = None
		self._planCell = None
		self._procCell = None



	def initialise(self, view, planCell, procCell, tool):
		self._view = view
		self._planCell = planCell
		self._procCell = procCell
		self._inputProductEvaluator.attachPlanAndProcedureCells( planCell, procCell )


	def shutdown(self, tool):
		self._inputProductEvaluator.detachPlanAndProcedureCells()
		self._view = None
		self._planCell = None
		self._procCell = None



	def _p_getInputProductCell(self):
		return self._inputProductCell

	inputProductCell = property( _p_getInputProductCell )





class BasicProcedureToolProductView (ProcedureToolProductView, RequestPainterObjectSource):
	class ProcInvokeProductCellEvaluator (CellEvaluatorWithExternalDependencies):
		def __init__(self, ownerCell):
			super( BasicProcedureToolProductView.ProcInvokeProductCellEvaluator, self ).__init__( ownerCell )
			self._inputCell = None
			self._procCell = None
			self._proc = None
			self._adjustableInvoker = None
			self.errorHandler = None



		def attachInputAndProcedureCells(self, inputCell, procCell, tool):
			assert self._inputCell is None
			self._inputCell = inputCell
			self._procCell = procCell
			self._procCell.changedSignal.connect( self._p_onProcChanged )
			self._proc = None
			adjustableInvokerClass = tool._f_getAdjustableInvokerClass()
			if adjustableInvokerClass is not None:
				self._adjustableInvoker = adjustableInvokerClass( procCell, inputCell, self.errorHandler )
				tool._f_setAdjustableInvoker( self._adjustableInvoker )

		def detachInputAndProcedureCells(self, tool):
			assert self._inputCell is not None
			self._proc = None
			self._inputCell = None
			self._procCell.changedSignal.disconnect( self._p_onProcChanged )
			self._procCell = None
			if self._adjustableInvoker is not None:
				self._adjustableInvoker.shutdownAdjustableInvoker()
				self._adjustableInvoker = None
				tool._f_setAdjustableInvoker( None )


		def evaluate(self, cell):
			assert self._inputCell is not None

			unblockSlot( self._p_onProcFieldCellModified )

			if self._adjustableInvoker is not None:
				return self._adjustableInvoker.immutable.result
			else:
				self._p_refreshProc()

				# Use the immutable value, since we are about to copy it
				inputProduct = self._inputCell.getImmutableValue()

				if inputProduct is not None:
					return self._proc.invoke( deepcopy( inputProduct ), self.errorHandler )
				else:
					return None


		def _p_onProcFieldCellModified(self, func):
			assert func is self._proc
			self._o_changed()
			blockSlot( self._p_onProcFieldCellModified )


		def _p_onProcChanged(self):
			if self._proc is not None:
				self._proc.cellModifiedSignal.disconnect( self._p_onProcFieldCellModified )
				self._proc = None


		def _p_refreshProc(self):
			if self._proc is None:
				self._proc = self._procCell.getValue()
				if self._proc is not None:
					self._proc.cellModifiedSignal.connect( self._p_onProcFieldCellModified )



	def __init__(self, painterFactory):
		super( BasicProcedureToolProductView, self ).__init__()
		self._procResultCell = Cell()
		self._procResultCellEvaluator = BasicProcedureToolProductView.ProcInvokeProductCellEvaluator( self._procCell )
		self._procResultCell.evaluator = self._procResultCellEvaluator
		self._painter = RequestViewportPainter( painterFactory, False )



	def initialise(self, view, planCell, procCell, tool):
		super( BasicProcedureToolProductView, self ).initialise( view, planCell, procCell, tool )

		self._painter.painterObjectSource = self
		self._procResultCellEvaluator.attachInputAndProcedureCells( self._inputProductCell, procCell, tool )
		self._procResultCell.changedSignal.connect( self._p_onProcResultChanged )


	def shutdown(self, tool):
		super( BasicProcedureToolProductView, self ).shutdown( tool )

		self._painter.painterObjectSource = None
		self._procResultCellEvaluator.detachInputAndProcedureCells( tool )
		self._procResultCell.changedSignal.disconnect( self._p_onProcResultChanged )



	def installPainters(self, view):
		self._painter.addToView( view )


	def uninstallPainters(self, view):
		self._painter.removeFromView( view )


	def requestObjectToPaint(self):
		return self.objectToPaint


	def _p_onProcResultChanged(self):
		self._view.postRedraw()



	def _p_setErrorHandler(self, errorHandler):
		self._procSource.errorHandler = errorHandler

	def _p_getOutputProductCell(self):
		return self._procResultCell

	def _p_getDisplayedProduct(self):
		return self._procResultCell.getImmutableValue()

	def _p_getObjectToPaint(self):
		return self._procResultCell.getImmutableValue()

	errorHandler = property( None, _p_setErrorHandler )
	outputProductCell = property( _p_getOutputProductCell )
	displayedProduct = property( _p_getDisplayedProduct )
	objectToPaint = property( _p_getObjectToPaint )









class GSProcedureTool (GSFunctionTool):
	functionClass = GSProcedure


	def _plan(self):
		proc = self.sheet
		if proc is not None:
			return proc.r_plan
		else:
			return None

	plan = FunctionRefField( _plan )



	def __init__(self, view, editorSettings, parentWindow, commandHistory, painterFactory):
		super( GSProcedureTool, self ).__init__( view, editorSettings, parentWindow, commandHistory, painterFactory )

		productView = self._productViewCell.value
		for fieldEditor in self._o_getFieldEditors():
			if isinstance( fieldEditor, GSProcedureToolFieldManipulator ):
				element = self._o_getElementForFieldEditor( fieldEditor )
				fieldEditor.attachInputProductCell( element, productView.inputProductCell )
				fieldEditor.attachOutputProductCell( element, productView.outputProductCell )



	def shutdownTool(self):
		productView = self._productViewCell.value
		for fieldEditor in self._o_getFieldEditors():
			if isinstance( fieldEditor, GSProcedureToolFieldManipulator ):
				element = self._o_getElementForFieldEditor( fieldEditor )
				fieldEditor.detachOutputProductCell( element, productView.outputProductCell )
				fieldEditor.detachInputProductCell( element, productView.inputProductCell )

		super( GSProcedureTool, self ).shutdownTool()



	def _p_createProductView(self):
		v = BasicProcedureToolProductView( self.painterFactory )
		v.initialise( self.view, self.cells.plan, self.cells.sheet, self )
		return v


	def _p_shutdownProductView(self, productView):
		productView.shutdown( self )




	def _p_getProcedure(self):
		return self.sheet

	def _p_getInputProduct(self):
		productView = self._productViewCell.value
		if productView is not None:
			return productView.inputProductCell.getValue()
		else:
			return None

	procedure = property( _p_getProcedure )
	inputProduct = property( _p_getInputProduct )






