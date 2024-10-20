/*
 *	cosys - c++20 coroutine library
 *
 *	Copyright (c) 2023, Zhao Yun Shan
 *	All rights reserved.
 *
 */
#pragma once
#include <memory>
#include <coev/coev.h>

namespace cosys
{
	using namespace coev;
	awaitable<void> sleep_for(long);
	awaitable<void> usleep_for(long);

}