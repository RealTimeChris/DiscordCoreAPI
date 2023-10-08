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

namespace discord_core_api {

	/**
	 * \addtogroup voice_connection
	 * @{
	 */

	/// @brief A class representing the song apis.
	class DiscordCoreAPI_Dll song_api {
	  public:
		friend class discord_core_internal::sound_cloud_api;
		friend class discord_core_internal::you_tube_api;
		friend class voice_connection;
		friend class guild_cache_data;
		friend class guild_data;

		discord_core_internal::event<co_routine<void, false>, song_completion_event_data> onSongCompletionEvent{};
		unbounded_message_block<audio_frame_data> audioDataBuffer{};
		discord_core_internal::event_delegate_token eventToken{};

		song_api(snowflake guildId);

		/// @brief For setting up behavior in response to a completed song
		/// @param handler a delegate taking a song_completion_event_data structure as an argument.
		void onSongCompletion(std::function<co_routine<void, false>(song_completion_event_data)> handler);

		/// @brief Skips to the next song in the queue, if applicable.
		/// @param wasItAfail a bool representing whether or not this skip is due to a playing failure.
		/// @return a bool suggesting the success or failure of the skip command.
		bool skip(bool wasItAfail = false);

		/// @brief Search for a song to play.
		/// @param searchQuery the song to search for.
		/// @param limit The maximum number of search results to return.
		/// @return a vector of song objects representing the search results.
		jsonifier::vector<song> searchForSong(jsonifier::string_view searchQuery, uint64_t limit = 20);

		/// @brief Plays the current song. (assuming that you are currently connected to a voice_connection).
		/// @param songNew the song to play.
		/// @return a bool suggesting the success or failure of the play command.
		bool play(song songNew);

		/// @brief Checks if there is currently playing music for the current guild.
		/// @return a bool representing the currently playing status.
		bool areWeCurrentlyPlaying() const;

		/// @brief Toggles pausing on and off.
		/// @return a bool suggesting the success or failure of the pauseToggle command.
		bool pauseToggle();

		/// @brief Plays the current song. (assuming that you are currently connected to a voice_connection).
		/// @return a bool suggesting the success or failure of the play command.
		bool play();

		/// @brief Stops the currently playing song.
		/// @return a bool suggesting the success or failure of the stop command.
		bool stop();

	  protected:
		co_routine<void, false> taskThread{};
		std::recursive_mutex accessMutex{};
		snowflake guildId{};

		void disconnect();
	};
	/**@}*/
};
