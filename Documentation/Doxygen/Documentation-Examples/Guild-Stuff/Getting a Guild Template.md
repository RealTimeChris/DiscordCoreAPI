Getting a Guild Template {#gettingaguildtemplate}
============
- Execute the, `Guilds::getGuildTemplateAsync()` function, while passing in a value of type `GetGuildTemplateData`, with a return value of type `auto` or `GuildTemplateData`.
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
				GetGuildTemplateData dataPackage01;
				dataPackage01.templateCode = "YmRr2bj5NyYX";

				auto responseData = Guilds::getGuildTemplateAsync(dataPackage01).get();

				std::cout << "USER NAME: " << responseData.creator.userName << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
