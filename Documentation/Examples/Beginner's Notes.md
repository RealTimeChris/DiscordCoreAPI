### **Beginner's Notes:**
---
- All of the central class managers are either members of the class `DiscordCoreAPI::DiscordCoreClient` and are of type `shared_ptr<ManagerType>`, or members of the namespace `DiscordCoreAPI`.
- Each of the functions takes an argument that is of a type who's name is derived from the function - for example, the data structure that gets passed into `DiscordCoreAPI::GuildManager::getGuildAsync()` is `DiscordCoreAPI::GetGuildData`.

```cpp
```
