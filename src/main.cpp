#include "fileref.h"
#include "tag.h"
#include <iostream>
#include "taglib/taglib/toolkit/tpropertymap.h"
#include "taglib/taglib/toolkit/tstring.h"
#include "taglib/taglib/mpeg/mpegfile.h"
#include "taglib/taglib/mpeg/id3v2/id3v2tag.h"

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        /*
         * Not consider files have id3V2 tags, treat like general tags.
         */
        std::cout << "Reading file" << argv[i] << std::endl;
        TagLib::FileRef fileRef(argv[i]);
        if (fileRef.isNull()) {
            std::cout << "File is NULL:" << argv[i] << std::endl;
            continue;
        }
        auto *tag = fileRef.tag();
        if (tag == nullptr) {
            std::cout << "Tag is NULL:" << argv[i] << std::endl;
            continue;
        }
        std::cout << "Title:" << tag->title() << "\n"
                  << "Artist:" << tag->artist() << "\n"
                  << "Album:" << tag->album() << "\n"
                  << "Year:" << tag->year() << "\n"
                  << "track:" << tag->track() << "\n"
                  << "Genre:" << tag->genre() << "\n"
                  << "comment:" << tag->comment() <<
                  std::endl;
        auto propertyMap = fileRef.file()->properties();
        // TODO: albumArtist is TagLib::string, should be std::string types.
        auto albumArtist = propertyMap["ALBUMARTIST"].toString();
        auto trackNumberString = propertyMap["TRACKNUMBER"].toString();
        int trackNumber = 0;
        int albumTrackCount = 0;
        if (!trackNumberString.isEmpty()) {
            auto pos = trackNumberString.split("/");
            if (pos.size() == 2) {
                // "1/20"
                trackNumber = pos[0].toInt();
                albumTrackCount = pos[1].toInt();
            } else if (trackNumberString[0] == '/') {
                // "/20"
                albumTrackCount = pos[1].toInt();
            } else {
                // "1"
                trackNumber = trackNumberString.toInt();
            }
        }
        std::cout << "AlbumArtist:" << albumArtist << "\n" <<
                  "TrackNumber:" << trackNumber << "\n" <<
                  "AlbumTrackCount:" << albumTrackCount << std::endl;
        const auto *audioProperties = fileRef.file()->audioProperties();
        int bitRate = audioProperties->bitrate();
        int sampleRate = audioProperties->sampleRate();
        int channels = audioProperties->channels();
        int length = audioProperties->length();
        std::cout << "Bit Rate:" << bitRate << "\n"
                  << "Sample Rate" << sampleRate << "\n"
                  << "Channels" << channels << "\n"
                  << "Length(seconds)" << length << std::endl;

    }
    for (int i = 1; i < argc; i++) {
        /*
         * Consider files have id3V2 tags, use specified methods to fetch more data.
         */
        // Second parameter (type bool) controls read file's audio properties or not.
        std::cout << "Reading file as id3v2" << argv[i] << std::endl;
        TagLib::MPEG::File mpegFile(argv[i], true);
        if (!mpegFile.hasID3v2Tag()) {
            continue;
        }
        auto id3v2Tag = mpegFile.ID3v2Tag();
        if (id3v2Tag == nullptr) {
            continue;
        }
        auto frameListMap = id3v2Tag->frameListMap();
        TagLib::String syncedLyrics = "";
        if (!frameListMap["SYLT"].isEmpty()) {
            syncedLyrics = frameListMap["SYLT"].front()->toString();
        }
        TagLib::String unsyncedLyrics = "";
        if (!frameListMap["USLT"].isEmpty()) {
            unsyncedLyrics = frameListMap["USLT"].front()->toString();
        }
        std::cout << "Unsychronised lyrics:" << unsyncedLyrics << "\n"
                  << "Sychronised lyrics:" << syncedLyrics << "\n"
                  << std::endl;
    }
    return 0;
}
