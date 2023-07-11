Creating a Stage Instance {#creatingastageinstance}
=============
- Execute the, `StageInstances::createStageInstanceAsync()` function, while passing in a value of type `CreateStageInstanceData`, with a return value of type `auto` or `StageInstance`.
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
				CreateStageInstanceData dataPackage;
				dataPackage.privacyLevel = StageInstancePrivacyLevel::GUILD_ONLY;
				dataPackage.channelId = "914726178022101052";
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.topic = "TESTING!";

				auto responseData = StageInstances::createStageInstanceAsync(dataPackage).get();

				std::cout << "THE NAME: " << responseData.topic << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
