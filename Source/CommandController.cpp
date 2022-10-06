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
/// CommandController.cpp - Source file for the command controller.
/// May 20, 2021
/// https://discordcoreapi.com
/// \file CommandController.cpp

#include <discordcoreapi/CommandController.hpp>

namespace DiscordCoreAPI {

	namespace Globals {
		Map<Vector<String>, std::unique_ptr<BaseFunction>> functions{};
	}

	CommandController::CommandController(DiscordCoreClient* discordCoreClientNew) {
		this->discordCoreClient = discordCoreClientNew;
	}

	Void CommandController::registerFunction(const Vector<String>& functionNames, std::unique_ptr<BaseFunction> baseFunction) {
		Globals::functions[functionNames] = std::move(baseFunction);
	}

	Map<Vector<String>, std::unique_ptr<BaseFunction>>& CommandController::getFunctions() {
		return Globals::functions;
	};

	CoRoutine<Void> CommandController::checkForAndRunCommand(CommandData commandData) {
		try {
			co_await NewThreadAwaitable<Void>();
			std::unique_ptr<BaseFunction> functionPointer{ this->getCommand(convertToLowerCase(commandData.commandName)) };
			if (!functionPointer.get()) {
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

	std::unique_ptr<BaseFunction> CommandController::getCommand(const String& commandName) {
		String functionName{};
		Bool isItFound{ false };
		if (commandName.size() > 0) {
			for (auto const& [keyFirst, value]: Globals::functions) {
				for (auto& key: keyFirst) {
					if (key.find(convertToLowerCase(commandName)) != String::npos) {
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

	std::unique_ptr<BaseFunction> CommandController::createFunction(const String& functionName) {
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