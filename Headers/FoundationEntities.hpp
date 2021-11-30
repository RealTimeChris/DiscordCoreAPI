// FoundationEntities.hpp - Header for all of the Discord/Support API data structures.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"

#ifdef DELETE
#undef DELETE
#endif

namespace DiscordCoreInternal {

    class DataParser;

};

namespace DiscordCoreAPI {

    class DiscordCoreClient;
    class VoiceConnection;
    class GuildMember;

    DiscordCoreAPI_Dll bool nanoSleep(int64_t ns);

    DiscordCoreAPI_Dll void spinLock(int64_t timeInNsToSpinLockFor);

    template<typename storageType>
    class DiscordCoreAPI_Dll ObjectCache {
    public:

        friend class Guilds;

        ObjectCache() {}

        ~ObjectCache() {}

        storageType returnValue(string valueId) {
            lock_guard<recursive_mutex> returnLock{ this->accessMutex };
            return this->cache.at(valueId);
        }

        storageType* returnPointer(string valueId) {
            lock_guard<recursive_mutex> returnLock{ this->accessMutex };
            return &(this->cache).at(valueId);
        }

        bool contains(string valueId) {
            lock_guard<recursive_mutex> containLock{ this->accessMutex };
            return this->cache.contains(valueId);
        }

        void erase(string valueId) {
            lock_guard<recursive_mutex> eraseLock{ this->accessMutex };
            if (this->cache.contains(valueId)) {
                this->cache.erase(valueId);
            }
        }

        void storeValue(string valueId, storageType storageValue) {
            lock_guard<recursive_mutex> storeLock{ this->accessMutex };
            this->cache.insert_or_assign(valueId, move(storageValue));
        }

    protected:

        map<string, storageType> cache{};

        recursive_mutex accessMutex{};
    };

    template<typename timeType>
    class DiscordCoreAPI_Dll StopWatch {
    public:
        StopWatch<timeType>(int64_t maxNumberOfMsNew) {
            this->maxNumberOfMs = maxNumberOfMsNew;
            this->startTime = chrono::duration_cast<timeType>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        }

        int64_t totalTimePassed() {
            int64_t currentTime = chrono::duration_cast<timeType>(chrono::high_resolution_clock::now().time_since_epoch()).count();
            int64_t elapsedTime = currentTime - this->startTime;
            return elapsedTime;
        }

        bool hasTimePassed() {
            int64_t currentTime = chrono::duration_cast<timeType>(chrono::high_resolution_clock::now().time_since_epoch()).count();
            int64_t elapsedTime = currentTime - this->startTime;
            if (elapsedTime >= this->maxNumberOfMs) {
                return true;
            }
            else {
                return false;
            }
        }
        void resetTimer() {
            this->startTime = chrono::duration_cast<timeType>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        }
    protected:
        int64_t maxNumberOfMs{ 0 };
        int64_t startTime{ 0 };
    };

    template <typename T>
    bool waitForTimeToPass(unbounded_buffer<T>* outBuffer, T* argOne, int32_t timeInMsNew) {
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

    template <typename T>
    bool waitForTimeToPass(concurrent_queue<T>* outBuffer, T* argOne, int32_t timeInMsNew) {
        StopWatch<chrono::milliseconds> stopWatch{ timeInMsNew };
        bool doWeBreak{ false };
        while (!outBuffer->try_pop(*argOne)) {
            wait(10);
            if (stopWatch.hasTimePassed()) {
                doWeBreak = true;
                break;
            }
        };
        return doWeBreak;
    }

    template <typename ...T>
    void executeFunctionAfterTimePeriod(function<void(T...)>theFunction, int32_t timeDelayInMs, bool isRepeating, T... args) {
        ThreadPoolTimer threadPoolTimer{ nullptr };
        if (timeDelayInMs > 0 && !isRepeating) {
            wait(timeDelayInMs);
            theFunction(args...);
            return;
        }
        else if (timeDelayInMs > 0 && isRepeating) {
            auto timeElapsedHandler = [=](ThreadPoolTimer threadPoolTimerNew)->void {
                theFunction(args...);
                return;
            };
            threadPoolTimer = threadPoolTimer.CreatePeriodicTimer(timeElapsedHandler, winrt::Windows::Foundation::TimeSpan(timeDelayInMs * 10000));
            DiscordCoreClient::threadPoolTimers.push_back(threadPoolTimer);
        }
        else {
            theFunction(args...);
        }
        return;
    }

    class DiscordCoreAPI_Dll TimeStamp : public string{
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

    DiscordCoreAPI_Dll string convertTimeInMsToDateTimeString(int64_t timeInMs);

    DiscordCoreAPI_Dll int64_t convertTimestampToInteger(string timeStamp);

    DiscordCoreAPI_Dll string convertTimeStampToNewOne(string timeStamp);

    DiscordCoreAPI_Dll void rethrowException(string stackTrace, unbounded_buffer<exception>* sendBuffer = nullptr, bool rethrow = false);

    DiscordCoreAPI_Dll string convertToLowerCase(string stringToConvert);

    DiscordCoreAPI_Dll bool hasTimeElapsed(string timeStamp, int64_t days = 0, int64_t hours = 0, int64_t minutes = 0);

    DiscordCoreAPI_Dll string convertMsToDurationString(int32_t durationInMs);

    DiscordCoreAPI_Dll string getTimeAndDate();

    DiscordCoreAPI_Dll string getISO8601TimeStamp(string year, string month, string day, string hour, string minute, string second);

    typedef string Snowflake;

    /**
    * \addtogroup foundation_entities
    * @{
    */

    /// Base class DiscordCoreAPI_Dll for all Discord entities. \brief Base class DiscordCoreAPI_Dll for all Discord entities.
    class DiscordCoreAPI_Dll DiscordEntity {
    public:
        Snowflake id{ "" };///< The identifier "snowflake" of the given entity.
        /// Converts the snowflake-id into a time and date stamp. \brief Converts the snowflake-id into a time and date stamps.
        /// \returns A string containing the timestamp.
        string getCreatedAtTimestamp();

        virtual ~DiscordEntity() {};
    };

    /// Role tags data. \brief Role tags data.
    struct DiscordCoreAPI_Dll RoleTagsData {
        string premiumSubscriber{ "" };///< Are they a premium subscriber?
        string integrationId{ "" };///< What is the integration id?
        string botId{ "" };///< What is the bot id?
    };

    /// Data structure representing a single Role. \brief Data structure representing a single Role.
    struct DiscordCoreAPI_Dll RoleData : public DiscordEntity{
        bool mentionable{ false }; ///< Is ths Role mentionable?
        string permissions{ "" }; ///< The Role's base Guild Permissions.
        int32_t position{ 0 }; ///< Its position amongst the rest of the Guild's roles.
        bool managed{ false }; ///< is it a managed Role?
        RoleTagsData tags{}; ///< Role tags for the Role.
        bool hoist{ false };///< Is it hoisted above the other roles?
        int32_t color{ 0 };///< The Role's color.
        string name{ "" };///< The Role's name.

        virtual ~RoleData() {};
    };

    /// Data structure representing a single user.
    struct DiscordCoreAPI_Dll UserData : public DiscordEntity {
        string discriminator{ "" }; ///< The # next to their User name.
        bool mfaEnabled{ false };///< MFA enabled?
        int32_t premiumType{ 0 };///< Their premium nitro status.
        int32_t publicFlags{ 0 };///< Public flags.
        bool verified{ false };///< Verified?
        string createdAt{ "" };///< When the User was created.
        string userName{ "" };///< Their username.
        bool system{ false };///< Are they a partof the system?        
        string avatar{ "" };///< Their avatar url.
        string locale{ "" };///< The region they are from/in.
        string email{ "" };///< Their email address.
        int32_t flags{ 0 };///< Flags.
        bool bot{ false };///< Are they a bot?

        virtual ~UserData() {};
    };    

    /// Attachment data. \brief Attachment data.
    struct DiscordCoreAPI_Dll AttachmentData : public DiscordEntity{
        string contentType{ "" }; ///< Type of content for the attachment.
        string filename{ "" };///< The file name of the attachment.
        string proxyUrl{ "" };///< The proxy url for the attachment.
        int32_t height{ 0 };///< The height of the attachment.
        int32_t width{ 0 };///< The width of the attachment.
        int32_t size{ 0 };///< The size of the attachment.
        string url{ "" };///< The url for the attachment.

        virtual ~AttachmentData() {};
    };

    enum class StickerFormatType {
        PNG = 1,
        APNG = 2,
        LOTTIE = 3
    };

    /// Embed footer data. \brief Embed footer data.
    struct DiscordCoreAPI_Dll EmbedFooterData {
        string proxyIconUrl{ "" };///< Proxy icon url.
        string iconUrl{ "" };///< Icon url.
        string text{ "" };///< Footer text.
    };

    /// Embed image data. \brief Embed image data.
    struct DiscordCoreAPI_Dll EmbedImageData {
        string proxyUrl{ "" };///< Proxy url.
        int32_t height{ 0 };///< Image height.
        int32_t width{ 0 };///< Image width.
        string url{ "" };///< Image url.        
    };

    /// Embed thumbnail data. \brief Embed thumbnail data.
    struct DiscordCoreAPI_Dll EmbedThumbnailData {
        string proxyUrl{ "" };///< Proxy url.
        int32_t height{ 0 };///< Image height.
        int32_t width{ 0 };///< Image width.
        string url{ "" };///< Image url.        
    };

    /// Embed video data. \brief Embed video data.
    struct DiscordCoreAPI_Dll EmbedVideoData {
        string proxyUrl{ "" };///< Proxy url.
        int32_t height{ 0 };///< Image height.
        int32_t width{ 0 };///< Image width.
        string url{ "" };///< Image url.        
    };

    /// Embed provider data. \brief Embed provider data.
    struct DiscordCoreAPI_Dll EmbedProviderData {
        string name{ "" }; ///< Name.
        string url{ "" };///< Url.
    };

    /// Embed author data.
    struct DiscordCoreAPI_Dll EmbedAuthorData {
        string proxyIconUrl{ "" };///< Proxy icon url.
        string iconUrl{ "" };///< Icon url.
        string name{ "" }; ///< Name.
        string url{ "" };///< Url.
    };

    /// Embed field data. \brief Embed field data.
    struct DiscordCoreAPI_Dll EmbedFieldData {
        bool Inline{ false };///< Is the field inline with the rest of them?
        string value{ "" };///< The text on the field.
        string name{ "" };///< The title of the field.
    };

    enum class EmbedType {
        Rich = 0,
        Image = 1,
        Video = 2,
        Gifv = 3,
        Article = 4,
        link = 5
    };

    /// Embed data. \brief Embed data.
    struct DiscordCoreAPI_Dll EmbedData {
        string hexColorValue{ "000000" };///< Hex color value of the embed.
        vector<EmbedFieldData> fields{};///< Array of embed fields.
        EmbedThumbnailData thumbnail{};///< Embed thumbnail data.
        EmbedProviderData provider{};///< Embed provider data.
        string description{ "" };///< Description of the embed.
        EmbedFooterData footer{};///< Embed footer data.
        EmbedAuthorData author{};///< Embed author data.
        string timestamp{ "" };///< Timestamp to be placed on the embed.
        EmbedImageData image{};///< Embed image data.
        EmbedVideoData video{};///< Embed video data.
        string title{ "" };///< Title of the embed.
        string type{ "" };///< Type of the embed.
        string url{ "" };///< Url for the embed.

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
        bool failIfNotExists{ false };///< Fail if the Message doesn't exist?
        string messageId{ "" };///< Id of the Message to reference.
        string channelId{ "" };///< Id of the Channel that the referenced Message was sent in.
        string guildId{ "" };///< Id of the Guild that the referenced Message was sent in.
    };

    enum class PermissionOverwritesType {
        Role = 0,
        User = 1
    };

    /// A Permission overwrite, for a given Channel. \brief A Permission overwrite, for a given Channel.
    struct DiscordCoreAPI_Dll OverWriteData : public DiscordEntity{
        PermissionOverwritesType type{};   ///< Role or User type.
        string channelId{ "" };///< Channel id for which Channel this overwrite beint64_ts to.
        string allow{ "" };///< Collection of Permissions to allow.
        string deny{ "" };///< Collection of Permissions to deny.

        virtual ~OverWriteData() {};
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

    /// Meta data for a thread type of Channel. \brief Meta data for a thread type of Channel.
    struct DiscordCoreAPI_Dll ThreadMetadataData {
        int32_t autoArchiveDuration{ 0 }; ///< How int64_t before archiving this thread.
        string archiveTimestamp{ "" }; ///< (Where applicable) the time at which this thread was archived.
        bool invitable{ false }; ///< The id of the individual who archived this thread.
        bool archived{ false }; ///< Whether or not this thread is currently archived.
        bool locked{ false }; ///< Whether or not this thread is currently locked.
    };

    /// Data for a single member of a thread. \brief Data for a single member of a thread.
    struct DiscordCoreAPI_Dll ThreadMemberData : public DiscordEntity{
        string joinTimestamp{ "" }; ///< The time at which the member joined this thread.
        string userId{ "" };    ///< The User's id.
        int32_t flags{ 0 }; ///< Flags.

        virtual ~ThreadMemberData() {};
    };

    enum class ThreadType {
        GUILD_NEWS_THREAD = 10,
        GUILD_PUBLIC_THREAD = 11,
        GUILD_PRIVATE_THREAD = 12,
    };

    enum class ThreadAutoArchiveDuration :int32_t {
        SHORTEST = 60,
        SHORT = 1440,
        LONG = 4320,
        LONGEST = 10080
    };

    /// Data structure representing a single Channel. \brief Data structure representing a single Channel.
    struct DiscordCoreAPI_Dll ChannelData : public DiscordEntity{
        map<string, OverWriteData> permissionOverwrites{}; ///< Permission overwrites for the given Channel.
        int32_t defaultAutoArchiveDuration{ 0 };
        ThreadMetadataData threadMetadata{}; ///< Metadata in the case that this Channel is a thread.
        ChannelType type{ ChannelType::DM };    ///< The type of the Channel.
        map<string, UserData> recipients{};  ///< Recipients, in the case of a group DM or DM.
        int32_t videoQualityMode{ 0 };  ///< Video quality mode.
        int32_t rateLimitPerUser{ 0 };  ///< Amount of seconds a User has to wait before sending another Message.
        string lastPinTimestamp{ "" };  ///< Timestamp of the last pinned Message.
        string lastMessageId{ "" }; ///< Id of the last Message.
        string applicationId{ "" }; ///< Application id of the current application.
        ThreadMemberData member{}; ///< Thread member object for the current User, if they have joined the thread.
        int32_t messageCount{ 0 }; ///< An approximate count of Messages in a thread, stops counting at 50.
        int32_t memberCount{ 0 };   ///< Count of members active in the Channel.
        string rtcRegion{ "" }; ///< Real-time clock region.
        int32_t userLimit{ 0 }; ///< User limit, in the case of voice channels.
        int32_t position{ 0 };  ///< The position of the Channel, in the Guild's Channel list.
        string parentId{ "" }; ///< Id of the parent Channel, if applicable.
        string guildId{ "" };  ///< Id of the Channel's Guild, if applicable.
        string ownerId{ "" };   ///< Id of the Channel's owner.
        int32_t bitrate{ 0 };   ///< Bitrate of the Channel, if it is a voice Channel.
        string topic{ "" }; ///< The Channel's topic.
        bool nsfw{ false }; ///< Whether or not it is nsfw.
        string name{ "" };  ///< Name of the Channel.
        string icon{ "" };  ///< Icon for the Channel, if applicable.

        virtual ~ChannelData() {};
    };

    struct DiscordCoreAPI_Dll VoiceData {
        string requestToSpeakTimestamp{ "" };///< The time at which the user requested to speak.
        bool selfStream{ false };///< Whether this user is streaming using "Go Live".
        bool selfVideo{ false };///< Whether this user's camera is enabled.
        bool selfDeaf{ false };///< Whether this user is locally deafened.
        bool selfMute{ false };///< Whether this user is locally muted.
        string channelId{ "" };///< The channel id this user is connected to.
        string sessionId{ "" };///< The session id for this voice state.
        bool suppress{ false };///< Whether this user is muted by the current user.
        string guildId{ "" };///< The guild id this voice state is for.
        string userId{ "" };///< The user id this voice state is for.
        bool deaf{ false };///< Whether this user is deafened by the server.
        bool mute{ false };///< Whether this user is muted by the server.
    };

    /// Data structure representing a single GuildMember. \brief Data structure representing a single GuildMember.
    struct DiscordCoreAPI_Dll GuildMemberData : DiscordEntity{
        string premiumSince{ "" };///< If applicable, when they first boosted the server.
        string permissions{ "" };///< Their base-level Permissions in the Guild.
        string userMention{ "" };///< What to enter to get them mentioned in a Message.
        vector<string> roles{}; ///< The Guild roles that they have.
        string joinedAt{ "" };///< When they joined the Guild.
        bool pending{ false };///< Are they waiting at the entry screen?
        VoiceData voiceData{};///< Info about this GuildMember's voice state.
        string guildId{ "" };///< The current Guild's id.
        bool deaf{ false };///< Are they server deafened?
        bool mute{ false };///< Are they server muted?
        string nick{ "" };///< Their nick/display name.
        UserData user{};///< User data for the current GuildMember.

        virtual ~GuildMemberData() {};
    };

    /// Voice state data. \brief Voice state data.
    struct DiscordCoreAPI_Dll VoiceStateData {
        string requestToSpeakTimestamp{ "" };///< The time at which the user requested to speak.
        GuildMemberData member{};///< The guild member this voice state is for.
        bool selfStream{ false };///< Whether this user is streaming using "Go Live".
        bool selfVideo{ false };///< Whether this user's camera is enabled.
        bool selfDeaf{ false };///< Whether this user is locally deafened.
        bool selfMute{ false };///< Whether this user is locally muted.
        string channelId{ "" };///< The channel id this user is connected to.
        string sessionId{ "" };///< The session id for this voice state.
        bool suppress{ false };///< Whether this user is muted by the current user.
        string guildId{ "" };///< The guild id this voice state is for.
        string userId{ "" };///< The user id this voice state is for.
        bool deaf{ false };///< Whether this user is deafened by the server.
        bool mute{ false };///< Whether this user is muted by the server.
        operator VoiceData() {
            VoiceData newData{};
            newData.requestToSpeakTimestamp = this->requestToSpeakTimestamp;
            newData.selfStream = this->selfStream;
            newData.selfVideo = this->selfVideo;
            newData.sessionId = this->sessionId;
            newData.channelId = this->channelId;
            newData.suppress = this->suppress;
            newData.selfDeaf = this->selfDeaf;
            newData.selfMute = this->selfMute;
            newData.guildId = this->guildId;
            newData.userId = this->userId;
            newData.deaf = this->deaf;
            newData.mute = this->mute;
            return newData;
        }
    };

    struct DiscordCoreAPI_Dll ActiveThreadsData {
        vector<ThreadMemberData> members{};
        vector<ChannelData> threads{};
        bool hasMore{ false };
    };

    struct DiscordCoreAPI_Dll ArchivedThreadsData : public ActiveThreadsData {};

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

    enum class ApplicationCommandPermissionType {
        Role = 1,
        User = 2
    };

    /// Permissions data for an ApplicationCommand. \brief Permissions data for an ApplicationCommand.
    struct DiscordCoreAPI_Dll ApplicationCommandPermissionData : public DiscordEntity{
        ApplicationCommandPermissionType type{ ApplicationCommandPermissionType::Role }; ///< The type of Permission.
        bool permission{ false }; ///< Whether the Permission is active or not.

        virtual ~ApplicationCommandPermissionData() {}
    };

    /// Represents the Permissions for accessing an ApplicationCommand from within a Guild. \brief Represents the Permissions for accessing an ApplicationCommand from within a Guild.
    struct DiscordCoreAPI_Dll GuildApplicationCommandPermissionsData : public DiscordEntity{
        vector<ApplicationCommandPermissionData> permissions{};///< The Permissions.
        string applicationId{ "" };///< The application's id.
        string guildId{ "" };///< The Guild's id.

        virtual ~GuildApplicationCommandPermissionsData() {}
    };

    /// Data structure representing a single emoji. \brief Data structure representing a single emoji.
    struct DiscordCoreAPI_Dll EmojiData : public DiscordEntity{
        bool requireColons{ false };///< Require colons to render it?
        vector<RoleData> roles{};///< Roles that are allowed to use this emoji.
        bool available{ true };///< Is it available to be used?
        bool animated{ false };///< Is it animated?
        bool managed{ false };///< Is it managed?
        string name{ "" };///< What is its name?
        UserData user{};///< User that created this emoji.

        virtual ~EmojiData() {};
    };

    /// For updating/modifying a given Channel's properties.
    struct DiscordCoreAPI_Dll UpdateChannelData {
        map<string, OverWriteData> permissionOverwrites{};
        int32_t defaultAutoArchiveDuration{ 10080 };
        int32_t videoQualityMode{ 1 };
        int32_t rateLimitPerUser{ 0 };
        int32_t bitrate{ 48000 };
        int32_t userLimit{ 0 };
        string parentId{ "" };
        string rtcRgion{ "" };
        int32_t position{ 0 };
        ChannelType type{};
        string topic{ "" };
        bool nsfw{ false };
        string name{ "" };
    };

    /// Data structure representing a single reaction.
    struct DiscordCoreAPI_Dll ReactionData : public DiscordEntity{
        GuildMemberData member{}; ///< The GuildMember who placed the reaction.
        string channelId{ "" }; ///< The id of the Channel where it was placed.
        string messageId{ "" }; ///< The id of the Message upon which it was placed.
        string guildId{ "" };///< The id of the Guild where it was placed.
        string userId{ "" }; ///< The id of the User who placed the reaction.
        int32_t count{ 0 };///< The number of times this particular emoji was placed as a reaction to the given Message.
        EmojiData emoji{}; ///< The emoji that was placed as a reaction.
        bool me{ false }; ///< Whether or not I (The bot) placed it.

        virtual ~ReactionData() {};
    };

    /// Structure representing Voice Region Data.
    struct DiscordCoreAPI_Dll VoiceRegionData {
        bool deprecated{ false };///< Whether this is a deprecated voice region(avoid switching to these).
        bool optimal{ false };///< True for a single server that is closest to the current user's client.
        bool custom{ false };///< Whether this is a custom voice region(used for events / etc).
        string name{ "" };///< Name of the region.
        string id{ "" };///< Unique ID for the region.
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
        vector<string> permissions{};///< Permissions for the team.
        int32_t membershipState{ 0 };///< Current state.
        string teamId{ "" };///< Id of the current team.
        UserData user{};///< User data of the current User.
    };

    /// Team object data. \brief Team object data.
    struct DiscordCoreAPI_Dll TeamObjectData : public DiscordEntity{
        vector<TeamMembersObjectData> members{};///< Array of team members object data.
        string ownerUserId{ "" };///< User id of the team owner.
        string icon{ "" };///<Icon for the team.

        virtual ~TeamObjectData() {};
    };

    /// Application data. \brief Application data.
    struct DiscordCoreAPI_Dll ApplicationData : public DiscordEntity{
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
        int32_t flags{ 0 };///< Application flags.
        string slug{ "" };///< Sluhg.
        string name{ "" };///< Application's name.
        string icon{ "" };///< Application's icon.
        UserData owner{};///< Application's owner.

        virtual ~ApplicationData() {};
    };

    /// Account data. \brief Account data.
    struct DiscordCoreAPI_Dll AccountData : public DiscordEntity{
        string name{ "" };///< Name of the account.
        virtual ~AccountData() {}
    };

    /// Guild Widget Data.
    struct DiscordCoreAPI_Dll GuildWidgetData {
        string channelId{ "" };///< The widget channel id.
        bool enabled{ false };///< Whether the widget is enabled.
    };

    enum class WidgetStyleOptions {
        Shield = 0,
        Banner1 = 1,
        Banner2 = 2,
        Banner3 = 3,
        Banner4 = 4
    };

    struct DiscordCoreAPI_Dll GuildWidgetImageData {
        string url{ "" };
    };

    /// Integration data. \brief Integration data.
    struct DiscordCoreAPI_Dll IntegrationData : public DiscordEntity{
        int32_t expireGracePeriod{ 0 };///< How int64_t before the integration expires.
        int32_t subscriberCount{ 0 };///< Number of current subscribers.
        ApplicationData application{};///<Application data.
        bool enableEmoticons{ true };///<Emoticons enabled?
        int32_t expireBehavior{ 0 };///< What to do upon expiry.
        bool enabled{ false };///< Enabled?
        bool syncing{ false };///< Is it syncing?
        AccountData account{};///< Account data.
        string syncedAt{ "" };///< Time it was last synced at.
        bool revoked{ false };///< Has it been revoked?
        string roleId{ "" };///< Role Id.
        string name{ "" };///< Name of the integration.
        string type{ "" };///< Type of integration.
        UserData user{};///< User data for the integration.

        virtual ~IntegrationData() {}
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

    /// Audit log entry info data \brief Audit log entry info data.
    struct DiscordCoreAPI_Dll AuditLogEntryInfoData : public DiscordEntity{
        string deleteMemberDays{ "" };///<Number of days for which the member's Messages were deleted.
        string membersRemoved{ "" };///< Number of members that were removed upon a prune.
        string channelId{ "" };///< Channel Id.
        string messageId{ "" };///< Message Id.
        string roleName{ "" };///< Role name.
        string count{ "" };///<Count.
        string type{ "" };///< Type.

        virtual ~AuditLogEntryInfoData() {}
    };

    /// Audit log change data. \brief Audit log change data.
    struct DiscordCoreAPI_Dll AuditLogChangeData {
        string newValueString{ "" };///< New value, if it's a string.
        string oldValueString{ "" };///< Old value, if it's a string.
        bool newValueBool{ false };///< New value, if it's a bool.
        bool oldValueBool{ false };///< Old value, if it's a bool.
        int32_t newValueInt{ 0 };///< New value, if it's an int32_t.
        int32_t oldValueInt{ 0 };///< Old value, if it's an int32_t.
        string key{ "" };///< The key of the audit log change.
    };

    /// Guild prune count data.
    struct DiscordCoreAPI_Dll GuildPruneCountData {
        int32_t count{ 0 };
    };

    /// Audit log entry data. \brief Audit log entry data.
    struct DiscordCoreAPI_Dll AuditLogEntryData : public DiscordEntity{
        vector<AuditLogChangeData> changes{};///< Array of audit log change data.
        TimeStamp createdTimeStamp{ "" };///< Time at which this entry was created.
        AuditLogEntryInfoData options{};///< Audit log entry info data.
        AuditLogEvent actionType{};///< Audit log action type.
        string targetId{ "" };///< Id of the target User.
        string userId{ "" };///< Id of the executing User.
        string reason{ "" };///< The reason that was entered for the given change.

        virtual ~AuditLogEntryData() {}
    };

    /// Party data. \brief Party data.
    struct DiscordCoreAPI_Dll PartyData : public DiscordEntity{
        vector<int32_t> size{ 0, 0 };///< The size of the party.

        virtual ~PartyData() {}
    };

    /// Assets data. \brief Party data.
    struct DiscordCoreAPI_Dll AssetsData {
        string largeImage{ "" };    ///< Keyname of an asset to display.
        string smallImage{ "" };    ///< Keyname of an asset to display.
        string largeText{ "" };     ///< Hover text for the large image.
        string smallText{ "" };     ///< Hover text for the small image.
    };

    /// Secrets data. \brief Secrets data.
    struct DiscordCoreAPI_Dll SecretsData {
        string spectate{ "" }; ///< Unique hash for the given match context.
        string match{ "" };	///< Unique hash for Spectate button.
        string join{ "" };	///< Unique hash for chat invitesand Ask to Join.
    };

    /// Timestamp data. \brief Timestamp data.
    struct DiscordCoreAPI_Dll TimestampData {
        int64_t start{ 0 }; ///< Unix timestamp - Send this to have an "elapsed" timer.
        int64_t end{ 0 };   ///< Unix timestamp - send this to have a "remaining" timer.
    };

    /// Button data. \brief Button data.
    struct DiscordCoreAPI_Dll ButtonData {
        string label{ "" }; ///< Visible label of the button.
        string url{ "" };   ///< URL to display on the button.
    };

    enum class ActivityType {
        Game = 0,
        Streaming = 1,
        Listening = 2,
        Watching = 3,
        Custom = 4,
        Competing = 5
    };

    /// Activity data. \brief Activity data.
    struct DiscordCoreAPI_Dll ActivityData {
        TimestampData timestamps{}; ///< Timestamp data.
        string applicationId{ "" }; ///< Application id for the current application.
        int32_t createdAt{ 0 };     ///< Timestamp of when the activity began.
        bool instance{ false }; ///< Whether this activity is an instanced context, like a match.
        SecretsData secrets{}; ///< Secrets data.
        string details{ "" };   ///< Details about the activity.
        ButtonData buttons{};   ///< Button Data.
        ActivityType type{};    ///< Activity data.
        AssetsData assets{};    ///< Assets data.
        int32_t flags{ 0 }; ///< Flags.
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

    enum class PremiumTier {
        NONE = 0,
        TIER_1 = 1,
        TIER_2 = 2,
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

    enum class StageInstancePrivacyLevel {
        PUBLIC = 1,
        GUILD_ONLY = 2
    };

    /// Stage instance data. \brief Stage instance data.
    struct DiscordCoreAPI_Dll StageInstanceData : public DiscordEntity{
        StageInstancePrivacyLevel privacyLevel{ 0 };  ///< Privacy level of the Channel.
        bool discoverableDisabled{ false }; ///< Is it discoverable?
        string channelId{ "" }; ///< The Channel's id.
        string guildId{ "" };   ///< The Guild id for which the Channel exists in.
        string topic{ "" }; ///< The topic of the stage instance.

        virtual ~StageInstanceData() {}
    };

    enum class StickerType {
        STANDARD = 1,
        GUILD = 2
    };

    /// Data representing a single Sticker. \brief Data representing a single Sticker.
    struct DiscordCoreAPI_Dll StickerData : public DiscordEntity{
        StickerFormatType formatType{}; ///< Format type.
        string description{ "" };   ///< Description of the sticker.
        bool available{ false };    ///< Is it available for use?
        int32_t nsfwLevel{ 0 }; ///< NSFW warning level.
        int32_t sortValue{ 0 };///< Where in the stack of stickers it resides.
        string guildId{ "" };   ///< The Guild id for which the sticker exists in.
        string packId{ "" };    ///< Pack id of the sticker.    
        string asset{ "" }; ///< Asset value for the sticker.
        StickerType type{}; ///< The type of sticker.
        string name{ "" }; ///< The sticker's name.
        string tags{ "" };  ///< Tags for the sticker to use.
        UserData user{};    ///< The user that uploaded the Guild sticker.

        virtual ~StickerData() {};
    };

    /// Data representing a single Guild preview. \brief Data representing a single Guild preview.
    struct DiscordCoreAPI_Dll GuildPreviewData {
        int32_t approximatePresenceCount{ 0 };
        int32_t approximateMemberCount{ 0 };
        string discoverySplash{ "" };
        vector<EmojiData> emojis{};
        vector<string> features{};
        string description{ "" };
        string splash{ "" };
        string name{ "" };
        string icon{ "" };
        string id{ "" };
    };

    enum class AfkTimeOutDurations {
        SHORTEST = 60,
        SHORT = 300,
        MEDIUM = 900,
        LONG = 1800,
        LONGEST = 3600
    };

    /// Data structure representing a single Guild. \brief Data structure representing a single Guild.
    struct DiscordCoreAPI_Dll GuildData : public DiscordEntity {
        DefaultMessageNotificationLevel defaultMessageNotifications{};///<Default Message notification level.
        ExplicitContentFilterLevel explicitContentFilter{}; ///< Explicit content filtering level, by default.
        map<string, PresenceUpdateData> presences{}; ///< Array of presences for each GuildMember.
        vector<StageInstanceData> stageInstances{}; ///< Array of stage instances.
        map<string, VoiceStateData> voiceStates{};///< Array of Guild-member voice-states.
        map<string, GuildMemberData> members{};  ///< Array of GuildMembers.
        int32_t premiumSubscriptionCount{ 0 }; ///< Premium subscription count.
        int32_t approximatePresenceCount{ 0 }; ///< Approximate quantity of presences.
        VerificationLevel verificationLevel{};  ///< Verification level required.
        map<string, StickerData> stickers{}; ///< Array of Guild stickers.
        map<string, ChannelData> channels{}; ///< Array of Guild channels.
        string publicUpdatesChannelId{ "" }; ///< Id of the public updates Channel.        
        int32_t approximateMemberCount{ 0 };    ///< Approximate member count.
        map<string, ChannelData> threads{};  ///< Array of Guild threads.        
        WelcomeScreenData welcomeScreen{};  ///< Welcome screen for the Guild.
        int32_t maxVideoChannelUsers{ 0 };  ///< Maximum quantity of users per video Channel.
        AfkTimeOutDurations afkTimeOut{};    ///< Time for an individual to time out as afk.
        int32_t systemChannelFlags{ 0 };    ///< System Channel flags.
        map<string, EmojiData> emoji{};  ///< Array of Guild emojis.
        map<string, RoleData> roles{};   ///< Array of Guild roles.
        string discoverySplash{ "" };   ///< Link to the discovery image's splash.
        string preferredLocale{ "" };   ///< Preferred locale, for voice chat servers.
        string widgetChannelId{ "" };   ///< Channel id for the Guild's widget.
        string systemChannelId{ "" };   ///< Channel id for the Guild's system Channel.
        string rulesChannelId{ "" };    ///< Channel id for the Guild's rules Channel.
        bool widgetEnabled{ false };    ///< Is the widget enabled?        
        string vanityURLCode{ "" }; ///< Vanity URL code, if applicable.
        string applicationId{ "" }; ///< The current application id.
        int32_t maxPresences{ 0 };  ///< Max number of presences allowed.
        string afkChannelId{ "" };  ///< Channel if of the "afk" Channel,
        vector<string> features{};  ///< List of Guild features.
        bool unavailable{ false };  ///< Is the Guild currently available to the bot?
        PremiumTier premiumTier{};  ///< What is the premium tier?
        string permissions{ "" };   ///< Current Permissions for the bot in the Guild.
        string description{ "" };   ///< Description of the Guild.
        int32_t memberCount{ 0 };   ///< Member count.
        int32_t maxMembers{ 0 };    ///< Max quantity of members.
        int32_t nsfwLevel{ 0 }; ///< NSFW warning level.
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

        virtual ~GuildData() {};
    };

    enum class GuildScheduledEventPrivacyLevel {
        PUBLIC = 1,
        GUILD_ONLY = 2
    };

    enum class GuildScheduledEventStatus {
        SCHEDULED = 1,
        ACTIVE = 2,
        COMPLETED = 3,
        CANCELED = 4
    };

    enum class GuildScheduledEventEntityType {
        NONE = 0,
        STAGE_INSTANCE = 1,
        VOICE = 2,
        EXTERNAL = 3
    };

    /// Guild scheduled event entity metadata. \brief Guild scheduled event entity metadata.
    struct DiscordCoreAPI_Dll GuildScheduledEventMetadata {
        string location{ "" };
    };

    /// Data representing a Guild Scheduled Event. \brief Data representing a Guild Scheduled Event.
    struct DiscordCoreAPI_Dll GuildScheduledEventData : public DiscordEntity {
        GuildScheduledEventPrivacyLevel privacyLevel{};///< The privacy level of the scheduled event.
        GuildScheduledEventMetadata entityMetadata{};///< Additional metadata for the guild scheduled event.
        GuildScheduledEventEntityType entityType{};///< The type of the scheduled event.
        GuildScheduledEventStatus status{};///< The status of the scheduled event.
        string scheduledStartTime{ "" };///< The time the scheduled event will start.
        string scheduledEndTime{ "" };///< The time the scheduled event will end, required if entity_type is EXTERNAL.
        string description{ "" };///< The description of the scheduled event(1 - 1000 characters).
        uint32_t userCount{ 0 };///< The number of users subscribed to the scheduled event.
        string channelId{ "" };///< The channel id in which the scheduled event will be hosted, or null if scheduled entity type is EXTERNAL.
        string creatorId{ "" };///< The id of the user that created the scheduled event *.
        string entityId{ "" };///< The id of an entity associated with a guild scheduled event.
        string guildId{ "" };///< The guild id which the scheduled event belongs to.
        UserData creator{};///< The user that created the scheduled event.
        string name{ "" };///< The name of the scheduled event(1 - 100 characters).

        virtual ~GuildScheduledEventData() {};
    };

    /// Data representing a single GuildScheduledEventUser. \brief Data representing a single GuildScheduledEventUser.
    struct DiscordCoreAPI_Dll GuildScheduledEventUserData {
        string guildScheduledEventId{ "" };///< The scheduled event id which the user subscribed to/
        GuildMemberData member{};///< Guild member data for this user for the guild which this event belongs to, if any.
        UserData user{};///< User which subscribed to an event.
    };    

    /// Invite data. \brief Invite data.
    struct DiscordCoreAPI_Dll InviteData {
        GuildScheduledEventData guildScheduledEvent{};///< Scheduled Guild event.
        int32_t approximatePresenceCount{ 0 };///< Approximate presence count.
        int32_t approximateMemberCount{ 0 };///< Approximate member count.
        ApplicationData targetApplication{};///< Application data.
        StageInstanceData stageInstance{};///< Stage instance data.        
        int32_t targetType{ 0 };///< Target type.
        bool temporary{ false };///< Is it temporary?
        string createdAt{ "" };///< Time it was created at.
        string expiresAt{ "" };///< When the invite expires.
        UserData targetUser{};///< Target User of the invite.
        ChannelData channel{};///< Channel data of the Channel that the invite is for.
        string guildId{ "" };///< The guild this invite is for.
        int32_t maxUses{ 0 };///< Max number of uses.
        int32_t maxAge{ 0 };///< Maximum age of the invite.
        UserData inviter{};///< The User who created the invite.
        string code{ "" };///< Unique invite code.
        GuildData guild{};///< Guild data of the Channel that the invite is for.
        int32_t uses{ 0 };///< Maximum nunmber of uses.
    };

    /// Represents a Guild Template. \brief Represents a Guild Template.
    struct DiscordCoreAPI_Dll GuildTemplateData {
        GuildData serializedSourceGuild{};///< The guild snapshot this template contains.
        string sourceGuildId{ "" };///< The ID of the guild this template is based on.
        string description{ "" };///< The description for the template.
        uint32_t usageCount{ 0 };///< Number of times this template has been used.
        string creatorId{ "" };///< The ID of the user who created the template.
        string createdAt{ "" };///< When this template was created.
        string updatedAt{ "" };///< When this template was last synced to the source guild.
        UserData creator{};///< The user who created the template.
        bool isDirty{ false };///< Whether the template has unsynced changes.
        string code{ "" };///< The template code(unique ID).
        string name{ "" };///< Template name.
    };

    enum class InviteTargetTypes {
        STREAM = 1,
        EMBEDDED_APPLICATION = 2
    };

    /// Webhook data. \brief Webhook data.
    struct DiscordCoreAPI_Dll WebhookData : public DiscordEntity{
        ChannelData sourceChannel{};///< Channel for which the webhook was issued.
        string applicationId{ "" };///< Application id.
        GuildData sourceGuild{};///< Source Guild id.
        string channelId{ "" };///< Channel id for which the webhook was issued.
        string guildId{ "" };///< Guild id for which the webhook was issued.
        string avatar{ "" };///< Avatar of the webhook.
        string token{ "" };///< Token of the webhook.
        int32_t type{ 0 };///< Type of webhook.
        string name{ "" };///< Name of the webhook.
        string url{ "" };///< Url of the webhook.
        UserData user{};///< User which create the webhook.

        virtual ~WebhookData() {}
    };

    /// Audit log data. \brief Audit log data.
    class DiscordCoreAPI_Dll AuditLogData {
    public:
        auto getAuditLogData(string userIdOfChanger, AuditLogEvent auditLogType) {
            for (auto& value : this->auditLogEntries) {
                if (value.id == userIdOfChanger && value.actionType == auditLogType) {
                    return value;
                }
            }
            return AuditLogEntryData();
        }
        auto getAuditLogData(AuditLogEvent auditLogType, string userIdOfTarget) {
            for (auto& value : this->auditLogEntries) {
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
        string valueString{ "" }; ///< The value, if the given choice is a string.
        float valueFloat{ 0.0f }; ///< The value, if the given choice is a float.
        int32_t	valueInt{ 0 }; ///< The value, if the given choice is an int32_t.
        string name{ "" };  ///< The name of the current choice.        
    };

    /// Data structure representing an ApplicationCommand's option. \brief Data structure representing an ApplicationCommand's option.
    struct DiscordCoreAPI_Dll ApplicationCommandOptionData {
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
        int32_t timestamp{ 0 };
        string channelId{ "" };
        string guildId{ "" };
        string userId{ "" };
    };

    struct DiscordCoreAPI_Dll YouTubeFormat {
        string signatureCipher{ "" };
        string audioSampleRate{ "" };
        int32_t averageBitrate{ 0 };
        int64_t contentLength{ 0 };
        string audioQuality{ "" };
        string downloadURL{ "" };
        string signature{ "" };
        string mimeType{ "" };
        int32_t bitrate{ 0 };
        string quality{ "" };
        string codecs{ "" };
        string aitags{ "" };
        int32_t height{ 0 };
        int32_t width{ 0 };
        int32_t itag{ 0 };
        int32_t fps{ 0 };
    };

    enum class ApplicationCommandType {
        CHAT_INPUT = 1,
        USER = 2,
        MESSAGE = 3
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
        bool repliedUser{ false };///< For replies, whether to mention the author of the Message being replied to (default false).
        vector<string> parse{}; ///< A vector of allowed mention types to parse from the content.
        vector<string> roles{}; ///< Array of role_ids to mention (Max size of 100)
        vector<string> users{}; ///<Array of user_ids to mention (Max size of 100)
    };

    enum class InteractionType {
        PING = 1,
        APPLICATION_COMMAND = 2,
        MESSAGE_COMPONENT = 3
    };

    /// Represents a single selection from a select-menu. \brief Represents a single selection from a select-menu.
    struct DiscordCoreAPI_Dll SelectOptionData {
        string description{ "" };///< Description of the select-menu-option.
        bool _default{ false };///<Is it the default option?
        string label{ "" };///< A visible label for the select-menu-option.
        string value{ "" };///< A value for identifying the option.
        EmojiData emoji{};///< An optional emoji to put on it.
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

    /// Represents a single Message-component. \brief Represents a single Message-component.
    struct DiscordCoreAPI_Dll ComponentData {
        vector<SelectOptionData> options{}; ///< A vector of select-options, in the case of this being a select-menu.
        string placeholder{ "" };   ///< Custom placeholder text if nothing is selected, max 100 characters.
        bool disabled{ false }; ///< Whether this component is active.
        string customId{ "" };  ///< A custom id for identifying the component.
        ComponentType type{};   ///< Which type of component?
        ButtonStyle style{};    ///< Which style of button?
        string label{ "" }; ///< What is the visible label?
        int32_t maxValues{ 0 }; ///< Maximum number of selectable options?
        int32_t minValues{ 0 }; ///< Minimum number of required selectable options?
        EmojiData emoji{}; ///< An emoji to put on the component.
        string url{ "" }; ///< An optional URL to place.
    };

    /// Action row data of Message components. \brief Action row data of Message components.
    struct DiscordCoreAPI_Dll ActionRowData {
        vector<ComponentData> components{}; ///< Array of components to make up the action-row.
    };

    enum class InteractionCallbackType {
        Pong = 1,
        ChannelMessageWithSource = 4,
        DeferredChannelMessageWithSource = 5,
        DeferredUpdateMessage = 6,
        UpdateMessage = 7
    };

    /// Interaction ApplicationCommand callback data. \brief Interaction ApplicationCommand callback data.
    struct DiscordCoreAPI_Dll InteractionApplicationCommandCallbackData {
        AllowedMentionsData allowedMentions{};///< Allowed mentions data.
        vector<ActionRowData> components{};///< Message components.
        vector<EmbedData> embeds{};///< Message embeds.
        string content{ "" };///< Message content.
        bool tts{ false };///< Is it TTS?
        int32_t flags{ 0 };///< Flags.
    };

    /// Interaction response data. \brief Interaction response data.
    struct DiscordCoreAPI_Dll InteractionResponseData {
        InteractionApplicationCommandCallbackData data{}; ///< Interaction ApplicationCommand callback data.
        InteractionCallbackType type{};///< Interaction callback type.
    };

    /// Data structure representing an ApplicationCommand. \brief Data structure representing an ApplicationCommand.
    struct DiscordCoreAPI_Dll ApplicationCommandData : public DiscordEntity{
        vector<ApplicationCommandOptionData> options{};///< A vector of possible options for the current ApplicationCommand.
        bool defaultPermission{ false };///< Whether or not the default Permission in the Guild is to have access to this command or not.
        ApplicationCommandType type{};///< The type of ApplicationCommand.
        string applicationId{ "" };///< The current application id.
        string description{ "" };///< A description of the current ApplicationCommand.
        string guildId{ "" };///< (Where applicable) a Guild id for which to assign this ApplicationCommand to.
        string version{ "" };///< An autoincremented version.
        string name{ "" };///< Name of the current ApplicationCommand.

        virtual ~ApplicationCommandData() {}
    };

    /// Function data for repeated functions to be loaded. \brief Function data for repeated functions to be loaded.
    struct DiscordCoreAPI_Dll RepeatedFunctionData {
        function<void()> function{ nullptr };///< The function pointer to be loaded.
        bool repeated{ false }; ///< Whether or not the function is repeating.
        int32_t intervalInMs{ 0 };  ///< The time interval at which to call the function.
    };

    /// Channel mention data. \brief Channel mention data.
    struct DiscordCoreAPI_Dll ChannelMentionData : public DiscordEntity{
        string guildId{ "" }; ///< The id of the Guild where it took place.
        string name{ "" }; ///< The name of the Channel that was mentioned.
        ChannelType type{}; ///< The type of Channel that was mentioned.

        virtual ~ChannelMentionData() {}
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
    struct DiscordCoreAPI_Dll ThreadMembersUpdateData : public DiscordEntity{
        vector<ThreadMemberData> addedMembers{}; ///< New members added to the thread.
        vector<string> removedMemberIds{}; ///< Members who have been removed.
        string guildId{ "" };   ///< Guild id of the thread.
        int32_t memberCount{ 0 };///< Number of Guild-members in the thread.

        virtual ~ThreadMembersUpdateData() {}
    };

    /// Message interaction data.
    struct DiscordCoreAPI_Dll MessageInteractionData : public DiscordEntity{
        InteractionType type{};
        string name{ "" };
        UserData user{};

        virtual ~MessageInteractionData() {}
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
        CHAT_INPUT_COMMAND = 20,
        THREAD_STARTER_MESSAGE = 21,
        GUILD_INVITE_REMINDER = 22,
        CONTEXT_MENU_COMMAND = 23
    };

    enum class MessageFlags {
        CROSSPOSTED = 1ull << 0,
        IS_CROSSPOST = 1ull << 1,
        SUPPRESS_EMBEDS = 1ull << 2,
        SOURCE_MESSAGE_DELETED = 1ull << 3,
        URGENT = 1ull << 4,
        HAS_THREAD = 1ull << 5,
        EPHEMERAL = 1ull << 6,
        LOADING = 1ull << 7
    };

    enum class StickerItemType {
        PNG = 1,
        APNG = 2,
        LOTTIE = 3
    };

    /// Message sticker item data. \brief Message sticker item data.
    struct DiscordCoreAPI_Dll StickerItemData : public DiscordEntity{
        StickerItemType formatType{}; ///< Message sticker item type.
        string name{ "" }; ///< The name of the sticker.

        virtual ~StickerItemData() {}
    };

    /// The core of a Message's data structure. \brief The core of a Message's data structure.
    struct DiscordCoreAPI_Dll MessageDataOld : public DiscordEntity{
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
        int32_t flags{ 0 };///< Flags.

        virtual ~MessageDataOld() {};

    };

    /// Data structure representing a single Message. \brief Data structure representing a single Message.
    struct DiscordCoreAPI_Dll MessageData : public MessageDataOld{
        MessageDataOld referencedMessage{}; ///< The referenced Message, to reply to.
        string requesterId{ "" }; ///< Requester's id, of who sent this Message.

        virtual ~MessageData() {};
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

    DiscordCoreAPI_Dll vector<ApplicationCommandInteractionDataOption> convertAppCommandInteractionDataOptions(vector<ApplicationCommandInteractionDataOption> originalOptions);

    /// Represents a Sticker pack. \brief Represents a Sticker pack.
    struct StickerPackData {
        vector<StickerData> stickers{};///< Array of sticker objects	the stickers in the pack.
        string coverStickerId{ "" };///< Id of a sticker in the pack which is shown as the pack's icon.
        string bannerAssetId{ "" };///< Id of the sticker pack's banner image.
        string description{ "" };///< Description of the sticker pack.
        string skuId{ "" };///< Id of the pack's SKU.
        string name{ "" };///< Name of the sticker pack.
        string Id{ "" };///< Id of the sticker pack.
    };
    

    /// ApplicationCommand interaction data option. \brief ApplicationCommand interaction data option.
    struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption {
        vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand interaction data options.
        ApplicationCommandOptionType type{};///< The type of ApplicationCommand options.
        string valueString{ "" };///< The value if it's a string.
        bool valueBool{ false };///< the value if it's a bool.
        string name{ "" };///< The name of the current option.
        int32_t valueInt{ 0 };///< The value if it's an int32_t.
    };

    /// ApplicationCommand interaction data.
    struct DiscordCoreAPI_Dll ApplicationCommandInteractionData : public DiscordEntity{
        vector<ApplicationCommandInteractionDataOption> options{};///< ApplicationCommand interaction data options.
        ApplicationCommandType type{}; ///< The type of ApplicationCommand.
        ResolvedData resolved{};///< Resolved data.
        string name{ "" };///< The name of the command.

        virtual ~ApplicationCommandInteractionData() {}
    };

    /// Interaction data data. \brief Interaction data data.
    struct DiscordCoreAPI_Dll InteractionDataData {
        ApplicationCommandInteractionData applicationCommanddata{};///< ApplicationCommand interaction data.
        MessageCommandInteractionData messageInteractionData{};///< Message command interaction data.
        UserCommandInteractionData userInteractionData{};///< User command interaction data.
        ComponentInteractionData componentData{};///< Component interaction data.
    };

    /// Interaction data. \brief Interaction data.
    struct DiscordCoreAPI_Dll InteractionData : public DiscordEntity{
        InteractionDataData data{};///< The interaction's data.
        string applicationId{ "" };///< The application's id.
        GuildMemberData member{};///< The data of the Guild member who sent the interaction, if applicable.
        string requesterId{ "" };///< The id of the sender of the interaction.
        InteractionType type{};///< The type of interaction.
        string channelId{ "" };///< The Channel the interaction was sent in.
        MessageData message{};///< The Message that the interaction came through on, if applicable.
        int32_t version{ 0 };///< The interaction version.
        string guildId{ "" };///< The Guild id of the Guild it was sent in.
        string token{ "" };///< The interaction token.
        UserData user{};///< The user data of the sender of the interaction.
        json rawData{};///< The interaction's raw data.
        InteractionData() {};
        InteractionData(string requesterId) {
            this->requesterId = requesterId;
        }

        virtual ~InteractionData() {}
    };

    /**@}*/

    /**
    * \addtogroup voice_connection
    * @{
    */

    /// Represents a single frame of raw audio data. \brief Represents a single frame of raw audio data.
    struct DiscordCoreAPI_Dll RawFrameData {
        vector<uint8_t> data{};///< The audio data.
        int32_t sampleCount{ -1 };///< The number of samples per this frame.
    };

    /// Represents a single frame of encoded audio data. \brief Represents a single frame of encoded audio data.
    struct DiscordCoreAPI_Dll EncodedFrameData {
        vector<uint8_t> data{};///< The audio data.
        int32_t sampleCount{ -1 };///< The number of samples per this frame.
    };

    enum class AudioFrameType {
        Unset = 0,
        Encoded = 1,
        RawPCM = 2,
        Cancel = 3
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
        int32_t contentSize{ 0 };
        string urlPath{ "" };
    };

    /**
    * \addtogroup foundation_entities
    * @{
    */

    enum class InputEventResponseType {
        Unset = 0,
        DeferredResponse = 1,
        InteractionResponse = 2,
        InteractionResponseEdit = 3,
        EphemeralInteractionResponse = 4,
        RegularMessage = 5,
        RegularMessageEdit = 6,
        FollowUpMessage = 7,
        FollowUpMessageEdit = 8,
        EphemeralFollowUpMessage = 9,
        SendDM = 10
    };

    enum class InputEventType {
        SLASH_COMMAND_INTERACTION = 1,
        BUTTON_INTERACTION = 2,
        REGULAR_MESSAGE = 3,
        SELECT_MENU_INTERACTION = 4,
        MESSAGE_COMMAND_INTERACTION = 5,
        USER_COMMAND_INTERACTION = 6
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
        int32_t chunkIndex{ 0 };
        int32_t chunkCount{ 0 };
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
        friend struct CommandData;
        friend class EventHandler;
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

        virtual ~InputEventData() {};

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
        friend struct CreateMessageData;
        friend struct EditMessageData;
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
            this->requesterId = dataPackage.requesterId;
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
        void addButton(bool disabled, string customId, string buttonLabel, ButtonStyle buttonStyle, string emojiName = "", string emojiId = "", string url = "") {
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
        void addSelectMenu(bool disabled, string customId, vector<SelectOptionData> options, string placeholder, int32_t maxValues, int32_t minValues) {
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
        int32_t flags{ 0 };
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

        Song() {};

        virtual ~Song() {};

    protected:

        operator SoundCloudSong();

        operator YouTubeSong();

        vector<DownloadURL> finalDownloadURLs{};
        string secondDownloadURL{ "" };
        string firstDownloadURL{ "" };
        int32_t contentLength{ 0 };
        string songId{ "" };

    };

    /// Song completion event data. \brief Song completion event data.
    struct DiscordCoreAPI_Dll SongCompletionEventData {
        VoiceConnection* voiceConnection{}; ///< A pointer to the current VoiceConnection.
        bool isThisAReplay{ false };///< Is this a replay? (Did a track recently fail to play?)
        Song previousSong{};///< The previously played Song.
    };

    /**@}*/

    /**
    * \addtogroup foundation_entities
    * @{
    */

    enum class Permissions : int64_t {
        CREATE_INSTANT_INVITE = 0x0000000001,
        KICK_MEMBERS = 0x0000000002,
        BAN_MEMBERS = 0x0000000004,
        ADMINISTRATOR = 0x0000000008,
        MANAGE_CHANNELS = 0x0000000010,
        MANAGE_GUILD = 0x0000000020,
        ADD_REACTIONS = 0x0000000040,
        VIEW_AUDIT_LOG = 0x0000000080,
        PRIORITY_SPEAKER = 0x0000000100,
        STREAM = 0x0000000200,
        VIEW_CHANNEL = 0x0000000400,
        SEND_MESSAGES = 0x0000000800,
        SEND_TTS_MESSAGES = 0x0000001000,
        MANAGE_MESSAGES = 0x0000002000,
        EMBED_LINKS = 0x0000004000,
        ATTACH_FILES = 0x0000008000,
        READ_MESSAGE_HISTORY = 0x0000010000,
        MENTION_EVERYONE = 0x0000020000,
        USE_EXTERNAL_EMOJIS = 0x0000040000,
        VIEW_GUILD_INSIGHTS = 0x0000080000,
        CONNECT = 0x0000100000,
        SPEAK = 0x0000200000,
        MUTE_MEMBERS = 0x0000400000,
        DEAFEN_MEMBERS = 0x0000800000,
        MOVE_MEMBERS = 0x0001000000,
        USE_VAD = 0x0002000000,
        CHANGE_NICKNAME = 0x0004000000,
        MANAGE_NICKNAMES = 0x0008000000,
        MANAGE_ROLES = 0x0010000000,
        MANAGE_WEBHOOKS = 0x0020000000,
        MANAGE_EMOJIS_AND_STICKERS = 0x0040000000,
        USE_APPLICATION_COMMANDS = 0x0080000000,
        REQUEST_TO_SPEAK = 0x0100000000,
        MANAGE_THREADS = 0x0400000000,
        CREATE_PUBLIC_THREADS = 0x0800000000,
        CREATE_PRIVATE_THREADS = 0x1000000000,
        USE_EXTERNAL_STICKERS = 0x2000000000,
        SEND_MESSAGES_IN_THREADS = 0x4000000000,
        START_EMBEDDED_ACTIVITIES = 0x8000000000
    };
    /**@}*/

    struct DiscordCoreAPI_Dll CacheOptions {
        bool cacheGuildMembers{ false };
        bool cacheChannels{ false };
        bool cacheGuilds{ false };
        bool cacheRoles{ false };
        bool cacheUsers{ false };
    };

    class DiscordCoreAPI_Dll YouTubeSong : public Song{
    public:

        friend class DiscordCoreInternal::DataParser;
        friend class SoundCloudAPI;
        friend class YouTubeAPI;
        friend class SongAPI;
        friend struct Song;

        operator Song();

        YouTubeSong collectFinalSong(GuildMemberData addedByGuildMember, YouTubeSong newSong);

        vector<YouTubeSong> searchForSong(string songQuery);

        virtual ~YouTubeSong() {};

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

    class DiscordCoreAPI_Dll SoundCloudSong : public Song{
    public:

        friend class DiscordCoreInternal::DataParser;
        friend class SoundCloudAPI;
        friend class SongAPI;
        friend struct Song;

        operator Song();

        static void initialize() noexcept;

        SoundCloudSong collectFinalSong(GuildMemberData addedByGuildMember, SoundCloudSong newSong);

        vector<SoundCloudSong> searchForSong(string songQuery);

        virtual ~SoundCloudSong() {}

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

        SoundCloudSong() noexcept;

        string collectClientId();

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

    struct DiscordCoreAPI_Dll GuildApplicationCommandPermissionData : public DiscordEntity{
        vector<ApplicationCommandPermissionData> permissions{};
        string applicationId{ "" };
        string guildId{ "" };

        virtual ~GuildApplicationCommandPermissionData() {}
    };

    struct DiscordCoreAPI_Dll PutBatchEditApplicationCommandPermissionsData {
        vector<GuildApplicationCommandPermissionData> permissions{};
        string guildId{ "" };
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
    /// PermissionsConverter class, for manipulating Permission values. \brief PermissionsConverter class, for manipulating Permission values.
    class DiscordCoreAPI_Dll PermissionsConverter {
    public:

        /// Checks a given string for a particular Permission's presence. \brief Checks a given string for a particular Permission's presence.
        /// \param permission Which Permission to check for.
        /// \param permissionString The string within which to check for the Permission.
        /// \returns A bool answering about whether the Permission is present.
        static bool checkForPresence(Permissions permission, string permissionString);

        /// Adds one or more Permissions to a string value. \brief Adds one or more Permissions to a string value.
        /// \param originalPermissionString The original Permission string to modify.
        /// \param permissionsToAdd A vector containing the Permissions you wish to add.
        /// \returns A string containing the new Permissions value.
        static string addPermissionsToString(string originalPermissionString, vector<Permissions> permissionsToAdd);

        /// Removes one or more Permissions from a string value. \brief Removes one or more Permissions from a string value.
        /// \param originalPermissionString The original Permission string to modify.
        /// \param permissionsToRemove A vector containing the Permissions you wish to remove.
        /// \returns A string containing the new Permissions value.
        static string removePermissionsFromString(string originalPermissionString, vector<Permissions> permissionsToRemove);

        /// Displays the currently present Permissions in a string, and returns a vector with each of them stored in string format. \brief Displays the currently present Permissions in a string, and returns a vector with each of them stored in string format.
        /// \param permissionString The string to check for Permissions.
        /// \returns A vector full of strings of the Permissions that are in the input string's value.
        static vector<string> displayPermissions(string permissionString);

        /// Returns a string containing ALL of the possible Permissions. \brief Returns a string containing ALL of the possible Permissions.
        /// \returns A string containing all of the possible Permissions.
        static string getAllPermissions();

        /// Returns a string containing all of a given user's Permissions for a given channel. \brief Returns a string containing all of a given user's Permissions for a given channel.
        /// \param guildMember The GuildMember who's Permissions to analyze.
        /// \param channel The channel withint which to check for Permissions.
        /// \returns A string containing the final Permission's value for a given channel.
        static string getAllOfMyPerrmissions(GuildMember guildMember, ChannelData channel);

        /// Checks for a given Permission in a chosen channel, for a specific user. \brief Checks for a given Permission in a chosen channel, for a specific user.
        /// \param guildMember The GuildMember who to check the Permissions of.
        /// \param channel The Channel within which to check for the Permission's presence.
        /// \param permission A Permission to check the current Channel for.
        /// \returns A bool suggesting the presence of the chosen Permission.
        static bool checkForPermission(GuildMember guildMember, ChannelData channel, Permissions permission);

    protected:
        static string computeBasePermissions(GuildMember guildMember);

        static string  computeOverwrites(string basePermissions, GuildMember guildMember, ChannelData channel);

        static string computePermissions(GuildMember guildMember, ChannelData channel);
    };

    struct DiscordCoreAPI_Dll RecurseThroughMessagePagesData {
        InputEventData inputEventData{};
        uint32_t currentPageIndex{};
        string buttonId{};
    };

    DiscordCoreAPI_Dll RecurseThroughMessagePagesData recurseThroughMessagePages(string userID, InputEventData originalEvent, uint32_t currentPageIndex, vector<EmbedData> messageEmbeds, bool deleteAfter, uint32_t waitForMaxMs, bool returnResult = false);
    /**@}*/
};

namespace  DiscordCoreInternal {

    class ApplicationCommands;
    class BaseWebSocketAgent;
    class HttpRequestAgent;
    class ThreadManager;
    class DataParser;

    enum class ConnectionWebSocketType {
        Receive = 0,
        Send = 1
    };

    struct DiscordCoreAPI_Dll ConnectionWebSocketData {
        friend class DiscordCoreAPI::VoiceConnection;
        friend class VoiceChannelWebSocketAgent;
        ConnectionWebSocketType type{};
        string endpoint{ "" };
        string port{ "" };
    protected:
        unbounded_buffer<DiscordCoreAPI::AudioFrameData>* audioDataBuffer{ nullptr };
    };

    struct DiscordCoreAPI_Dll UpdatePresenceData {
        vector<DiscordCoreAPI::ActivityData> activities{};
        string status{ "" };
        int64_t since{ 0 };
        bool afk{ false };
    };

    struct DiscordCoreAPI_Dll VoiceStateData {
        string requestToSpeakTimestamp{ "" };
        DiscordCoreAPI::GuildMemberData guildMember{};
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

    enum class HeaderTypes {
        Bot_Auth = 0,
        X_Audit_Log_Reason = 1
    };

    struct DiscordCoreAPI_Dll Headers {
        HeaderTypes headerType{};
        string headerValue{ "" };
    };

    enum class HttpWorkloadClass {
        GET = 0,
        PUT = 1,
        POST = 2,
        PATCH = 3,
        DELETE = 4
    };

    enum class HttpWorkloadType {
        UNSET = 0,
        GET_GLOBAL_APPLICATION_COMMANDS = 1,
        POST_GLOBAL_APPLICATION_COMMAND = 2,
        GET_GLOBAL_APPLICATION_COMMAND = 3,
        PATCH_GLOBAL_APPLICATION_COMMAND = 4,
        DELETE_GLOBAL_APPLICATION_COMMAND = 5,
        BULK_PUT_GLOBAL_APPLICATION_COMMANDS = 6,
        GET_GUILD_APPLICATION_COMMANDS = 7,
        POST_GUILD_APPLICATION_COMMAND = 8,
        GET_GUILD_APPLICATION_COMMAND = 9,
        PATCH_GUILD_APPLICATION_COMMAND = 10,
        DELETE_GUILD_APPLICATION_COMMAND = 11,
        BULK_PUT_GUILD_APPLICATION_COMMANDS = 12,
        GET_GUILD_APPLICATION_COMMANDS_PERMISSIONS = 13,
        GET_GUILD_APPLICATION_COMMAND_PERMISSIONS = 14,
        PUT_GUILD_APPLICATION_COMMAND_PERMISSIONS = 15,
        BATCH_PUT_GUILD_APPLICATION_COMMAND_PERMISSIONS = 16,
        POST_INTERACTION_RESPONSE = 17,
        GET_INTERACTION_RESPONSE = 18,
        PATCH_INTERACTION_RESPONSE = 19,
        DELETE_INTERACTION_RESPONSE = 20,
        POST_FOLLOWUP_MESSAGE = 21,
        GET_FOLLOWUP_MESSAGE = 22,
        PATCH_FOLLOWUP_MESSAGE = 23,
        DELETE_FOLLOWUP_MESSAGE = 24,
        GET_GUILD_AUDIT_LOGS = 25,
        GET_CHANNEL = 26,
        PATCH_CHANNEL = 27,
        DELETE_CHANNEL = 28,
        GET_MESSAGES = 29,
        GET_MESSAGE = 30,
        POST_MESSAGE = 31,
        CROSSPOST_MESSAGE = 32,
        PUT_REACTION = 33,
        DELETE_OWN_REACTION = 34,
        DELETE_USER_REACTION = 35,
        GET_REACTIONS = 36,
        DELETE_ALL_REACTIONS = 37,
        DELETE_REACTIONS_BY_EMOJI = 38,
        PATCH_MESSAGE = 39,
        DELETE_MESSAGE = 40,
        DELETE_MESSAGE_OLD = 41,
        BULK_DELETE_MESSAGES = 42,
        PUT_CHANNEL_PERMISSION_OVERWRITES = 43,
        GET_CHANNEL_INVITES = 44,
        POST_CHANNEL_INVITE = 45,
        DELETE_CHANNEL_PERMISSION_OVERWRITES = 46,
        POST_FOLLOW_NEWS_CHANNEL = 47,
        POST_TRIGGER_TYPING_INDICATOR = 48,
        GET_PINNED_MESSAGES = 49,
        PUT_PIN_MESSAGE = 50,
        DELETE_PIN_MESSAGE = 51,
        PUT_RECIPIENT_TO_GROUP_DM = 52,
        DELETE_RECIPIENT_FROM_GROUP_DM = 53,
        POST_THREAD_WITH_MESSAGE = 54,
        POST_THREAD_WITHOUT_MESSAGE = 55,
        PUT_SELF_IN_THREAD = 56,
        PUT_THREAD_MEMBER = 57,
        DELETE_SELF_FROM_THREAD = 58,
        DELETE_THREAD_MEMBER = 59,
        GET_THREAD_MEMBER = 60,
        GET_THREAD_MEMBERS = 61,
        GET_ACTIVE_THREADS = 62,
        GET_PUBLIC_ARCHIVED_THREADS = 63,
        GET_PRIVATE_ARCHIVED_THREADS = 64,
        GET_JOINED_PRIVATE_ARCHIVED_THREADS = 65,
        GET_EMOJI_LIST = 66,
        GET_GUILD_EMOJI = 67,
        POST_GUILD_EMOJI = 68,
        PATCH_GUILD_EMOJI = 69,
        DELETE_GUILD_EMOJI = 70,
        POST_GUILD = 71,
        GET_GUILD = 72,
        GET_GUILD_PREVIEW = 73,
        PATCH_GUILD = 74,
        DELETE_GUILD = 75,
        GET_GUILD_CHANNELS = 76,
        POST_GUILD_CHANNEL = 77,
        PATCH_GUILD_CHANNEL_POSITIONS = 78,
        GET_GUILD_ACTIVE_THREADS = 79,
        GET_GUILD_MEMBER = 80,
        GET_GUILD_MEMBERS = 81,
        GET_SEARCH_GUILD_MEMBERS = 82,
        PUT_GUILD_MEMBER = 83,
        PATCH_GUILD_MEMBER = 84,
        PATCH_CURRENT_GUILD_MEMBER = 85,
        PUT_GUILD_MEMBER_ROLE = 86,
        DELETE_GUILD_MEMBER_ROLE = 87,
        DELETE_GUILD_MEMBER = 88,
        GET_GUILD_BANS = 89,
        GET_GUILD_BAN = 90,
        PUT_GUILD_BAN = 91,
        DELETE_GUILD_BAN = 92,
        GET_GUILD_ROLES = 93,
        POST_GUILD_ROLE = 94,
        PATCH_GUILD_ROLE_POSITIONS = 95,
        PATCH_GUILD_ROLE = 96,
        DELETE_GUILD_ROLE = 97,
        GET_GUILD_PRUNE_COUNT = 98,
        POST_GUILD_PRUNE = 99,
        GET_GUILD_VOICE_REGIONS = 100,
        GET_GUILD_INVITES = 101,
        GET_GUILD_INTEGRATIONS = 102,
        DELETE_GUILD_INTEGRATION = 103,
        GET_GUILD_WIDGET_SETTINGS = 104,
        MODIFY_GUILD_WIDGET = 105,
        GET_GUILD_WIDGET = 106,
        GET_VANITY_INVITE = 107,
        GET_GUILD_WIDGET_IMAGE = 108,
        GET_GUILD_WELCOME_SCREEN = 109,
        PATCH_GUILD_WELCOME_SCREEN = 110,
        PATCH_CURRENT_USER_VOICE_STATE = 111,
        PATCH_USER_VOICE_STATE = 112,
        GET_GUILD_SCHEDULED_EVENTS = 113,
        POST_GUILD_SCHEDULED_EVENT = 114,
        GET_GUILD_SCHEDULED_EVENT = 115,
        PATCH_GUILD_SCHEDULED_EVENT = 116,
        DELETE_GUILD_SCHEDULED_EVENT = 117,
        GET_GUILD_SCHEDULED_EVENT_USERS = 118,
        GET_GUILD_TEMPLATE = 119,
        POST_GUILD_FROM_GUILD_TEMPLATE = 120,
        GET_GUILD_TEMPLATES = 121,
        POST_GUILD_TEMPLATE = 122,
        PUT_GUILD_TEMPLATE = 123,
        PATCH_GUILD_TEMPLATE = 124,
        DELETE_GUILD_TEMPLATE = 125,
        GET_INVITE = 126,
        DELETE_INVITE = 127,
        POST_STAGE_INSTANCE = 128,
        GET_STAGE_INSTANCE = 129,
        PATCH_STAGE_INSTANCE = 130,
        DELETE_STAGE_INSTANCE = 131,

        GET_USER,
        GET_USER_SELF,
        GET_USER_GUILDS,
        POST_USER_DM,
        POST_CREATE_USER_DM,
        GET_APPLICATION,
        GET_SOCKET_PATH,
        YOUTUBE_SEARCH,
        YOUTUBE_VIDEO_QUERY,
        DELETE_LEAVE_GUILD,
        SOUNDCLOUD_SEARCH,
        SOUNDCLOUD_AUTH,
        SOUNDCLOUD_SONG_GET,
        GET_CURRENT_USER
    };

    struct DiscordCoreAPI_Dll HttpData {
        vector<string> responseHeaderValues{};
        string returnMessage{ "" };
        int32_t returnCode{ 0 };
        json data{};
    };

    struct DiscordCoreAPI_Dll HttpWorkloadData {
        map<string, string> headersToInsert{};
        HttpWorkloadClass workloadClass{};
        HttpWorkloadType workloadType{};
        string relativePath{ "" };
        string callStack{ "" };
        string content{ "" };
        string baseURL{ "" };
    };

    struct DiscordCoreAPI_Dll RateLimitData {
        RateLimitData() {};
        RateLimitData(RateLimitData& other) {
            this->nextExecutionTime = other.nextExecutionTime;
            this->getsRemaining = other.getsRemaining;
            this->msRemainTotal = other.msRemainTotal;
            this->timeStartedAt = other.timeStartedAt;
            this->workloadType = other.workloadType;
            this->isItMarked = other.isItMarked;
            this->theMutex.swap(other.theMutex);
            this->totalGets = other.totalGets;
            this->msRemain = other.msRemain;
            this->bucket = other.bucket;
        }
        unique_ptr<recursive_mutex> theMutex{ make_unique<recursive_mutex>() };
        HttpWorkloadType workloadType{};
        int64_t nextExecutionTime{ 0 };
        int64_t msRemainTotal{ 0 };
        int64_t timeStartedAt{ 0 };
        int32_t getsRemaining{ 0 };
        bool isItMarked{ false };
        int32_t totalGets{ 0 };
        int64_t msRemain{ 0 };
        string bucket{ "" };
    };

    struct DiscordCoreAPI_Dll VoiceConnectInitData {
        string channelId{ "" };
        string guildId{ "" };
        string userId{ "" };
    };

    struct DiscordCoreAPI_Dll VoiceConnectionData {
        vector<uint8_t> secretKey{};
        string voiceEncryptionMode{ "" };
        uint32_t audioSSRC{ 0 };
        string externalIp{ "" };
        string voicePort{ "" };
        string sessionId{ "" };
        string endPoint{ "" };
        string voiceIp{ "" };
        string token{ "" };
        string keys{ "" };
    };
};