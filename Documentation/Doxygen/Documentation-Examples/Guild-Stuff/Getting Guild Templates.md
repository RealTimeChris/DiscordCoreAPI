Getting Guild Templates {#gettingguildtemplates}
==============
- Execute the, `Guilds::getGuildTemplatesAsync()` function, while passing in a value of type `GetGuildTemplatesData`, with a return value of type `auto` or `vector<GuildTemplateData>`.
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
				GetGuildTemplatesData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();

				auto responseData = Guilds::getGuildTemplatesAsync(dataPackage01).get();

				for (auto& value: responseData) {
					std::cout << "THE NAME: " << value.name << std::endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
