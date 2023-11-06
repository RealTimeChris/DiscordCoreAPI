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
/// CommandController.hpp - wrangler header for the command controller.
/// May 20, 2021
/// https://discordcoreapi.com
/// \file CommandController.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace discord_core_api {

	/// @brief A class for handling commands from user input.
	class DiscordCoreAPI_Dll command_controller {
	  public:
		/// @brief Registers a function to be called.
		/// @param functionNames a vector of strings to be accepted as function names.
		/// @param baseFunction a unique_ptr to the function to be called.
		void registerFunction(const jsonifier::vector<jsonifier::string>& functionNames, unique_ptr<base_function> baseFunction);

		/// @brief For returning the contained map of functions.
		/// @return a map containing the function names as well as unique_ptrs to the functions.
		unordered_map<jsonifier::vector<jsonifier::string>, unique_ptr<base_function>>& getFunctions();

		co_routine<void> checkForAndRunCommand(command_data&& commandData);

	  protected:
		unique_ptr<base_function> createFunction(jsonifier::string_view functionName);

		unique_ptr<base_function> getCommand(jsonifier::string_view commandName);
	};

};
