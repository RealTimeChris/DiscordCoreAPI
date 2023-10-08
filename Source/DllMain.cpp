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
/// DllMain.cpp : defines the entry point for the dll application.
/// Feb 18, 2022
/// https://discordcoreapi.com
/// \file DllMain.cpp

#include <discordcoreapi/Index.hpp>

#if defined _WIN32
bool __stdcall dll_main(void*, uint64_t fdwReason, void*) {
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH: {
			[[fallthrough]];
		}
		case DLL_THREAD_ATTACH: {
			[[fallthrough]];
		}
		case DLL_THREAD_DETACH: {
			[[fallthrough]];
		}
		case DLL_PROCESS_DETACH: {
			[[fallthrough]];
		}
		default: {
			return true;
		}
	}
}
#endif
