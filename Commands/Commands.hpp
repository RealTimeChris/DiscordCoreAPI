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

		virtual task<void> execute(shared_ptr<BaseFunctionArguments> args) = 0;
		virtual BaseFunction* create() = 0;
		string helpDescription{ "" };
		virtual ~BaseFunction() {};
		string commandName{ "" };
		EmbedData helpEmbed{};

	};

	class CommandCenter {
	public:
		static map<string, BaseFunction*> functions;

		static void registerFunction(vector<string> functionNames, BaseFunction* baseFunction) {
			for (auto value : functionNames) {
				CommandCenter::functions.insert(make_pair(convertToLowerCase(value), baseFunction));
			}
		}

		static void checkForAndRunCommand(CommandData commandData) {
			shared_ptr<BaseFunction> functionPointer{ nullptr };
			bool messageOption = false;
			if (commandData.eventData.eventType == InputEventType::REGULAR_MESSAGE) {
				string commandName = parseCommandName(convertToLowerCase(commandData.eventData.getMessageData().content), commandData);
				functionPointer = CommandCenter::getCommand(commandName, commandData);
				messageOption = true;
			}
			else if (commandData.eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
				DiscordCoreInternal::DataParser::parseObject(commandData.eventData.getInteractionData().rawData, &commandData);
				string newCommandName = getBotDiscordUserData().data.prefix + commandData.commandName;
				functionPointer = CommandCenter::getCommand(convertToLowerCase(newCommandName), commandData);
				messageOption = false;
			}
			else {
				string newCommandName = getBotDiscordUserData().data.prefix + commandData.commandName;
				functionPointer = CommandCenter::getCommand(newCommandName, commandData);
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
			try {
				functionPointer->execute(make_shared<BaseFunctionArguments>(args)).get();
			}
			catch (...) {
				rethrowException("CommandCenter::execute() Error: ");
			}
			
			functionPointer.~shared_ptr();
			functionPointer = nullptr;
		};

	protected:

		static shared_ptr<BaseFunction> createFunction(string functionName) {
			return shared_ptr<BaseFunction>(CommandCenter::functions.at(functionName)->create());
		}

		static shared_ptr<BaseFunction> getCommand(string commandName, CommandData commandData) {
			try {
				BaseFunction* lowestValue{ nullptr };
				string functionName{ "" };
				bool isItFound{ false };
				if (commandName.size() > 0) {
					for (auto const& [key, value] : DiscordCoreAPI::CommandCenter::functions) {

						if (commandName[0] == getBotDiscordUserData().data.prefix[0]) {
							if (key.find(convertToLowerCase(commandName.substr(1, commandName.size() - 1))) != string::npos) {
								isItFound = true;
								lowestValue = value;
								functionName = convertToLowerCase(commandName.substr(1, key.length()));
							}

						}
					}
				}
				if (isItFound) {
					shared_ptr<BaseFunction> newValue = createFunction(functionName);
					return newValue;
				}
			}
			catch (exception& e) {
				cout << "CommandCenter::getCommand() Error: " << e.what() << endl << endl;
			}
			return nullptr;
		}

		static string parseCommandName(string messageContents, CommandData commandData) {
			if (messageContents.size() > 0) {
				if (messageContents[0] == getBotDiscordUserData().data.prefix[0]) {
					for (auto [key, value] : CommandCenter::functions) {
						if (messageContents.find(key) != string::npos) {
							if (messageContents.find_first_of(" ") == string::npos) {
								if (messageContents.substr(1, messageContents.find_first_of('\u0000') - 1) == key) {
									string commandName = getBotDiscordUserData().data.prefix[0] + messageContents.substr(1, messageContents.find_first_of('\u0000') - 1);
									return commandName;
								}
							}
							else {
								if (messageContents.substr(1, messageContents.find_first_of(' ') - 1) == key) {
									string commandName = getBotDiscordUserData().data.prefix[0] + messageContents.substr(1, messageContents.find_first_of(' ') - 1);
									return commandName;
								}
							}

						}
					}
				}
			}
			return string();
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
				cout << "CommandCenter::parseArguments() Error: " << e.what() << endl << endl;
				return args;
			}
		}

	};

	map<string, BaseFunction*> CommandCenter::functions;
};
#endif
