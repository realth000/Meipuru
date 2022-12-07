#ifndef MEIPURU_MEIPURUREADER_H
#define MEIPURU_MEIPURUREADER_H

#include <iostream>
#include "BaseTag.h"
#include "ID3v2Tag.h"
#include "fileref.h"

namespace Meipuru {
    class MeipuruReaderOption {
    public:
        explicit MeipuruReaderOption();

        Util::Encoding encoding;
        Util::LogLevel logLevel;
    };

    class MeipuruReader {
    public:
        explicit MeipuruReader(const MeipuruReaderOption &meipuruReaderOption = MeipuruReaderOption());

        BaseTag *readTagFromFile(const std::string &filePath);

    private:
        bool fetchBaseTag(const TagLib::FileRef &fileRef, BaseTag *baseTag) const;

        MeipuruReaderOption option;
    };
}

#endif//MEIPURU_MEIPURUREADER_H
