Modifying a Stage Instance {#modifyingastageinstance}
============
- Execute the, `StageInstances::modifyStageInstanceAsync()` function, while passing in a value of type `ModifyStageInstanceData`, with a return value of type `auto` or `StageInstance`.
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
				ModifyStageInstanceData dataPackage;
				dataPackage.channelId = "914726178022101052";
				dataPackage.privacyLevel = StageInstancePrivacyLevel::GUILD_ONLY;
				dataPackage.reason = "TESTING!";
				dataPackage.topic = "NEW TOPIC!";

				auto responseData = StageInstances::modifyStageInstanceAsync(dataPackage).get();

				std::cout << "THE TOPIC: " << responseData.topic << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
