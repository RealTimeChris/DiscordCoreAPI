Using a Message Collector {#usingmessagecollector}
============
- First, define a filtering function-which is one that takes a `Message` as its one argument, that returns a `bool`. Define it so that if it returns true, the message is "collected".
- Instantiate the `MessageCollector<Message>` class with arguments to define how many messages are collected, for how long in milliseconds the collection runs, a userId, and the filter function.
- Execute the `MessageCollector<Message>::collectMessages` function.
- This function returns a struct of type `MessageCollector<Message>`, which contains a vector of collected `Message`s.
 
```cpp
function<bool(Message)> messageFilter = [=](Message message)-> bool {
	if (userID == message.author.id) {
		return true;
	} else {
		return false;
	}
                    };
MessageCollector<Message> messageCollector;
auto returnedMessages = messageCollector.collectMessages(1, 120000, messageFilter);
if (returnedMessages.messages.size() == 0) {
	msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
	msgEmbeds = Update_MessageEmbeds(SongAPI::getPlaylist(guild.id), discordGuild, newEvent, args.eventData, currentPageIndex);
	doWeQuit = true;
	break;
}
std::vector<string> args2;
std::string newString = convertToLowerCase(returnedMessages.objects.at(0).content);
std::regex wordRegex("[a-z]{1,12}");
smatch wordRegexMatch;
std::regex_search(newString, wordRegexMatch, wordRegex,
				  std::regex_constants::match_flag_type::match_any | std::regex_constants::match_flag_type::match_not_null |
					  std::regex_constants::match_flag_type::match_prev_avail);
args2.emplace_back(wordRegexMatch.str());
std::regex_iterator<const char*>::regex_type rx("\\d{1,4}");
std::regex_iterator<const char*> next(newString.c_str(), newString.c_str() + strlen(newString.c_str()), rx);
std::regex_iterator<const char*> end;
```