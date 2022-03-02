Getting a Guild Template {#gettingaguildtemplate}
============
- Execute the, `DiscordCoreAPI::Guilds::getGuildTemplateAsync()` function, while passing in a value of type `DiscordCoreAPI::GetGuildTemplateData`, with a return value of type `auto` or `DiscordCoreAPI::GuildTemplateData`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return make_unique<Test>();
		}

		virtual void executeAsync(std::unique_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				DiscordCoreAPI::GetGuildTemplateData dataPackage01{};
				dataPackage01.templateCode = "YmRr2bj5NyYX";

				auto responseData = DiscordCoreAPI::Guilds::getGuildTemplateAsync(dataPackage01).get();

				cout << "USER NAME: " << responseData.creator.userName << endl;

				
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
```
