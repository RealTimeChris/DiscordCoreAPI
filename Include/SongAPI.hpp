// SongAPI.hpp - Header for the song api related stuff.
// Sep 17, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include <FoundationEntities.hpp>
#include <GuildMemberEntities.hpp>
#include <EventEntities.hpp>

namespace DiscordCoreAPI {

	/**
	* \addtogroup voice_connection
	* @{
	*/
	/// A class DiscordCoreAPI_Dll representing the Song APIs. \brief A class DiscordCoreAPI_Dll representing the Song APIs.
	class DiscordCoreAPI_Dll SongAPI {
	public:

		friend class VoiceConnection;
		friend class SoundCloudAPI;
		friend class YouTubeAPI;
		
		UniEvent<CoRoutine<void>, SongCompletionEventData> onSongCompletionEvent{};
		Playlist playlist{};

		SongAPI(std::string guildId);

		/// For setting up behavior in response to a completed song. \brief For setting up behavior in response to a completed song.
		/// \param handler A delegate taking a SongCompletionEventData structure as an argument.
		/// \param guildId The id of the guild for which you would like to instantiate this event.
		/// \returns An event_token for later de-registering the event. 
		static void onSongCompletion(std::function<CoRoutine<void>(SongCompletionEventData)> handler, std::string guildId);

		/// Search for a Song to play. \brief Search for a Song to play.
		/// \param searchQuery The Song to search for.
		/// \param guildId The Guild id to search for the Song within.
		/// \returns A std::vector of Song objects representing the search results.
		static std::vector<Song> searchForSong(std::string searchQuery, std::string guildId);

		/// Adds a Song to the current Playlist's UnboundedMessageBlock. \brief Adds a Song to the current Playlist's UnboundedMessageBlock.
		/// \param guildMember The GuildMember that is adding the Song to the UnboundedMessageBlock.
		/// \param song The Song to be added to the UnboundedMessageBlock.
		/// \returns The Song that was added to the UnboundedMessageBlock.
		static Song addSongToQueue(GuildMember guildMember, Song song);

		/// Checks to see if there are any playable Songs in the current Playlist. \brief Checks to see if there are any playable Songs in the current Playlist.
		/// \param guildId The id of the Guild for which we would like to check its Playlist for Songs.
		/// \returns A bool representing whether there are any playable Songs.
		static bool isThereAnySongs(std::string guildId);

		/// Send the next playable song off of the current Guild's Playlist to be played. \brief Send the next playable song off of the current Guild's Playlist to be played.
		/// \param guildMember The GuildMember who is requesting the Song to be sent.
		/// \returns A bool suggesting the success status of the send.
		static bool sendNextSong(GuildMember guildMember);

		/// Plays the current Song. (Assuming that you are currently connected to a VoiceConnection). \brief Plays the current Song. (Assuming that you are currently connected to a VoiceConnection).
		/// \param guildId The id of the Guild within which to play the current Song.
		/// \returns A bool suggesting the success or failure of the play command.
		static bool play(std::string guildId);

		/// Skips to the next Song in the UnboundedMessageBlock, if applicable. \brief Skips to the next Song in the UnboundedMessageBlock, if applicable.
		/// \param guildMember The GuildMember structure of the individual who is skipping the Song.
		static void skip(GuildMember guildMember);

		/// Stops the currently playing Song. \brief Stops the currently playing Song.
		/// \param guildId The id of the Guild within which to stop the currently playing music.
		static void stop(std::string guildId);

		/// Toggles pausing on and off. \brief Toggles pausing on and off.
		/// \param guildId The id of the Guild which you would like to pause the Song for.
		static void pauseToggle(std::string guildId);

		/// Checks if there is currently playing music for the current Guild. \brief Checks if there is currently playing music for the current Guild.
		/// \param guildId The id for the desired Guild to check the current playing status.
		/// \returns A bool representing the currently playing status.
		static bool areWeCurrentlyPlaying(std::string guildId);

		/// Collects the Playlist from the SongAPI. \brief Collects the Playlist from the SongAPI.
		/// \param guildId The Guild for which to collect the Playlist from.
		/// \returns A Playlist.
		static Playlist getPlaylist(std::string guildId);

		/// Sets the playlist of the desired Guild. \brief Sets the playlist of the desired Guild.
		/// \param playlistNew The new Playlist to be set.
		/// \param guildId The id of the desired Guild to set the Playlist of.
		static void setPlaylist(Playlist playlistNew, std::string guildId);

		/// Returns the current loop-all status of the current Guild's Playlist. \brief Returns the current loop-all status of the current Guild's Playlist.
		/// \param guildId The id of the Guild for which you would like to check the loop-all status of.
		/// \returns A bool representing the current loop-all status.
		static bool isLoopAllEnabled(std::string guildId);

		/// Enables or disables the loop-all status of the playlist. \brief Enables or disables the loop-all status of the playlist.
		/// \param enabled A bool representing whether or not to enable the loop-all status.
		/// \param guildId The Guild id for which Guild to update this status in.
		static void setLoopAllStatus(bool enabled, std::string guildId);

		/// Returns the current loop-song of the current Guild's Playlist. \brief Returns the current loop-song of the current Guild's Playlist.
		/// \param guildId The id of the Guild for which you would like to check the loop-song status of.
		/// \returns A bool representing the current loop-song status.
		static bool isLoopSongEnabled(std::string guildId);

		/// Enables or disables the loop-song status of the playlist. \brief Enables or disables the loop-song status of the playlist.
		/// \param enabled A bool representing whether or not to enable the loop-song status.
		/// \param guildId The Guild id for which Guild to update this status in.
		static void setLoopSongStatus(bool enabled, std::string guildId);		

		/// Sets the position of a Song in the current Playlist's song UnboundedMessageBlock. \brief Sets the position of a Song in the current Playlist's song UnboundedMessageBlock.
		/// \param firstSongPosition The first Song's initial position.
		/// \param secondSongPosition The first Song's final position.
		/// \param guildId The id of the desired Guild to update the Song positions in.
		static void modifyQueue(int32_t firstSongPosition, int32_t secondSongPosition, std::string guildId);

		/// Collects the currently playing Song. \brief Collects the currently playing Song.
		/// \param guildId The id for which Guild to collect the current Song of.
		/// \returns The current Song for the given Guild.
		static Song getCurrentSong(std::string guildId);

		/// Sets the currently playing Song. \brief Sets the currently playing Song.
		/// \param song The Song to set as the currently playing Song.
		/// \param guildId The id for which Guild to set the current Song of.
		static void setCurrentSong(Song song, std::string guildId);

	protected:

		static std::mutex accessMutex;

		std::unique_ptr<CoRoutine<void>>theTask{ nullptr };
		bool areWeInstantiated{ false };
		std::string guildId{ "" };

		bool sendNextSong(std::string);
	};
	/**@}*/
};
