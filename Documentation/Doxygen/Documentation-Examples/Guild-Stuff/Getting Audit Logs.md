Getting Audit Logs {#gettingauditlogs}
============
- Execute the, `Guilds::getGuildAuditLogsAsync()` function, while passing in a data structure of type `GetGuildAuditLogsData` with a return value of type `auto` or `AuditLogData`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			GetGuildAuditLogsData dataPackage;
			dataPackage.actionType = AuditLogEvent::MEMBER_BAN_ADD;
			dataPackage.guildId = args.eventData.getGuildId();
			dataPackage.limit = 25;
			dataPackage.userId = args.eventData.getAuthorId();

			AuditLogData auditLogData = Guilds::getGuildAuditLogsAsync(dataPackage).get(;
		}
	};
}
```
