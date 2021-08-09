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
		BaseFunctionArguments() {}
		BaseFunctionArguments(InputEventData inputEventData) {
			this->eventData = inputEventData;
		}
		InputEventData eventData;
		vector<string> argumentsArray;
	};

	class BaseFunction {
	public:
		virtual ~BaseFunction() {}
		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) = 0;
		virtual BaseFunction* create() = 0;
		string commandName = "";
		string helpDescription = "";
	};

	class CommandCenter {
	public:
		static map<string, BaseFunction*> functions;

		static void registerFunction(string functionName, BaseFunction* baseFunction) {
			CommandCenter::functions.insert(make_pair(functionName, baseFunction));
		}

		static void checkForAndRunCommand(CommandData commandData) {
			shared_ptr<BaseFunction> functionPointer{ nullptr };
			bool messageOption = false;
			if (commandData.eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				functionPointer = CommandCenter::getCommand(convertToLowerCase(commandData.eventData.getMessageData().content), commandData);
				messageOption = true;
			}
			else if (commandData.eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				DiscordCoreInternal::parseObject(commandData.eventData.getInteractionData().dataRaw, &commandData);
				string newCommandName = commandData.eventData.discordCoreClient->discordUser->data.prefix + commandData.commandName;
				functionPointer = CommandCenter::getCommand(convertToLowerCase(newCommandName), commandData);
				messageOption = false;
			}

			if (functionPointer == nullptr) {
				return;
			}

			BaseFunctionArguments args(commandData.eventData);

			if (messageOption == true) {
				args.argumentsArray = CommandCenter::parseArguments(commandData.eventData.getMessageData().content);
			}
			else if (messageOption == false) {
				args.argumentsArray = commandData.optionsArgs;
			}
			functionPointer->execute(make_shared<BaseFunctionArguments>(args)).get();
			functionPointer.~shared_ptr();
			functionPointer = nullptr;
		};

	protected:

		static shared_ptr<BaseFunction> createFunction(string functionName) {
			return shared_ptr<BaseFunction>(CommandCenter::functions.at(functionName)->create());
		}

		static shared_ptr<BaseFunction> getCommand(string messageContents, CommandData commandData) {
			try {
				size_t currentPosition = INFINITE;
				BaseFunction* lowestValue{ nullptr };
				string functionName;
				bool isItFound = false;
				for (auto const& [key, value] : DiscordCoreAPI::CommandCenter::functions) {
					if (messageContents[0] == commandData.eventData.discordCoreClient->discordUser->data.prefix[0]) {
						if (messageContents.find(key) != string::npos && messageContents.find(key) < currentPosition) {
							isItFound = true;
							currentPosition = messageContents.find(key);
							lowestValue = value;
							functionName = messageContents.substr(currentPosition, key.length());
						}
					}
				}
				if (isItFound) {
					shared_ptr<BaseFunction> newValue = createFunction(functionName);
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

	map<string, BaseFunction*> CommandCenter::functions;
};
#endif
