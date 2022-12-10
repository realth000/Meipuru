#ifndef MEIPURU_MEIPURUREADER_H
#define MEIPURU_MEIPURUREADER_H

#include <iostream>

#include "fileref.h"
#include "tag.h"

#include "src/BaseTag.h"
#include "src/ID3v2Tag.h"
#include "src/MeipuruExport.h"

namespace Meipuru {
    class MEIPURU_EXPORT MeipuruReaderOption {
    public:
        explicit MeipuruReaderOption();

        bool useUnicode() const;

        Util::Encoding encoding;
        Util::LogLevel logLevel;

    private:
        bool unicode;
    };

    class MEIPURU_EXPORT MeipuruReader {
    public:
        explicit MeipuruReader(const MeipuruReaderOption &meipuruReaderOption = MeipuruReaderOption());

        BaseTag *readTagFromFile(const std::string &filePath);

        ID3v2Tag *readID3v2TagFromFile(const std::string &filePath);

    private:
        bool fetchBaseTag(const TagLib::File *file, BaseTag *baseTag) const;

        MeipuruReaderOption option;
    };
}

#endif//MEIPURU_MEIPURUREADER_H
