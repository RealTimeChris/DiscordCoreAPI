/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// DllMain.cpp : Defines the entry point for the DLL application.
/// Feb 18, 2022
/// https://discordcoreapi.com
/// \file DllMain.cpp

#include <discordcoreapi/Index.hpp>

#ifdef _WIN32
bool __stdcall DllMain(void*, unsigned long fdwReason, void*) {
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
			return TRUE;
		}
	}
}
#endif