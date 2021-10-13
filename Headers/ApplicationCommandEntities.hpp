// ApplicationCommandEntities.hpp - Header for the ApplicationCommand classes and structs.
// Aug 25, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _APPLICATION_COMMAND_ENTITIES_
#define _APPLICATION_COMMAND_ENTITIES_

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreAPI {

    /**
    * \addtogroup foundation_entities
    * @{
    */
    /// For collecting a single global ApplicationCommand. \brief For collecting a single global ApplicationCommand.
    struct DiscordCoreAPI_Dll GetGlobalApplicationCommandData {
        string commandId{ "" }; ///< The id of the command which you would like to collect.
    };

    /// For creating a single global ApplicationCommand. \brief For creating a single global ApplicationCommand.
    struct DiscordCoreAPI_Dll CreateGlobalApplicationCommandData {
        vector<DiscordCoreAPI::ApplicationCommandOptionData> options{}; ///< The options for the ApplicationCommand.
        bool defaultPermission{ true }; ///< Whether the command is enabled by default when the bot is added to a Guild
        ApplicationCommandType type{}; ///< The type of ApplicationCommand.
        string description{ "" };   ///< A description of the command.
        string name{ "" }; ///< A name for the new command.
    };

    /// For editing a single global ApplicationCommand. \brief For editing a single global ApplicationCommand.
    struct DiscordCoreAPI_Dll EditGlobalApplicationCommandData {
        vector<DiscordCoreAPI::ApplicationCommandOptionData> options{};///< The options for the ApplicationCommand.
        bool defaultPermission{ false };///< Whether the command is enabled by default when the bot is added to a Guild
        string description{ "" };   ///< A description of the command.
        string name{ "" }; ///< A name for the new command.
    };

    /// For deleting a single global ApplicationCommand. \brief For deleting a single global ApplicationCommand.
    struct DiscordCoreAPI_Dll DeleteGlobalApplicationCommandData {
        string name{ "" }; ///< The name of the command to delete.
    };

    /// For bulk-overwriting a collection of global ApplicationCommands. \brief For bulk-overwriting a collection of global ApplicationCommands.
    struct DiscordCoreAPI_Dll BulkOverwriteGlobalApplicationCommandsData {
        vector<CreateGlobalApplicationCommandData> data{};///< A vector of the options for the ApplicationCommands.
    };

    /// For acquiring all of the Guild ApplicationCommands of a single Guild. \brief For acquiring all of the Guild ApplicationCommands of a single Guild.
    struct DiscordCoreAPI_Dll GetGuildApplicationCommandsData {
        string guildId{ "" }; ///< The id of the Guild for which you would like to acquire the ApplicationCommands from.
    };

    /// For creating a single Guild ApplicationCommand. \brief For creating a single Guild ApplicationCommand.
    struct DiscordCoreAPI_Dll CreateGuildApplicationCommandData {
        vector<DiscordCoreAPI::ApplicationCommandOptionData> options{}; ///< The options for the ApplicationCommand.
        bool defaultPermission{ true }; ///< Whether the command is enabled by default when the bot is added to a Guild.
        ApplicationCommandType type{}; ///< The type of ApplicationCommand.
        string description{ "" };   ///< A description of the command.
        string guildId{ "" }; ///< The id of the Guild which you would like to add the new command to.
        string name{ "" }; ///< A name for the new command.
    };

    /// For acquiring a single Guild ApplicationCommand. \brief For acquiring a single Guild ApplicationCommand.
    struct DiscordCoreAPI_Dll GetGuildApplicationCommandData {
        string commandId{ "" };///< The command id which you would like to acquire.
        string guildId{ "" };///< The id of the Guild from which you would like to acquire the ApplicationCommand from.
    };    

    /// For editing a single Guild ApplicationCommand. \brief For editing a single Guild ApplicationCommand.
    struct DiscordCoreAPI_Dll EditGuildApplicationCommandData {
        vector<DiscordCoreAPI::ApplicationCommandOptionData> options{}; ///< The options for the ApplicationCommand.
        bool defaultPermission{ true }; ///< Whether the command is enabled by default when the bot is added to a Guild.
        string description{ "" };   ///< A description of the command.
        string guildId{ "" }; ///< The id of the Guild which you would like to add the new command to.
        string name{ "" }; ///< A name for the new command.
    };

    /// For deleting a single Guild ApplicationCommand. \brief For deleting a single Guild ApplicationCommand.
    struct DiscordCoreAPI_Dll DeleteGuildApplicationCommandData {
        string guildId{ "" }; ///< The id of the Guild which you would like to delete the command from.
        string name{ "" }; ///< A name of the command which you would like to delete.
    };

    /// For bulk-overwriting a collection of Guild ApplicationCommands. \brief For bulk-overwriting a collection of Guild ApplicationCommands.
    struct DiscordCoreAPI_Dll BulkOverwriteGuildApplicationCommandsData {
        vector<CreateGuildApplicationCommandData> data{};   ///< A vector of the options for the ApplicationCommands.
        string guildId{ "" };   ///< The id of the Guild which you would like to overwrite the commands of.
    };

    /// For acquiring the permissions of a collection of Guild ApplicationCommands. \brief For acquiring the permissions of a collection of Guild ApplicationCommands.
    struct DiscordCoreAPI_Dll GetGuildApplicationCommandPermissionsData {
        string guildId{ "" }; ///< The id of the Guild from which you would like to acquire the command permissions.
    };

    /// For acquiring the permissions of a single Guild ApplicationCommand. \brief For acquiring the permissions of a single Guild ApplicationCommand.
    struct DiscordCoreAPI_Dll GetApplicationCommandPermissionsData {
        string commandName{ "" };///< The name of the command which you would like to collect the permissions of.
        string guildId{ "" };///< The id of the Guild from which you would like to acquire the command permissions.
    };

    /// For editing the permissions of a single Guild ApplicationCommand. \brief For editing the permissions of a single Guild ApplicationCommand.
    struct DiscordCoreAPI_Dll EditGuildApplicationCommandPermissionsData {
        vector<ApplicationCommandPermissionData> permissions{}; ///< A vector of ApplicationCommand permissions.
        string commandName{ "" }; ///< The command name which you would like to edit the permissions of.
        string commandId{ "" };///< The command id which you would like to edit the permissions of.
        string guildId{ "" };///< The Guild id of the Guild for which you would like to edit the command permissions.
    };

    /// For batch editing the permissions of a collection of Guild ApplicationCommands. \brief For batch editing the permissions of a collection of Guild ApplicationCommands.
    struct DiscordCoreAPI_Dll BatchEditGuildApplicationCommandPermissionsData {
        vector<EditGuildApplicationCommandPermissionsData> permissions{}; ///< A vector of edit-Guild-application-permissions data to edit.
        string guildId{ "" };///< The Guild id of the Guild for which you would like to batch edit Guild application permissions.
    };

    /// A single ApplicationCommand. \brief A single ApplicationCommand.
    class DiscordCoreAPI_Dll ApplicationCommand : public ApplicationCommandData {
    public:

        friend struct Concurrency::details::_ResultHolder<ApplicationCommand>;
        friend class DiscordCoreInternal::ApplicationCommandManager;
        friend struct OnApplicationCommandDeletionData;
        friend struct OnApplicationCommandCreationData;
        friend struct OnApplicationCommandUpdateData;
        friend class DiscordCoreClient;

    protected:

        ApplicationCommand();

        ApplicationCommand(ApplicationCommandData dataNew);
    };
    /**@}*/
}

namespace DiscordCoreInternal {

    class DiscordCoreAPI_Dll ApplicationCommandManager : ThreadContext {
    public:

        template <class _Ty>
        friend _CONSTEXPR20_DYNALLOC void std::_Destroy_in_place(_Ty& _Obj) noexcept;
        friend class DiscordCoreAPI::ApplicationCommands;
        friend class DiscordCoreAPI::DiscordCoreClient;
        friend class DiscordCoreAPI::ButtonCollector;
        friend class DiscordCoreAPI::Interactions;
        friend class DiscordCoreAPI::SelectMenu;

        ApplicationCommandManager(ApplicationCommandManager* pointer);

    protected:

        string applicationId{ "" };

        task<vector<DiscordCoreAPI::ApplicationCommand>> getGlobalApplicationCommandsAsync();

        task<DiscordCoreAPI::ApplicationCommand> createGlobalApplicationCommandAsync(DiscordCoreAPI::CreateGlobalApplicationCommandData dataPackage);

        task<DiscordCoreAPI::ApplicationCommand> getGlobalApplicationCommandAsync(DiscordCoreAPI::GetGlobalApplicationCommandData dataPackage);

        task<DiscordCoreAPI::ApplicationCommand> editGlobalApplicationCommandAsync(DiscordCoreAPI::EditGlobalApplicationCommandData dataPackage);

        void deleteGlobalApplicationCommand(DiscordCoreAPI::DeleteGlobalApplicationCommandData dataPackage);

        task<vector<DiscordCoreAPI::ApplicationCommand>> bulkOverwriteGlobalApplicationCommandsAsync(DiscordCoreAPI::BulkOverwriteGlobalApplicationCommandsData dataPackage);

        task<vector<DiscordCoreAPI::ApplicationCommand>> getGuildApplicationCommandsAsync(DiscordCoreAPI::GetGuildApplicationCommandsData dataPackage);

        task<DiscordCoreAPI::ApplicationCommand> createGuildApplicationCommandAsync(DiscordCoreAPI::CreateGuildApplicationCommandData dataPackage);

        task<DiscordCoreAPI::ApplicationCommand> getGuildApplicationCommandAsync(DiscordCoreAPI::GetGuildApplicationCommandData dataPackage);

        task<DiscordCoreAPI::ApplicationCommand> editGuildApplicationCommandAsync(DiscordCoreAPI::EditGuildApplicationCommandData dataPackage);

        void deleteGuildApplicationCommand(DiscordCoreAPI::DeleteGuildApplicationCommandData dataPackage);

        task<vector<DiscordCoreAPI::ApplicationCommand>> bulkOverwriteGuildApplicationCommandsAsync(DiscordCoreAPI::BulkOverwriteGuildApplicationCommandsData dataPackage);

        task<vector<DiscordCoreAPI::GuildApplicationCommandPermissionsData>> getGuildApplicationCommandPermissionsAsync(DiscordCoreAPI::GetGuildApplicationCommandPermissionsData dataPackage);

        DiscordCoreAPI::GuildApplicationCommandPermissionsData getApplicationCommandPermissions(DiscordCoreAPI::GetApplicationCommandPermissionsData dataPackage);

        DiscordCoreAPI::GuildApplicationCommandPermissionsData editGuildApplicationCommandPermissions(DiscordCoreAPI::EditGuildApplicationCommandPermissionsData dataPackage);

        vector<DiscordCoreAPI::GuildApplicationCommandPermissionsData> batchEditGuildApplicationCommandPermissions(DiscordCoreAPI::BatchEditGuildApplicationCommandPermissionsData dataPackage);

        task<void> displayGlobalApplicationCommandsAsync();

        void displayOptions(vector<DiscordCoreAPI::ApplicationCommandOptionData> applicationCommandOptionData);

        void copyOptionsData(vector<ApplicationCommandOptionData>* appCommandOptionData, vector<DiscordCoreAPI::ApplicationCommandOptionData> appCommandOptionDataSource);
        
    };
}

#endif