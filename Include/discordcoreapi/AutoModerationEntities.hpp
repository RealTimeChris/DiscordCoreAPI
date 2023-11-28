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
/// AutoModerationEntities.hpp - Header for the auto-moderation related classes and structs.
/// Jun 17, 2022 Chris M.
/// https://discordcoreapi.com
/// \file AutoModerationEntities.hpp
#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Utilities/HttpsClient.hpp>

namespace discord_core_api {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */

	/// @brief For listing all of the auto-moderation-rules for a particular guild .
	struct list_auto_moderation_rules_for_guild_data {
		snowflake guildId{};///< The id of the guild for which you would like to list the auto-moderation rules.
	};

	/// @brief For collecting an auto-moderation-rule for a particular auto_moderation_rule_data.
	struct get_auto_moderation_rule_data {
		snowflake autoModerationRuleId{};///< The id of the auto-moderation-rule you would like to collect.
		snowflake guildId{};///< The id of the guild from which you would like to collect the auto-moderation-rule from.
	};

	/// @brief For creating an auto-moderation-rule.
	struct create_auto_moderation_rule_data {
		jsonifier::vector<snowflake> exemptChannels{};///< The channel ids that should not be affected by the rule(maximum of 50).
		jsonifier::vector<snowflake> exemptRoles{};///< The role ids that should not be affected by the rule(maximum of 20).
		trigger_meta_data triggerMetadata{};///< The trigger metadata.
		jsonifier::vector<action_data> actions{};///< The actions which will execute when the rule is triggered
		trigger_type triggerType{};///< The trigger type.
		event_type eventType{};///< The event type.
		snowflake guildId{};///< The guild within which to create the auto_moderation_rule_data.
		jsonifier::string name{};///< The rule name.
		bool enabled{};///< Whether the rule is enabled(false by default).
	};

	/// @brief For modifying an auto-moderation-rule.
	struct modify_auto_moderation_rule_data {
		jsonifier::vector<snowflake> exemptChannels{};///< The channel ids that should not be affected by the rule(maximum of 50).
		jsonifier::vector<snowflake> exemptRoles{};///< The role ids that should not be affected by the rule(maximum of 20).
		trigger_meta_data triggerMetadata{};///< The trigger metadata.
		jsonifier::vector<action_data> actions{};///< The actions which will execute when the rule is triggered
		snowflake autoModerationRuleId{};///< The id of the auto-moderation-rule you would like to modify.
		event_type eventType{};///< The event type.
		snowflake guildId{};///< The auto_moderation_rule_data within which to modify the auto-moderation-rule.
		jsonifier::string name{};///< The rule name.
		bool enabled{};///< Whether the rule is enabled(false by default).
	};

	/// @brief For when an auto-moderation-rule is executed.
	struct auto_moderation_action_execution_event_data {
		snowflake alertSystemMessageId{};///< The id of any system auto moderation messages posted as a result of this action.
		trigger_type ruleTriggerType{};///< The trigger type of rule which was triggered.
		jsonifier::string matchedKeyword{};///< The word or phrase configured in the rule that triggered the rule
		jsonifier::string matchedContent{};///< The substring in content that triggered the rule.
		jsonifier::string content{};///< The user generated text content.
		snowflake channelId{};///< The id of the channel in which user content was posted.
		snowflake messageId{};///< The id of any user message which content belongs to.
		action_data action{};///< The action which was executed.
		snowflake guildId{};///< The id of the guild in which action was executed.
		snowflake ruleId{};///< The id of the rule which action belongs to.
		snowflake userId{};///< The id of the user which generated the content which triggered the rule.

		auto_moderation_action_execution_event_data() = default;
	};

	/// @brief For deleting an auto-moderation-rule.
	struct delete_auto_moderation_rule_data {
		snowflake autoModerationRuleId{};///< The id of the auto-moderation-rule you would like to delete.
		snowflake guildId{};///< Guild within which to delete the auto-moderation-rule.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */
	/// @brief An interface class for the auto_moderation_rule_data related discord endpoints.
	class DiscordCoreAPI_Dll auto_moderation_rules {
	  public:
		static void initialize(discord_core_internal::https_client*);

		/// @brief Get all of the guild's auto-moderation-rules.
		/// @param dataPackage the moderation rule data to get.
		/// @return a co_routine containing a vector<auto_moderation_rule_data>.
		co_routine<jsonifier::vector<auto_moderation_rule_data>> listAutoModerationRulesForGuildAsync(list_auto_moderation_rules_for_guild_data dataPackage);

		/// @brief Get a particular auto-moderation-rule
		/// @param dataPackage the moderation rule data to get.
		/// @return a co_routine containing a auto_moderation_rule_data.
		co_routine<auto_moderation_rule_data> getAutoModerationRuleAsync(get_auto_moderation_rule_data dataPackage);

		/// @brief Create a particular auto-moderation-rule
		/// @param dataPackage the moderation rule data to create.
		/// @return a co_routine containing a auto_moderation_rule_data.
		co_routine<auto_moderation_rule_data> createAutoModerationRuleAsync(create_auto_moderation_rule_data dataPackage);

		/// @brief Modify a particular auto-moderation-rule
		/// @param dataPackage the moderation rule data to apply.
		/// @return a co_routine containing a auto_moderation_rule_data.
		co_routine<auto_moderation_rule_data> modifyAutoModerationRuleAsync(modify_auto_moderation_rule_data dataPackage);

		/// @brief Delete a particular auto-moderation-rule
		/// @param dataPackage the moderation rule data to delete.
		/// @return a co_routine containing a void.
		co_routine<void> deleteAutoModerationRuleAsync(delete_auto_moderation_rule_data dataPackage);

	  protected:
		static discord_core_internal::https_client* httpsClient;
	};

	/**@}*/

}// namespace discord_core_internal
