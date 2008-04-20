##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import pygtk
import gtk

import LayoutFactory



"Form UI layout"


def gcd(u, v):
	"""Compute the greatest common deniminator of the integers @u and @v
	An implementation of J. Stein's binary greatest common denominator algorithm (1967)
	Used www.wikipedia.org as a reference"""
	k = 0

	while ( u & 1 ) == 0  and  ( v & 1 ) == 0:
		u = u >> 1
		v = v >> 1
		k = k + 1

	while u > 0:
		if ( u & 1 ) == 0:
			u = u >> 1
		elif ( v & 1 ) == 0:
			v = v >> 1
		elif u >= v:
			u = ( u - v ) >> 1
		else:
			v = ( v - u ) >> 1

	return v << k

def computeRequiredWidth(currentWidth, rowWidth):
	"Compute the required grid width given the current width (@currentWidth), and the width of an additional row (@rowWidth)"
	if currentWidth == rowWidth:
		return currentWidth;
	elif rowWidth == 0:
		return currentWidth;
	elif currentWidth == 0:
		return rowWidth;
	else:
		denom = gcd( currentWidth, rowWidth )
		return currentWidth * rowWidth / denom



class FormLayoutNotWidgetException: pass



class FormLayoutBase (object):
	"""Form layout class
	Note: after adding the widgets to the layout, it must be finalised using the 'finalise()' method before use"""

	class SectionBase (object):
		class Row (object):
			"Form layout row class"
			def __init__(self, width):
				"""Constructor:
				@width - initial width of the row"""
				self._entries = []
				self._width = width
				self._widthAccumulator = 0


			def __lshift__(self, x):
				"Left shift operator - append widget to row"
				if x is None:
					self.append( None, 1 )
				elif isinstance( x, tuple )  or  isinstance( x, list ):
					assert len( x ) == 2, 'The tuple / list should be of the form (widget, width)'
					self.append( x[0], x[1] )
				else:
					self.append( x, 1 )
				return self

			def append(self, widget, width):
				"Append a widget, using @width spaces, to the row"
				self._widthAccumulator += width
				self._width = max( self._width, self._widthAccumulator )
				self._entries.append( (widget, width ) )


		def __init__(self):
			"Constructor"
			self._rows = []
			self._bFinalised = False

		def block(self, height, width = 0):
			"Create a new block of height @height, with an initial width of @width"
			assert not self._bFinalised, "cannot modify finalised form layout"
			r = self.Row( width )
			self._rows.append( (r, height) )
			return r


		def i_setTableSize(self, width, height):
			"Set the size of the table"
			assert False, 'abstract'

		def i_tableAttach(self, child, leftAttach, rightAttach, topAttach, bottomAttach):
			"Internal - Attach a child to the table"
			assert False, 'abstract'

		def i_tableFinish(self):
			"Internal - Finish the table; show etc."
			assert False, 'abstract'

		def _p_createWidget(self, child):
			"Private - Create a widget for a child"
			if isinstance( child, str ):
				label = gtk.Label( child )
				label.show()
				return label
			elif isinstance( child, gtk.Widget ):
				return child
			else:
				try:
					getWidget = child.getWidget
				except AttributeError:
					raise TypeError, 'widget is not a gtk.Widget and does not have a getWidget method, it is a %s' % ( child, )
				else:
					return getWidget()


		def finalise(self):
			"Finalise the form layout section"
			if not self._bFinalised:
				tableWidth = 0
				tableHeight = 0

				for r, height in self._rows:
					w = r._width
					tableWidth = computeRequiredWidth( tableWidth, w )
					tableHeight += height

				tableWidth = max( tableWidth, 1 )
				tableHeight = max( tableHeight, 1 )
				self.i_setTableSize( tableWidth, tableHeight )

				y = 0
				for r, rowHeight in self._rows:
					if r._width > 0  and  len( r._entries )  >  0:
						rowXScale = tableWidth / r._width

						x = 0
						for e in r._entries:
							scaledWidth = e[1] * rowXScale

							if e[0] != None:
								self.i_tableAttach( e[0], x, x + scaledWidth, y, y + rowHeight )

							x += scaledWidth

					y += rowHeight

				self.i_tableFinish()

				self._bFinalised = True


		def _p_isEmpty(self):
			return len( self._rows ) == 0




	class TextBlock (object):
		"Form layout text block class"
		def __init__(self, layout, text):
			self._layout = layout
			self._text = text


		def getWidget(self):
			view = gtk.TextView()
			view.get_buffer().set_text( self._text )
			view.set_wrap_mode( gtk.WRAP_WORD )
			view.set_editable( False )
			view.set_cursor_visible( False )
			view.show()
			return view






	def __init__(self, sectionFactory):
		"Constructor"
		self._bFinalised = False
		self._currentSection = None
		self._sectionFactory = sectionFactory
		self.section()



	def row(self, width = 0):
		"Create a new row, with an initial width of @width"
		assert not self._bFinalised, 'form layout must not be finalised'
		return self.block( 1, width )

	def block(self, height, width = 0):
		"Create a new block of height @height, with an initial width of @width"
		assert not self._bFinalised, 'form layout must not be finalised'
		return self._currentSection.block( height, width )

	def textBlock(self, text):
		"Create a multi-line text block"
		assert not self._bFinalised, 'form layout must not be finalised'
		textBlock = self.TextBlock( self, text )
		self.section()
		self.row()  <<  textBlock
		self.section()
		return textBlock

	def section(self, bHomogeneous=None, xAttachOptions=None, yAttachOptions=None, xPadding=None, yPadding=None):
		"Start a new section"
		assert not self._bFinalised, 'form layout must not be finalised'

		if self._currentSection is None  or  not self._currentSection._p_isEmpty():
			self._p_finaliseCurrentSection()
			self._currentSection = self._sectionFactory( bHomogeneous, xAttachOptions, yAttachOptions, xPadding, yPadding )
			self.i_newSection( self._currentSection )


	def finalise(self):
		"Finalise the form layout"
		if not self._bFinalised:
			self._p_finaliseCurrentSection()
			self._bFinalised = True


	def _p_finaliseCurrentSection(self):
		"Finalise the current section"
		if self._currentSection is not None:
			if not self._currentSection._p_isEmpty():
				self._currentSection.finalise()
				self.i_packSection( self._currentSection )


	def i_packSection(self, section):
		"Internal - Pack the section"
		assert False, abstract

	def i_newSection(self, section):
		"Internal - New section"
		pass


	@staticmethod
	def space(w):
		return ( None, w )








class FormLayout (FormLayoutBase):
	"""Form layout class
	Note: after adding the widgets to the layout, it must be finalised using the 'finalise()' method before use"""

	class Section (FormLayoutBase.SectionBase):
		def __init__(self, bHomogeneous, xAttachOptions, yAttachOptions, xPadding, yPadding):
			super( FormLayout.Section, self ).__init__()
			"""Constructor:
			@xAttachOptions - X GTK table attach options
			@yAttachOptions - Y GTK table attach options"""
			self._xAttachOptions = xAttachOptions
			self._yAttachOptions = yAttachOptions
			self._xPadding = xPadding
			self._yPadding = yPadding
			self._table = gtk.Table( 1, 1, bHomogeneous )

		def getWidget(self):
			assert self._bFinalised, 'Section must be finalised before widget can be acquired'
			return self._table

		def i_setTableSize(self, width, height):
			"Internal - Set the size of the table"
			self._table.resize( height, width )

		def i_tableAttach(self, child, leftAttach, rightAttach, topAttach, bottomAttach):
			"Internal - Attach a child to the table"
			self._table.attach( self._p_createWidget( child ), leftAttach, rightAttach, topAttach, bottomAttach, self._xAttachOptions, self._yAttachOptions, self._xPadding, self._yPadding )

		def i_tableFinish(self):
			"Internal - Finish the table; show etc."
			self._table.show()



	def __init__(self, bHomogeneous=True, xAttachOptions=gtk.SHRINK | gtk.FILL | gtk.EXPAND, yAttachOptions=gtk.SHRINK | gtk.FILL, xPadding=0, yPadding=0):
		"""Constructor:
		@xAttachOptions - X GTK table attach options
		@yAttachOptions - Y GTK table attach options"""
		def sectionFactory(_bHomogeneous, _xAttachOptions, _yAttachOptions, _xPadding, _yPadding):
			if _bHomogeneous is None:
				_bHomogeneous = bHomogeneous
			if _xAttachOptions is None:
				_xAttachOptions = xAttachOptions
			if _yAttachOptions is None:
				_yAttachOptions = yAttachOptions
			if _xPadding is None:
				_xPadding = xPadding
			if _yPadding is None:
				_yPadding = yPadding
			return FormLayout.Section( bHomogeneous, xAttachOptions, yAttachOptions, xPadding, yPadding )

		self._box = gtk.VBox()
		super( FormLayout, self ).__init__( sectionFactory )



	def getWidget(self):
		"Get the widget of the form layout"
		assert self._bFinalised, "form layout must be finalised before use"
		return self._box


	def i_packSection(self, section):
		"Internal - pack the section"
		section.getWidget().show()
		self._box.pack_start( section.getWidget(), False, True )






class FormLayoutFactory (LayoutFactory.LayoutFactory, FormLayoutBase):
	"""Form layout factory class
	Note: after adding the widgets to the layout, it must be finalised using the 'finalise()' method before use"""

	class Section (FormLayoutBase.SectionBase):
		def __init__(self, bHomogeneous, xAttachOptions, yAttachOptions, xPadding, yPadding):
			super( FormLayoutFactory.Section, self ).__init__()
			"""Constructor:
			@bHomogeneous - True if packing should be homogeneous (all widgets the same size)
			@xAttachOptions - X GTK table attach options
			@yAttachOptions - Y GTK table attach options"""
			self._xAttachOptions = xAttachOptions
			self._yAttachOptions = yAttachOptions
			self._xPadding = xPadding
			self._yPadding = yPadding
			self._bHomogeneous = bHomogeneous
			self._entries = []
			self._tableWidth = 1
			self._tableHeight = 1

		def i_setTableSize(self, width, height):
			"Internal - Set the size of the table"
			self._tableWidth = width
			self._tableHeight = height

		def i_tableAttach(self, child, leftAttach, rightAttach, topAttach, bottomAttach):
			"Internal - Attach a child to the table"
			self._entries.append( ( child, leftAttach, rightAttach, topAttach, bottomAttach ) )

		def i_tableFinish(self):
			"Internal - Finish the table; show etc."
			pass


		def makeTable(self, widgetMappingFunction):
			table = gtk.Table( self._tableHeight, self._tableWidth, self._bHomogeneous )

			for e in self._entries:
				child, leftAttach, rightAttach, topAttach, bottomAttach = e
				table.attach( self._p_createWidget( child, widgetMappingFunction ), leftAttach, rightAttach, topAttach, bottomAttach, self._xAttachOptions, self._yAttachOptions, self._xPadding, self._yPadding )

			return table

		def _p_createWidget(self, inputWidget, widgetMappingFunction):
			if isinstance( inputWidget, str ):
				label = gtk.Label( inputWidget )
				label.show()
				return label
			else:
				if isinstance( inputWidget, FormLayoutBase.TextBlock ):
					widget = inputWidget
				else:
					widget = widgetMappingFunction( inputWidget )
				if isinstance( widget, gtk.Widget ):
					return widget
				else:
					try:
						getWidget = widget.getWidget
					except AttributeError:
						raise TypeError, 'widget \'%s\' is not a gtk.Widget and does not have a getWidget method, inputWidget=\'%s\'' % ( widget, inputWidget )
					else:
						return getWidget()



	def __init__(self, bHomogeneous=True, xAttachOptions=gtk.SHRINK | gtk.FILL | gtk.EXPAND, yAttachOptions=gtk.SHRINK | gtk.FILL, xPadding=0, yPadding=0):
		"""Constructor:
		@xAttachOptions - X GTK table attach options
		@yAttachOptions - Y GTK table attach options"""
		def sectionFactory(_bHomogeneous, _xAttachOptions, _yAttachOptions, _xPadding, _yPadding):
			if _bHomogeneous is None:
				_bHomogeneous = bHomogeneous
			if _xAttachOptions is None:
				_xAttachOptions = xAttachOptions
			if _yAttachOptions is None:
				_yAttachOptions = yAttachOptions
			if _xPadding is None:
				_xPadding = xPadding
			if _yPadding is None:
				_yPadding = yPadding
			return FormLayoutFactory.Section( _bHomogeneous, _xAttachOptions, _yAttachOptions, _xPadding, _yPadding )

		self._sections = []
		super( FormLayoutFactory, self ).__init__( sectionFactory )



	def i_packSection(self, section):
		"Internal - pack the section"
		self._sections.append( section )


	def makeLayout(self, widgetMappingFunction):
		"Make the layout"
		assert self._bFinalised, "form layout must be finalised before use"

		box = gtk.VBox()

		for s in self._sections:
			table = s.makeTable( widgetMappingFunction )
			table.show()
			box.pack_start( table, False, True )

		return box










if __name__ == '__main__':
	class FormLayoutTest:
		def delete_event(self, widget, event, data=None):
			print( "delete_event" )
			return gtk.FALSE

		def destroy(self, widget, data=None):
			gtk.main_quit()


		def __init__(self):
			self.window = gtk.Window( gtk.WINDOW_TOPLEVEL );
			self.window.connect( 'delete-event', self.delete_event )
			self.window.connect( 'destroy', self.destroy )
			self.window.set_border_width( 10 )

# 			layoutClass = FormLayout;
# 			def layoutWidgetGetter(l):
# 				return l.getWidget()

			layoutClass = FormLayoutFactory;
			def widgetFunctor(w):
				return w
			def layoutWidgetGetter(l):
				return l.makeLayout( widgetFunctor )

			self.layout = layoutClass()
			self.layout.row() << 'ooo'
			self.layout.row() << 'Hello'
			self.layout.row() << 'ooo'
			self.layout.row() << 'world'
			self.layout.row() << 'ooo'
			self.layout.block( 2 ) << 'A' << 'B' << 'C'
			self.layout.row() << 'ooo'
			self.layout.section()
			self.layout.row() << '---'
			self.layout.section()
			self.layout.row() << 'ooo'
			self.layout.block( 3 ) << 'x' << 'y' << 'z'
			self.layout.row() << 'ooo'
			self.layout.block( 2 ) << 'p' << 'q' << 'r'
			self.layout.row() << 'ooo'

			self.layout.finalise()
			w = layoutWidgetGetter( self.layout )
			w.show()

			self.window.add( w );
			self.window.show();

		def main(self):
			gtk.main()

	if __name__ == "__main__":
		hello = FormLayoutTest()
		hello.main()


