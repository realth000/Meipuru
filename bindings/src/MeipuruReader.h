#pragma once

#include <taglib/mpeg/mpegfile.h>
#include <taglib/tag.h>

#include <iostream>

#include "BaseTag.h"
#include "ID3v2Tag.h"

namespace Meipuru {
class MeipuruReaderOption {
   public:
    explicit MeipuruReaderOption();

    bool useUnicode() const;

    Util::Encoding encoding;
    Util::LogLevel logLevel;

   private:
    bool unicode;
};

class MeipuruReader {
   public:
    explicit MeipuruReader(const MeipuruReaderOption& meipuruReaderOption = MeipuruReaderOption());

    BaseTag* readTagFromFile(const std::string& filePath);

    BaseTag* readTagFromFileW(const std::wstring& filePath);

    ID3v2Tag* readID3v2TagFromFile(const std::string& filePath);

    ID3v2Tag* readID3v2TagFromFileW(const std::wstring& filePath);

   private:
    bool fetchBaseTag(const TagLib::File* file, BaseTag* baseTag) const;

    MeipuruReaderOption option;
};
}  // namespace Meipuru
