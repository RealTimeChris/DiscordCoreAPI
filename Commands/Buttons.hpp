// Buttons.hpp - Exploring Buttons.
// June 1, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _BUTTONS_
#define _BUTTONS_

#include "pch.h"
#include "../DiscordCoreClient.hpp"
#include "Commands.hpp"

namespace DiscordCoreAPI {
	class Buttons : public BaseFunction {
	public:
		Buttons() {
			this->commandName = "buttons";
			this->helpDescription = "__**Buttons:**__ Enter !buttons or /buttons.";
		}

		virtual task<void> execute(DiscordCoreAPI::BaseFunctionArguments* args) {
			ReplyMessageData createMessageData;
			createMessageData.embed.setAuthor(args->message.data.author.username, args->message.data.author.getAvatarURL());
			createMessageData.embed.setColor("FFFFFF");
			createMessageData.embed.setTimeStamp(getTimeAndDate());
			createMessageData.embed.setDescription("Testing the buttons!");
			createMessageData.embed.setTitle("Time to test buttons!");
			createMessageData.replyingToMessageData = args->message.data;
			ActionRowData actionRowData;
			ComponentData componentData;
			componentData.customId = "test_button";
			componentData.emoji.name = "🍉";
			componentData.label = "The Button";
			componentData.style = ButtonStyle::Success;
			componentData.type = ComponentType::Button;
			actionRowData.components.push_back(componentData);
			createMessageData.components.push_back(actionRowData);
			
			Message msg = args->coreClient->messages->replyAsync(createMessageData).get();

			co_return;
		}
	};
	Buttons buttons{};
}
#endif