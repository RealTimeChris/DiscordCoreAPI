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

namespace DiscordCoreAPI {

	UnorderedMap<jsonifier::vector<jsonifier::string>, UniquePtr<BaseFunction>> functions{};

	void CommandController::registerFunction(const jsonifier::vector<jsonifier::string>& functionNames, UniquePtr<BaseFunction> baseFunction) {
		functions[functionNames] = std::move(baseFunction);
	}

	UnorderedMap<jsonifier::vector<jsonifier::string>, UniquePtr<BaseFunction>>& CommandController::getFunctions() {
		return functions;
	};

	CoRoutine<void> CommandController::checkForAndRunCommand(CommandData&& commandData) {
		BaseFunctionArguments theArgsNew{ commandData };
		co_await NewThreadAwaitable<void>();
		UniquePtr<BaseFunction> functionPointer{ getCommand(theArgsNew.getCommandName()) };
		if (!functionPointer.get()) {
			co_return;
		}

		functionPointer->execute(theArgsNew);
		co_return;
	}

	UniquePtr<BaseFunction> CommandController::getCommand(jsonifier::string_view commandName) {
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
			UniquePtr<BaseFunction> newValue = createFunction(functionName);
			return newValue;
		}
		return nullptr;
	}

	UniquePtr<BaseFunction> CommandController::createFunction(jsonifier::string_view functionName) {
		for (auto& [key01, value01]: functions) {
			for (auto& value02: key01) {
				if (functionName == value02) {
					return value01->create();
				}
			}
		}
		return nullptr;
	}

}
