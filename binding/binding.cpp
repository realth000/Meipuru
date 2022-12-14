#include "../include/binding.h"
#include "../include/MeipuruReader.h"

MeipuruTag *MeipuruReadTag(const char *filePath) {
    auto reader = Meipuru::MeipuruReader();
    auto tag = reader.readTagFromFile(filePath);

    auto *meipuruTag = new MeipuruTag{
            tag->filePath.c_str(),
            tag->fileName.c_str(),
            tag->title.c_str(),
            tag->artist.c_str(),
            tag->albumTitle.c_str(),
            tag->albumArtist.c_str(),
            tag->year,
            tag->track,
            tag->albumTotalTrack,
            tag->genre.c_str(),
            tag->comment.c_str(),
            tag->bitRate,
            tag->sampleRate,
            tag->channels,
            tag->length,
    };
    return meipuruTag;
}

MeipuruID3v2Tag *MeipuruReadID3v2Tag(const char *filePath) {
    auto reader = Meipuru::MeipuruReader();
    auto id3v2Tag = reader.readID3v2TagFromFile(filePath);

    if (id3v2Tag == nullptr) {
        return nullptr;
    }

    auto *meipuruID3v2Tag = new MeipuruID3v2Tag{
            id3v2Tag->filePath.c_str(),
            id3v2Tag->fileName.c_str(),
            id3v2Tag->title.c_str(),
            id3v2Tag->artist.c_str(),
            id3v2Tag->albumTitle.c_str(),
            id3v2Tag->albumArtist.c_str(),
            id3v2Tag->year,
            id3v2Tag->track,
            id3v2Tag->albumTotalTrack,
            id3v2Tag->genre.c_str(),
            id3v2Tag->comment.c_str(),
            id3v2Tag->bitRate,
            id3v2Tag->sampleRate,
            id3v2Tag->channels,
            id3v2Tag->length,
            id3v2Tag->lyrics.c_str(),
            (unsigned long) (id3v2Tag->lyrics.size()),
            id3v2Tag->albumCover.data,
            id3v2Tag->albumCover.size,
    };
    return meipuruID3v2Tag;
}

void MeipuruFreeTag(MeipuruTag *tag) {
    if (tag == nullptr) {
        return;
    }

    delete tag->filePath;
    delete tag->fileName;
    delete tag->title;
    delete tag->artist;
    delete tag->albumTitle;
    delete tag->albumArtist;
    delete tag->genre;
    delete tag->comment;

    delete tag;
    tag = nullptr;
}

void MeipuruFreeID3v2Tag(MeipuruID3v2Tag *id3V2Tag) {
    if (id3V2Tag == nullptr) {
        return;
    }

    delete id3V2Tag->filePath;
    delete id3V2Tag->fileName;
    delete id3V2Tag->title;
    delete id3V2Tag->artist;
    delete id3V2Tag->albumTitle;
    delete id3V2Tag->albumArtist;
    delete id3V2Tag->genre;
    delete id3V2Tag->comment;

    delete id3V2Tag->lyrics;
    delete id3V2Tag->albumCover;

    delete id3V2Tag;
    id3V2Tag = nullptr;
}
