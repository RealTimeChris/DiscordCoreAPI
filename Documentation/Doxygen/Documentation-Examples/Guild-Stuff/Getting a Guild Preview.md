Getting a Guild Preview {#gettingaguildpreview}
============
- Execute the, `DiscordCoreAPI::Guilds::getGuildPreviewAsync()` function, while passing in a value of type `DiscordCoreAPI::GetGuildPreviewData`, with a return value of type `auto` or `DiscordCoreAPI::GuildPreviewData`.
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

		virtual Void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::GetGuildPreviewData dataPackage00;
				dataPackage00.guildId = args.eventData.getGuildId();

				auto guild = DiscordCoreAPI::Guilds::getGuildPreviewAsync(dataPackage00).get();

				cout << "THE NAME: " << guild.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
