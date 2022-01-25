// DataParsingFunctions.hpp - Header for the functions that will parse the Discord data into C++ types.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

    class DataParser {
    public:
        template<typename Type>
        static void parseObject(nlohmann::json&, Type*);
    };

}