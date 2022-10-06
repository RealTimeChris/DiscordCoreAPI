Getting a Sticker {#gettingasticker}
============
- Execute the, `DiscordCoreAPI::Stickers::getStickerAsync()` function, while passing in a value of type `DiscordCoreAPI::GetStickerData`, with a return value of type `auto` or `DiscordCoreAPI::Sticker`.
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

		virtual Void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::GetStickerData dataPackage;
				dataPackage.stickerId = "YOUR_STICKER_ID_HERE";

				auto responseData = DiscordCoreAPI::Stickers::getStickerAsync(dataPackage).get();

				cout << "THE NAME: " << responseData.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}
```
