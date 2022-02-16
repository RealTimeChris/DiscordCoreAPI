### **Getting Audit Logs:**
---
- Access within the `DiscordCoreAPI` namespace, the `Guilds` class.
- Populate a data structure of type `DiscordCoreAPI::GetAuditLogData`.
- Select, from the `Guilds` class, the `getAuditLogDataAsync()` function and execute it, with a return value of type `auto` or `AuditLogData` while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include <../DiscordCoreClient02.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Test* create() {
			return new Test;
		}

		virtual  task<void> execute(shared_ptr<BaseFunctionArguments> args) {

			GetAuditLogData dataPackage;
			dataPackage.actionType = AuditLogEvent::MEMBER_BAN_ADD;
			dataPackage.guildId = args->eventData.getGuildId();
			dataPackage.limit = 25;
			dataPackage.userId = args->eventData.getAuthorId();
			
			AuditLogData auditLogData = args->eventData.discordCoreClient->guilds->getAuditLogDataAsync(dataPackage).get();

			co_return;
		}
	};
}
#endif
```
