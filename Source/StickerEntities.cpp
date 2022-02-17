// StickerEntities.cpp - Source file for the sticker related classes and structs.
// May 13, 2021
// Chris M.
// https://github.com/RealTimeChris

#include <StickerEntities.hpp>
#include <CoRoutine.hpp>
#include <Http.hpp>

namespace DiscordCoreAPI {

	Sticker::Sticker(StickerData dataNew) {
		this->description = dataNew.description;
		this->formatType = dataNew.formatType;
		this->sortValue = dataNew.sortValue;
		this->available = dataNew.available;
		this->guildId = dataNew.guildId;
		this->packId = dataNew.packId;
		this->asset = dataNew.asset;
		this->type = dataNew.type;
		this->name = dataNew.name;
		this->tags = dataNew.tags;
		this->user = dataNew.user;
		this->id = dataNew.id;
	};

	void Stickers::initialize(DiscordCoreInternal::HttpClient*theClient) {
		Stickers::httpClient = theClient;
	}

	CoRoutine<Sticker> Stickers::getStickerAsync(GetStickerData dataPackage) {
		try {
			co_await NewThreadAwaitable<Sticker>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Sticker;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/stickers/" + dataPackage.stickerId;
			workload.callStack = "Stickers::getStickerAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Sticker>(*Stickers::httpClient, workload);
		}
		catch (...) {
			reportException("Stickers::getStickerAsync()");
		}
	}

	CoRoutine<std::vector<StickerPackData>> Stickers::getNitroStickerPacksAsync() {
		try {
			co_await NewThreadAwaitable<std::vector<StickerPackData>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Nitro_Sticker_Packs;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/sticker-packs";
			workload.callStack = "Stickers::getNitroStickerPacksAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<StickerPackData>>(*Stickers::httpClient, workload);
		}
		catch (...) {
			reportException("Stickers::getNitroStickerPacksAsync()");
		}
	}

	CoRoutine<std::vector<Sticker>> Stickers::getGuildStickersAsync(GetGuildStickersData dataPackage) {
		try {
			co_await NewThreadAwaitable<std::vector<Sticker>>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Get_Guild_Stickers;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Get;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers";
			workload.callStack = "Stickers::getGuildStickersAsync";
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<std::vector<Sticker>>(*Stickers::httpClient, workload);
		}
		catch (...) {
			reportException("Stickers::getGuildStickersAsync()");
		}
	}

	CoRoutine<Sticker> Stickers::createGuildStickerAsync(CreateGuildStickerData dataPackage) {
		try {
			co_await NewThreadAwaitable<Sticker>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Post_Guild_Sticker;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Post;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers";
			nlohmann::json responseData = { {"description",dataPackage.description},{"name",dataPackage.name},{"tags",dataPackage.tags}, {"file",dataPackage.file} };
			workload.content = responseData.dump();
			workload.callStack = "Stickers::createGuildStickerAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Sticker>(*Stickers::httpClient, workload);
		}
		catch (...) {
			reportException("Stickers::createGuildStickerAsync()");
		}
	}

	CoRoutine<Sticker> Stickers::modifyGuildStickerAsync(ModifyGuildStickerData dataPackage) {
		try {
			co_await NewThreadAwaitable<Sticker>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Patch_Guild_Sticker;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Patch;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers/" + dataPackage.stickerId;
			nlohmann::json responseData = { {"description",dataPackage.description},{"name",dataPackage.name},{"tags",dataPackage.tags} };
			workload.content = responseData.dump();
			workload.callStack = "Stickers::modifyGuildStickerAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<Sticker>(*Stickers::httpClient, workload);
		}
		catch (...) {
			reportException("Stickers::modifyGuildStickerAsync()");
		}
	}

	CoRoutine<void> Stickers::deleteGuildStickerAsync(DeleteGuildStickerData dataPackage) {
		try {
			co_await NewThreadAwaitable<void>();
			DiscordCoreInternal::HttpWorkloadData workload{};
			workload.workloadType = DiscordCoreInternal::HttpWorkloadType::Delete_Guild_Sticker;
			workload.workloadClass = DiscordCoreInternal::HttpWorkloadClass::Delete;
			workload.relativePath = "/guilds/" + dataPackage.guildId + "/stickers/" + dataPackage.stickerId;
			workload.callStack = "Stickers::deleteGuildStickerAsync";
			if (dataPackage.reason != "") {
				workload.headersToInsert.insert(std::make_pair("X-Audit-Log-Reason", dataPackage.reason));
			}
			co_return DiscordCoreInternal::submitWorkloadAndGetResult<void>(*Stickers::httpClient, workload);
		}
		catch (...) {
			reportException("Stickers::deleteGuildStickerAsync()");
		}
	}
	DiscordCoreInternal::HttpClient* Stickers::httpClient{ nullptr };
};
