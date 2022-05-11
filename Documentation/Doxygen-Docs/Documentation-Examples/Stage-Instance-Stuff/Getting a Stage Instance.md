Getting a Stage Instance {#gettingastageinstance}
============
- Execute the, `DiscordCoreAPI::StageInstances::getStageInstanceAsync()` function, while passing in a value of type `DiscordCoreAPI::GetStageInstanceData`, with a return value of type `auto` or `DiscordCoreAPI::StageInstance`.
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

		unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::GetStageInstanceData dataPackage;
				dataPackage.channelId = "914726178022101052";

				auto responseData = DiscordCoreAPI::StageInstances::getStageInstanceAsync(dataPackage).get();

				cout << "THE TOPIC: " << responseData.topic << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
