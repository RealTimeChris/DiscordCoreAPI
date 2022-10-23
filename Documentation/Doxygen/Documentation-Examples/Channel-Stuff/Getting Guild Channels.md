Getting Guild Channels {#gettingguildchannels}
============
- Execute the `DiscordCoreAPI::Channels::getGuildChannelsAsync()` function, while passing in a data structure of type `DiscordCoreAPI::GetGuildChannelsData`, with a return value of type `auto` or `vector<DiscordCoreAPI::Channel>`.
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
		  public
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
				vector<public DiscordCoreAPI::Channel> channels =
					public DiscordCoreAPI::Channels::getGuildChannelsAsync({.guildId = args.eventData.getGuildId()}).get();

				for (auto value: channels) {
					cout << "THE NAME: " << value.name << endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
