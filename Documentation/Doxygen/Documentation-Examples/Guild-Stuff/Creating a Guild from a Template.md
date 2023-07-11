Creating a Guild From a Guild Template {#creatingaguildfromaguildtemplate}
============
- Execue the, `Guilds::createGuildFromGuildTemplateAsync()` function, while passing in a value of type `CreateGuildFromGuildTemplateData`, with a return value of type `auto` or `Guild`.
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
				CreateGuildFromGuildTemplateData dataPackage01;
				dataPackage01.templateCode = "YmRr2bj5NyYX";
				dataPackage01.name = "TEST GUILD!";

				auto responseData = Guilds::createGuildFromGuildTemplateAsync(dataPackage01).get();

				std::cout << "Guild Name: " << responseData.name << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
