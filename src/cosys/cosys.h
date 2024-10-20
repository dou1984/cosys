/*
 *	cosys - c++20 coroutine library
 *
 *	Copyright (c) 2023, Zhao Yun Shan
 *	All rights reserved.
 *
 */
#pragma once
#include <stdint.h>
#include <ev.h>
#include <coev/coev.h>

#define max_ev_loop (0x100)

namespace cosys
{
	using namespace coev;
	struct cosys final
	{
		static void start();
		static struct ev_loop *data();
		static struct ev_loop *at(uint64_t);
		static void resume(event *);
		static void stop();
	};
}
