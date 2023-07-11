Creating a Guild Template {#creatingaguildtemplate}
=============
- Execute the, `Guilds::createGuildTemplateAsync()` function, while passing in a value of type `CreateGuildTemplateData`, with a return value of type `auto` or `GuildTemplateData`.
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
				CreateGuildTemplateData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.description = "TESTING TEMPLATE!";
				dataPackage01.name = "TEST TEMPLATE!";

				auto responseData = Guilds::createGuildTemplateAsync(dataPackage01).get();

				std::cout << "THE NAME: " << responseData.name << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
