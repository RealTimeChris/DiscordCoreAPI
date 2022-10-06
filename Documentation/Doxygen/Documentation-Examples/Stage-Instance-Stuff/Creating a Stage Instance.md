Creating a Stage Instance {#creatingastageinstance}
=============
- Execute the, `DiscordCoreAPI::StageInstances::createStageInstanceAsync()` function, while passing in a value of type `DiscordCoreAPI::CreateStageInstanceData`, with a return value of type `auto` or `DiscordCoreAPI::StageInstance`.
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
				DiscordCoreAPI::CreateStageInstanceData dataPackage;
				dataPackage.privacyLevel = StageInstancePrivacyLevel::GUILD_ONLY;
				dataPackage.channelId = "914726178022101052";
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.topic = "TESTING!";

				auto responseData = DiscordCoreAPI::StageInstances::createStageInstanceAsync(dataPackage).get();

				cout << "THE NAME: " << responseData.topic << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
