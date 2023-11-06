Starting a Thread without a Message {#startingathreadwithoutamessage}
============
- Execute the, from the `threads::startThreadWithoutMessageAsync()` function, while passing in a value of type `start_thread_without_message_data`, with a return value of type `auto` or `thread`.
- call the function with `.get()` added to the end in order to wait for the results now.

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
				start_thread_without_message_data dataPackage;
				dataPackage.invitable = true;
				dataPackage.type = thread_type::GUILD_PUBLIC_THREAD;
				dataPackage.reason = "testing reasons!";
				dataPackage.autoArchiveDuration = thread_auto_archive_duration::short;
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.threadName = "new thread";

				channel newThread = threads::startThreadWithoutMessageAsync(const& dataPackage).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
