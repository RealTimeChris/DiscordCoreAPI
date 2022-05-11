Getting Audit Logs {#gettingauditlogs}
============
- Execute the, `DiscordCoreAPI::Guilds::getGuildAuditLogsAsync()` function, while passing in a data structure of type `DiscordCoreAPI::GetGuildAuditLogsData` with a return value of type `auto` or `DiscordCoreAPI::AuditLogData`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "Index.hpp"

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			DiscordCoreAPI::GetGuildAuditLogsData dataPackage;
			dataPackage.actionType = AuditLogEvent::MEMBER_BAN_ADD;
			dataPackage.guildId = args.eventData.getGuildId();
			dataPackage.limit = 25;
			dataPackage.userId = args.eventData.getAuthorId();

			DiscordCoreAPI::AuditLogData auditLogData = DiscordCoreAPI::Guilds::getGuildAuditLogsAsync(dataPackage).get(;
		}
	};
}
```
