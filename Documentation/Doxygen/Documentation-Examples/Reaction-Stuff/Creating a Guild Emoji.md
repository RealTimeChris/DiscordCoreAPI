Creating a Guild Emoji {#creatingaguildemoji}
============
- Execute the, `DiscordCoreAPI::Reactions::createGuildEmojiAsync()` function, while passing in a value of type `DiscordCoreAPI::CreateGuildEmojiData`, with a return value of type `auto` or `DiscordCoreAPI::EmojiData`.
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

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::CreateGuildEmojiData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.name = "TESTEMOJI";
				dataPackage.type = ImageType::JPG;
				vector<unsigned __int8> testImageData { };
				dataPackage.imageData = testImageData;
				auto emoji = DiscordCoreAPI::Reactions::createGuildEmojiAsync(dataPackage).get();

				cout << "THE NAME: " << emoji.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
