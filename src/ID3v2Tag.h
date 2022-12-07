#ifndef MEIPURU_ID3V2TAG_H
#define MEIPURU_ID3V2TAG_H

#include "BaseTag.h"
#include "Utils.h"

namespace Meipuru {

    class ID3v2Tag : public BaseTag {
    public:
        Util::Picture albumCover;
        std::string lyrics;
    };
}

#endif //MEIPURU_ID3V2TAG_H
