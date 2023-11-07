Using a Message Collector {#using_a_message_collector}
============
- first, define a filtering function-which is one that takes a `discord_core_api::message_data` as its one argument, that returns A `bool`. define it so that if it returns true, the message is "collected".
- instantiate the `discord_core_api::object_collector<discord_core_api::message_data>` class with arguments to define how many messages are collected, for how long in milliseconds the collection runs, a userId, and the filter function.
- Execute the `discord_core_api::object_collector<discord_core_api::message_data>::collectObjects` function.
- this function returns A struct of type `discord_core_api::object_collector<discord_core_api::message_data>`, which contains a vector of collected `discord_core_api::message_data`s.
 
```cpp
function<bool(message)> messageFilter = [=](message message)-> bool {
	if (userID == message.author.id) {
		return true;
	} else {
		return false;
	}
                    };
message_collector<message> messageCollector;
auto returnedMessages = messageCollector.collectObjects(1, 120000, messageFilter);
if (returnedMessages.messages.size() == 0) {
	msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
	msgEmbeds = Update_MessageEmbeds(song_api::getPlaylist(guild_data.id), discordGuild, newEvent, args.eventData, currentPageIndex);
	doWeQuit = true;
	break;
}
jsonifier::vector<string> args2;
jsonifier::string newString = convertToLowerCase(returnedMessages.objects.at(0).content);
std::regex wordRegex("[a-z]{1,12}");
smatch wordRegexMatch;
std::regex_search(newString, wordRegexMatch, wordRegex,
				  std::regex_constants::match_flag_type::match_any | std::regex_constants::match_flag_type::match_not_null |
					  std::regex_constants::match_flag_type::match_prev_avail);
args2.emplace_back(wordRegexMatch.str());
std::regex_iterator<const char*>::regex_type rx("\\d{1,4}");
std::regex_iterator<const char*> next(newString.data(), newString.data() + strlen(newString.data()), rx);
std::regex_iterator<const char*> end;
```