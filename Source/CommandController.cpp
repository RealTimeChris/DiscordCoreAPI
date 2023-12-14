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
/// CommandController.cpp - Source file for the command controller.
/// May 20, 2021
/// https://discordcoreapi.com
/// \file CommandController.cpp

#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DiscordCoreClient.hpp>

namespace discord_core_api {

	unordered_map<jsonifier::vector<jsonifier::string>, unique_ptr<base_function>> functions{};

	void command_controller::registerFunction(const jsonifier::vector<jsonifier::string>& functionNames, unique_ptr<base_function> baseFunction) {
		functions[functionNames] = std::move(baseFunction);
	}

	unordered_map<jsonifier::vector<jsonifier::string>, unique_ptr<base_function>>& command_controller::getFunctions() {
		return functions;
	};

	co_routine<void> command_controller::checkForAndRunCommand(command_data&& commandData) {
		unique_ptr<base_function_arguments> theArgsNew{ makeUnique<base_function_arguments>(commandData) };
		co_await newThreadAwaitable<void>();
		unique_ptr<base_function> functionPointer{ getCommand(theArgsNew->getCommandName()) };
		if (!functionPointer.get()) {
			co_return;
		}

		functionPointer->execute(*theArgsNew);
		co_return;
	}

	unique_ptr<base_function> command_controller::getCommand(jsonifier::string_view commandName) {
		jsonifier::string functionName{};
		bool isItFound{};
		if (commandName.size() > 0) {
			for (auto const& [keyFirst, value]: functions) {
				for (auto& key: keyFirst) {
					if (key.find(convertToLowerCase(commandName)) != jsonifier::string::npos) {
						isItFound	 = true;
						functionName = convertToLowerCase(commandName.substr(0, key.size()));
						break;
					}
				}
			}
		}
		if (isItFound) {
			return createFunction(functionName);
		}
		return unique_ptr<base_function>{};
	}

	unique_ptr<base_function> command_controller::createFunction(jsonifier::string_view functionName) {
		for (auto& [key01, value01]: functions) {
			for (auto& value02: key01) {
				if (functionName == value02) {
					return value01->create();
				}
			}
		}
		return unique_ptr<base_function>{};
	}

}
