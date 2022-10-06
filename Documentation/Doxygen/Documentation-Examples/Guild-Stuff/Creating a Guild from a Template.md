Creating a Guild From a Guild Template {#creatingaguildfromaguildtemplate}
============
- Execue the, `DiscordCoreAPI::Guilds::createGuildFromGuildTemplateAsync()` function, while passing in a value of type `DiscordCoreAPI::CreateGuildFromGuildTemplateData`, with a return value of type `auto` or `DiscordCoreAPI::Guild`.
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
				DiscordCoreAPI::CreateGuildFromGuildTemplateData dataPackage01;
				dataPackage01.templateCode = "YmRr2bj5NyYX";
				dataPackage01.name = "TEST GUILD!";

				auto responseData = DiscordCoreAPI::Guilds::createGuildFromGuildTemplateAsync(dataPackage01).get();

				cout << "Guild Name: " << responseData.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
