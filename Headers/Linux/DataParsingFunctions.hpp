// DataParsingFunctions.hpp - Header for the functions that will parse the Discord data into C++ types.
// Apr 21, 2021
// Chris M.
// https://github.com/RealTimeChris

#ifndef DATA_PARSING_FUNCTIONS
#define DATA_PARSING_FUNCTIONS

#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

    class DataParser {
    public:
        template<typename Type>
        static void parseObject(nlohmann::json const&, Type*);
    };

}
#endif