Creating a Guild Emoji {#creatingaguildemoji}
============
- Execute the, `Reactions::createGuildEmojiAsync()` function, while passing in a value of type `CreateGuildEmojiData`, with a return value of type `auto` or `EmojiData`.
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

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				CreateGuildEmojiData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.name = "TESTEMOJI";
				dataPackage.type = ImageType::JPG;
				vector<unsigned __int8> testImageData { };
				dataPackage.imageData = testImageData;
				auto emoji = Reactions::createGuildEmojiAsync(dataPackage).get();

				std::cout << "THE NAME: " << emoji.name << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
