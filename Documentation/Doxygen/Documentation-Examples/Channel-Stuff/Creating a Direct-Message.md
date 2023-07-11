Creating a Direct-Message {#creatingadm}
============
- Execute the, `Channels::createDMChannelAsync()` function, while passing in a data structure of type `CreateDMChannelData`, with a return value of type `auto` or `Channel`.
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
				CreateDMChannelData dataPackage;
				dataPackage.userId = args.eventData.getAuthorId();

				Channel dmChannel = Channels::createDMChannelAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}
```
