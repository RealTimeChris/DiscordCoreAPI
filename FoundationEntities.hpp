// FoundationEntities.hpp - Header for all of the Discord/Support API data structures.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _FOUNDATION_ENTITIES_
#define _FOUNDATION_ENTITIES_

#include "../pch.h"

namespace DiscordCoreAPI {
    class DiscordCoreClient;

    string convertToLowerCase(string stringToConvert) {
        string newString;
        for (auto& value : stringToConvert) {
            if (isupper(value)) {
                newString += (char)tolower(value);
            }
            else {
                newString += value;
            }
        }
        return newString;
    }

    string convertTimeInMsToDateTimeString(__int64 timeInMs) {
        __int64 timeValue = timeInMs / 1000;
        __time64_t rawTime(timeValue);
        tm timeInfo;
        char timeBuffer[32];
        errno_t error;
        error = localtime_s(&timeInfo, &rawTime);
        if (error)
        {
            printf("Invalid argument to _localtime64_s.");
        }
        strftime(timeBuffer, 32, "%a %b %d %Y %X", &timeInfo);
        return timeBuffer;
    }

    string convertSnowFlakeToDateTimeString(string snowFlake) {
        string returnString;
        __int64 timeInMs = (stoll(snowFlake) >> 22) + 1420070400000;
        returnString = convertTimeInMsToDateTimeString(timeInMs);
        return returnString;
    }

    class StopWatch {
    public:
        StopWatch(unsigned long long maxNumberOfMsNew) {
            this->maxNumberOfMs = maxNumberOfMsNew;
            this->startTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        }

        bool hasTimePassed() {
            unsigned long long currentTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
            unsigned long long elapsedTime = currentTime - this->startTime;
            if (elapsedTime >= this->maxNumberOfMs) {
                return true;
            }
            else {
                return false;
            }
        }

        void resetTimer() {
            this->startTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        }
    protected:
        unsigned long long maxNumberOfMs;
        unsigned long long startTime;
    };

    long long convertTimestampToInteger(string timeStamp) {
        CTime timeValue = CTime::CTime(stoi(timeStamp.substr(0, 4)), stoi(timeStamp.substr(5, 6)), stoi(timeStamp.substr(8, 9)),
            stoi(timeStamp.substr(11, 12)), stoi(timeStamp.substr(14, 15)), stoi(timeStamp.substr(17, 18)));
        return timeValue.GetTime();
    }

    string convertTimeStampToNewOne(string timeStamp) {
        long long timeInMs = convertTimestampToInteger(timeStamp) * 1000;
        string returnString = convertTimeInMsToDateTimeString(timeInMs);
        return returnString;
    }

    bool hasTimeElapsed(string timeStamp, long long days = 0, long long hours = 0, long long minutes = 0) {
        long long startTime = convertTimestampToInteger(timeStamp);
        long long currentTime = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();
        long long secondsPerMinute = 60;
        long long secondsPerHour = secondsPerMinute * 60;
        long long secondsPerDay = secondsPerHour * 24;
        long long targetElapsedTime = (days * secondsPerDay) + (hours * secondsPerHour) + (minutes * secondsPerMinute);
        long long actualElapsedTime = currentTime - startTime;
        if (actualElapsedTime < 0) {
            actualElapsedTime = 0;
        }
        if (actualElapsedTime >= targetElapsedTime) {
            return true;
        }
        else {
            return false;
        }
    }

    class GuildMember;
    struct GuildMemberData;

};

namespace  DiscordCoreInternal {

    class ThreadManager;

    struct AllowedMentionsData {
        vector<string> parse;
        vector<string> roles;
        vector<string> users;
        bool repliedUser{ false };
    };

    struct MessageReferenceData {
        string messageId;
        string channelId;
        string guildId;
        bool failIfNotExists{ false };
    };

    struct AttachmentData {
        string id;
        string filename;
        string contentType;
        int size{ 0 };
        string url;
        string proxyUrl;
        int height{ 0 };
        int width{ 0 };
    };

    struct EmbedFooterData {
        string iconUrl;
        string text;
        string proxyIconUrl;
    };

    struct EmbedImageData {
        string url;
        string proxyUrl;
        int height{ 0 };
        int width{ 0 };
    };

    struct EmbedThumbnailData {
        string url;
        string proxyUrl;
        int height{ 0 };
        int width{ 0 };
    };

    struct EmbedVideoData {
        string url;
        string proxyUrl;
        int height{ 0 };
        int width{ 0 };
    };

    struct EmbedProviderData {
        string name;
        string url;
    };

    struct EmbedAuthorData {
        string name;
        string url;
        string iconUrl;
        string proxyIconUrl;
    };

    struct EmbedFieldData {
        string name;
        string value;
        bool Inline{ false };
    };

    struct EmbedData {
        EmbedData setTitle(string titleNew) {
            this->title = titleNew;
            return *this;
        }
        EmbedData setAuthor(string authorName, string authorAvatarURL = "") {
            this->author.name = authorName;
            this->author.iconUrl = authorAvatarURL;
            return *this;
        }
        EmbedData setImage(string imageURL) {
            this->image.url = imageURL;
            return *this;
        }
        EmbedData setThumbnail(string thumbnailURL) {
            this->thumbnail.url = thumbnailURL;
            return *this;
        }
        EmbedData setColor(string hexColorValueNew) {
            this->hexColorValue = hexColorValueNew;
            return *this;
        }
        EmbedData setDescription(string descriptionNew) {
            this->description = descriptionNew;
            return *this;
        }
        EmbedData setFooter(string footerText, string footerIconURLText = "") {
            this->footer.text = footerText;
            this->footer.iconUrl = footerIconURLText;
            return *this;
        }
        EmbedData setTimeStamp(string timeStamp) {
            this->timestamp = timeStamp;
            this->timestampRaw = DiscordCoreAPI::convertTimeStampToNewOne(timeStamp);
            return *this;
        }
        EmbedData addField(string name, string value, bool Inline = true) {
            EmbedFieldData embedFieldData;
            embedFieldData.name = name;
            embedFieldData.Inline = Inline;
            embedFieldData.value = value;
            this->fields.push_back(embedFieldData);
            return *this;
        }
        string title;
        string type;
        string description;
        string url;
        string timestamp;
        string hexColorValue;
        EmbedFooterData footer;
        EmbedImageData image;
        EmbedThumbnailData thumbnail;
        string timestampRaw;
        EmbedVideoData video;
        EmbedProviderData provider;
        EmbedAuthorData author;
        vector<EmbedFieldData> fields;
    };

    enum class Permissions :__int64 {
        CREATE_INSTANT_INVITE = (1 << 0),
        KICK_MEMBERS = (1 << 1),
        BAN_MEMBERS = (1 << 2),
        ADMINISTRATOR = (1 << 3),
        MANAGE_CHANNELS = (1 << 4),
        MANAGE_GUILD = (1 << 5),
        ADD_REACTIONS = (1 << 6),
        VIEW_AUDIT_LOG = (1 << 6),
        PRIORITY_SPEAKER = (1 << 8),
        STREAM = (1 << 9),
        VIEW_CHANNEL = (1 << 10),
        SEND_MESSAGES = (1 << 11),
        SEND_TTS_MESSAGES = (1 << 12),
        MANAGE_MESSAGES = (1 << 13),
        EMBED_LINKS = (1 << 14),
        ATTACH_FILES = (1 << 15),
        READ_MESSAGE_HISTORY = (1 << 16),
        MENTION_EVERYONE = (1 << 17),
        USE_EXTERNAL_EMOJIS = (1 << 18),
        VIEW_GUILD_INSIGHTS = (1 << 19),
        CONNECT = (1 << 20),
        SPEAK = (1 << 21),
        MUTE_MEMBERS = (1 << 22),
        DEAFEN_MEMBERS = (1 << 23),
        MOVE_MEMBERS = (1 << 24),
        USE_VAD = (1 << 25),
        CHANGE_NICKNAME = (1 << 26),
        MANAGE_NICKNAMES = (1 << 27),
        MANAGE_ROLES = (1 << 28),
        MANAGE_WEBHOOKS = (1 << 29),
        MANAGE_EMOJIS = (1 << 30),
        USE_SLASH_COMMANDS = (1ull << 31),
        REQUEST_TO_SPEAK = (1ull << 32),
        MANAGE_THREADS = (1ull << 33),
        USE_PUBLIC_THREADS = (1ull << 34),
        USE_PRIVATE_THREADS = (1ull << 35)
    };

    enum class UserFlags {
        NONE = 0,
        DISCORD_EMPLOYEE = 1 << 0,
        PARTNERED_SERVER_OWNER = 1 << 1,
        HYPESQUAD_EVENTS = 1 << 2,
        BUG_HUNTER_LEVEL_1 = 1 << 3,
        HOUSE_BRAVERY = 1 << 6,
        HOUSE_BRILLIANCE = 1 << 7,
        HOUSE_BALANCE = 1 << 8,
        EARLY_SUPPORTER = 1 << 9,
        TEAM_USER = 1 << 10,
        BUG_HUNTER_LEVEL_2 = 1 << 14,
        VERIFIED_BOT = 1 << 16,
        EARLY_VERIFIED_BOT_DEVELOPER = 1 << 17
    };

    enum class DefaultMessageNotificationLevel {
        ALL_MESSAGES = 0,
        ONLY_MENTIONS = 1
    };

    enum class ExplicitContentFilterLevel {
        DISABLED = 0,
        MEMBERS_WITHOUT_ROLES = 1,
        ALL_MEMBERS = 2
    };

    enum class MFALevel {
        NONE = 0,
        ELEVATED = 1
    };

    enum class VerificationLevel {
        NONE = 0,
        LOW = 1,
        MEDIUM = 2,
        HIGH = 3,
        VERY_HIGH = 4
    };

    enum class PremiumTier {
        NONE = 0,
        TIER_2 = 2,
        TIER_1 = 1,
        TIER_3 = 3
    };

    enum class NitroSubscriptionLevel {
        NONE = 0,
        NITRO_CLASSIC = 1,
        NITRO = 2
    };

    enum class InteractionCallbackType {
        Pong = 1,
        Acknowledge = 2,
        ChannelMessage = 3,
        ChannelMessageWithSource = 4,
        DeferredChannelMessageWithSource = 5,
        DeferredUpdateMessage = 6,
        UpdateMessage = 7
    };

    enum class ChannelType {
        GUILD_TEXT = 0,
        DM = 1,
        GUILD_VOICE = 2,
        GROUP_DM = 3,
        GUILD_CATEGORY = 4,
        GUILD_NEWS = 5,
        GUILD_STORE = 6,
        GUILD_NEWS_THREAD = 10,
        GUILD_PUBLIC_THREAD = 11,
        GUILD_PRIVATE_THREAD = 12,
        GUILD_STAGE_VOICE = 13
    };

    struct UserData {
        string username;
        string id;
        string discriminator;
        string avatar;
        bool bot{ false };
        bool system{ false };
        bool mfaEnabled{ false };
        string locale;
        bool verified{ false };
        string email;
        int flags{ 0 };
        int premiumType{ 0 };
        int publicFlags{ 0 };
        string createdAt;
    };

    struct RoleTagsData {
        string botId;
        string integrationId;
        string premiumSubscriber;
    };

    struct RoleData {
        string id;
        string name;
        int color{ 0 };
        bool hoist{ false };
        int position{ 0 };
        string permissions;
        bool managed{ false };
        bool mentionable{ false };
        RoleTagsData tags;
    };

    struct DiscordCoreAPI::GuildMemberData;

    struct GuildMemberData {
        string guildId;
        UserData user;
        string nick;
        vector<string> roles;
        string joinedAt;
        string premiumSince;
        bool deaf{ false };
        bool mute{ false };
        bool pending{ false };
        string permissions;
        string userMention;        
    };

    struct WelcomeScreenChannelData {
        string channelId;
        string description;
        string emojiId;
        string emojiName;
    };

    struct WelcomeScreenData {
        string description;
        vector<WelcomeScreenChannelData> welcomeChannels{};
    };

    struct PartyData {
        string id;
        vector<int> size{ 0, 0 };
    };

    struct AssetsData {
        string largeImage;
        string largeText;
        string smallImage;
        string smallText;
    };

    struct SecretsData {
        string join;
        string spectate;
        string match;
    };

    struct TimestampData {
        __int64 start = 0;
        __int64 end = 0;
    };

    struct ButtonData {
        string label;
        string url;
    };

    struct EmojiData {
        string id{ "" };
        string name{ "" };
        vector<RoleData> roles;
        UserData user;
        bool requireColons{ false };
        bool managed{ false };
        bool animated{ false };
        bool available{ false };
    };

    struct ReactionData {
        int count;
        bool me;
        EmojiData emoji;
        string userId;
        string channelId;
        string messageId;
        string guildId;
        GuildMemberData member;
    };

    struct ActivityData {
        int createdAt = 0;
        TimestampData timestamps;
        string applicationId = "";
        string details = "";
        string state = "";
        EmojiData emoji;
        PartyData party;
        AssetsData assets;
        SecretsData secrets;
        bool instance = false;
        int flags = 0;
        string name = "";
        int type;
        string url = "";
        ButtonData buttons;
    };

    struct ClientStatusData {
        string desktop;
        string mobile;
        string web;
    };

    struct UpdatePresenceData {
        __int64 since;
        vector<ActivityData> activities;
        string status;
        bool afk;
    };

    struct PresenceUpdateData {
        UserData user;
        string guildId;
        string status;
        vector<ActivityData> activities;
        ClientStatusData clientStatus;
    };

    enum class EditChannelPermissionOverwritesType {
        Role = 0,
        User = 1
    };

    struct OverWriteData {
        string id;
        EditChannelPermissionOverwritesType type = EditChannelPermissionOverwritesType::Role;	           //	either 0 (role) or 1 (member)
        string allow;
        string deny;
        string channelId;
    };

    struct ChannelData {
        string id;
        ChannelType type = ChannelType::DM;
        string guildId;
        int position = -1;
        map<string, OverWriteData> permissionOverwrites;
        string name;
        string topic;
        bool nsfw{ false };
        string lastMessageId;
        int bitrate = -1;
        int userLimit = -1;
        int rateLimitPerUser = -1;
        vector<UserData> recipients;
        string icon;
        string ownerId;
        string applicationId;
        string parentId;
        string lastPinTimestamp;
        string rtcRegion;
        int videoQualityMode = -1;
    };

    struct VoiceStateData {
        string guildId;
        string channelId;
        string userId;
        GuildMemberData guildMember;
        string sessionId;
        bool deaf;
        bool mute;
        bool selfDeaf;
        bool selfMute;
        bool selfStream;
        bool selfVideo;
        bool suppress;
        string requestToSpeakTimestamp;
    };

    struct GuildData {
        string icon;
        string name;
        string id;
        string iconHash;
        string splash;
        string discoverySplash;
        string preferredLocale;
        string publicUpdatesChannelID;
        string vanityURLCode;
        string description;
        string banner;
        string ruleChannelID;
        string applicationID;
        string joinedAt;
        string widgetChannelID;
        string systemChannelID;
        string region;
        string afkChannelID;
        string ownerID;
        vector<string> features{};
        string permissions;
        bool owner{ false };
        int afkTimeOut{ 0 };
        bool widgetEnabled{ false };
        VerificationLevel verificationLevel = VerificationLevel::NONE;
        DefaultMessageNotificationLevel defaultMessageNotifications = DefaultMessageNotificationLevel::ALL_MESSAGES;
        ExplicitContentFilterLevel explicitContentFilter = ExplicitContentFilterLevel::DISABLED;
        vector<EmojiData> emoji{};
        map<string, RoleData> roles{};
        int systemChannelFlags{ 0 };
        MFALevel mfaLevel = MFALevel::NONE;
        bool large{ false };
        bool unavailable{ false };
        int memberCount{ 0 };
        vector<VoiceStateData> voiceStates{};
        vector<PresenceUpdateData> presences{};
        int maxPresences{ 0 };
        int maxMembers{ 0 };
        int premiumSubscriptionCount{ 0 };
        PremiumTier premiumTier = PremiumTier::NONE;
        int maxVideoChannelUsers{ 0 };
        int approximateMemberCount{ 0 };
        int approximatePresenceCount{ 0 };
        WelcomeScreenData welcomeScreen{};
        vector<GuildMemberData> members{};
        vector<ChannelData> channels{};
    };

    struct ThreadContext {
    public:
        ThreadContext(Scheduler* newScheduler) {
            this->scheduler = newScheduler;
        }
        ThreadContext(ThreadContext* threadContext) {
            this->scheduler = threadContext->scheduler;
            this->dispatcherQueue = threadContext->dispatcherQueue;
            this->schedulerGroup = threadContext->schedulerGroup;
        }

        void releaseGroup() {
            if (this->schedulerGroup != nullptr) {
                this->schedulerGroup->Release();
                this->schedulerGroup = nullptr;
            }
        };

        Scheduler* scheduler{ nullptr };
        ScheduleGroup* schedulerGroup{ nullptr };
        shared_ptr<DispatcherQueue> dispatcherQueue{ nullptr };
    };

    struct HttpAgentResources {
        string baseURL = "";
        hstring userAgent = L"";
    };

    struct DeleteInteractionResponseData {
        HttpAgentResources agentResources;
        string applicationId;
        string interactionToken;
        unsigned int timeDelayInMs;
    };

    struct DeleteFollowUpMessageData {
        HttpAgentResources agentResources;
        string applicationId;
        string interactionToken;
        string messageId;
        unsigned int timeDelayInMs;
    };

    struct ChannelMentionData {
        string id;
        string guildId;
        int type = -1;
        string name;
    };

    struct MessageActivityData {
        int type = -1;
        string partyId;
    };

    struct TeamMembersObjectData {
        int membershipState = -1;
        vector<string> permissions;
        string teamId;
        UserData user;
    };

    struct TeamObjectData {
        string icon;
        string id;
        vector<TeamMembersObjectData> members;
        string ownerUserId;
    };

    struct ApplicationData {
        string id;
        string name;
        string icon;
        string description;
        vector<string> rpcOrigins;
        bool botPublic{ false };
        bool botRequireCodeGrant{ false };
        string termsOfServiceUrl;
        string privacyPolicyUrl;
        UserData owner;
        string  summary;
        string verifyKey;
        TeamObjectData team;
        string guildId;
        string primarySkuId;
        string slug;
        string coverImage;
        int flags{ 0 };
    };

    struct MessageStickerData {
        string id;
        string packId;
        string name;
        string description;
        string tags;
        string asset;
        int formatType;
        bool available;
        string guildId;
        UserData user;
        int sortValue;
    };

    enum class ComponentType {
        ActionRow = 1,
        Button = 2,
        SelectMenu = 3
    };

    enum class ButtonStyle {
        Primary = 1,
        Secondary = 2,
        Success = 3,
        Danger = 4,
        Link = 5
    };

    struct SelectOptionData {
        string label;
        string value;
        string description;
        EmojiData emoji;
        bool _default;
    };

    struct ComponentData {
        ComponentType type = ComponentType::ActionRow;
        ButtonStyle style = ButtonStyle::Danger;
        string label{ "" };
        EmojiData emoji;
        string customId{ "" };
        string url{ "" };
        string placeholder;
        bool disabled{ false };
        vector<SelectOptionData> options;
        int maxValues;
        int minValues;
    };

    struct ActionRowData {
        vector<ComponentData> components;
    };

    struct InteractionApplicationCommandCallbackData {
        bool tts{ false };
        string content;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        int flags{ 0 };
        vector<ActionRowData> components;
    };

    enum class MessageType {
        DEFAULT = 0,
        RECIPIENT_ADD = 1,
        RECIPIENT_REMOVE = 2,
        CALL = 3,
        CHANNEL_NAME_CHANGE = 4,
        CHANNEL_ICON_CHANGE = 5,
        CHANNEL_PINNED_MESSAGE = 6,
        GUILD_MEMBER_JOIN = 7,
        USER_PREMIUM_GUILD_SUBSCRIPTION = 8,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_1 = 9,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_2 = 10,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_3 = 11,
        CHANNEL_FOLLOW_ADD = 12,
        GUILD_DISCOVERY_DISQUALIFIED = 14,
        GUILD_DISCOVERY_REQUALIFIED = 15,
        GUILD_DISCOVERY_GRACE_PERIOD_INITIAL_WARNING = 16,
        GUILD_DISCOVERY_GRACE_PERIOD_FINAL_WARNING = 17,
        THREAD_CREATED = 18,
        REPLY = 19,
        APPLICATION_COMMAND = 20,
        THREAD_STARTER_MESSAGE = 21,
        GUILD_INVITE_REMINDER = 22
    };

    struct ReactionAddEventData {
        string userId;
        string channelId;
        string messageId;
        string guildId;
        GuildMemberData member;
        EmojiData emoji;
    };

    enum class HttpWorkloadClass {
        GET = 0,
        PUT = 1,
        POST = 2,
        PATCH = 3,
        DELETED = 4
    };

    enum class HttpWorkloadType {
        UNSET = 0,
        GET_MESSAGE = 1,
        POST_MESSAGE = 2,
        DELETE_MESSAGE = 3,
        GET_USER = 4,
        GET_USER_SELF = 5,
        GET_GUILD = 6,
        GET_CHANNEL = 7,
        GET_REACTION = 8,
        PUT_REACTION = 9,
        DELETE_REACTION = 10,
        PATCH_MESSAGE = 11,
        DELETE_ALL_REACTION = 12,
        GET_GUILD_MEMBER = 13,
        GET_GUILD_MEMBERS = 14,
        GET_ROLES = 15,
        GET_USER_GUILDS = 16,
        POST_USER_DM = 17,
        GET_DM_CHANNEL = 18,
        PATCH_ROLE = 19,
        GET_APPLICATION = 20,
        POST_APPLICATION_COMMAND = 21,
        GET_SLASH_COMMANDS = 22,
        GET_SOCKET_PATH = 23,
        DELETE_SLASH_COMMAND = 24,
        PATCH_SLASH_COMMAND = 25,
        POST_INTERACTION_RESPONSE = 26,
        PATCH_INTERACTION_RESPONSE = 27,
        POST_ROLE = 28,
        PUT_GUILD_MEMBER_ROLE = 30,
        DELETE_GUILD_MEMBER_ROLE = 31,
        PUT_CHANNEL_PERMISSION_OVERWRITES = 32,
        DELETE_CHANNEL_PERMISSION_OVERWRITES = 33,
        POST_FOLLOW_UP_MESSAGE = 34,
        DELETE_INTERACTION = 35,
        PATCH_FOLLOW_UP_MESSAGE = 36,
        POST_DEFERRED_INTERACTION_RESPONSE = 37,
        GET_MESSAGES = 38,
        DELETE_MESSAGES_BULK = 39,
        GET_AUDIT_LOG = 40,
        DELETE_FOLLOW_UP_MESSAGE = 41,
        DELETE_GUILD_ROLE = 42,
        PATCH_GUILD_ROLES = 43,
        YOUTUBE_SEARCH = 44,
        YOUTUBE_VIDEO_QUERY = 45,
        GET_INVITES = 46,
        PATCH_GUILD_MEMBER = 47,
        GET_PINNED_MESSAGES = 48,
        GET_INTERACTION_RESPONSE = 49,
        GET_INVITE = 50,
        GET_VANITY_INVITE = 51,
        PUT_PIN_MESSAGE = 52,
        PUT_GUILD_BAN = 53,
        DELETE_LEAVE_GUILD = 54
    };

    struct GetApplicationData {
        HttpAgentResources agentResources;
    };

    struct PatchInteractionResponseData {
        HttpAgentResources agentResources;
        string  content;
        string applicationId;
        string interactionToken;
    };

    struct InteractionResponseData {
        InteractionApplicationCommandCallbackData data;
        InteractionCallbackType type;
    };

    struct PostInteractionResponseData {
        HttpAgentResources agentResources;
        string content;
        string interactionId;
        string interactionToken;
    };

    struct PostFollowUpMessageData {
        HttpAgentResources agentResources;
        string content;
        string applicationId;
        string interactionToken;
    };

    struct GetInteractionResponseData {
        HttpAgentResources agentResources;
        string applicationId;
        string interactionToken;
    };

    struct PostDeferredInteractionResponseData {
        HttpAgentResources agentResources;
        string interactionId;
        string interactionToken;
        string content;
    };

    enum class InteractionType {
        Ping = 1,
        ApplicationCommand = 2,
        MessageComponent = 3
    };

    struct MessageInteractionData {
        string id;
        InteractionType type = InteractionType::ApplicationCommand;
        string name;
        UserData user;
    };

    struct HttpData {
        unsigned int returnCode{ 0 };
        string returnMessage{ "" };
        json data;
    };

    struct GetMessagesData {
        HttpAgentResources agentResources;
        string channelId;
        unsigned int limit;
        string beforeThisId = "";
        string afterThisId = "";
        string aroundThisId = "";
    };

    struct DeleteMessagesBulkData {
        HttpAgentResources agentResources;
        string channelId;
        vector<string> messageIds;
        string content;
    };

    struct HttpWorkload {
        HttpWorkloadClass workloadClass;
        HttpWorkloadType workloadType = HttpWorkloadType::UNSET;
        string relativePath;
        string content;
    };

    enum class MessageStickerItemType {
        PNG = 1,
        APNG = 2,
        LOTTIE = 3
    };

    struct MessageStickerItemData {
        string id;
        string name;
        MessageStickerItemType formatType;
    };

    struct MessageDataOld {
        string id;
        string channelId;
        string guildId;
        UserData author;
        GuildMemberData member;
        string content;
        string timestamp;
        string timestampRaw;
        string editedTimestamp;
        bool tts{ false };
        bool mentionEveryone{ false };
        vector<UserData> mentions;
        vector<string> mentionRoles;
        vector<ChannelMentionData> mentionChannels;
        vector<AttachmentData> attachments;
        vector<EmbedData> embeds;
        vector<ReactionData> reactions;
        string nonce;
        bool pinned{ false };
        string webhookId;
        MessageType type = MessageType::APPLICATION_COMMAND;
        MessageActivityData activity;
        ApplicationData application;
        string applicationId;
        MessageReferenceData messageReference;
        int flags{ 0 };
        vector<MessageStickerData> stickers;
        MessageInteractionData interaction;
        vector<ActionRowData> components;
        ChannelData thread;
        vector<MessageStickerItemData> stickerItems;
    };

    struct MessageData : MessageDataOld {
        MessageDataOld referencedMessage;
        string requesterId;
    };

    struct RateLimitData {
        HttpWorkloadType workloadType;
        int getsRemaining = 1;
        float msRemain = 0.0f;
        float timeStartedAt = 0.0f;
        string bucket;
        float nextExecutionTime = 0.0f;
    };

    struct CollectGuildData {
        HttpAgentResources agentResources;
        string guildId;
    };

    struct GetGuildData {
        HttpAgentResources agentResources;
        string guildId;
    };

    struct PutReactionData {
        HttpAgentResources agentResources;
        string channelId;
        string messageId;
        string emoji;
    };

    enum class ReactionDeletionType {
        SELF_DELETE = 0,
        USER_DELETE = 1,
        EMOJI_DELETE = 2,
        ALL_DELETE = 3
    };

    struct DeleteReactionDataAll {
        HttpAgentResources agentResources;
        string channelId;
        string messageId;
        string encodedEmoji;
        string userId;
        ReactionDeletionType deletionType;
    };

    struct GetReactionData {
        string channelId;
        string messageId;
        string userId;
        string emojiName;
    };

    struct CollectChannelData {
        HttpAgentResources agentResources;
        string channelId;
    };

    struct GetChannelData {
        HttpAgentResources agentResources;
        string channelId;
    };

    struct PutPermissionOverwritesData {
        HttpAgentResources agentResources;
        string roleOrUserId;
        string content;
        string channelId;
    };

    struct DeleteChannelPermissionOverwritesData {
        HttpAgentResources agentResources;
        string roleOrUserId;
        string channelId;
    };

    struct GetDMChannelData {
        HttpAgentResources agentResources;
        string userId;
    };

    struct GetMessageData {
        HttpAgentResources agentResources;
        string channelId;
        string messageId;
        string requesterId;
    };

    struct PutPinMessageData {
        HttpAgentResources agentResources;
        string channelId;
        string messageId;
    };

    enum class GetUserDataType {
        SELF = 0,
        USER = 1
    };

    struct CollectUserData {
        HttpAgentResources agentResources;
        string userId;
        GetUserDataType userType;
    };

    struct LeaveGuildData {
        HttpAgentResources agentResources;
        string guildId;
    };

    struct GetUserData {
        HttpAgentResources agentResources;
        string userId;
        GetUserDataType userType;
    };

    struct GetPinnedMessagesData {
        HttpAgentResources agentResources;
        string channelId;
    };

    struct PostMessageData {
        HttpAgentResources agentResources;
        string requesterId;
        string channelId;
        string content;
    };

    struct PatchMessageData {
        HttpAgentResources agentResources;
        string content;
        string channelId;
        string messageId;
        string requesterId;
    };

    struct DeleteMessageData {
        HttpAgentResources agentResources;
        unsigned int timeDelay{ 0 };
        string channelId;
        string messageId;
    };

    struct PostDMData {
        HttpAgentResources agentResources;
        string userId;
        string channelId;
        string finalContent;
        string content;
        int nonce;
        bool tts{ false };
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        MessageReferenceData messageReference;
        vector<ActionRowData> components;
    };

    struct CollectGuildMemberData {
        HttpAgentResources agentResources;
        string guildId;
        string guildMemberId;
    };

    struct GetGuildMemberData {
        HttpAgentResources agentResources;
        string guildId;
        string guildMemberId;
    };

    struct GetGuildMemberRolesData {
        HttpAgentResources agentResources;
        vector<string> roleIds;
    };

    struct ModifyGuildMemberData {
        HttpAgentResources agentResources;
        string guildMemberId;
        string guildId;
        string nick;
        vector<string> roleIds;
        bool mute;
        bool deaf;
        string newVoiceChannelId = "";
        string currentChannelId = "";
    };

    struct UpdateVoiceStateData {
        string guildId;
        string channelId;
        bool selfMute;
        bool selfDeaf;
    };

    struct GetRolesData {
        HttpAgentResources agentResources;
        string guildId;
    };

    struct GetRoleData {
        HttpAgentResources agentResources;
        string guildId;
        string roleId;
    };

    struct DeleteGuildMemberRoleData {
        HttpAgentResources agentResources;
        string guildId;
        string userId;
        string roleId;
    };

    struct DeleteGuildRoleData {
        HttpAgentResources agentResources;
        string guildId;
        string roleId;
    };

    struct RolePositionData {
        string roleId;
        unsigned int rolePosition;
    };

    struct UpdateRolePositionData {
        HttpAgentResources agentResources;
        string guildId;
        vector<RolePositionData> rolePositions;
        string content;
    };

    struct CollectRoleData {
        HttpAgentResources agentResources;
        string guildId;
        string roleId;
    };

    struct PostRoleData {
        string content;
        HttpAgentResources agentResources;
        string guildId;
        string roleId;
        string name;
        __int64 permissions;
        string hexColorValue;
        bool hoist;
        bool mentionable;
    };

    struct PutRoleData {
        HttpAgentResources agentResources;
        string guildId;
        string userId;
        string roleId;
    };

    struct PatchRoleData {
        HttpAgentResources agentResources;
        string guildId;
        string roleId;
        string content;
        string name;
        string permissions;
        string hexColorValue;
        bool hoist;
        bool mentionable;
    };

    enum class ApplicationCommandOptionType {
        SUB_COMMAND = 1,
        SUB_COMMAND_GROUP = 2,
        STRING = 3,
        INTEGER = 4,
        BOOLEAN = 5,
        USER = 6,
        CHANNEL = 7,
        ROLE = 8,
        MENTIONABLE = 9
    };

    struct ApplicationCommandOptionChoiceData {
        string name{ "" };
        int	valueInt{ 0 };
        string valueString{ "" };
    };

    struct ApplicationCommandOptionData {
        ApplicationCommandOptionType type = ApplicationCommandOptionType::SUB_COMMAND_GROUP;
        string name{ "" };
        string description{ "" };
        bool required{ false };
        vector<ApplicationCommandOptionChoiceData>	choices;
        vector<ApplicationCommandOptionData> options;
    };

    enum class ApplicationCommandType {
        CHAT_INPUT = 1,
        USER = 2,
        MESSAGE = 3
    };

    struct ApplicationCommandData {
        string id;
        ApplicationCommandType type;
        string applicationId;
        string guildId;
        string name;
        string description;
        vector<ApplicationCommandOptionData> options;
        bool defaultPermission;
    };

    struct EditApplicationCommandData {
        string name;
        string description;
        vector<ApplicationCommandOptionData> options;
        bool defaultPermission;
    };

    struct CreateApplicationCommandData {
        ApplicationCommandType type;
        string applicationId;
        string name;
        string description;
        vector<ApplicationCommandOptionData> options;
        bool defaultPermission;
    };

    struct ApplicationCommandInteractionDataResolved {
        map<string, UserData> users{};
        map<string, GuildMemberData> members{};
        map<string, RoleData> roles{};
        map<string, ChannelData> channels{};
    };

    struct ApplicationCommandInteractionDataOption {
        string name;
        ApplicationCommandOptionType type = ApplicationCommandOptionType::BOOLEAN;
        bool valueBool{ false };
        string valueString;
        int valueInt = -1;
        vector<ApplicationCommandInteractionDataOption> options;
    };

    struct ApplicationCommandInteractionData {
        string name;
        string id;
        ApplicationCommandInteractionDataResolved resolved;
        vector<ApplicationCommandInteractionDataOption> options;
        string customId;
        int componentType = -1;
    };

    struct InteractionData {
        string id;
        string applicationId;
        InteractionType type = InteractionType::ApplicationCommand;
        ApplicationCommandInteractionData data;
        json dataRaw;
        string guildId;
        string channelId;
        GuildMemberData member;
        ComponentType componentType;
        UserData user;
        string token;
        int version = -1;
        MessageData message;
        string customId;
        string requesterId;
        vector<string> values;
    };

    struct InteractionResponseFullData {
        InteractionData interactionData;
        PostInteractionResponseData interactionResponseData;
    };

    enum class InputEventType {
        SLASH_COMMAND_INTERACTION = 1,
        BUTTON_INTERACTION = 2,
        REGULAR_MESSAGE = 3,
        SELECT_MENU_INPUT = 4
    };

    enum class InputEventResponseType {
        UNSET = 0,
        REGULAR_MESSAGE_RESPONSE = 1,
        REGULAR_MESSAGE_EDIT = 2,
        INTERACTION_RESPONSE = 3,
        INTERACTION_RESPONSE_DEFERRED = 4,
        INTERACTION_RESPONSE_EPHEMERAL = 5,
        INTERACTION_RESPONSE_EDIT = 6,
        INTERACTION_FOLLOW_UP_MESSAGE = 7,
        INTERACTION_FOLLOW_UP_MESSAGE_EDIT = 8,
        DEFER_COMPONENT_RESPONSE = 9
    };

    struct InputEventData {
        InputEventData() {}
        InputEventData(MessageData messageData, InteractionData interactionData, InputEventType eventType) {
            this->messageData = messageData;
            this->interactionData = interactionData;
            this->eventType = eventType;
            if (this->interactionData.user.id != "") {
                this->messageData.author = this->interactionData.user;
            }
            else {
                this->interactionData.user = this->messageData.author;
            }
            if (this->messageData.member.user.id == "") {
                this->messageData.member = this->interactionData.member;
            }
            else {
                this->interactionData.member = this->messageData.member;
            }
            if (this->messageData.channelId == "") {
                this->messageData.channelId = this->interactionData.channelId;
            }
            else {
                this->interactionData.channelId = this->messageData.channelId;
            }
            if (this->messageData.id == "") {
                this->messageData.id = this->interactionData.message.id;
            }
            else {
                this->interactionData.message.id = this->messageData.id;
            }
            if (this->messageData.guildId == "") {
                this->messageData.guildId = this->interactionData.guildId;
            }
            else {
                this->interactionData.guildId = this->messageData.guildId;
            }
            if (this->interactionData.id == "") {
                this->interactionData.id = this->messageData.interaction.id;
            }
            if (this->messageData.requesterId != "") {
                this->requesterId = this->messageData.requesterId;
            }
            else {
                this->requesterId = this->interactionData.requesterId;
            }
        }
        shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
        InputEventType eventType;
        InputEventResponseType inputEventResponseType;
        InteractionData getInteractionData() {
            return this->interactionData;
        }
        string getMessageContent() {
            if (this->interactionData.message.content != "") {
                return this->interactionData.message.content;
            }
            else {
                return this->messageData.content;
            }
        }
        vector<ActionRowData> getComponents() {
            if (this->interactionData.message.components.size() > 0) {
                if (this->interactionData.message.components.at(0).components.at(0).customId != "") {
                    return this->interactionData.message.components;
                }
            }
            else {
                return this->messageData.components;
            }
            return vector<ActionRowData>();
        }
        vector<EmbedData> getEmbeds() {
            if (this->interactionData.message.embeds.size() > 0) {
                if (this->interactionData.message.embeds.at(0).description != "") {
                    return this->interactionData.message.embeds;
                }
                else if (this->interactionData.message.embeds.at(0).fields.size() > 0) {
                    if (this->interactionData.message.embeds.at(0).fields.at(0).value != "") {
                        return this->interactionData.message.embeds;
                    }
                }
            }
            else {
                return this->messageData.embeds;
            }
            return vector<EmbedData>();
        }
        string getApplicationId() {
            if (this->interactionData.applicationId == "") {
                return this->messageData.application.id;
            }
            else {
                return this->interactionData.applicationId;
            }
        }
        string getInteractionToken() {
            return this->interactionData.token;
        }
        string getInteractionId() {
            if (this->interactionData.id == "") {
                return this->messageData.interaction.id;
            }
            else {
                return this->interactionData.id;
            }
        }
        string getUserName() {
            if (this->messageData.author.username == "" && this->interactionData.member.user.username != "") {
                return this->interactionData.member.user.username;
            }
            else if (this->interactionData.member.user.username == "" && this->interactionData.user.username != "") {
                return this->interactionData.user.username;
            }
            else {
                return this->messageData.author.username;
            }
        }
        string getAvatarURL() {
            if (this->messageData.author.avatar == "" && this->interactionData.member.user.avatar != "") {
                return this->interactionData.member.user.avatar;
            }
            else if (this->interactionData.member.user.avatar == "" && this->interactionData.user.avatar != "") {
                return this->interactionData.user.avatar;
            }
            else {
                return this->messageData.author.avatar;
            }
        }
        string getChannelId() {
            if (this->interactionData.channelId == "") {
                return this->messageData.channelId;
            }
            else {
                return this->interactionData.channelId;
            }
        }
        string getMessageId() {
            if (this->messageData.id == "") {
                return this->interactionData.message.id;
            }
            else {
                return this->messageData.id;
            }
        }
        string getAuthorId() {
            if (this->messageData.author.id == "") {
                return this->interactionData.user.id;
            }
            else {
                return this->messageData.author.id;
            }
        }
        string getGuildId() {
            if (this->messageData.guildId == "") {
                return this->interactionData.guildId;
            }
            else {
                return this->messageData.guildId;
            }
        }
        MessageData getMessageData() {
            return this->messageData;
        }
        string getRequesterId() {
            return this->requesterId;
        }
    protected:
        friend class CommandController;
        friend class InputEventStuff;
        friend class DiscordCoreClient;
        InteractionData interactionData;
        MessageData messageData;
        string requesterId;
    };

    struct PatchFollowUpMessageData {
        HttpAgentResources agentResources;
        string applicationId;
        string interactionToken;
        string messageId;
        string content;
    };

    enum class AuditLogEvent {
        GUILD_UPDATE = 1,
        CHANNEL_CREATE = 10,
        CHANNEL_UPDATE = 11,
        CHANNEL_DELETE = 12,
        CHANNEL_OVERWRITE_CREATE = 13,
        CHANNEL_OVERWRITE_UPDATE = 14,
        CHANNEL_OVERWRITE_DELETE = 15,
        MEMBER_KICK = 20,
        MEMBER_PRUNE = 21,
        MEMBER_BAN_ADD = 22,
        MEMBER_BAN_REMOVE = 23,
        MEMBER_UPDATE = 24,
        MEMBER_ROLE_UPDATE = 25,
        MEMBER_MOVE = 26,
        MEMBER_DISCONNECT = 27,
        BOT_ADD = 28,
        ROLE_CREATE = 30,
        ROLE_UPDATE = 31,
        ROLE_DELETE = 32,
        INVITE_CREATE = 40,
        INVITE_UPDATE = 41,
        INVITE_DELETE = 42,
        WEBHOOK_CREATE = 50,
        WEBHOOK_UPDATE = 51,
        WEBHOOK_DELETE = 52,
        EMOJI_CREATE = 60,
        EMOJI_UPDATE = 61,
        EMOJI_DELETE = 62,
        MESSAGE_DELETE = 72,
        MESSAGE_BULK_DELETE = 73,
        MESSAGE_PIN = 74,
        MESSAGE_UNPIN = 75,
        INTEGRATION_CREATE = 80,
        INTEGRATION_UPDATE = 81,
        INTEGRATION_DELETE = 82
    };

    struct GetAuditLogData {
        HttpAgentResources agentResources;
        string userId;
        string guildId;
        AuditLogEvent actionType;
        unsigned int limit;
    };

    struct GetInviteData {
        HttpAgentResources agentResources;
        string inviteId;
    };

    struct GetVanityInviteData {
        HttpAgentResources agentResources;
        string guildId;
    };

    struct GetInvitesData {
        HttpAgentResources agentResources;
        string guildId;
    };

    struct VoiceConnectionData {
        string channelId;
        string endpoint;
        string sessionId;
        string guildId;
        string token;
        string userId;
        int audioSSRC;
        string keys;
    };

    struct VoiceReadyPayload {
        int ssrc;
        string ip;
        int port;
        vector<string> modes;
    };

    struct PutGuildBanData {
        HttpAgentResources agentResources;
        string guildId;
        string guildMemberId;
        int deleteMessageDays = 0;
        string reason = "";
    };
}

namespace DiscordCoreAPI {

    class DiscordCoreClient;

    template <typename... T, typename Function>
    void executeFunctionAfterTimePeriod(Function function, unsigned int timeDelayInMs, bool isRepeating, T... args) {
        ThreadPoolTimer threadPoolTimer{ nullptr };
        if (timeDelayInMs > 0) {
            TimerElapsedHandler timeElapsedHandler = [=](ThreadPoolTimer threadPoolTimerNew)->void {
                function(args...);
            };
            if (isRepeating) {
                threadPoolTimer = threadPoolTimer.CreatePeriodicTimer(timeElapsedHandler, winrt::Windows::Foundation::TimeSpan(timeDelayInMs * 10000));
            }
            else {
                threadPoolTimer = threadPoolTimer.CreateTimer(timeElapsedHandler, winrt::Windows::Foundation::TimeSpan(timeDelayInMs * 10000));
            }
        }
        else {
            function(args...);
        }
        return;
    }

    template <>
    void executeFunctionAfterTimePeriod(function<void(ThreadPoolTimer)> function, unsigned int timeDelayInMs, bool isRepeating) {
        ThreadPoolTimer threadPoolTimer{ nullptr };
        if (timeDelayInMs > 0) {
            TimerElapsedHandler timeElapsedHandler = [=](ThreadPoolTimer threadPoolTimerNew)->void {
                function(threadPoolTimerNew);
            };
            if (isRepeating) {
                threadPoolTimer = threadPoolTimer.CreatePeriodicTimer(timeElapsedHandler, winrt::Windows::Foundation::TimeSpan(timeDelayInMs * 10000));
            }
            else {
                threadPoolTimer = threadPoolTimer.CreateTimer(timeElapsedHandler, winrt::Windows::Foundation::TimeSpan(timeDelayInMs * 10000));
            }
        }
        else {
            function(threadPoolTimer);
        }
        return;
    }

    DispatcherQueueTimer getDispatcherQueueTimer() {
        DispatcherQueueOptions options{
               sizeof(DispatcherQueueOptions),
               DQTYPE_THREAD_DEDICATED,
               DQTAT_COM_ASTA
        };
        ABI::Windows::System::IDispatcherQueueController* ptrNew{ nullptr };
        check_hresult(CreateDispatcherQueueController(options, &ptrNew));
        DispatcherQueueController queueController = { ptrNew, take_ownership_from_abi };
        DispatcherQueue threadQueue = queueController.DispatcherQueue();
        DispatcherQueueTimer timer = threadQueue.CreateTimer();
        return timer;
    }

    string getTimeAndDate() {
        const time_t now = time(nullptr);
        char charArray[26];
        std::tm time;
        DYNAMIC_TIME_ZONE_INFORMATION timeZoneInfo{};
        auto returnValue = GetDynamicTimeZoneInformation(&timeZoneInfo);
        localtime_s(&time, &now);
        if ((int)returnValue == 1) {
            time.tm_hour = (time.tm_hour + (timeZoneInfo.Bias / 60)) % 24;
        }
        else {
            time.tm_hour = (time.tm_hour + (timeZoneInfo.Bias / 60) - 1) % 24;
        }
        strftime(charArray, 26, "%F %R", &time);
        return charArray;
    }

    enum class Permissions :__int64 {
        CREATE_INSTANT_INVITE = (1 << 0),
        KICK_MEMBERS = (1 << 1),
        BAN_MEMBERS = (1 << 2),
        ADMINISTRATOR = (1 << 3),
        MANAGE_CHANNELS = (1 << 4),
        MANAGE_GUILD = (1 << 5),
        ADD_REACTIONS = (1 << 6),
        VIEW_AUDIT_LOG = (1 << 6),
        PRIORITY_SPEAKER = (1 << 8),
        STREAM = (1 << 9),
        VIEW_CHANNEL = (1 << 10),
        SEND_MESSAGES = (1 << 11),
        SEND_TTS_MESSAGES = (1 << 12),
        MANAGE_MESSAGES = (1 << 13),
        EMBED_LINKS = (1 << 14),
        ATTACH_FILES = (1 << 15),
        READ_MESSAGE_HISTORY = (1 << 16),
        MENTION_EVERYONE = (1 << 17),
        USE_EXTERNAL_EMOJIS = (1 << 18),
        VIEW_GUILD_INSIGHTS = (1 << 19),
        CONNECT = (1 << 20),
        SPEAK = (1 << 21),
        MUTE_MEMBERS = (1 << 22),
        DEAFEN_MEMBERS = (1 << 23),
        MOVE_MEMBERS = (1 << 24),
        USE_VAD = (1 << 25),
        CHANGE_NICKNAME = (1 << 26),
        MANAGE_NICKNAMES = (1 << 27),
        MANAGE_ROLES = (1 << 28),
        MANAGE_WEBHOOKS = (1 << 29),
        MANAGE_EMOJIS = (1 << 30),
        USE_SLASH_COMMANDS = (1ull << 31),
        REQUEST_TO_SPEAK = (1ull << 32),
        MANAGE_THREADS = (1ull << 33),
        USE_PUBLIC_THREADS = (1ull << 34),
        USE_PRIVATE_THREADS = (1ull << 35),
        USE_EXTERNAL_STICKERS = (1ull << 37)
    };

    enum class ComponentType {
        ActionRow = 1,
        Button = 2,
        SelectMenu = 3
    };

    enum class ButtonStyle {
        Primary = 1,
        Success = 3,
        Secondary = 2,
        Danger = 4,
        Link = 5
    };

    struct RoleTagsData {
        operator DiscordCoreInternal::RoleTagsData() {
            DiscordCoreInternal::RoleTagsData newData;
            newData.botId = this->botId;
            newData.integrationId = this->integrationId;
            return newData;
        }
        string integrationId;
        string botId;
        string premiumSubscriber;
    };

    struct RoleData {
        operator DiscordCoreInternal::RoleData() {
            DiscordCoreInternal::RoleData newData;
            newData.color = this->color;
            newData.hoist = this->hoist;
            newData.id = this->id;
            newData.managed = this->managed;
            newData.mentionable = this->mentionable;
            newData.name = this->name;
            newData.permissions = this->permissions;
            newData.position = this->position;
            newData.tags = this->tags;
            return newData;
        }
        RoleTagsData tags;
        string permissions;
        bool mentionable;
        int position;
        string name;
        bool managed;
        bool hoist;
        string id = "";
        int color;
    };

    struct UserData {
        operator DiscordCoreInternal::UserData() {
            DiscordCoreInternal::UserData newData;
            newData.avatar = this->avatar;
            newData.bot = this->bot;
            newData.discriminator = this->discriminator;
            newData.email = this->email;
            newData.flags = this->flags;
            newData.id = this->id;
            newData.locale = this->locale;
            newData.mfaEnabled = this->mfaEnabled;
            newData.premiumType = this->premiumType;
            newData.publicFlags = this->publicFlags;
            newData.system = this->system;
            newData.username = this->username;
            newData.verified = this->verified;
            newData.createdAt = this->createdAt;
            return newData;
        }
        string username;
        string id;
        string discriminator;
        string avatar;
        bool bot;
        bool system{ false };
        bool mfaEnabled{ false };
        string locale;
        bool verified{ false };
        string email;
        int flags{ 0 };
        int premiumType{ 0 };
        int publicFlags{ 0 };
        string createdAt;
    };

    struct EmojiData {
        operator DiscordCoreInternal::EmojiData() {
            DiscordCoreInternal::EmojiData newData;
            newData.animated = this->animated;
            newData.available = this->available;
            newData.id = this->id;
            newData.managed = this->managed;
            newData.name = this->name;
            newData.requireColons = this->requireColons;
            for (auto value : this->roles) {
                newData.roles.push_back(value);
            }
            newData.user = this->user;
            return newData;
        }
        string id{ "" };
        string name{ "" };
        vector<RoleData> roles;
        UserData user;
        bool requireColons;
        bool managed;
        bool animated;
        bool available;
    };

    struct SelectOptionData {
        operator DiscordCoreInternal::SelectOptionData() {
            DiscordCoreInternal::SelectOptionData newData;
            newData.description = this->description;
            newData.emoji = this->emoji;
            newData.label = this->label;
            newData.value = this->value;
            newData._default = this->_default;
            return newData;
        }
        string label;
        string value;
        string description;
        EmojiData emoji;
        bool _default;
    };

    struct ComponentData {
        operator DiscordCoreInternal::ComponentData() {
            DiscordCoreInternal::ComponentData newData;
            newData.customId = this->customId;
            newData.disabled = this->disabled;
            newData.emoji = this->emoji;
            newData.label = this->label;
            newData.style = (DiscordCoreInternal::ButtonStyle)this->style;
            newData.type = (DiscordCoreInternal::ComponentType)this->type;
            newData.url = this->url;
            newData.maxValues = this->maxValues;
            newData.minValues = this->minValues;
            for (auto value : this->options) {
                newData.options.push_back(value);
            }
            newData.placeholder = this->placeholder;
            return newData;
        }
        ComponentType type;
        ButtonStyle style;
        string label{ "" };
        EmojiData emoji;
        string customId{ "" };
        string url{ "" };
        string placeholder;
        bool disabled{ false };
        vector<SelectOptionData> options;
        int maxValues;
        int minValues;
    };

    struct AttachmentData {
        operator DiscordCoreInternal::AttachmentData() {
            DiscordCoreInternal::AttachmentData newData;
            newData.contentType = this->contentType;
            newData.filename = this->filename;
            newData.height = this->height;
            newData.id = this->id;
            newData.proxyUrl = this->proxyUrl;
            newData.size = this->size;
            newData.url = this->url;
            newData.width = this->width;
            return newData;
        }
        string id{ "" };
        string filename{ "" };
        string contentType{ "" };
        int size{ 0 };
        string url{ "" };
        string proxyUrl{ "" };
        int height{ 0 };
        int width{ 0 };
    };

    struct EmbedFooterData {
        operator DiscordCoreInternal::EmbedFooterData() {
            DiscordCoreInternal::EmbedFooterData newData;
            newData.iconUrl = this->iconUrl;
            newData.proxyIconUrl = this->proxyIconUrl;
            newData.text = this->text;
            return newData;
        };
        string iconUrl;
        string text;
        string proxyIconUrl;
    };

    struct EmbedImageData {
        operator DiscordCoreInternal::EmbedImageData() {
            DiscordCoreInternal::EmbedImageData newData;
            newData.height = this->height;
            newData.proxyUrl = this->proxyUrl;
            newData.url = this->url;
            newData.width = this->width;
            return newData;
        }
        string url;
        string proxyUrl;
        int height;
        int width;
    };

    struct EmbedThumbnailData {
        operator DiscordCoreInternal::EmbedThumbnailData() {
            DiscordCoreInternal::EmbedThumbnailData newData;
            newData.height = this->height;
            newData.proxyUrl = this->proxyUrl;
            newData.url = this->url;
            newData.width = this->width;
            return newData;
        }
        string url;
        string proxyUrl;
        int height;
        int width;
    };

    struct EmbedVideoData {
        operator DiscordCoreInternal::EmbedVideoData() {
            DiscordCoreInternal::EmbedVideoData newData;
            newData.height = this->height;
            newData.proxyUrl = this->proxyUrl;
            newData.url = this->url;
            newData.width = this->width;
            return newData;
        }
        string url;
        string proxyUrl;
        int height;
        int width;
    };

    struct EmbedProviderData {
        operator DiscordCoreInternal::EmbedProviderData() {
            DiscordCoreInternal::EmbedProviderData newData;
            newData.name = this->name;
            newData.url = this->url;
            return newData;
        }
        string name;
        string url;
    };

    struct EmbedAuthorData {
        operator DiscordCoreInternal::EmbedAuthorData() {
            DiscordCoreInternal::EmbedAuthorData newData;
            newData.iconUrl = this->iconUrl;
            newData.name = this->name;
            newData.proxyIconUrl = this->proxyIconUrl;
            newData.url = this->url;
            return newData;
        };
        string name;
        string url;
        string iconUrl;
        string proxyIconUrl;
    };

    struct EmbedFieldData {
        operator DiscordCoreInternal::EmbedFieldData() {
            DiscordCoreInternal::EmbedFieldData newData;
            newData.Inline = this->Inline;
            newData.name = this->name;
            newData.value = this->value;
            return newData;
        }
        string name;
        string value;
        bool Inline;
    };

    struct EmbedData {

        EmbedData(){}

        EmbedData(const EmbedData& p1) {
            this->author = p1.author;
            this->description = p1.description;
            for (auto value : p1.fields) {
                this->fields.push_back(value);
            }
            this->footer = p1.footer;
            this->hexColorValue = p1.hexColorValue;
            this->image = p1.image;
            this->provider=p1.provider;
            this->thumbnail = p1.thumbnail;
            this->timestampRaw = p1.timestampRaw;
            this->timestamp = p1.timestampRaw;
            this->title = p1.title;
            this->type = p1.type;
            this->url = p1.url;
            this->video = p1.video;
            return;
        }

        operator DiscordCoreInternal::EmbedData() {
            DiscordCoreInternal::EmbedData newData;
            newData.author = this->author;
            newData.description = this->description;
            for (auto value : this->fields) {
                newData.fields.push_back(value);
            }
            newData.footer = this->footer;
            newData.hexColorValue = this->hexColorValue;
            newData.image = this->image;
            newData.provider = this->provider;
            newData.thumbnail = this->thumbnail;
            newData.timestamp = this->timestampRaw;
            newData.title = this->title;
            newData.type = this->type;
            newData.url = this->url;
            newData.video = this->video;
            return newData;
        };
        EmbedData* setTitle(string titleNew) {
            this->title = titleNew;
            return this;
        }
        EmbedData* setAuthor(string authorName, string authorAvatarURL = "") {
            this->author.name = authorName;
            this->author.iconUrl = authorAvatarURL;
            return this;
        }
        EmbedData* setImage(string imageURL) {
            this->image.url = imageURL;
            return this;
        }
        EmbedData* setThumbnail(string thumbnailURL) {
            this->thumbnail.url = thumbnailURL;
            return this;
        }
        EmbedData* setColor(string hexColorValueNew) {
            this->hexColorValue = hexColorValueNew;
            return this;
        }
        EmbedData* setDescription(string descriptionNew) {
            this->description = descriptionNew;
            return this;
        }
        EmbedData* setFooter(string footerText, string footerIconURLText = "") {
            this->footer.text = footerText;
            this->footer.iconUrl = footerIconURLText;
            return this;
        }
        EmbedData* setTimeStamp(string timeStamp) {
            this->timestamp = timeStamp;
            return this;
        }
        EmbedData* addField(string name, string value, bool Inline = true) {
            EmbedFieldData embedFieldData;
            embedFieldData.name = name;
            embedFieldData.Inline = Inline;
            embedFieldData.value = value;
            this->fields.push_back(embedFieldData);
            return this;
        }
        string title;
        string type;
        string description;
        string url;
        string timestamp;
        string hexColorValue;
        EmbedFooterData footer;
        EmbedImageData image;
        EmbedThumbnailData thumbnail;
        string timestampRaw;
        EmbedVideoData video;
        EmbedProviderData provider;
        EmbedAuthorData author;
        vector<EmbedFieldData> fields;
    };

    struct ActionRowData {
        operator DiscordCoreInternal::ActionRowData() {
            DiscordCoreInternal::ActionRowData newData;
            for (auto value : this->components) {
                DiscordCoreInternal::ComponentData component;
                component = value;
                newData.components.push_back(value);
            }
            return newData;
        }
        vector<ComponentData> components;
    };

    enum class InputEventResponseType {
        UNSET = 0,
        REGULAR_MESSAGE_RESPONSE = 1,
        REGULAR_MESSAGE_EDIT = 2,
        INTERACTION_RESPONSE = 3,
        INTERACTION_RESPONSE_DEFERRED = 4,
        INTERACTION_RESPONSE_EPHEMERAL = 5,
        INTERACTION_RESPONSE_EDIT = 6,
        INTERACTION_FOLLOW_UP_MESSAGE = 7,
        INTERACTION_FOLLOW_UP_MESSAGE_EDIT = 8,
        DEFER_COMPONENT_RESPONSE = 9
    };

    struct AllowedMentionsData {
        operator DiscordCoreInternal::AllowedMentionsData() {
            DiscordCoreInternal::AllowedMentionsData newData;
            newData.parse = this->parse;
            newData.repliedUser = this->repliedUser;
            newData.roles = this->roles;
            newData.users = this->users;
            return newData;
        }
        vector<string> parse;
        vector<string> roles;
        vector<string> users;
        bool repliedUser;
    };

    struct InteractionApplicationCommandCallbackData {
        operator DiscordCoreInternal::InteractionApplicationCommandCallbackData() {
            DiscordCoreInternal::InteractionApplicationCommandCallbackData newData;
            newData.allowedMentions = this->allowedMentions;
            for (auto value : this->components) {
                newData.components.push_back(value);
            }
            newData.content = this->content;
            for (auto value : this->embeds) {
                newData.embeds.push_back(value);
            }
            newData.flags = this->flags;
            newData.tts = this->tts;
            return newData;
        }
        bool tts;
        string content;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        int flags{ 0 };
        vector<ActionRowData> components;
    };

    enum class InteractionCallbackType {
        Pong = 1,
        Acknowledge = 2,
        ChannelMessage = 3,
        ChannelMessageWithSource = 4,
        DeferredChannelMessageWithSource = 5,
        DeferredUpdateMessage = 6,
        UpdateMessage = 7
    };

    struct MessageReferenceData {
        operator DiscordCoreInternal::MessageReferenceData() {
            DiscordCoreInternal::MessageReferenceData newData;
            newData.channelId = this->channelId;
            newData.failIfNotExists = this->failIfNotExists;
            newData.guildId = this->guildId;
            newData.messageId = this->messageId;
            return newData;
        }
        string messageId;
        string channelId;
        string guildId;
        bool failIfNotExists;
    };

    enum class InteractionType {
        Ping = 1,
        ApplicationCommand = 2,
        MessageComponent = 3
    };

    enum class EditChannelPermissionOverwritesType {
        Role = 0,
        User = 1
    };

    struct OverWriteData {
        operator DiscordCoreInternal::OverWriteData() {
            DiscordCoreInternal::OverWriteData newData;
            newData.allow = this->allow;
            newData.deny = this->deny;
            newData.id = this->id;
            newData.type = (DiscordCoreInternal::EditChannelPermissionOverwritesType)this->type;
            newData.channelId = this->channelId;
            return newData;
        }
        string id;
        EditChannelPermissionOverwritesType type;   //	either 0 (role) or 1 (member;
        string allow;
        string deny;
        string channelId;
    };

    enum class ChannelType {
        GUILD_TEXT = 0,
        DM = 1,
        GUILD_VOICE = 2,
        GROUP_DM = 3,
        GUILD_CATEGORY = 4,
        GUILD_NEWS = 5,
        GUILD_STORE = 6,
        GUILD_NEWS_THREAD = 10,
        GUILD_PUBLIC_THREAD = 11,
        GUILD_PRIVATE_THREAD = 12,
        GUILD_STAGE_VOICE = 13
    };

    struct ThreadMetadataData {
        bool archived;
        string archiverId;
        int autoArchiveDuration;
        string archiveTimestamp;
        bool locked;
    };

    struct ThreadMember {
        string id;
        string userId;
        string joinTimestamp;
        int flags;
    };

    struct ChannelData {
        operator DiscordCoreInternal::ChannelData() {
            DiscordCoreInternal::ChannelData newData;
            newData.applicationId = this->applicationId;
            newData.bitrate = this->bitrate;
            newData.guildId = this->guildId;
            newData.icon = this->icon;
            newData.id = this->id;
            newData.lastMessageId = this->lastMessageId;
            newData.lastPinTimestamp = this->lastPinTimestamp;
            newData.name = this->name;
            newData.nsfw = this->nsfw;
            newData.ownerId = this->ownerId;
            newData.parentId = this->parentId;
            for (auto [key, value] : this->permissionOverwrites) {
                newData.permissionOverwrites.insert(make_pair(key, value));
            }
            newData.position = this->position;
            newData.rateLimitPerUser = this->rateLimitPerUser;
            for (auto value : this->recipients) {
                newData.recipients.push_back(value);
            }
            newData.rtcRegion = this->rtcRegion;
            newData.topic = this->topic;
            newData.type = (DiscordCoreInternal::ChannelType)this->type;
            newData.userLimit = this->userLimit;
            newData.videoQualityMode = this->videoQualityMode;
            return newData;
        }
        string id;
        ChannelType type = ChannelType::DM;
        string guildId;
        int position;
        map<string, OverWriteData> permissionOverwrites;
        string name;
        string topic;
        bool nsfw;
        string lastMessageId;
        int bitrate;
        int userLimit;
        int rateLimitPerUser;
        vector<UserData> recipients;
        string icon;
        string ownerId;
        string applicationId;
        string parentId;
        string lastPinTimestamp;
        string rtcRegion;
        int videoQualityMode;
        int messageCount;
        int memberCount;
        ThreadMetadataData threadMetadata;
        ThreadMember member;
    };

    struct VoiceStateData {
        string guildId;
        string channelId;
        string userId;
        string sessionId;
        bool deaf;
        bool mute;
        bool selfDeaf;
        bool selfMute;
        bool selfStream;
        bool selfVideo;
        bool suppress;
        string requestToSpeakTimestamp;
    };

    struct GuildMemberData {
        operator DiscordCoreInternal::GuildMemberData() {
            DiscordCoreInternal::GuildMemberData newData;
            newData.deaf = this->deaf;
            newData.guildId = this->guildId;
            newData.joinedAt = this->joinedAt;
            newData.mute = this->mute;
            newData.nick = this->nick;
            newData.pending = this->pending;
            newData.permissions = this->permissions;
            newData.premiumSince = this->premiumSince;
            newData.roles = this->roles;
            newData.user = this->user;
            newData.userMention = this->userMention;
            return newData;
        }
        string guildId;
        UserData user;
        string nick;
        vector<string> roles;
        string joinedAt;
        string premiumSince;
        bool deaf;
        bool mute;
        bool pending;
        string permissions;
        string userMention;
        VoiceStateData voiceData;
    };

    struct ApplicationCommandInteractionDataResolved {
        operator DiscordCoreInternal::ApplicationCommandInteractionDataResolved() {
            DiscordCoreInternal::ApplicationCommandInteractionDataResolved newData;
            for (auto [key, value] : this->channels) {
                newData.channels.insert(make_pair(key, value));
            }
            for (auto [key, value] : this->members) {
                newData.members.insert(make_pair(key, value));
            }
            for (auto [key, value] : this->roles) {
                newData.roles.insert(make_pair(key, value));
            }
            for (auto [key, value] : this->users) {
                newData.users.insert(make_pair(key, value));
            }
            return newData;
        }
        map<string, UserData> users{};
        map<string, GuildMemberData> members{};
        map<string, RoleData> roles{};
        map<string, ChannelData> channels{};
    };

    enum class ApplicationCommandOptionType {
        SUB_COMMAND = 1,
        SUB_COMMAND_GROUP = 2,
        STRING = 3,
        INTEGER = 4,
        BOOLEAN = 5,
        USER = 6,
        CHANNEL = 7,
        ROLE = 8,
        MENTIONABLE = 9
    };

    struct ApplicationCommandInteractionDataOption;

    vector<DiscordCoreInternal::ApplicationCommandInteractionDataOption> convertAppCommandInteractionDataOptions(vector<ApplicationCommandInteractionDataOption> originalOptions);

    struct ApplicationCommandInteractionDataOption {
        operator DiscordCoreInternal::ApplicationCommandInteractionDataOption() {
            DiscordCoreInternal::ApplicationCommandInteractionDataOption newData;
            newData.name = this->name;
            newData.options = convertAppCommandInteractionDataOptions(this->options);
            newData.type = (DiscordCoreInternal::ApplicationCommandOptionType)this->type;
            newData.valueBool = this->valueBool;
            newData.valueInt = this->valueInt;
            newData.valueString = this->valueString;
            return newData;
        }
        string name;
        ApplicationCommandOptionType type;
        bool valueBool;
        string valueString;
        int valueInt;
        vector<ApplicationCommandInteractionDataOption> options;
    };

    vector<DiscordCoreInternal::ApplicationCommandInteractionDataOption> convertAppCommandInteractionDataOptions(vector<ApplicationCommandInteractionDataOption> originalOptions) {
        vector<DiscordCoreInternal::ApplicationCommandInteractionDataOption> newVector;
        for (auto value : originalOptions) {
            DiscordCoreInternal::ApplicationCommandInteractionDataOption newItem = value;
            newItem.options = convertAppCommandInteractionDataOptions(value.options);
            newVector.push_back(newItem);
        }
        return newVector;
    }

    struct ApplicationCommandInteractionData {
        operator DiscordCoreInternal::ApplicationCommandInteractionData() {
            DiscordCoreInternal::ApplicationCommandInteractionData newData;
            newData.componentType = this->componentType;
            newData.customId = this->customId;
            newData.id = this->id;
            newData.name = this->name;
            for (auto value : this->options) {
                newData.options.push_back(value);
            }
            newData.resolved = this->resolved;
            return newData;
        }
        string name;
        string id;
        ApplicationCommandInteractionDataResolved resolved;
        vector<ApplicationCommandInteractionDataOption> options;
        string customId;
        int componentType;
    };

    struct ChannelMentionData {
        operator DiscordCoreInternal::ChannelMentionData() {
            DiscordCoreInternal::ChannelMentionData newData;
            newData.guildId = this->guildId;
            newData.id = this->id;
            newData.name = this->name;
            newData.type = this->type;
            return newData;
        }
        string id;
        string guildId;
        int type;
        string name;
    };

    struct ReactionData {
        operator DiscordCoreInternal::ReactionData() {
            DiscordCoreInternal::ReactionData newData;
            newData.channelId = this->channelId;
            newData.count = this->count;
            newData.emoji = this->emoji;
            newData.guildId = this->guildId;
            newData.me = this->me;
            newData.member = this->member;
            newData.messageId = this->messageId;
            newData.userId = this->userId;
            return newData;
        }
        int count;
        bool me;
        EmojiData emoji;
        string userId;
        string channelId;
        string messageId;
        string guildId;
        GuildMemberData member;
    };

    enum class MessageType {
        DEFAULT = 0,
        RECIPIENT_ADD = 1,
        RECIPIENT_REMOVE = 2,
        CALL = 3,
        CHANNEL_NAME_CHANGE = 4,
        CHANNEL_ICON_CHANGE = 5,
        CHANNEL_PINNED_MESSAGE = 6,
        GUILD_MEMBER_JOIN = 7,
        USER_PREMIUM_GUILD_SUBSCRIPTION = 8,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_1 = 9,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_2 = 10,
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_3 = 11,
        CHANNEL_FOLLOW_ADD = 12,
        GUILD_DISCOVERY_DISQUALIFIED = 14,
        GUILD_DISCOVERY_REQUALIFIED = 15,
        GUILD_DISCOVERY_GRACE_PERIOD_INITIAL_WARNING = 16,
        GUILD_DISCOVERY_GRACE_PERIOD_FINAL_WARNING = 17,
        THREAD_CREATED = 18,
        REPLY = 19,
        APPLICATION_COMMAND = 20,
        THREAD_STARTER_MESSAGE = 21,
        GUILD_INVITE_REMINDER = 22
    };

    struct MessageActivityData {
        int type;
        string partyId;
    };

    struct MessageInteractionData {
        operator DiscordCoreInternal::MessageInteractionData() {
            DiscordCoreInternal::MessageInteractionData newData;
            newData.id = this->id;
            newData.name = this->name;
            newData.type = (DiscordCoreInternal::InteractionType)this->type;
            newData.user = this->user;
            return newData;
        }
        string id;
        InteractionType type;
        string name;
        UserData user;
    };

    struct MessageStickerData {
        operator DiscordCoreInternal::MessageStickerData() {
            DiscordCoreInternal::MessageStickerData newData;
            newData.asset = this->asset;
            newData.description = this->description;
            newData.formatType = this->formatType;
            newData.id = this->id;
            newData.name = this->name;
            newData.packId = this->packId;
            newData.tags = this->tags;
            newData.available = this->available;
            newData.guildId = this->guildId;
            newData.sortValue = this->sortValue;
            newData.user = this->user;
            return newData;
        }
        string id;
        string packId;
        string name;
        string description;
        string tags;
        string asset;
        int formatType;
        bool available;
        string guildId;
        UserData user;
        int sortValue;
    };

    struct TeamMembersObjectData {
        operator DiscordCoreInternal::TeamMembersObjectData() {
            DiscordCoreInternal::TeamMembersObjectData newData;
            newData.membershipState = this->membershipState;
            newData.permissions = this->permissions;
            newData.teamId = this->teamId;
            newData.user = this->user;
            return newData;
        }
        int membershipState;
        vector<string> permissions;
        string teamId;
        UserData user;
    };

    struct TeamObjectData {
        operator DiscordCoreInternal::TeamObjectData() {
            DiscordCoreInternal::TeamObjectData newData;
            newData.icon = this->icon;
            newData.id = this->id;
            newData.ownerUserId = this->ownerUserId;
            return newData;
        }
        string icon;
        string id;
        vector<TeamMembersObjectData> members;
        string ownerUserId;
    };

    struct ApplicationData {
        operator DiscordCoreInternal::ApplicationData() {
            DiscordCoreInternal::ApplicationData newData;
            newData.botPublic = this->botPublic;
            newData.botRequireCodeGrant = this->botRequireCodeGrant;
            newData.coverImage = this->coverImage;
            newData.description = this->description;
            newData.flags = this->flags;
            newData.guildId = this->guildId;
            newData.icon = this->icon;
            newData.id = this->id;
            newData.name = this->name;
            newData.owner = this->owner;
            newData.primarySkuId = this->primarySkuId;
            newData.privacyPolicyUrl = this->privacyPolicyUrl;
            newData.rpcOrigins = this->rpcOrigins;
            newData.slug = this->slug;
            newData.summary = this->summary;
            newData.team = this->team;
            newData.termsOfServiceUrl = this->termsOfServiceUrl;
            newData.verifyKey = this->verifyKey;
            return newData;
        }
        string id;
        string name;
        string icon;
        string description;
        vector<string> rpcOrigins;
        bool botPublic;
        bool botRequireCodeGrant;
        string termsOfServiceUrl;
        string privacyPolicyUrl;
        UserData owner;
        string  summary;
        string verifyKey;
        TeamObjectData team;
        string guildId;
        string primarySkuId;
        string slug;
        string coverImage;
        int flags{ 0 };
    };

    enum class MessageStickerItemType {
        PNG = 1,
        APNG = 2,
        LOTTIE = 3
    };

    struct MessageStickerItemData {
        operator DiscordCoreInternal::MessageStickerItemData() {
            DiscordCoreInternal::MessageStickerItemData newData;
            newData.formatType = (DiscordCoreInternal::MessageStickerItemType)this->formatType;
            newData.id = this->id;
            newData.name = this->name;
            return newData;
        }
        string id;
        string name;
        MessageStickerItemType formatType;
    };

    struct MessageDataOld {
        operator DiscordCoreInternal::MessageDataOld() {
            DiscordCoreInternal::MessageDataOld newData;
            newData.activity.partyId = this->activity.partyId;
            newData.activity.type = this->activity.type;
            newData.application = this->application;
            for (auto value : this->attachments) {
                newData.attachments.push_back(value);
            }
            newData.author = this->author;
            newData.channelId = this->channelId;
            for (auto value : this->components) {
                newData.components.push_back(value);
            }
            newData.content = this->content;
            newData.editedTimestamp = this->editedTimestamp;
            for (auto value : this->embeds) {
                newData.embeds.push_back(value);
            }
            newData.flags = this->flags;
            newData.guildId = this->guildId;
            newData.id = this->id;
            newData.interaction = this->interaction;
            newData.member = this->member;
            for (auto value : this->mentionChannels) {
                newData.mentionChannels.push_back(value);
            }
            newData.mentionEveryone = this->mentionEveryone;
            newData.mentionRoles = this->mentionRoles;
            for (auto value : this->mentions) {
                newData.mentions.push_back(value);
            }
            newData.messageReference = this->messageReference;
            newData.nonce = this->nonce;
            newData.pinned = this->pinned;
            for (auto value : this->reactions) {
                newData.reactions.push_back(value);
            }
            for (auto value : this->stickers) {
                newData.stickers.push_back(value);
            }
            for (auto value : this->stickerItems) {
                newData.stickerItems.push_back(value);
            }
            newData.timestamp = this->timestamp;
            newData.tts = this->tts;
            newData.type = (DiscordCoreInternal::MessageType)this->type;
            newData.webhookId = this->webhookId;
            return newData;
        }
        string id;
        string channelId;
        string guildId;
        UserData author;
        GuildMemberData member;
        string content;
        string timestamp;
        string timestampRaw;
        string editedTimestamp;
        bool tts;
        bool mentionEveryone;
        vector<UserData> mentions;
        vector<string> mentionRoles;
        vector<ChannelMentionData> mentionChannels;
        vector<AttachmentData> attachments;
        vector<EmbedData> embeds;
        vector<ReactionData> reactions;
        string nonce;
        bool pinned;
        string webhookId;
        MessageType type;
        MessageActivityData activity;
        ApplicationData application;
        string applicationId;
        MessageReferenceData messageReference;
        int flags{ 0 };
        vector<MessageStickerData> stickers;
        MessageInteractionData interaction;
        vector<ActionRowData> components;
        ChannelData thread;
        vector<MessageStickerItemData> stickerItems;
    };

    struct MessageData : MessageDataOld {
        operator DiscordCoreInternal::MessageData() {
            DiscordCoreInternal::MessageData newData;
            newData.requesterId = this->requesterId;
            newData.referencedMessage = this->referencedMessage;
            return newData;
        }
        string requesterId;
        MessageDataOld referencedMessage;
    };

    struct InteractionData {
        InteractionData() {};
        InteractionData(string requesterId) {
            this->requesterId = requesterId;
        }
        operator DiscordCoreInternal::InteractionData() {
            DiscordCoreInternal::InteractionData newData;
            newData.applicationId = this->applicationId;
            newData.channelId = this->channelId;
            newData.customId = this->customId;
            newData.data = this->data;
            newData.componentType = static_cast<DiscordCoreInternal::ComponentType>(this->componentType);
            newData.dataRaw = this->dataRaw;
            newData.guildId = this->guildId;
            newData.id = this->id;
            newData.member = this->member;
            newData.message = this->message;
            newData.token = this->token;
            newData.type = (DiscordCoreInternal::InteractionType)this->type;
            newData.user = this->user;
            newData.version = this->version;
            newData.requesterId = this->requesterId;
            newData.values = this->values;
            return newData;
        }
        string id;
        string applicationId;
        InteractionType type;
        ApplicationCommandInteractionData data;
        ComponentType componentType;
        json dataRaw;
        string guildId;
        string channelId;
        GuildMemberData member;
        string name;
        UserData user;
        string token;
        int version;
        MessageData message;
        string customId;
        vector<string> values;
        string requesterId;
    };

    struct PartyData {
        operator DiscordCoreInternal::PartyData() {
            DiscordCoreInternal::PartyData newData;
            newData.id = this->id;
            newData.size[0] = this->size[0];
            newData.size[1] = this->size[1];
            return newData;
        }
        string id;
        vector<int> size{ 0, 0 };
    };

    struct AssetsData {
        operator DiscordCoreInternal::AssetsData() {
            DiscordCoreInternal::AssetsData newData;
            newData.largeImage = this->largeImage;
            newData.largeText = this->largeText;
            newData.smallImage = this->smallImage;
            newData.smallText = this->smallText;
            return newData;
        }
        string largeImage;
        string largeText;
        string smallImage;
        string smallText;
    };

    struct SecretsData {
        operator DiscordCoreInternal::SecretsData() {
            DiscordCoreInternal::SecretsData newData;
            newData.join = this->join;
            newData.match = this->match;
            newData.spectate = this->spectate;
            return newData;
        }
        string join;
        string spectate;
        string match;
    };

    struct TimestampData {
        operator DiscordCoreInternal::TimestampData() {
            DiscordCoreInternal::TimestampData newData;
            newData.end = this->end;
            newData.start = this->start;
            return newData;
        }
        __int64 start;
        __int64 end;
    };

    struct ButtonData {
        operator DiscordCoreInternal::ButtonData() {
            DiscordCoreInternal::ButtonData newData;
            newData.label = this->label;
            newData.url = this->url;
            return newData;
        }
        string label;
        string url;
    };

    enum class ActivityType {
        Game = 0,
        Streaming = 1,
        Listening = 2,
        Watching = 3,
        Custom = 4,
        Competing = 5
    };

    struct ActivityData {
        operator DiscordCoreInternal::ActivityData() {
            DiscordCoreInternal::ActivityData newData;
            newData.applicationId = this->applicationId;
            newData.assets = this->assets;
            newData.createdAt = this->createdAt;
            newData.details = this->details;
            newData.emoji = this->emoji;
            newData.flags = this->flags;
            newData.instance = this->instance;
            newData.party = this->party;
            newData.secrets = this->secrets;
            newData.state = this->state;
            newData.timestamps = this->timestamps;
            newData.type = (int)this->type;
            newData.url = this->url;
            newData.name = this->name;
            newData.buttons = this->buttons;
            return newData;
        }
        int createdAt = 0;
        TimestampData timestamps;
        string applicationId = "";
        string details = "";
        string state = "";
        EmojiData emoji;
        PartyData party;
        AssetsData assets;
        SecretsData secrets;
        bool instance = false;
        int flags = 0;
        string name = "";
        ActivityType type;
        string url = "";
        ButtonData buttons;
    };

    struct ClientStatusData {
        string desktop;
        string mobile;
        string web;
    };

    enum class PremiumTier {
        NONE = 0,
        TIER_2 = 2,
        TIER_1 = 1,
        TIER_3 = 3
    };

    enum class DefaultMessageNotificationLevel {
        ALL_MESSAGES = 0,
        ONLY_MENTIONS = 1
    };

    enum class ExplicitContentFilterLevel {
        DISABLED = 0,
        MEMBERS_WITHOUT_ROLES = 1,
        ALL_MEMBERS = 2
    };

    enum class MFALevel {
        NONE = 0,
        ELEVATED = 1
    };

    enum class VerificationLevel {
        NONE = 0,
        LOW = 1,
        MEDIUM = 2,
        HIGH = 3,
        VERY_HIGH = 4
    };

    struct WelcomeScreenChannelData {
        string channelId;
        string description;
        string emojiId;
        string emojiName;
    };

    struct WelcomeScreenData {
        string description;
        vector<WelcomeScreenChannelData> welcomeChannels{};
    };

    struct PresenceUpdateData {
        UserData user;
        string guildId;
        string status;
        vector<ActivityData> activities;
        ClientStatusData clientStatus;
    };

    struct StageInstanceData {
        string id;
        string guildId;
        string channelId;
        string topic;
        int privacyLevel;
        bool discoverableDisabled;
    };

    struct GuildData {
        string icon;
        string name;
        string id;
        string iconHash;
        string splash;
        string discoverySplash;
        string preferredLocale;
        string publicUpdatesChannelID;
        string vanityURLCode;
        string description;
        string banner;
        string ruleChannelID;
        string applicationID;
        string createdAt;
        string joinedAt;
        string widgetChannelID;
        string systemChannelID;
        string region;
        string afkChannelID;
        string ownerID;
        vector<string> features{};
        vector<ChannelData> threads;
        string permissions;
        bool owner{ false };
        int afkTimeOut{ 0 };
        bool widgetEnabled{ false };
        VerificationLevel verificationLevel = VerificationLevel::NONE;
        DefaultMessageNotificationLevel defaultMessageNotifications = DefaultMessageNotificationLevel::ALL_MESSAGES;
        ExplicitContentFilterLevel explicitContentFilter = ExplicitContentFilterLevel::DISABLED;
        vector<EmojiData> emoji{};
        vector<RoleData> roles{};
        int systemChannelFlags{ 0 };
        MFALevel mfaLevel = MFALevel::NONE;
        bool large{ false };
        vector<VoiceStateData> voiceStates{};
        vector<PresenceUpdateData> presences{};
        int maxPresences{ 0 };
        int maxMembers{ 0 };
        int premiumSubscriptionCount{ 0 };
        PremiumTier premiumTier = PremiumTier::NONE;
        int maxVideoChannelUsers{ 0 };
        int approximateMemberCount{ 0 };
        bool unavailable{ false };
        int memberCount{ 0 };
        int approximatePresenceCount{ 0 };
        int nsfwLevel{ 0 };
        WelcomeScreenData welcomeScreen{};
        vector<GuildMemberData> members{};
        vector<ChannelData> channels{};
        vector<StageInstanceData> stageInstances{};
    };

    struct ReactionAddData {
        DiscordCoreClient* discordCoreClient{ nullptr };
        GuildMemberData member;
        string channelId;
        string messageId;
        EmojiData emoji;
        string guildId;
        string userId;
    };

    struct ReactionRemoveData {
        DiscordCoreClient* discordCoreClient{ nullptr };
        string channelId;
        string messageId;
        EmojiData emoji;
        string guildId;
        string userId;
    };

    struct InteractionResponseData {
        InteractionApplicationCommandCallbackData data;
        InteractionCallbackType type;
    };

    struct InteractionPackageData {
        string applicationId;
        string interactionId;
        string interactionToken;
    };

    struct MessagePackageData {
        string channelId;
        string messageId;
    };

    enum class InputEventType {
        SLASH_COMMAND_INTERACTION = 1,
        BUTTON_INTERACTION = 2,
        REGULAR_MESSAGE = 3,
        SELECT_MENU_INPUT = 4,
        MESSAGE_COMMAND_INTERACTION = 5,
        USER_COMMAND_INTERACTION = 6
    };

    enum class ApplicationCommandType {
        CHAT_INPUT = 1,
        USER = 2,
        MESSAGE = 3
    };

    struct UserCommandInteractionData {
        string applicationId;
        string channelId;
        string menuId;
        string name;
        GuildMemberData members;
        GuildMemberData member;
        UserData users;
        string targetId;
        ApplicationCommandType type;
        string guildId;
        string token;
        string interactionId;
        int version;
    };

    struct MessageCommandInteractionData {
        string applicationId;
        string channelId;
        string menuId;
        string name;
        MessageData messages;
        string targetId;
        GuildMemberData member;
        ApplicationCommandType type;
        string guildId;
        string token;
        string interactionId;
        int version;
    };

    struct InputEventData {
        InputEventData() {}
        InputEventData(MessageData messageData, InteractionData interactionData, InputEventType eventType) {
            this->messageData = messageData;
            this->interactionData = interactionData;
            this->eventType = eventType;
            if (this->interactionData.user.id != "") {
                this->messageData.author = this->interactionData.user;
            }
            else {
                this->interactionData.user = this->messageData.author;
            }
            if (this->messageData.member.user.id == "") {
                this->messageData.member = this->interactionData.member;
            }
            else {
                this->interactionData.member = this->messageData.member;
            }
            if (this->messageData.channelId == "") {
                this->messageData.channelId = this->interactionData.channelId;
            }
            else {
                this->interactionData.channelId = this->messageData.channelId;
            }
            if (this->messageData.id == "") {
                this->messageData.id = this->interactionData.message.id;
            }
            else {
                this->interactionData.message.id = this->messageData.id;
            }
            if (this->messageData.guildId == "") {
                this->messageData.guildId = this->interactionData.guildId;
            }
            else {
                this->interactionData.guildId = this->messageData.guildId;
            }
            if (this->interactionData.id == "") {
                this->interactionData.id = this->messageData.interaction.id;
            }
            if (this->messageData.requesterId != "") {
                this->requesterId = this->messageData.requesterId;
            }
            else {
                this->requesterId = this->interactionData.requesterId;
            }
        }
        shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
        InputEventType eventType;
        InputEventResponseType inputEventResponseType;
        InteractionData getInteractionData() {
            return this->interactionData;
        }
        string getMessageContent() {
            if (this->interactionData.message.content != "") {
                return this->interactionData.message.content;
            }
            else {
                return this->messageData.content;
            }
        }
        vector<ActionRowData> getComponents() {
            if (this->interactionData.message.components.size() > 0) {
                if (this->interactionData.message.components.at(0).components.at(0).customId != "") {
                    return this->interactionData.message.components;
                }
            }
            else {
                return this->messageData.components;
            }
            return vector<ActionRowData>();
        }
        vector<EmbedData> getEmbeds() {
            if (this->interactionData.message.embeds.size() > 0) {
                if (this->interactionData.message.embeds.at(0).description != "") {
                    return this->interactionData.message.embeds;
                }
                else if (this->interactionData.message.embeds.at(0).fields.size() > 0) {
                    if (this->interactionData.message.embeds.at(0).fields.at(0).value != "") {
                        return this->interactionData.message.embeds;
                    }
                }
            }
            else {
                return this->messageData.embeds;
            }
            return vector<EmbedData>();
        }
        string getApplicationId() {
            if (this->interactionData.applicationId == "") {
                return this->messageData.application.id;
            }
            else if(this->interactionData.applicationId !=""){
                return this->interactionData.applicationId;
            }
            else if (this->messageCommandInteractionData.applicationId != "") {
                return this->messageCommandInteractionData.applicationId;
            }
            else {
                return this->userCommandInteractionData.applicationId;
            }
        }
        string getInteractionToken() {
            if (this->interactionData.token != "") {
                return this->interactionData.token;
            }
            else if (this->messageCommandInteractionData.token != "") {
                return this->messageCommandInteractionData.token;
            }
            else {
                return this->userCommandInteractionData.token;
            }            
        }
        string getInteractionId() {
            if (this->interactionData.id == "") {
                return this->messageData.interaction.id;
            }
            else if (this->interactionData.id != "") {
                return this->interactionData.id;
            }
            else if (this->messageCommandInteractionData.interactionId != "") {
                return this->messageCommandInteractionData.interactionId;
            }
            else {
                return this->userCommandInteractionData.interactionId;
            }
        }
        string getUserName() {
            if (this->messageData.author.username == "" && this->interactionData.member.user.username != "") {
                return this->interactionData.member.user.username;
            }
            else if (this->interactionData.member.user.username == "" && this->interactionData.user.username != "") {
                return this->interactionData.user.username;
            }
            else if (this->messageData.author.username != "") {
                return this->messageData.author.username;
            }
            else if (this->messageCommandInteractionData.member.user.username != "") {
                return this->messageCommandInteractionData.member.user.username;
            }
            else {
                return this->userCommandInteractionData.member.user.username;
            }
        }
        string getAvatarURL() {
            if (this->messageData.author.avatar == "" && this->interactionData.member.user.avatar != "") {
                return this->interactionData.member.user.avatar;
            }
            else if (this->interactionData.member.user.avatar == "" && this->interactionData.user.avatar != "") {
                return this->interactionData.user.avatar;
            }
            else if (this->messageData.author.avatar != "") {
                return this->messageData.author.avatar;
            }
            else if (this->messageCommandInteractionData.member.user.avatar != "") {
                return this->messageCommandInteractionData.member.user.avatar;
            }
            else {
                return this->userCommandInteractionData.member.user.avatar;
            }
        }
        string getChannelId() {
            if (this->interactionData.channelId == "") {
                return this->messageData.channelId;
            }
            else if (this->interactionData.channelId != "") {
                return this->interactionData.channelId;
            }
            else if (this->messageCommandInteractionData.channelId !="") {
                return this->messageCommandInteractionData.channelId;
            }
            else {
                return this->userCommandInteractionData.channelId;
            }
        }
        string getMessageId() {
            if (this->messageData.id == "") {
                return this->interactionData.message.id;
            }
            else {
                return this->messageData.id;
            }
        }
        string getAuthorId() {
            if (this->messageData.author.id == "") {
                return this->interactionData.user.id;
            }
            else if (this->messageData.author.id !=""){
                return this->messageData.author.id;
            }
            else if (this->messageCommandInteractionData.member.user.id != "") {
                return this->messageCommandInteractionData.member.user.id;
            }
            else {
                return this->userCommandInteractionData.member.user.id;
            }
        }
        string getGuildId() {
            if (this->messageData.guildId == "") {
                return this->interactionData.guildId;
            }
            else if(this->messageData.guildId != "") {
                return this->messageData.guildId;
            }
            else if (this->userCommandInteractionData.guildId != "") {
                return this->userCommandInteractionData.guildId;
            }
            else {
                return this->messageCommandInteractionData.guildId;
            }
        }
        MessageData getMessageData() {
            return this->messageData;
        }
        string getRequesterId() {
            return this->requesterId;
        }
    protected:
        friend class CommandCenter;
        friend class InputEventStuff;
        friend struct CommandData;
        friend class DiscordCoreClient;
        friend class EventHandler;
        InteractionData interactionData;
        UserCommandInteractionData userCommandInteractionData;
        MessageCommandInteractionData messageCommandInteractionData;
        MessageData messageData;
        string requesterId;
    };

    struct WebhookData {
        string id;
        unsigned int type;
        string guildId;
        string channelId;
        UserData user;
        string name;
        string avatar;
        string token;
        string applicationId;
        GuildData sourceGuild;
        ChannelData sourceChannel;
        string url;
    };

    struct AccountData {
        string id;
        string name;
    };

    struct IntegrationData {
        string id;
        string name;
        string type;
        bool enabled;
        bool syncing;
        string roleId;
        bool enableEmoticons;
        unsigned int expireBehavior;
        unsigned int expireGracePeriod;
        UserData user;
        AccountData account;
        string syncedAt;
        unsigned int subscriberCount;
        bool revoked;
        ApplicationData application;
    };

    enum class AuditLogEvent {
        GUILD_UPDATE = 1,
        CHANNEL_CREATE = 10,
        CHANNEL_UPDATE = 11,
        CHANNEL_DELETE = 12,
        CHANNEL_OVERWRITE_CREATE = 13,
        CHANNEL_OVERWRITE_UPDATE = 14,
        CHANNEL_OVERWRITE_DELETE = 15,
        MEMBER_KICK = 20,
        MEMBER_PRUNE = 21,
        MEMBER_BAN_ADD = 22,
        MEMBER_BAN_REMOVE = 23,
        MEMBER_UPDATE = 24,
        MEMBER_ROLE_UPDATE = 25,
        MEMBER_MOVE = 26,
        MEMBER_DISCONNECT = 27,
        BOT_ADD = 28,
        ROLE_CREATE = 30,
        ROLE_UPDATE = 31,
        ROLE_DELETE = 32,
        INVITE_CREATE = 40,
        INVITE_UPDATE = 41,
        INVITE_DELETE = 42,
        WEBHOOK_CREATE = 50,
        WEBHOOK_UPDATE = 51,
        WEBHOOK_DELETE = 52,
        EMOJI_CREATE = 60,
        EMOJI_UPDATE = 61,
        EMOJI_DELETE = 62,
        MESSAGE_DELETE = 72,
        MESSAGE_BULK_DELETE = 73,
        MESSAGE_PIN = 74,
        MESSAGE_UNPIN = 75,
        INTEGRATION_CREATE = 80,
        INTEGRATION_UPDATE = 81,
        INTEGRATION_DELETE = 82
    };

    struct AuditLogEntryInfoData {
        string deleteMemberDays;
        string membersRemoved;
        string channelId;
        string messageId;
        string count;
        string id;
        string type;
        string roleName;
    };

    struct AuditLogChangeData {
        string newValueString{ "" };
        string oldValueString{ "" };
        string key;
    };

    struct AuditLogEntryData {
        string targetId;
        vector<AuditLogChangeData> changes;
        string userId;
        string id;
        AuditLogEvent actionType;
        AuditLogEntryInfoData options;
        string reason;
    };

    struct AuditLogData {
        auto getAuditLogData(string userIdOfChanger, AuditLogEvent auditLogType) {
            for (auto value : this->auditLogEntries) {
                if (value.id == userIdOfChanger && value.actionType == auditLogType) {
                    return value;
                }
            }
        }
        auto getAuditLogData(AuditLogEvent auditLogType, string userIdOfTarget) {
            for (auto value : this->auditLogEntries) {
                if (value.targetId == userIdOfTarget && value.actionType == auditLogType) {
                    return value;
                }
            }
        }
        vector<WebhookData> webhooks;
        vector<UserData> users;
        vector<AuditLogEntryData> auditLogEntries;
        vector<IntegrationData> integrations;
        vector<ChannelData> threads;
    };

    struct InviteData {
        string channelId;
        string guildId;
        ChannelData channel;
        string code;
        string createdAt;
        string expiresAt;
        GuildData guild;
        UserData inviter;
        int maxAge = 0;
        int maxUses = 0;
        int targetType = 0;
        UserData targetUser;
        int approximatePresenceCount;
        int approximateMemberCount;
        ApplicationData targetApplication;
        StageInstanceData stageInstance;
        bool temporary;
        int uses = 0;
    };

    struct TypingStartData {
        std::string channelId;
        std::string guildId;
        std::string userId;
        int timestamp;
        GuildMemberData member;
    };

    struct YouTubeFormat {
        int itag = 0;
        string mimeType = "";
        string codecs = "";
        int bitrate = 0;
        int width = 0;
        int height = 0;
        string quality = "";
        int fps = 0;
        string audioQuality = "";
        string signatureCipher = "";
        string audioSampleRate = "";
        int averageBitrate = 0;
        __int64 contentLength = 0;
        string downloadURL = "";
        string signature = "";
        string aitags = "";
    };

    struct CreateGuildBanData {
        string guildId;
        string guildMemberId;
        int deleteMessageDays;
        string reason;
    };

    struct BanData {
        string reason;
        UserData user;
        bool failedDueToPerms = false;
    };

    struct RawFrame {
        vector<uint8_t> data{};
        uint32_t sampleCount = 0;
    };

    struct YouTubeSearchResult {
        string videoId = "";
        string thumbNailURL = "";
        string videoTitle = "";
        string duration = "";
        string description = "";
        string videoURL = "";
        vector<YouTubeFormat> formats{};
    };

    struct YouTubeSong {
        vector<RawFrame> frames{};
        string imageURL = "";
        string title = "";
        string formatDownloadURL = "";
        string url = "";
        string description = "";
        string duration = "";
        string videoId = "";
    };

    struct YouTubeSongDB {
        string imageURL = "";
        string title = "";
        string formatDownloadURL = "";
        string url = "";
        string description = "";
        string duration = "";
        string videoId = "";
        int contentLength = 0;
    };

    struct Playlist {
        string getVideoId() {
            if (this->currentSong.videoId != "") {
                return this->currentSong.videoId;
            }
            else if (this->songs.size() > 0) {
                return this->songs.at(0).videoId;
            }
            else {
                return string();
            }
        }
        string getVideoURL() {
            if (this->currentSong.url != "") {
                return this->currentSong.url;
            }
            else if (this->songs.size() > 0) {
                return this->songs.at(0).url;
            }
            else {
                return string();
            }
        }
        string getImageURL() {
            if (this->currentSong.imageURL != "") {
                return this->currentSong.imageURL;
            }
            else if (this->songs.size() > 0) {
                return this->songs.at(0).imageURL;
            }
            else {
                return string();
            }
        }
        string getDuration() {
            if (this->currentSong.duration != "") {
                return this->currentSong.duration;
            }
            else if (this->songs.size() > 0) {
                return this->songs.at(0).duration;
            }
            else {
                return string();
            }
        }
        string getDescription() {
            if (this->currentSong.description != "") {
                return this->currentSong.description;
            }
            else if (this->songs.size() > 0) {
                return this->songs.at(0).description;
            }
            else {
                return string();
            }
        }
        string getTitle() {
            if (this->currentSong.title != "") {
                return this->currentSong.title;
            }
            else if (this->songs.size() > 0) {
                return this->songs.at(0).title;
            }
            else {
                return string();
            }
        }
        vector<YouTubeSongDB> songs{};
        YouTubeSongDB currentSong{};
        string voiceChannelId = "";
        string textChannelId = "";
        bool loopAll = false;
        bool loopSong = false;
    };

    struct SendNextSongReturnData {
        Playlist dataPackage{};
        bool didItSend = false;
    };

    static string commandPrefix;

    struct RepeatedFunctionData {
        function<void(shared_ptr<DiscordCoreClient>)> function;
        bool repeated;
        int intervalInMs;
    };

    struct CommandData {
        CommandData(InputEventData inputEventData) {
            this->eventData = inputEventData;
            if (inputEventData.userCommandInteractionData.name != "") {
                this->commandName = convertToLowerCase(inputEventData.userCommandInteractionData.name);
                if (inputEventData.userCommandInteractionData.users.id != "") {
                    this->optionsArgs.push_back(inputEventData.userCommandInteractionData.users.id);
                }
                else {
                    this->optionsArgs.push_back(inputEventData.userCommandInteractionData.members.user.id);
                }
            }
            else if (inputEventData.messageCommandInteractionData.name != "") {
                this->commandName = convertToLowerCase(inputEventData.messageCommandInteractionData.name);
                this->optionsArgs.push_back(inputEventData.messageCommandInteractionData.messages.id);
            }
        }
        vector<string> optionsArgs;
        string commandName;
        InputEventData eventData;
    };

    string constructStringContent(CommandData commandData) {
        string finalCommandString;
        finalCommandString = commandData.commandName + " = ";
        for (auto& value : commandData.optionsArgs) {
            finalCommandString += value + ", ";
        }
        return finalCommandString;
    };

    // Interaction Stuff.

    struct ButtonInteractionData {
        ButtonInteractionData() {}
        ButtonInteractionData(const ButtonInteractionData& dataPackage) {
            this->applicationId = dataPackage.applicationId;
            this->channelId = dataPackage.channelId;
            this->customId = dataPackage.customId;
            this->guildId = dataPackage.guildId;
            this->id = dataPackage.id;
            this->isItForHere = dataPackage.isItForHere;
            this->member = dataPackage.member;
            this->message = dataPackage.message;
            this->token = dataPackage.token;
            this->type = dataPackage.type;
            this->user = dataPackage.user;
        }
        bool isItForHere;
        string token;
        string id;
        string applicationId;
        InteractionType type;
        string customId;
        GuildMemberData member;
        UserData user;
        MessageData message;
        string channelId;
        string guildId;
    };

    struct SelectMenuInteractionData {
        SelectMenuInteractionData() {}
        SelectMenuInteractionData(const SelectMenuInteractionData& dataPackage) {
            this->applicationId = dataPackage.applicationId;
            this->channelId = dataPackage.channelId;
            this->customId = dataPackage.customId;
            this->guildId = dataPackage.guildId;
            this->id = dataPackage.id;
            this->isItForHere = dataPackage.isItForHere;
            this->values = dataPackage.values;
            this->member = dataPackage.member;
            this->message = dataPackage.message;
            this->token = dataPackage.token;
            this->type = dataPackage.type;
            this->user = dataPackage.user;
        }
        bool isItForHere;
        string token;
        string id;
        vector<string> values;
        string applicationId;
        InteractionType type;
        string customId;
        GuildMemberData member;
        UserData user;
        MessageData message;
        string channelId;
        string guildId;

    };

    struct DeferComponentResponseData {
        DeferComponentResponseData(InteractionData dataPackage) {
            this->type = InteractionCallbackType::DeferredUpdateMessage;
            this->interactionPackage.interactionId = dataPackage.id;
            this->interactionPackage.applicationId = dataPackage.applicationId;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->responseType = InputEventResponseType::DEFER_COMPONENT_RESPONSE;
        }
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventStuff;
        friend struct CreateInteractionResponseData;
        InteractionPackageData interactionPackage;
        InteractionCallbackType type;
        InputEventResponseType responseType;
    };

    struct CreateDeferredInteractionResponseData {
        CreateDeferredInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->type = InteractionCallbackType::DeferredChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
            this->channelId = dataPackage.getChannelId();
            this->guildId = dataPackage.getGuildId();
        }
        string requesterId;
        InteractionCallbackType type;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventStuff;
        InteractionPackageData interactionPackage;
        string guildId;
        string channelId;
    };

    struct CreateInteractionResponseData {
    public:
        CreateInteractionResponseData(SelectMenuInteractionData dataPackage) {
            this->interactionPackage.interactionId = dataPackage.id;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->type = InteractionCallbackType::ChannelMessageWithSource;
            if (dataPackage.member.user.id != "") {
                this->requesterId = dataPackage.message.member.user.id;
            }
            else {
                this->requesterId = dataPackage.user.id;
            }
        }
        CreateInteractionResponseData(ButtonInteractionData dataPackage) {
            this->interactionPackage.interactionId = dataPackage.id;
            this->interactionPackage.interactionToken = dataPackage.token;
            this->type = InteractionCallbackType::ChannelMessageWithSource;
            if (dataPackage.member.user.id != "") {
                this->requesterId = dataPackage.message.member.user.id;
            }
            else {
                this->requesterId = dataPackage.user.id;
            }
        }
        CreateInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->type = InteractionCallbackType::ChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
        }
        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.components.push_back(actionRowData);
            }
            if (this->data.components.size() < 5) {
                if (this->data.components.at(this->data.components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.name = emojiName;
                    component.emoji.id = emojiId;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.url = url;
                    component.type = ComponentType::Button;
                    this->data.components.at(this->data.components.size() - 1).components.push_back(component);
                }
                else if (this->data.components.at(this->data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.components.push_back(actionRowData);
                }
            }
        }
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->data.components.size() == 0) {
                ActionRowData actionRowData;
                this->data.components.push_back(actionRowData);
            }
            if (this->data.components.size() < 5) {
                if (this->data.components.at(this->data.components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    this->data.components.at(this->data.components.size() - 1).components.push_back(componentData);
                }
                else if (this->data.components.at(this->data.components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->data.components.push_back(actionRowData);
                }

            }
        }
        InteractionApplicationCommandCallbackData data;
        string requesterId;
        InteractionCallbackType type;
    protected:
        CreateInteractionResponseData() {}
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventStuff;
        friend class Button;
        friend class SelectMenu;
        InteractionPackageData interactionPackage;
    };

    struct CreateEphemeralInteractionResponseData {
    public:
        CreateEphemeralInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->type = InteractionCallbackType::ChannelMessageWithSource;
            this->requesterId = dataPackage.getRequesterId();
        }
        InteractionApplicationCommandCallbackData data;
        string requesterId;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventStuff;
        InteractionCallbackType type;
        InteractionPackageData interactionPackage;
    };

    struct EditInteractionResponseData {
        EditInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->type = InteractionCallbackType::UpdateMessage;
            this->requesterId = dataPackage.getRequesterId();
        }
        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.name = emojiName;
                    component.emoji.id = emojiId;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.url = url;
                    component.type = ComponentType::Button;
                    this->components.at(this->components.size() - 1).components.push_back(component);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
        }
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    this->components.at(this->components.size() - 1).components.push_back(componentData);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }

            }
        }
        string content;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        vector<AttachmentData> attachments;
        vector<ActionRowData>components;
        int flags;
        InteractionCallbackType type;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventStuff;
        InteractionPackageData interactionPackage;
        string requesterId;
    };

    struct DeleteInteractionResponseData {
        DeleteInteractionResponseData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
        }
        unsigned int timeDelay = 0;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventStuff;
        InteractionPackageData interactionPackage;
    };

    struct CreateFollowUpMessageData {
        CreateFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->requesterId = dataPackage.getRequesterId();
        }
        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.name = emojiName;
                    component.emoji.id = emojiId;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.url = url;
                    component.type = ComponentType::Button;
                    this->components.at(this->components.size() - 1).components.push_back(component);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
        }
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    this->components.at(this->components.size() - 1).components.push_back(componentData);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
        }
        string content;
        string username;
        string avatarUrl;
        bool tts;
        vector<EmbedData> embeds;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
        int flags = 0;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventStuff;
        InteractionPackageData interactionPackage;
        string requesterId;
    };

    struct EditFollowUpMessageData {
        EditFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->messagePackage.messageId = dataPackage.getMessageId();
            this->requesterId = dataPackage.getRequesterId();
        }
        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.name = emojiName;
                    component.emoji.id = emojiId;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.url = url;
                    component.type = ComponentType::Button;
                    this->components.at(this->components.size() - 1).components.push_back(component);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
        }
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    this->components.at(this->components.size() - 1).components.push_back(componentData);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
        }
        string content;
        string username;
        string avatarUrl;
        bool tts;
        vector<EmbedData> embeds;
        DiscordCoreInternal::AllowedMentionsData allowedMentions;
        vector<ActionRowData> components;
        int flags;
        string requesterId;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventStuff;
        InteractionPackageData interactionPackage;
        MessagePackageData messagePackage;
    };

    struct GetInteractionResponseData {
        string applicationId;
        string interactionToken;
    };

    struct DeleteFollowUpMessageData {
        DeleteFollowUpMessageData(InputEventData dataPackage) {
            this->interactionPackage.applicationId = dataPackage.getApplicationId();
            this->interactionPackage.interactionToken = dataPackage.getInteractionToken();
            this->interactionPackage.interactionId = dataPackage.getInteractionId();
            this->messagePackage.messageId = dataPackage.getMessageId();
        }
        unsigned int timeDelay = 0;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventStuff;
        InteractionPackageData interactionPackage;
        MessagePackageData messagePackage;
    };

    struct EditMessageData {
        EditMessageData(InputEventData dataPackage) {
            this->channelId = dataPackage.getChannelId();
            this->messageId = dataPackage.getMessageId();
            this->originalMessageData = dataPackage.getMessageData();
            this->requesterId = dataPackage.getRequesterId();
        }
        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.name = emojiName;
                    component.emoji.id = emojiId;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.url = url;
                    component.type = ComponentType::Button;
                    this->components.at(this->components.size() - 1).components.push_back(component);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
        }
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    this->components.at(this->components.size() - 1).components.push_back(componentData);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }

            }
        }
        string content = "";
        vector<EmbedData> embeds;
        int flags = 0;
        vector<AttachmentData> attachments;
        AllowedMentionsData allowedMentions;
        MessageData originalMessageData;
        vector<ActionRowData> components;
        MessageReferenceData messageReference;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventStuff;
        friend class MessageManager;
        friend class MessageManagerAgent;
        string requesterId;
        string channelId;
        string messageId;
    };

    struct CreateMessageData {
        CreateMessageData() {}
        CreateMessageData(InputEventData dataPackage) {
            this->channelId = dataPackage.getChannelId();
            this->requesterId = dataPackage.getRequesterId();
        }
        AllowedMentionsData allowedMentions;
        string content = "";
        vector<EmbedData> embeds;
        MessageReferenceData messageReference;
        vector<ActionRowData> components;
        int nonce;
        bool tts = false;
        string channelId;
    protected:
        friend class MessageManager;
        string requesterId;
    };

    struct ReplyMessageData {
        ReplyMessageData(InputEventData dataPackage) {
            this->replyingToMessageData = dataPackage.getMessageData();
            this->messageReference.channelId = dataPackage.getChannelId();
            this->messageReference.failIfNotExists = false;
            this->messageReference.guildId = dataPackage.getGuildId();
            this->messageReference.messageId = dataPackage.getMessageId();
            this->requesterId = dataPackage.getRequesterId();
        }
        void addButton(bool disabled, string customId, string buttonLabel, string emojiName, DiscordCoreAPI::ButtonStyle buttonStyle, string emojiId = "", string url = "") {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData component;
                    component.customId = customId;
                    component.disabled = disabled;
                    component.emoji.name = emojiName;
                    component.emoji.id = emojiId;
                    component.label = buttonLabel;
                    component.style = buttonStyle;
                    component.url = url;
                    component.type = ComponentType::Button;
                    this->components.at(this->components.size() - 1).components.push_back(component);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }
            }
        }
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int maxValues, int minValues) {
            if (this->components.size() == 0) {
                ActionRowData actionRowData;
                this->components.push_back(actionRowData);
            }
            if (this->components.size() < 5) {
                if (this->components.at(this->components.size() - 1).components.size() < 5) {
                    ComponentData componentData;
                    componentData.type = ComponentType::SelectMenu;
                    componentData.disabled = disabled;
                    componentData.customId = customId;
                    componentData.options = options;
                    componentData.placeholder = placeholder;
                    componentData.maxValues = maxValues;
                    componentData.minValues = minValues;
                    this->components.at(this->components.size() - 1).components.push_back(componentData);
                }
                else if (this->components.at(this->components.size() - 1).components.size() == 5) {
                    ActionRowData actionRowData;
                    this->components.push_back(actionRowData);
                }

            }
        }
        string content = "";
        bool tts = false;
        vector<EmbedData> embeds;
        AllowedMentionsData allowedMentions;
        MessageReferenceData messageReference;
        vector<ActionRowData> components;
        int nonce;
    protected:
        friend class InteractionManagerAgent;
        friend class InteractionManager;
        friend class InputEventStuff;
        friend class MessageManager;
        friend class MessageManagerAgent;
        MessageData replyingToMessageData;
        string requesterId;
    };

    // Channel Stuff.

    struct EditChannelPermissionOverwritesData {
        string allow;
        string deny;
        EditChannelPermissionOverwritesType type;
        string roleOrUserId;
        string channelId;
    };

    struct FetchChannelData {
        string channelId;
    };

    struct GetChannelData {
        string channelId;
    };

    struct DeleteChannelPermissionOverwritesData {
        string channelId;
        string roleOrUserId;
    };

    struct FetchDMChannelData {
        string userId;
    };

    // Message Stuff.

    struct SendDMData {
        SendDMData(InputEventData dataPackage) : messageData(dataPackage) {}
        string channelId;
        string userId;
        CreateMessageData messageData;
    };

    struct FetchPinnedMessagesData {
        string channelId;
    };

    struct FetchMessagesData {
        string channelId;
        unsigned int limit;
        string beforeThisId = "";
        string afterThisId = "";
        string aroundThisId = "";
    };

    struct FetchMessageData {
        string requesterId;
        string channelId;
        string id;
    };

    struct PinMessageData {
        string channelId;
        string messageId;
    };

    struct DeleteMessageData {
        unsigned int timeDelay = 0;
        string channelId;
        string messageId;
    };

    struct DeleteMessagesBulkData {
        string channelId;
        vector<string> messageIds;
    };

    // Role Stuff.
    struct UpdateRoleData {
        string guildId;
        string roleId;
        string name;
        string permissions;
        string hexColorValue;
        bool hoist;
        bool mentionable;
    };

    struct GetGuildRolesData {
        string guildId;
    };

    struct FetchRoleData {
        string guildId;
        string roleId;
    };

    struct UpdateRolePositionData {
        string guildId;
        string roleId;
        int newPosition;
    };

    struct RemoveRoleFromGuildMemberData {
        string guildId;
        string userId;
        string roleId;
    };

    struct GetRoleData {
        string guildId;
        string roleId;
    };

    struct RemoveRoleFromGuildData {
        string guildId;
        string roleId;
    };

    struct AddRoleToGuildMemberData {
        string guildId;
        string userId;
        string roleId;
    };

    struct CreateRoleData {
        string name;
        string permissions = "0";
        string hexColorValue;
        bool hoist;
        bool mentionable;
        string guildId;
        int position;
    };
};

#endif