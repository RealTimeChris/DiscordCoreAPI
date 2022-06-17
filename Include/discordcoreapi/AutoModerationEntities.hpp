/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// AutoModerationEntities.hpp - Header for the Auto-Moderation related classes and structs. 
/// Jun 17, 2022 Chris M.
/// https://discordcoreapi.com
/// \file AutoModerationEntities.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/CoRoutine.hpp>
#include <discordcoreapi/Http.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup foundation_entities
	 * @{
	 */
	
	/// Event types for auto-moderation. \brief Event types for auto-moderation.
	enum class EventType {
		Message_Send = 1,///< When a member sends or edits a message in the guild.
	};

	/// Trigger types for auto-moderation. \brief Trigger types for auto-moderation.
	enum class TriggerType {
		Keyword = 1,///< Check if content contains words from a user defined list of keywords.
		Harmful_Link = 2,///< Check if content contains any harmful links.
		Spam = 3,///< Check if content represents generic spam.
		Keyword_Preset = 4///< Check if content contains words from internal pre-defined wordsets.
	};

	/// Keyword preset types for auto-moderation. \brief Keyword preset types for auto-moderation.
	enum class KeywordPresetType {
		Profanity = 1,///< Words that may be considered forms of swearing or cursing.
		Sexual_Content = 2,///< Words that refer to sexually explicit behavior or activity
		Slurs = 3///< Personal insults or words that may be considered hate speech.
	};

	/// Action types for auto-moderation. \brief Action types for auto-moderation.
	enum class ActionType {
		Block_Message = 1,///< Blocks the content of a message according to the rule.
		Send_Alert_Message = 2,///< Logs user content to a specified channel.
		Timeout = 3///< Timeout user for a specified duration.
	};

	/// Action metadata for auto-moderation-rules. \brief Action metadata for auto-moderation-rules.
	struct ActionMetaData {
		uint64_t channelId{};///< Channel to which user content should be logged.
		int64_t durationSeconds{};///< Timeout duration in seconds.
	};

	/// Trigger metadata for auto-moderation-rules. \brief Trigger metadata for auto-moderation-rules.
	struct TriggerMetaData {
		std::vector<std::string> keywordFilter{};///< Substrings which will be searched for in content.
		std::vector<KeywordPresetType> presets{};///< The internally pre-defined wordsets which will be searched for in content.
	};

	/// Represents an auto-moderation-rule. \brief Represents an auto-moderation-rule.
	struct AutoModerationRule : public DiscordEntity {
		std::vector<uint64_t> exemptChannels{};///< The channel ids that should not be affected by the rule(Maximum of 50).
		std::vector<ActionMetaData> actions{};///< Actions which will execute when the rule is triggered.
		std::vector<uint64_t> exemptRoles{};///< The role ids that should not be affected by the rule(Maximum of 20).
		TriggerMetaData triggerMetaData{};///< The rule trigger metadata actions array of action objects the.
		TriggerType triggerType{};///< The rule trigger type.
		EventType eventType{};///< The rule event type.
		uint64_t creatorId{};///< The user which first created this rule.
		uint64_t guildId{};///< The guild which this rule belongs to.
		std::string name{};///< The rule name.
		bool enabled{};///< Whether the rule is enabled.		
	};

	/// For listing all of the auto-moderation-rules for a particular Guild. \brief For listing all of the auto-moderation-rules for a particular Guild.
	struct ListAutoModerationRulesForGuildData {
		uint64_t guildId{};///< The id of the guild for which you would like to list the auto-moderation rules.
	};

	/// For collecting an auto-moderation-rule for a particular Guild. \brief For collecting an auto-moderation-rule for a particular Guild.
	struct GetAutoModerationRuleData {
		uint64_t guildId{};///< The id of the Guild from which you would like to collect the auto-moderation-rule from.
		uint64_t autoModerationRuleId{};///< The id of the auto-moderation-rule you would like to collect.
	};

	/**@}*/

	/**
	 * \addtogroup main_endpoints
	 * @{
	 */

	class AutoModerationRules {
	  public:

		static void initialize(DiscordCoreInternal::HttpClient*);

		CoRoutine<std::vector<AutoModerationRule>> listAutoModerationRulesForGuildAsync(ListAutoModerationRulesForGuildData dataPackage);

		CoRoutine<AutoModerationRule> getAutoModerationRuleAsync(GetAutoModerationRuleData dataPackage);

	  protected:
		static DiscordCoreInternal::HttpClient* httpClient;
	};

	/**@}*/

}// namespace DiscordCoreInternal
