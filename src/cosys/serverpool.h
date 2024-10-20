/*
 *	cosys - c++20 coroutine library
 *
 *	Copyright (c) 2023, Zhao Yun Shan
 *	All rights reserved.
 *
 */
#pragma once
#include <mutex>
#include <unordered_map>
#include "server.h"

namespace cosys::tcp
{
	class serverpool final
	{
		int m_fd = INVALID;
		std::unordered_map<uint64_t, server> m_pool;
		std::mutex m_mutex;

	public:
		server &get();
		int start(const char *, int);
		int stop();
	};

}