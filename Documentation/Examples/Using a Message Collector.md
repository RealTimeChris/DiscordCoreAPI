### **Using a Message Collector:**
---
- First, define a filtering function - which is one that takes a `DiscordCoreAPI::Message` as its one argument, that returns a `bool`. Define it so that if it returns true, the message is "collected".
- Instantiate the `DiscordCoreAPI::MessageCollector` class with arguments to define how many messages are collected, for how long in milliseconds the collection runs, a userId, and the filter function.
- Execute the `MessageCollector::collectMessages` function, with `.get()` added to the end of it to block the current thread to wait for the message results.
- This function returns a struct of type `MessageCollectorReturnData`, which contains a vector of collected `Message`s.
 
```cpp
function<bool(Message)> messageFilter = [=](Message message)-> bool {
                        if (userID == message.author.id) {
                            return true;
                        }
                        else {
                            return false;
                        }
                    };

                    MessageCollector messageCollector(1, 120000, userID, messageFilter);
                    auto returnedMessages = messageCollector.collectMessages().get();
                    if (returnedMessages.messages.size() == 0) {
                        msgEmbeds.erase(msgEmbeds.begin() + currentPageIndex, msgEmbeds.begin() + currentPageIndex + 1);
                        msgEmbeds = updateMessageEmbeds(*YouTubeAPI::getQueue(guild.id), discordGuild, newEvent, args->eventData, currentPageIndex);
                        doWeQuit = true;
                        break;
                    }
                    vector<string> args2;
                    string newString = convertToLowerCase(returnedMessages.messages.at(0).content);
                    regex wordRegex("[a-z]{1,12}");
                    smatch wordRegexMatch;
                    regex_search(newString, wordRegexMatch, wordRegex, regex_constants::match_flag_type::match_any | regex_constants::match_flag_type::match_not_null | regex_constants::match_flag_type::match_prev_avail);
                    args2.push_back(wordRegexMatch.str());
                    regex_iterator<const char*>::regex_type rx("\\d{1,4}");
                    regex_iterator<const char*> next(newString.c_str(), newString.c_str() + strlen(newString.c_str()), rx);
                    regex_iterator<const char*> end;
```
