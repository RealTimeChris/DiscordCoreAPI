// FoundationEntities.hpp - Header for all of the Discord/Support API data structures.
// May 12, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"

#ifdef DELETE
#undef DELETE
#endif

namespace  DiscordCoreInternal {

    class ApplicationCommands;
    class BaseWebSocketAgent;
    class HttpRequestAgent;
    class ThreadManager;
    class DataParser;
};

namespace DiscordCoreAPI {

    struct SongCompletionEventData;
    struct ButtonInteractionData;
    struct ClientStatusData;
    struct GuildMemberData;
    struct AudioFrameData;
    struct ActivityData;
    struct CommandData;
    struct UserData;
    struct Song;

    template<typename returnType>
    class CoRoutine;
    class RespondToInputEventData;
    class DatabaseManagerAgent;
    class PermissionsConverter;
    class SelectMenuCollector;
    class ApplicationCommands;
    class DiscordCoreClient;
    class InputEvents;
    class ButtonCollector;
    class VoiceConnection;
    class SoundCloudSong;
    class SoundCloudAPI;
    class Interactions;
    class EventHandler;
    class GuildMembers;
    class YouTubeSong;
    class SelectMenu;
    class YouTubeAPI;
    class Reactions;
    class Messages;
    class Channels;
    class BotUser;
    class SongAPI;
    class Message;
    class Guilds;
    class Roles;
    class Users;
    class Guild;

    DiscordCoreAPI_Dll bool nanoSleep(__int64 ns);

    DiscordCoreAPI_Dll void spinLock(__int64 timeInNsToSpinLockFor);

    class DiscordCoreAPI_Dll StopWatch {
    public:
        StopWatch(__int64 maxNumberOfMsNew) {
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

    template <typename T>
    bool waitForTimeToPass(concurrent_queue<T>* outBuffer, T* argOne, __int32 timeInMsNew) {
        StopWatch stopWatch(timeInMsNew);
        bool doWeBreak{ false };
        while (!outBuffer->try_pop(*argOne)) {
            concurrency::wait(10);
            if (stopWatch.hasTimePassed()) {
                doWeBreak = true;
                break;
            }
        };
        return doWeBreak;
    }

    template <typename ...T>
    CoRoutine<void> executeFunctionAfterTimePeriod(function<void(T...)>theFunction, __int32 timeDelayInMs, bool isRepeating, T... args) {
        co_await NewThreadAwaitable<void>();
        ThreadPoolTimer threadPoolTimer{ nullptr };
        if (timeDelayInMs > 0 && !isRepeating) {
            auto timeElapsedHandler = [=]()->void {
                concurrency::wait(timeDelayInMs);
                theFunction(args...);
                return;
            };
            timeElapsedHandler();
        }
        else if (timeDelayInMs > 0 && isRepeating) {
            auto timeElapsedHandler = [=](ThreadPoolTimer threadPoolTimerNew)->void {
                theFunction(args...);
                return;
            };
            threadPoolTimer = threadPoolTimer.CreatePeriodicTimer(timeElapsedHandler, winrt::Windows::Foundation::TimeSpan(timeDelayInMs * 10000));
        }
        else {
            theFunction(args...);
        }
        co_return;
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
            __time64_t ____int64ime;
            errno_t err;
            // Get time as 64-bit integer.
            _time64(&____int64ime);
            // Convert to local time.
            err = _localtime64_s(&newtime, &____int64ime);
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

    DiscordCoreAPI_Dll string convertTimeInMsToDateTimeString(__int64 timeInMs);

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
    typedef string Snowflake;

    /// Base class DiscordCoreAPI_Dll for all Discord entities. \brief Base class DiscordCoreAPI_Dll for all Discord entities.
    class DiscordCoreAPI_Dll DiscordEntity {
    public:
        Snowflake id{ "" };///< The identifier "snowflake" of the given entity.
        /// Converts the snowflake-id into a time and date stamp. \brief Converts the snowflake-id into a time and date stamps.
        /// \returns A string containing the timestamp.
        string getCreatedAtTimestamp();

        virtual ~DiscordEntity() {};
    };

    /**@}*/
};

namespace DiscordCoreAPI {

    /// Role tags data. \brief Role tags data.
    struct DiscordCoreAPI_Dll RoleTagsData {
        string premiumSubscriber{ "" };///< Are they a premium subscriber?
        string integrationId{ "" };///< What is the integration id?
        string botId{ "" };///< What is the bot id?
    };

    /// Data structure representing a single Role. \brief Data structure representing a single Role.
    struct DiscordCoreAPI_Dll RoleData : public DiscordEntity {
        bool mentionable{ false }; ///< Is ths Role mentionable?
        string permissions{ "" }; ///< The Role's base Guild Permissions.
        __int32 position{ 0 }; ///< Its position amongst the rest of the Guild's roles.
        bool managed{ false }; ///< is it a managed Role?
        RoleTagsData tags{}; ///< Role tags for the Role.
        bool hoist{ false };///< Is it hoisted above the other roles?
        __int32 color{ 0 };///< The Role's color.
        string name{ "" };///< The Role's name.

        virtual ~RoleData() {};
    };

    /// A single Role.
    class DiscordCoreAPI_Dll Role : public RoleData {
    public:

        virtual ~Role() {};

    protected:

        friend struct Concurrency::details::_ResultHolder<Role>;
        friend class DiscordCoreInternal::HttpRequestAgent;
        friend class DiscordCoreInternal::DataParser;
        template<typename returnValueType>
        friend class CoRoutine;
        friend struct OnRoleDeletionData;
        friend struct OnRoleCreationData;
        friend struct OnRoleUpdateData;
        friend class DiscordCoreClient;
        friend class EventHandler;
        friend class EventManager;
        friend class Roles;
        friend class Guild;

        Role();

        Role(RoleData dataNew);
    };

    /// Data structure representing a single user.
    struct DiscordCoreAPI_Dll UserData : public DiscordEntity {
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

        virtual ~UserData() {};
    };

    /// A single User. \brief A single User.
    class DiscordCoreAPI_Dll User : public UserData {
    public:

        friend struct Concurrency::details::_ResultHolder<User>;
        friend class DiscordCoreInternal::HttpRequestAgent;
        friend class DiscordCoreInternal::DataParser;
        template<typename returnValueType>
        friend class CoRoutine;
        friend struct OnGuildMemberRemoveData;
        friend struct OnGuildBanRemoveData;
        friend struct OnGuildBanAddData;
        friend struct OnUserUpdateData;
        friend class DiscordCoreClient;
        friend class Users;
        friend class Guild;

        virtual ~User() {};

    protected:

        User();

        User(UserData dataNew);
    };

    /// Attachment data. \brief Attachment data.
    struct DiscordCoreAPI_Dll AttachmentData : public DiscordEntity {
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
        string proxyIconUrl{ "" };///< Proxy icon url.
        string iconUrl{ "" };///< Icon url.
        string text{ "" };///< Footer text.
    };

    /// Embed image data. \brief Embed image data.
    struct DiscordCoreAPI_Dll EmbedImageData {
        string proxyUrl{ "" };///< Proxy url.
        __int32 height{ 0 };///< Image height.
        __int32 width{ 0 };///< Image width.
        string url{ "" };///< Image url.        
    };

    /// Embed thumbnail data. \brief Embed thumbnail data.
    struct DiscordCoreAPI_Dll EmbedThumbnailData {
        string proxyUrl{ "" };///< Proxy url.
        __int32 height{ 0 };///< Image height.
        __int32 width{ 0 };///< Image width.
        string url{ "" };///< Image url.        
    };

    /// Embed video data. \brief Embed video data.
    struct DiscordCoreAPI_Dll EmbedVideoData {
        string proxyUrl{ "" };///< Proxy url.
        __int32 height{ 0 };///< Image height.
        __int32 width{ 0 };///< Image width.
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

    /// Edit Channel Permission overwrites type. \brief Edit Channel Permission overwrites type.
    enum class EditChannelPermissionOverwritesType {
        Role = 0,///<Role
        User = 1///<User
    };

    /// A Permission overwrite, for a given Channel. \brief A Permission overwrite, for a given Channel.
    struct DiscordCoreAPI_Dll OverWriteData : public DiscordEntity {
        EditChannelPermissionOverwritesType type{};   ///< Role or User type.
        string channelId{ "" };///< Channel id for which Channel this overwrite be__int64s to.
        string allow{ "" };///< Collection of Permissions to allow.
        string deny{ "" };///< Collection of Permissions to deny.
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
        __int32 autoArchiveDuration{ 0 }; ///< How __int64 before archiving this thread.
        string archiveTimestamp{ "" }; ///< (Where applicable) the time at which this thread was archived.
        bool invitable{ false }; ///< The id of the individual who archived this thread.
        bool archived{ false }; ///< Whether or not this thread is currently archived.
        bool locked{ false }; ///< Whether or not this thread is currently locked.
    };

    /// Data for a single member of a thread. \brief Data for a single member of a thread.
    struct DiscordCoreAPI_Dll ThreadMemberData : public DiscordEntity {
        string joinTimestamp{ "" }; ///< The time at which the member joined this thread.
        string userId{ "" };    ///< The User's id.
        __int32 flags{ 0 }; ///< Flags.
    };

    enum class ThreadType {
        GUILD_NEWS_THREAD = 10,
        GUILD_PUBLIC_THREAD = 11,
        GUILD_PRIVATE_THREAD = 12,
    };

    enum class ThreadAutoArchiveDuration :__int32 {
        SHORTEST = 60,
        SHORT = 1440,
        LONG = 4320,
        LONGEST = 10080
    };

    /// Data structure representing a single Channel. \brief Data structure representing a single Channel.
    struct DiscordCoreAPI_Dll ChannelData : public DiscordEntity {
        map<string, OverWriteData> permissionOverwrites{}; ///< Permission overwrites for the given Channel.
        __int32 defaultAutoArchiveDuration{ 0 };
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

        virtual ~ChannelData() {};
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
        shared_ptr<VoiceStateData> voiceData{ nullptr };///< The voice state data for the GuildMember.
        string premiumSince{ "" };///< If applicable, when they first boosted the server.
        string permissions{ "" };///< Their base-level Permissions in the Guild.
        string userMention{ "" };///< What to enter to get them mentioned in a Message.
        vector<string> roles{}; ///< The Guild roles that they have.
        string joinedAt{ "" };///< When they joined the Guild.
        bool pending{ false };///< Are they waiting at the entry screen?
        string guildId{ "" };///< The current Guild's id.
        bool deaf{ false };///< Are they server deafened?
        bool mute{ false };///< Are they server muted?
        string nick{ "" };///< Their nick/display name.
        UserData user{};///< User data for the current GuildMember.

        virtual ~GuildMemberData() {};
    };

    struct ActiveThreadsData {
        vector<ThreadMemberData> members{};
        vector<ChannelData> threads{};
        bool hasMore{ false };
    };

    struct ArchivedThreadsData : public ActiveThreadsData {};

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

    /// ApplicationCommand Permission type. \brief ApplicationCommand Permission type.
    enum class ApplicationCommandPermissionType {
        Role = 1,///< Role type.
        User = 2///< User type.
    };

    /// Permissions data for an ApplicationCommand. \brief Permissions data for an ApplicationCommand.
    struct DiscordCoreAPI_Dll ApplicationCommandPermissionData : public DiscordEntity {
        ApplicationCommandPermissionType type{ ApplicationCommandPermissionType::Role }; ///< The type of Permission.
        bool permission{ false }; ///< Whether the Permission is active or not.
    };

    /// Represents the Permissions for accessing an ApplicationCommand from within a Guild. \brief Represents the Permissions for accessing an ApplicationCommand from within a Guild.
    struct DiscordCoreAPI_Dll GuildApplicationCommandPermissionsData : public DiscordEntity {
        vector<ApplicationCommandPermissionData> permissions{};///< The Permissions.
        string applicationId{ "" };///< The application's id.
        string guildId{ "" };///< The Guild's id.
    };

    /// Data structure representing a single emoji. \brief Data structure representing a single emoji.
    struct DiscordCoreAPI_Dll EmojiData : public DiscordEntity {
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
        __int32 defaultAutoArchiveDuration{ 10080 };
        __int32 videoQualityMode{ 1 };
        __int32 rateLimitPerUser{ 0 };
        __int32 bitrate{ 48000 };
        __int32 userLimit{ 0 };
        string parentId{ "" };
        string rtcRgion{ "" };
        __int32 position{ 0 };
        ChannelType type{};
        string topic{ "" };
        bool nsfw{ false };
        string name{ "" };
    };

    /// Data structure representing a single reaction.
    struct DiscordCoreAPI_Dll ReactionData : public DiscordEntity {
        GuildMemberData member{}; ///< The GuildMember who placed the reaction.
        string channelId{ "" }; ///< The id of the Channel where it was placed.
        string messageId{ "" }; ///< The id of the Message upon which it was placed.
        string guildId{ "" };///< The id of the Guild where it was placed.
        string userId{ "" }; ///< The id of the User who placed the reaction.
        __int32 count{ 0 };///< The number of times this particular emoji was placed as a reaction to the given Message.
        EmojiData emoji{}; ///< The emoji that was placed as a reaction.
        bool me{ false }; ///< Whether or not I (The bot) placed it.

        virtual ~ReactionData() {};
    };

    /// A single Reaction. \brief A single Reaction.
    class DiscordCoreAPI_Dll Reaction : public ReactionData {
    public:

        virtual ~Reaction() {};

    protected:

        friend struct Concurrency::details::_ResultHolder<Reaction>;
        friend class DiscordCoreInternal::HttpRequestAgent;
        template<typename returnValueType>
        friend class CoRoutine;
        friend class DiscordCoreInternal::DataParser;
        friend struct OnReactionAddData;
        friend class DiscordCoreClient;

        Reaction();

        Reaction(ReactionData dataNew);
        
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
        __int32 membershipState{ 0 };///< Current state.
        string teamId{ "" };///< Id of the current team.
        UserData user{};///< User data of the current User.
    };

    /// Team object data. \brief Team object data.
    struct DiscordCoreAPI_Dll TeamObjectData : public DiscordEntity {
        vector<TeamMembersObjectData> members{};///< Array of team members object data.
        string ownerUserId{ "" };///< User id of the team owner.
        string icon{ "" };///<Icon for the team.
    };

    /// Application data. \brief Application data.
    struct DiscordCoreAPI_Dll ApplicationData : public DiscordEntity {
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
        vector<__int32> size{ 0, 0 };///< The size of the party.
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
        __int64 start{ 0 }; ///< Unix timestamp - Send this to have an "elapsed" timer.
        __int64 end{ 0 };   ///< Unix timestamp - send this to have a "remaining" timer.
    };

    /// Button data. \brief Button data.
    struct DiscordCoreAPI_Dll ButtonData {
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

        virtual ~StickerData() {};
    };

    struct GuildPreviewData {
        __int32 approximatePresenceCount{ 0 };
        __int32 approximateMemberCount{ 0 };
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

    /// Data structure representing a single guiild. \brief Data structure representing a single guiild.
    struct DiscordCoreAPI_Dll GuildData : public DiscordEntity {
        DefaultMessageNotificationLevel defaultMessageNotifications{};///<Default Message notification level.
        ExplicitContentFilterLevel explicitContentFilter{}; ///< Explicit content filtering level, by default.
        map<string, shared_ptr<VoiceStateData>> voiceStates{};///< Array of Guild-member voice-states.
        vector<StageInstanceData> stageInstances{}; ///< Array of stage instances.
        vector<PresenceUpdateData> presences{}; ///< Array of presences for each GuildMember.
        __int32 premiumSubscriptionCount{ 0 }; ///< Premium subscription count.
        __int32 approximatePresenceCount{ 0 }; ///< Approximate quantity of presences.
        VerificationLevel verificationLevel{};  ///< Verification level required.
        string publicUpdatesChannelId{ "" }; ///< Id of the public updates Channel.        
        __int32 approximateMemberCount{ 0 };    ///< Approximate member count.
        vector<GuildMemberData> members{};  ///< Array of GuildMembers.
        WelcomeScreenData welcomeScreen{};  ///< Welcome screen for the Guild.
        __int32 maxVideoChannelUsers{ 0 };  ///< Maximum quantity of users per video Channel.
        AfkTimeOutDurations afkTimeOut{};    ///< Time for an individual to time out as afk.
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
        string permissions{ "" };   ///< Current Permissions for the bot in the Guild.
        vector<RoleData> roles{};   ///< Array of Guild roles.
        string description{ "" };   ///< Description of the Guild.
        __int32 memberCount{ 0 };   ///< Member count.
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

        virtual ~GuildData() {};
    };

    enum class GuildScheduledEventPrivacyLevel {
        PUBLIC = 1,///< The scheduled event is public and available in discovery.
        GUILD_ONLY = 2///< The scheduled event is only accessible to guild members.
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

    struct GuildScheduledEventEntityMetadata {
        string location{ "" };
    };

    struct GuildScheduledEventData {
        GuildScheduledEventEntityMetadata entityMetadata{};
        GuildScheduledEventPrivacyLevel privacyLevel{};
        GuildScheduledEventEntityType entityType{};
        GuildScheduledEventStatus status{};
        string scheduledStartTime{ "" };
        string scheduledEndTime{ "" };
        string description{ "" };
        __int32 userCount{ 0 };
        string channelId{ "" };
        string creatorId{ "" };
        string entityId{ "" };
        string guildId{ "" };
        UserData creator{};
        string name{ "" };
        string id{ "" };
    };

    /// Invite data. \brief Invite data.
    struct DiscordCoreAPI_Dll InviteData {
        GuildScheduledEventData guildScheduledEvent{};///< Scheduled Guild event.
        __int32 approximatePresenceCount{ 0 };///< Approximate presence count.
        __int32 approximateMemberCount{ 0 };///< Approximate member count.
        ApplicationData targetApplication{};///< Application data.
        StageInstanceData stageInstance{};///< Stage instance data.        
        __int32 targetType{ 0 };///< Target type.
        bool temporary{ false };///< Is it temporary?
        string createdAt{ "" };///< Time it was created at.
        string expiresAt{ "" };///< When the invite expires.
        UserData targetUser{};///< Target User of the invite.
        ChannelData channel{};///< Channel data of the Channel that the invite is for.
        __int32 maxUses{ 0 };///< Max number of uses.
        __int32 maxAge{ 0 };///< Maximum age of the invite.
        UserData inviter{};///< The User who created the invite.
        string code{ "" };///< Unique invite code.
        GuildData guild{};///< Guild data of the Channel that the invite is for.
        __int32 uses{ 0 };///< Maximum nunmber of uses.
    };

    /// Represents the type of a given invite target. \brief Represents the type of a given invite target.
    enum class InviteTargetTypes {
        STREAM = 1,
        EMBEDDED_APPLICATION = 2
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
        string valueString{ "" }; ///< The value, if the given choice is a string.
        float valueFloat{ 0.0f }; ///< The value, if the given choice is a float.
        __int32	valueInt{ 0 }; ///< The value, if the given choice is an __int32.
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
        bool defaultPermission{ false };///< Whether or not the default Permission in the Guild is to have access to this command or not.
        ApplicationCommandType type{};///< The type of ApplicationCommand.
        string applicationId{ "" };///< The current application id.
        string description{ "" };///< A description of the current ApplicationCommand.
        string guildId{ "" };///< (Where applicable) a Guild id for which to assign this ApplicationCommand to.
        string version{ "" };///< An autoincremented version.
        string name{ "" };///< Name of the current ApplicationCommand.
    };

    /// A single ApplicationCommand. \brief A single ApplicationCommand.
    class DiscordCoreAPI_Dll ApplicationCommand : public ApplicationCommandData {
    public:

        friend struct Concurrency::details::_ResultHolder<ApplicationCommand>;
        friend class DiscordCoreInternal::HttpRequestAgent;
        friend struct OnApplicationCommandDeletionData;
        friend struct OnApplicationCommandCreationData;
        friend struct OnApplicationCommandUpdateData;
        friend class DiscordCoreInternal::DataParser;
        template<typename returnValueType>
        friend class CoRoutine;
        friend class ApplicationCommands;
        friend class DiscordCoreClient;

    protected:

        ApplicationCommand();

        ApplicationCommand(ApplicationCommandData dataNew);
    };

    /// Function data for repeated functions to be loaded. \brief Function data for repeated functions to be loaded.
    struct DiscordCoreAPI_Dll RepeatedFunctionData {
        function<void()> function{ nullptr };///< The function pointer to be loaded.
        bool repeated{ false }; ///< Whether or not the function is repeating.
        __int32 intervalInMs{ 0 };  ///< The time interval at which to call the function.
    };

    /// Channel mention data. \brief Channel mention data.
    struct DiscordCoreAPI_Dll ChannelMentionData : public DiscordEntity {
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

    /// Message sticker item type. \brief Message sticker item type.
    enum class StickerItemType {
        PNG = 1,///< PNG
        APNG = 2,///< APNG
        LOTTIE = 3///< Lottie
    };

    /// Message sticker item data. \brief Message sticker item data.
    struct DiscordCoreAPI_Dll StickerItemData : public DiscordEntity {
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

        virtual ~MessageData() {};
    };

    /// A single Message. \brief A single Message.
    class DiscordCoreAPI_Dll Message : public MessageData {
    public:
        Message();

        Message(MessageData dataNew);

    protected:

        friend struct Concurrency::details::_ResultHolder<Message>;
        friend class DiscordCoreInternal::HttpRequestAgent;
        friend class DiscordCoreInternal::DataParser;
        template<typename returnValueType>
        friend class CoRoutine;
        friend struct OnMessageCreationData;
        friend struct OnMessageUpdateData;
        friend class DiscordCoreClient;
        friend class MessageCollector;
        friend class InputEvents;
    };

    /// Resolved data.
    struct DiscordCoreAPI_Dll ResolvedData {
        map<string, GuildMemberData> members{}; ///< Map full of GuildMemeberData.
        map<string, MessageData> messages{};///< Map full of MessageData.
        map<string, ChannelData> channels{};///< Map full of ChannelData.
        map<string, UserData> users{};///< Map full of UserData.
        map<string, RoleData> roles{};///< Map full of RoleData.
    };

    /// MessageCollectorReturn data. \brief MessageCollectorReturn data.
    struct MessageCollectorReturnData {
        vector<Message> messages; ///< A vector of collected Messages.
    };

    struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption;

    DiscordCoreAPI_Dll vector<ApplicationCommandInteractionDataOption> convertAppCommandInteractionDataOptions(vector<ApplicationCommandInteractionDataOption> originalOptions);

    /// ApplicationCommand interaction data option. \brief ApplicationCommand interaction data option.
    struct DiscordCoreAPI_Dll ApplicationCommandInteractionDataOption {
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
        SELECT_MENU_INTERACTION = 4, ///< Select-menu input
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
        VoiceConnection* voiceConnection{}; ///< A pointer to the current VoiceConnection.
        bool isThisAReplay{ false };///< Is this a replay? (Did a track recently fail to play?)
        Song previousSong{};///< The previously played Song.
    };

    /**@}*/

    /**
    * \addtogroup foundation_entities
    * @{
    */
    /// Permissions. \brief Permissions.
    enum class Permissions : __int64 {
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

        static void initialize() noexcept;

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

    struct DiscordCoreAPI_Dll GuildApplicationCommandPermissionData : public DiscordEntity {
        vector<ApplicationCommandPermissionData> permissions{};
        string applicationId{ "" };
        string guildId{ "" };
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
        static string getAllOfMyPerrmissions(GuildMemberData guildMember, ChannelData channel);

        /// Checks for a given Permission in a chosen channel, for a specific user. \brief Checks for a given Permission in a chosen channel, for a specific user.
        /// \param guildMember The GuildMember who to check the Permissions of.
        /// \param channel The Channel within which to check for the Permission's presence.
        /// \param permission A Permission to check the current Channel for.
        /// \returns A bool suggesting the presence of the chosen Permission.
        static bool checkForPermission(GuildMemberData guildMember, ChannelData channel, Permissions permission);

    protected:
        static string computeBasePermissions(GuildMemberData guildMember);

        static string  computeOverwrites(string basePermissions, GuildMemberData guildMember, ChannelData channel);

        static string computePermissions(GuildMemberData guildMember, ChannelData channel);
    };

    /// Returns from an execution of recurseThroughMessagePages(). \brief Returns from an execution of recurseThroughMessagePages().
    struct DiscordCoreAPI_Dll RecurseThroughMessagePagesData {
        InputEventData inputEventData{};///< The latest InputEventData that was returned by updating the message.
        unsigned __int32 currentPageIndex{};//< The current page index, within the number of submitted pages to turn through.
        string buttonId{};///< The id of the last button that was pressed.
    };

    /// Recurses through a succession of messages. \brief Recurses through a succession of messages.
    /// \param userID A user's id to check for the responses of to the buttons.
    /// \param originalEvent The original InputEventData structure.
    /// \param currentPageIndex The current page within the stack of pages to recurse through.
    /// \param messageEmbeds A vector full of EmbedData to recurse through.
    /// \param deleteAfter A bool determining whether or not the message will be deleted upon timeout/exiting.
    /// \param waitForMaxMs Maximum number of milliseconds to wait for a button response.
    /// \param returnResult Whether or not to accept a request to return the result.
    /// \returns A RecurseThroughMessagePageData structure.
    DiscordCoreAPI_Dll RecurseThroughMessagePagesData recurseThroughMessagePages(string userID, InputEventData originalEvent, unsigned __int32 currentPageIndex, vector<EmbedData> messageEmbeds, bool deleteAfter, unsigned __int32 waitForMaxMs, bool returnResult = false);
    /**@}*/
};

namespace  DiscordCoreInternal {
    /**
    * \addtogroup foundation_entities
    * @{
    */
    class ApplicationCommands;
    class BaseWebSocketAgent;
    class HttpRequestAgent;
    class ThreadManager;
    class DataParser;

    enum class ConnectionWebSocketType {
        Receive = 0,
        Send = 1
    };

    struct ConnectionWebSocketData {
        friend class DiscordCoreAPI::VoiceConnection;
        friend class VoiceChannelWebSocketAgent;
        ConnectionWebSocketType type{};
        string endpoint{ "" };
        string port{ "" };
    protected:
        shared_ptr<unbounded_buffer<DiscordCoreAPI::AudioFrameData>> audioDataBuffer{ nullptr };
    };

    struct DiscordCoreAPI_Dll UpdatePresenceData {
        vector<DiscordCoreAPI::ActivityData> activities{};
        string status{ "" };
        __int64 since{ 0 };
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

    enum class ThreadType {
        Regular = 0,
        Music = 1
    };

    class DiscordCoreAPI_Dll SchedulerWrapper {
    public:
        SchedulerWrapper(Scheduler* dataPackage) {
            this->scheduler = dataPackage;
        }
        Scheduler* scheduler{ nullptr };
    };

    class DiscordCoreAPI_Dll ThreadContext {
    public:
        shared_ptr<SchedulerWrapper> scheduler{ nullptr };

        static unique_ptr<ThreadContext> getThreadContext(ThreadType threadType = ThreadType::Regular);

        Scheduler* operator*();

        ThreadContext(ThreadType threadType);

        ThreadContext(string);

        ~ThreadContext();

    protected:
        friend class DiscordCoreAPI::DiscordCoreClient;

        static vector<shared_ptr<SchedulerWrapper>> schedulers;

        static vector<unique_ptr<thread>> threads;

        unique_ptr<thread> threadOfExecution{ nullptr };

        ThreadContext(ThreadContext& newThread);

        void releaseContext();

        static void cleanup();

    };

    enum class HeaderTypes {
        Bot_Auth = 0,
        X_Audit_Log_Reason = 1
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
        GET_GUILD_MEMBERS = 80,
        GET_SEARCH_GUILD_MEMBERS = 81,
        PUT_GUILD_MEMBER = 82,
        PATCH_CURRENT_GUILD_MEMBER = 83,
        PUT_GUILD_MEMBER_ROLE = 84,
        DELETE_GUILD_MEMBER_ROLE = 85,
        DELETE_GUILD_MEMBER = 86,
        GET_GUILD_BANS = 87,
        GET_GUILD_BAN = 88,
        PUT_GUILD_BAN = 89,
        DELETE_GUILD_BAN = 90,
        GET_GUILD_ROLES = 91,
        POST_GUILD_ROLE = 92,
        PATCH_GUILD_ROLE_POSITIONS = 93,
        PATCH_GUILD_ROLE = 94,
        DELETE_GUILD_ROLE = 95,

        GET_USER,
        GET_USER_SELF,
        GET_GUILD_MEMBER,
        GET_USER_GUILDS,
        POST_USER_DM,
        POST_CREATE_USER_DM,
        GET_APPLICATION,
        GET_SOCKET_PATH,
        YOUTUBE_SEARCH,
        YOUTUBE_VIDEO_QUERY,
        GET_INVITES,
        PATCH_GUILD_MEMBER,
        GET_INVITE,
        GET_VANITY_INVITE,
        DELETE_LEAVE_GUILD,
        SOUNDCLOUD_SEARCH,
        SOUNDCLOUD_AUTH,
        SOUNDCLOUD_SONG_GET,
        DELETE_OR_CLOSE_CHANNEL,
        GET_CURRENT_USER
    };

    struct DiscordCoreAPI_Dll HttpData {
        vector<string> responseHeaderValues{};
        string returnMessage{ "" };
        __int32 returnCode{ 0 };
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
        __int64 nextExecutionTime{ 0 };
        __int64 msRemainTotal{ 0 };
        __int64 timeStartedAt{ 0 };
        __int32 getsRemaining{ 0 };
        bool isItMarked{ false };
        __int32 totalGets{ 0 };
        __int64 msRemain{ 0 };
        string bucket{ "" };
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
};