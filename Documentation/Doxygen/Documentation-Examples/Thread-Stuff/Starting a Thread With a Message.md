Starting a Thread with a Message {#startingathreadwithamessage}
============
- Execute the, `Threads::startThreadWithMessageAsync()` function, while passing in a value of type `StartThreadWithMessageData`, with a return value of type `auto` or `Channel`.
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
				GuildMember guildMember = GuildMembers::getCachedGuildMember(
															  {.guildMemberId = args.eventData.getAuthorId(), .guildId = args.eventData.getGuildId()})
															  .get();

				StartThreadWithMessageData dataPackage;
				dataPackage.messageId = args.eventData.getMessageId();
				dataPackage.reason = "TESTING REASONS!";
				dataPackage.autoArchiveDuration = ThreadAutoArchiveDuration::SHORTEST;
				dataPackage.channelId = args.eventData.getChannelId();

				Channel newThread = Threads::startThreadWithMessageAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
