// Help.hpp - Header for the "help" command.
// May 20, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _HELP_
#define _HELP_

#include "../DiscordCoreClient02.hpp"

namespace DiscordCoreAPI {

	class Help : public  BaseFunction {
	public:
		Help() {
			this->commandName = "help";
			this->helpDescription = "A help command for this bot!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !help or /help, and follow the instructions!\n------");
			msgEmbed.setTitle("__**Help Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		Help* create() {
			return new Help;
		}

		virtual  task<void> execute(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				Channel channel = Channels::getChannelAsync({ .channelId = args->eventData.getChannelId() }).get();

				if (channel.type != ChannelType::DM) {
					InputEvents::deleteInputEventResponseAsync(args->eventData);
				}

				Guild guild = Guilds::getGuildAsync({ .guildId = args->eventData.getGuildId() }).get();
				DiscordGuild discordGuild(guild);
				string messageString = "------\nHello! How are you doing today?! I'm " + getBotUser().userName + " and I'm here to help you out!\n" +
					"Please, select one of my commands from the drop-down menu below, to gain more information about them! (Or select 'Go Back' to go back to the previous menu)\n------";
				InputEventData newEvent = args->eventData;
				if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
					RespondToInputEventData responseData(newEvent);
					responseData.type = DesiredInputEventResponseType::RegularMessage;
					responseData.addContent("1");
					newEvent = InputEvents::respondToEvent(responseData);
				}
				else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
					RespondToInputEventData responseData(newEvent);
					responseData.type = DesiredInputEventResponseType::EphemeralInteractionResponse;
					responseData.addContent("1");
					newEvent = InputEvents::respondToEvent(responseData);
				}
				while (1) {
					vector<vector<SelectOptionData>> selectOptions;
					int counter{ 0 };
					int currentHelpPage{ 0 };
					for (auto [key, value] : CommandCenter::functions) {
						if (counter % 24 == 0) {
							selectOptions.push_back(vector<SelectOptionData>());
							currentHelpPage += 1;
						}
						string newString;
						newString.push_back((char)toupper(value->commandName[0]));
						newString += value->commandName.substr(1, value->commandName.length() - 1);
						SelectOptionData newData;
						newData.label = newString;
						newData.description = value->helpDescription;
						newData.value = convertToLowerCase(newString);
						newData.emoji.name = "✅";
						bool doWeContinue{ false };
						for (auto value02 : selectOptions) {
							for (auto value03 : value02) {
								if (value03.value == newData.value) {
									doWeContinue = true;
									break;
								}
							}
						}
						if (doWeContinue) {
							continue;
						}
						selectOptions.at(currentHelpPage - 1).push_back(newData);
						counter += 1;
					}
					SelectOptionData newData;
					newData.label = "Go Back";
					newData.description = "Go back to the previous menu.";
					newData.value = "go back";
					newData.emoji.name = "❌";
					vector<vector<SelectOptionData>> selectOptionsNew;
					for (auto value : selectOptions) {
						value.push_back(newData);
						selectOptionsNew.push_back(value);
					}
					int counter02{ 0 };
					string messageNew = "------\nSelect which page of help items you would like to view, by clicking a button below!\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(newEvent.getUserName(), newEvent.getAvatarURL());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(messageNew);
					msgEmbed.setTitle("__**" + getBotUser().userName + " Help: Front Page**__");

					vector<string> numberEmojiNames{ "✅", "🍬", "🅱", "❌", };
					vector<string> numberEmojiId;

					if (newEvent.eventType == InputEventType::REGULAR_MESSAGE) {
						RespondToInputEventData responseData(newEvent);
						responseData.type = DesiredInputEventResponseType::RegularMessageEdit;
						responseData.addMessageEmbed(msgEmbed);
						counter02 = 0;
						for (auto value : selectOptionsNew) {
							counter02 += 1;
							string customId{ "select_page_" + to_string(counter02) };
							responseData.addButton(false, customId, to_string(counter02), numberEmojiNames[counter02], ButtonStyle::Success);
							numberEmojiId.push_back(customId);
						}
						responseData.addButton(false, "exit", "Exit", "❌", ButtonStyle::Danger);
						newEvent = InputEvents::respondToEvent(responseData);
					}
					else if (newEvent.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
						RespondToInputEventData responseData(newEvent);
						responseData.type = DesiredInputEventResponseType::InteractionResponseEdit;
						responseData.addMessageEmbed(msgEmbed);
						counter02 = 0;
						for (auto value : selectOptionsNew) {
							counter02 += 1;
							string customId{ "select_page_" + to_string(counter02) };
							responseData.addButton(false, customId, to_string(counter02), numberEmojiNames[counter02], ButtonStyle::Success);
							numberEmojiId.push_back(customId);
						}
						responseData.addButton(false, "exit", "Exit", "❌", ButtonStyle::Danger);
						newEvent = InputEvents::respondToEvent(responseData);
					}
					ButtonCollector button(newEvent);
					auto buttonData = button.collectButtonData(false, 120000, 1, args->eventData.getAuthorId());
					int counter03{ 0 };
					vector<RespondToInputEventData> editMessageData00;
					vector<RespondToInputEventData> editInteractionResponseData00;
					for (auto value : selectOptionsNew) {
						EmbedData msgEmbed00;
						msgEmbed00.setAuthor(newEvent.getUserName(), newEvent.getAvatarURL());
						msgEmbed00.setColor(discordGuild.data.borderColor);
						msgEmbed00.setTimeStamp(getTimeAndDate());
						msgEmbed00.setDescription(messageString);
						msgEmbed00.setTitle("__**" + getBotUser().userName + " Help: Page " + to_string(counter03 + 1) + " of " + to_string(selectOptions.size()) + "**__");
						RespondToInputEventData responseData(newEvent);
						responseData.type = DesiredInputEventResponseType::RegularMessageEdit;
						responseData.addMessageEmbed(msgEmbed00);
						responseData.addSelectMenu(false, "help_menu", value, "Commands", 1, 1);
						editMessageData00.push_back(responseData);
						RespondToInputEventData responseData02(newEvent);
						responseData.type = DesiredInputEventResponseType::InteractionResponseEdit;
						responseData02.addMessageEmbed(msgEmbed00);
						responseData02.addSelectMenu(false, "help_menu", value, "Commands", 1, 1);
						editInteractionResponseData00.push_back(responseData02);
						counter03 += 1;
					}
					if (buttonData.size() > 0) {
						if (buttonData.at(0).buttonId == "exit") {
							InputEvents::deleteInputEventResponseAsync(newEvent).get();
							break;
						}
						counter02 = 0;
						for (int y = 0; y < numberEmojiId.size(); y += 1) {
							if (buttonData.at(0).buttonId == numberEmojiId[y]) {
								counter02 = y;
								break;
							}
						}
						if (newEvent.eventType == InputEventType::REGULAR_MESSAGE) {
							newEvent = InputEvents::respondToEvent(editMessageData00.at(counter02));
						}
						else if (newEvent.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
							newEvent = InputEvents::respondToEvent(editInteractionResponseData00.at(counter02));
						}
					}
					else {
						InputEvents::deleteInputEventResponseAsync(newEvent).get();
						break;
					}
					SelectMenu selectMenu(newEvent);
					auto selectMenuReturnData = selectMenu.collectSelectMenuData(false, 120000, args->eventData.getAuthorId());
					if (selectMenuReturnData.size() > 0) {
						if (selectMenuReturnData.at(0).values.at(0).value == "go back") {
							continue;
						}
						EmbedData newEmbed = CommandCenter::functions.at(selectMenuReturnData.at(0).values.at(0).value)->helpEmbed;
						if (newEvent.eventType == InputEventType::REGULAR_MESSAGE) {
							RespondToInputEventData responseData(newEvent);
							responseData.type = DesiredInputEventResponseType::RegularMessageEdit;
							responseData.addMessageEmbed(newEmbed);
							responseData.addButton(false, "back", "Back", "🔙", ButtonStyle::Success);
							responseData.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
							newEvent = InputEvents::respondToEvent(responseData);
						}
						else if (newEvent.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
							RespondToInputEventData responseData(newEvent);
							responseData.type = DesiredInputEventResponseType::InteractionResponseEdit;
							responseData.addMessageEmbed(newEmbed);
							responseData.addButton(false, "back", "Back", "🔙", ButtonStyle::Success);
							responseData.addButton(false, "exit", "Exit", "❌", ButtonStyle::Success);
							newEvent = InputEvents::respondToEvent(responseData);
						}
						ButtonCollector button02(newEvent);
						auto buttonReturnData = button02.collectButtonData(false, 120000, 1, args->eventData.getAuthorId());
						if (buttonReturnData.size() > 0) {
							if (buttonReturnData.at(0).buttonId == "back") {
								continue;
							}
							else if (buttonReturnData.at(0).buttonId == "exit") {
								InputEvents::deleteInputEventResponseAsync(newEvent).get();
								break;
							}
						}
						else {
							InputEvents::deleteInputEventResponseAsync(newEvent).get();
							break;
						}
					}
					else {
						InputEvents::deleteInputEventResponseAsync(newEvent).get();
						break;
					}
				}


				co_return;
			}
			catch (exception error) {
				cout << "Help::execute() Error: " << error.what() << endl << endl;
			}

		}
	};
}
#endif
