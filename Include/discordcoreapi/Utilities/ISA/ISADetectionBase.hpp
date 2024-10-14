/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// Isadetection.hpp - Header for the detection of the AVX instruction set.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file ISADetection.hpp
#pragma once

#include <discordcoreapi/Utilities/Config.hpp>

#if !defined(DCA_CHECK_FOR_INSTRUCTION)
	#define DCA_CHECK_FOR_INSTRUCTION(x) (DCA_CPU_INSTRUCTIONS & x)
#endif

#if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
	#define DCA_NEON 1 << 4
#else
	#define DCA_NEON 0
#endif
#if defined(__AVX__)
	#define DCA_AVX 1 << 5
#else
	#define DCA_AVX 0
#endif
#if defined(__AVX2__)
	#define DCA_AVX2 1 << 6
#else
	#define DCA_AVX2 0
#endif
#if defined(__AVX512__)
	#define DCA_AVX512 1 << 7
#else
	#define DCA_AVX512 0
#endif

#if !defined(DCA_CPU_INSTRUCTIONS)
	#define DCA_CPU_INSTRUCTIONS (DCA_NEON | DCA_AVX | DCA_AVX2 | DCA_AVX512)
#endif

