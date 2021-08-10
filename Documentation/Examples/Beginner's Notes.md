### **Beginner's Notes:**
---
- All of the central class managers are either members of the class `DiscordCoreAPI::DiscordCoreClient` and are of type `shared_ptr<ManagerType>`, or members of the namespace `DiscordCoreAPI`.
  ##### They are as Follows:
    `DiscordCoreAPI::DiscordCoreClient->channels = shared_ptr<DiscordCoreAPI::ChannelManager>`
- Each of the functions takes an argument that is of a type who's name is derived from the function - for example, the data structure that gets passed into `DiscordCoreAPI::GuildManager::getGuildAsync()` is `DiscordCoreAPI::GetGuildData`.
- Most of the functions in this library are `Async`, meaning that they return a value of type `task<TaskType>`, which you call `.get()` on to retrieve the returned value, and to suspend the current thread in the process of waiting for it.
- There is a copy of `shared_ptr<DiscordCoreAPI::DiscordCoreClient>` passed around on most of the members of this library - so be sure to refer back to it when looking for new members!
