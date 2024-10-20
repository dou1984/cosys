/*
 *	cosys - c++20 coroutine library
 *
 *	Copyright (c) 2023, Zhao Yun Shan
 *	All rights reserved.
 *
 */
#include <coev/coev.h>
#include "awaken.h"
#include "cosys.h"

namespace cosys
{
	void awaken::cb_async(struct ev_loop *loop, ev_async *w, int revents)
	{
		if (revents & EV_ASYNC)
		{
			awaken *_this = (awaken *)w->data;
			assert(_this != nullptr);
			_this->__resume();
		}
	}
	awaken::awaken() : awaken(cosys::data(), gtid())
	{
	}
	awaken::awaken(struct ev_loop *__loop, uint64_t __tag)
	{
		m_awaken.data = this;
		ev_async_init(&m_awaken, awaken::cb_async);
		ev_async_start(__loop, &m_awaken);
		m_tid = __tag;
	}
	awaken::~awaken()
	{
		ev_async_stop(cosys::at(m_tid), &m_awaken);
	}
	int awaken::resume()
	{
		ev_async_send(cosys::at(m_tid), &m_awaken);
		return 0;
	}
	int awaken::resume_event(event *ev)
	{
		std::lock_guard<decltype(m_lock)> _(m_lock);
		m_trigger.push_back(ev);
		return resume();
	}
	int awaken::__resume()
	{
		async _trigger;
		{
			std::lock_guard<decltype(m_lock)> _(m_lock);
			m_trigger.moveto(&_trigger);
		}
		while (coev::notify(_trigger))
		{
		}
		return 0;
	}
}