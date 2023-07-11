Creating a Guild Channel {#creatingguildchannel}
============
- Execute the, `Channels::createGuildChannelAsync()` function, while passing in a data structure of type `CreateGuildChannelData`, with a return value of type `auto` or `Channel`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				CreateGuildChannelData dataPackage;
				dataPackage.type = ChannelType::Guild_Text;
				dataPackage.name = "TEST CHANNEL";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.reason = "TESTING PURPOSES!";

				vector<Channel> channels = Channels::getGuildChannelsAsync({.guildId = args.eventData.getGuildId()}).get();

				for (auto value: channels) {
					if (value.type == ChannelType::GUILD_CATEGORY) {
						dataPackage.parentId = value.id;
						break;
					}
				}

				Channel channel = Channels::createGuildChannelAsync(dataPackage).get();

				std::cout << "THE NAME: " << channel.name << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
