Editing a WebHook Message {#editingawebhookmessage}
============
- Execute the, `DiscordCoreAPI::WebHooks::editWebHookMessageAsync()` function, while passing in a value of type `DiscordCoreAPI::EditWebHookMessageData`, with a return value of type `auto` or `DiscordCoreAPI::Message`.
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

				DiscordCoreAPI::GetWebHookMessageData dataPackage02;
				dataPackage02.webHookId = newWebHooks[0].id;
				dataPackage02.webhookToken = newWebHooks[0].token;
				dataPackage02.messageId = newMessage.id;

				auto newMessage02 = DiscordCoreAPI::WebHooks::getWebHookMessageAsync(dataPackage02).get();

				cout << "MESSAGE ID: " << newMessage02.id << endl;

				DiscordCoreAPI::EditWebHookMessageData dataPackage03 {newWebHooks[0]};
				dataPackage03.addContent("NEWER CONTENTS!");
				dataPackage03.messageId = newMessage02.id;

				auto newMessage03 = DiscordCoreAPI::WebHooks::editWebHookMessageAsync(dataPackage03).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}
```
