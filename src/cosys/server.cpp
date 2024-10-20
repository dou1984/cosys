/*
 *	cosys - c++20 coroutine library
 *
 *	Copyright (c) 2023, Zhao Yun Shan
 *	All rights reserved.
 *
 */
#include "server.h"
#include "cosys.h"
#include <coev/coev.h>

namespace cosys::tcp
{
	static int __real_accept(int fd, ipaddress &info)
	{
		sockaddr_in addr;
		socklen_t addr_len = sizeof(sockaddr_in);
		int rfd = accept(fd, (sockaddr *)&addr, &addr_len);
		if (rfd != INVALID)
		{
			parseAddr(addr, info);
		}
		return rfd;
	}
	void server::cb_accept(struct ev_loop *loop, struct ev_io *w, int revents)
	{
		if (EV_ERROR & revents)
			return;
		server *_this = (server *)(w->data);
		assert(_this != nullptr);
		coev::notify(_this->m_trigger);
	}
	server::~server()
	{
		stop();
	}
	int server::start(const char *ip, int port)
	{
		assert(m_fd == INVALID);
		m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
		if (m_fd == INVALID)
		{
			return m_fd;
		}
		int on = 1;
		if (setReuseAddr(m_fd, on) < 0)
		{
		__error_return__:
			::close(m_fd);
			m_fd = INVALID;
			return m_fd;
		}
		if (bindAddr(m_fd, ip, port) < 0)
		{
			goto __error_return__;
		}
		if (setNoBlock(m_fd, true) < 0)
		{
			goto __error_return__;
		}
		if (listen(m_fd, 1000) < 0)
		{
			goto __error_return__;
		}
		return m_fd;
	}
	int server::stop()
	{
		if (m_fd != INVALID)
		{
			__remove(gtid());
			::close(m_fd);
			m_fd = INVALID;
		}
		return 0;
	}
	int server::__insert(uint64_t _tid)
	{
		m_Reav.data = this;
		ev_io_init(&m_Reav, server::cb_accept, m_fd, EV_READ);
		ev_io_start(cosys::at(_tid), &m_Reav);
		return m_fd;
	}
	int server::__remove(uint64_t _tid)
	{
		ev_io_stop(cosys::at(_tid), &m_Reav);
		return m_fd;
	}
	bool server::__valid() const
	{
		return m_fd != INVALID;
	}
	awaitable<int> server::__accept()
	{
		if (!__valid())
		{
			co_return INVALID;
		}
		co_await coev::wait_for(m_trigger);
		ipaddress peer;
		auto fd = __real_accept(m_fd, peer);
		if (fd != INVALID)
		{
			setNoBlock(fd, true);
		}
		[=, this]() -> awaitable<int>
		{
			iocontext io(fd);
			co_return co_await m_dispatch(peer, io);
		}();
		co_return fd;
	}
	awaitable<int> server::accept(const faccept &dispatch)
	{
		m_dispatch = dispatch;
		while (__valid())
		{
			co_await __accept();
		}
		co_return INVALID;
	}
}