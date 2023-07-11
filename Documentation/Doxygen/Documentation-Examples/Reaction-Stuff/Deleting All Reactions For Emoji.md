Deleting All Reactions for Emoji {#deletingallreactionsforemoji}
============
- Execute the, `Reactions::deleteReactionsByEmojiAsync()` function, while passing in a data structure of type `DeleteReactionsByEmojiData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for its return value now.

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
				DeleteReactionsByEmojiData dataPackage;
				dataPackage.messageId = args.eventData.getMessageId();
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.emojiName = "💯";

				Reactions::deleteReactionsByEmojiAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
