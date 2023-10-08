Getting Current User's Guilds {#gettingcurrentusersguilds}
============
- Execute the, `Guilds::getCurrentUserGuildsAsync()` function, while passing in a value of type `GetCurrentUserGuildsData`, with a return value of type `auto` or `vector<Guild>`.
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
				GetCurrentUserGuildsData dataPackage;
				dataPackage.limit = 2;

				auto currentOutput = Guilds::getCurrentUserGuildsAsync(dataPackage).get();

				for (auto& value: currentOutput) {
					std::cout << "GUILD NAME: " << value.name << std::endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() = default;
	};
}


```
