Removing a Role from a Guild Member {#removingarolefromaguildmember}
============
- Execute the, `DiscordCoreAPI::Roles::removeGuildMemberRoleAsync()` function, while passing in a value of type `DiscordCoreAPI::RemoveGuildMemberRoleData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::RemoveGuildMemberRoleData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.userId = args.eventData.getAuthorId();
				dataPackage01.reason = "TESTING PURPOSES!";
				dataPackage01.roleId = "866124519303020554";

				DiscordCoreAPI::Roles::removeGuildMemberRoleAsync(dataPackage01).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
