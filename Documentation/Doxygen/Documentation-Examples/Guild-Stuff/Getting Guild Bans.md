Getting Guild Bans {#gettingguildbans}
============
- Execute the, `DiscordCoreAPI::Guilds::getGuildBansAsync()` function, while passing in a value of type `DiscordCoreAPI::GetGuildBansData`, with a return value of type `auto` or `vector<DiscordCoreAPI::BanData>`.
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
				DiscordCoreAPI::GetGuildBansData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();

				auto guildBans = DiscordCoreAPI::Guilds::getGuildBansAsync(dataPackage01).get();

				for (auto value: guildBans) {
					cout << "THE BAN NAME: " << value.user.username << endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
