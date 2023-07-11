Help {#Help}
============
```cpp

#pragma once

#include "../HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Help : public BaseFunction {
	  public:
		Help() {
			commandName = "help";
			helpDescription = "A help command for this bot!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter /help, and follow the instructions!\n------");
			msgEmbed.setTitle("__**Help Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Help>();
		}

		void execute(BaseFunctionArguments& newArgs) {
			try {
				bool isItFirst{ true };
				InputEventData newEvent01(newArgs.eventData);

				while (1) {
					RespondToInputEventData responseData{ newEvent01 };
					std::vector<std::vector<SelectOptionData>> selectOptions;
					int32_t counter{};
					int32_t currentHelpPage{};
					for (auto& [key, value]: newArgs.discordCoreClient->getCommandController().getFunctions()) {
						if (counter % 24 == 0) {
							selectOptions.emplace_back(std::vector<SelectOptionData>());
							currentHelpPage += 1;
						}
						std::string newString;
						newString.emplace_back(( char )toupper(value->commandName[0]));
						newString += value->commandName.substr(1, value->commandName.length() - 1);
						SelectOptionData newData;
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
					SelectOptionData newData;
					newData.label = "Go Back";
					newData.description = "Go back to the previous menu.";
					newData.value = "go back";
					newData.emoji.name = "❌";
					std::vector<std::vector<SelectOptionData>> selectOptionsNew;
					for (auto& value: selectOptions) {
						value.emplace_back(newData);
						selectOptionsNew.emplace_back(value);
					}

					int32_t counter02{};
					std::string messageNew = "------\nSelect which page of help items you would like to view, by clicking a button below!\n------";
					EmbedData msgEmbed{};
					msgEmbed.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
					msgEmbed.setColor("FeFeFe");
					msgEmbed.setTimeStamp(getTimeAndDate());
					msgEmbed.setDescription(messageNew);
					msgEmbed.setTitle("__**" + static_cast<std::string>(newArgs.discordCoreClient->getBotUser().userName) + " Help: Front Page**__");

					std::string msgString = "------\nHello! How are you doing today?! I'm " + static_cast<std::string>(newArgs.discordCoreClient->getBotUser().userName) +
						" and I'm here to help you out!\n" +
						"Please, select one of my commands from the drop-down menu below, to gain more information about them! (Or select 'Go Back' to go back "
						"to the previous menu)\n------";
					InputEventData newEvent{};
					std::vector<std::string> numberEmojiNames{
						"✅",
						"🍬",
						"🅱",
						"❌",
					};
					std::vector<std::string> numberEmojiId;

					responseData.addMessageEmbed(msgEmbed);
					for (int32_t x = 0; x < selectOptionsNew.size(); x += 1) {
						std::string customId{ "select_page_" + std::to_string(x) };
						responseData.addButton(false, customId, std::to_string(x), ButtonStyle::Success, numberEmojiNames[x]);
						numberEmojiId.emplace_back(customId);
					}
					responseData.addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
					if (isItFirst) {
						responseData.setResponseType(InputEventResponseType::Ephemeral_Interaction_Response);
						isItFirst = false;
						newEvent01 = InputEvents::respondToInputEventAsync(responseData).get();
					} else {
						responseData.setResponseType(InputEventResponseType::Edit_Interaction_Response);
						newEvent01 = InputEvents::respondToInputEventAsync(responseData).get();
					}
					UniquePtr<ButtonCollector> button{ makeUnique<ButtonCollector>(newEvent01) };
					auto buttonData = button->collectButtonData(false, 120000, 1, newArgs.eventData.getAuthorId()).get();
					int32_t counter03{};
					std::vector<RespondToInputEventData> editInteractionResponseData00;
					for (auto& value: selectOptionsNew) {
						EmbedData msgEmbed00;
						msgEmbed00.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
						msgEmbed00.setColor("FeFeFe");
						msgEmbed00.setTimeStamp(getTimeAndDate());
						msgEmbed00.setDescription(msgString);
						msgEmbed00.setTitle("__**" + static_cast<std::string>(newArgs.discordCoreClient->getBotUser().userName) + " Help: Page " + std::to_string(counter03 + 1) +
							" of " + std::to_string(selectOptions.size()) + "**__");
						RespondToInputEventData responseData03(*buttonData.at(0).interactionData);
						responseData03.setResponseType(InputEventResponseType::Edit_Interaction_Response);
						responseData03.addMessageEmbed(msgEmbed00);
						responseData03.addSelectMenu(false, "help_menu", value, "Commands", 1, 1);
						editInteractionResponseData00.emplace_back(responseData03);
						counter03 += 1;
					}
					if (buttonData.size() > 0) {
						if (buttonData.at(0).buttonId == "exit" || buttonData.at(0).buttonId == "empty") {
							EmbedData msgEmbed00;
							msgEmbed00.setAuthor(newArgs.eventData.getUserName(), newArgs.eventData.getAvatarUrl());
							msgEmbed00.setColor("FeFeFe");
							msgEmbed00.setTimeStamp(getTimeAndDate());
							msgEmbed00.setDescription(messageNew);
							msgEmbed00.setTitle("__**" + static_cast<std::string>(newArgs.discordCoreClient->getBotUser().userName) + " Help: Page " +
								std::to_string(counter03 + 1) + " of " + std::to_string(selectOptions.size()) + "**__");
							RespondToInputEventData responseData03(*buttonData.at(0).interactionData);
							responseData03.setResponseType(InputEventResponseType::Edit_Interaction_Response);
							responseData03.addMessageEmbed(msgEmbed00);
							newEvent = InputEvents::respondToInputEventAsync(responseData03).get();
							break;
						}
						counter02 = 0;
						for (int32_t y = 0; y < numberEmojiId.size(); y += 1) {
							if (buttonData.at(0).buttonId == numberEmojiId[y]) {
								counter02 = y;
								break;
							}
						}
						newEvent = InputEvents::respondToInputEventAsync(editInteractionResponseData00.at(counter02)).get();
					} else {
						break;
					}
					UniquePtr<SelectMenuCollector> selectMenu{ makeUnique<SelectMenuCollector>(newEvent01) };
					auto selectMenuReturnData = selectMenu->collectSelectMenuData(false, 120000, 1, newArgs.eventData.getAuthorId()).get();
					EmbedData newEmbed{};
					for (auto& [key, value]: newArgs.discordCoreClient->getCommandController().getFunctions()) {
						for (auto& valueNew: key) {
							if (valueNew == selectMenuReturnData.at(0).values.at(0)) {
								newEmbed = newArgs.discordCoreClient->getCommandController().getFunctions().at(key)->helpEmbed;
							}
						}
					}
					if (selectMenuReturnData.at(0).values.at(0) == "go back") {
						RespondToInputEventData responseData02(*selectMenuReturnData.at(0).interactionData);
						responseData02.setResponseType(InputEventResponseType::Edit_Interaction_Response);
						responseData02.addMessageEmbed(msgEmbed);
						for (int32_t x = 0; x < selectOptionsNew.size(); x += 1) {
							std::string customId{ "select_page_" + std::to_string(x) };
							responseData02.addButton(false, customId, std::to_string(x), ButtonStyle::Success, numberEmojiNames[x]);
							numberEmojiId.emplace_back(customId);
						}
						responseData02.addButton(false, "exit", "Exit", ButtonStyle::Danger, "❌");
						newEvent = InputEvents::respondToInputEventAsync(responseData02).get();
						continue;
					}

					RespondToInputEventData responseData02(*selectMenuReturnData.at(0).interactionData);
					responseData02.setResponseType(InputEventResponseType::Edit_Interaction_Response);
					responseData02.addMessageEmbed(newEmbed);
					responseData02.addButton(false, "back", "Back", ButtonStyle::Success, "🔙");
					responseData02.addButton(false, "exit", "Exit", ButtonStyle::Success, "❌");
					newEvent = InputEvents::respondToInputEventAsync(responseData02).get();
					auto buttonReturnData02 = ButtonCollector{ newEvent01 }.collectButtonData(false, 120000, 1, newArgs.eventData.getAuthorId()).get();
					if (buttonReturnData02.at(0).buttonId == "back") {
						responseData = RespondToInputEventData{ *buttonReturnData02.at(0).interactionData };
						responseData.setResponseType(InputEventResponseType::Deferred_Response);
						auto interactionData = InputEvents::respondToInputEventAsync(responseData).get().getInteractionData();
						responseData = RespondToInputEventData{ interactionData };
						continue;
					} else if (buttonReturnData02.at(0).buttonId == "exit" || buttonReturnData02.at(0).buttonId == "empty") {
						RespondToInputEventData responseData03(*buttonReturnData02.at(0).interactionData);
						responseData03.setResponseType(InputEventResponseType::Edit_Interaction_Response);
						responseData03.addMessageEmbed(newEmbed);
						newEvent = InputEvents::respondToInputEventAsync(responseData03).get();
						break;
					}
				}

				return;
			} catch (...) {
				reportException("Help::execute()");
			}
		}
		~Help(){};
	};
}
```