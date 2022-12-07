#include <fstream>
#include <iostream>

#include "fileref.h"
#include "taglib/taglib/toolkit/tpropertymap.h"
#include "taglib/taglib/toolkit/tstring.h"
#include "taglib/taglib/mpeg/mpegfile.h"
#include "taglib/taglib/mpeg/id3v2/id3v2tag.h"
#include "taglib/taglib/mpeg/id3v2/frames/attachedpictureframe.h"

#include "MeipuruReader.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " [audio file]";
        return 0;
    }
    auto meipuruReader = Meipuru::MeipuruReader();
    auto baseTag = meipuruReader.readTagFromFile(argv[1]);
    baseTag->print();
    return 0;

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
                  << "Sychronised lyrics:" << syncedLyrics
                  << std::endl;
        TagLib::ID3v2::AttachedPictureFrame *albumCover = nullptr;
        if (!frameListMap["APIC"].isEmpty()) {
            albumCover = reinterpret_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameListMap["APIC"].front());
            if (albumCover != nullptr) {
                std::cout << "Album Cover: YES" << albumCover->mimeType() << std::endl;
                std::fstream picStream;
                picStream.open("./test.jpg", std::ios::out | std::ios::binary | std::ios::trunc);
                picStream.write(albumCover->picture().data(), albumCover->picture().size());
                picStream.close();
            }
        }
    }
    return 0;
}
