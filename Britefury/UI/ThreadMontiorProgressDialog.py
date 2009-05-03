##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import gtk
import gobject




def getResultFromMonitoredThreadWithProgressDialog(threadMonitor, dialogTitle, dialogText, parentWindow, stages=[]):
	def _timeout():
		if threadMonitor.isFinished():
			for bar, stage in zip( progressBars, stages ):
				bar.set_fraction( 1.0 )
				bar.set_text( stage[1] )
			dialog.destroy()
			return False
		else:
			progressStageIndex = threadMonitor.getProgressStage()
			progress = threadMonitor.getProgress()
			
			for i, ( bar, ( inProgressText, finishedText ) )   in   enumerate( zip( progressBars, stages ) ):
				if i < progressStageIndex:
					bar.set_fraction( 1.0 )
					bar.set_text( finishedText )
				elif i > progressStageIndex:
					bar.set_fraction( 0.0 )
					bar.set_text( '' )
				else:
					bar.set_fraction( progress )
					bar.set_text( '%s: %.1f%%'  %  ( inProgressText, progress * 100.0 ) )
					
		return bStillRunning
	
	
	bStillRunning = True
	dialog = gtk.Dialog( dialogTitle, parentWindow, gtk.DIALOG_MODAL, None )
	
	progressBars = [ gtk.ProgressBar()   for stage in stages ]
	
	dialog.vbox.pack_start( gtk.Label( dialogText ), False, False, 25 )
	for bar in progressBars:
		dialog.vbox.pack_start( bar, False, False, 0 )
	
	dialog.set_size_request( 640, -1 )
	
	gobject.timeout_add( 150, _timeout )
	
	dialog.show_all()
	dialog.run()
	bStillRunning = False
	
	return threadMonitor.getResult()
