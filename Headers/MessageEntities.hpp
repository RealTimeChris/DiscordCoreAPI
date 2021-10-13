// MessageEntities.hpp - Header for the Message related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _MESSAGE_ENTITIES_
#define _MESSAGE_ENTITIES_

#include "IndexInitial.hpp"
#include "JSONifier.hpp"
#include "ThreadManager.hpp"
#include "Http.hpp"

namespace DiscordCoreAPI {

	/**
	* \addtogroup foundation_entities
	* @{
	*/
	/// Get Messages data. \brief Get Messages data.
	struct DiscordCoreAPI_Dll GetMessagesData {
		string aroundThisId{ "" };///<Around this id.
		string beforeThisId{ "" };///< Before this id.
		string afterThisId{ "" };///< After this id.
		string channelId{ "" };///< Channel from which to collect the Messages.
		 __int32 limit{ 0 };///< Limit of Messages to collect.		
	};

	/// Get Message data. \brief Get Message data.
	struct DiscordCoreAPI_Dll GetMessageData {
		string requesterId{ "" };///< The requester id.
		string channelId{ "" };///< The Channel from which to collect the Message.
		string id{ "" };///< The id of the Message to collect.
	};

	/// Create Message data. \brief Create Message data.
	struct DiscordCoreAPI_Dll CreateMessageData {

		friend class DiscordCoreInternal::MessageManager;

		CreateMessageData(string channelIdNew) {
			this->channelId = channelIdNew;
		}

		CreateMessageData(InputEventData dataPackage) {
			this->requesterId = dataPackage.getRequesterId();
			this->channelId = dataPackage.getChannelId();
		}

		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customId A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.        
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		/// \returns void
		void addButton(bool disabled, string customId, string buttonLabel, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.type = ComponentType::Button;
					component.emoji.name = emojiName;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.id = emojiId;
					component.url = url;
					this->components.at(this->components.size() - 1).components.push_back(component);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}
			}
		}

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customId A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		/// \returns void
		void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, __int32 maxValues, __int32 minValues) {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData componentData;
					componentData.type = ComponentType::SelectMenu;
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		/// \returns void
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		/// For setting the components in a response. \brief For setting the components in a response. 
		/// \param dataPackage An ActionRowData structure.
		/// \returns void
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		/// \returns void
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		/// \returns void
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		/// \returns void
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}

	protected:

		MessageReferenceData messageReference{};
		AllowedMentionsData allowedMentions{};
		vector<ActionRowData> components{};
		vector<string> stickerIds{};
		vector<EmbedData> embeds{};
		string requesterId{ "" };
		string channelId{ "" };
		string content{ "" };
		bool tts{ false };

	};

	/// Reply Message data. \brief Reply Message data.
	struct DiscordCoreAPI_Dll ReplyMessageData {

		friend class DiscordCoreInternal::MessageManagerAgent;
		friend class DiscordCoreInternal::MessageManager;
		friend class InputEvents;

		ReplyMessageData(InputEventData dataPackage) {
			this->messageReference.messageId = dataPackage.getMessageId();
			this->messageReference.channelId = dataPackage.getChannelId();
			this->messageReference.guildId = dataPackage.getGuildId();
			this->requesterId = dataPackage.getRequesterId();
			this->messageReference.failIfNotExists = false;
		}

		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customId A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.        
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		/// \returns void
		void addButton(bool disabled, string customId, string buttonLabel, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.type = ComponentType::Button;
					component.emoji.name = emojiName;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.id = emojiId;
					component.url = url;
					this->components.at(this->components.size() - 1).components.push_back(component);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}
			}
		}

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customId A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		/// \returns void
		void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, __int32 maxValues, __int32 minValues) {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData componentData;
					componentData.type = ComponentType::SelectMenu;
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		/// \returns void
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		/// For setting the components in a response. \brief For setting the components in a response. 
		/// \param dataPackage An ActionRowData structure.
		/// \returns void
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		/// \returns void
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		/// \returns void
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		/// \returns void
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}

	protected:
		MessageReferenceData messageReference{};
		AllowedMentionsData allowedMentions{};
		vector<ActionRowData> components{};
		vector<EmbedData> embeds{};
		string requesterId{ "" };
		string content{ "" };
		__int32 nonce{ 0 };
		bool tts{ false };

		ReplyMessageData() {};

	};

	/// Edit Message data. \brief Edit Message data.
	struct DiscordCoreAPI_Dll EditMessageData {

		friend class DiscordCoreInternal::MessageManagerAgent;
		friend class DiscordCoreInternal::MessageManager;
		friend class InputEvents;

		EditMessageData(InputEventData dataPackage) {
			this->requesterId = dataPackage.getRequesterId();
			this->channelId = dataPackage.getChannelId();
			this->messageId = dataPackage.getMessageId();
		}

		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customId A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.        
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		/// \returns void
		void addButton(bool disabled, string customId, string buttonLabel, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.type = ComponentType::Button;
					component.emoji.name = emojiName;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.id = emojiId;
					component.url = url;
					this->components.at(this->components.size() - 1).components.push_back(component);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}
			}
		}

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customId A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		/// \returns void
		void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, __int32 maxValues, __int32 minValues) {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData componentData;
					componentData.type = ComponentType::SelectMenu;
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		/// \returns void
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		/// For setting the components in a response. \brief For setting the components in a response. 
		/// \param dataPackage An ActionRowData structure.
		/// \returns void
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		/// \returns void
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		/// \returns void
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		/// \returns void
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}

	protected:

		AllowedMentionsData allowedMentions{};
		vector<AttachmentData> attachments{};
		vector<ActionRowData> components{};
		vector<EmbedData> embeds{};
		string requesterId{ "" };
		string channelId{ "" };
		string messageId{ "" };
		string content{ "" };
		__int32 flags{ 0 };
		bool tts{ false };

		EditMessageData() {};

	};

	/// Delete Message data. \brief Delete Message data.
	struct DiscordCoreAPI_Dll DeleteMessageData {
	public:
		 __int32 timeDelay{ 0 }; ///< Number of milliseconds to wait before deleting the Message.
		MessageData messageData;///< The Message to delete.
	};

	/// Delete Messages bulk data. \brief Delete Messages bulk data.
	struct DiscordCoreAPI_Dll DeleteMessagesBulkData {
		vector<string> messageIds{}; ///< Array of Message ids to delete.
		string channelId{ "" };///< Channel within which to delete the Messages.
	};

	/// Get pinned Messages data. \brief Get pinned Messages data.
	struct DiscordCoreAPI_Dll GetPinnedMessagesData {
		string channelId{ "" };///< The Channel from which to collect pinned Messages.
	};

	/// Pin Message data. \brief Pin Message data.
	struct DiscordCoreAPI_Dll PinMessageData {
		string channelId{ "" };///< The Channel within which to pin the Message.
		string messageId{ "" };///< The Message which you would like to pin.
	};

	/// Send DM data. \brief Send DM data.
	struct DiscordCoreAPI_Dll SendDMData {

		friend class DiscordCoreInternal::MessageManager;
		friend class InputEvents;

		SendDMData(RespondToInputEventData dataPackage) {
			this->requesterId = dataPackage.requesterId;
			this->channelId = dataPackage.channelId;
			this->embeds = dataPackage.embeds;
			this->allowedMentions = dataPackage.allowedMentions;
			this->content = dataPackage.content;
			this->components = dataPackage.components;
			this->tts = dataPackage.tts;
			this->userId = dataPackage.targetUserId;
		}

		SendDMData(InputEventData dataPackage, string targetUserId) {
			this->requesterId = dataPackage.getRequesterId();
			this->channelId = dataPackage.getChannelId();
			this->userId = targetUserId;
		}

		/// Adds a button to the response Message. \brief Adds a button to the response Message.
		/// \param disabled Whether the button is active or not.
		/// \param customId A custom id to give for identifying the button.
		/// \param buttonLabel A visible label for the button.
		/// \param buttonStyle The style of the button.
		/// \param emojiName An emoji name, if desired.        
		/// \param emojiId An emoji id, if desired.
		/// \param url A url, if applicable.
		/// \returns void
		void addButton(bool disabled, string customId, string buttonLabel, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData component;
					component.type = ComponentType::Button;
					component.emoji.name = emojiName;
					component.label = buttonLabel;
					component.style = buttonStyle;
					component.customId = customId;
					component.disabled = disabled;
					component.emoji.id = emojiId;
					component.url = url;
					this->components.at(this->components.size() - 1).components.push_back(component);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}
			}
		}

		/// Adds a select-menu to the response Message. \brief Adds a select-menu to the response Message.
		/// \param disabled Whether the select-menu is active or not.
		/// \param customId A custom id to give for identifying the select-menu.
		/// \param options A vector of select-menu-options to offer.
		/// \param placeholder Custom placeholder text if nothing is selected, max 100 characters.
		/// \param maxValues Maximum number of selections that are possible.
		/// \param minValues Minimum required number of selections that are required.
		/// \returns void
		void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, __int32 maxValues, __int32 minValues) {
			if (this->components.size() == 0) {
				ActionRowData actionRowData;
				this->components.push_back(actionRowData);
			}
			if (this->components.size() < 5) {
				if (this->components.at(this->components.size() - 1).components.size() < 5) {
					ComponentData componentData;
					componentData.type = ComponentType::SelectMenu;
					componentData.placeholder = placeholder;
					componentData.maxValues = maxValues;
					componentData.minValues = minValues;
					componentData.disabled = disabled;
					componentData.customId = customId;
					componentData.options = options;
					this->components.at(this->components.size() - 1).components.push_back(componentData);
				}
				else if (this->components.at(this->components.size() - 1).components.size() == 5) {
					ActionRowData actionRowData;
					this->components.push_back(actionRowData);
				}

			}
		}

		/// For setting the allowable mentions in a response. \brief For setting the allowable mentions in a response.
		/// \param dataPackage An AllowedMentionsData structure.
		/// \returns void
		void addAllowedMentions(AllowedMentionsData dataPackage) {
			this->allowedMentions = dataPackage;
		}

		/// For setting the components in a response. \brief For setting the components in a response. 
		/// \param dataPackage An ActionRowData structure.
		/// \returns void
		void addComponentRow(ActionRowData dataPackage) {
			this->components.push_back(dataPackage);
		}

		/// For setting the embeds in a response. \brief For setting the embeds in a response.
		/// \param dataPackage An EmbedData structure.
		/// \returns void
		void addMessageEmbed(EmbedData dataPackage) {
			this->embeds.push_back(dataPackage);
		}

		/// For setting the Message content in a response. \brief For setting the Message content in a response.
		/// \param dataPackage A string, containing the content.
		/// \returns void
		void addContent(string dataPackage) {
			this->content = dataPackage;
		}

		/// For setting the tts status of a response. \brief For setting the tts status of a response.
		/// \param enabledTTs A bool.
		/// \returns void
		void setTTSStatus(bool enabledTTs) {
			this->tts = enabledTTs;
		}

	protected:
		MessageReferenceData messageReference{};
		AllowedMentionsData allowedMentions{};
		vector<ActionRowData> components{};
		vector<EmbedData> embeds{};
		string requesterId{ "" };
		string channelId{ "" };
		string content{ "" };
		string userId{ "" };
		__int32 nonce{ 0 };
		bool tts{ false };		
	};

	/// A single Message. \brief A single Message.
	class DiscordCoreAPI_Dll Message : public MessageData {
	protected:

		friend struct Concurrency::details::_ResultHolder<Message>;
		friend class DiscordCoreInternal::MessageManagerAgent;
		friend class DiscordCoreInternal::MessageManager;
		friend struct OnMessageCreationData;
		friend struct OnMessageUpdateData;
		friend class DiscordCoreClient;

		Message();

		Message(MessageData dataNew);
	};
	/**@}*/

	/**
	* \addtogroup utilities
	* @{
	*/
	/// MessageCollectorReturn data. \brief MessageCollectorReturn data.
	struct DiscordCoreAPI_Dll MessageCollectorReturnData {
		vector<Message> messages; ///< A vector of collected Messages.
	};

	/// MessageCollector, for collecting Messages from a Channel. \brief Message collector, for collecting Messages from a Channel.
	class DiscordCoreAPI_Dll MessageCollector : DiscordCoreInternal::ThreadContext, agent {
	public:

		friend class DiscordCoreClient;
		 
		 MessageCollector() :
			ThreadContext(*DiscordCoreInternal::ThreadManager::getThreadContext().get()),
			agent(*DiscordCoreInternal::ThreadManager::getThreadContext().get()->scheduler->scheduler) {
			
		}

		/// Begin waiting for Messages. \brief Begin waiting for Messages.
		/// \param quantityToCollect Maximum quantity of Messages to collect before returning the results.
		/// \param msToCollectForNew Maximum number of milliseconds to wait for Messages before returning the results.
		/// \param userIdNew User id to set for possible comparison.
		/// \param filteringFunctionNew A filter function to apply to new Messages, where returning "true" from the function results in a Message being stored.
		/// \returns A task containing MessageCollectorReturnData.
		task<MessageCollectorReturnData>  collectMessages(__int32 quantityToCollect, __int32 msToCollectForNew, string userIdNew, function<bool(Message)> filteringFunctionNew) {
			this->quantityOfMessageToCollect = quantityToCollect;
			this->filteringFunction = filteringFunctionNew;
			this->msToCollectFor = msToCollectForNew;
			this->userId = userIdNew;
			this->messagesBuffer = new unbounded_buffer<Message>();
			MessageCollector::messagesBufferMap.insert_or_assign(this->userId, this->messagesBuffer);
			apartment_context mainThread{};
			co_await resume_background();
			this->start();
			wait(this);
			co_await mainThread;
			co_return this->messageReturnData;
		}

		 ~MessageCollector() {
			MessageCollector::messagesBufferMap.erase(this->userId);
		}

	protected:
		static map<string, unbounded_buffer<Message>*> messagesBufferMap;
		unbounded_buffer<Message>* messagesBuffer{ nullptr };
		function<bool(Message)> filteringFunction{ nullptr };
		MessageCollectorReturnData messageReturnData{};
		__int32 quantityOfMessageToCollect{ 0 };
		__int32 msToCollectFor{ 0 };
		__int32 startingTime{ 0 };
		__int32 elapsedTime{ 0 };
		string userId{ "" };

		void  run() {
			this->startingTime = (__int32)chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
			while (this->elapsedTime < this->msToCollectFor) {
				try {
					Message message = receive(this->messagesBuffer, this->msToCollectFor - this->elapsedTime);
					if (this->filteringFunction(message)) {
						this->messageReturnData.messages.push_back(message);
					}
					if (this->messageReturnData.messages.size() >= this->quantityOfMessageToCollect) {
						break;
					}
				}
				catch (operation_timed_out&) {};

				this->elapsedTime = (__int32)chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - this->startingTime;
			}
			done();
		}

	};
	/**@}*/
};

namespace DiscordCoreInternal {

	class DiscordCoreAPI_Dll MessageManagerAgent : agent {
	protected:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class MessageManager;

		static shared_ptr<ThreadContext> threadContext;

		unbounded_buffer<DeleteMessagesBulkData> requestDeleteMultMessagesBuffer{ nullptr };
		unbounded_buffer<vector<DiscordCoreAPI::Message>> outMultMessagesBuffer{ nullptr };
		unbounded_buffer<GetPinnedMessagesData> requestGetPinnedMessagesBuffer{ nullptr };
		unbounded_buffer<DeleteMessageData> requestDeleteMessageBuffer{ nullptr };
		unbounded_buffer<PutPinMessageData> requestPutPinMessageBuffer{ nullptr };
		unbounded_buffer<PatchMessageData> requestPatchMessageBuffer{ nullptr };
		unbounded_buffer<DiscordCoreAPI::Message> outMessageBuffer{ nullptr };
		unbounded_buffer<PostMessageData> requestPostMessageBuffer{ nullptr };
		unbounded_buffer<GetMessagesData> requestGetMessagesBuffer{ nullptr };
		unbounded_buffer<GetMessageData> requestGetMessageBuffer{ nullptr };
		unbounded_buffer<PostDMData> requestPostDMMessageBuffer{ nullptr };

		MessageManagerAgent();

		static void initialize();

		static void cleanup();

		DiscordCoreAPI::Message getObjectData(GetMessageData dataPackage);

		vector<DiscordCoreAPI::Message> getObjectData(GetMessagesData dataPackage);

		vector<DiscordCoreAPI::Message> getObjectData(GetPinnedMessagesData dataPackage);

		DiscordCoreAPI::Message patchObjectData(PatchMessageData dataPackage);

		DiscordCoreAPI::Message postObjectData(PostMessageData dataPackage);

		DiscordCoreAPI::Message postObjectData(PostDMData dataPackage);

		void postObjectData(DeleteMessagesBulkData dataPackage);

		void putObjectData(PutPinMessageData dataPackage);

		void onDeleteData(DeleteMessageData dataPackage);

		void deleteObjectData(DeleteMessageData dataPackage);

		void run();
	};

	class DiscordCoreAPI_Dll MessageManager : ThreadContext {
	public:

		friend class DiscordCoreAPI::DiscordCoreClient;
		friend class DiscordCoreAPI::InputEvents;
		friend class DiscordCoreAPI::Messages;

		MessageManager(MessageManager* pointer);

		~MessageManager();

	protected:

		task<std::optional<vector<DiscordCoreAPI::Message>>> getMessagesAsync(DiscordCoreAPI::GetMessagesData dataPackage);

		task<DiscordCoreAPI::Message> getMessageAsync(DiscordCoreAPI::GetMessageData dataPackage);

		task<DiscordCoreAPI::Message> createMessageAsync(DiscordCoreAPI::CreateMessageData dataPackage);

		task<DiscordCoreAPI::Message> replyAsync(DiscordCoreAPI::ReplyMessageData dataPackage);

		task<DiscordCoreAPI::Message> editMessageAsync(DiscordCoreAPI::EditMessageData dataPackage);

		task<void> deleteMessageAsync(DiscordCoreAPI::DeleteMessageData dataPackage);

		task<void> deleteMessagesBulkAsync(DiscordCoreAPI::DeleteMessagesBulkData dataPackage);

		task<vector<DiscordCoreAPI::Message>> getPinnedMessagesAsync(DiscordCoreAPI::GetPinnedMessagesData dataPackage);

		task<void> pinMessageAsync(DiscordCoreAPI::PinMessageData dataPackage);

		task<DiscordCoreAPI::Message> sendDMAsync(DiscordCoreAPI::SendDMData dataPackage);
	};
}
#endif