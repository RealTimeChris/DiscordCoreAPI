/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.
*/
/// CommandController.cpp - Source file for the command controller.
/// May 20, 2021
/// https://discordcoreapi.com
/// \file CommandController.cpp

#include <discordcoreapi/CommandController.hpp>

namespace DiscordCoreAPI {

	namespace Globals {
		std::map<std::vector<std::string>, std::unique_ptr<BaseFunction>> functions{};
	}

	CommandController::CommandController(DiscordCoreClient* discordCoreClientNew) {
		this->discordCoreClient = discordCoreClientNew;
	}

	void CommandController::registerFunction(const std::vector<std::string>& functionNames, std::unique_ptr<BaseFunction> baseFunction) {
		Globals::functions[functionNames] = std::move(baseFunction);
	}

	std::map<std::vector<std::string>, std::unique_ptr<BaseFunction>>& CommandController::getFunctions() {
		return Globals::functions;
	};

	CoRoutine<void> CommandController::checkForAndRunCommand(CommandData commandData) {
		co_await NewThreadAwaitable<void>();
		try {
			std::unique_ptr<BaseFunction> functionPointer{ this->getCommand(convertToLowerCase(commandData.commandName)) };
			if (functionPointer == nullptr) {
				co_return;
			}
			BaseFunctionArguments theArgs{ commandData, this->discordCoreClient };
			functionPointer->execute(theArgs);
			co_return;
		} catch (...) {
			reportException("CommandController::checkForAndRunCommand()");
		}
		co_return;
	}

	std::unique_ptr<BaseFunction> CommandController::getCommand(const std::string& commandName) {
		std::string functionName{};
		bool isItFound{ false };
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
			std::unique_ptr<BaseFunction> newValue = this->createFunction(functionName);
			return newValue;
		}
		return nullptr;
	}

	std::unique_ptr<BaseFunction> CommandController::createFunction(const std::string& functionName) {
		for (auto& [key01, value01]: Globals::functions) {
			for (auto& value02: key01) {
				if (functionName == value02) {
					return Globals::functions[key01]->create();
				}
			}
		}
		return nullptr;
	}

}