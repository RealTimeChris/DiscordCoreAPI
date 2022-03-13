### **Getting Voice Regions:**
---
- Access within the `DiscordCoreAPI` namespace, the `Channels` class.
- Select, from the `Channels` class, the `getVoiceRegionsAsync()` function and execute it, with a return value of type `auto` or `vector<VoiceRegionData>`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		unique_ptr<BaseFunction> create() {
			return make_unique<Test>();
		}

		virtual CoRoutine<void> executeAsync(BaseFunctionArguments args) {
			try {

				auto responseData = Channels::getVoiceRegionsAsync().get();

				for (auto& value : responseData) {
					cout << "THE REGION: " << value.name << endl;
				}
				
				
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}

		virtual ~Test() = default;
	};
}
```
