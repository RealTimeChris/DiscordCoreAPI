/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
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

		/// \brief Skips to the next Song in the queue, if applicable.
		/// \param guildMember The GuildMember structure of the individual who is skipping the Song.
		/// \param wasItAFail A bool representing whether or not this skip is due to a playing failure.
		/// \returns A bool suggesting the success or failure of the skip command.
		bool skip(const GuildMember& guildMember, bool wasItAfail = false) noexcept;

		/// \brief Search for a Song to play.
		/// \param searchQuery The Song to search for.
		/// \returns A vector of Song objects representing the search results.
		std::vector<Song> searchForSong(const std::string& searchQuery) noexcept;

		/// \brief Plays the current Song. (Assuming that you are currently connected to a VoiceConnection).
		/// \param songNew The song to play.
		/// \param guildMember The GuildMember that is running this song.
		/// \returns A bool suggesting the success or failure of the play command.
		bool play(Song songNew, const GuildMember& guildMember) noexcept;

		/// \brief Checks if there is currently playing music for the current Guild.
		/// \returns A bool representing the currently playing status.
		bool areWeCurrentlyPlaying() const noexcept;

		/// \brief Plays the current Song. (Assuming that you are currently connected to a VoiceConnection).
		/// \param guildId The id of the guild that the VoiceConnection belongs to.
		/// \returns A bool suggesting the success or failure of the play command.
		bool play(Snowflake guildId) noexcept;

		/// \brief Toggles pausing on and off.
		/// \returns A bool suggesting the success or failure of the pauseToggle command.
		bool pauseToggle() noexcept;

		/// \brief Stops the currently playing Song.
		/// \returns A bool suggesting the success or failure of the stop command.
		bool stop() noexcept;

		~SongAPI() noexcept;

	  protected:
		UniquePtr<std::jthread> taskThread{};
		std::recursive_mutex accessMutex{};
		Snowflake guildId{};

		void disconnect() noexcept;
	};
	/**@}*/
};
