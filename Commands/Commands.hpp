// Commands.hpp - Wrangler header for the commands.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _COMMANDS_
#define _COMMANDS_

#include "../pch.h"
#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class DiscordCoreClient;

	struct BaseFunctionArguments {
		BaseFunctionArguments(InputEventData inputEventData) {
			this->eventData = inputEventData;
		}
		InputEventData eventData;
		vector<string> argumentsArray;
	};

	class BaseFunction {
	public:
		BaseFunction() {}
		BaseFunction(BaseFunction* thisPtr) {
			*this = *thisPtr;
		}
		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) = 0;
		string commandName;
		string helpDescription;
		virtual ~BaseFunction() {};
	};

	class CommandController {
	public:

		static map<string, shared_ptr<BaseFunction>> commands;

		static void addCommand(shared_ptr<BaseFunction> newFunction) {
			DiscordCoreAPI::CommandController::commands.insert(make_pair(newFunction->commandName, newFunction));
		}

		static void checkForAndRunCommand(CommandData commandData) {
			shared_ptr<BaseFunction*> functionPointer{ nullptr };
			bool messageOption = false;
			if (commandData.eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				functionPointer = CommandController::getCommand(convertToLowerCase(commandData.eventData.getMessageData().content), commandData);
				messageOption = true;
			}
			else if (commandData.eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				DiscordCoreInternal::CommandData commandDataNew;
				DiscordCoreInternal::parseObject(commandData.eventData.interactionData.dataRaw, &commandDataNew);
				string newCommandName = commandData.eventData.discordCoreClient->discordUser->data.prefix + commandDataNew.commandName;
				functionPointer = CommandController::getCommand(convertToLowerCase(newCommandName), commandData);
				messageOption = false;
			}

			if (functionPointer == nullptr) {
				return;
			}

			BaseFunctionArguments args(commandData.eventData);

			if (messageOption == true) {
				args.argumentsArray = CommandController::parseArguments(commandData.eventData.getMessageData().content);
			}
			else if (messageOption == false) {
				DiscordCoreInternal::CommandData commandDataNew;
				DiscordCoreInternal::parseObject(commandData.eventData.getInteractionData().dataRaw, &commandDataNew);
				args.argumentsArray = commandDataNew.optionsArgs;
			}
			(**functionPointer).execute(make_shared<BaseFunctionArguments>(args)).get();
			functionPointer.~shared_ptr();
		};

	protected:

		template<typename T>
		static shared_ptr<T*>getCommandToWrap(shared_ptr<T> function) {
			shared_ptr<T*> functionPtr;
			if (function.get() == nullptr) {
				functionPtr = make_shared<T*>();
			}
			else {
				functionPtr = make_shared<T*>(function.get());
			}
			return functionPtr;
		}

		static shared_ptr<BaseFunction*> getCommand(string messageContents, CommandData commandData) {
			try {
				size_t currentPosition = INFINITE;
				shared_ptr<BaseFunction> lowestValue{ nullptr };
				bool isItFound = false;
				for (auto const [key, value] : DiscordCoreAPI::CommandController::commands) {
					if (messageContents[0] == commandData.eventData.discordCoreClient->discordUser->data.prefix[0]) {
						if (messageContents.find(key) != string::npos && messageContents.find(key) < currentPosition) {
							isItFound = true;
							currentPosition = messageContents.find(key);
							lowestValue = value;
						}
					}
				}
				if (isItFound) {
					shared_ptr<BaseFunction*>newValue = getCommandToWrap(lowestValue);
					return newValue;
				}				
			}
			catch (exception& e) {
				cout << "CommandController::getCommand() Error: " << e.what() << endl << endl;
			}
			return nullptr;
		}

		static vector<string> parseArguments(string messageContents) {
			vector<string> args;
			try {
				size_t startingPosition = messageContents.find("=");
				if (startingPosition == string::npos) {
					return args;
				}
				string newString = messageContents.substr(startingPosition + 1);
				if (newString.find(",") == string::npos) {
					size_t startingPositionNew = newString.find_first_not_of(" ");
					if (startingPositionNew == string::npos) {
						return args;
					}
					newString = newString.substr(startingPositionNew);
					size_t endingPosition = newString.find(",");
					string argument;
					if (endingPosition == string::npos) {
						argument = newString.substr(0);
						args.push_back(argument);
						return args;
					}
					argument = newString.substr(0, endingPosition);
					newString = newString.substr(endingPosition + 1);
					args.push_back(argument);
					return args;
				}
				while (newString.find(",") != string::npos) {
					size_t startingPositionNew = newString.find_first_not_of(" ");
					if (startingPositionNew == string::npos) {
						return args;
					}
					newString = newString.substr(startingPositionNew);
					size_t endingPosition = newString.find(",");
					string argument;
					if (endingPosition == string::npos) {
						argument = newString.substr(0);
						args.push_back(argument);
						return args;
					}
					argument = newString.substr(0, endingPosition);
					newString = newString.substr(endingPosition + 1);
					args.push_back(argument);
					if (newString.find(",") == string::npos) {
						startingPositionNew = newString.find_first_not_of(" ");
						if (startingPositionNew == string::npos) {
							return args;
						}
						newString = newString.substr(startingPositionNew);
						endingPosition = newString.find(",");
						if (endingPosition == string::npos) {
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
			catch (exception& e) {
				cout << "CommandController::parseArguments() Error: " << e.what() << endl << endl;
				return args;
			}
		}
	};
	map<string, shared_ptr<BaseFunction>> CommandController::commands;
};
#endif
