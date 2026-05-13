#pragma once

#include "BaseTag.h"
#include "Utils.h"

namespace Meipuru {
class ID3v2Tag : public BaseTag {
   public:
    void print() override;

    Util::Picture albumCover;
    std::string lyrics;
};
}  // namespace Meipuru
