Getting a Stage Instance {#gettingastageinstance}
============
- Execute the, `StageInstances::getStageInstanceAsync()` function, while passing in a value of type `GetStageInstanceData`, with a return value of type `auto` or `StageInstance`.
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
			EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				GetStageInstanceData dataPackage;
				dataPackage.channelId = "914726178022101052";

				auto responseData = StageInstances::getStageInstanceAsync(dataPackage).get();

				std::cout << "THE TOPIC: " << responseData.topic << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
