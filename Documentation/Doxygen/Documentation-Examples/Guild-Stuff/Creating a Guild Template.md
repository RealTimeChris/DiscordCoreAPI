Creating a Guild Template {#creatingaguildtemplate}
=============
- Execute the, `DiscordCoreAPI::Guilds::createGuildTemplateAsync()` function, while passing in a value of type `DiscordCoreAPI::CreateGuildTemplateData`, with a return value of type `auto` or `DiscordCoreAPI::GuildTemplateData`.
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
				DiscordCoreAPI::CreateGuildTemplateData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.description = "TESTING TEMPLATE!";
				dataPackage01.name = "TEST TEMPLATE!";

				auto responseData = DiscordCoreAPI::Guilds::createGuildTemplateAsync(dataPackage01).get();

				cout << "THE NAME: " << responseData.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
