// FoundationEntities.hpp - Header for all of the Discord/Support API data structures.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _FOUNDATION_ENTITIES_
#define _FOUNDATION_ENTITIES_

#include "IndexInitial.hpp"

namespace DiscordCoreAPI {

    struct SongCompletionEventData;
    class RespondToInputEventData;
    struct ButtonInteractionData;
    class DatabaseManagerAgent;
    class PermissionsConverter;
    class SelectMenuCollector;
    class ApplicationCommands;
    class DiscordCoreClient;
    class ButtonCollector;
    class VoiceConnection;
    class SoundCloudSong;
    class SoundCloudAPI;
    class Interactions;
    class EventHandler;
    class GuildMembers;
    struct CommandData;
    class InputEvents;
    class YouTubeSong;
    class SelectMenu;
    class YouTubeAPI;
    class Reactions;
    class Messages;
    class Channels;
    class BotUser;
    class SongAPI;
    class Guilds;
    class Roles;
    class Users;
    class Guild;

    DiscordCoreAPI_Dll bool nanoSleep(__int64 ns);

    DiscordCoreAPI_Dll void spinLock(__int64 timeInNsToSpinLockFor);

    class DiscordCoreAPI_Dll StopWatch {
    public:

        StopWatch( __int64 maxNumberOfMsNew) {
            this->maxNumberOfMs = maxNumberOfMsNew;
            this->startTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        }

        bool hasTimePassed() {
             __int64 currentTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
             __int64 elapsedTime = currentTime - this->startTime;
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

         __int64 maxNumberOfMs{ 0 };
         __int64 startTime{ 0 };
    };

    template <typename T>
    bool waitForTimeToPass(unbounded_buffer<T>* outBuffer, T* argOne, __int32 timeInMsNew) {
    StopWatch stopWatch(timeInMsNew);
    bool doWeBreak{ false };
    while (!try_receive(outBuffer, *argOne)) {
            concurrency::wait(10);
            if (stopWatch.hasTimePassed()) {
                doWeBreak = true;
                break;
            }
        };
        return doWeBreak;
    }

    template <typename ...T>
    void executeFunctionAfterTimePeriod(function<void(T...)>theFunction,  __int32 timeDelayInMs, bool isRepeating, T... args) {
        ThreadPoolTimer threadPoolTimer{ nullptr };
        if (timeDelayInMs > 0) {
            TimerElapsedHandler timeElapsedHandler = [=](ThreadPoolTimer threadPoolTimerNew)->void {
                theFunction(args...);
            };
            if (isRepeating) {
                threadPoolTimer = threadPoolTimer.CreatePeriodicTimer(timeElapsedHandler, winrt::Windows::Foundation::TimeSpan(timeDelayInMs * 10000));
            }
            else {
                threadPoolTimer = threadPoolTimer.CreateTimer(timeElapsedHandler, winrt::Windows::Foundation::TimeSpan(timeDelayInMs * 10000));
            }
        }
        else {
            theFunction(args...);
        }
    }

    class DiscordCoreAPI_Dll TimeStamp : public string {
    public:
        string getOldSchoolTimeStamp() {
            string dayVal{ "00" };
            string monthVal{ "" };
            string hourVal{ "" };
            string minuteVal{ "" };
            string secondVal{ "00" };
            string millisecondVal{ "00" };
            if (this->find(" ") != string::npos) {
                if (this->substr(this->find_first_of(" ") + 1, 3) == "Jan") {
                    monthVal = "01";
                }
                else if (this->substr(this->find_first_of(" ") + 1, 3) == "Feb") {
                    monthVal = "02";
                }
                else if (this->substr(this->find_first_of(" ") + 1, 3) == "Mar") {
                    monthVal = "03";
                }
                else if (this->substr(this->find_first_of(" ") + 1, 3) == "Apr") {
                    monthVal = "04";
                }
                else if (this->substr(this->find_first_of(" ") + 1, 3) == "May") {
                    monthVal = "05";
                }
                else if (this->substr(this->find_first_of(" ") + 1, 3) == "Jun") {
                    monthVal = "06";
                }
                else if (this->substr(this->find_first_of(" ") + 1, 3) == "Jul") {
                    monthVal = "07";
                }
                else if (this->substr(this->find_first_of(" ") + 1, 3) == "Aug") {
                    monthVal = "08";
                }
                else if (this->substr(this->find_first_of(" ") + 1, 3) == "Sep") {
                    monthVal = "09";
                }
                else if (this->substr(this->find_first_of(" ") + 1, 3) == "Oct") {
                    monthVal = "10";
                }
                else if (this->substr(this->find_first_of(" ") + 1, 3) == "Nov") {
                    monthVal = "11";
                }
                else if (this->substr(this->find_first_of(" ") + 1, 3) == "Dec") {
                    monthVal = "12";
                }
            }
            if (this->find(":") != string::npos) {
                hourVal = this->substr(this->find_first_of(":") - 2, 2);
            }
            if (this->find_last_of(":") != string::npos) {
                minuteVal = this->substr(this->find_last_of(":") - 2, 2);
            }
            if (this->find_last_of(":") != string::npos) {
                secondVal = this->substr(this->find_last_of(":") + 1, 2);
            }
            if (this->find_first_not_of("abcdefghijklmnopqrstuvwxyz ") != string::npos) {
                dayVal = this->substr(this->find_first_not_of("abcdefghijklmnopqrstuvwxyz ") + 8, 2);
            }
            struct DiscordCoreAPI_Dll tm newtime;
            __time64_t __int64_time;
            errno_t err;

            // Get time as 64-bit integer.
            _time64(&__int64_time);
            // Convert to local time.
            err = _localtime64_s(&newtime, &__int64_time);
            if (err)
            {
                printf("Invalid argument to _localtime64_s.");
                return "";
            }

            string finalValue{};
            finalValue = to_string(newtime.tm_year + 1900) + "-" + monthVal + "-" + dayVal + "T" + hourVal + ":" + minuteVal + ":" + secondVal;
            return finalValue;
        }

    };

    DiscordCoreAPI_Dll TimeStamp convertTimeInMsToDateTimeString(__int64 timeInMs);

    DiscordCoreAPI_Dll __int64 convertTimestampToInteger(string timeStamp);

    DiscordCoreAPI_Dll string convertTimeStampToNewOne(string timeStamp);

    DiscordCoreAPI_Dll void rethrowException(string stackTrace, unbounded_buffer<exception>* sendBuffer = nullptr, bool rethrow = false);

    DiscordCoreAPI_Dll string convertToLowerCase(string stringToConvert);

    DiscordCoreAPI_Dll bool hasTimeElapsed(string timeStamp, __int64 days = 0, __int64 hours = 0, __int64 minutes = 0);

    DiscordCoreAPI_Dll string convertMsToDurationString(__int32 durationInMs);

    /**
    * \addtogroup utilities
    * @{
    */
    /// Acquires a time and date stamp that is compatible with Discord Messages. \brief Acquires a time and date stamp that is compatible with Discord Messages.
    /// \returns A string containing the time/date-stamp.
    DiscordCoreAPI_Dll string getTimeAndDate();
    /**@}*/

    /** 
    * \addtogroup foundation_entities
    * @{
    */
    /// Snowflake, which is a time-based identifier for any/all of the Discord entities. \brief Snowflake, which is a time-based identifier for any/all of the Discord entities.
    typedef DiscordCoreAPI_Dll string Snowflake;

    /// Base class DiscordCoreAPI_Dll for all Discord entities. \brief Base class DiscordCoreAPI_Dll for all Discord entities.
    class DiscordCoreAPI_Dll DiscordEntity {
    public:

        Snowflake id{ "" };///< The identifier "snowflake" of the given entity.

        /// Converts the snowflake-id into a time and date stamp. \brief Converts the snowflake-id into a time and date stamps.
        /// \returns A string containing the timestamp.
        string getCreatedAtTimestamp();
    };
    /**@}*/
};

namespace  DiscordCoreInternal {

    /**
    * \addtogroup foundation_entities
    * @{
    */
    class ApplicationCommandManager;
    class BaseWebSocketAgent;
    class InteractionManagerAgent;
    class GuildMemberManagerAgent;
    class ReactionManagerAgent;
    class MessageManagerAgent;
    class StickerManagerAgent;
    class ChannelManagerAgent;
    class GuildMemberManager;
    class InteractionManager;
    class GuildManagerAgent;
    class RoleManagerAgent;
    class UserManagerAgent;
    class ReactionManager;
    class ChannelManager;
    class StickerManager;
    class MessageManager;
    class ThreadManager;
    class GuildManager;
    class RoleManager;
    class UserManager;
    class DataParser;

    struct DiscordCoreAPI_Dll AllowedMentionsData {
        bool repliedUser{ false };
        vector<string> parse{};
        vector<string> roles{};
        vector<string> users{};
    };

    struct DiscordCoreAPI_Dll MessageReferenceData {
        bool failIfNotExists{ false };
        string messageId{ "" };
        string channelId{ "" };
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll AttachmentData : public DiscordCoreAPI::DiscordEntity {
        string contentType{ "" };
        string filename{ "" };
        string proxyUrl{ "" };
        string url{ "" };
        __int32 height{ 0 };
        __int32 width{ 0 };
        __int32 size{ 0 };
    };

    struct DiscordCoreAPI_Dll EmbedFooterData {
        string proxyIconUrl{ "" };
        string iconUrl{ "" };
        string text{ "" };
    };

    struct DiscordCoreAPI_Dll EmbedImageData {
        string proxyUrl{ "" };
        string url{ "" };
        __int32 height{ 0 };
        __int32 width{ 0 };
    };

    struct DiscordCoreAPI_Dll EmbedThumbnailData {
        string proxyUrl{ "" };
        string url{ "" };
        __int32 height{ 0 };
        __int32 width{ 0 };
    };

    struct DiscordCoreAPI_Dll EmbedVideoData {
        string proxyUrl{ "" };
        string url{ "" };
        __int32 height{ 0 };
        __int32 width{ 0 };
    };

    struct DiscordCoreAPI_Dll EmbedProviderData {
        string name{ "" };
        string url{ "" };
    };

    struct DiscordCoreAPI_Dll EmbedAuthorData {
        string proxyIconUrl{ "" };
        string proxyUrl{ "" };
        string iconUrl{ "" };
        string name{ "" };
        string url{ "" };
    };

    struct DiscordCoreAPI_Dll EmbedFieldData {
        string iconUrl{ "" };
        bool Inline{ false };
        string value{ "" };
        string name{ "" };
    };

    struct DiscordCoreAPI_Dll EmbedData {

        vector<EmbedFieldData> fields{};
        EmbedThumbnailData thumbnail{};
        EmbedProviderData provider{};
        string hexColorValue{ "" };
        string description{ "" };
        EmbedFooterData footer{};
        EmbedAuthorData author{};
        string timestamp{ "" };
        EmbedImageData image{};
        EmbedVideoData video{};
        string title{ "" };
        string type{ "" };
        string url{ "" };

        EmbedData* setAuthor(string authorName, string authorAvatarURL = "") {
            this->author.name = authorName;
            this->author.iconUrl = authorAvatarURL;
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

        EmbedData* setDescription(string descriptionNew) {
            this->description = descriptionNew;
            return this;
        }

        EmbedData* setColor(string hexColorValueNew) {
            this->hexColorValue = hexColorValueNew;
            return this;
        }

        EmbedData* setThumbnail(string thumbnailURL) {
            this->thumbnail.url = thumbnailURL;
            return this;
        }

        EmbedData* setTitle(string titleNew) {
            this->title = titleNew;
            return this;
        }

        EmbedData* setImage(string imageURL) {
            this->image.url = imageURL;
            return this;
        }
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
        GUILD_protected_THREAD = 12,
        GUILD_STAGE_VOICE = 13
    };

    struct DiscordCoreAPI_Dll UserData : public DiscordCoreAPI::DiscordEntity {
        string discriminator{ "" };
        bool mfaEnabled{ false };
        bool verified{ false };
        string createdAt{ "" };
        string userName{ "" };
        bool system{ false };
        __int32 premiumType{ 0 };
        __int32 publicFlags{ 0 };
        string avatar{ "" };
        string locale{ "" };
        string email{ "" };
        bool bot{ false };
        __int32 flags{ 0 };
    };

    struct DiscordCoreAPI_Dll RoleTagsData {
        string premiumSubscriber{ "" };
        string integrationId{ "" };
        string botId{ "" };
    };

    struct DiscordCoreAPI_Dll RoleData : public DiscordCoreAPI::DiscordEntity {
        bool mentionable{ false };
        bool managed{ false };
        bool hoist{ false };
        RoleTagsData tags{};
        string permissions;
        string name{ "" };
        __int32 position{ 0 };
        __int32 color{ 0 };
    };

    struct DiscordCoreAPI_Dll GuildMemberData {
        string premiumSince{ "" };
        string permissions{ "" };
        string userMention{ "" };
        vector<string> roles{};
        bool pending{ false };
        string joinedAt{ "" };
        string guildId{ "" };
        bool deaf{ false };
        bool mute{ false };
        string nick{ "" };
        UserData user{};
    };

    struct DiscordCoreAPI_Dll WelcomeScreenChannelData {
        string description{ "" };
        string channelId{ "" };
        string emojiName{ "" };
        string emojiId{ "" };
    };

    struct DiscordCoreAPI_Dll WelcomeScreenData {
        vector<WelcomeScreenChannelData> welcomeChannels{};
        string description{};
    };

    struct DiscordCoreAPI_Dll PartyData : public DiscordCoreAPI::DiscordEntity {
        vector<__int32> size{ 0, 0 };
    };

    struct DiscordCoreAPI_Dll AssetsData {
        string largeImage{ "" };
        string smallImage{ "" };
        string largeText{ "" };
        string smallText{ "" };
    };

    struct DiscordCoreAPI_Dll SecretsData {
        string spectate{ "" };
        string match{ "" };
        string join{ "" };
    };

    struct DiscordCoreAPI_Dll TimestampData {
        __int64 start{ 0 };
        __int64 end{ 0 };
    };

    struct DiscordCoreAPI_Dll ButtonData {
        string label{ "" };
        string url{ "" };
    };

    struct DiscordCoreAPI_Dll EmojiData : public DiscordCoreAPI::DiscordEntity {
        bool requireColons{ false };
        vector<RoleData> roles{};
        bool available{ false };
        bool animated{ false };
        bool managed{ false };
        string name{ "" };
        UserData user{};
    };

    struct DiscordCoreAPI_Dll ReactionData {
        GuildMemberData member{};
        string channelId{ "" };
        string messageId{ "" };
        string guildId{ "" };
        string userId{ "" };
        EmojiData emoji{};
        bool me{ false };
        __int32 count{ 0 };
    };

    struct DiscordCoreAPI_Dll ActivityData {
        TimestampData timestamps{};
        string applicationId{ "" };
        bool instance{ false };
        SecretsData secrets{};
        string details{ "" };
        ButtonData buttons{};
        AssetsData assets{};
        string state{ "" };
        __int32 createdAt = 0;
        EmojiData emoji{};
        PartyData party{};
        string name{ "" };
        string url{ "" };
        __int32 flags{ 0 };
        __int32 type{ 0 };
    };

    struct DiscordCoreAPI_Dll ClientStatusData {
        string desktop{ "" };
        string mobile{ "" };
        string web{ "" };
    };

    struct DiscordCoreAPI_Dll UpdatePresenceData {
        vector<ActivityData> activities{};
        string status{ "" };
        __int64 since{ 0 };
        bool afk{ false };
    };

    struct DiscordCoreAPI_Dll PresenceUpdateData {
        vector<ActivityData> activities{};
        ClientStatusData clientStatus{};
        string guildId{ "" };
        string status{ "" };
        UserData user{};
    };

    enum class EditChannelPermissionOverwritesType {
        Role = 0,
        User = 1
    };

    struct DiscordCoreAPI_Dll OverWriteData : public DiscordCoreAPI::DiscordEntity {
        EditChannelPermissionOverwritesType type{};
        string channelId{ "" };
        string allow{ "" };
        string deny{ "" };
    };

    struct DiscordCoreAPI_Dll ThreadMetadataData {
        __int32 autoArchiveDuration{ 0 };
        string archiveTimestamp{ "" };
        string archiverId{ "" };
        bool archived{ false };
        bool locked{ false };
    };

    struct DiscordCoreAPI_Dll ThreadMemberData : public DiscordCoreAPI::DiscordEntity {
        string joinTimestamp{ "" };
        string userId{ "" };
        __int32 flags{ 0 };
    };

    struct DiscordCoreAPI_Dll VoiceStateData {
        string requestToSpeakTimestamp{ "" };
        GuildMemberData guildMember{};
        bool selfStream{ false };
        bool selfVideo{ false };
        string channelId{ "" };
        string sessionId{ "" };
        bool selfDeaf{ false };
        bool selfMute{ false };
        bool suppress{ false };
        string guildId{ "" };
        string userId{ "" };
        bool deaf{ false };
        bool mute{ false };
    };

    struct DiscordCoreAPI_Dll StageInstanceData : public DiscordCoreAPI::DiscordEntity {
        bool discoverableDisabled{ false };
        __int32 privacyLevel{ 0 };
        string channelId{ "" };
        string guildId{ "" };
        string topic{ "" };
    };

    enum class StickerType {
        STANDARD = 1,
        GUILD = 2
    };

    enum class StickerFormatType {
        PNG = 1,
        APNG = 2,
        LOTTIE = 3
    };

    struct DiscordCoreAPI_Dll StickerData : public DiscordCoreAPI::DiscordEntity {
        StickerFormatType formatType{};
        string description{ "" };
        bool available{ false };
        __int32 sortValue{ 0 };
        string guildId{ "" };
        string packId{ "" };
        string asset{ "" };
        StickerType type{};        
        string name{ "" };
        string tags{ "" };
        UserData user{};
    };

    class DiscordCoreAPI_Dll SchedulerWrapper {
    public:

        SchedulerWrapper(Scheduler* dataPackage) {
            this->scheduler = dataPackage;
        }
        Scheduler* scheduler{ nullptr };
    };

    class DiscordCoreAPI_Dll DispatcherQueueControllerWrapper {
    public:

        DispatcherQueueControllerWrapper(shared_ptr<DispatcherQueueController> dataPackage) {
            this->dispatcherQueueController = dataPackage;
        }
        shared_ptr<DispatcherQueueController> dispatcherQueueController{ nullptr };
    };

    class DiscordCoreAPI_Dll DispatcherQueueWrapper {
    public:

        DispatcherQueueWrapper(shared_ptr<DispatcherQueue> dataPackage) {
            this->dispatcherQueue = dataPackage;
        }
        shared_ptr<DispatcherQueue> dispatcherQueue{ nullptr };
    };

    class DiscordCoreAPI_Dll ThreadContext{
    public:

        shared_ptr<DispatcherQueueControllerWrapper> queueController{ nullptr };
        shared_ptr<DispatcherQueueWrapper> dispatcherQueue{ nullptr };
        shared_ptr<SchedulerWrapper> scheduler{ nullptr };

        ThreadContext();

        ThreadContext(ThreadContext* threadContext);

        void releaseContext();

        ~ThreadContext();
    };

    enum class HeaderTypes {
        Bot_Auth = 0
    };

    struct DiscordCoreAPI_Dll Headers {
        HeaderTypes headerType{};
        string headerValue{ "" };
    };

    struct DiscordCoreAPI_Dll HttpAgentResources {
        vector<Headers> headers{};
        string userAgent{ "" };
        string baseURL{ "" };
    };

    struct DiscordCoreAPI_Dll DeleteInteractionResponseData {
         __int32 timeDelayInMs{ 0 };
        string interactionToken{ "" };
        string applicationId{ "" };
    };

    struct DiscordCoreAPI_Dll DeleteFollowUpMessageData {
         __int32 timeDelayInMs{ 0 };
        string interactionToken{ "" };
        string applicationId{ "" };
        string messageId{ "" };
    };

    struct DiscordCoreAPI_Dll ChannelMentionData : public DiscordCoreAPI::DiscordEntity {
        ChannelType type{ -1 };
        string guildId{ "" };
        string name{ "" };        
    };

    struct DiscordCoreAPI_Dll TeamMembersObjectData {
        vector<string> permissions{};
        __int32 membershipState{ -1 };
        string teamId{ "" };
        UserData user{};
    };

    struct DiscordCoreAPI_Dll TeamObjectData : public DiscordCoreAPI::DiscordEntity {
        vector<TeamMembersObjectData> members{};
        string ownerUserId{ "" };
        string icon{ "" };
    };

    struct DiscordCoreAPI_Dll ApplicationData : public DiscordCoreAPI::DiscordEntity {
        bool botRequireCodeGrant{ false };
        string termsOfServiceUrl{ "" };
        string privacyPolicyUrl{ "" };
        vector<string> rpcOrigins{};
        string primarySkuId{ "" };
        string description{ "" };
        bool botPublic{ false };
        string coverImage{ "" };
        string verifyKey{ "" };
        string  summary{ "" };
        TeamObjectData team{};
        string guildId{ "" };
        __int32 flags{ 0 };
        string slug{ "" };
        string name{ "" };
        string icon{ "" };
        UserData owner{};
    };

    struct DiscordCoreAPI_Dll SelectOptionData {
        string description{ "" };
        bool _default{ false };
        string label{ "" };
        string value{ "" };
        EmojiData emoji{};
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

    struct DiscordCoreAPI_Dll ComponentData {
        vector<SelectOptionData> options{};
        string placeholder{ "" };
        bool disabled{ false };
        __int32 maxValues{ 0 };
        __int32 minValues{ 0 };
        string customId{ "" };
        ComponentType type{};
        ButtonStyle style{};
        string label{ "" };        
        EmojiData emoji{};
        string url{ "" };
    };

    struct DiscordCoreAPI_Dll ActionRowData {
        vector<ComponentData> components{};
    };

    struct DiscordCoreAPI_Dll InteractionApplicationCommandCallbackData {
        AllowedMentionsData allowedMentions{};
        vector<ActionRowData> components{};
        vector<EmbedData> embeds{};
        string content{ "" };
        __int32 flags{ 0 };
        bool tts{ false };       
    };

    struct DiscordCoreAPI_Dll ReactionAddEventData {
        GuildMemberData member{};
        string channelId{ "" };
        string messageId{ "" };
        string guildId{ "" };
        string userId{ "" };
        EmojiData emoji{};
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
        GET_APPLICATION_COMMANDS = 22,
        GET_SOCKET_PATH = 23,
        DELETE_APPLICATION_COMMAND = 24,
        PATCH_APPLICATION_COMMAND = 25,
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
        DELETE_LEAVE_GUILD = 54,
        SOUNDCLOUD_SEARCH = 55,
        GET_APPLICATION_COMMAND = 56,
        PUT_BULK_OVERWRITE_APPLICATION_COMMANDS = 57,
        POST_GUILD_APPLICATION_COMMAND = 58,
        GET_GUILD_APPLICATION_COMMANDS = 59,
        GET_GUILD_APPLICATION_COMMAND = 60,
        DELETE_MESSAGE_OLD = 61,
        SOUNDCLOUD_AUTH = 62,
        SOUNDCLOUD_SONG_GET = 63,
        GET_GUILD_APPLICATION_COMMAND_PERMISSIONS = 64,
        GET_APPLICATION_COMMAND_PERMISSIONS = 65,
        PUT_EDIT_APPLICATION_COMMAND_PERMISSIONS = 66,
        PUT_BATCH_EDIT_APPLICATION_COMMAND_PERMISSIONS = 67
    };

    enum class StickerItemType {
        PNG = 1,
        APNG = 2,
        LOTTIE = 3
    };

    struct DiscordCoreAPI_Dll StickerItemData : public DiscordCoreAPI::DiscordEntity {
        StickerItemType formatType{};
        string name{ "" };
    };

    enum class InteractionType {
        Ping = 1,
        ApplicationCommand = 2,
        MessageComponent = 3
    };

    struct DiscordCoreAPI_Dll MessageInteractionData : public DiscordCoreAPI::DiscordEntity {
        InteractionType type{};
        string name{ "" };
        UserData user{};
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
        MENTIONABLE = 9,
        NUMBER = 10
    };

    struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption {
        vector<ApplicationCommandInteractionDataOption> options{};
        ApplicationCommandOptionType type{};
        __int32 valueInt{ false };
        string valueString{ "" };
        bool valueBool{ false };        
        string name{ "" };
    };

    enum class InputEventType {
        SLASH_COMMAND_INTERACTION = 1,
        BUTTON_INTERACTION = 2,
        REGULAR_MESSAGE = 3,
        SELECT_MENU_INPUT = 4
    };

    enum class ApplicationCommandType {
        CHAT_INPUT = 1,
        USER = 2,
        MESSAGE = 3
    };

    enum class ApplicationCommandPermissionType {
        Role = 1,
        User = 2
    };

    struct DiscordCoreAPI_Dll ApplicationCommandPermissionData : public DiscordCoreAPI::DiscordEntity {
        ApplicationCommandPermissionType type{ ApplicationCommandPermissionType::Role };
        bool permission{ false };
    };

    struct DiscordCoreAPI_Dll GuildApplicationCommandPermissionData : public DiscordCoreAPI::DiscordEntity {
        vector<ApplicationCommandPermissionData> permissions{};
        string applicationId{ "" };
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll InteractionResponseData {
        InteractionApplicationCommandCallbackData data{};
        InteractionCallbackType type{};
    };

    struct DiscordCoreAPI_Dll PatchInteractionResponseData {
        InteractionResponseData data{};
        string interactionToken{ "" };
        string applicationId{ "" };
    };

    struct DiscordCoreAPI_Dll PostInteractionResponseData {
        InteractionApplicationCommandCallbackData data{};
        InteractionCallbackType type{};
        string interactionToken{ "" };
        string interactionId{ "" };
    };

    struct DiscordCoreAPI_Dll PostFollowUpMessageData {
        AllowedMentionsData allowedMentions{};
        vector<ActionRowData> components{};
        string interactionToken{ "" };
        string applicationId{ "" };
        vector<EmbedData> embeds{};
        string content{ "" };
        __int32 flags{ 0 };
        bool tts{ false };        
    };

    struct DiscordCoreAPI_Dll GetInteractionResponseData {
        string interactionToken{ "" };
        string applicationId{ "" };
    };

    struct DiscordCoreAPI_Dll PostDeferredInteractionResponseData {
        InteractionCallbackType type{};
        string interactionToken{ "" };
        string interactionId{ "" };
        __int32 flags{ 0 };
    };

    struct DiscordCoreAPI_Dll HttpData {
        vector<string> responseHeaderValues{};
        string returnMessage{ "" };
         __int32 returnCode{ 0 };
        json data{};
    };

    struct DiscordCoreAPI_Dll GetMessagesData {
        string beforeThisId{ "" };
        string aroundThisId{ "" };
        string afterThisId{ "" };
        string channelId{ "" };
         __int32 limit{ 0 };
    };

    struct DiscordCoreAPI_Dll DeleteMessagesBulkData {
        vector<string> messageIds{};
        string channelId{ "" };
        string content{ "" };
    };

    struct DiscordCoreAPI_Dll HttpWorkloadData {
        map<string, string> headerValuesToCollect{};
        HttpWorkloadClass workloadClass{};
        HttpWorkloadType workloadType{};
        string relativePath{ "" };
        string callStack{ "" };
        string content{ "" };
    };

    struct DiscordCoreAPI_Dll RateLimitData {
        HttpWorkloadType workloadType{};
        float nextExecutionTime{ 0.0f };
        float timeResettingAt{ 0.0f };
        float msRemainTotal{ 0.0f };
        float timeStartedAt{ 0.0f };
        __int32 getsRemaining{ 1 };
        bool isItMarked{ false };
        __int32 totalGets{ 0 };
        float msRemain{ 0.0f };        
        string bucket{ "" };
    };

    struct DiscordCoreAPI_Dll GetGuildData {
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll PutReactionData {
        string channelId{ "" };
        string messageId{ "" };
        string emoji{ "" };
    };

    enum class ReactionDeletionType {
        SELF_DELETE = 0,
        USER_DELETE = 1,
        EMOJI_DELETE = 2,
        ALL_DELETE = 3
    };

    struct DiscordCoreAPI_Dll DeleteReactionDataAll {
        ReactionDeletionType deletionType{};
        string encodedEmoji{ "" };
        string channelId{ "" };
        string messageId{ "" };
        string userId{ "" };
    };

    struct DiscordCoreAPI_Dll GetReactionData {
        string channelId{ "" };
        string messageId{ "" };
        string emojiName{ "" };
        string userId{ "" };
    };

    struct DiscordCoreAPI_Dll GetChannelData {
        string channelId{ "" };
    };

    struct DiscordCoreAPI_Dll PutPermissionOverwritesData {
        EditChannelPermissionOverwritesType type{};
        string roleOrUserId{ "" };
        string channelId{ "" };
        string allow{ "" };
        string deny{ "" };
    };

    struct DiscordCoreAPI_Dll DeleteChannelPermissionOverwritesData {
        string roleOrUserId{ "" };
        string channelId{ "" };
    };

    struct DiscordCoreAPI_Dll GetDMChannelData {
        string userId{ "" };
    };

    struct DiscordCoreAPI_Dll GetMessageData {
        string requesterId{ "" };
        string channelId{ "" };
        string messageId{ "" };
    };

    struct DiscordCoreAPI_Dll PutPinMessageData {
        string channelId{ "" };
        string messageId{ "" };
    };

    struct DiscordCoreAPI_Dll DeleteMessageData {
        string messageTimeStamp{ "" };
        string channelId{ "" };
        string messageId{ "" };
        __int32 timeDelay{ 0 };
    };

    enum class GetUserDataType {
        SELF = 0,
        USER = 1
    };

    struct DiscordCoreAPI_Dll LeaveGuildData {
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll GetUserData {
        GetUserDataType userType{};
        string userId{ "" };
    };

    struct DiscordCoreAPI_Dll GetPinnedMessagesData {
        string channelId{ "" };
    };

    struct DiscordCoreAPI_Dll PostMessageData {
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

    struct DiscordCoreAPI_Dll PatchMessageData {
        AllowedMentionsData allowedMentions{};
        vector<AttachmentData> attachments{};
        vector<ActionRowData> components{};
        vector<EmbedData> embeds{};
        string requesterId{ "" };
        string messageId{ "" };
        string channelId{ "" };
        string content{ "" };
        __int32 flags{ 0 };
    };

    struct DiscordCoreAPI_Dll PostDMData {
        MessageReferenceData messageReference{};
        AllowedMentionsData allowedMentions{};
        vector<ActionRowData> components{};
        vector<string> stickerIds{};
        vector<EmbedData> embeds{};
        string requesterId{ "" };
        string channelId{ "" };
        string content{ "" };
        string userId{ "" };
        __int32 nonce{ 0 };
        bool tts{ false };
    };

    struct DiscordCoreAPI_Dll GetGuildMemberData {
        string guildMemberId{ "" };
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll GetGuildMemberRolesData {
        vector<string> roleIds{};
    };

    struct DiscordCoreAPI_Dll PatchGuildMemberData {
        string newVoiceChannelId{ "" };
        string currentChannelId{ "" };
        string guildMemberId{ "" };
        vector<string> roleIds{};
        string guildId{ "" };
        bool mute{ false };
        bool deaf{ false };
        string nick{ "" };
    };

    struct DiscordCoreAPI_Dll UpdateVoiceStateData {
        string channelId{ "" };
        bool selfMute{ false };
        bool selfDeaf{ false };
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll GetRolesData {
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll GetRoleData {
        string guildId{ "" };
        string roleId{ "" };
    };

    struct DiscordCoreAPI_Dll DeleteGuildMemberRoleData {
        string guildId{ "" };
        string userId{ "" };
        string roleId{ "" };
    };

    struct DiscordCoreAPI_Dll DeleteGuildRoleData {
        string guildId{ "" };
        string roleId{ "" };
    };

    struct DiscordCoreAPI_Dll RolePositionData {
         __int32 rolePosition{ 0 };
        string roleId{ "" };
    };

    struct DiscordCoreAPI_Dll PatchRolePositionData {
        vector<RolePositionData> rolePositions{};
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll PostRoleData {
        string hexColorValue{ "" };
        bool mentionable{ false };
        __int64 permissions{ 0 };
        string guildId{ "" };
        string roleId{ "" };
        bool hoist{ false };
        string name{ "" };
    };

    struct DiscordCoreAPI_Dll PutRoleData {
        string guildId{ "" };
        string userId{ "" };
        string roleId{ "" };
    };

    struct DiscordCoreAPI_Dll PatchRoleData {
        string hexColorValue{ "" };
        bool mentionable{ false };
        string permissions{ "" };
        string content{ "" };
        string guildId{ "" };
        string roleId{ "" };
        bool hoist{ false };
        string name{ "" };
    };

    struct DiscordCoreAPI_Dll ApplicationCommandOptionChoiceData {
        float valueFloat{ 0.0f };
        string valueString{ "" };
        __int32	valueInt{ 0 };
        string name{ "" };        
    };

    struct DiscordCoreAPI_Dll ApplicationCommandOptionData {
        vector<ApplicationCommandOptionChoiceData>	choices{};
        vector<ApplicationCommandOptionData> options{};
        ApplicationCommandOptionType type{};
        vector<ChannelType> channelTypes{};
        string description{ "" };
        bool required{ false };
        string name{ "" };
    };

    struct DiscordCoreAPI_Dll ApplicationCommandData : public DiscordCoreAPI::DiscordEntity {
        vector<ApplicationCommandOptionData> options{};
        bool defaultPermission{ true };
        ApplicationCommandType type{};
        string applicationId{ "" };
        string description{ "" };
        string guildId{ "" };
        string version{ "" };
        string name{ "" };
    };

    struct DiscordCoreAPI_Dll PatchApplicationCommandData {
        vector<ApplicationCommandOptionData> options{};
        bool defaultPermission{ true };
        string description{ "" };
        string name{ "" };
    };

    struct DiscordCoreAPI_Dll PostApplicationCommandData {
        vector<ApplicationCommandOptionData> options{};
        bool defaultPermission{ true };
        ApplicationCommandType type{};
        string applicationId{ "" };
        string description{ "" };
        string name{ "" };
    };

    struct DiscordCoreAPI_Dll PutEditApplicationCommandPermissionsData {
        vector<ApplicationCommandPermissionData> permissions{};
        string commandName{ "" };
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll PutBatchEditApplicationCommandPermissionsData {
        vector<GuildApplicationCommandPermissionData> permissions{};
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll PatchFollowUpMessageData {
        DiscordCoreInternal::AllowedMentionsData allowedMentions{};
        vector<ActionRowData> components{};
        string interactionToken{ "" };
        string applicationId{ "" };
        vector<EmbedData> embeds{};
        string requesterId{ "" };
        string messageId{ "" };
        string content{ "" };
        __int32 flags{ 0 };
        bool tts{ false };
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

    struct DiscordCoreAPI_Dll GetAuditLogData {
        AuditLogEvent actionType{};
         __int32 limit{ 0 };
        string guildId{ "" };
        string userId{ "" };
    };

    struct DiscordCoreAPI_Dll GetInviteData {
        string inviteId{ "" };
    };

    struct DiscordCoreAPI_Dll GetVanityInviteData {
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll GetInvitesData {
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll VoiceConnectInitData {
        string channelId{ "" };
        string guildId{ "" };
        string userId{ "" };
    };

    struct DiscordCoreAPI_Dll VoiceConnectionData {
        vector<unsigned __int8> secretKey{};
        string voiceEncryptionMode{ "" };
        unsigned __int32 audioSSRC{ 0 };
        string externalIp{ "" };
        string voicePort{ "" };
        string sessionId{ "" };
        string endPoint{ "" };
        string voiceIp{ "" };
        string token{ "" };
        string keys{ "" };
    };

    struct DiscordCoreAPI_Dll VoiceReadyPayload {
        unsigned __int32 ssrc{ 0 };
        vector<string> modes{};
        __int32 port{ 0 };
        string ip{ "" };
    };

    struct DiscordCoreAPI_Dll PutGuildBanData {
        __int32 deleteMessageDays{ 0 };
        string guildMemberId{ "" };        
        string guildId{ "" };
        string reason{ "" };
    };

};

namespace DiscordCoreAPI {

    /// Role tags data. \brief Role tags data.
    struct DiscordCoreAPI_Dll RoleTagsData {

        operator DiscordCoreInternal::RoleTagsData() {
            DiscordCoreInternal::RoleTagsData newData;
            newData.integrationId = this->integrationId;
            newData.botId = this->botId;
            return newData;
        }

        string premiumSubscriber{ "" };///< Are they a premium subscriber?
        string integrationId{ "" };///< What is the integration id?
        string botId{ "" };///< What is the bot id?
    };

    /// Data structure representing a single Role. \brief Data structure representing a single Role.
    struct DiscordCoreAPI_Dll RoleData : public DiscordEntity {

        operator DiscordCoreInternal::RoleData() {
            DiscordCoreInternal::RoleData newData;
            newData.permissions = this->permissions;
            newData.mentionable = this->mentionable;
            newData.position = this->position;
            newData.managed = this->managed;
            newData.color = this->color;
            newData.hoist = this->hoist;
            newData.name = this->name;
            newData.tags = this->tags;
            newData.id = this->id;
            return newData;
        }

        bool mentionable{ false }; ///< Is ths Role mentionable?
        string permissions{ "" }; ///< The Role's base Guild permissions.
        __int32 position{ 0 }; ///< Its position amongst the rest of the Guild's roles.
        bool managed{ false }; ///< is it a managed Role?
        RoleTagsData tags{}; ///< Role tags for the Role.
        bool hoist{ false };///< Is it hoisted above the other roles?
        __int32 color{ 0 };///< The Role's color.
        string name{ "" };///< The Role's name.
    };

    /// Data structure representing a single user.
    struct DiscordCoreAPI_Dll UserData : public DiscordEntity {

        operator DiscordCoreInternal::UserData() {
            DiscordCoreInternal::UserData newData;
            newData.discriminator = this->discriminator;
            newData.premiumType = this->premiumType;
            newData.publicFlags = this->publicFlags;
            newData.mfaEnabled = this->mfaEnabled;
            newData.createdAt = this->createdAt;
            newData.userName = this->userName;
            newData.verified = this->verified;
            newData.avatar = this->avatar;
            newData.locale = this->locale;
            newData.system = this->system;
            newData.email = this->email;
            newData.flags = this->flags;
            newData.bot = this->bot;
            newData.id = this->id;
            return newData;
        }

        string discriminator{ "" }; ///< The # next to their User name.
        bool mfaEnabled{ false };///< MFA enabled?
        __int32 premiumType{ 0 };///< Their premium nitro status.
        __int32 publicFlags{ 0 };///< Public flags.
        bool verified{ false };///< Verified?
        string createdAt{ "" };///< When the User was created.
        string userName{ "" };///< Their username.
        bool system{ false };///< Are they a partof the system?        
        string avatar{ "" };///< Their avatar url.
        string locale{ "" };///< The region they are from/in.
        string email{ "" };///< Their email address.
        __int32 flags{ 0 };///< Flags.
        bool bot{ false };///< Are they a bot?        
    };

    /// Attachment data. \brief Attachment data.
    struct DiscordCoreAPI_Dll AttachmentData : public DiscordEntity {

        operator DiscordCoreInternal::AttachmentData() {
            DiscordCoreInternal::AttachmentData newData;
            newData.contentType = this->contentType;
            newData.proxyUrl = this->proxyUrl;
            newData.filename = this->filename;
            newData.height = this->height;
            newData.width = this->width;
            newData.size = this->size;
            newData.url = this->url;
            newData.id = this->id;
            return newData;
        }

        string contentType{ "" }; ///< Type of content for the attachment.
        string filename{ "" };///< The file name of the attachment.
        string proxyUrl{ "" };///< The proxy url for the attachment.
        __int32 height{ 0 };///< The height of the attachment.
        __int32 width{ 0 };///< The width of the attachment.
        __int32 size{ 0 };///< The size of the attachment.
        string url{ "" };///< The url for the attachment.        
    };

    /// Sticker format type. \brief Sticker format type.
    enum class StickerFormatType {
        PNG = 1,///< PNG
        APNG = 2,///< APNG
        LOTTIE = 3///< Lottie
    };

    /// Embed footer data. \brief Embed footer data.
    struct DiscordCoreAPI_Dll EmbedFooterData {

        operator DiscordCoreInternal::EmbedFooterData() {
            DiscordCoreInternal::EmbedFooterData newData;
            newData.proxyIconUrl = this->proxyIconUrl;
            newData.iconUrl = this->iconUrl;
            newData.text = this->text;
            return newData;
        };

        string proxyIconUrl{ "" };///< Proxy icon url.
        string iconUrl{ "" };///< Icon url.
        string text{ "" };///< Footer text.
    };

    /// Embed image data. \brief Embed image data.
    struct DiscordCoreAPI_Dll EmbedImageData {

        operator DiscordCoreInternal::EmbedImageData() {
            DiscordCoreInternal::EmbedImageData newData;
            newData.proxyUrl = this->proxyUrl;
            newData.height = this->height;
            newData.width = this->width;
            newData.url = this->url;
            return newData;
        }

        string proxyUrl{ "" };///< Proxy url.
        __int32 height{ 0 };///< Image height.
        __int32 width{ 0 };///< Image width.
        string url{ "" };///< Image url.        
    };

    /// Embed thumbnail data. \brief Embed thumbnail data.
    struct DiscordCoreAPI_Dll EmbedThumbnailData {

        operator DiscordCoreInternal::EmbedThumbnailData() {
            DiscordCoreInternal::EmbedThumbnailData newData;
            newData.proxyUrl = this->proxyUrl;
            newData.height = this->height;
            newData.width = this->width;
            newData.url = this->url;
            return newData;
        }

        string proxyUrl{ "" };///< Proxy url.
        __int32 height{ 0 };///< Image height.
        __int32 width{ 0 };///< Image width.
        string url{ "" };///< Image url.        
    };

    /// Embed video data. \brief Embed video data.
    struct DiscordCoreAPI_Dll EmbedVideoData {

        operator DiscordCoreInternal::EmbedVideoData() {
            DiscordCoreInternal::EmbedVideoData newData;
            newData.proxyUrl = this->proxyUrl;
            newData.height = this->height;
            newData.width = this->width;
            newData.url = this->url;
            return newData;
        }

        string proxyUrl{ "" };///< Proxy url.
        __int32 height{ 0 };///< Image height.
        __int32 width{ 0 };///< Image width.
        string url{ "" };///< Image url.        
    };

    /// Embed provider data. \brief Embed provider data.
    struct DiscordCoreAPI_Dll EmbedProviderData {

        operator DiscordCoreInternal::EmbedProviderData() {
            DiscordCoreInternal::EmbedProviderData newData;
            newData.name = this->name;
            newData.url = this->url;
            return newData;
        }

        string name{ "" }; ///< Name.
        string url{ "" };///< Url.
    };

    /// Embed author data.
    struct DiscordCoreAPI_Dll EmbedAuthorData {

        operator DiscordCoreInternal::EmbedAuthorData() {
            DiscordCoreInternal::EmbedAuthorData newData;
            newData.proxyIconUrl = this->proxyIconUrl;
            newData.iconUrl = this->iconUrl;
            newData.name = this->name;
            newData.url = this->url;
            return newData;
        };

        string proxyIconUrl{ "" };///< Proxy icon url.
        string iconUrl{ "" };///< Icon url.
        string name{ "" }; ///< Name.
        string url{ "" };///< Url.
    };

    /// Embed field data. \brief Embed field data.
    struct DiscordCoreAPI_Dll EmbedFieldData {

        operator DiscordCoreInternal::EmbedFieldData() {
            DiscordCoreInternal::EmbedFieldData newData;
            newData.Inline = this->Inline;
            newData.value = this->value;
            newData.name = this->name;
            return newData;
        }

        bool Inline{ false };///< Is the field inline with the rest of them?
        string value{ "" };///< The text on the field.
        string name{ "" };///< The title of the field.
    };

    /// Embed data. \brief Embed data.
    struct DiscordCoreAPI_Dll EmbedData {

        vector<EmbedFieldData> fields{};///< Array of embed fields.
        EmbedThumbnailData thumbnail{};///< Embed thumbnail data.
        EmbedProviderData provider{};///< Embed provider data.
        string hexColorValue{ "" };///< Hex color value of the embed.
        string description{ "" };///< Description of the embed.
        EmbedFooterData footer{};///< Embed footer data.
        EmbedAuthorData author{};///< Embed author data.
        string timestamp{ "" };///< Timestamp to be placed on the embed.
        EmbedImageData image{};///< Embed image data.
        EmbedVideoData video{};///< Embed video data.
        string title{ "" };///< Title of the embed.
        string type{ "" };///< Type of the embed.
        string url{ "" };///< Url for the embed.

        operator DiscordCoreInternal::EmbedData() {
            DiscordCoreInternal::EmbedData newData;
            newData.hexColorValue = this->hexColorValue;
            newData.description = this->description;
            newData.timestamp = this->timestamp;
            for (auto value : this->fields) {
                newData.fields.push_back(value);
            }
            newData.thumbnail = this->thumbnail;
            newData.provider = this->provider;
            newData.footer = this->footer;
            newData.author = this->author;
            newData.image = this->image;
            newData.title = this->title;
            newData.video = this->video;
            newData.type = this->type;
            newData.url = this->url;
            return newData;
        }

        /// Sets the author's name and avatar for the embed. \brief Sets the author's name and avatar for the embed.
        /// \param authorName The author's name.
        /// \param authorAvatarURL The url to their avatar.
        /// \returns A pointer to this embed.
        EmbedData* setAuthor(string authorName, string authorAvatarURL = "") {
            this->author.name = authorName;
            this->author.iconUrl = authorAvatarURL;
            return this;
        }

        /// Sets the footer's values for the embed. \brief Sets the footer's values for the embed.
        /// \param footerText The footer's text.
        /// \param footerIconURLText Url to the footer's icon.
        /// \returns A pointer to this embed.
        EmbedData* setFooter(string footerText, string footerIconURLText = "") {
            this->footer.text = footerText;
            this->footer.iconUrl = footerIconURLText;
            return this;
        }

        /// Sets the timestamp on the embed. \brief Sets the timestamp on the embed.
        /// \param timeStamp The timestamp to be set.
        /// \returns A pointer to this embed.
        EmbedData* setTimeStamp(string timeStamp) {
            this->timestamp = timeStamp;
            return this;
        }

        /// Adds a field to the embed. \brief Adds a field to the embed.
        /// \param name The title of the embed field.
        /// \param value The contents of the embed field.
        /// \param Inline Is it inline with the rest of the fields on the embed?
        /// \returns A pointer to this embed.
        EmbedData* addField(string name, string value, bool Inline = true) {
            EmbedFieldData embedFieldData;
            embedFieldData.name = name;
            embedFieldData.Inline = Inline;
            embedFieldData.value = value;
            this->fields.push_back(embedFieldData);
            return this;
        }

        /// Sets the description (the main contents) of the embed. \brief Sets the description (the main contents) of the embed.
        /// \param descriptionNew The contents of the description to set.
        /// \returns A pointer to this embed.
        EmbedData* setDescription(string descriptionNew) {
            this->description = descriptionNew;
            return this;
        }

        /// Sets the color of the embed, by applying a hex-color value. \brief Sets the color of the embed, by applying a hex-color value.
        /// \param hexColorValueNew A string containing a hex-number value (Between 0x00 and 0xFFFFFF).
        /// \returns A pointer to this embed.
        EmbedData* setColor(string hexColorValueNew) {
            this->hexColorValue = hexColorValueNew;
            return this;
        }

        /// Sets the thumbnail of the embed. \brief Sets the thumbnail of the embed.
        /// \param thumbnailURL The url to the thumbnail to be used.
        /// \returns A pointer to this embed.
        EmbedData* setThumbnail(string thumbnailURL) {
            this->thumbnail.url = thumbnailURL;
            return this;
        }

        /// Sets the title of the embed. \brief Sets the title of the embed.
        /// \param titleNew A string containing the desired title.
        /// \returns A pointer to this embed.
        EmbedData* setTitle(string titleNew) {
            this->title = titleNew;
            return this;
        }

        /// Sets the image of the embed. \brief Sets the image of the embed.
        /// \param imageURL The url of the image to be set on the embed.
        /// \returns A pointer to this embed.
        EmbedData* setImage(string imageURL) {
            this->image.url = imageURL;
            return this;
        }
    };

    /// Message reference data.\brief Message reference data.
    struct DiscordCoreAPI_Dll MessageReferenceData {

        operator DiscordCoreInternal::MessageReferenceData() {
            DiscordCoreInternal::MessageReferenceData newData;
            newData.failIfNotExists = this->failIfNotExists;
            newData.channelId = this->channelId;
            newData.messageId = this->messageId;
            newData.guildId = this->guildId;
            return newData;
        }

        bool failIfNotExists{ false };///< Fail if the Message doesn't exist?
        string messageId{ "" };///< Id of the Message to reference.
        string channelId{ "" };///< Id of the Channel that the referenced Message was sent in.
        string guildId{ "" };///< Id of the Guild that the referenced Message was sent in.
    };
    
    /// Edit Channel permission overwrites type. \brief Edit Channel permission overwrites type.
    enum class EditChannelPermissionOverwritesType {
        Role = 0,///<Role
        User = 1///<User
    };

    /// A permission overwrite, for a given Channel. \brief A permission overwrite, for a given Channel.
    struct DiscordCoreAPI_Dll OverWriteData : public DiscordEntity {

        operator DiscordCoreInternal::OverWriteData() {
            DiscordCoreInternal::OverWriteData newData;
            newData.type = (DiscordCoreInternal::EditChannelPermissionOverwritesType)this->type;
            newData.channelId = this->channelId;
            newData.allow = this->allow;
            newData.deny = this->deny;
            newData.id = this->id;
            return newData;
        }

        EditChannelPermissionOverwritesType type{};   ///< Role or User type.
        string channelId{ "" };///< Channel id for which Channel this overwrite be__int64s to.
        string allow{ "" };///< Collection of permissions to allow.
        string deny{ "" };///< Collection of permissions to deny.
    };

    /// A type of Channel. \brief A type of Channel.
    enum class ChannelType {
        GUILD_TEXT = 0, ///< Guild Text
        DM = 1, ///< Direct Message
        GUILD_VOICE = 2, ///< Guild Voice
        GROUP_DM = 3,///< Group Direct Message
        GUILD_CATEGORY = 4,///< Guild Category
        GUILD_NEWS = 5,///< Guild News
        GUILD_STORE = 6,///< Guild Store
        GUILD_NEWS_THREAD = 10,///< Guild News Thread
        GUILD_PUBLIC_THREAD = 11,///< Guild Public Thread
        GUILD_PRIVATE_THREAD = 12,///< Guild Private Thread
        GUILD_STAGE_VOICE = 13///< Guild Stage Voice
    };

    /// Meta data for a thread type of Channel. \brief Meta data for a thread type of Channel.
    struct DiscordCoreAPI_Dll ThreadMetadataData {

        operator DiscordCoreInternal::ThreadMetadataData() {
            DiscordCoreInternal::ThreadMetadataData newData;
            newData.autoArchiveDuration = this->autoArchiveDuration;
            newData.archiveTimestamp = this->archiveTimestamp;
            newData.archiverId = this->archiverId;
            newData.archived = this->archived;
            newData.locked = this->locked;
            return newData;
        };

        __int32 autoArchiveDuration{ 0 }; ///< How __int64 before archiving this thread.
        string archiveTimestamp{ "" }; ///< (Where applicable) the time at which this thread was archived.
        string archiverId{ "" }; ///< The id of the individual who archived this thread.
        bool archived{ false }; ///< Whether or not this thread is currently archived.
        bool locked{ false }; ///< Whether or not this thread is currently locked.
    };

    /// Data for a single member of a thread. \brief Data for a single member of a thread.
    struct DiscordCoreAPI_Dll ThreadMemberData : public DiscordEntity {

        operator DiscordCoreInternal::ThreadMemberData() {
            DiscordCoreInternal::ThreadMemberData newData;
            newData.joinTimestamp = this->joinTimestamp;
            newData.userId = this->userId;
            newData.flags = this->flags;
            newData.id = this->id;
            return newData;
        }
        string joinTimestamp{ "" }; ///< The time at which the member joined this thread.
        string userId{ "" };    ///< The User's id.
        __int32 flags{ 0 }; ///< Flags.
    };

    /// Data structure representing a single Channel. \brief Data structure representing a single Channel.
    struct DiscordCoreAPI_Dll ChannelData : public DiscordEntity {
        map<string, OverWriteData> permissionOverwrites{}; ///< Permission overwrites for the given Channel.
        ThreadMetadataData threadMetadata{}; ///< Metadata in the case that this Channel is a thread.
        ChannelType type{ ChannelType::DM };    ///< The type of the Channel.
        __int32 videoQualityMode{ 0 };  ///< Video quality mode.
        __int32 rateLimitPerUser{ 0 };  ///< Amount of seconds a User has to wait before sending another Message.
        vector<UserData> recipients{};  ///< Recipients, in the case of a group DM or DM.
        string lastPinTimestamp{ "" };  ///< Timestamp of the last pinned Message.
        string lastMessageId{ "" }; ///< Id of the last Message.
        string applicationId{ "" }; ///< Application id of the current application.
        ThreadMemberData member{}; ///< Thread member object for the current User, if they have joined the thread.
        __int32 messageCount{ 0 }; ///< An approximate count of Messages in a thread, stops counting at 50.
        __int32 memberCount{ 0 };   ///< Count of members active in the Channel.
        string rtcRegion{ "" }; ///< Real-time clock region.
        __int32 userLimit{ 0 }; ///< User limit, in the case of voice channels.
        __int32 position{ 0 };  ///< The position of the Channel, in the Guild's Channel list.
        string parentId{ "" }; ///< Id of the parent Channel, if applicable.
        string guildId{ "" };  ///< Id of the Channel's Guild, if applicable.
        string ownerId{ "" };   ///< Id of the Channel's owner.
        __int32 bitrate{ 0 };   ///< Bitrate of the Channel, if it is a voice Channel.
        string topic{ "" }; ///< The Channel's topic.
        bool nsfw{ false }; ///< Whether or not it is nsfw.
        string name{ "" };  ///< Name of the Channel.
        string icon{ "" };  ///< Icon for the Channel, if applicable.
    };
    

    /// Voice state data. \brief Voice state data.
    struct DiscordCoreAPI_Dll VoiceStateData {
        string requestToSpeakTimestamp{ "" }; ///< When they last requested to speak.
        bool selfStream{ false };///<Are they streaming?
        bool selfVideo{ false };///< Is their video turned on?
        bool selfDeaf{ false };///< Are they self-deafened?
        bool selfMute{ false };///< Are they self-muted?
        string sessionId{ "" };///< The session id.
        string channelId{ "" };///< The current Channel id.
        bool suppress{ false };///< Are they currently suppressed?
        string guildId{ "" };///< The current Guild id.
        string userId{ "" };///< Their User id.
        bool deaf{ false };///< Are they server-deafened?
        bool mute{ false };///< Are they server-muted?
    };

    /// Data structure representing a single GuildMember. \brief Data structure representing a single GuildMember.
    struct DiscordCoreAPI_Dll GuildMemberData : DiscordEntity {

        operator DiscordCoreInternal::GuildMemberData() {
            DiscordCoreInternal::GuildMemberData newData;
            newData.premiumSince = this->premiumSince;
            newData.permissions = this->permissions;
            newData.userMention = this->userMention;
            newData.joinedAt = this->joinedAt;
            newData.pending = this->pending;
            newData.guildId = this->guildId;
            newData.roles = this->roles;
            newData.deaf = this->deaf;
            newData.mute = this->mute;
            newData.nick = this->nick;
            newData.user = this->user;
            return newData;
        }

        VoiceStateData voiceData{};///< The voice state data for the GuildMember.
        string premiumSince{ "" };///< If applicable, when they first boosted the server.
        string permissions{ "" };///< Their base-level permissions in the Guild.
        string userMention{ "" };///< What to enter to get them mentioned in a Message.
        vector<string> roles{}; ///< The Guild roles that they have.
        string joinedAt{ "" };///< When they joined the Guild.
        bool pending{ false };///< Are they waiting at the entry screen?
        string guildId{ "" };///< The current Guild's id.
        bool deaf{ false };///< Are they server deafned?
        bool mute{ false };///< Are they server muted?
        string nick{ "" };///< Their nick/display name.
        UserData user{};///< User data for the current GuildMember.
    };

    /// ApplicationCommand option type. \brief ApplicationCommand option type.
    enum class ApplicationCommandOptionType {
        SUB_COMMAND = 1,///< Sub Command
        SUB_COMMAND_GROUP = 2,///< Sub Command Group
        STRING = 3,///< String
        INTEGER = 4,///< Integer
        BOOLEAN = 5,///< Boolean
        USER = 6,///< User
        CHANNEL = 7,///< Channel
        ROLE = 8,///< Role
        MENTIONABLE = 9,///< Mentionable
        NUMBER = 10///< Number
    };

    /// ApplicationCommand permission type. \brief ApplicationCommand permission type.
    enum class ApplicationCommandPermissionType {
        Role = 1,///< Role type.
        User = 2///< User type.
    };

    /// Permissions data for an ApplicationCommand. \brief Permissions data for an ApplicationCommand.
    struct DiscordCoreAPI_Dll ApplicationCommandPermissionData : public DiscordEntity {
        operator DiscordCoreInternal::ApplicationCommandPermissionData() {
            DiscordCoreInternal::ApplicationCommandPermissionData newData{};
            newData.id = this->id;
            newData.permission = this->permission;
            newData.type = (DiscordCoreInternal::ApplicationCommandPermissionType)this->type;
            return newData;
        }
        ApplicationCommandPermissionType type{ ApplicationCommandPermissionType::Role }; ///< The type of permission.
        bool permission{ false }; ///< Whether the permission is active or not.
    };

    /// Represents the permissions for accessing an ApplicationCommand from within a Guild. \brief Represents the permissions for accessing an ApplicationCommand from within a Guild.
    struct DiscordCoreAPI_Dll GuildApplicationCommandPermissionsData : public DiscordEntity {
        vector<ApplicationCommandPermissionData> permissions{};///< The permissions.
        string applicationId{ "" };///< The application's id.
        string guildId{ "" };///< The Guild's id.
    };

    /// Data structure representing a User reaction to a Message.

    /// Data structure representing a single emoji. \brief Data structure representing a single emoji.
    struct DiscordCoreAPI_Dll EmojiData : public DiscordEntity {

        operator DiscordCoreInternal::EmojiData() {
            DiscordCoreInternal::EmojiData newData;
            newData.requireColons = this->requireColons;
            newData.available = this->available;
            for (auto value : this->roles) {
                newData.roles.push_back(value);
            }
            newData.animated = this->animated;
            newData.managed = this->managed;
            newData.user = this->user;
            newData.name = this->name;
            newData.id = this->id;
            return newData;
        }

        bool requireColons{ false };///< Require colons to render it?
        vector<RoleData> roles{};///< Roles that are allowed to use this emoji.
        bool available{ true };///< Is it available to be used?
        bool animated{ false };///< Is it animated?
        bool managed{ false };///< Is it managed?
        string name{ "" };///< What is its name?
        UserData user{};///< User that created this emoji.
    };
    
    /// Data structure representing a single reaction.
    struct DiscordCoreAPI_Dll ReactionData : public DiscordEntity{

        operator DiscordCoreInternal::ReactionData() {
            DiscordCoreInternal::ReactionData newData;
            newData.channelId = this->channelId;
            newData.messageId = this->messageId;
            newData.guildId = this->guildId;
            newData.member = this->member;
            newData.userId = this->userId;
            newData.count = this->count;
            newData.emoji = this->emoji;
            newData.me = this->me;
            return newData;
        }

        GuildMemberData member{}; ///< The GuildMember who placed the reaction.
        string channelId{ "" }; ///< The id of the Channel where it was placed.
        string messageId{ "" }; ///< The id of the Message upon which it was placed.
        string guildId{ "" };///< The id of the Guild where it was placed.
        string userId{ "" }; ///< The id of the User who placed the reaction.
        __int32 count{ 0 };///< The number of times this particular emoji was placed as a reaction to the given Message.
        EmojiData emoji{}; ///< The emoji that was placed as a reaction.
        bool me{ false }; ///< Whether or not I (The bot) placed it.
    };

    enum class MessageActivityType {
        JOIN = 1,
        SPECTATE = 2,
        LISTEN = 3,
        JOIN_REQUEST = 5
    };

    /// Message activity data. \brief Message activity data.
    struct DiscordCoreAPI_Dll MessageActivityData {
        MessageActivityType type{ MessageActivityType::JOIN };///< Message activity type.
        string partyId{ "" };///< Party id.
    };

    /// Ban data. \brief Ban data.
    struct DiscordCoreAPI_Dll BanData {
        bool failedDueToPerms{ false };///< Failed due to perms?
        string reason{ "" };///< Reason for the ban.
        UserData user{};///< User that was banned.
    };

    /// Team members object data. \brief Team members object data.
    struct DiscordCoreAPI_Dll TeamMembersObjectData {

        operator DiscordCoreInternal::TeamMembersObjectData() {
            DiscordCoreInternal::TeamMembersObjectData newData;
            newData.membershipState = this->membershipState;
            newData.permissions = this->permissions;
            newData.teamId = this->teamId;
            newData.user = this->user;
            return newData;
        }

        vector<string> permissions{};///< Permissions for the team.
        __int32 membershipState{ 0 };///< Current state.
        string teamId{ "" };///< Id of the current team.
        UserData user{};///< User data of the current User.
    };

    /// Team object data. \brief Team object data.
    struct DiscordCoreAPI_Dll TeamObjectData : public DiscordEntity {

        operator DiscordCoreInternal::TeamObjectData() {
            DiscordCoreInternal::TeamObjectData newData;
            newData.ownerUserId = this->ownerUserId;
            newData.icon = this->icon;
            newData.id = this->id;
            return newData;
        }

        vector<TeamMembersObjectData> members{};///< Array of team members object data.
        string ownerUserId{ "" };///< User id of the team owner.
        string icon{ "" };///<Icon for the team.
    };

    /// Application data. \brief Application data.
    struct DiscordCoreAPI_Dll ApplicationData : public DiscordEntity {

        operator DiscordCoreInternal::ApplicationData() {
            DiscordCoreInternal::ApplicationData newData;
            newData.botRequireCodeGrant = this->botRequireCodeGrant;
            newData.termsOfServiceUrl = this->termsOfServiceUrl;
            newData.privacyPolicyUrl = this->privacyPolicyUrl;
            newData.primarySkuId = this->primarySkuId;
            newData.description = this->description;
            newData.coverImage = this->coverImage;
            newData.rpcOrigins = this->rpcOrigins;
            newData.botPublic = this->botPublic;
            newData.verifyKey = this->verifyKey;
            newData.guildId = this->guildId;
            newData.summary = this->summary;
            newData.owner = this->owner;
            newData.flags = this->flags;
            newData.icon = this->icon;
            newData.name = this->name;
            newData.slug = this->slug;
            newData.team = this->team;
            newData.id = this->id;
            return newData;
        }

        bool botRequireCodeGrant{ false };///< Does the bot require a code grant?
        string termsOfServiceUrl{ "" };///< Terms of service URL.
        string privacyPolicyUrl{ "" };///< Privacy policy URL.
        vector<string> rpcOrigins{};///< Array of RPC origin strings.
        string primarySkuId{ "" };///< Primary SKU Id.
        string description{ "" };///< Description of the application.
        bool botPublic{ false };///< Is the bot public?
        string coverImage{ "" };///< The cover image.
        string verifyKey{ "" };///< The verification key.
        TeamObjectData team{};///< Team object data.
        string  summary{ "" };///< Summary of the application.
        string guildId{ "" };///< Guild id.
        __int32 flags{ 0 };///< Application flags.
        string slug{ "" };///< Sluhg.
        string name{ "" };///< Application's name.
        string icon{ "" };///< Application's icon.
        UserData owner{};///< Application's owner.
    };

    /// Account data. \brief Account data.
    struct DiscordCoreAPI_Dll AccountData : public DiscordEntity {
        string name{ "" };///< Name of the account.
    };

    /// Integration data. \brief Integration data.
    struct DiscordCoreAPI_Dll IntegrationData : public DiscordEntity {
         __int32 expireGracePeriod{ 0 };///< How __int64 before the integration expires.
         __int32 subscriberCount{ 0 };///< Number of current subscribers.
        ApplicationData application{};///<Application data.
        bool enableEmoticons{ true };///<Emoticons enabled?
        __int32 expireBehavior{ 0 };///< What to do upon expiry.
        bool enabled{ false };///< Enabled?
        bool syncing{ false };///< Is it syncing?
        AccountData account{};///< Account data.
        string syncedAt{ "" };///< Time it was last synced at.
        bool revoked{ false };///< Has it been revoked?
        string roleId{ "" };///< Role Id.
        string name{ "" };///< Name of the integration.
        string type{ "" };///< Type of integration.
        UserData user{};///< User data for the integration.
    };

    /// Audit log event. \brief Audit log event.
    enum class AuditLogEvent {
        GUILD_UPDATE = 1,///< Guild Update
        CHANNEL_CREATE = 10,///<Channel Create
        CHANNEL_UPDATE = 11,///<Channel Update
        CHANNEL_DELETE = 12,///< Channel Delete
        CHANNEL_OVERWRITE_CREATE = 13,///< Channel Overwrite Create
        CHANNEL_OVERWRITE_UPDATE = 14,///< Channel Overwrite Update
        CHANNEL_OVERWRITE_DELETE = 15,///< Channel Overwrite Delete
        MEMBER_KICK = 20,///< Member Kick
        MEMBER_PRUNE = 21,///< Member Prune
        MEMBER_BAN_ADD = 22,///< Member Ban Add
        MEMBER_BAN_REMOVE = 23,///< Member Ban Remove
        MEMBER_UPDATE = 24,///< Member Update
        MEMBER_ROLE_UPDATE = 25,///< Member Role Update
        MEMBER_MOVE = 26,///< Member Move
        MEMBER_DISCONNECT = 27,///< Member Disconnect
        BOT_ADD = 28,///< Bot Add
        ROLE_CREATE = 30,///< Role Create
        ROLE_UPDATE = 31,///< Role Update
        ROLE_DELETE = 32,///< Role Delete
        INVITE_CREATE = 40,///< Invite Create
        INVITE_UPDATE = 41,///< Invite Update
        INVITE_DELETE = 42,///< Invite Delete
        WEBHOOK_CREATE = 50,///<Webhook Create
        WEBHOOK_UPDATE = 51,///<Webhook Update
        WEBHOOK_DELETE = 52,///<Webhook Delete
        EMOJI_CREATE = 60,///<Emoji Create
        EMOJI_UPDATE = 61,///<Emoji Update
        EMOJI_DELETE = 62,///<Emoji Delete
        MESSAGE_DELETE = 72,///<Message Delete
        MESSAGE_BULK_DELETE = 73,///<Message Bulk Delete
        MESSAGE_PIN = 74,///<Message Pin
        MESSAGE_UNPIN = 75,///<Message Unpin
        INTEGRATION_CREATE = 80,///<Integration Create
        INTEGRATION_UPDATE = 81,///<Integration Update
        INTEGRATION_DELETE = 82///<Integration Delete
    };

    /// Audit log entry info data \brief Audit log entry info data.
    struct DiscordCoreAPI_Dll AuditLogEntryInfoData : public DiscordEntity {
        string deleteMemberDays{ "" };///<Number of days for which the member's Messages were deleted.
        string membersRemoved{ "" };///< Number of members that were removed upon a prune.
        string channelId{ "" };///< Channel Id.
        string messageId{ "" };///< Message Id.
        string roleName{ "" };///< Role name.
        string count{ "" };///<Count.
        string type{ "" };///< Type.
    };

    /// Audit log change data. \brief Audit log change data.
    struct DiscordCoreAPI_Dll AuditLogChangeData {
        string newValueString{ "" };///< New value, if it's a string.
        string oldValueString{ "" };///< Old value, if it's a string.
        bool newValueBool{ false };///< New value, if it's a bool.
        bool oldValueBool{ false };///< Old value, if it's a bool.
        __int32 newValueInt{ 0 };///< New value, if it's an __int32.
        __int32 oldValueInt{ 0 };///< Old value, if it's an __int32.
        string key{ "" };///< The key of the audit log change.
    };

    /// Audit log entry data. \brief Audit log entry data.
    struct DiscordCoreAPI_Dll AuditLogEntryData : public DiscordEntity {
        vector<AuditLogChangeData> changes{};///< Array of audit log change data.
        TimeStamp createdTimeStamp{ "" };///< Time at which this entry was created.
        AuditLogEntryInfoData options{};///< Audit log entry info data.
        AuditLogEvent actionType{};///< Audit log action type.
        string targetId{ "" };///< Id of the target User.
        string userId{ "" };///< Id of the executing User.
        string reason{ "" };///< The reason that was entered for the given change.
    };   

    /// Party data. \brief Party data.
    struct DiscordCoreAPI_Dll PartyData : public DiscordEntity {

        operator DiscordCoreInternal::PartyData() {
            DiscordCoreInternal::PartyData newData;
            newData.size[0] = this->size[0];
            newData.size[1] = this->size[1];
            newData.id = this->id;
            return newData;
        }

        vector<__int32> size{ 0, 0 };///< The size of the party.
    };

    /// Assets data. \brief Party data.
    struct DiscordCoreAPI_Dll AssetsData {

        operator DiscordCoreInternal::AssetsData() {
            DiscordCoreInternal::AssetsData newData;
            newData.largeImage = this->largeImage;
            newData.smallImage = this->smallImage;
            newData.largeText = this->largeText;
            newData.smallText = this->smallText;
            return newData;
        }

        string largeImage{ "" };    ///< Keyname of an asset to display.
        string smallImage{ "" };    ///< Keyname of an asset to display.
        string largeText{ "" };     ///< Hover text for the large image.
        string smallText{ "" };     ///< Hover text for the small image.
    };

    /// Secrets data. \brief Secrets data.
    struct DiscordCoreAPI_Dll SecretsData {

        operator DiscordCoreInternal::SecretsData() {
            DiscordCoreInternal::SecretsData newData;
            newData.spectate = this->spectate;
            newData.match = this->match;
            newData.join = this->join;
            return newData;
        }

        string spectate{ "" }; ///< Unique hash for the given match context.
        string match{ "" };	///< Unique hash for Spectate button.
        string join{ "" };	///< Unique hash for chat invitesand Ask to Join.
    };

    /// Timestamp data. \brief Timestamp data.
    struct DiscordCoreAPI_Dll TimestampData {

        operator DiscordCoreInternal::TimestampData() {
            DiscordCoreInternal::TimestampData newData;
            newData.start = this->start;
            newData.end = this->end;
            return newData;
        }

        __int64 start{ 0 }; ///< Unix timestamp - Send this to have an "elapsed" timer.
        __int64 end{ 0 };   ///< Unix timestamp - send this to have a "remaining" timer.
    };

    /// Button data. \brief Button data.
    struct DiscordCoreAPI_Dll ButtonData {

        operator DiscordCoreInternal::ButtonData() {
            DiscordCoreInternal::ButtonData newData;
            newData.label = this->label;
            newData.url = this->url;
            return newData;
        }

        string label{ "" }; ///< Visible label of the button.
        string url{ "" };   ///< URL to display on the button.
    };

    /// Activity type. \brief Activity type.
    enum class ActivityType {
        Game = 0,///< Gaming
        Streaming = 1,///< Streaming
        Listening = 2,///< Listening
        Watching = 3,///< Watching
        Custom = 4,///< Custom
        Competing = 5///< Competing
    };

    /// Activity data. \brief Activity data.
    struct DiscordCoreAPI_Dll ActivityData {

        operator DiscordCoreInternal::ActivityData() {
            DiscordCoreInternal::ActivityData newData;
            newData.applicationId = this->applicationId;
            newData.timestamps = this->timestamps;
            newData.createdAt = this->createdAt;
            newData.instance = this->instance;
            newData.secrets = this->secrets;
            newData.details = this->details;
            newData.buttons = this->buttons;
            newData.type = (__int32)this->type;
            newData.assets = this->assets;
            newData.emoji = this->emoji;
            newData.flags = this->flags;
            newData.party = this->party;
            newData.state = this->state;
            newData.name = this->name;
            newData.url = this->url;
            return newData;
        }

        TimestampData timestamps{}; ///< Timestamp data.
        string applicationId{ "" }; ///< Application id for the current application.
        __int32 createdAt{ 0 };     ///< Timestamp of when the activity began.
        bool instance{ false }; ///< Whether this activity is an instanced context, like a match.
        SecretsData secrets{}; ///< Secrets data.
        string details{ "" };   ///< Details about the activity.
        ButtonData buttons{};   ///< Button Data.
        ActivityType type{};    ///< Activity data.
        AssetsData assets{};    ///< Assets data.
        __int32 flags{ 0 }; ///< Flags.
        string state{ "" };      ///< The player's current party status
        EmojiData emoji{};  ///< Emoji associated with the activity.
        PartyData party{};  ///< Party data.
        string name{ "" };  ///< Name of the activity.
        string url{ "" };   ///< Url associated with the activity.        
    };
    
    /// Client status data. \brief Client status data.
    struct DiscordCoreAPI_Dll ClientStatusData {
        string desktop{ "" }; ///< Desktop name.
        string mobile{ "" };///< Mobile name.
        string web{ "" };///< Web link.
    };

    /// Describes the server's boost level. \brief Describes the server's boost level.
    enum class PremiumTier {
        NONE = 0,   ///< The Guild has not unlocked any Server Boost perks.
        TIER_1 = 1, ///< The Guild has unlocked Server Boost level 1 perks.
        TIER_2 = 2, ///< The Guild has unlocked Server Boost level 2 perks
        TIER_3 = 3 ///< The Guild has unlocked Server Boost level 3 perks.
    };

    /// Default Message notification level. \brief Default Message notification level.
    enum class DefaultMessageNotificationLevel {
        ALL_MESSAGES = 0,///< All Messages
        ONLY_MENTIONS = 1///< Only Mentions
    };

    /// Explicit content filtering level, by default. \brief Explicit content filtering level, by default.
    enum class ExplicitContentFilterLevel {
        DISABLED = 0, ///< Disabled
        MEMBERS_WITHOUT_ROLES = 1,///<Members Without Roles
        ALL_MEMBERS = 2///< All Members
    };

    ///  MFA level. \brief MFA level.
    enum class MFALevel {
        NONE = 0,///< None
        ELEVATED = 1///< Elevated
    };

    /// Verification level required. \brief Verification level required.
    enum class VerificationLevel {
        NONE = 0,///< None
        LOW = 1,///< Low
        MEDIUM = 2,///< Medium
        HIGH = 3,///< High
        VERY_HIGH = 4///< Very High
    };

    /// Welcome screen Channel data. \brief Welcome screen Channel data.
    struct DiscordCoreAPI_Dll WelcomeScreenChannelData {
        string description{ "" }; ///< Description of the welcome Channel.
        string channelId{ "" };///< Id of the welcome Channel.
        string emojiName{ "" };///< Emoji name for the Channel.
        string emojiId{ "" };///< Emoji id for the Channel.
    };

    /// Welcome screen data. \brief Welcome screen data.
    struct DiscordCoreAPI_Dll WelcomeScreenData {
        vector<WelcomeScreenChannelData> welcomeChannels{}; ///< Welcome screen Channel data.
        string description{ "" };   ///< Description of the welcome screen.
    };

    /// Presence update data. \brief Presence update data.
    struct DiscordCoreAPI_Dll PresenceUpdateData {
        vector<ActivityData> activities{}; ///< Array of activities.
        ClientStatusData clientStatus{};    ///< Current client status.
        string guildId{ "" };   ///< Guild id for the current presence.
        string status{ "" };    ///< Status of the current presence.
        UserData user{};    ///< User data for the current presence.
    };

    /// Stage instance data. \brief Stage instance data.
    struct DiscordCoreAPI_Dll StageInstanceData : public DiscordEntity {
        bool discoverableDisabled{ false }; ///< Is it discoverable?
        __int32 privacyLevel{ 0 };  ///< Privacy level of the Channel.
        string channelId{ "" }; ///< The Channel's id.
        string guildId{ "" };   ///< The Guild id for which the Channel exists in.
        string topic{ "" }; ///< The topic of the stage instance.
    };

    /// Sticker type. \brief Sticker type.
    enum class StickerType {
        STANDARD = 1,///< Standard
        GUILD = 2///< Guild
    };

    /// Sticker data. \brief Sticker data.
    struct DiscordCoreAPI_Dll StickerData : public DiscordEntity {
        StickerFormatType formatType{}; ///< Format type.
        string description{ "" };   ///< Description of the sticker.
        bool available{ false };    ///< Is it available for use?
        __int32 nsfwLevel{ 0 }; ///< NSFW warning level.
        __int32 sortValue{ 0 };///< Where in the stack of stickers it resides.
        string guildId{ "" };   ///< The Guild id for which the sticker exists in.
        string packId{ "" };    ///< Pack id of the sticker.    
        string asset{ "" }; ///< Asset value for the sticker.
        StickerType type{}; ///< The type of sticker.
        string name{ "" }; ///< The sticker's name.
        string tags{ "" };  ///< Tags for the sticker to use.
        UserData user{};    ///< The user that uploaded the Guild sticker.
    };

    /// Data structure representing a single guiild. \brief Data structure representing a single guiild.
    struct DiscordCoreAPI_Dll GuildData : public DiscordEntity {
        DefaultMessageNotificationLevel defaultMessageNotifications{};///<Default Message notification level.
        ExplicitContentFilterLevel explicitContentFilter{}; ///< Explicit content filtering level, by default.
        vector<StageInstanceData> stageInstances{}; ///< Array of stage instances.
        vector<PresenceUpdateData> presences{}; ///< Array of presences for each GuildMember.
        __int32 premiumSubscriptionCount{ 0 }; ///< Premium subscription count.
        __int32 approximatePresenceCount{ 0 }; ///< Approximate quantity of presences.
        VerificationLevel verificationLevel{};  ///< Verification level required.
        vector<VoiceStateData> voiceStates{};   ///< Array of Guild-member voice-states.
        string publicUpdatesChannelId{ "" }; ///< Id of the public updates Channel.        
        __int32 approximateMemberCount{ 0 };    ///< Approximate member count.
        vector<GuildMemberData> members{};  ///< Array of GuildMembers.
        WelcomeScreenData welcomeScreen{};  ///< Welcome screen for the Guild.
        __int32 maxVideoChannelUsers{ 0 };  ///< Maximum quantity of users per video Channel.
        __int32 systemChannelFlags{ 0 };    ///< System Channel flags.
        vector<ChannelData> channels{}; ///< Array of Guild channels.
        vector<StickerData> stickers{}; ///< Array of Guild stickers.
        vector<ChannelData> threads{};  ///< Array of Guild threads.        
        string discoverySplash{ "" };   ///< Link to the discovery image's splash.
        string preferredLocale{ "" };   ///< Preferred locale, for voice chat servers.
        string widgetChannelId{ "" };   ///< Channel id for the Guild's widget.
        string systemChannelId{ "" };   ///< Channel id for the Guild's system Channel.
        string rulesChannelId{ "" };    ///< Channel id for the Guild's rules Channel.
        bool widgetEnabled{ false };    ///< Is the widget enabled?        
        string vanityURLCode{ "" }; ///< Vanity URL code, if applicable.
        string applicationId{ "" }; ///< The current application id.
        __int32 maxPresences{ 0 };  ///< Max number of presences allowed.
        vector<EmojiData> emoji{};  ///< Array of Guild emojis.
        string afkChannelId{ "" };  ///< Channel if of the "afk" Channel,
        vector<string> features{};  ///< List of Guild features.
        bool unavailable{ false };  ///< Is the Guild currently available to the bot?
        PremiumTier premiumTier{};  ///< What is the premium tier?
        string permissions{ "" };   ///< Current permissions for the bot in the Guild.
        vector<RoleData> roles{};   ///< Array of Guild roles.
        string description{ "" };   ///< Description of the Guild.
        __int32 memberCount{ 0 };   ///< Member count.
        __int32 afkTimeOut{ 0 };    ///< Time for an individual to time out as afk.
        __int32 maxMembers{ 0 };    ///< Max quantity of members.
        __int32 nsfwLevel{ 0 }; ///< NSFW warning level.
        string createdAt{ "" }; ///< When was the Guild created?
        string iconHash{ "" };      ///< Url to the Guild's icon.
        string joinedAt{ "" };  ///< When the bot joined this Guild.
        string ownerId{ "" };   ///< User id of the Guild's owner.
        string region{ "" };    ///< Region of the world where the Guild's servers are.
        bool owner{ false };    ///< Is the bot the owner?        
        MFALevel mfaLevel{};    ///< MFA level.
        bool large{ false };    ///< Is this Guild "large"?        
        string splash{ "" };    ///< URL to the Guild's splash.
        string banner{ "" };    ///< URL to the Guild's banner.       
        string icon{ "" };  ///< URL to the Guild's icon.
        string name{ "" };  ///< The Guild's name.
    };

    /// Invite data. \brief Invite data.
    struct DiscordCoreAPI_Dll InviteData {
        __int32 approximatePresenceCount{ 0 };///< Approximate presence count.
        ApplicationData targetApplication{};///< Application data.
        __int32 approximateMemberCount{ 0 };///< Approximate member count.
        StageInstanceData stageInstance{};///< Stage instance data.        
        __int32 targetType{ 0 };///< Target type.
        bool temporary{ false };///< Is it temporary?
        string createdAt{ "" };///< Time it was created at.
        string expiresAt{ "" };///< When the invite expires.
        string channelId{ "" };///< Channel id for which the invite is for.
        ChannelData channel{};///< Channel data of the Channel that the invite is for.
        UserData targetUser{};///< Target User id of the invite.
        string guildId{ "" };///< Guild id for which the invite is for.
        __int32 maxUses{ 0 };///< Max number of uses.
        __int32 maxAge{ 0 };///< Maximum age of the invite.
        UserData inviter{};///< The User who created the invite.
        GuildData Guild{};///< Guild data of the Channel that the invite is for.
        string code{ "" };///< Unique invite code.
        __int32 uses{ 0 };///< Maximum nunmber of uses.
    };

    /// Webhook data. \brief Webhook data.
    struct DiscordCoreAPI_Dll WebhookData : public DiscordEntity {
        ChannelData sourceChannel{};///< Channel for which the webhook was issued.
        string applicationId{ "" };///< Application id.
        GuildData sourceGuild{};///< Source Guild id.
        string channelId{ "" };///< Channel id for which the webhook was issued.
        string guildId{ "" };///< Guild id for which the webhook was issued.
        string avatar{ "" };///< Avatar of the webhook.
        string token{ "" };///< Token of the webhook.
        __int32 type{ 0 };///< Type of webhook.
        string name{ "" };///< Name of the webhook.
        string url{ "" };///< Url of the webhook.
        UserData user{};///< User which create the webhook.
    };

    /// Audit log data. \brief Audit log data.
    class DiscordCoreAPI_Dll AuditLogData {
    public:
        auto getAuditLogData(string userIdOfChanger, AuditLogEvent auditLogType) {
            for (auto value : this->auditLogEntries) {
                if (value.id == userIdOfChanger && value.actionType == auditLogType) {
                    return value;
                }
            }
            return AuditLogEntryData();
        }
        auto getAuditLogData(AuditLogEvent auditLogType, string userIdOfTarget) {
            for (auto value : this->auditLogEntries) {
                if (value.targetId == userIdOfTarget && value.actionType == auditLogType) {
                    return value;
                }
            }
            return AuditLogEntryData();
        }
        vector<AuditLogEntryData> auditLogEntries{};///< Array of audit log entries.
        vector<IntegrationData> integrations{};///< Integration data.
        vector<WebhookData> webhooks{};///<Array of webhook data.
        vector<ChannelData> threads{};///< Array of Channel data.
        vector<UserData> users{};///< Array of u
    };

    struct DiscordCoreAPI_Dll ReactionRemoveData {
        string channelId{ "" };
        string messageId{ "" };
        string guildId{ "" };
        string userId{ "" };
        EmojiData emoji{};
    };

    struct DiscordCoreAPI_Dll InteractionPackageData {
        string interactionToken{ "" };
        string applicationId{ "" };
        string interactionId{ "" };
    };

    struct DiscordCoreAPI_Dll MessagePackageData {
        string channelId{ "" };
        string messageId{ "" };
    };

    /// Data structure representing an ApplicationCommand's option choice. \brief Data structure representing an ApplicationCommand's option choice.
    struct DiscordCoreAPI_Dll ApplicationCommandOptionChoiceData {
        operator DiscordCoreInternal::ApplicationCommandOptionChoiceData() {
            DiscordCoreInternal::ApplicationCommandOptionChoiceData newData;
            newData.valueString = this->valueString;
            newData.valueFloat = this->valueFloat;
            newData.valueInt = this->valueInt;
            newData.name = this->name;
            return newData;
        }
        string valueString{ "" }; ///< The value, if the given choice is a string.
        float valueFloat{ 0.0f }; ///< The value, if the given choice is a float.
        __int32	valueInt{ 0 }; ///< The value, if the given choice is an __int32.
        string name{ "" };  ///< The name of the current choice.        
    };

    /// Data structure representing an ApplicationCommand's option. \brief Data structure representing an ApplicationCommand's option.
    struct DiscordCoreAPI_Dll ApplicationCommandOptionData {
        operator DiscordCoreInternal::ApplicationCommandOptionData() {
            DiscordCoreInternal::ApplicationCommandOptionData newData;
            for (auto value : this->choices) {
                newData.choices.push_back(value);
            }
            newData.description = this->description;
            newData.name = this->name;
            for (auto value : this->options) {
                newData.options.push_back(value);
            }
            newData.required = this->required;
            newData.type = (DiscordCoreInternal::ApplicationCommandOptionType)this->type;
            return newData;
        }
        vector<ApplicationCommandOptionChoiceData>	choices{}; ///< A vector of possible choices for the current ApplicationCommand option.
        vector<ApplicationCommandOptionData> options{};///< A vector of possible options for the current ApplicationCommand option.
        ApplicationCommandOptionType type{};///< The type of command option.
        vector<ChannelType> channelTypes{};///< Set when the ApplicationCommand option type is set to Channel.
        string description{ "" };///< A description of the current ApplicationCommand option.
        bool required{ false };///< Whether this option is required to enter the command or not.
        string name{ "" };///< Name of the current ApplicationCommand option.
    };

    struct DiscordCoreAPI_Dll TypingStartData {
        GuildMemberData member{};
        __int32 timestamp{ 0 };
        string channelId{ "" };
        string guildId{ "" };
        string userId{ "" };
    };

    struct DiscordCoreAPI_Dll YouTubeFormat {
        string signatureCipher{ "" };
        string audioSampleRate{ "" };
        __int32 averageBitrate{ 0 };
        __int64 contentLength{ 0 };
        string audioQuality{ "" };
        string downloadURL{ "" };
        string signature{ "" };
        string mimeType{ "" };
        __int32 bitrate{ 0 };
        string quality{ "" };
        string codecs{ "" };
        string aitags{ "" };
        __int32 height{ 0 };
        __int32 width{ 0 };
        __int32 itag{ 0 };
        __int32 fps{ 0 };
    };

    /// ApplicationCommand type. \brief ApplicationCommand type.
    enum class ApplicationCommandType {
        CHAT_INPUT = 1,///< Chat-Input
        USER = 2,///< User
        MESSAGE = 3///< Message
    };

    /// User command interaction data. \brief User command interaction data.
    struct DiscordCoreAPI_Dll UserCommandInteractionData {
        string targetId{ "" }; ///< The target User's id.
    };

    /// Message command interacction data. \brief Message command interacction data.
    struct DiscordCoreAPI_Dll MessageCommandInteractionData {
        string targetId{ "" }; ///< The target Message's id.
    };

    enum class ComponentType;

    /// Component interaction data. \brief Component interaction data.
    struct DiscordCoreAPI_Dll ComponentInteractionData {
        ComponentType componentType{}; ///< The type of component.
        vector<string> values{};///< The values of the components.
        string customId{ "" };///< The custom id of the interaction entity.
    };

    /// Allowable mentions for a Message. \brief Allowable mentions for a Message.
    struct DiscordCoreAPI_Dll AllowedMentionsData {

        operator DiscordCoreInternal::AllowedMentionsData() {
            DiscordCoreInternal::AllowedMentionsData newData;
            newData.repliedUser = this->repliedUser;
            newData.parse = this->parse;
            newData.roles = this->roles;
            newData.users = this->users;
            return newData;
        }

        bool repliedUser{ false };///< For replies, whether to mention the author of the Message being replied to (default false).
        vector<string> parse{}; ///< A vector of allowed mention types to parse from the content.
        vector<string> roles{}; ///< Array of role_ids to mention (Max size of 100)
        vector<string> users{}; ///<Array of user_ids to mention (Max size of 100)
    };

    /// Interaction type. \brief Interaction type.
    enum class InteractionType {
        PING = 1,///< Ping type.
        APPLICATION_COMMAND = 2,///< ApplicationCommand type.
        MESSAGE_COMPONENT = 3///<Message component type.
    };

    /// Represents a single selection from a select-menu. \brief Represents a single selection from a select-menu.
    struct DiscordCoreAPI_Dll SelectOptionData {

        operator DiscordCoreInternal::SelectOptionData() {
            DiscordCoreInternal::SelectOptionData newData;
            newData.description = this->description;
            newData._default = this->_default;
            newData.emoji = this->emoji;
            newData.label = this->label;
            newData.value = this->value;
            return newData;
        }

        string description{ "" };///< Description of the select-menu-option.
        bool _default{ false };///<Is it the default option?
        string label{ "" };///< A visible label for the select-menu-option.
        string value{ "" };///< A value for identifying the option.
        EmojiData emoji{};///< An optional emoji to put on it.
    };

    /// The type of a given component. \brief The type of a given component.
    enum class ComponentType {
        ActionRow = 1, ///< Action row component type.
        Button = 2, ///< Button component type.
        SelectMenu = 3  ///< Select-menu component type.
    };

    /// The style of a given button. \brief The style of a given button.
    enum class ButtonStyle {
        Primary = 1, ///< Primary button style.
        Success = 3,///< Success button style.
        Secondary = 2,///< Secondary button style.
        Danger = 4,///< Danger button style.
        Link = 5    ///< Link button style, for URLs.
    };

    /// Represents a single Message-component. \brief Represents a single Message-component.
    struct DiscordCoreAPI_Dll ComponentData {

        operator DiscordCoreInternal::ComponentData() {
            DiscordCoreInternal::ComponentData newData;
            newData.style = (DiscordCoreInternal::ButtonStyle)this->style;
            newData.type = (DiscordCoreInternal::ComponentType)this->type;
            newData.placeholder = this->placeholder;
            for (auto value : this->options) {
                newData.options.push_back(value);
            }
            newData.maxValues = this->maxValues;
            newData.minValues = this->minValues;
            newData.customId = this->customId;
            newData.disabled = this->disabled;
            newData.emoji = this->emoji;
            newData.label = this->label;
            newData.url = this->url;
            return newData;
        }

        vector<SelectOptionData> options{}; ///< A vector of select-options, in the case of this being a select-menu.
        string placeholder{ "" };   ///< Custom placeholder text if nothing is selected, max 100 characters.
        bool disabled{ false }; ///< Whether this component is active.
        string customId{ "" };  ///< A custom id for identifying the component.
        ComponentType type{};   ///< Which type of component?
        ButtonStyle style{};    ///< Which style of button?
        string label{ "" }; ///< What is the visible label?
        __int32 maxValues{ 0 }; ///< Maximum number of selectable options?
        __int32 minValues{ 0 }; ///< Minimum number of required selectable options?
        EmojiData emoji{}; ///< An emoji to put on the component.
        string url{ "" }; ///< An optional URL to place.
    };

    /// Action row data of Message components. \brief Action row data of Message components.
    struct DiscordCoreAPI_Dll ActionRowData {
        operator DiscordCoreInternal::ActionRowData() {
            DiscordCoreInternal::ActionRowData newData;
            for (auto value : this->components) {
                newData.components.push_back(value);
            }
            return newData;
        }
        vector<ComponentData> components{}; ///< Array of components to make up the action-row.
    };

    /// Interaction callback type. \brief Interaction callback type.
    enum class InteractionCallbackType {
        Pong = 1,///< Pong.
        ChannelMessageWithSource = 4,///< Sending a Channel Message with source.
        DeferredChannelMessageWithSource = 5,///< Deferred Channel Message with source.
        DeferredUpdateMessage = 6,///< Deferred update Message.
        UpdateMessage = 7///< Update Message.
    };

    /// Interaction ApplicationCommand callback data. \brief Interaction ApplicationCommand callback data.
    struct DiscordCoreAPI_Dll InteractionApplicationCommandCallbackData {

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

        AllowedMentionsData allowedMentions{};///< Allowed mentions data.
        vector<ActionRowData> components{};///< Message components.
        vector<EmbedData> embeds{};///< Message embeds.
        string content{ "" };///< Message content.
        bool tts{ false };///< Is it TTS?
        __int32 flags{ 0 };///< Flags.
    };

    /// Interaction response data. \brief Interaction response data.
    struct DiscordCoreAPI_Dll InteractionResponseData {
        InteractionApplicationCommandCallbackData data{}; ///< Interaction ApplicationCommand callback data.
        InteractionCallbackType type{};///< Interaction callback type.
    };
    
    /// Data structure representing an ApplicationCommand. \brief Data structure representing an ApplicationCommand.
    struct DiscordCoreAPI_Dll ApplicationCommandData : public DiscordEntity {
        vector<ApplicationCommandOptionData> options{};///< A vector of possible options for the current ApplicationCommand.
        bool defaultPermission{ false };///< Whether or not the default permission in the Guild is to have access to this command or not.
        ApplicationCommandType type{};///< The type of ApplicationCommand.
        string applicationId{ "" };///< The current application id.
        string description{ "" };///< A description of the current ApplicationCommand.
        string guildId{ "" };///< (Where applicable) a Guild id for which to assign this ApplicationCommand to.
        string version{ "" };///< An autoincremented version.
        string name{ "" };///< Name of the current ApplicationCommand.
    };

    /// Function data for repeated functions to be loaded. \brief Function data for repeated functions to be loaded.
    struct DiscordCoreAPI_Dll RepeatedFunctionData {
        function<void()> function{ nullptr };///< The function pointer to be loaded.
        bool repeated{ false }; ///< Whether or not the function is repeating.
        __int32 intervalInMs{ 0 };  ///< The time interval at which to call the function.
    };

    /// Channel mention data. \brief Channel mention data.
    struct DiscordCoreAPI_Dll ChannelMentionData : public DiscordEntity {

        operator DiscordCoreInternal::ChannelMentionData() {
            DiscordCoreInternal::ChannelMentionData newData;
            newData.guildId = this->guildId;
            newData.name = this->name;
            newData.type = (DiscordCoreInternal::ChannelType)this->type;
            newData.id = this->id;
            return newData;
        }

        string guildId{ "" }; ///< The id of the Guild where it took place.
        string name{ "" }; ///< The name of the Channel that was mentioned.
        ChannelType type{}; ///< The type of Channel that was mentioned.
    };

    /// Data for when some Channel pins are updated. \brief Data for when some Channel pins are updated.
    struct DiscordCoreAPI_Dll ChannelPinsUpdateEventData {
        string lastPinTimestamp{ "" }; ///< The time of the last pinned Message.
        string channelId{ "" };     ///< The id of the Channel within which the Message was pinned.
        string guildId{ "" };   ///< The id of the Guild within which the Message was pinned.
    };

    /// Data for when threads are synced. \brief Data for when threads are synced.
    struct DiscordCoreAPI_Dll ThreadListSyncData {
        vector<ThreadMemberData> members{}; ///< Array of members that are a part of the thread.
        vector<ChannelData> threads{};  ///< All active threads in the given channels that the current User can access.
        vector<string> channelIds{};    ///< The parent Channel ids whose threads are being synced. If omitted, then threads were synced for the entire Guild. This array may contain channel_ids that have no active threads as well, so you know to clear that data.
        string guildId{ "" };   ///< The id of the Guild for which the threads are being synced.
    };

    /// Represents a thread-members-update. \brief Represents a thread-members-update.
    struct DiscordCoreAPI_Dll ThreadMembersUpdateData : public DiscordEntity {
        vector<ThreadMemberData> addedMembers{}; ///< New members added to the thread.
        vector<string> removedMemberIds{}; ///< Members who have been removed.
        string guildId{ "" };   ///< Guild id of the thread.
        __int32 memberCount{ 0 };///< Number of Guild-members in the thread.
    };
    
    /// Message interaction data.
    struct DiscordCoreAPI_Dll MessageInteractionData : public DiscordEntity {

        operator DiscordCoreInternal::MessageInteractionData() {
            DiscordCoreInternal::MessageInteractionData newData;
            newData.type = (DiscordCoreInternal::InteractionType)this->type;
            newData.name = this->name;
            newData.user = this->user;
            newData.id = this->id;
            return newData;
        }

        InteractionType type{};
        string name{ "" };
        UserData user{};
    };

    /// Message type. \brief Message type.
    enum class MessageType { 
        DEFAULT = 0,///< Default
        RECIPIENT_ADD = 1,///< Recipient Add
        RECIPIENT_REMOVE = 2,///< Recipient Remove
        CALL = 3,///< Call
        CHANNEL_NAME_CHANGE = 4,///< Channel Name Change
        CHANNEL_ICON_CHANGE = 5,///< Channel Icon Change
        CHANNEL_PINNED_MESSAGE = 6,///< Channel Pinned Message
        GUILD_MEMBER_JOIN = 7,///< Guild Member Join
        USER_PREMIUM_GUILD_SUBSCRIPTION = 8,///< User Premium Guild Subscription
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_1 = 9,///< User Premium Guild Subscription Tier 1
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_2 = 10,///< User Premium Guild Subscription tier 2
        USER_PREMIUM_GUILD_SUBSCRIPTION_TIER_3 = 11,///< User Premium Guild Subscription Tier 3
        CHANNEL_FOLLOW_ADD = 12,///< Channel Follow Add
        GUILD_DISCOVERY_DISQUALIFIED = 14,///< Guild Discovery Disqualified
        GUILD_DISCOVERY_REQUALIFIED = 15,///< Guild Discovery Requalified
        GUILD_DISCOVERY_GRACE_PERIOD_INITIAL_WARNING = 16,///< Guild Discovery Grace Period Initial Warning
        GUILD_DISCOVERY_GRACE_PERIOD_FINAL_WARNING = 17,///< Guild Discovery Grace Period Final Warning
        THREAD_CREATED = 18,///< Thread Created
        REPLY = 19,///< Reply
        CHAT_INPUT_COMMAND = 20,///< Chat Input Command
        THREAD_STARTER_MESSAGE = 21,///< Thread Starter Message
        GUILD_INVITE_REMINDER = 22,///< Guild Invite Reminder
        CONTEXT_MENU_COMMAND = 23///< Context Menu Command
    };

    /// Message sticker item type. \brief Message sticker item type.
    enum class StickerItemType {
        PNG = 1,///< PNG
        APNG = 2,///< APNG
        LOTTIE = 3///< Lottie
    };

    /// Message sticker item data. \brief Message sticker item data.
    struct DiscordCoreAPI_Dll StickerItemData : public DiscordEntity {

        operator DiscordCoreInternal::StickerItemData() {
            DiscordCoreInternal::StickerItemData newData;
            newData.formatType = (DiscordCoreInternal::StickerItemType)this->formatType;
            newData.name = this->name;
            newData.id = this->id;
            return newData;
        }

        StickerItemType formatType{}; ///< Message sticker item type.
        string name{ "" }; ///< The name of the sticker.
    };

    /// The core of a Message's data structure. \brief The core of a Message's data structure.
    struct DiscordCoreAPI_Dll MessageDataOld : public DiscordEntity {
        vector<ChannelMentionData> mentionChannels{};///< array of Channel mention data.
        MessageReferenceData messageReference{};///< Message reference data.
        vector<StickerItemData> stickerItems{};///< Array of Message sticker item data.
        vector<AttachmentData> attachments{};///< Array of attachment data.
        MessageInteractionData interaction{};///< Message interaction data.
        vector<ActionRowData> components{};///< Array of action row data.
        vector<ReactionData> reactions{};//< Array of reaction data.
        vector<StickerData> stickers{};///< Array of Message sticker data.
        MessageActivityData activity{};///< Message activity data.
        ApplicationData application{};///< Application data.
        vector<string> mentionRoles{};///< Vector of "mention roles" ids.
        bool mentionEveryone{ false };///< Does the Message mention everyone?
        string editedTimestamp{ "" };///< The time at which it was edited.
        vector<UserData> mentions{};///< Array of User data, for individual's that were mentioned.
        vector<EmbedData> embeds{};///< Array of Message embeds.
        string applicationId{ "" };///< Application id.
        string timestampRaw{ "" };///< The raw timestamp of when the Message was created.
        GuildMemberData member{};///< The author's Guild member data.
        string channelId{ "" };///< The Channel it was sent in.
        string timestamp{ "" };///< The timestamp of when the Message was created.
        string webhookId{ "" };///< Webhook id of the Message, if applicable.
        string guildId{ "" };///< The id of the Guild the Message was sent in.
        bool pinned{ false };///< Is it pinned?
        string content{ "" };///< The Message's content.
        ChannelData thread{};///< The thread that the Message was sent in, if applicable.
        string nonce{ "" };///< Nonce.
        MessageType type{};///< Message type.
        UserData author{};///< The author's User data.
        bool tts{ false };///< Is it a text-to-speech Message?
        __int32 flags{ 0 };///< Flags.
    };

    /// Data structure representing a single Message. \brief Data structure representing a single Message.
    struct DiscordCoreAPI_Dll MessageData :public MessageDataOld {
        MessageDataOld referencedMessage{}; ///< The referenced Message, to reply to.
        string requesterId{ "" }; ///< Requester's id, of who sent this Message.
    };

    /// Resolved data.
    struct DiscordCoreAPI_Dll ResolvedData {
        map<string, GuildMemberData> members{}; ///< Map full of GuildMemeberData.
        map<string, MessageData> messages{};///< Map full of MessageData.
        map<string, ChannelData> channels{};///< Map full of ChannelData.
        map<string, UserData> users{};///< Map full of UserData.
        map<string, RoleData> roles{};///< Map full of RoleData.
    };

    struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption;

    DiscordCoreAPI_Dll vector<DiscordCoreInternal::ApplicationCommandInteractionDataOption> convertAppCommandInteractionDataOptions(vector<ApplicationCommandInteractionDataOption> originalOptions);

    /// ApplicationCommand interaction data option. \brief ApplicationCommand interaction data option.
    struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption {

        operator DiscordCoreInternal::ApplicationCommandInteractionDataOption() {
            DiscordCoreInternal::ApplicationCommandInteractionDataOption newData;
            newData.type = (DiscordCoreInternal::ApplicationCommandOptionType)this->type;
            newData.options = convertAppCommandInteractionDataOptions(this->options);
            newData.valueString = this->valueString;
            newData.valueBool = this->valueBool;
            newData.valueInt = this->valueInt;
            newData.name = this->name;
            return newData;
        }

        vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand interaction data options.
        ApplicationCommandOptionType type{};///< The type of ApplicationCommand options.
        string valueString{ "" };///< The value if it's a string.
        bool valueBool{ false };///< the value if it's a bool.
        string name{ "" };///< The name of the current option.
        __int32 valueInt{ 0 };///< The value if it's an __int32.

    };

    /// ApplicationCommand interaction data.
    struct DiscordCoreAPI_Dll ApplicationCommandInteractionData : public DiscordEntity {
        vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand interaction data options.
        ApplicationCommandType type{}; ///< The type of ApplicationCommand.
        ResolvedData resolved{};///< Resolved data.
        string name{ "" };///< The name of the command.
    };

    /// Interaction data data. \brief Interaction data data.
    struct DiscordCoreAPI_Dll InteractionDataData {
        ApplicationCommandInteractionData applicationCommanddata{};///< ApplicationCommand interaction data.
        MessageCommandInteractionData messageInteractionData{};///< Message command interaction data.
        UserCommandInteractionData userInteractionData{};///< User command interaction data.
        ComponentInteractionData componentData{};///< Component interaction data.
    };

    /// Interaction data. \brief Interaction data.
    struct DiscordCoreAPI_Dll InteractionData : public DiscordEntity {

        InteractionDataData data{};///< The interaction's data.
        string applicationId{ "" };///< The application's id.
        GuildMemberData member{};///< The data of the Guild member who sent the interaction, if applicable.
        string requesterId{ "" };///< The id of the sender of the interaction.
        InteractionType type{};///< The type of interaction.
        string channelId{ "" };///< The Channel the interaction was sent in.
        MessageData message{};///< The Message that the interaction came through on, if applicable.
        __int32 version{ 0 };///< The interaction version.
        string guildId{ "" };///< The Guild id of the Guild it was sent in.
        string token{ "" };///< The interaction token.
        UserData user{};///< The user data of the sender of the interaction.
        json rawData{};///< The interaction's raw data.

        InteractionData() {};

        InteractionData(string requesterId) {
            this->requesterId = requesterId;
        }
    };

    /**@}*/

    /**
    * \addtogroup voice_connection
    * @{
    */
    /// Represents a single frame of raw audio data. \brief Represents a single frame of raw audio data.
    struct DiscordCoreAPI_Dll RawFrameData {
        vector<unsigned __int8> data{};///< The audio data.
        __int32 sampleCount{ -1 };///< The number of samples per this frame.
    };

    /// Represents a single frame of encoded audio data. \brief Represents a single frame of encoded audio data.
    struct DiscordCoreAPI_Dll EncodedFrameData {
        vector<unsigned __int8> data{};///< The audio data.
        __int32 sampleCount{ -1 };///< The number of samples per this frame.
    };

    /// Audio frame type. \brief Audio frame type.
    enum class AudioFrameType {
        Unset = 0,///< Unset
        Encoded = 1,///< Encoded
        RawPCM = 2,///< Raw PCM
        Cancel = 3///< Cancel
    };

    /// Represents a single frame of audio data. \brief Represents a single frame of audio data.
    struct DiscordCoreAPI_Dll AudioFrameData {
        AudioFrameType type{ AudioFrameType::Unset };///< The type of audio frame.
        EncodedFrameData encodedFrameData{};///< To be filled if it's already encoded.
        RawFrameData rawFrameData{};///< To be filled if it's raw audio data.
    };
    /**@}*/

    enum class SongType {
        YouTube = 0,
        SoundCloud = 1
    };

    struct DiscordCoreAPI_Dll DownloadURL {
        __int32 contentSize{ 0 };
        string urlPath{ "" };        
    };

    /**
    * \addtogroup foundation_entities
    * @{
    */
    /// Input-event response type. \brief Input-event response type.
    enum class InputEventResponseType {
        Unset = 0,///< Unset
        DeferredResponse = 1,///<Deferred Response
        InteractionResponse = 2,///<Interaction Response
        InteractionResponseEdit = 3,///<Interaction Response Edit
        EphemeralInteractionResponse = 4,///<Ephemeral Interaction Response
        RegularMessage = 5,///< Regular Message
        RegularMessageEdit = 6,///< Regular Message Edit
        FollowUpMessage = 7,///< Follow Up Message
        FollowUpMessageEdit = 8,///< Follow Up Message Edit
        EphemeralFollowUpMessage = 9,///< Ephemeral Follow Up Message
        SendDM = 10///< Send Direct-Message
    };

    /// Input-event type. \brief Input-event type.
    enum class InputEventType {
        SLASH_COMMAND_INTERACTION = 1, ///< A chat-ApplicationCommand
        BUTTON_INTERACTION = 2, ///< A button interaction
        REGULAR_MESSAGE = 3, ///< A regular Message
        SELECT_MENU_INPUT = 4, ///< Select-menu input
        MESSAGE_COMMAND_INTERACTION = 5, ///<Message-command-interaction
        USER_COMMAND_INTERACTION = 6 ///<User-command-interaction.
    };

    struct DiscordCoreAPI_Dll GuildEmojisUpdateEventData {
        vector<EmojiData> emojis{};
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll GuildStickersUpdateEventData {
        vector<StickerData> stickers{};
        string guildId{ "" };
    };

    struct DiscordCoreAPI_Dll GuildMembersChunkEventData {
        vector<PresenceUpdateData> presences{};
        vector<GuildMemberData> members{};
        vector<string> notFound{};
        __int32 chunkIndex{ 0 };
        __int32 chunkCount{ 0 };
        string guildId{ "" };
        string nonce{ "" };
    };

    /// Data representing an input-event, which is any Message or Interaction that is coming into the bot as an input. \brief Data representing an input-event, which is any Message or Interaction that is coming into the bot as an input.
    class DiscordCoreAPI_Dll InputEventData {
    public:

        friend struct RecurseThroughMessagePagesData;
        friend struct OnInteractionCreationData;
        friend class RespondToInputEventData;
        friend class BaseFunctionArguments;
        friend class DiscordCoreClient;
        friend class EventHandler;
        friend struct CommandData;
        friend class InputEvents;

        InputEventResponseType inputEventResponseType{}; ///< The type of event response that is represented by this structure.
        InputEventType eventType{}; ///< The type of input-event that is represented by this structure.

        InputEventData() {};

        InputEventData(MessageData messageData, InteractionData interactionData, InputEventType eventType) {
            this->interactionData = interactionData;
            this->messageData = messageData;
            this->eventType = eventType;
            if (this->messageData.channelId == "") {
                this->messageData.channelId = this->interactionData.channelId;
            }
            else {
                this->interactionData.channelId = this->messageData.channelId;
            }
            if (this->interactionData.id == "") {
                this->interactionData.id = this->messageData.interaction.id;
            }
            if (this->messageData.guildId == "") {
                this->messageData.guildId = this->interactionData.guildId;
            }
            else {
                this->interactionData.guildId = this->messageData.guildId;
            }
            if (this->messageData.id == "") {
                this->messageData.id = this->interactionData.message.id;
            }
            else {
                this->interactionData.message.id = this->messageData.id;
            }
            if (this->messageData.member.user.id == "") {
                this->messageData.member = this->interactionData.member;
            }
            else {
                this->interactionData.member = this->messageData.member;
            }
            if (this->interactionData.user.id != "") {
                this->messageData.author = this->interactionData.user;
            }
            else {
                this->interactionData.user = this->messageData.author;
            }
            if (this->messageData.requesterId != "") {
                this->requesterId = this->messageData.requesterId;
            }
            else {
                this->requesterId = this->interactionData.requesterId;
            }
        }

        /// Returns the username of the last User to trigger this input-event. \brief Returns the username of the last User to trigger this input-event.
        /// \returns A string containing the User name.
        string getUserName() {
            if (this->messageData.author.userName == "" && this->interactionData.member.user.userName != "") {
                return this->interactionData.member.user.userName;
            }
            else if (this->interactionData.member.user.userName == "" && this->interactionData.user.userName != "") {
                return this->interactionData.user.userName;
            }
            else if (this->messageData.author.userName != "") {
                return this->messageData.author.userName;
            }
            else {
                return string();
            }
        }

        /// Gets the avatar URL of the last User to trigger this input-event. \brief Gets the avatar URL of the last User to trigger this input-event.
        /// \returns A string containing the avatar URL.
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
            else {
                return string();
            }
        }

        /// Returns the Message embeds that are on the Message, if applicable. \brief Returns the Message embeds that are on the Message, if applicable.
        /// \returns A vector containing the EmbedData.
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

        /// Returns the Message components that are on the Message, if applicable. \brief Returns the Message components that are on the Message, if applicable.
        /// \returns A vector containing ActionRowData.
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

        /// Returns the User id of the last requester of this input-event. \brief Returns the User id of the last requester of this input-event.
        /// \returns A string containing the author's id.
        string getAuthorId() {
            if (this->messageData.author.id == "") {
                return this->interactionData.user.id;
            }
            else if (this->messageData.author.id != "") {
                return this->messageData.author.id;
            }
            else {
                return string();
            }
        }

        /// Returns the interaction id, if appplicable, of this input-event. \brief Returns the interaction id, if appplicable, of this input-event.
        /// \returns A string containing the interaction id.
        string getInteractionId() {
            if (this->interactionData.id == "") {
                return this->messageData.interaction.id;
            }
            else if (this->interactionData.id != "") {
                return this->interactionData.id;
            }
            else if (this->interactionData.message.interaction.id != "") {
                return this->interactionData.message.interaction.id;
            }
            else {
                return string();
            }
        }

        /// Returns the application id. \brief Returns the application id.
        /// \returns A string containing the application id.
        string getApplicationId() {
            if (this->interactionData.applicationId == "") {
                return this->messageData.application.id;
            }
            else if (this->interactionData.applicationId != "") {
                return this->interactionData.applicationId;
            }
            else {
                return string();
            }
        }

        /// Returns the Channel id of this input-event. \brief Returns the Channel id of this input-event.
        /// \returns A string containing the Channel id.
        string getChannelId() {
            if (this->interactionData.channelId == "") {
                return this->messageData.channelId;
            }
            else if (this->interactionData.channelId != "") {
                return this->interactionData.channelId;
            }
            else {
                return string();
            }
        }

        /// Returns the interaction token, if applicable, of this input-event. \brief Returns the interaction token, if applicable, of this input-event.
        /// \returns A string containing the interaction token.
        string getInteractionToken() {
            if (this->interactionData.token != "") {
                return this->interactionData.token;
            }
            else {
                return string();
            }
        }

        /// Returns the Guild id, of this input-event. \brief Returns the Guild id, of this input-event.
        /// \returns A string containing the Guild id.
        string getGuildId() {
            if (this->messageData.guildId == "") {
                return this->interactionData.guildId;
            }
            else if (this->messageData.guildId != "") {
                return this->messageData.guildId;
            }
            else {
                return string();
            }
        }

        /// Returns the Message content, if applicable, of this input-event. \brief Returns the Message content, if applicable, of this input-event.
        /// \returns A string containing the Message's content.
        string getMessageContent() {
            if (this->interactionData.message.content != "") {
                return this->interactionData.message.content;
            }
            else {
                return this->messageData.content;
            }
        }

        /// Returns the Message id, if applicable, of this input-event. \brief Returns the Message id, if applicable, of this input-event.
        /// \returns A string containing the Message id.
        string getMessageId() {
            if (this->messageData.id != "") {
                return this->messageData.id;

            }
            else if (this->interactionData.message.id != "") {
                return this->interactionData.message.id;
            }
            else {
                return string();
            }
        }

        /// Returns the interaction data, if applicable, of this input-event. \brief Returns the InteractionData, if applicable, of this input-event.
        /// \returns An InteractionData structure.
        InteractionData getInteractionData() {
            return this->interactionData;
        }

        /// Returns the Message data, if applicable, of this input-event. \brief Returns the Message data, if applicable, of this input-event.
        /// \returns A MessageData structure.
        MessageData getMessageData() {
            if (this->messageData.id != "") {
                return this->messageData;

            }
            else {
                return this->interactionData.message;
            }
        }

        /// Returns the requester id, of this input-event. \brief Returns the requester id, of this input-event.
        /// \returns A string containing the requester's id.
        string getRequesterId() {
            return this->requesterId;
        };

    protected:
        InteractionData interactionData{};
        MessageData messageData{};
        string requesterId{ "" };
    };

    /// \brief Data for responding to an input-event.
    class DiscordCoreAPI_Dll RespondToInputEventData {
    public:
        friend struct CreateEphemeralInteractionResponseData;
        friend struct CreateDeferredInteractionResponseData;
        friend struct CreateEphemeralFollowUpMessageData;
        friend struct CreateInteractionResponseData;
        friend struct EditInteractionResponseData;
        friend struct DeferComponentResponseData;
        friend struct CreateFollowUpMessageData;
        friend struct EditFollowUpMessageData;
        friend struct SendDMData;
        friend class InputEvents;

        InputEventResponseType type{}; ///< The type of response to make.

        RespondToInputEventData(string channelIdNew) {
            this->channelId = channelIdNew;
            this->type = InputEventResponseType::RegularMessage;
        }

        RespondToInputEventData(InteractionData dataPackage) {
            this->type = InputEventResponseType::DeferredResponse;
            this->applicationId = dataPackage.applicationId;
            this->interactionToken = dataPackage.token;
            this->messageId = dataPackage.message.id;
            this->channelId = dataPackage.channelId;
            this->interactionId = dataPackage.id;
        };

        RespondToInputEventData(InputEventData dataPackage) {
            this->interactionToken = dataPackage.getInteractionToken();
            this->applicationId = dataPackage.getApplicationId();
            this->interactionId = dataPackage.getInteractionId();
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

        /// For setting the direct-Message User target of a response. \brief For setting the direct-Message User target of a response.
        /// \param targetUserIdNew A string, containging the target User's id.
        /// \returns void
        void setTargetUserID(string targetUserIdNew) {
            this->targetUserId = targetUserIdNew;
        }

    protected:

        AllowedMentionsData allowedMentions{};
        vector<ActionRowData> components{};
        string interactionToken{ "" };
        vector<EmbedData> embeds{};
        string interactionId{ "" };
        string applicationId{ "" };
        string targetUserId{ "" };
        string requesterId{ "" };
        string channelId{ "" };
        string messageId{ "" };
        string content{ "" };
        __int32 flags{ 0 };
        bool tts{ false };
    };

    struct DiscordCoreAPI_Dll CommandData {
    public:

        vector<string> optionsArgs{};
        InputEventData eventData{};
        string commandName{ "" };

        CommandData(InputEventData inputEventData) {
            this->eventData = inputEventData;
            if (inputEventData.interactionData.data.applicationCommanddata.name != "") {
                this->commandName = inputEventData.interactionData.data.applicationCommanddata.name;
            }
            if (inputEventData.interactionData.data.messageInteractionData.targetId != "") {
                this->optionsArgs.push_back(inputEventData.interactionData.data.messageInteractionData.targetId);
            }
            else if (inputEventData.interactionData.data.userInteractionData.targetId != "") {
                this->optionsArgs.push_back(inputEventData.interactionData.data.userInteractionData.targetId);
            }
        }
    };
    /**@}*/
    
    /**
    * \addtogroup voice_connection
    * @{
    */
    /// A song from the various platforms. \brief A song from the various platforms.
    struct DiscordCoreAPI_Dll Song {
    public:

        friend vector<Song> cleanQueue(vector<Song> originalQueue);
        friend class DiscordCoreInternal::DataParser;
        friend class DatabaseManagerAgent;
        friend class SoundCloudSong;
        friend class SoundCloudAPI;
        friend class YouTubeSong;
        friend class YouTubeAPI;
        friend class SongAPI;

        SongType type{ SongType::SoundCloud };///< The type of song.
        string addedByUserName{ "" };///< The user name of the individual who added this Song to the playlist.
        string addedByUserId{ "" };///< The user id of the individual who added this Song to the playlist.
        string thumbnailURL{ "" };///< The Url of the thumbnail image of this Song.
        string description{ "" };///< A description of the Song.
        string songTitle{ "" };///< The title of the Song.
        string duration{ "" };///< The duration of the Song.
        string viewURL{ "" };///< The url for listening to this Song through a browser.

        Song() {}

    protected:

        operator SoundCloudSong();

        operator YouTubeSong();

        vector<DownloadURL> finalDownloadURLs{};
        string secondDownloadURL{ "" };
        string firstDownloadURL{ "" };
        __int32 contentLength{ 0 };
        string songId{ "" };

    };

    /// Song completion event data. \brief Song completion event data.
    struct DiscordCoreAPI_Dll SongCompletionEventData {
        VoiceConnection* voiceConnection; ///< A pointer to the current VoiceConnection.
        bool isThisAReplay{ false };///< Is this a replay? (Did a track recently fail to play?)
        Song previousSong{};///< The previously played Song.
    };

    /**@}*/

    /**
    * \addtogroup foundation_entities
    * @{
    */
    /// Permissions. \brief Permissions.
    enum class Permissions :__int64 {
        CREATE_INSTANT_INVITE = (1 << 0),///< Create Instant Invite
        KICK_MEMBERS = (1 << 1),///< Kick Members
        BAN_MEMBERS = (1 << 2),///< Ban Members
        ADMINISTRATOR = (1 << 3),///< Administrator
        MANAGE_CHANNELS = (1 << 4),///< Manage Channels
        MANAGE_GUILD = (1 << 5),///< Manage Guild
        ADD_REACTIONS = (1 << 6),///< Add Reactions
        VIEW_AUDIT_LOG = (1 << 6),///< View Audit Log
        PRIORITY_SPEAKER = (1 << 8),///< Priority Speaker
        STREAM = (1 << 9),///< Stream
        VIEW_CHANNEL = (1 << 10),///< View Channel
        SEND_MESSAGES = (1 << 11),///< Send Messages
        SEND_TTS_MESSAGES = (1 << 12),///< Send TTS Messages
        MANAGE_MESSAGES = (1 << 13),///< Manage Messages
        EMBED_LINKS = (1 << 14),///< Embed Links
        ATTACH_FILES = (1 << 15),///< Attach Files
        READ_MESSAGE_HISTORY = (1 << 16),///< Read Message History
        MENTION_EVERYONE = (1 << 17),///< Mention Everyone
        USE_EXTERNAL_EMOJIS = (1 << 18),///< Use External Emojis
        VIEW_GUILD_INSIGHTS = (1 << 19),///< View Guild Insights
        CONNECT = (1 << 20),///< Connect
        SPEAK = (1 << 21),///< Speak
        MUTE_MEMBERS = (1 << 22),///< Mute Members
        DEAFEN_MEMBERS = (1 << 23),///< Deafen Members
        MOVE_MEMBERS = (1 << 24),///< Move Members
        USE_VAD = (1 << 25),///< Use VAD
        CHANGE_NICKNAME = (1 << 26),///< Change Nickname
        MANAGE_NICKNAMES = (1 << 27),///< Manage Nicknames
        MANAGE_ROLES = (1 << 28),///< Manage Roles
        MANAGE_WEBHOOKS = (1 << 29),///< Manage Webhooks
        MANAGE_EMOJIS = (1 << 30),///< Manage Emojis
        USE_SLASH_COMMANDS = (1ull << 31),///< Use Slash Commands
        REQUEST_TO_SPEAK = (1ull << 32),///< Request To Speak
        MANAGE_THREADS = (1ull << 33),///< Manage Threads
        USE_PUBLIC_THREADS = (1ull << 34),///< Use Public Threads
        USE_PRIVATE_THREADS = (1ull << 35),///< Use Private Threads
        USE_EXTERNAL_STICKERS = (1ull << 37)///< Use External Stickers
    };
    /**@}*/

    class DiscordCoreAPI_Dll YouTubeSong : public Song {
    public:

        friend class DiscordCoreInternal::DataParser;
        friend class SoundCloudAPI;
        friend class YouTubeAPI;
        friend class SongAPI;
        friend struct Song;

        operator Song();

        YouTubeSong collectFinalSong(GuildMemberData addedByGuildMember, YouTubeSong newSong);

        vector<YouTubeSong> searchForSong(string songQuery);

    protected:

        string baseSearchURL{ "https://www.youtube.com/results?search_query=" };
        string baseWatchURL{ "https://www.youtube.com/watch?v=" };
        string baseURL{ "https://www.youtube.com" };

        vector<YouTubeFormat> formats{};
        string html5PlayerFile{ "" };
        string playerResponse{ "" };
        string html5Player{ "" };

        YouTubeSong();

        YouTubeSong collectDownloadInfo(GuildMemberData guildMember, YouTubeSong newSong);

    };

    class DiscordCoreAPI_Dll SoundCloudSong : public Song {
    public:

        friend class DiscordCoreInternal::DataParser;
        friend class SoundCloudAPI;
        friend class SongAPI;
        friend struct Song;

        operator Song();

        static void initialize();

        SoundCloudSong collectFinalSong(GuildMemberData addedByGuildMember, SoundCloudSong newSong);

        vector<SoundCloudSong> searchForSong(string songQuery);

    protected:

        static string clientId;

        string baseSearchURL02{ "https://api-v2.soundcloud.com/search?q=" };
        string baseSearchURL{ "https://soundcloud.com/search?q=" };
        string appVersion{ "1631696495" };

        HttpClient httpClientForGettingSecondURL{ HttpClient() };
        HttpClient httpClientForGettingFinalURLs{ HttpClient() };
        HttpRequestHeaderCollection headerCollectionForSecondURL{ httpClientForGettingSecondURL.DefaultRequestHeaders() };
        HttpRequestHeaderCollection headerCollectionForFinalURLs{ httpClientForGettingSecondURL.DefaultRequestHeaders() };
        string trackAuthorization{ "" };

        SoundCloudSong();

        string collectClientId(string searchQuery);

        SoundCloudSong findSecondDownloadURL(SoundCloudSong newSong);

        SoundCloudSong findFinalDownloadURLs(SoundCloudSong newSong);
    };

    struct DiscordCoreAPI_Dll DBPlaylist;

    /**
    * \addtogroup voice_connection
    * @{
    */
    /// Playlist of songs and other variables. \brief Playlist of songs and other variables.
    struct DiscordCoreAPI_Dll Playlist {
    public:
        operator DBPlaylist();
        bool isLoopSongEnabled{ false };///< Is looping of Songs currently enabled?
        bool isLoopAllEnabled{ false };///< Is looping of the entire Playlist currently enabled?
        vector<Song> songQueue{};///< The list of Songs that are stored to be played.
        Song currentSong{};///< The current Song that is playing.
    };

    struct DiscordCoreAPI_Dll DBPlaylist {
    public:
        operator Playlist();
        bool isLoopSongEnabled{ false };///< Is looping of Songs currently enabled?
        bool isLoopAllEnabled{ false };///< Is looping of the entire Playlist currently enabled?
        vector<Song> songList{};///< The list of Songs that are stored to be played.
        Song currentSong{};///< The current Song that is playing.
    };
    /**@}*/

    /**
    * \addtogroup utilities
    * @{
    */
    /// PermissionsConverter class, for manipulating permission values. \brief PermissionsConverter class, for manipulating permission values.
    class DiscordCoreAPI_Dll PermissionsConverter {
    public:

        /// Checks a given string for a particular permission's presence. \brief Checks a given string for a particular permission's presence.
        /// \param permission Which permission to check for.
        /// \param permissionString The string within which to check for the permission.
        /// \returns A bool answering about whether the permission is present.
        static bool checkForPresence(Permissions permission, string permissionString);

        /// Adds one or more permissions to a string value. \brief Adds one or more permissions to a string value.
        /// \param originalPermissionString The original permission string to modify.
        /// \param permissionsToAdd A vector containing the permissions you wish to add.
        /// \returns A string containing the new permissions value.
        static string addPermissionsToString(string originalPermissionString, vector<Permissions> permissionsToAdd);

        /// Removes one or more permissions from a string value. \brief Removes one or more permissions from a string value.
        /// \param originalPermissionString The original permission string to modify.
        /// \param permissionsToRemove A vector containing the permissions you wish to remove.
        /// \returns A string containing the new permissions value.
        static string removePermissionsFromString(string originalPermissionString, vector<Permissions> permissionsToRemove);

        /// Displays the currently present permissions in a string, and returns a vector with each of them stored in string format. \brief Displays the currently present permissions in a string, and returns a vector with each of them stored in string format.
        /// \param permissionString The string to check for permissions.
        /// \returns A vector full of strings of the permissions that are in the input string's value.
        static vector<string> displayPermissions(string permissionString);

        /// Returns a string containing ALL of the possible permissions. \brief Returns a string containing ALL of the possible permissions.
        /// \returns A string containing all of the possible permissions.
        static string getAllPermissions();

        /// Returns a string containing all of a given user's permissions for a given channel. \brief Returns a string containing all of a given user's permissions for a given channel.
        /// \param guildMember The GuildMember who's permissions to analyze.
        /// \param channel The channel withint which to check for permissions.
        /// \returns A string containing the final permission's value for a given channel.
        static string getAllOfMyPerrmissions(GuildMemberData guildMember, ChannelData channel);

        /// Checks for a given permission in a chosen channel, for a specific user. \brief Checks for a given permission in a chosen channel, for a specific user.
        /// \param guildMember The GuildMember who to check the permissions of.
        /// \param channel The Channel within which to check for the permission's presence.
        /// \param permission A Permission to check the current Channel for.
        /// \returns A bool suggesting the presence of the chosen permission.
        static bool checkForPermission(GuildMemberData guildMember, ChannelData channel, Permissions permission);

    protected:
        static string computeBasePermissions(GuildMemberData guildMember);

        static string  computeOverwrites(string basePermissions, GuildMemberData guildMember, ChannelData channel);

        static string computePermissions(GuildMemberData guildMember, ChannelData channel);
    };

    /// Returns from an execution of recurseThroughMessagePages(). \brief Returns from an execution of recurseThroughMessagePages().
    struct DiscordCoreAPI_Dll RecurseThroughMessagePagesData {
        InputEventData inputEventData;///< The latest InputEventData that was returned by updating the message.
        unsigned int currentPageIndex;//< The current page index, within the number of submitted pages to turn through.
        string buttonId;///< The id of the last button that was pressed.
    };

    /// Recurses through a succession of messages. \brief Recurses through a succession of messages.
    /// \param userID A user's id to check for the responses of to the buttons.
    /// \param originalEvent The original InputEventData structure.
    /// \param currentPageIndex The current page within the stack of pages to recurse through.
    /// \param messageEmbeds A vector full of EmbedData to recurse through.
    /// \param deleteAfter A bool determining whether or not the message will be deleted upon timeout/exiting.
    /// \param waitForMaxMs Maximum number of milliseconds to wait for a button response.
    /// \returns A RecurseThroughMessagePageData structure.
    DiscordCoreAPI_Dll RecurseThroughMessagePagesData recurseThroughMessagePages(string userID, InputEventData originalEvent, unsigned int currentPageIndex, vector<EmbedData> messageEmbeds, bool deleteAfter, unsigned int waitForMaxMs);
    /**@}*/
};
#endif
 