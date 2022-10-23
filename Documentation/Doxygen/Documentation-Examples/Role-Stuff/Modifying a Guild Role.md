Modifying a Guild Role {#modifyingaguildrole}
=============
- Execute the, `DiscordCoreAPI::Roles::modifyGuildRoleAsync()` function, while passing in a value of type `DiscordCoreAPI::ModifyGuildRoleData`, with a return value of type `auto` or `DiscordCoreAPI::Role`.
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

		unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::ModifyGuildRoleData dataPackage01;
				dataPackage01.name = "TEST ROLE";
				dataPackage01.roleId = "886366417316896799";
				dataPackage01.reason = "TESTING!";
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.hexColorValue = "Fefe12";
				dataPackage01.mentionable = false;
				dataPackage01.guildId = args.eventData.getGuildId();

				auto newRole = DiscordCoreAPI::Roles::modifyGuildRoleAsync(dataPackage01).get();

				cout << "ROLE NAME: " << newRole.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}


```
