Getting a User {#gettingauser}
============
- Execute the, `DiscordCoreAPI::Users::getCachedUserAsync()` (which collects it from the cache), or `DiscordCoreAPI::Users::getUserAsync()` (which collects it from the Discord servers) function, while passing in a value of type `DiscordCoreAPI::GetUserData`, with a return value of `auto` or `DiscordCoreAPI::User`.
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
			DiscordCoreAPI::EmbedData msgEmbed { };
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
				DiscordCoreAPI::GetUserData dataPackage;
				dataPackage.userId = args.eventData.getAuthorId();

				auto currentUser = DiscordCoreAPI::Users::getUserAsync(dataPackage).get();

				cout << currentUser.userName << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}


```
