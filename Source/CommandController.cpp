/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// CommandController.cpp - Source file for the command controller.
/// May 20, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <CommandController.hpp>
#include <DataParsingFunctions.hpp>

namespace DiscordCoreAPI {

	CommandController::CommandController(std::string commandPrefix, DiscordCoreClient* discordCoreClientNew) {
		this->commandPrefix = commandPrefix;
		this->discordCoreClient = discordCoreClientNew;
	}

	void CommandController::registerFunction(std::vector<std::string> functionNames, std::unique_ptr<BaseFunction> baseFunction) {
		try {
			this->functions.insert(std::make_pair(functionNames, std::move(baseFunction)));
		}
		catch (...) {
			reportException("CommandController::registerFunction()");
		}
	}

	CoRoutine<void> CommandController::checkForAndRunCommand(std::unique_ptr<CommandData> commandData) {
		try {
			co_await NewThreadAwaitable<void>();
			std::unique_ptr<BaseFunction> functionPointer{ nullptr };
			bool messageOption = false;
			if (commandData->eventData.eventType == InputEventType::Regular_Message) {
				std::string commandName = parseCommandName(convertToLowerCase(commandData->eventData.getMessageData().content));
				functionPointer = this->getCommand(commandName);
				messageOption = true;
			}
			else if (commandData->eventData.eventType == InputEventType::Application_Command_Interaction) {
				auto jsonValue = commandData->eventData.getInteractionData().rawData;
				DiscordCoreInternal::DataParser::parseObject(jsonValue, *commandData.get());
				std::string newCommandName = this->commandPrefix + commandData->commandName;
				functionPointer = this->getCommand(convertToLowerCase(newCommandName));
				messageOption = false;
			}
			else {
				std::string newCommandName = this->commandPrefix + commandData->commandName;
				functionPointer = this->getCommand(newCommandName);
			}
			if (functionPointer == nullptr) {
				co_return;
			}

			std::unique_ptr<BaseFunctionArguments> args{ std::make_unique<BaseFunctionArguments>(commandData->eventData,this->discordCoreClient) };

			if (messageOption == true) {
				args->argumentsArray = this->parseArguments(commandData->eventData.getMessageData().content);
			}
			else if (messageOption == false) {
				args->argumentsArray = commandData->optionsArgs;
			}
			functionPointer->execute(std::move(args));
			co_return;
		}
		catch (...) {
			reportException("CommandController::checkForAndRunCommand()");
		}
	}

	std::unique_ptr<BaseFunction> CommandController::getCommand(std::string commandName) {
		try {
			std::string functionName{ "" };
			bool isItFound{ false };
			if (commandName.size() > 0) {
				for (auto const& [keyFirst, value] : this->functions) {
					if (commandName[0] == this->commandPrefix[0]) {
						for (auto &key : keyFirst) {
							if (key.find(convertToLowerCase(commandName.substr(1, commandName.size() - 1))) != std::string::npos) {
								isItFound = true;
								functionName = convertToLowerCase(commandName.substr(1, key.length()));
							}
						}
					}
				}
			}
			if (isItFound) {
				std::unique_ptr<BaseFunction> newValue = createFunction(functionName);
				return newValue;
			}
		}
		catch (...) {
			reportException("CommandController::getCommand()");
		}
		return nullptr;
	}

	std::string CommandController::parseCommandName(std::string messageContents) {
		try {
			if (messageContents.size() > 0) {
				if (messageContents[0] == this->commandPrefix[0]) {
					for (auto& [keyFirst, value] : this->functions) {
						for (std::string key : keyFirst) {
							if (messageContents.find(key) != std::string::npos) {
								if (messageContents.find_first_of(" ") == std::string::npos) {
									if (messageContents.substr(1, messageContents.find_first_of('\u0000') - 1) == key) {
										std::string commandName = this->commandPrefix[0] + messageContents.substr(1, messageContents.find_first_of('\u0000') - 1);
										return commandName;
									}
								}
								else {
									if (messageContents.substr(1, messageContents.find_first_of(' ') - 1) == key) {
										std::string commandName = this->commandPrefix[0] + messageContents.substr(1, messageContents.find_first_of(' ') - 1);
										return commandName;
									}
								}
							}
						}
					}
				}
			}
			return std::string();
		}
		catch (...) {
			reportException("CommandController::parseCommandName()");
		}
		return std::string();
	}

	std::unique_ptr<BaseFunction> CommandController::createFunction(std::string functionName) {
		try {
			for (auto& [key01, value01] : this->functions) {
				for (auto& value02 : key01) {
					if (functionName == value02) {
						return std::move(this->functions.at(key01))->create();
					}
				}
			}
		}
		catch (...) {
			reportException("CommandController::createFunction()");
		}
		return std::unique_ptr<BaseFunction>{nullptr};
	}

	std::vector<std::string> CommandController::parseArguments(std::string messageContents) {
		std::vector<std::string> args;
		try {
			uint64_t startingPosition = messageContents.find("=");
			if (startingPosition == std::string::npos) {
				return args;
			}
			std::string newString = messageContents.substr(startingPosition + 1);
			if (newString.find(",") == std::string::npos) {
				uint64_t startingPositionNew = newString.find_first_not_of(" ");
				if (startingPositionNew == std::string::npos) {
					return args;
				}
				newString = newString.substr(startingPositionNew);
				uint64_t endingPosition = newString.find(",");
				std::string argument;
				if (endingPosition == std::string::npos) {
					argument = newString.substr(0);
					args.push_back(argument);
					return args;
				}
				argument = newString.substr(0, endingPosition);
				newString = newString.substr(endingPosition + 1);
				args.push_back(argument);
				return args;
			}
			while (newString.find(",") != std::string::npos) {
				uint64_t startingPositionNew = newString.find_first_not_of(" ");
				if (startingPositionNew == std::string::npos) {
					return args;
				}
				newString = newString.substr(startingPositionNew);
				uint64_t endingPosition = newString.find(",");
				std::string argument;
				if (endingPosition == std::string::npos) {
					argument = newString.substr(0);
					args.push_back(argument);
					return args;
				}
				argument = newString.substr(0, endingPosition);
				newString = newString.substr(endingPosition + 1);
				args.push_back(argument);
				if (newString.find(",") == std::string::npos) {
					startingPositionNew = newString.find_first_not_of(" ");
					if (startingPositionNew == std::string::npos) {
						return args;
					}
					newString = newString.substr(startingPositionNew);
					endingPosition = newString.find(",");
					if (endingPosition == std::string::npos) {
						argument = newString.substr(0);
						args.push_back(argument);
						return args;
					}
					argument = newString.substr(0, endingPosition);
					args.push_back(argument);
				}
			}
			return args;
		}
		catch (...) {
			reportException("CommandController::parseArguments()");
			return args;
		}
	}

	CommandController::~CommandController() {
		std::vector<std::vector<std::string>> functionKeys{};
		for (auto& [key, value] : this->functions) {
			std::vector<std::string> newVector{};
			for (auto& value : key) {
				newVector.push_back(value);
			}
			functionKeys.push_back(newVector);
		}
		for (auto& value : functionKeys) {
			this->functions.erase(value);
		}
	}
}