Getting A list of Guild Emoji {#gettingalistofguildemoji}
============
- Execute the, `DiscordCoreAPI::Reactions::getEmojiListAsync()` function, while passing in a value of type `DiscordCoreAPI::GetEmojiListData`, with a return value of type `auto` or `vector<DiscordCoreAPI::EmojiData>`.
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

		virtual Void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::GetEmojiListData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();

				auto emojiList = DiscordCoreAPI::Reactions::getEmojiListAsync(dataPackage).get();

				for (auto value: emojiList) {
					cout << "THE NAME: " << value.name << endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
