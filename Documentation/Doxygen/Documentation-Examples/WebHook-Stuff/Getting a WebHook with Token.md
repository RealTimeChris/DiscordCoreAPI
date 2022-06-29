Getting a WebHook with Token {#gettingawebhookwithtoken}
============
- Execute the, `DiscordCoreAPI::WebHooks::getWebHookWithTokenAsync()` function, while passing in a value of type `DiscordCoreAPI::GetWebHookWithTokenData`, with a return value of type `auto` or `DiscordCoreAPI::WebHook`.
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
				DiscordCoreAPI::GetGuildWebHooksData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();

				auto newWebHooks = DiscordCoreAPI::WebHooks::getGuildWebHooksAsync(dataPackage).get();

				DiscordCoreAPI::GetWebHookWithTokenData dataPackage01;
				dataPackage01.webHookId = newWebHooks[0].id;
				dataPackage01.webhookToken = newWebHooks[0].token;

				auto newWebHook = DiscordCoreAPI::WebHooks::getWebHookWithTokenAsync(dataPackage01).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() = default;
	};
}
```
