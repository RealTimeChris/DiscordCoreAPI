/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
/// CommandController.cpp - Source file for the command controller.
/// May 20, 2021
/// https://discordcoreapi.com
/// \file CommandController.cpp

#include <discordcoreapi/CommandController.hpp>

namespace DiscordCoreAPI {

	namespace Globals {
		std::map<std::vector<std::string>, UniquePtr<BaseFunction>> functions{};
	}

	CommandController::CommandController(DiscordCoreClient* discordCoreClientNew) {
		discordCoreClient = discordCoreClientNew;
	}

	void CommandController::registerFunction(const std::vector<std::string>& functionNames, UniquePtr<BaseFunction> baseFunction) {
		Globals::functions[functionNames] = std::move(baseFunction);
	}

	std::map<std::vector<std::string>, UniquePtr<BaseFunction>>& CommandController::getFunctions() {
		return Globals::functions;
	};

	CoRoutine<void> CommandController::checkForAndRunCommand(CommandData commandData) {
		co_await NewThreadAwaitable<void>();
		BaseFunctionArguments theArgsNew{ commandData, discordCoreClient };
		UniquePtr<BaseFunction> functionPointer{ getCommand(convertToLowerCase(commandData.getCommandName())) };
		if (!functionPointer.get()) {
			co_return;
		}

		functionPointer->execute(theArgsNew);
		co_return;
	}

	UniquePtr<BaseFunction> CommandController::getCommand(const std::string& commandName) {
		std::string functionName{};
		bool isItFound{};
		if (commandName.size() > 0) {
			for (auto const& [keyFirst, value]: Globals::functions) {
				for (auto& key: keyFirst) {
					if (key.find(convertToLowerCase(commandName)) != std::string::npos) {
						isItFound = true;
						functionName = convertToLowerCase(commandName.substr(0, key.length()));
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

	UniquePtr<BaseFunction> CommandController::createFunction(const std::string& functionName) {
		for (auto& [key01, value01]: Globals::functions) {
			for (auto& value02: key01) {
				if (functionName == value02) {
					return value01->create();
				}
			}
		}
		return nullptr;
	}

}