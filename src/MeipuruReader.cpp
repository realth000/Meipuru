#include "MeipuruReader.h"

#include "tag.h"
#include "taglib/taglib/toolkit/tpropertymap.h"
#include "taglib/taglib/toolkit/tstring.h"
#include "taglib/taglib/mpeg/mpegfile.h"
#include "taglib/taglib/mpeg/id3v2/id3v2tag.h"
#include "taglib/taglib/mpeg/id3v2/frames/attachedpictureframe.h"

namespace Meipuru {

    MeipuruReaderOption::MeipuruReaderOption() {
        encoding = Util::Encoding::Unicode;
        logLevel = Util::LogLevel::Error;
    }

    BaseTag *MeipuruReader::readTagFromFile(const std::string &filePath) {
        std::cout << "Reading file" << filePath << std::endl;
        const TagLib::FileRef fileRef(filePath.c_str());
        if (fileRef.isNull()) {
            std::cout << "File is NULL:" << filePath << std::endl;
            return {};
        }

        auto baseTag = new BaseTag();
        baseTag->filePath = filePath;
        baseTag->fileName = fileRef.file()->name().toString().to8Bit(option.encoding == Util::Encoding::Unicode);
        fetchBaseTag(fileRef, baseTag);
        if (option.logLevel == Util::LogLevel::Debug) {
            std::cout << "Read tag from file:" << std::endl;
            baseTag->print();
        }
        return baseTag;
    }

    bool MeipuruReader::fetchBaseTag(const TagLib::FileRef &fileRef, BaseTag *baseTag) const {
        const auto *tag = fileRef.tag();
        if (tag == nullptr) {
            std::cout << "Tag is NULL:" << fileRef.file()->name().toString() << std::endl;
            return false;
        }
        const bool useUnicode = option.encoding == Util::Encoding::Unicode;
        baseTag->title = tag->title().to8Bit(useUnicode);
        baseTag->artist = tag->artist().to8Bit(useUnicode);
        baseTag->albumTitle = tag->album().to8Bit(useUnicode);
        baseTag->year = tag->year();
        baseTag->track = tag->track();
        baseTag->genre = tag->genre().to8Bit(useUnicode);
        baseTag->comment = tag->comment().to8Bit(useUnicode);

        const auto propertyMap = fileRef.file()->properties();
        // TODO: albumArtist is TagLib::string, should be std::string types.
        baseTag->albumArtist = propertyMap["ALBUMARTIST"].toString().to8Bit(useUnicode);
        const auto trackNumberString = propertyMap["TRACKNUMBER"].toString();
        if (!trackNumberString.isEmpty()) {
            const auto pos = trackNumberString.split("/");
            if (pos.size() == 2) {
                // "1/20"
                baseTag->track = pos[0].toInt();
                baseTag->albumTotalTrack = pos[1].toInt();
            } else if (trackNumberString[0] == '/') {
                // "/20"
                baseTag->albumTotalTrack = pos[1].toInt();
            } else {
                // "1"
                baseTag->track = trackNumberString.toInt();
            }
        }
        const auto *audioProperties = fileRef.file()->audioProperties();
        baseTag->bitRate = audioProperties->bitrate();
        baseTag->sampleRate = audioProperties->sampleRate();
        baseTag->channels = audioProperties->channels();
        baseTag->length = audioProperties->length();
        return true;
    }

    MeipuruReader::MeipuruReader(const MeipuruReaderOption &meipuruReaderOption) : option(meipuruReaderOption) {
    }

}
