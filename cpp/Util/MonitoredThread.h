//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MONITOREDTHREAD_H__
#define MONITOREDTHREAD_H__

#include <glib.h>

#include <stdlib.h>

#include <Util/ProgressMonitor.h>


template <typename ProgressType, typename ResultType> class MonitoredThread
{
private:
	ProgressType progress;
	ResultType result;
	GMutex *mutex;
	GThread *thread;
	bool bJoined;
	
public:
	inline MonitoredThread()
	{
		if ( !g_thread_supported() )
		{
			g_thread_init( NULL );
		}
		
		mutex = g_mutex_new();
		
		bJoined = false;
		thread = NULL;
	}
	
	virtual ~MonitoredThread()
	{
		join();
		g_mutex_free( mutex );
	}
	
	
	inline void run()
	{
		thread = g_thread_create( (GThreadFunc)&threadFunc, (gpointer)this, true, NULL );
	}
	
	
	
	inline ProgressType getProgress()
	{
		g_mutex_lock( mutex );
		ProgressType p = progress;
		g_mutex_unlock( mutex );
		return p;
	}
	
	inline void updateProgress(const ProgressType &v)
	{
		g_mutex_lock( mutex );
		progress = v;
		g_mutex_unlock( mutex );
	}
	
	
	inline ResultType getResult()
	{
		join();
		g_mutex_lock( mutex );
		ResultType r = result;
		g_mutex_unlock( mutex );
		return r;
	}
	
	
	
	inline void join()
	{
		if ( !bJoined )
		{
			g_thread_join( thread );
			bJoined = true;
		}
	}
	
	
protected:
	virtual ResultType execute() = 0;
	
			 
	inline void setResult(const ResultType &r)
	{
		g_mutex_lock( mutex );
		result = r;
		g_mutex_unlock( mutex );
	}

	
	
private:
	inline static gpointer threadFunc(MonitoredThread<ProgressType, ResultType> *t)
	{
		t->result = t->execute();
		return NULL;
	}
};



template <typename ProgressType, typename ResultType> class ThreadProgressMonitor : public ProgressMonitor<ProgressType>
{
private:
	MonitoredThread<ProgressType, ResultType> *thread;
	
public:
	inline ThreadProgressMonitor(MonitoredThread<ProgressType, ResultType> *thread)
	{
		this->thread = thread;
	}
	
	virtual ~ThreadProgressMonitor()
	{
	}
	
	
	virtual void updateProgress(const ProgressType &v)
	{
		thread->updateProgress( v );
	}
};


#endif
