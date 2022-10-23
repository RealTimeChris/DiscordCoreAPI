Deleting a WebHook Message {#deletingawebhookmessage}
=============
- Execute the, `DiscordCoreAPI::WebHooks::deleteWebHookMessageAsync()` function, while passing in a value of type `DiscordCoreAPI::DeleteWebHookMessageData`, with a return value of type `void`.
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

				DiscordCoreAPI::ExecuteWebHookData dataPackage01 {newWebHooks[0]};
				dataPackage01.addContent("TEST CONTENT");
				dataPackage01.wait = true;

				auto newMessage = DiscordCoreAPI::WebHooks::executeWebHookAsync(dataPackage01).get();

				DiscordCoreAPI::DeleteWebHookMessageData dataPackage03;
				dataPackage03.webHookId = newWebHooks[0].id;
				dataPackage03.webhookToken = newWebHooks[0].token;
				dataPackage03.messageId = newMessage.id;

				DiscordCoreAPI::WebHooks::deleteWebHookMessageAsync(dataPackage03).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}
```
