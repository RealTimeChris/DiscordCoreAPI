Help {#Help}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace discord_core_api {

	class help : public base_function {
	  public:
		help() {
			commandName = "help";
			helpDescription = "a help command for this bot!";
			embed_data msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /help, and follow the instructions!\n------");
			msgEmbed.setTitle("__**help usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<help>();
		}

		void execute(base_function_arguments& newArgs) {
			try {
				bool isItFirst{ true };
				input_event_data newEvent01(newArgs.eventData);

				while (1) {
					respond_to_input_event_data responseData{ newEvent01 };
					jsonifier::vector<jsonifier::vector<select_option_data>> selectOptions;
					int32_t counter{};
					int32_t currentHelpPage{};
					for (auto& [key, value]: discord_core_client::getInstance()->getCommandController().getFunctions()) {
						if (counter % 24 == 0) {
							selectOptions.emplace_back(jsonifier::vector<select_option_data>());
							currentHelpPage += 1;
						}
						jsonifier::string newString;
						newString.emplace_back(( char )toupper(value->commandName[0]));
						newString += value->commandName.substr(1, value->commandName.size() - 1);
						select_option_data newData;
						newData.label = newString;
						newData.description = value->helpDescription;
						newData.value = convertToLowerCase(newString);
						value->helpEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						newData.emoji.name = "✅";
						bool doWeContinue{};
						for (auto& value02: selectOptions) {
							for (auto& value03: value02) {
								if (value03.value == newData.value) {
									doWeContinue = true;
									break;
								}
							}
						}
						if (doWeContinue) {
							continue;
						}
						selectOptions.at(( int64_t )currentHelpPage - ( int64_t )1).emplace_back(newData);
						counter += 1;
					}
					select_option_data newData;
					newData.label = "go back";
					newData.description = "go back to the previous menu.";
					newData.value = "go back";
					newData.emoji.name = "❌";
					jsonifier::vector<jsonifier::vector<select_option_data>> selectOptionsNew;
					for (auto& value: selectOptions) {
						value.emplace_back(newData);
						selectOptionsNew.emplace_back(value);
					}

					int32_t counter02{};
					jsonifier::string messageNew = "------\nSelect which page of help items you would like to view, by clicking a button below!\n------";
					embed_data msgEmbed{};
					msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					msgEmbed.setColor("fe_fe_fe");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(messageNew);
					msgEmbed.setTitle("__**" + static_cast<jsonifier::string>(discord_core_client::getInstance()->getBotUser().userName) + " help: front page**__");

					jsonifier::string msgString = "------\nHello! how are you doing today?! i'm " + static_cast<jsonifier::string>(discord_core_client::getInstance()->getBotUser().userName) +
						" and i'm here to help you out!\n" +
						"please, select one of my commands from the drop-down menu below, to gain more information about them! (or select 'go back' to go back "
						"to the previous menu)\n------";
					input_event_data newEvent{};
					jsonifier::vector<jsonifier::string> numberEmojiNames{
						"✅",
						"🍬",
						"🅱",
						"❌",
					};
					jsonifier::vector<jsonifier::string> numberEmojiId;

					responseData.addMessageEmbed(msgEmbed);
					for (int32_t x = 0; x < selectOptionsNew.size(); x += 1) {
						jsonifier::string customId{ "select_page_" + jsonifier::toString(x) };
						responseData.addButton(false, customId, jsonifier::toString(x), button_style::success, numberEmojiNames[x]);
						numberEmojiId.emplace_back(customId);
					}
					responseData.addButton(false, "exit", "exit", button_style::danger, "❌");
					if (isItFirst) {
						responseData.setResponseType(input_event_response_type::Ephemeral_Interaction_Response);
						isItFirst = false;
						newEvent01 = input_events::respondToInputEventAsync(responseData).get();
					} else {
						responseData.setResponseType(input_event_response_type::Edit_Interaction_Response);
						newEvent01 = input_events::respondToInputEventAsync(responseData).get();
					}
					unique_ptr<button_collector> button{ makeUnique<button_collector>(newEvent01) };
					auto buttonData = button->collectButtonData(false, 120000, 1, newArgs.eventData.getAuthorId()).get();
					int32_t counter03{};
					jsonifier::vector<respond_to_input_event_data> editInteractionResponseData00;
					for (auto& value: selectOptionsNew) {
						embed_data msgEmbed00;
						msgEmbed00.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						msgEmbed00.setColor("fe_fe_fe");
						msgEmbed00.setTimeStamp(getTimeAndDate());
						msgEmbed00.setDescription(msgString);
						msgEmbed00.setTitle("__**" + static_cast<jsonifier::string>(discord_core_client::getInstance()->getBotUser().userName) + " help: page " + jsonifier::toString(counter03 + 1) +
							" of " + jsonifier::toString(selectOptions.size()) + "**__");
						respond_to_input_event_data responseData03(*buttonData.at(0).interactionData);
						responseData03.setResponseType(input_event_response_type::Edit_Interaction_Response);
						responseData03.addMessageEmbed(msgEmbed00);
						responseData03.addSelectMenu(false, "help_menu", value, "commands", 1, 1);
						editInteractionResponseData00.emplace_back(responseData03);
						counter03 += 1;
					}
					if (buttonData.size() > 0) {
						if (buttonData.at(0).buttonId == "exit" || buttonData.at(0).buttonId == "empty") {
							embed_data msgEmbed00;
							msgEmbed00.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
							msgEmbed00.setColor("fe_fe_fe");
							msgEmbed00.setTimeStamp(getTimeAndDate());
							msgEmbed00.setDescription(messageNew);
							msgEmbed00.setTitle("__**" + static_cast<jsonifier::string>(discord_core_client::getInstance()->getBotUser().userName) + " help: page " +
								jsonifier::toString(counter03 + 1) + " of " + jsonifier::toString(selectOptions.size()) + "**__");
							respond_to_input_event_data responseData03(*buttonData.at(0).interactionData);
							responseData03.setResponseType(input_event_response_type::Edit_Interaction_Response);
							responseData03.addMessageEmbed(msgEmbed00);
							newEvent = input_events::respondToInputEventAsync(responseData03).get();
							break;
						}
						counter02 = 0;
						for (int32_t y = 0; y < numberEmojiId.size(); y += 1) {
							if (buttonData.at(0).buttonId == numberEmojiId[y]) {
								counter02 = y;
								break;
							}
						}
						newEvent = input_events::respondToInputEventAsync(editInteractionResponseData00.at(counter02)).get();
					} else {
						break;
					}
					unique_ptr<select_menu_collector> selectMenu{ makeUnique<select_menu_collector>(newEvent01) };
					auto selectMenuReturnData = selectMenu->collectSelectMenuData(false, 120000, 1, newArgs.eventData.getAuthorId()).get();
					embed_data newEmbed{};
					for (auto& [key, value]: discord_core_client::getInstance()->getCommandController().getFunctions()) {
						for (auto& valueNew: key) {
							if (valueNew == selectMenuReturnData.at(0).values.at(0)) {
								newEmbed = discord_core_client::getInstance()->getCommandController().getFunctions().at(key)->helpEmbed;
							}
						}
					}
					if (selectMenuReturnData.at(0).values.at(0) == "go back") {
						respond_to_input_event_data responseData02(*selectMenuReturnData.at(0).interactionData);
						responseData02.setResponseType(input_event_response_type::Edit_Interaction_Response);
						responseData02.addMessageEmbed(msgEmbed);
						for (int32_t x = 0; x < selectOptionsNew.size(); x += 1) {
							jsonifier::string customId{ "select_page_" + jsonifier::toString(x) };
							responseData02.addButton(false, customId, jsonifier::toString(x), button_style::success, numberEmojiNames[x]);
							numberEmojiId.emplace_back(customId);
						}
						responseData02.addButton(false, "exit", "exit", button_style::danger, "❌");
						newEvent = input_events::respondToInputEventAsync(responseData02).get();
						continue;
					}

					respond_to_input_event_data responseData02(*selectMenuReturnData.at(0).interactionData);
					responseData02.setResponseType(input_event_response_type::Edit_Interaction_Response);
					responseData02.addMessageEmbed(newEmbed);
					responseData02.addButton(false, "back", "back", button_style::success, "🔙");
					responseData02.addButton(false, "exit", "exit", button_style::success, "❌");
					newEvent = input_events::respondToInputEventAsync(responseData02).get();
					auto buttonReturnData02 = button_collector{ newEvent01 }.collectButtonData(false, 120000, 1, newArgs.eventData.getAuthorId()).get();
					if (buttonReturnData02.at(0).buttonId == "back") {
						responseData = respond_to_input_event_data{ *buttonReturnData02.at(0).interactionData };
						responseData.setResponseType(input_event_response_type::Deferred_Response);
						auto interactionData = input_events::respondToInputEventAsync(responseData).get().getInteractionData();
						responseData = respond_to_input_event_data{ interactionData };
						continue;
					} else if (buttonReturnData02.at(0).buttonId == "exit" || buttonReturnData02.at(0).buttonId == "empty") {
						respond_to_input_event_data responseData03(*buttonReturnData02.at(0).interactionData);
						responseData03.setResponseType(input_event_response_type::Edit_Interaction_Response);
						responseData03.addMessageEmbed(newEmbed);
						newEvent = input_events::respondToInputEventAsync(responseData03).get();
						break;
					}
				}

				return;
			} catch (...) {
				reportException("help::execute()");
			}
		}
		~help(){};
	};
}
```