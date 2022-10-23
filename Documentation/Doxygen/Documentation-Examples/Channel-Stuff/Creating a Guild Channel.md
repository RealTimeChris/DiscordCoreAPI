Creating a Guild Channel {#creatingguildchannel}
============
- Execute the, `DiscordCoreAPI::Channels::createGuildChannelAsync()` function, while passing in a data structure of type `DiscordCoreAPI::CreateGuildChannelData`, with a return value of type `auto` or `DiscordCoreAPI::Channel`.
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
				DiscordCoreAPI::CreateGuildChannelData dataPackage;
				dataPackage.type = DiscordCoreAPI::ChannelType::Guild_Text;
				dataPackage.name = "TEST CHANNEL";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.reason = "TESTING PURPOSES!";

				vector<DiscordCoreAPI::Channel> channels = DiscordCoreAPI::Channels::getGuildChannelsAsync({.guildId = args.eventData.getGuildId()}).get();

				for (auto value: channels) {
					if (value.type == ChannelType::GUILD_CATEGORY) {
						dataPackage.parentId = value.id;
						break;
					}
				}

				DiscordCoreAPI::Channel channel = DiscordCoreAPI::Channels::createGuildChannelAsync(dataPackage).get();

				cout << "THE NAME: " << channel.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
