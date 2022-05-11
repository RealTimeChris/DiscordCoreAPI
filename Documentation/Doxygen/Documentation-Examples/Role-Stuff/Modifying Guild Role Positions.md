Modifying Guild Role Positions {#modifyingguildrolepositions}
============
- Execute the, `DiscordCoreAPI::Roles::modifyGuildRolePositionsAsync()` function, while passing in a value of type `DiscordCoreAPI::ModifyGuildRolePositionsData`, with a return value of type `auto` or `vector<DiscordCoreAPI::Role>`.
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
				DiscordCoreAPI::ModifyGuildRolePositionsData dataPackage01;
				dataPackage01.newPosition = 12;
				dataPackage01.roleId = "886366417316896799";
				dataPackage01.reason = "TESTING!";
				dataPackage01.guildId = args.eventData.getGuildId();

				auto newRoles = DiscordCoreAPI::Roles::modifyGuildRolePositionsAsync(dataPackage01).get();

				for (auto value: newRoles) {
					cout << "ROLE NAME: " << value.name << endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}


```
