Syncing a Guild Template {#syncingaguildtemplate}
============
- Execute the, `DiscordCoreAPI::Guilds::syncGuildTemplateAsync()` function, while passing in a value of type `DiscordCoreAPI::SyncGuildTemplateData`, with a return value of type `auto` or `DiscordCoreAPI::GuildTemplateData`.
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
				DiscordCoreAPI::GetGuildTemplatesData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();

				auto responseVector = DiscordCoreAPI::Guilds::getGuildTemplatesAsync(dataPackage).get();

				DiscordCoreAPI::SyncGuildTemplateData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.templateCode = responseVector[0].code;

				auto responseData = DiscordCoreAPI::Guilds::syncGuildTemplateAsync(dataPackage01).get();

				cout << "THE NAME: " << responseData.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
