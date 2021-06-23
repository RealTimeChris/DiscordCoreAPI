### **Adding a Command:**
---
- In a new header for the command (that is stored within the Commands folder) #include the `pch.h`,`../DiscordCoreClient.hpp`, and `Commands.hpp` files.
- Add this header to the `Commands/CommandsList.hpp` header.
```cpp
// CommandsList.hpp - Commands! All of them!
// https://github.com/RealTimeChris

#pragma once

#ifndef _COMMANDS_LIST_
#define _COMMANDS_LIST_

#include "Commands.hpp"
#include "Test.hpp"

#endif
```
- Create a new class, within the `DiscordCoreAPI` namespace, derived from the `BaseFunction` class.
- Set the `commandName` and `helpDescription` members of the class.
- Add a `virtual task<void> execute()` function with an argument of type `DiscordCoreAPI::BaseFunctionArguments*`.
- Create an instance of this new class within the `DiscordCoreAPI` namespace.
- CONTINUED FURTHER DOWN.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "../DiscordCoreClient.hpp"

namespace DiscordCoreAPI {

	class Test : public  BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "__**Test:**__ Enter !test or /test to run this command!";
		}

		virtual  task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			try {

				EmbedData msgEmbed;
				msgEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
				msgEmbed.setColor("000000");
				msgEmbed.setDescription(args->argumentsArray.at(0));
				msgEmbed.setTimeStamp(getTimeAndDate());
				msgEmbed.setTitle("__**Welcome:**__");
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					InputEventResponseData responseData(InputEventResponseType::REGULAR_MESSAGE_RESPONSE);
					responseData.channelId = args->eventData.messageData.channelId;
					responseData.messageId = args->eventData.messageData.id;
					responseData.embeds.push_back(msgEmbed);
					InputEventHandler::respondToEvent(responseData).get();
				}
				DiscordCoreAPI::GetAuditLogData dataPackage;
				dataPackage.actionType = DiscordCoreAPI::AuditLogEvent::ROLE_UPDATE;
				dataPackage.guildId = args->eventData.getGuildId();
				dataPackage.limit = 25;
				dataPackage.userId = args->eventData.getAuthorId();
				AuditLogData auditLogData = args->eventData.discordCoreClient->guilds->getAuditLogDataAsync(dataPackage).get();

				for (auto value : auditLogData.auditLogEntries) {
					for (auto value2 : value.changes) {
						cout << value2.newValueString << endl;
						cout << value2.oldValueString << endl;
					}
				}
				DeleteMessagesBulkData dataPackage2;
				dataPackage2.deletePinned = true;
				dataPackage2.limit = 25;
				dataPackage2.beforeThisId = args->eventData.getMessageId();
				args->eventData.discordCoreClient->messages->deleteMessasgeBulkAsync(dataPackage2).get();

				co_return;
			}
			catch (exception error) {
				cout << "Help::execute() Error: " << error.what() << endl << endl;
			}

		}
	};
	Test test{};
}
#endif
```
- Add the command to the library's command list by using the `DiscordCoreAPI::CommandController::addCommand()` function.

```cpp
// Main.cpp - Main source file, for "the framework".
// https://github.com/RealTimeChris

#include "pch.h"
#include "./DiscordCoreAPI/Index.hpp"

int main() {
	winrt::init_apartment();
	string botToken = "YOUR_BOT_TOKEN_HERE";
	shared_ptr<DiscordCoreAPI::DiscordCoreClient> pDiscordCoreClient = DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken);
   	DiscordCoreAPI::CommandController::addCommand(&DiscordCoreAPI::test, DiscordCoreAPI::test.commandName);
    	agent::wait((agent*)pDiscordCoreClient.get());
    	exception error;
    	while (pDiscordCoreClient.get()->getError(error)) {
	        cout << "DiscordCoreClient() Error: " << error.what() << endl << endl;
    	}
    	return 0;
}
```
- NOTE: By default, the format for triggering a command is `!COMMANDNAME = ARGUMENT1, ARGUMENT2, ARGUMENT3... ARGUMENT-N`. This prefix can be changed by changing the prefix value provided by your database. You can also modify this fundamental behavior by modifying the code within the Commands.hpp header file.
- Accessing command arguments is done by accessing the `argumentsArray` member of the `BaseFunctionArguments` structure that is passed through to the command – as can be seen in the first example block of code here.
- Now, when someone enters `!commandName` or `!commandName = ARGUMENT1, ARGUMENT2... ARGUMENT-N`, this will invoke the execution of this command and pass in the arguments! So, as for the example in the code here, this means that typing in `!test = test` into a Discord server that is shared with the bot will result in the audit log data being printed out into the console – as well as some messages being bulk-deleted.
