### **Beginner's Notes:**
---
- All of the central class managers are either classes filled with static methods that are a part of the `DiscordCoreAPI` namespace, or members of the namespace `DiscordCoreAPI`.
  ##### They are as Follows:
    `DiscordCoreAPI::ChannelStuff`
    
    `DiscordCoreAPI::EventManager`
    
    `DiscordCoreAPI::GuildMemberStuff`
    
    `DiscordCoreAPI::GuildStuff`
    
    `DiscordCoreAPI::DiscordCoreClient->interactions = shared_ptr<DiscordCoreAPI::InteractionManager>`
    
    `DiscordCoreAPI::MessageStuff`
    
    `DiscordCoreAPI::DiscordCoreClient->reactions = shared_ptr<DiscordCoreAPI::ReactionManager>`
    
    `DiscordCoreAPI::RoleStuff`
    
    `DiscordCoreAPI::SlashCommandStuff`
    
    `DiscordCoreAPI::UserStuff`
    
    `DiscordCoreAPI::InputEventManager`
    
    
- Each of the functions in this library takes an argument that is of a type who's name is derived from the function - for example, the data structure that gets passed into `DiscordCoreAPI::GuildManager::getGuildAsync()` is `DiscordCoreAPI::GetGuildData`.
- Most of the functions in this library are `Async`, meaning that they return a value of type `task<TaskType>`, which you call `.get()` on to retrieve the returned value, and to suspend the current thread in the process of waiting for it. Alternatively, you have the option of leaving the task's completion up to the background thread that is handling it, by not calling `.get()`.
- There is a copy of `shared_ptr<DiscordCoreAPI::DiscordCoreClient>` passed around on most of the members of this library - so be sure to refer back to it when looking for new members!
 
