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
#include <Util/Array.h>


template <typename ResultType> class MonitoredThread
{
private:
	int progressStage;
	float progress;
	ResultType result;
	GMutex *mutex;
	GThread *thread;
	bool bJoined, bFinished;
	
public:
	inline MonitoredThread()
	{
		progressStage = 0;
		progress = 0.0f;
		
		if ( !g_thread_supported() )
		{
			g_thread_init( NULL );
		}
		
		mutex = g_mutex_new();
		
		bJoined = false;
		bFinished = false;
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
	
	
	
	inline float getProgress()
	{
		g_mutex_lock( mutex );
		float p = progress;
		g_mutex_unlock( mutex );
		return p;
	}
	
	inline float getProgressStage()
	{
		g_mutex_lock( mutex );
		int stage = progressStage;
		g_mutex_unlock( mutex );
		return stage;
	}
	
	inline void updateProgress(int progressStage, float progress)
	{
		g_mutex_lock( mutex );
		this->progressStage = progressStage;
		this->progress = progress;
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
	
	
	inline bool isFinished()
	{
		return bFinished;
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
	inline static gpointer threadFunc(MonitoredThread<ResultType> *t)
	{
		t->result = t->execute();
		t->bFinished = true;
		return NULL;
	}
};



template <typename ResultType> class ThreadProgressMonitor : public ProgressMonitor
{
private:
	MonitoredThread<ResultType> *thread;
	int progressStage;
	
public:
	inline ThreadProgressMonitor(MonitoredThread<ResultType> *thread, int progressStage)
	{
		this->thread = thread;
		this->progressStage = progressStage;
	}
	
	virtual ~ThreadProgressMonitor()
	{
	}
	
	
	virtual void updateProgress(float progress)
	{
		thread->updateProgress( progressStage, progress );
	}
};


#endif
