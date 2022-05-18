Searching Guild Members {#searchingguildmembers}
============
- Execute the, `DiscordCoreAPI::GuildMembers::searchGuildMembersAsync()` function, while passing in a value of type `DiscordCoreAPI::SearchGuildMembersData`, with a return value of type `auto` or `vector<DiscordCoreAPI::GuildMember>`.
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
			DiscordCoreAPI::EmbedData msgEmbed;
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
				DiscordCoreAPI::SearchGuildMembersData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.limit = 20;
				dataPackage.query = "Real";

				auto guildMembers = DiscordCoreAPI::GuildMembers::searchGuildMembersAsync(dataPackage).get();

				for (auto value: guildMembers) {
					cout << "THE NAME: " << value.user.username << endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
