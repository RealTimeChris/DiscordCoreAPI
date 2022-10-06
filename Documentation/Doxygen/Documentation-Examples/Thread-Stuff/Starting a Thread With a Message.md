Starting a Thread with a Message {#startingathreadwithamessage}
============
- Execute the, `DiscordCoreAPI::Threads::startThreadWithMessageAsync()` function, while passing in a value of type `DiscordCoreAPI::StartThreadWithMessageData`, with a return value of type `auto` or `DiscordCoreAPI::Channel`.
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
				DiscordCoreAPI::GuildMember guildMember = DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync(
															  {.guildMemberId = args.eventData.getAuthorId(), .guildId = args.eventData.getGuildId()})
															  .get();

				DiscordCoreAPI::StartThreadWithMessageData dataPackage;
				dataPackage.messageId = args.eventData.getMessageId();
				dataPackage.reason = "TESTING REASONS!";
				dataPackage.autoArchiveDuration = ThreadAutoArchiveDuration::SHORTEST;
				dataPackage.channelId = args.eventData.getChannelId();

				DiscordCoreAPI::Channel newThread = DiscordCoreAPI::Threads::startThreadWithMessageAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
