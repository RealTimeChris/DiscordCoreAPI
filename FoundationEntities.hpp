// FoundationEntities.hpp - Header for all of the Discord/Support API data structures.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _FOUNDATION_ENTITIES_
#define _FOUNDATION_ENTITIES_

#include "../pch.h"

namespace DiscordCoreAPI {

    struct RespondToInputEventData;
    class DiscordCoreClientBase;
    class DatabaseManagerAgent;
    class PermissionsConverter;
    class ApplicationCommands;    
    class DiscordCoreClient;
    class VoiceConnection;
    class Interactions;
    class EventHandler;
    class GuildMembers;
    class InputEvents;
    class SelectMenu;
    class Reactions;
    class Messages;
    class Channels;
    class Button;
    class Guilds;
    class Roles;
    class Users;
    class Guild;

    bool nanosleep(LONGLONG ns) {
        HANDLE timer = CreateWaitableTimerExW(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
        LARGE_INTEGER largeInt{ .QuadPart = -ns / 100 };
        if (!timer) {
            return FALSE;
        }
        
        if (!SetWaitableTimerEx(timer, &largeInt, 0, NULL, NULL, NULL, 0)) {
            CloseHandle(timer);
            cout << GetLastError() << endl;
            return FALSE;
        }
        WaitForSingleObjectEx(timer, INFINITE, false);
        CloseHandle(timer);
        return TRUE;
    }

    void spinLock(long long timeInNsToSpinLockFor){
        long long startTime = chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        long long timePassed{ 0 };
        while (timePassed < timeInNsToSpinLockFor) {
            timePassed = chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count() - startTime;
        }
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

        unsigned long long maxNumberOfMs{ 0 };
        unsigned long long startTime{ 0 };
    };

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

    void saveFile(hstring filePath, hstring fileName, IBuffer readBuffer) {
        auto folder = winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(filePath).get();
        winrt::Windows::Storage::StorageFile storageFile = folder.CreateFileAsync(fileName, CreationCollisionOption::ReplaceExisting).get();
        winrt::Windows::Storage::FileIO::WriteBufferAsync(storageFile, readBuffer).get();
    }

    IBuffer loadFile(hstring filePath, hstring fileName) {
        auto folder = winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(filePath).get();
        winrt::Windows::Storage::StorageFile storageFile = folder.GetFileAsync(fileName).get();
        return winrt::Windows::Storage::FileIO::ReadBufferAsync(storageFile).get();
    }

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

    string convertSnowFlakeToDateTimeString(string snowFlake) {
        string returnString;
        __int64 timeInMs = (stoll(snowFlake) >> 22) + 1420070400000;
        returnString = convertTimeInMsToDateTimeString(timeInMs);
        return returnString;
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

};

namespace  DiscordCoreInternal {
    
    class ApplicationCommandManager;
    class WebSocketConnectionAgent;
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

    struct AllowedMentionsData {
        bool repliedUser{ false };
        vector<string> parse{};
        vector<string> roles{};
        vector<string> users{};
    };

    struct MessageReferenceData {
        bool failIfNotExists{ false };
        string messageId{ "" };
        string channelId{ "" };
        string guildId{ "" };
    };

    struct AttachmentData {
        string contentType{ "" };
        string filename{ "" };
        string proxyUrl{ "" };
        string url{ "" };
        string id{ "" };
        int height{ 0 };
        int width{ 0 };
        int size{ 0 };
    };

    struct EmbedFooterData {
        string proxyIconUrl{ "" };
        string iconUrl{ "" };
        string text{ "" };
    };

    struct EmbedImageData {
        string proxyUrl{ "" };
        string url{ "" };
        int height{ 0 };
        int width{ 0 };
    };

    struct EmbedThumbnailData {
        string proxyUrl{ "" };
        string url{ "" };
        int height{ 0 };
        int width{ 0 };
    };

    struct EmbedVideoData {
        string proxyUrl{ "" };
        string url{ "" };
        int height{ 0 };
        int width{ 0 };
    };

    struct EmbedProviderData {
        string name{ "" };
        string url{ "" };
    };

    struct EmbedAuthorData {
        string proxyIconUrl{ "" };
        string proxyUrl{ "" };
        string iconUrl{ "" };
        string name{ "" };
        string url{ "" };
    };

    struct EmbedFieldData {
        string iconUrl{ "" };
        bool Inline{ false };
        string value{ "" };
        string name{ "" };
    };

    struct EmbedData {

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
        GUILD_PRIVATE_THREAD = 12,
        GUILD_STAGE_VOICE = 13
    };

    struct UserData {
        string discriminator{ "" };
        bool mfaEnabled{ false };
        bool verified{ false };
        string createdAt{ "" };
        string userName{ "" };
        bool system{ false };
        int premiumType{ 0 };
        int publicFlags{ 0 };
        string avatar{ "" };
        string locale{ "" };
        string email{ "" };
        bool bot{ false };
        string id{ "" };
        int flags{ 0 };
    };

    struct RoleTagsData {
        string premiumSubscriber{ "" };
        string integrationId{ "" };
        string botId{ "" };
    };

    struct RoleData {
        bool mentionable{ false };
        bool managed{ false };
        bool hoist{ false };
        RoleTagsData tags{};
        string permissions;
        string name{ "" };
        int position{ 0 };
        string id{ "" };
        int color{ 0 };
    };

    struct GuildMemberData {
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

    struct WelcomeScreenChannelData {
        string description{ "" };
        string channelId{ "" };
        string emojiName{ "" };
        string emojiId{ "" };
    };

    struct WelcomeScreenData {
        vector<WelcomeScreenChannelData> welcomeChannels{};
        string description{};
    };

    struct PartyData {
        vector<int> size{ 0, 0 };
        string id{ "" };
    };

    struct AssetsData {
        string largeImage{ "" };
        string smallImage{ "" };
        string largeText{ "" };
        string smallText{ "" };
    };

    struct SecretsData {
        string spectate{ "" };
        string match{ "" };
        string join{ "" };
    };

    struct TimestampData {
        __int64 start{ 0 };
        __int64 end{ 0 };
    };

    struct ButtonData {
        string label{ "" };
        string url{ "" };
    };

    struct EmojiData {
        bool requireColons{ false };
        vector<RoleData> roles{};
        bool available{ false };
        bool animated{ false };
        bool managed{ false };
        string name{ "" };
        UserData user{};
        string id{ "" };
    };

    struct ReactionData {
        GuildMemberData member{};
        string channelId{ "" };
        string messageId{ "" };
        string guildId{ "" };
        string userId{ "" };
        EmojiData emoji{};
        bool me{ false };
        int count{ 0 };
    };

    struct ActivityData {
        TimestampData timestamps{};
        string applicationId{ "" };
        bool instance{ false };
        SecretsData secrets{};
        string details{ "" };
        ButtonData buttons{};
        AssetsData assets{};
        string state{ "" };
        int createdAt = 0;
        EmojiData emoji{};
        PartyData party{};
        string name{ "" };
        string url{ "" };
        int flags{ 0 };
        int type{ 0 };
    };

    struct ClientStatusData {
        string desktop{ "" };
        string mobile{ "" };
        string web{ "" };
    };

    struct UpdatePresenceData {
        vector<ActivityData> activities{};
        string status{ "" };
        __int64 since{ 0 };
        bool afk{ false };
    };

    struct PresenceUpdateData {
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

    struct OverWriteData {
        EditChannelPermissionOverwritesType type{};
        string channelId{ "" };
        string allow{ "" };
        string deny{ "" };
        string id{ "" };
    };

    struct ThreadMetadataData {
        string archiveTimestamp{ "" };
        int autoArchiveDuration{ 0 };
        string archiverId{ "" };
        bool archived{ false };
        bool locked{ false };
    };

    struct ThreadMemberData {
        string joinTimestamp{ "" };
        string userId{ "" };
        string id{ "" };
        int flags{ 0 };
    };

    struct ChannelData {
        map<string, OverWriteData> permissionOverwrites{};
        ThreadMetadataData threadMetadata{};
        vector<UserData> recipients{};
        string lastPinTimestamp{ "" };
        string lastMessageId{ "" };
        int rateLimitPerUser{ -1 };        
        string applicationId{ "" };
        int videoQualityMode{ -1 };
        ThreadMemberData member{};
        string rtcRegion{ "" };
        string parentId{ "" };
        string guildId{ "" };
        string ownerId{ "" };
        int userLimit{ -1 };
        string topic{ "" };
        bool nsfw{ false };
        ChannelType type{};
        int position{ -1 };
        string name{ "" };
        int bitrate{ -1 };
        string icon{ "" };
        string id{ "" };
    };

    struct VoiceStateData {
        string requestToSpeakTimestamp{ "" };
        GuildMemberData guildMember{};
        bool selfStream{ false };
        string channelId{ "" };
        string sessionId{ "" };
        bool selfDeaf{ false };
        bool selfMute{ false };
        bool selfVideo{ false };
        bool suppress{ false };
        string guildId{ "" };
        string userId{ "" };
        bool deaf{ false };
        bool mute{ false };
    };

    struct StageInstanceData {
        bool discoverableDisabled{ false };
        string channelId{ "" };
        int privacyLevel{ 0 };
        string guildId{ "" };
        string topic{ "" };
        string id{ "" };
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

    struct StickerData {
        StickerFormatType formatType{};
        string description{ "" };
        bool available{ false };
        string guildId{ "" };
        string packId{ "" };
        string asset{ "" };
        StickerType type{};
        int sortValue{ 0 };
        string name{ "" };
        string tags{ "" };
        string id{ "" };
        UserData user{};
    };

    struct GuildData {
        shared_ptr<DiscordCoreAPI::DiscordCoreClientBase> discordCoreClientBase{ nullptr };
        shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
        DefaultMessageNotificationLevel defaultMessageNotifications{};        
        ExplicitContentFilterLevel explicitContentFilter{};
        vector<StageInstanceData> stageInstances{};
        vector<PresenceUpdateData> presences{};
        VerificationLevel verificationLevel{};
        vector<VoiceStateData> voiceStates{};
        string publicUpdatesChannelId{ "" };
        vector<GuildMemberData> members{};
        WelcomeScreenData welcomeScreen{};
        int premiumSubscriptionCount{ 0 };
        int approximatePresenceCount{ 0 };
        int approximateMemberCount{ 0 };
        vector<ChannelData> channels{};
        vector<StickerData> stickers{};
        vector<ChannelData> threads{};
        int maxVideoChannelUsers{ 0 };
        string discoverySplash{ "" };
        string preferredLocale{ "" };
        string widgetChannelId{ "" };
        string systemChannelId{ "" };
        string rulesChannelId{ "" };
        bool widgetEnabled{ false };
        int systemChannelFlags{ 0 };
        string vanityURLCode{ "" };
        string applicationId{ "" };
        vector<EmojiData> emoji{};
        string description{ "" };
        string afkChannelId{ "" };
        vector<string> features{};
        bool unavailable{ false };        
        PremiumTier premiumTier{};
        string permissions{ "" };
        vector<RoleData> roles{};
        string createdAt{ "" };
        string iconHash{ "" };
        int maxPresences{ 0 };
        string joinedAt{ "" };
        string ownerId{ "" };
        int memberCount{ 0 };
        string region{ "" };
        bool owner{ false };
        int afkTimeOut{ 0 };
        MFALevel mfaLevel{};
        bool large{ false };
        int maxMembers{ 0 };
        string splash{ "" };
        string banner{ "" };
        int nsfwLevel{ 0 };
        string icon{ "" };
        string name{ "" };
        string id{ "" };
    };

    struct ScheduleGroupWrapper {
        ScheduleGroupWrapper(ScheduleGroup* dataPackage) {
            this->scheduleGroup = dataPackage;

        }
        ScheduleGroup* scheduleGroup{ nullptr };
    };

    struct ScheduleWrapper {
        ScheduleWrapper(Scheduler* dataPackage) {
            this->scheduler = dataPackage;
        }
        Scheduler* scheduler{ nullptr };
    };

    struct ThreadContext {

        ThreadContext() {};

        ThreadContext(ThreadContext* threadContext) {
            this->dispatcherQueue = threadContext->dispatcherQueue;
            this->scheduleGroup = threadContext->scheduleGroup;
            this->scheduler = threadContext->scheduler;
        }

        void releaseGroup() {
            if (this->scheduleGroup != nullptr) {
                this->scheduleGroup->scheduleGroup->Release();
                this->scheduleGroup = nullptr;
            }
        };

        ~ThreadContext() {
            if (this->scheduleGroup != nullptr) {
                this->scheduleGroup->scheduleGroup->Release();
                this->scheduleGroup = nullptr;
            }
        }

        shared_ptr<ScheduleGroupWrapper> scheduleGroup{ nullptr };
        shared_ptr<DispatcherQueue> dispatcherQueue{ nullptr };
        shared_ptr<ScheduleWrapper> scheduler{ nullptr };        
    };

    struct HttpAgentResources {
        hstring userAgent{ L"" };
        string baseURL{ "" };       
    };

    struct DeleteInteractionResponseData {
        HttpAgentResources agentResources{};
        unsigned int timeDelayInMs{ 0 };
        string interactionToken{ "" };
        string applicationId{ "" };        
    };

    struct DeleteFollowUpMessageData {
        HttpAgentResources agentResources{};
        unsigned int timeDelayInMs{ 0 };
        string interactionToken{ "" };
        string applicationId{ "" };
        string messageId{ "" };
    };

    struct ChannelMentionData {
        string guildId{ "" };
        string name{ "" };
        string id{ "" };
        int type{ -1 };
    };

    struct MessageActivityData {
        string partyId{ "" };
        int type{ -1 };
    };

    struct TeamMembersObjectData {
        vector<string> permissions{};
        int membershipState{ -1 };
        string teamId{ "" };
        UserData user{};
    };

    struct TeamObjectData {
        vector<TeamMembersObjectData> members{};
        string ownerUserId{ "" };
        string icon{ "" };
        string id{ "" };
    };

    struct ApplicationData {
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
        string slug{ "" };
        string name{ "" };
        string icon{ "" };
        UserData owner{};
        string id{ "" };
        int flags{ 0 };
    };

    struct MessageStickerData {
        string description{ "" };
        bool available{ false };
        string guildId{ "" };
        string packId{ "" };
        int formatType{ 0 };
        int sortValue{ 0 };
        string asset{ "" };
        string name{ "" };
        string tags{ "" };
        string id{ "" };
        UserData user{};
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
        string description{ "" };
        bool _default{ false };
        string label{ "" };
        string value{ "" };
        EmojiData emoji{};
    };

    struct ComponentData {
        vector<SelectOptionData> options{};
        string placeholder{ "" };
        bool disabled{ false };
        string customId{ "" };
        ComponentType type{};
        ButtonStyle style{};
        string label{ "" };
        int maxValues{ 0 };
        int minValues{ 0 };
        EmojiData emoji{};
        string url{ "" };
    };

    struct ActionRowData {
        vector<ComponentData> components{};
    };

    struct InteractionApplicationCommandCallbackData {
        AllowedMentionsData allowedMentions{};
        vector<ActionRowData> components{};
        vector<EmbedData> embeds{};
        string content{ "" };
        bool tts{ false };
        int flags{ 0 };
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
        DELETE_LEAVE_GUILD = 54,
        SOUNDCLOUD_SEARCH = 55,
        GET_APPLICATION_COMMAND = 56,
        PUT_BULK_OVERWRITE_APPLICATION_COMMANDS = 57,
        POST_GUILD_APPLICATION_COMMAND = 58,
        GET_GUILD_APPLICATION_COMMANDS = 59,
        GET_GUILD_APPLICATION_COMMAND = 60
    };

    enum class MessageStickerItemType {
        PNG = 1,
        APNG = 2,
        LOTTIE = 3
    };

    struct MessageStickerItemData {
        MessageStickerItemType formatType{};
        string name{ "" };
        string id{ "" };
    };

    enum class InteractionType {
        Ping = 1,
        ApplicationCommand = 2,
        MessageComponent = 3
    };

    struct MessageInteractionData {
        InteractionType type{};
        string name{ "" };
        UserData user{};
        string id{ "" };
    };

    struct MessageDataOld {
        vector<MessageStickerItemData> stickerItems{};
        vector<ChannelMentionData> mentionChannels{};
        MessageReferenceData messageReference{};
        vector<MessageStickerData> stickers{};
        MessageInteractionData interaction{};
        vector<AttachmentData> attachments{};
        vector<ActionRowData> components{};
        vector<ReactionData> reactions{};
        MessageActivityData activity{};
        bool mentionEveryone{ false };
        vector<string> mentionRoles{};
        ApplicationData application{};
        string editedTimestamp{ "" };
        vector<UserData> mentions{};
        vector<EmbedData> embeds{};
        string applicationId{ "" };
        string timestampRaw{ "" };
        GuildMemberData member{};
        string webhookId{ "" };
        string channelId{ "" };
        string timestamp{ "" };
        ChannelData thread{};
        bool pinned{ false };
        string guildId{ "" };
        string content{ "" };
        MessageType type{};
        string nonce{ "" };
        UserData author{};
        bool tts{ false };
        string id{ "" };
        int flags{ 0 };
    };

    struct MessageData : MessageDataOld {
        MessageDataOld referencedMessage{};
        string requesterId{ "" };
    };

    struct ApplicationCommandInteractionDataResolved {
        map<string, GuildMemberData> members{};
        map<string, ChannelData> channels{};
        map<string, UserData> users{};       
        map<string, RoleData> roles{};        
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

    struct ApplicationCommandInteractionDataOption {
        vector<ApplicationCommandInteractionDataOption> options{};
        ApplicationCommandOptionType type{};
        string valueString{ "" };
        bool valueBool{ false };
        int valueInt{ false };
        string name{ "" };
    };

    struct ApplicationCommandInteractionData {
        vector<ApplicationCommandInteractionDataOption> options{};
        ApplicationCommandInteractionDataResolved resolved{};
        int componentType{ -1 };
        string customId{ "" };
        string name{ "" };
        string id{ "" };
    };

    enum class ApplicationCommandPermissionType {
        Role = 1,
        User = 2
    };

    struct ApplicationCommandPermissionData {
        ApplicationCommandPermissionType type;
        bool permission{ false };
        string id{ "" };
    };

    struct GuildApplicationCommandPermissionData {
        vector<ApplicationCommandPermissionData> permissions{};
        string applicationId{ "" };
        string guildId{ "" };
        string id{ "" };
    };

    struct InteractionData {
        ApplicationCommandInteractionData data{};
        ComponentType componentType{};
        string applicationId{ "" };
        GuildMemberData member{};
        string requesterId{ "" };
        vector<string> values{};
        InteractionType type{};
        string channelId{ "" };
        MessageData message{};
        string customId{ "" };
        string guildId{ "" };
        string token{ "" };
        int version{ -1 };
        string id{ "" };
        UserData user{};
        json dataRaw{};
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

    enum class ApplicationCommandType {
        CHAT_INPUT = 1,
        USER = 2,
        MESSAGE = 3
    };

    struct UserCommandInteractionData {
        ApplicationCommandType type{};
        string applicationId{ "" };
        string interactionId{ "" };
        GuildMemberData members{};
        GuildMemberData member{};
        string channelId{ "" };
        string targetId{ "" };
        string guildId{ "" };
        string menuId{ "" };
        string token{ "" };
        string name{ "" };
        UserData users{};
        int version{ 0 };
    };

    struct MessageCommandInteractionData {
        ApplicationCommandType type{};
        string interactionId{ "" };
        string applicationId{ "" };
        GuildMemberData member{};
        string channelId{ "" };
        MessageData messages{};
        string targetId{ "" };
        string guildId{ "" };
        string menuId{ "" };
        string token{ "" };
        string name{ "" };
        int version{ 0 };
    };

    struct GetApplicationData {
        HttpAgentResources agentResources{};
    };

    struct InteractionResponseData {
        InteractionApplicationCommandCallbackData data{};
        InteractionCallbackType type{};
    };

    struct PatchInteractionResponseData {
        HttpAgentResources agentResources{};
        InteractionResponseData data{};
        string interactionToken{ "" };
        string applicationId{ "" };
    };

    struct PostInteractionResponseData {
        InteractionApplicationCommandCallbackData data{};
        HttpAgentResources agentResources{};
        InteractionCallbackType type{};
        string interactionToken{ "" };
        string interactionId{ "" };
    };

    struct PostFollowUpMessageData {
        AllowedMentionsData allowedMentions{};
        HttpAgentResources agentResources{};
        vector<ActionRowData> components{};
        string interactionToken{ "" };
        string applicationId{ "" };
        vector<EmbedData> embeds{};
        string content{ "" };
        bool tts{ false };
        int flags{ 0 };
    };

    struct GetInteractionResponseData {
        HttpAgentResources agentResources{};
        string interactionToken{ "" };
        string applicationId{ "" };
    };

    struct PostDeferredInteractionResponseData {
        HttpAgentResources agentResources{};
        InteractionCallbackType type{};
        string interactionToken{ "" };
        string interactionId{ "" };
        int flags{ 0 };
    };

    struct HttpData {
        unsigned int returnCode{ 0 };
        string returnMessage{ "" };
        json data{};
    };

    struct GetMessagesData {
        HttpAgentResources agentResources{};
        string beforeThisId{ "" };
        string aroundThisId{ "" };
        string afterThisId{ "" };
        unsigned int limit{ 0 };
        string channelId{ "" };
    };

    struct DeleteMessagesBulkData {
        HttpAgentResources agentResources{};
        vector<string> messageIds{};
        string channelId{ "" };
        string content{ "" };
    };

    struct HttpWorkload {
        HttpWorkloadClass workloadClass{};
        HttpWorkloadType workloadType{};
        string relativePath{ "" };
        string content{ "" };
    };

    struct RateLimitData {
        HttpWorkloadType workloadType{};
        float nextExecutionTime{ 0.0f };
        float timeStartedAt{ 0.0f };
        int getsRemaining{ 1 };
        float msRemain{ 0.0f };
        string bucket{ "" };
    };

    struct CollectGuildData {
        HttpAgentResources agentResources{};
        string guildId{ "" };
    };

    struct GetGuildData {
        HttpAgentResources agentResources{};
        string guildId{ "" };
    };

    struct PutReactionData {
        HttpAgentResources agentResources{};
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

    struct DeleteReactionDataAll {
        HttpAgentResources agentResources{};
        ReactionDeletionType deletionType{};
        string encodedEmoji{ "" };
        string channelId{ "" };
        string messageId{ "" };
        string userId{ "" };
    };

    struct GetReactionData {
        string channelId{ "" };
        string messageId{ "" };
        string emojiName{ "" };
        string userId{ "" };
    };

    struct CollectChannelData {
        HttpAgentResources agentResources{};
        string channelId{ "" };
    };

    struct GetChannelData {
        HttpAgentResources agentResources{};
        string channelId{ "" };
    };

    struct PutPermissionOverwritesData {
        EditChannelPermissionOverwritesType type{};
        HttpAgentResources agentResources{};
        string roleOrUserId{ "" };
        string channelId{ "" };
        string allow{ "" };
        string deny{ "" };
    };

    struct DeleteChannelPermissionOverwritesData {
        HttpAgentResources agentResources{};
        string roleOrUserId{ "" };
        string channelId{ "" };
    };

    struct GetDMChannelData {
        HttpAgentResources agentResources{};
        string userId{ "" };
    };

    struct GetMessageData {
        HttpAgentResources agentResources{};
        string requesterId{ "" };
        string channelId{ "" };
        string messageId{ "" };
    };

    struct PutPinMessageData {
        HttpAgentResources agentResources{};
        string channelId{ "" };
        string messageId{ "" };
    };

    enum class GetUserDataType {
        SELF = 0,
        USER = 1
    };

    struct CollectUserData {
        HttpAgentResources agentResources{};
        GetUserDataType userType{};
        string userId{ "" };        
    };

    struct LeaveGuildData {
        HttpAgentResources agentResources{};
        string guildId{ "" };
    };

    struct GetUserData {
        HttpAgentResources agentResources{};
        GetUserDataType userType{};
        string userId{ "" };        
    };

    struct GetPinnedMessagesData {
        HttpAgentResources agentResources{};
        string channelId{ "" };
    };

    struct PostMessageData {
        MessageReferenceData messageReference{};
        AllowedMentionsData allowedMentions{};
        HttpAgentResources agentResources{};
        vector<ActionRowData> components{};
        vector<EmbedData> embeds{};
        string requesterId{ "" };
        string channelId{ "" };
        string content{ "" };
        bool tts{ false };
        int nonce{ 0 };
    };

    struct PatchMessageData {
        MessageReferenceData messageReference{};
        AllowedMentionsData allowedMentions{};
        vector<AttachmentData> attachments{};
        HttpAgentResources agentResources{};
        vector<ActionRowData> components{};
        MessageData originalMessageData{};
        vector<EmbedData> embeds{};
        string requesterId{ "" };
        string messageId{ "" };
        string channelId{ "" };
        string content{ "" };
        int flags{ 0 };
    };

    struct DeleteMessageData {
        HttpAgentResources agentResources{};
        unsigned int timeDelay{ 0 };
        string channelId{ "" };
        string messageId{ "" };
    };

    struct PostDMData {
        MessageReferenceData messageReference{};
        AllowedMentionsData allowedMentions{};
        HttpAgentResources agentResources{};
        vector<ActionRowData> components{};
        vector<EmbedData> embeds{};
        string requesterId{ "" };
        string channelId{ "" };
        string content{ "" };
        string userId{ "" };
        bool tts{ false };
        int nonce{ 0 };
    };

    struct CollectGuildMemberData {
        HttpAgentResources agentResources{};
        string guildMemberId{ "" };
        string guildId{ "" };
    };

    struct GetGuildMemberData {
        HttpAgentResources agentResources{};
        string guildMemberId{ "" };
        string guildId{ "" };
    };

    struct GetGuildMemberRolesData {
        HttpAgentResources agentResources{};
        vector<string> roleIds{};
    };

    struct PatchGuildMemberData {
        HttpAgentResources agentResources{};
        string newVoiceChannelId{ "" };
        string currentChannelId{ "" };
        string guildMemberId{ "" };
        vector<string> roleIds{};
        string guildId{ "" };
        bool mute{ false };
        bool deaf{ false };
        string nick{ "" };
    };

    struct UpdateVoiceStateData {
        string channelId{ "" };
        bool selfMute{ false };
        bool selfDeaf{ false };
        string guildId{ "" };
    };

    struct GetRolesData {
        HttpAgentResources agentResources{};
        string guildId{ "" };
    };

    struct GetRoleData {
        HttpAgentResources agentResources{};
        string guildId{ "" };
        string roleId{ "" };
    };

    struct DeleteGuildMemberRoleData {
        HttpAgentResources agentResources{};
        string guildId{ "" };
        string userId{ "" };
        string roleId{ "" };
    };

    struct DeleteGuildRoleData {
        HttpAgentResources agentResources{};
        string guildId{ "" };
        string roleId{ "" };
    };

    struct RolePositionData {
        unsigned int rolePosition{ 0 };
        string roleId{ "" };
    };

    struct PatchRolePositionData {
        vector<RolePositionData> rolePositions{};
        HttpAgentResources agentResources{};
        string guildId{ "" };
    };

    struct CollectRoleData {
        HttpAgentResources agentResources{};
        string guildId{ "" };
        string roleId{ "" };
    };

    struct PostRoleData {
        HttpAgentResources agentResources{};
        string hexColorValue{ "" };
        bool mentionable{ false };
        __int64 permissions{ 0 };
        string guildId{ "" };
        string roleId{ "" };
        bool hoist{ false };
        string name{ "" };
    };

    struct PutRoleData {
        HttpAgentResources agentResources{};
        string guildId{ "" };
        string userId{ "" };
        string roleId{ "" };
    };

    struct PatchRoleData {
        HttpAgentResources agentResources{};
        string hexColorValue{ "" };
        bool mentionable{ false };
        string permissions{ "" };
        string content{ "" };
        string guildId{ "" };
        string roleId{ "" };
        bool hoist{ false };
        string name{ "" };
    };

    struct ApplicationCommandOptionChoiceData {
        float valueFloat{ 0.0f };
        string valueString{ "" };
        string name{ "" };
        int	valueInt{ 0 };
    };

    struct ApplicationCommandOptionData {
        vector<ApplicationCommandOptionChoiceData>	choices{};
        vector<ApplicationCommandOptionData> options{};
        ApplicationCommandOptionType type{};
        string description{ "" };
        bool required{ false };
        string name{ "" };
    };

    struct ApplicationCommandData {
        vector<ApplicationCommandOptionData> options{};
        bool defaultPermission{ true };
        ApplicationCommandType type{};
        string applicationId{ "" };
        string description{ "" };
        string guildId{ "" };
        string name{ "" };
        string id{ "" };
    };

    struct PatchApplicationCommandData {
        vector<ApplicationCommandOptionData> options{};
        bool defaultPermission{ true };
        string description{ "" };
        string name{ "" };
    };

    struct PostApplicationCommandData {
        vector<ApplicationCommandOptionData> options{};
        bool defaultPermission{ true };
        ApplicationCommandType type{};
        string applicationId{ "" };
        string description{ "" };
        string name{ "" };
    };

    struct InteractionResponseFullData {
        PostInteractionResponseData interactionResponseData{};
        InteractionData interactionData{};
    };

    struct PatchFollowUpMessageData {
        DiscordCoreInternal::AllowedMentionsData allowedMentions{};
        HttpAgentResources agentResources{};
        vector<ActionRowData> components{};
        string interactionToken{ "" };
        string applicationId{ "" };
        vector<EmbedData> embeds{};
        string requesterId{ "" };
        string messageId{ "" };
        string content{ "" };
        bool tts{ false };
        int flags{ 0 };
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
        HttpAgentResources agentResources{};
        AuditLogEvent actionType{};
        unsigned int limit{ 0 };
        string guildId{ "" };
        string userId{ "" };
    };

    struct GetInviteData {
        HttpAgentResources agentResources{};
        string inviteId{ "" };
    };

    struct GetVanityInviteData {
        HttpAgentResources agentResources{};
        string guildId{ "" };
    };

    struct GetInvitesData {
        HttpAgentResources agentResources{};
        string guildId{ "" };
    };

    struct VoiceConnectInitData {
        string channelId{ "" };
        string guildId{ "" };
        string userId{ "" };
    };

    struct VoiceConnectionData {
        string voiceEncryptionMode{ "" };
        vector<uint8_t> secretKey{};
        string externalIp{ "" };
        string voicePort{ "" };
        string sessionId{ "" };
        string endPoint{ "" };
        string voiceIp{ "" };
        string token{ "" };
        int audioSSRC{ 0 };
        string keys{ "" };
    };

    struct VoiceReadyPayload {
        vector<string> modes{};
        string ip{ "" };
        int ssrc{ 0 };
        int port{ 0 };
    };

    struct PutGuildBanData {
        HttpAgentResources agentResources{};
        string guildMemberId{ "" };
        int deleteMessageDays{ 0 };
        string guildId{ "" };
        string reason{ "" };
    };
};

namespace DiscordCoreAPI {

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
            newData.integrationId = this->integrationId;
            newData.botId = this->botId;
            return newData;
        }

        string premiumSubscriber{ "" };
        string integrationId{ "" };
        string botId{ "" };
    };

    struct RoleData {

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

        shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
        bool mentionable{ false };
        string permissions{ "" };
        bool managed{ false };
        RoleTagsData tags{};
        bool hoist{ false };
        int position{ 0 };
        string name{ "" };
        string id{ "" };
        int color{ 0 };
    };

    struct UserData {

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

        shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
        string discriminator{ "" };
        bool mfaEnabled{ false };
        bool verified{ false };
        string createdAt{ "" };
        string userName{ "" };
        bool system{ false };
        int premiumType{ 0 };
        int publicFlags{ 0 };
        string avatar{ "" };
        string locale{ "" };
        string email{ "" };
        bool bot{ false };
        string id{ "" };
        int flags{ 0 };
    };

    struct EmojiData {

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

        bool requireColons{ false };
        vector<RoleData> roles{};
        bool available{ true };
        bool animated{ false };
        bool managed{ false };
        string name{ "" };
        UserData user{};
        string id{ "" };
    };

    struct SelectOptionData {

        operator DiscordCoreInternal::SelectOptionData() {
            DiscordCoreInternal::SelectOptionData newData;
            newData.description = this->description;
            newData._default = this->_default;
            newData.emoji = this->emoji;
            newData.label = this->label;
            newData.value = this->value;
            return newData;
        }

        string description{ "" };
        bool _default{ false };
        string label{ "" };
        string value{ "" };
        EmojiData emoji{};        
    };

    struct ComponentData {

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

        vector<SelectOptionData> options{};
        string placeholder{ "" };
        bool disabled{ false };
        string customId{ "" };
        ComponentType type{};
        ButtonStyle style{};
        string label{ "" };
        int maxValues{ 0 };
        int minValues{ 0 };
        EmojiData emoji{};
        string url{ "" };
    };

    struct AttachmentData {

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

        string contentType{ "" };
        string filename{ "" };
        string proxyUrl{ "" };
        string url{ "" };
        string id{ "" };        
        int height{ 0 };
        int width{ 0 };
        int size{ 0 };
    };

    struct EmbedFooterData {

        operator DiscordCoreInternal::EmbedFooterData() {
            DiscordCoreInternal::EmbedFooterData newData;
            newData.proxyIconUrl = this->proxyIconUrl;
            newData.iconUrl = this->iconUrl;
            newData.text = this->text;
            return newData;
        };

        string proxyIconUrl{ "" };
        string iconUrl{ "" };
        string text{ "" };
    };

    struct EmbedImageData {

        operator DiscordCoreInternal::EmbedImageData() {
            DiscordCoreInternal::EmbedImageData newData;
            newData.proxyUrl = this->proxyUrl;
            newData.height = this->height;
            newData.width = this->width;
            newData.url = this->url;            
            return newData;
        }

        string proxyUrl{ "" };
        string url{ "" };        
        int height{ 0 };
        int width{ 0 };
    };

    struct EmbedThumbnailData {

        operator DiscordCoreInternal::EmbedThumbnailData() {
            DiscordCoreInternal::EmbedThumbnailData newData;
            newData.proxyUrl = this->proxyUrl;
            newData.height = this->height;
            newData.width = this->width;
            newData.url = this->url;
            return newData;
        }

        string proxyUrl{ "" };
        string url{ "" };
        int height{ 0 };
        int width{ 0 };
    };

    struct EmbedVideoData {

        operator DiscordCoreInternal::EmbedVideoData() {
            DiscordCoreInternal::EmbedVideoData newData;
            newData.proxyUrl = this->proxyUrl;
            newData.height = this->height;
            newData.width = this->width;
            newData.url = this->url;
            return newData;
        }

        string proxyUrl{ "" };
        string url{ "" };
        int height{ 0 };
        int width{ 0 };
    };

    struct EmbedProviderData {

        operator DiscordCoreInternal::EmbedProviderData() {
            DiscordCoreInternal::EmbedProviderData newData;
            newData.name = this->name;
            newData.url = this->url;
            return newData;
        }

        string name{ "" };
        string url{ "" };
    };

    struct EmbedAuthorData {

        operator DiscordCoreInternal::EmbedAuthorData() {
            DiscordCoreInternal::EmbedAuthorData newData;
            newData.proxyIconUrl = this->proxyIconUrl;
            newData.iconUrl = this->iconUrl;
            newData.name = this->name;
            newData.url = this->url;
            return newData;
        };

        string proxyIconUrl{ "" };
        string iconUrl{ "" };
        string name{ "" };
        string url{ "" };
    };

    struct EmbedFieldData {

        operator DiscordCoreInternal::EmbedFieldData() {
            DiscordCoreInternal::EmbedFieldData newData;
            newData.Inline = this->Inline;
            newData.value = this->value;
            newData.name = this->name;
            return newData;
        }

        bool Inline{ false };
        string value{ "" };
        string name{ "" };
    };

    struct EmbedData {

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

    struct ActionRowData {
        operator DiscordCoreInternal::ActionRowData() {
            DiscordCoreInternal::ActionRowData newData;
            for (auto value : this->components) {
                newData.components.push_back(value);
            }
            return newData;
        }
        vector<ComponentData> components{};
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
            newData.repliedUser = this->repliedUser;
            newData.parse = this->parse;
            newData.roles = this->roles;
            newData.users = this->users;
            return newData;
        }

        bool repliedUser{ false };
        vector<string> parse{};
        vector<string> roles{};
        vector<string> users{};
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

        AllowedMentionsData allowedMentions{};
        vector<ActionRowData> components{};
        vector<EmbedData> embeds{};
        string content{ "" };
        bool tts{ false };
        int flags{ 0 };
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
            newData.failIfNotExists = this->failIfNotExists;
            newData.channelId = this->channelId;
            newData.messageId = this->messageId;
            newData.guildId = this->guildId;            
            return newData;
        }

        bool failIfNotExists{ false };
        string messageId{ "" };
        string channelId{ "" };
        string guildId{ "" };
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
            newData.type = (DiscordCoreInternal::EditChannelPermissionOverwritesType)this->type;
            newData.channelId = this->channelId;
            newData.allow = this->allow;
            newData.deny = this->deny;
            newData.id = this->id;
            return newData;
        }

        EditChannelPermissionOverwritesType type{};   //	either 0 (role) or 1 (member;
        string channelId{ "" };
        string allow{ "" };
        string deny{ "" };
        string id{ "" };
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

        operator DiscordCoreInternal::ThreadMetadataData() {
            DiscordCoreInternal::ThreadMetadataData newData;
            newData.autoArchiveDuration = this->autoArchiveDuration;
            newData.archiveTimestamp = this->archiveTimestamp;
            newData.archiverId = this->archiverId;
            newData.archived = this->archived;
            newData.locked = this->locked;
            return newData;
        };

        string archiveTimestamp{ "" };
        int autoArchiveDuration{ 0 };
        string archiverId{ "" };
        bool archived{ false };
        bool locked{ false };
    };

    struct ThreadMemberData {

        operator DiscordCoreInternal::ThreadMemberData() {
            DiscordCoreInternal::ThreadMemberData newData;
            newData.joinTimestamp = this->joinTimestamp;
            newData.userId = this->userId;
            newData.flags = this->flags;
            newData.id = this->id;
            return newData;
        }

        string joinTimestamp{ "" };
        string userId{ "" };
        string id{ "" };
        int flags{ 0 };
    };

    struct ChannelData {

        operator DiscordCoreInternal::ChannelData() {
            DiscordCoreInternal::ChannelData newData;
            for (auto [key, value] : this->permissionOverwrites) {
                newData.permissionOverwrites.insert(make_pair(key, value));
            }
            newData.type = (DiscordCoreInternal::ChannelType)this->type;
            newData.rateLimitPerUser = this->rateLimitPerUser;
            newData.lastPinTimestamp = this->lastPinTimestamp;
            newData.videoQualityMode = this->videoQualityMode;
            newData.threadMetadata = this->threadMetadata;
            newData.applicationId = this->applicationId;
            newData.lastMessageId = this->lastMessageId;
            for (auto value : this->recipients) {
                newData.recipients.push_back(value);
            }
            newData.rtcRegion = this->rtcRegion;
            newData.userLimit = this->userLimit;
            newData.parentId = this->parentId;
            newData.position = this->position;
            newData.bitrate = this->bitrate;
            newData.guildId = this->guildId;
            newData.ownerId = this->ownerId;
            newData.member = this->member;
            newData.topic = this->topic;
            newData.icon = this->icon;
            newData.name = this->name;
            newData.nsfw = this->nsfw;
            newData.id = this->id;            
            return newData;
        }

        shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
        map<string, OverWriteData> permissionOverwrites{};
        ThreadMetadataData threadMetadata{};
        ChannelType type{ ChannelType::DM };
        vector<UserData> recipients{};
        string lastPinTimestamp{ "" };
        string lastMessageId{ "" };
        string applicationId{ "" };
        int videoQualityMode{ 0 };
        int rateLimitPerUser{ 0 };
        ThreadMemberData member{};
        string rtcRegion{ "" };
        int messageCount{ 0 };
        string parentId{ "" };
        string guildId{ "" };
        string ownerId{ "" };
        int memberCount{ 0 };
        string topic{ "" };
        bool nsfw{ false };
        int userLimit{ 0 };
        string name{ "" };
        int position{ 0 };
        string icon{ "" };
        int bitrate{ 0 };
        string id{ "" };
    };

    struct VoiceStateData {
        string requestToSpeakTimestamp{ "" };
        bool selfStream{ false };
        bool selfVideo{ false };
        bool selfDeaf{ false };
        bool selfMute{ false };
        string sessionId{ "" };
        string channelId{ "" };
        bool suppress{ false };
        string guildId{ "" };
        string userId{ "" };
        bool deaf{ false };
        bool mute{ false };
    };

    struct GuildMemberData {

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

        shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
        VoiceStateData voiceData{};
        string premiumSince{ "" };
        string permissions{ "" };
        string userMention{ "" };
        vector<string> roles{};
        string joinedAt{ "" };
        bool pending{ false };
        string guildId{ "" };
        bool deaf{ false };
        bool mute{ false };
        string nick{ "" };
        UserData user{};
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

        map<string, GuildMemberData> members{};
        map<string, ChannelData> channels{};
        map<string, UserData> users{};
        map<string, RoleData> roles{};
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

    struct ApplicationCommandInteractionDataOption;

    vector<DiscordCoreInternal::ApplicationCommandInteractionDataOption> convertAppCommandInteractionDataOptions(vector<ApplicationCommandInteractionDataOption> originalOptions);

    struct ApplicationCommandInteractionDataOption {

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

        vector<ApplicationCommandInteractionDataOption> options{};
        ApplicationCommandOptionType type{};
        string valueString{ "" };
        bool valueBool{ false };
        string name{ "" };
        int valueInt{ 0 };
        
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
            for (auto value : this->options) {
                newData.options.push_back(value);
            }
            newData.customId = this->customId;
            newData.resolved = this->resolved;
            newData.name = this->name;
            newData.id = this->id;
            return newData;
        }

        vector<ApplicationCommandInteractionDataOption> options{};
        ApplicationCommandInteractionDataResolved resolved{};
        int componentType{ 0 };
        string customId{ "" };
        string name{ "" };
        string id{ "" };
    };

    struct ChannelMentionData {

        operator DiscordCoreInternal::ChannelMentionData() {
            DiscordCoreInternal::ChannelMentionData newData;
            newData.guildId = this->guildId;
            newData.name = this->name;
            newData.type = this->type;
            newData.id = this->id;
            return newData;
        }

        string guildId{ "" };
        string name{ "" };
        string id{ "" };
        int type{ 0 };
        
    };

    struct ReactionData {

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

        shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
        GuildMemberData member{};
        string channelId{ "" };
        string messageId{ "" };
        string guildId{ "" };
        string userId{ "" };
        EmojiData emoji{};
        bool me{ false };
        int count{ 0 };
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
        string partyId{ "" };
        int type{ 0 };
    };

    struct MessageInteractionData {

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
        string id{ "" };
    };

    struct MessageStickerData {

        operator DiscordCoreInternal::MessageStickerData() {
            DiscordCoreInternal::MessageStickerData newData;
            newData.description = this->description;
            newData.formatType = this->formatType;
            newData.available = this->available;
            newData.sortValue = this->sortValue;
            newData.guildId = this->guildId;
            newData.packId = this->packId;
            newData.asset = this->asset;
            newData.name = this->name;
            newData.tags = this->tags;
            newData.user = this->user;
            newData.id = this->id;            
            return newData;
        }

        bool available{ false };
        string description{ "" };
        string guildId{ "" };
        string packId{ "" };
        int formatType{ 0 };
        string asset{ "" };
        int sortValue{ 0 };
        string name{ "" };
        string tags{ "" };
        string id{ "" };
        UserData user{};
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

        vector<string> permissions{};
        int membershipState{ 0 };
        string teamId{ "" };
        UserData user{};
    };

    struct TeamObjectData {

        operator DiscordCoreInternal::TeamObjectData() {
            DiscordCoreInternal::TeamObjectData newData;
            newData.ownerUserId = this->ownerUserId;
            newData.icon = this->icon;
            newData.id = this->id;            
            return newData;
        }

        vector<TeamMembersObjectData> members{};
        string ownerUserId{ "" };
        string icon{ "" };
        string id{ "" };
    };

    struct ApplicationData {

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

        bool botRequireCodeGrant{ false };
        string termsOfServiceUrl{ "" };
        string privacyPolicyUrl{ "" };
        vector<string> rpcOrigins{};
        string primarySkuId{ "" };
        string description{ "" };
        bool botPublic{ false };
        string coverImage{ "" };
        string verifyKey{ "" };
        TeamObjectData team{};
        string  summary{ "" };
        string guildId{ "" };
        string slug{ "" };
        string name{ "" };
        string icon{ "" };
        UserData owner{};
        string id{ "" };
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
            newData.name = this->name;
            newData.id = this->id;            
            return newData;
        }

        MessageStickerItemType formatType{};
        string name{ "" };
        string id{ "" };
    };

    struct MessageDataOld {

        operator DiscordCoreInternal::MessageDataOld() {
            DiscordCoreInternal::MessageDataOld newData;
            newData.type = (DiscordCoreInternal::MessageType)this->type;
            newData.messageReference = this->messageReference;
            newData.activity.partyId = this->activity.partyId;
            newData.editedTimestamp = this->editedTimestamp;
            newData.mentionEveryone = this->mentionEveryone;
            for (auto value : this->mentionChannels) {
                newData.mentionChannels.push_back(value);
            }
            newData.activity.type = this->activity.type;
            for (auto value : this->stickerItems) {
                newData.stickerItems.push_back(value);
            }
            newData.mentionRoles = this->mentionRoles;
            for (auto value : this->attachments) {
                newData.attachments.push_back(value);
            }
            for (auto value : this->components) {
                newData.components.push_back(value);
            }
            newData.interaction = this->interaction;
            newData.application = this->application;
            for (auto value : this->reactions) {
                newData.reactions.push_back(value);
            }
            for (auto value : this->stickers) {
                newData.stickers.push_back(value);
            }
            for (auto value : this->mentions) {
                newData.mentions.push_back(value);
            }
            for (auto value : this->embeds) {
                newData.embeds.push_back(value);
            }
            newData.channelId = this->channelId;
            newData.webhookId = this->webhookId;
            newData.timestamp = this->timestamp;
            newData.content = this->content;
            newData.guildId = this->guildId;
            newData.author = this->author;
            newData.member = this->member;
            newData.pinned = this->pinned;
            newData.flags = this->flags;
            newData.nonce = this->nonce;
            newData.tts = this->tts;
            newData.id = this->id;
            return newData;
        }

        vector<MessageStickerItemData> stickerItems{};
        vector<ChannelMentionData> mentionChannels{};
        MessageReferenceData messageReference{};
        vector<MessageStickerData> stickers{};
        vector<AttachmentData> attachments{};
        MessageInteractionData interaction{};
        vector<ActionRowData> components{};
        vector<ReactionData> reactions{};
        MessageActivityData activity{};
        ApplicationData application{};
        vector<string> mentionRoles{};
        bool mentionEveryone{ false };
        string editedTimestamp{ "" };
        vector<UserData> mentions{};
        vector<EmbedData> embeds{};
        string applicationId{ "" };
        string timestampRaw{ "" };
        GuildMemberData member{};
        string channelId{ "" };
        string timestamp{ "" };
        string webhookId{ "" };
        string guildId{ "" };
        bool pinned{ false };
        string content{ "" };
        ChannelData thread{};
        string nonce{ "" };
        MessageType type{};
        UserData author{};
        bool tts{ false };
        string id{ "" };
        int flags{ 0 };
    };

    struct MessageData :public MessageDataOld {

        operator DiscordCoreInternal::MessageData() {
            DiscordCoreInternal::MessageData newData;
            newData.referencedMessage = this->referencedMessage;
            newData.requesterId = this->requesterId;
            return newData;
        }

        shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
        MessageDataOld referencedMessage{};
        string requesterId{ "" };        
    };

    struct InteractionData {

        operator DiscordCoreInternal::InteractionData() {
            DiscordCoreInternal::InteractionData newData;
            newData.componentType = (DiscordCoreInternal::ComponentType)this->componentType;
            newData.type = (DiscordCoreInternal::InteractionType)this->type;
            newData.applicationId = this->applicationId;
            newData.requesterId = this->requesterId;
            newData.channelId = this->channelId;
            newData.customId = this->customId;
            newData.message = this->message;
            newData.dataRaw = this->dataRaw;
            newData.guildId = this->guildId;
            newData.version = this->version;
            newData.member = this->member;
            newData.values = this->values;
            newData.token = this->token;
            newData.user = this->user;
            newData.data = this->data;
            newData.id = this->id;
            return newData;
        }

        ApplicationCommandInteractionData data{};
        ComponentType componentType{};
        string applicationId{ "" };
        GuildMemberData member{};
        string requesterId{ "" };
        vector<string> values{};
        string channelId{ "" };
        InteractionType type{};
        MessageData message{};
        string customId{ "" };
        string guildId{ "" };
        string token{ "" };
        string name{ "" };
        int version{ 0 };
        UserData user{};
        string id{ "" };
        json dataRaw{};

        InteractionData() {};

        InteractionData(string requesterId) {
            this->requesterId = requesterId;
        }
    };

    struct PartyData {

        operator DiscordCoreInternal::PartyData() {
            DiscordCoreInternal::PartyData newData;
            newData.size[0] = this->size[0];
            newData.size[1] = this->size[1];
            newData.id = this->id;
            return newData;
        }

        vector<int> size{ 0, 0 };
        string id{ "" };
    };

    struct AssetsData {

        operator DiscordCoreInternal::AssetsData() {
            DiscordCoreInternal::AssetsData newData;
            newData.largeImage = this->largeImage;
            newData.smallImage = this->smallImage;
            newData.largeText = this->largeText;
            newData.smallText = this->smallText;
            return newData;
        }

        string largeImage{ "" };
        string smallImage{ "" };
        string largeText{ "" };
        string smallText{ "" };
    };

    struct SecretsData {

        operator DiscordCoreInternal::SecretsData() {
            DiscordCoreInternal::SecretsData newData;
            newData.spectate = this->spectate;
            newData.match = this->match;
            newData.join = this->join;
            return newData;
        }

        string spectate{ "" };
        string match{ "" };
        string join{ "" };
    };

    struct TimestampData {

        operator DiscordCoreInternal::TimestampData() {
            DiscordCoreInternal::TimestampData newData;
            newData.start = this->start;
            newData.end = this->end;
            return newData;
        }

        __int64 start{ 0 };
        __int64 end{ 0 };
    };

    struct ButtonData {

        operator DiscordCoreInternal::ButtonData() {
            DiscordCoreInternal::ButtonData newData;
            newData.label = this->label;
            newData.url = this->url;
            return newData;
        }

        string label{ "" };
        string url{ "" };
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
            newData.timestamps = this->timestamps;
            newData.createdAt = this->createdAt;
            newData.instance = this->instance;
            newData.secrets = this->secrets;
            newData.details = this->details;
            newData.buttons = this->buttons;
            newData.type = (int)this->type;
            newData.assets = this->assets;
            newData.emoji = this->emoji;
            newData.flags = this->flags;
            newData.party = this->party;
            newData.state = this->state;
            newData.name = this->name;
            newData.url = this->url;
            return newData;
        }

        TimestampData timestamps{};
        string applicationId{ "" };
        bool instance{ false };
        SecretsData secrets{};
        string details{ "" };
        ButtonData buttons{};
        ActivityType type{};
        AssetsData assets{};
        int createdAt{ 0 };
        string state{ "" };
        EmojiData emoji{};
        PartyData party{};
        string name{ "" };
        string url{ "" };
        int flags{ 0 };
    };

    struct ClientStatusData {
        string desktop{ "" };
        string mobile{ "" };
        string web{ "" };
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
        string description{ "" };
        string channelId{ "" };
        string emojiName{ "" };
        string emojiId{ "" };
    };

    struct WelcomeScreenData {
        vector<WelcomeScreenChannelData> welcomeChannels{};
        string description{ "" };
    };

    struct PresenceUpdateData {
        vector<ActivityData> activities{};
        ClientStatusData clientStatus{};
        string guildId{ "" };
        string status{ "" };
        UserData user{};
    };

    struct StageInstanceData {
        bool discoverableDisabled{ false };
        string channelId{ "" };
        int privacyLevel{ 0 };
        string guildId{ "" };
        string topic{ "" };
        string id{ "" };
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

    struct StickerData {
        shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
        StickerFormatType formatType{};
        string description{ "" };
        bool available{ false };
        string guildId{ "" };
        string packId{ "" };
        string asset{ "" };
        StickerType type{};
        int sortValue{ 0 };
        string name{ "" };
        string tags{ "" };
        string id{ "" };
        UserData user{};
    };

    struct GuildData {
        shared_ptr<DiscordCoreClientBase> discordCoreClientBase{ nullptr };
        DefaultMessageNotificationLevel defaultMessageNotifications{};
        shared_ptr<DiscordCoreClient> discordCoreClient{ nullptr };
        ExplicitContentFilterLevel explicitContentFilter{};
        vector<StageInstanceData> stageInstances{};
        vector<PresenceUpdateData> presences{};
        VerificationLevel verificationLevel{};
        vector<VoiceStateData> voiceStates{};
        string publicUpdatesChannelId{ "" };
        vector<GuildMemberData> members{};
        WelcomeScreenData welcomeScreen{};
        int premiumSubscriptionCount{ 0 };
        int approximatePresenceCount{ 0 };
        int approximateMemberCount{ 0 };
        vector<ChannelData> channels{};
        vector<StickerData> stickers{};
        vector<ChannelData> threads{};
        int maxVideoChannelUsers{ 0 };
        string discoverySplash{ "" };
        string preferredLocale{ "" };
        string widgetChannelId{ "" };
        string systemChannelId{ "" };
        string rulesChannelId{ "" };
        bool widgetEnabled{ false };
        int systemChannelFlags{ 0 };
        string vanityURLCode{ "" };
        string applicationId{ "" };
        vector<EmojiData> emoji{};
        string description{ "" };
        string afkChannelId{ "" };
        vector<string> features{};
        bool unavailable{ false };
        PremiumTier premiumTier{};
        string permissions{ "" };
        vector<RoleData> roles{};
        string createdAt{ "" };
        string iconHash{ "" };
        int maxPresences{ 0 };
        string joinedAt{ "" };
        string ownerId{ "" };
        int memberCount{ 0 };
        string region{ "" };
        bool owner{ false };
        int afkTimeOut{ 0 };
        MFALevel mfaLevel{};
        bool large{ false };
        int maxMembers{ 0 };
        string splash{ "" };
        string banner{ "" };
        int nsfwLevel{ 0 };
        string icon{ "" };
        string name{ "" };
        string id{ "" };
    };

    struct ReactionAddData {
        DiscordCoreClient* discordCoreClient{ nullptr };
        GuildMemberData member{};
        string channelId{ "" };
        string messageId{ "" };
        string guildId{ "" };
        string userId{ "" };
        EmojiData emoji{};
    };

    struct ReactionRemoveData {
        DiscordCoreClient* discordCoreClient{ nullptr };
        string channelId{ "" };
        string messageId{ "" };
        string guildId{ "" };
        string userId{ "" };
        EmojiData emoji{};
    };

    struct InteractionResponseData {
        InteractionApplicationCommandCallbackData data{};
        InteractionCallbackType type{};
    };

    struct InteractionPackageData {
        string interactionToken{ "" };
        string applicationId{ "" };
        string interactionId{ "" };
    };

    struct MessagePackageData {
        string channelId{ "" };
        string messageId{ "" };
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

    struct ApplicationCommandOptionChoiceData {
        operator DiscordCoreInternal::ApplicationCommandOptionChoiceData() {
            DiscordCoreInternal::ApplicationCommandOptionChoiceData newData;
            newData.valueString = this->valueString;
            newData.valueFloat = this->valueFloat;
            newData.valueInt = this->valueInt;
            newData.name = this->name;
            return newData;
        }
        string valueString{ "" };
        float valueFloat{ 0.0f };
        string name{ "" };
        int	valueInt{ 0 };
    };

    struct ApplicationCommandOptionData {
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
        vector<ApplicationCommandOptionChoiceData>	choices{};
        vector<ApplicationCommandOptionData> options{};
        ApplicationCommandOptionType type{};
        string description{ "" };
        bool required{ false };
        string name{ "" };
    };

    struct ApplicationCommandData {
        vector<ApplicationCommandOptionData> options{};
        bool defaultPermission{ false };
        ApplicationCommandType type{};
        string applicationId{ "" };
        string description{ "" };        
        string guildId{ "" };
        string name{ "" };
        string id{ "" };
    };

    struct UserCommandInteractionData {
        ApplicationCommandType type{};
        string interactionId{ "" };
        string applicationId{ "" };
        GuildMemberData members{};
        GuildMemberData member{};
        string channelId{ "" };
        string targetId{ "" };
        string guildId{ "" };
        string menuId{ "" };
        string token{ "" };
        string name{ "" };
        UserData users{};
        int version{ 0 };
    };

    struct MessageCommandInteractionData {
        ApplicationCommandType type{};
        string interactionId{ "" };
        string applicationId{ "" };
        GuildMemberData member{};
        MessageData messages{};
        string channelId{ "" };
        string targetId{ "" };
        string guildId{ "" };
        string menuId{ "" };
        string token{ "" };
        string name{ "" };
        int version{ 0 };
    };

    struct InputEventData {

        friend struct RecurseThroughMessagePagesData;
        friend struct OnInteractionCreationData;
        friend struct RespondToInputEventData;
        friend struct BaseFunctionArguments;
        friend class DiscordCoreClient;
        friend struct CommandData;
        friend class InputEvents;

        shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClient{ nullptr };
        InputEventResponseType inputEventResponseType{};
        InputEventType eventType{};

        InputEventData(MessageData messageData, InteractionData interactionData, InputEventType eventType, shared_ptr<DiscordCoreAPI::DiscordCoreClient> discordCoreClientNew) {
            this->discordCoreClient = discordCoreClientNew;
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
            else if (this->messageCommandInteractionData.member.user.userName != "") {
                return this->messageCommandInteractionData.member.user.userName;
            }
            else {
                return this->userCommandInteractionData.member.user.userName;
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

        string getAuthorId() {
            if (this->messageData.author.id == "") {
                return this->interactionData.user.id;
            }
            else if (this->messageData.author.id != "") {
                return this->messageData.author.id;
            }
            else if (this->messageCommandInteractionData.member.user.id != "") {
                return this->messageCommandInteractionData.member.user.id;
            }
            else {
                return this->userCommandInteractionData.member.user.id;
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

        string getApplicationId() {
            if (this->interactionData.applicationId == "") {
                return this->messageData.application.id;
            }
            else if (this->interactionData.applicationId != "") {
                return this->interactionData.applicationId;
            }
            else if (this->messageCommandInteractionData.applicationId != "") {
                return this->messageCommandInteractionData.applicationId;
            }
            else {
                return this->userCommandInteractionData.applicationId;
            }
        }

        string getChannelId() {
            if (this->interactionData.channelId == "") {
                return this->messageData.channelId;
            }
            else if (this->interactionData.channelId != "") {
                return this->interactionData.channelId;
            }
            else if (this->messageCommandInteractionData.channelId != "") {
                return this->messageCommandInteractionData.channelId;
            }
            else {
                return this->userCommandInteractionData.channelId;
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

        string getGuildId() {
            if (this->messageData.guildId == "") {
                return this->interactionData.guildId;
            }
            else if (this->messageData.guildId != "") {
                return this->messageData.guildId;
            }
            else if (this->userCommandInteractionData.guildId != "") {
                return this->userCommandInteractionData.guildId;
            }
            else {
                return this->messageCommandInteractionData.guildId;
            }
        }

        string getMessageContent() {
            if (this->interactionData.message.content != "") {
                return this->interactionData.message.content;
            }
            else {
                return this->messageData.content;
            }
        }

        string getMessageId() {
            if (this->messageData.id != "") {
                return this->messageData.id;

            }
            else if (this->interactionData.message.id != "") {
                return this->interactionData.message.id;
            }
            else  {
                return this->messageCommandInteractionData.messages.id;
            }
        }

        InteractionData getInteractionData() {
            return this->interactionData;
        }

        MessageData getMessageData() {
            if (this->messageData.id != "") {
                return this->messageData;

            }
            else {
                return this->interactionData.message;
            }
        }

        string getRequesterId() {
            return this->requesterId;
        }

    protected:
        MessageCommandInteractionData messageCommandInteractionData{};
        UserCommandInteractionData userCommandInteractionData{};
        InteractionData interactionData{};
        MessageData messageData{};
        string requesterId{ "" };

        InputEventData() {}
    };

    struct WebhookData {
        ChannelData sourceChannel{};
        string applicationId{ "" };
        GuildData sourceGuild{};
        unsigned int type{ 0 };
        string channelId{ "" };
        string guildId{ "" };
        string avatar{ "" };
        string token{ "" };
        string name{ "" };
        string url{ "" };
        string id{ "" };
        UserData user{};
    };

    struct AccountData {
        string name{ "" };
        string id{ "" };
    };

    struct IntegrationData {
        unsigned int expireGracePeriod{ 0 };
        unsigned int subscriberCount{ 0 };
        unsigned int expireBehavior{ 0 };
        ApplicationData application{};
        bool enableEmoticons{ true };
        bool enabled{ false };
        bool syncing{ false };
        AccountData account{};
        string syncedAt{ "" };
        bool revoked{ false };
        string roleId{ "" };
        string name{ "" };
        string type{ "" };
        string id{ "" };
        UserData user{};
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
        string deleteMemberDays{ "" };
        string membersRemoved{ "" };
        string channelId{ "" };
        string messageId{ "" };
        string roleName{ "" };
        string count{ "" };
        string type{ "" };
        string id{ "" };
    };

    struct AuditLogChangeData {
        string newValueString{ "" };
        string oldValueString{ "" };
        string key{ "" };
    };

    struct AuditLogEntryData {
        vector<AuditLogChangeData> changes{};
        AuditLogEntryInfoData options{};
        AuditLogEvent actionType{};
        string targetId{ "" };
        string userId{ "" };
        string reason{ "" };
        string id{ "" };
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
        vector<AuditLogEntryData> auditLogEntries{};
        vector<IntegrationData> integrations{};
        vector<WebhookData> webhooks{};
        vector<ChannelData> threads{};
        vector<UserData> users{};
    };

    struct InviteData {
        ApplicationData targetApplication{};
        int approximatePresenceCount{ 0 };
        StageInstanceData stageInstance{};
        int approximateMemberCount{ 0 };
        bool temporary{ false };
        string createdAt{ "" };
        string expiresAt{ "" };
        string channelId{ "" };
        ChannelData channel{};
        UserData targetUser{};
        string guildId{ "" };
        int targetType{ 0 };
        UserData inviter{};
        GuildData guild{};
        string code{ "" };
        int maxUses{ 0 };
        int maxAge{ 0 };
        int uses{ 0 };
    };

    struct TypingStartData {
        GuildMemberData member{};
        string channelId{ "" };
        string guildId{ "" };
        string userId{ "" };
        int timestamp{ 0 };
    };

    struct YouTubeFormat {
        string signatureCipher{ "" };
        string audioSampleRate{ "" };
        __int64 contentLength{ 0 };
        string audioQuality{ "" };
        string downloadURL{ "" };
        int averageBitrate{ 0 };
        string signature{ "" };
        string mimeType{ "" };
        string quality{ "" };
        string codecs{ "" };
        string aitags{ "" };
        int bitrate{ 0 };
        int height{ 0 };
        int width{ 0 };
        int itag{ 0 };
        int fps{ 0 };
    };

    struct BanData {
        bool failedDueToPerms{ false };
        string reason{ "" };
        UserData user{};
    };

    struct RawFrameData {
        int32_t sampleCount{ -1 };
        vector<uint8_t> data{};
    };

    struct EncodedFrameData {
        int32_t sampleCount{ -1 };
        vector<uint8_t> data{};
    };

    enum class AudioFrameType {
        Unset = 0,
        Encoded = 1,
        RawPCM = 2,
        Cancel = 3
    };

    struct AudioFrameData {
        AudioFrameType type{ AudioFrameType::Unset };
        EncodedFrameData encodedFrameData{};
        RawFrameData rawFrameData{};
    };

    struct SoundCloudSearchResult {
        string thumbNailURL{ "" };
        string description{ "" };
        string songTitle{ "" };
        string duration{ "" };
        string songURL{ "" };
        string songId{ "" };
    };

    struct YouTubeSearchResult {
        vector<YouTubeFormat> formats{};
        string thumbNailURL{ "" };
        string description{ "" };
        string videoTitle{ "" };
        string duration{ "" };
        string videoURL{ "" };
        string videoId{ "" };
    };

    struct YouTubeSong {
        string formatDownloadURL{ "" };
        string addedByUserName{ "" };
        string description{ "" };
        int contentLength{ 0 };
        string addedById{ "" };
        string imageURL{ "" };
        string duration{ "" };
        string videoId{ "" };
        string songId{ "" };
        string title{ "" };
        string url{ "" };
    };

    struct Playlist {
        vector<YouTubeSong> songQueue{};
        bool isLoopSongEnabled{ false };
        bool isLoopAllEnabled{ false };
    };

    struct SendNextSongReturnData {
    public:
        YouTubeSong currentSong{};
    };

    static string commandPrefix;

    struct RepeatedFunctionData {
        function<void(shared_ptr<DiscordCoreClient>)> function{ nullptr };
        bool repeated{ false };
        int intervalInMs{ 0 };
    };

    struct CommandData {

        vector<string> optionsArgs{};
        InputEventData eventData{};
        string commandName{ "" };

        CommandData(InputEventData inputEventData) {
            this->eventData = inputEventData;
            if (inputEventData.userCommandInteractionData.name != "") {
                this->commandName = inputEventData.userCommandInteractionData.name;
                if (inputEventData.userCommandInteractionData.users.id != "") {
                    this->optionsArgs.push_back(inputEventData.userCommandInteractionData.users.id);
                }
                else {
                    this->optionsArgs.push_back(inputEventData.userCommandInteractionData.members.user.id);
                }
            }
            else if (inputEventData.messageCommandInteractionData.name != "") {
                this->commandName = inputEventData.messageCommandInteractionData.name;
                this->optionsArgs.push_back(inputEventData.messageCommandInteractionData.messages.id);
            }
        }
    };

    string constructStringContent(CommandData commandData) {
        string finalCommandString = commandData.commandName + " = ";
        for (auto& value : commandData.optionsArgs) {
            finalCommandString += value + ", ";
        }
        return finalCommandString;
    };

    struct ChannelPinsUpdateEventData {
        string lastPinTimestamp{ "" };
        string channelId{ "" };
        string guildId{ "" };
    };

};

#endif