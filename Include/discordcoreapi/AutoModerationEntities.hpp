/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// AutoModerationEntities.hpp - Header for the Auto-Moderation related classes and structs.
/// Jun 17, 2022 Chris M.
/// https://discordcoreapi.com
/// \file AutoModerationEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For listing all of the auto-moderation-rules for a particular Guild .
	struct ListAutoModerationRulesForGuildData {
		Snowflake guildId{};///< The id of the Guild for which you would like to list the auto-moderation rules.
	};

	/// @brief For collecting an auto-moderation-rule for a particular AutoModerationRuleData.
	struct GetAutoModerationRuleData {
		uint64_t autoModerationRuleId{};///< The id of the auto-moderation-rule you would like to collect.
		Snowflake guildId{};///< The id of the Guild from which you would like to collect the auto-moderation-rule from.
	};

	/// @brief For creating an auto-moderation-rule.
	struct CreateAutoModerationRuleData {
		std::vector<uint64_t> exemptChannels{};///< The channel ids that should not be affected by the rule(Maximum of 50).
		std::vector<uint64_t> exemptRoles{};///< The role ids that should not be affected by the rule(Maximum of 20).
		TriggerMetaData triggerMetadata{};///< The trigger metadata.
		std::vector<ActionData> actions{};///< The actions which will execute when the rule is triggered
		TriggerType triggerType{};///< The trigger type.
		EventType eventType{};///< The event type.
		Snowflake guildId{};///< The Guild within which to create the AutoModerationRuleData.
		std::string name{};///< The rule name.
		bool enabled{};///< Whether the rule is enabled(False by default).
	};

	/// @brief For modifying an auto-moderation-rule.
	struct ModifyAutoModerationRuleData {
		std::vector<uint64_t> exemptChannels{};///< The channel ids that should not be affected by the rule(Maximum of 50).
		std::vector<uint64_t> exemptRoles{};///< The role ids that should not be affected by the rule(Maximum of 20).
		TriggerMetaData triggerMetadata{};///< The trigger metadata.
		std::vector<ActionData> actions{};///< The actions which will execute when the rule is triggered
		uint64_t autoModerationRuleId{};///< The id of the auto-moderation-rule you would like to modify.
		EventType eventType{};///< The event type.
		Snowflake guildId{};///< The AutoModerationRuleData within which to modify the auto-moderation-rule.
		std::string name{};///< The rule name.
		bool enabled{};///< Whether the rule is enabled(False by default).
	};

	/// @brief For when an auto-moderation-rule is executed.
	struct AutoModerationActionExecutionEventData {
		Snowflake alertSystemMessageId{};///< The id of any system auto moderation messages posted as a result of this action.
		TriggerType ruleTriggerType{};///< The trigger type of rule which was triggered.
		std::string matchedKeyword{};///< The word or phrase configured in the rule that triggered the rule
		std::string matchedContent{};///< The substring in content that triggered the rule.
		std::string content{};///< The user generated text content.
		Snowflake channelId{};///< The id of the channel in which user content was posted.
		Snowflake messageId{};///< The id of any user message which content belongs to.
		ActionData action{};///< The action which was executed.
		Snowflake guildId{};///< The id of the guild in which action was executed.
		Snowflake ruleId{};///< The id of the rule which action belongs to.
		Snowflake userId{};///< The id of the user which generated the content which triggered the rule.

		AutoModerationActionExecutionEventData() = default;

		virtual ~AutoModerationActionExecutionEventData() = default;
	};

	/// @brief For deleting an auto-moderation-rule.
	struct DeleteAutoModerationRuleData {
		uint64_t autoModerationRuleId{};///< The id of the auto-moderation-rule you would like to delete.
		Snowflake guildId{};///< Guild within which to delete the auto-moderation-rule.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the AutoModerationRuleData related Discord endpoints.
	class DiscordCoreAPI_Dll AutoModerationRules {
	  public:
		static void initialize(DiscordCoreInternal::HttpsClient*);

		/// @brief Get all of the Guild's Auto-Moderation-Rules.
		/// @param dataPackage The moderation rule data to get.
		/// @return A CoRoutine containing a vector<AutoModerationRuleData>.
		CoRoutine<std::vector<AutoModerationRuleData>> listAutoModerationRulesForGuildAsync(ListAutoModerationRulesForGuildData dataPackage);

		/// @brief Get a particular Auto-Moderation-Rule
		/// @param dataPackage The moderation rule data to get.
		/// @return A CoRoutine containing a AutoModerationRuleData.
		CoRoutine<AutoModerationRuleData> getAutoModerationRuleAsync(GetAutoModerationRuleData dataPackage);

		/// @brief Create a particular Auto-Moderation-Rule
		/// @param dataPackage The moderation rule data to create.
		/// @return A CoRoutine containing a AutoModerationRuleData.
		CoRoutine<AutoModerationRuleData> createAutoModerationRuleAsync(CreateAutoModerationRuleData dataPackage);

		/// @brief Modify a particular Auto-Moderation-Rule
		/// @param dataPackage The moderation rule data to apply.
		/// @return A CoRoutine containing a AutoModerationRuleData.
		CoRoutine<AutoModerationRuleData> modifyAutoModerationRuleAsync(ModifyAutoModerationRuleData dataPackage);

		/// @brief Delete a particular Auto-Moderation-Rule
		/// @param dataPackage The moderation rule data to delete.
		/// @return A CoRoutine containing a void.
		CoRoutine<void> deleteAutoModerationRuleAsync(DeleteAutoModerationRuleData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpsClient* httpsClient;
	};

	/**@}*/

}// namespace DiscordCoreInternal
