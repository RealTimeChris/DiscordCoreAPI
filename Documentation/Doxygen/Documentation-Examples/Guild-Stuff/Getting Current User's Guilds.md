Getting Current User's Guilds {#gettingcurrentusersguilds}
============
- Execute the, `DiscordCoreAPI::Guilds::getCurrentUserGuildsAsync()` function, while passing in a value of type `DiscordCoreAPI::GetCurrentUserGuildsData`, with a return value of type `auto` or `vector<DiscordCoreAPI::Guild>`.
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
				DiscordCoreAPI::GetCurrentUserGuildsData dataPackage;
				dataPackage.limit = 2;

				auto currentOutput = DiscordCoreAPI::Guilds::getCurrentUserGuildsAsync(dataPackage).get();

				for (auto& value: currentOutput) {
					cout << "GUILD NAME: " << value.name << endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}


```
