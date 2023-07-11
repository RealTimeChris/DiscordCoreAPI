Deleting a Guild Emoji {#deletingaguildemoji}
============
- Execute the, `Reactions::deleteGuildEmojiAsync()` function, while passing in a value of type `DeleteGuildEmojiData`, with a return value of type `void`.
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
				GetEmojiListData dataPackage00 { };
				dataPackage00.guildId = args.eventData.getGuildId();

				auto emojiList = Reactions::getEmojiListAsync(dataPackage00).get();

				DeleteGuildEmojiData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.emojiId = emojiList[0].id;
				dataPackage.reason = "REASON FOR DELETING!";
				Reactions::deleteGuildEmojiAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
