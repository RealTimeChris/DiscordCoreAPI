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
/// CommandController.hpp - Wrangler header for the command controller.
/// May 20, 2021
/// https://discordcoreapi.com
/// \file CommandController.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace DiscordCoreAPI {

	/// @brief A class for handling commands from user input.
	class DiscordCoreAPI_Dll CommandController {
	  public:
		CommandController(DiscordCoreClient*);

		/// @brief Registers a function to be called.
		/// @param functionNames A vector of strings to be accepted as function names.
		/// @param baseFunction A unique_ptr to the function to be called.
		void registerFunction(const Jsonifier::Vector<std::string>& functionNames, UniquePtr<BaseFunction> baseFunction);

		/// @brief For returning the contained map of functions.
		/// @return A map containing the function names as well as unique_ptrs to the functions.
		UnorderedMap<Jsonifier::Vector<std::string>, UniquePtr<BaseFunction>>& getFunctions();

		CoRoutine<void> checkForAndRunCommand(CommandData commandData);

	  protected:
		DiscordCoreClient* discordCoreClient{};

		UniquePtr<BaseFunction> createFunction(const std::string& functionName);

		UniquePtr<BaseFunction> getCommand(const std::string& commandName);
	};

};
