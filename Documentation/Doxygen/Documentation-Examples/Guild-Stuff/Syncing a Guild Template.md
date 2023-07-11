Syncing a Guild Template {#syncingaguildtemplate}
============
- Execute the, `Guilds::syncGuildTemplateAsync()` function, while passing in a value of type `SyncGuildTemplateData`, with a return value of type `auto` or `GuildTemplateData`.
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
				GetGuildTemplatesData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();

				auto responseVector = Guilds::getGuildTemplatesAsync(dataPackage).get();

				SyncGuildTemplateData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.templateCode = responseVector[0].code;

				auto responseData = Guilds::syncGuildTemplateAsync(dataPackage01).get();

				std::cout << "THE NAME: " << responseData.name << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
