Help {#Help}
============
```cpp
#pragma once

#include "Index.hpp"

	namespace DiscordCoreAPI {

	class Help : public DiscordCoreAPI::BaseFunction {
	  public:
		Help() {
			this->commandName = "help";
			this->helpDescription = "A help command for this bot!";
			EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter /help, and follow the instructions!\n------");
			msgEmbed.setTitle("__**Help Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Help>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				Channel channel = Channels::getCachedChannelAsync({.channelId = args.eventData->getChannelId()}).get();

				if (channel.type != ChannelType::Dm) {
					InputEvents::deleteInputEventResponseAsync(std::make_unique<InputEventData>(*args.eventData)).get();
				}


				Guild guild = Guilds::getCachedGuildAsync({.guildId = args.eventData->getGuildId()}).get();
				DiscordGuild discordGuild(guild);
				bool isItFirst {true};
				std::unique_ptr<InputEventData> newEvent01 {std::make_unique<InputEventData>(*args.eventData)};
				RespondToInputEventData responseData02 {*newEvent01};
				responseData02.addContent("TEST");
				responseData02.setResponseType(InputEventResponseType::Interaction_Response);
				newEvent01 = InputEvents::respondToInputEventAsync(responseData02);
				RespondToInputEventData responseData {*newEvent01};
				while (1) {
					std::vector<std::vector<SelectOptionData>> selectOptions;
					int32_t counter {0};
					int32_t currentHelpPage {0};
					for (auto& [key, value]: args.discordCoreClient->commandController.getFunctions()) {
						if (counter % 24 == 0) {
							selectOptions.push_back(std::vector<SelectOptionData>());
							currentHelpPage++;
						}
						std::string newString;
						newString.push_back((char)toupper(value->commandName[0]));
						newString += value->commandName.substr(1, value->commandName.length()-1);
						SelectOptionData newData;
						newData.label = newString;
						newData.description = value->helpDescription;
						newData.value = convertToLowerCase(newString);
						newData.emoji.name = "✅";
						bool doWeContinue {false};
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
						selectOptions.at((int64_t)currentHelpPage-(int64_t)1).push_back(newData);
						counter++;
					}
					SelectOptionData newData;
					newData.label = "Go Back";
					newData.description = "Go back to the previous menu.";
					newData.value = "go back";
					newData.emoji.name = "❌";
					std::vector<std::vector<SelectOptionData>> selectOptionsNew;
					for (auto& value: selectOptions) {
						value.push_back(newData);
						selectOptionsNew.push_back(value);
					}

					int32_t counter02 {0};
					std::string messageNew = "------\nSelect which page of help items you would like to view, by clicking a button below!\n------";
					EmbedData msgEmbed;
					msgEmbed.setAuthor(args.eventData->getUserName(), args.eventData->getAvatarUrl());
					msgEmbed.setColor(discordGuild.data.borderColor);
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(messageNew);
					msgEmbed.setTitle("__**" + args.discordCoreClient->getBotUser().userName + " Help: Front Page**__");

					std::string msgString = "------\nHello! How are you doing today?! I'm " + args.discordCoreClient->getBotUser().userName +
						" and I'm here to help you out!\n" +
						"Please, select one of my commands from the drop-down menu below, to gain more information about them! (Or select 'Go Back' to go back "
						"to the previous menu)\n------";
					std::unique_ptr<InputEventData> newEvent {std::make_unique<InputEventData>()};
					std::vector<std::string> numberEmojiNames {
						"✅",
						"🍬",
						"🅱",
						"❌",
					};
					std::vector<std::string> numberEmojiId;

					responseData.addMessageEmbed(msgEmbed);
					for (uint32_t x = 0; x < selectOptionsNew.size(); x++) {
						std::string customId {"select_page_" + std::to_string(x)};
						responseData.addButton(false, customId, std::to_string(x), ButtonStyle::Success, numberEmojiNames[x]);
						numberEmojiId.push_back(customId);
					}
					responseData.addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
					if (isItFirst) {
						responseData.setResponseType(InputEventResponseType::Ephemeral_Follow_Up_Message);
						isItFirst = false;
						newEvent01 = InputEvents::respondToInputEventAsync(responseData);
					} else {
						responseData.setResponseType(InputEventResponseType::Edit_Ephemeral_Interaction_Response);
						InputEvents::respondToInputEventAsync(responseData);
					}
					ButtonCollector button(*newEvent01);
					auto buttonData = button.collectButtonData(false, 120000, 1, args.eventData->getRequesterId()).get();
					int32_t counter03 {0};
					std::vector<RespondToInputEventData> editInteractionResponseData00;
					for (auto& value: selectOptionsNew) {
						EmbedData msgEmbed00;
						msgEmbed00.setAuthor(newEvent01->getUserName(), newEvent01->getAvatarUrl());
						msgEmbed00.setColor(discordGuild.data.borderColor);
						msgEmbed00.setTimeStamp(getTimeAndDate());
						msgEmbed00.setDescription(msgString);
						msgEmbed00.setTitle("__**" + args.discordCoreClient->getBotUser().userName + " Help: Page " + std::to_string(counter03 + 1) + " of " +
											std::to_string(selectOptions.size()) + "**__");
						RespondToInputEventData responseData03(buttonData.at(0).interactionData);
						responseData03.setResponseType(InputEventResponseType::Edit_Ephemeral_Interaction_Response);
						responseData03.addMessageEmbed(msgEmbed00);
						responseData03.addSelectMenu(false, "help_menu", value, "Commands", 1, 1);
						editInteractionResponseData00.push_back(responseData03);
						counter03++;
					}
					if (buttonData.size() > 0) {
						if (buttonData.at(0).buttonId == "exit" || buttonData.at(0).buttonId == "empty") {
							EmbedData msgEmbed00;
							msgEmbed00.setAuthor(newEvent01->getUserName(), newEvent01->getAvatarUrl());
							msgEmbed00.setColor(discordGuild.data.borderColor);
							msgEmbed00.setTimeStamp(getTimeAndDate());
							msgEmbed00.setDescription(messageNew);
							msgEmbed00.setTitle("__**" + args.discordCoreClient->getBotUser().userName + " Help: Page " + std::to_string(counter03 + 1) +
												" of " + std::to_string(selectOptions.size()) + "**__");
							RespondToInputEventData responseData03(buttonData.at(0).interactionData);
							responseData03.setResponseType(InputEventResponseType::Edit_Ephemeral_Interaction_Response);
							responseData03.addMessageEmbed(msgEmbed00);
							newEvent = InputEvents::respondToInputEventAsync(responseData02);
							InputEvents::deleteInputEventResponseAsync(std::move(newEvent01)).get();
							break;
						}
						counter02 = 0;
						for (int32_t y = 0; y < numberEmojiId.size(); y++) {
							if (buttonData.at(0).buttonId == numberEmojiId[y]) {
								counter02 = y;
								break;
							}
						}
						newEvent = InputEvents::respondToInputEventAsync(editInteractionResponseData00.at(counter02));
					} else {
						InputEvents::deleteInputEventResponseAsync(std::move(newEvent)).get();
						break;
					}
					SelectMenuCollector selectMenu(*newEvent01);
					auto selectMenuReturnData = selectMenu.collectSelectMenuData(false, 120000, 1, args.eventData->getRequesterId()).get();
					if (selectMenuReturnData.size() > 0) {
						EmbedData newEmbed;
						for (auto& [key, value]: args.discordCoreClient->commandController.getFunctions()) {
							for (auto& valueNew: key) {
								if (valueNew == selectMenuReturnData.at(0).values.at(0)) {
									newEmbed = args.discordCoreClient->commandController.getFunctions().at(key)->helpEmbed;
								}
							}
						}
						if (selectMenuReturnData.at(0).values.at(0) == "go back") {
							RespondToInputEventData responseData02(selectMenuReturnData.at(0).interactionData);
							responseData02.setResponseType(InputEventResponseType::Edit_Ephemeral_Interaction_Response);
							responseData02.addMessageEmbed(msgEmbed);
							for (uint32_t x = 0; x < selectOptionsNew.size(); x++) {
								std::string customId {"select_page_" + std::to_string(x)};
								responseData02.addButton(false, customId, std::to_string(x), ButtonStyle::Success, numberEmojiNames[x]);
								numberEmojiId.push_back(customId);
							}
							responseData02.addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
							newEvent = InputEvents::respondToInputEventAsync(responseData02);
							continue;
						}

						RespondToInputEventData responseData02(selectMenuReturnData.at(0).interactionData);
						responseData02.setResponseType(InputEventResponseType::Edit_Ephemeral_Interaction_Response);
						responseData02.addMessageEmbed(newEmbed);
						responseData02.addButton(false, "back", "Back", ButtonStyle::Success, "🔙");
						responseData02.addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
						newEvent = InputEvents::respondToInputEventAsync(responseData02);
						auto buttonReturnData02 = ButtonCollector {*newEvent01}.collectButtonData(false, 120000, 1, args.eventData->getRequesterId()).get();
						if (buttonReturnData02.size() > 0) {
							{
							}
							if (buttonReturnData02.at(0).buttonId == "back") {
								responseData = RespondToInputEventData {buttonReturnData02.at(0).interactionData};
								continue;
							} else if (buttonReturnData02.at(0).buttonId == "exit" || buttonReturnData02.at(0).buttonId == "empty") {
								RespondToInputEventData responseData02(buttonReturnData02.at(0).interactionData);
								responseData02.setResponseType(InputEventResponseType::Edit_Ephemeral_Interaction_Response);
								responseData02.addMessageEmbed(newEmbed);
								newEvent = InputEvents::respondToInputEventAsync(responseData02);
								InputEvents::deleteInputEventResponseAsync(std::move(newEvent)).get();
								break;
							}
						} else {
							InputEvents::deleteInputEventResponseAsync(std::move(newEvent)).get();
							break;
						}
					} else {
						InputEvents::deleteInputEventResponseAsync(std::move(newEvent)).get();
						break;
					}
				}


				return;
			} catch (...) {
				reportException("Help::execute()");
			}
		}
		virtual ~Help();
	};
}
```