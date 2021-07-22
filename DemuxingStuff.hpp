// DemuxingStuff.hpp - Header for the demuxing stuff.
// Jul 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#ifndef _DEMUXING_STUFF_
#define _DEMUXING_STUFF_

#include "../pch.h"

namespace DiscordCoreAPI{ 

    IBuffer demuxWebA(IBuffer bufferToDemux, __int64 currentPosition, __int64 totalByteSize) {
        IBuffer returnBuffer{ nullptr };

        return returnBuffer;
    }

}

#endif