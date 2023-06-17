/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// SongAPI.hpp - Header for the song api related stuff.
/// Sep 17, 2021
/// https://discordcoreapi.com
/// \file SongAPI.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>
#include <discordcoreapi/GuildMemberEntities.hpp>
#include <discordcoreapi/VoiceConnection.hpp>

namespace DiscordCoreAPI {

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	/// \brief A class representing the Song APIs.
	class DiscordCoreAPI_Dll SongAPI {
	  public:
		friend class DiscordCoreInternal::SoundCloudAPI;
		friend class DiscordCoreInternal::YouTubeAPI;
		friend class VoiceConnection;
		friend class GuildData;

		DiscordCoreInternal::Event<CoRoutine<void>, SongCompletionEventData> onSongCompletionEvent{};
		UnboundedMessageBlock<AudioFrameData> audioDataBuffer{};
		DiscordCoreInternal::EventDelegateToken eventToken{};

		SongAPI(Snowflake guildId);

		/// \brief For setting up behavior in response to a completed song
		/// \param handler A delegate taking a SongCompletionEventData structure as an argument.
		/// \param guildId The id of the Guild for which you would like to instantiate this event.
		void onSongCompletion(std::function<CoRoutine<void>(SongCompletionEventData)> handler, const Snowflake guildId) noexcept;

		/// \brief Search for a Song to play.
		/// \param searchQuery The Song to search for.
		/// \param guildId The Guild id to search for the Song within.
		/// \returns A vector of Song objects representing the search results.
		std::vector<Song> searchForSong(const std::string& searchQuery) noexcept;

		/// \brief Plays the current Song. (Assuming that you are currently connected to a VoiceConnection).
		/// \param songNew The song to play.
		/// \param guildMember The GuildMember that is running this song.
		/// \returns A bool suggesting the success or failure of the play command.
		bool play(Song songNew, const GuildMember& guildId) noexcept;

		/// \brief Plays the current Song. (Assuming that you are currently connected to a VoiceConnection).
		/// \returns A bool suggesting the success or failure of the play command.
		bool play(Snowflake guildId) noexcept;

		/// \brief Skips to the next Song in the queue, if applicable.
		/// \param guildMember The GuildMember structure of the individual who is skipping the Song.
		void skip(Song songNew, const GuildMember& guildId) noexcept;

		/// \brief Stops the currently playing Song.
		/// \param guildId The id of the Guild within which to stop the currently playing music.
		void stop() noexcept;

		/// \brief Toggles pausing on and off.
		/// \param guildId The id of the Guild which you would like to pause the Song for.
		void pauseToggle() noexcept;

		/// \brief Checks if there is currently playing music for the current Guild.
		/// \param guildId The id for the desired Guild to check the current playing status.
		/// \returns A bool representing the currently playing status.
		bool areWeCurrentlyPlaying() const noexcept;

		~SongAPI() noexcept;

	  protected:
		UniquePtr<std::jthread> taskThread{};
		std::recursive_mutex accessMutex{};
		Snowflake guildId{};

		void disconnect() noexcept;
	};
	/**@}*/
};
