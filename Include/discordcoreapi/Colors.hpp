/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.
	Copyright 2021, 2022 Chris M. (RealTimeChris)
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
/// Colors.hpp - Header for premade color constants
/// and structs. Nov 03, 2022 DJ::Oetzi
/// https://discordcoreapi.com
/// \file Colors.hpp
#pragma once
#ifndef COLORS

#include <string>

namespace DiscordCoreAPI {
	/// Several premade color constants ready to use for embeds
	namespace Colors {
		const std::string
			white = "FFFFFF",
			discord_white = "FFFFFE",
			light_gray = "C0C0C0",
			gray = "808080",
			dark_gray = "404040",
			black = "000000",
			discord_black = "000001",
			red = "FF0000",
			pink = "FFAFAF",
			orange = "FFC800",
			yellow = "FFFF00",
			green = "00FF00",
			magenta = "FF00FF",
			cyan = "00FFFF",
			blue = "0000FF",
			light_sea_green = "1ABC9C",
			medium_sea_green = "2ECC71",
			summer_sky = "3498DB",
			deep_lilac = "9B59B6",
			ruby = "E91E63",
			moon_yellow = "F1C40F",
			tahiti_gold = "E67E22",
			cinnabar = "E74C3C",
			submarine = "95A5A6",
			blue_aquamarine = "607D8B",
			deep_sea = "11806A",
			sea_green = "1F8B4C",
			endeavour = "206694",
			vivid_violet = "71368A",
			jazzberry_jam = "AD1457",
			dark_goldenrod = "C27C0E",
			rust = "A84300",
			brown = "992D22",
			gray_chateau = "979C9F",
			bismark = "546E7A",
			sti_blue = "0E4BEF",
			wrx_blue = "00247D",
			rallyart_crimson = "E60012",
			lime = "00FF00",
			forest_green = "228B22",
			cadmium_green = "097969",
			aquamarine = "7FFFD4",
			blue_green = "088F8F",
			raspberry = "E30B5C",
			scarlet_red = "FF2400";
	};
};
#endif
