Getting Audit Logs {#getting_audit_logs}
============
- Execute the, `discord_core_api::guilds::getGuildAuditLogsAsync()` function, while passing in a data structure of type `discord_core_api::get_guild_audit_logs_data` with a return value of type `auto` or `discord_core_api::audit_log_data`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "index.hpp"

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**test Usage__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			get_guild_audit_logs_data& dataPackage;
			dataPackage.actionType = audit_log_event::MEMBER_BAN_ADD;
			dataPackage.guildId = args.eventData.getGuildId();
			dataPackage.limit = 25;
			dataPackage.userId = args.eventData.getAuthorId();

			audit_log_data auditLogData = guilds::getGuildAuditLogsAsync(dataPackage).get(;
		}
	};
}
```
