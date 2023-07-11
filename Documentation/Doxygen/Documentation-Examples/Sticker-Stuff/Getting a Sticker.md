Getting a Sticker {#gettingasticker}
============
- Execute the, `Stickers::getStickerAsync()` function, while passing in a value of type `GetStickerData`, with a return value of type `auto` or `Sticker`.
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
				GetStickerData dataPackage;
				dataPackage.stickerId = "YOUR_STICKER_ID_HERE";

				auto responseData = Stickers::getStickerAsync(dataPackage).get();

				std::cout << "THE NAME: " << responseData.name << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}
```
