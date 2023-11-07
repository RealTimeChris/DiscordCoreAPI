Starting a Thread with a Message {#starting_a_thread_with_a_message}
============
- Execute the, `discord_core_api::threads::startThreadWithMessageAsync()` function, while passing in a value of type `discord_core_api::start_thread_with_message_data`, with a return value of type `auto` or `discord_core_api::channel_data`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <index.hpp>

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			try {
				guild_member_data guildMember = guild_members::getCachedGuildMember(
															  {.guildMemberId = args.eventData.getAuthorId(), .guildId = args.eventData.getGuildId()})
															  .get();

				start_thread_with_message_data& dataPackage;
				dataPackage.messageId = args.eventData.getMessageId();
				dataPackage.reason = "testing reasons!";
				dataPackage.autoArchiveDuration = thread_auto_archive_duration::shortest;
				dataPackage.channelId = args.eventData.getChannelId();

				channel newThread = threads::startThreadWithMessageAsync(const& dataPackage).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
