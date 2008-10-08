//
// Thread_POSIX.cpp
//
// $Id: //poco/svn/Foundation/src/Thread_POSIX.cpp#2 $
//
// Library: Foundation
// Package: Threading
// Module:  Thread
//
// Copyright (c) 2004-2007, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Thread_POSIX.h"
#include "Poco/Exception.h"
#include "Poco/ErrorHandler.h"
#include <signal.h>
#if defined(__sun) && defined(__SVR4)
#	if !defined(__EXTENSIONS__)
#		define __EXTENSIONS__
#	endif
#	include <limits.h>
#endif


//
// Block SIGPIPE in main thread.
//
#if defined(POCO_OS_FAMILY_UNIX)
namespace
{
	class SignalBlocker
	{
	public:
		SignalBlocker()
		{
			sigset_t sset;
			sigemptyset(&sset);
			sigaddset(&sset, SIGPIPE); 
			pthread_sigmask(SIG_BLOCK, &sset, 0);
		}
		~SignalBlocker()
		{
		}
	};
	
	static SignalBlocker signalBlocker;
}
#endif


namespace Poco {


pthread_key_t ThreadImpl::_currentKey;
bool ThreadImpl::_haveCurrentKey = false;


ThreadImpl::ThreadImpl():
	_pData(new ThreadData)
{
	if (!_haveCurrentKey)
	{
		if (pthread_key_create(&_currentKey, NULL))
			throw SystemException("cannot allocate thread context key");
		_haveCurrentKey = true;
	}
}

			
ThreadImpl::~ThreadImpl()
{
	if (isRunningImpl())
		pthread_detach(_pData->thread);
}


void ThreadImpl::setPriorityImpl(int prio)
{
	if (prio != _pData->prio)
	{
		_pData->prio = prio;
		if (isRunningImpl())
		{
			struct sched_param par;
			par.sched_priority = mapPrio(_pData->prio);
			if (pthread_setschedparam(_pData->thread, SCHED_OTHER, &par))
				throw SystemException("cannot set thread priority");
		}
	}
}


void ThreadImpl::setOSPriorityImpl(int prio)
{
	if (prio != _pData->osPrio)
	{
		if (_pData->pRunnableTarget || _pData->pCallbackTarget)
		{
			struct sched_param par;
			par.sched_priority = prio;
			if (pthread_setschedparam(_pData->thread, SCHED_OTHER, &par))
				throw SystemException("cannot set thread priority");
		}
		_pData->prio   = reverseMapPrio(prio);
		_pData->osPrio = prio;
	}
}


int ThreadImpl::getMinOSPriorityImpl()
{
#if defined(__VMS) || defined(__digital__)
	return PRI_OTHER_MIN;
#else
	return sched_get_priority_min(SCHED_OTHER);
#endif
}


int ThreadImpl::getMaxOSPriorityImpl()
{
#if defined(__VMS) || defined(__digital__)
	return PRI_OTHER_MAX;
#else
	return sched_get_priority_max(SCHED_OTHER);
#endif
}


void ThreadImpl::setStackSizeImpl(int size)
{
#ifndef PTHREAD_STACK_MIN
	_pData->stackSize = 0;
#else
 	if (size !=0 && size < PTHREAD_STACK_MIN)
 		size = PTHREAD_STACK_MIN;

 	_pData->stackSize = size;
#endif // PTHREAD_STACK_MIN
}


void ThreadImpl::startImpl(Runnable& target)
{
	if (_pData->pRunnableTarget)
		throw SystemException("thread already running");

	pthread_attr_t attributes;
	pthread_attr_init(&attributes);

	if (_pData->stackSize != 0)
	{
		if (0 != pthread_attr_setstacksize(&attributes, _pData->stackSize))
			throw SystemException("cannot set thread stack size");
	}

	_pData->pRunnableTarget = &target;
	if (pthread_create(&_pData->thread, &attributes, runnableEntry, this))
	{
		_pData->pRunnableTarget = 0;
		throw SystemException("cannot start thread");
	}

	if (_pData->prio != PRIO_NORMAL_IMPL)
	{
		struct sched_param par;
		par.sched_priority = mapPrio(_pData->prio);
		if (pthread_setschedparam(_pData->thread, SCHED_OTHER, &par))
			throw SystemException("cannot set thread priority");
	}
}


void ThreadImpl::startImpl(Callable target, void* pData)
{
	if (_pData->pCallbackTarget && _pData->pCallbackTarget->callback)
		throw SystemException("thread already running");

	pthread_attr_t attributes;
	pthread_attr_init(&attributes);

	if (_pData->stackSize != 0)
	{
		if (0 != pthread_attr_setstacksize(&attributes, _pData->stackSize))
			throw SystemException("can not set thread stack size");
	}

	if (0 == _pData->pCallbackTarget.get())
		_pData->pCallbackTarget = new CallbackData;

	_pData->pCallbackTarget->callback = target;
	_pData->pCallbackTarget->pData = pData;

	if (pthread_create(&_pData->thread, &attributes, callableEntry, this))
	{
		_pData->pCallbackTarget->callback = 0;
		_pData->pCallbackTarget->pData = 0;
		throw SystemException("cannot start thread");
	}

	if (_pData->prio != PRIO_NORMAL_IMPL)
	{
		struct sched_param par;
		par.sched_priority = mapPrio(_pData->prio);
		if (pthread_setschedparam(_pData->thread, SCHED_OTHER, &par))
			throw SystemException("cannot set thread priority");
	}
}


void ThreadImpl::joinImpl()
{
	_pData->done.wait();
	void* result;
	if (pthread_join(_pData->thread, &result))
		throw SystemException("cannot join thread"); 
}


bool ThreadImpl::joinImpl(long milliseconds)
{
	if (_pData->done.tryWait(milliseconds))
	{
		void* result;
		if (pthread_join(_pData->thread, &result))
			throw SystemException("cannot join thread");
		return true;
	}
	else return false;
}


ThreadImpl* ThreadImpl::currentImpl()
{
	if (_haveCurrentKey)
		return reinterpret_cast<ThreadImpl*>(pthread_getspecific(_currentKey));
	else
		return 0;
}


void* ThreadImpl::runnableEntry(void* pThread)
{
	pthread_setspecific(_currentKey, pThread);

#if defined(POCO_OS_FAMILY_UNIX)
	sigset_t sset;
	sigemptyset(&sset);
	sigaddset(&sset, SIGQUIT);
	sigaddset(&sset, SIGTERM);
	sigaddset(&sset, SIGPIPE); 
	pthread_sigmask(SIG_BLOCK, &sset, 0);
#endif

	ThreadImpl* pThreadImpl = reinterpret_cast<ThreadImpl*>(pThread);
	AutoPtr<ThreadData> pData = pThreadImpl->_pData;
	try
	{
		pData->pRunnableTarget->run();
	}
	catch (Exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (std::exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (...)
	{
		ErrorHandler::handle();
	}

	pData->pRunnableTarget = 0;
	pData->done.set();
	return 0;
}


void* ThreadImpl::callableEntry(void* pThread)
{
	pthread_setspecific(_currentKey, pThread);

#if defined(POCO_OS_FAMILY_UNIX)
	sigset_t sset;
	sigemptyset(&sset);
	sigaddset(&sset, SIGQUIT);
	sigaddset(&sset, SIGTERM);
	sigaddset(&sset, SIGPIPE); 
	pthread_sigmask(SIG_BLOCK, &sset, 0);
#endif

	ThreadImpl* pThreadImpl = reinterpret_cast<ThreadImpl*>(pThread);
	AutoPtr<ThreadData> pData = pThreadImpl->_pData;
	try
	{
		pData->pCallbackTarget->callback(pData->pCallbackTarget->pData);
	}
	catch (Exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (std::exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (...)
	{
		ErrorHandler::handle();
	}

	pData->pCallbackTarget->callback = 0;
	pData->pCallbackTarget->pData = 0;

	pData->done.set();
	return 0;
}


int ThreadImpl::mapPrio(int prio)
{
#if defined(__VMS) || defined(__digital__)
	static const int pmin = PRI_OTHER_MIN;
	static const int pmax = PRI_OTHER_MAX;
#else
	static const int pmin = sched_get_priority_min(SCHED_OTHER);
	static const int pmax = sched_get_priority_max(SCHED_OTHER);
#endif

	switch (prio)
	{
	case PRIO_LOWEST_IMPL:
		return pmin;
	case PRIO_LOW_IMPL:
		return pmin + (pmax - pmin)/4;
	case PRIO_NORMAL_IMPL:
		return pmin + (pmax - pmin)/2;
	case PRIO_HIGH_IMPL:
		return pmin + 3*(pmax - pmin)/4;
	case PRIO_HIGHEST_IMPL:
		return pmax;
	default:
		poco_bugcheck_msg("invalid thread priority");
	}
	return -1; // just to satisfy compiler - we'll never get here anyway
}


int ThreadImpl::reverseMapPrio(int prio)
{
	int pmin = getMinOSPriorityImpl();
	int pmax = getMaxOSPriorityImpl();
	int normal = pmin + (pmax - pmin)/2;
	if (prio == pmax)
		return PRIO_HIGHEST_IMPL;
	if (prio > normal)
		return PRIO_HIGH_IMPL;
	else if (prio == normal)
		return PRIO_NORMAL_IMPL;
	else if (prio > pmin)
		return PRIO_LOW_IMPL;
	else
		return PRIO_LOWEST_IMPL;
}


} // namespace Poco