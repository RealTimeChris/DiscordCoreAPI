### **Adding a Command:**
---
- In a new header for the command (that is stored within the Commands folder) #include the `Index.hpp` file.
- Add this header to the `Commands/CommandsList.hpp` header.
```cpp
// CommandsList.hpp - Commands! All of them!
// https://github.com/RealTimeChris

#pragma once

#include "Test.hpp"

```
- Create a new class, within the `DiscordCoreAPI` namespace, derived from the `BaseFunction` class.
- Set the `commandName` and `helpDescription` members of the class.
- Add a `create()` function, where the return value is a `unique_ptr` to the class type of the current command.
- Add a `virtual CoRoutine<void> execute()` function with an argument of type `DiscordCoreAPI::BaseFunctionArguments`.
- CONTINUED FURTHER DOWN.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "__**Test:**__ Enter !test or /test to run this command!";
		}

		unique_ptr<BaseFunction> create() {
			return make_unique<Test>();
		}

		virtual CoRoutine<void> execute(BaseFunctionArguments args) {
			try {
			if(args->argumentsArray[0] == "test"){
			
				DiscordCoreAPI::GetAuditLogData dataPackage;
				dataPackage.actionType = DiscordCoreAPI::AuditLogEvent::ROLE_UPDATE;
				dataPackage.guildId = args->eventData.getGuildId();
				dataPackage.limit = 25;
				dataPackage.userId = args->eventData.getAuthorId();
				AuditLogData auditLogData = Guilds::getAuditLogDataAsync(dataPackage).get();

				for (auto value : auditLogData.auditLogEntries) {
					for (auto value2 : value.changes) {
						cout << value2.newValueString << endl;
						cout << value2.oldValueString << endl;
					}
				}
				auto messages = Messages::fetchMessagesAsync({ .channelId = args->eventData.getChannelId(), .limit = 100, .beforeThisId = args->eventData.getMessageId() }).get();
				vector<string> messageIds;
				for (auto value : messages) {
					messageIds.push_back(value.data.id);
				}
				DeleteMessagesBulkData dataPackage2;
				dataPackage2.channelId = true;
				dataPackage2.messageIds = messageIds;
				Messages::deleteMessasgeBulkAsync(dataPackage2).get();
			}
				co_return;
			}
			catch (exception error) {
				cout << "Test::execute() Error: " << error.what() << endl << endl;
			}

		}
	};
}
#endif
```
- Add the command to the library's command list by using the `DiscordCoreAPI::CommandCenter::registerFunction()` function.

```cpp
// Main.cpp - Main source file, for "the framework".
// https://github.com/RealTimeChris

#include "Index.hpp"

int main()
{
    init_apartment();
    string botToken = "ODYwMTA1MDY3MzYwMjg4ODA5.YN2ZRA.U8G-Y78hLhFzBfL-VH8v0-zHhzI";
    DiscordCoreAPI::DiscordCoreClient::finalSetup(botToken);
    DiscordCoreAPI::CommandController::registerFunction(vector<string>{"test"}, move(make_unique<DiscordCoreAPI::Test>()));
    DiscordCoreAPI::DiscordCoreClient::runBot();
}
```
- NOTE: By default, the format for triggering a command is `!COMMANDNAME = ARGUMENT1, ARGUMENT2, ARGUMENT3... ARGUMENT-N`. This prefix can be changed by changing the prefix value provided by your database. You can also modify this fundamental behavior by modifying the code within the Commands.hpp header file.
- Accessing command arguments is done by accessing the `argumentsArray` member of the `BaseFunctionArguments` structure that is passed through to the command – as can be seen in the first example block of code here.
- Now, when someone enters `!commandName` or `!commandName = ARGUMENT1, ARGUMENT2... ARGUMENT-N`, this will invoke the execution of this command and pass in the arguments! So, as for the example in the code here, this means that typing in `!test = test` into a Discord server that is shared with the bot will result in the audit log data being printed out into the console – as well as some messages being bulk-deleted.
