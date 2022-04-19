/*
*
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
/// Chris M.
/// https://discordcoreapi.com
/// \file CommandController.cpp

#include <discordcoreapi/CommandController.hpp>
#include <discordcoreapi/DataParsingFunctions.hpp>

namespace DiscordCoreAPI {

	namespace Globals {
		std::map<std::vector<std::string>, std::unique_ptr<BaseFunction>> functions{};
	}

	CommandController::CommandController(DiscordCoreClient* discordCoreClientNew) {
		this->discordCoreClient = discordCoreClientNew;
	}

	void CommandController::registerFunction(std::vector<std::string> functionNames, std::unique_ptr<BaseFunction> baseFunction) {
		try {
			Globals::functions.insert(std::make_pair(functionNames, std::move(baseFunction)));
		} catch (...) {
			reportException("CommandController::registerFunction()");
		}
	}

	std::map<std::vector<std::string>, std::unique_ptr<BaseFunction>>& CommandController::getFunctions() {
		return Globals::functions;
	};

	void CommandController::checkForAndRunCommand(std::unique_ptr<CommandData> commandData) {
		try {
			std::unique_ptr<BaseFunction> functionPointer{ nullptr };
			functionPointer = this->getCommand(convertToLowerCase(commandData->commandName));
			if (functionPointer == nullptr) {
				return;
			}

			functionPointer->args = BaseFunctionArguments{ *commandData, this->discordCoreClient };
			functionPointer->execute(functionPointer->args);
			return;
		} catch (...) {
			reportException("CommandController::checkForAndRunCommand()");
		}
	}

	std::unique_ptr<BaseFunction> CommandController::getCommand(std::string commandName) {
		try {
			std::string functionName{ "" };
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
				std::unique_ptr<BaseFunction> newValue = createFunction(functionName);
				return newValue;
			}
		} catch (...) {
			reportException("CommandController::getCommand()");
		}
		return nullptr;
	}

	std::unique_ptr<BaseFunction> CommandController::createFunction(std::string functionName) {
		try {
			for (auto& [key01, value01]: Globals::functions) {
				for (auto& value02: key01) {
					if (functionName == value02) {
						return std::move(Globals::functions[key01])->create();
					}
				}
			}
		} catch (...) {
			reportException("CommandController::createFunction()");
		}
		return std::unique_ptr<BaseFunction>{ nullptr };
	}

}