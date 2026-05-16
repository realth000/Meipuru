#include "binding.h"

#include <frames/textidentificationframe.h>
#include <taglib/fileref.h>
#include <taglib/mpeg/id3v2/id3v2tag.h>
#include <taglib/mpeg/mpegfile.h>
#include <taglib/toolkit/tfile.h>
#include <taglib/toolkit/tmap.h>
#include <taglib/toolkit/tpropertymap.h>

#include <array>
#include <cstring>
#include <filesystem>
#include <numeric>
#include <optional>

namespace fs = std::filesystem;

struct BufferChunk {
    const std::string str;
    offset_t& offset;
    uint32_t& length;
};

template <typename Header, size_t N>
byte_t* buildBuffer(Header& header, std::array<BufferChunk, N>& chunks, uint32_t& bufferSize) {
    uint32_t acc = sizeof(Header);
    uint32_t strSize = 0;
    for (auto& chunk : chunks) {
        auto& [s, offset, length] = chunk;
        const auto delta = static_cast<uint32_t>(s.size());

        offset = acc;
        length = delta;
        strSize += delta;
        acc += delta;
    }

    const auto totalSize = sizeof(Header) + strSize * sizeof(byte_t);
    bufferSize = totalSize;

    byte_t* buffer = (byte_t*)malloc(totalSize);
    if (!buffer) {
        return nullptr;
    }
    std::memset(buffer, 0x00, totalSize);

    std::size_t offset = 0;
    std::memcpy(buffer, &header, sizeof(Header));
    offset += sizeof(Header);

    for (const auto& chunk : chunks) {
        if (chunk.str.empty()) {
            continue;
        }

        std::memcpy(buffer + offset, chunk.str.data(), chunk.str.size());
        offset += static_cast<uint32_t>(chunk.str.size());
    }

    return buffer;
}

struct MeipuruResource {
    MeipuruResource(const char* filePath)
        : filePath({filePath}),
          fileName{fs::path{filePath}.filename().string()},
          f{new TagLib::FileRef(TagLib::FileName(filePath))} {}

    ~MeipuruResource() { delete this->f; }

    const std::string filePath;
    const std::string fileName;
    const TagLib::FileRef* f;
};

/**
 * @brief Tag fields in basic tag and shared in different types of tags.
 *
 * These fields are shared because base tag is shared in types of tags.
 */
struct BasicTagFields {
    uint32_t year;
    uint32_t track;
    uint32_t albumTotalTrack;
    int32_t bitRate;
    int32_t sampleRate;
    int32_t channels;
    int32_t length;
    std::string title;
    std::string artist;
    std::string albumTitle;
    std::string albumArtist;
    std::string genre;
    std::string comment;
};

std::optional<BasicTagFields> parseBaseTagFields(TagLib::File* file) {
    if (!file) {
        return std::nullopt;
    }

    const auto baseTag = file->tag();
    if (baseTag == nullptr) {
        return std::nullopt;
    }

    BasicTagFields fields = {};

    fields.year = baseTag->year();
    fields.track = baseTag->track();

    const auto propertyMap = file->properties();

    if (const auto it = propertyMap.find("TRACKNUMBER"); it != propertyMap.end()) {
        const auto trackNumberString = it->second.toString();
        if (!trackNumberString.isEmpty()) {
            const auto pos = trackNumberString.split("/");
            if (pos.size() == 2) {
                // "1/20"
                fields.track = static_cast<uint32_t>(std::stoul(pos[0].to8Bit(true)));
                fields.albumTotalTrack = static_cast<uint32_t>(std::stoul(pos[1].to8Bit(true)));
            } else if (trackNumberString[0] == '/') {
                // "/20"
                fields.albumTotalTrack = static_cast<uint32_t>(std::stoul(pos[1].to8Bit(true)));
            } else {
                // "1", do nothing because track is already assigned.
            }
        }
    }

    const auto audioProperties = file->audioProperties();
    if (audioProperties != nullptr) {
        fields.bitRate = audioProperties->bitrate();
        fields.sampleRate = audioProperties->sampleRate();
        fields.channels = audioProperties->channels();
        fields.length = audioProperties->length();
    }

    fields.title = baseTag->title().to8Bit(true);
    fields.artist = baseTag->artist().to8Bit(true);
    fields.albumTitle = baseTag->album().to8Bit(true);
    if (auto it = propertyMap.find("ALBUMARTIST"); it != propertyMap.end()) {
        fields.albumArtist = it->second.toString().to8Bit(true);
    }
    fields.genre = baseTag->genre().to8Bit(true);
    fields.comment = baseTag->comment().to8Bit(true);

    return fields;
}

/////////////////////////////////////////////////
//
// Public APIs begin
//
/////////////////////////////////////////////////

MeipuruResource* meipuruMakeResource(const char* filePath) { return new MeipuruResource(filePath); }

void meipuruFreeResource(MeipuruResource* resource) { delete resource; }

MeipuruTagBuffer* meipuruGetReadonlyBaseTag(MeipuruResource* resource) {
    if (!resource) {
        return nullptr;
    }

    auto baseFields = parseBaseTagFields(resource->f->file());
    if (!baseFields.has_value()) {
        return nullptr;
    }

    auto header = MeipuruBaseTagHeader();
    header.year = baseFields->year;
    header.track = baseFields->track;
    header.albumTotalTrack = baseFields->albumTotalTrack;
    header.bitRate = baseFields->bitRate;
    header.sampleRate = baseFields->sampleRate;
    header.channels = baseFields->channels;
    header.length = baseFields->length;

    uint32_t bufferSize = 0;
    auto buffer = buildBuffer(header,
                              std::array<BufferChunk, 8>{{
                                  {resource->filePath, header.filePathOffset, header.filePathLength},
                                  {resource->fileName, header.fileNameOffset, header.fileNameLength},
                                  {baseFields->title, header.titleOffset, header.titleLength},
                                  {baseFields->artist, header.artistOffset, header.artistLength},
                                  {baseFields->albumTitle, header.albumTitleOffset, header.albumTitleLength},
                                  {baseFields->albumArtist, header.albumArtistOffset, header.albumArtistLength},
                                  {baseFields->genre, header.genreOffset, header.genreLength},
                                  {baseFields->comment, header.commentOffset, header.commentLength},
                              }},
                              bufferSize);

    auto tagBuffer = new MeipuruTagBuffer{buffer, bufferSize};
    return tagBuffer;
}

void meipuruFreeTagBuffer(MeipuruTagBuffer* tagBuffer) {
    if (!tagBuffer) {
        return;
    }

    free(tagBuffer->buffer);
    delete tagBuffer;
}

MeipuruTagBuffer* meipuruGetReadonlyID3v2Tag(MeipuruResource* resource) {
    if (!resource) {
        return nullptr;
    }

    auto mpegFile = dynamic_cast<TagLib::MPEG::File*>(resource->f->file());
    if (!mpegFile || !(mpegFile->hasID3v2Tag())) {
        return nullptr;
    }

    auto baseFields = parseBaseTagFields(mpegFile);
    if (!baseFields.has_value()) {
        return nullptr;
    }

    MeipuruBaseTagHeader baseHeader = {};
    baseHeader.year = baseFields->year;
    baseHeader.track = baseFields->track;
    baseHeader.albumTotalTrack = baseFields->albumTotalTrack;
    baseHeader.bitRate = baseFields->bitRate;
    baseHeader.sampleRate = baseFields->sampleRate;
    baseHeader.channels = baseFields->channels;
    baseHeader.length = baseFields->length;

    MeipuruID3v2TagHeader header = {};
    header.baseHeader = baseHeader;

    const auto* id3v2Tag = mpegFile->ID3v2Tag();
    if (!id3v2Tag) {
        return nullptr;
    }
    const auto frameListMap = id3v2Tag->frameListMap();

    std::string lyrics = {};
    if (const auto it = frameListMap.find("USLT"); it != frameListMap.end()) {
        lyrics = it->second.front()->toString().to8Bit(true);
    } else if (const auto it = frameListMap.find("TXXX"); it != frameListMap.end()) {
        // Text lyrics saved in TXXX.
        for (const auto* frame : it->second) {
            const auto* uTxFrame = dynamic_cast<const TagLib::ID3v2::UserTextIdentificationFrame*>(frame);
            if (uTxFrame) {
                const auto desc = uTxFrame->description().to8Bit(true);
                if (desc == "LYRICS" || desc == "lyrics") {
                    lyrics = uTxFrame->fieldList().back().to8Bit(true);
                }
            }
        }
    }

    uint32_t bufferSize = 0;
    auto buffer = buildBuffer(
        header,
        std::array<BufferChunk, 9>{{
            {resource->filePath, header.baseHeader.filePathOffset, header.baseHeader.filePathLength},
            {resource->fileName, header.baseHeader.fileNameOffset, header.baseHeader.fileNameLength},
            {baseFields->title, header.baseHeader.titleOffset, header.baseHeader.titleLength},
            {baseFields->artist, header.baseHeader.artistOffset, header.baseHeader.artistLength},
            {baseFields->albumTitle, header.baseHeader.albumTitleOffset, header.baseHeader.albumTitleLength},
            {baseFields->albumArtist, header.baseHeader.albumArtistOffset, header.baseHeader.albumArtistLength},
            {baseFields->genre, header.baseHeader.genreOffset, header.baseHeader.genreLength},
            {baseFields->comment, header.baseHeader.commentOffset, header.baseHeader.commentLength},
            {lyrics, header.lyricsOffset, header.lyricsLength},
        }},
        bufferSize);

    auto tagBuffer = new MeipuruTagBuffer{buffer, bufferSize};
    return tagBuffer;
}
