#include "binding.h"

#include <taglib/fileref.h>
#include <taglib/toolkit/tfile.h>
#include <taglib/toolkit/tmap.h>
#include <taglib/toolkit/tpropertymap.h>

#include <array>
#include <cstring>
#include <filesystem>
#include <numeric>

#include "MeipuruReader.h"

namespace fs = std::filesystem;

typedef struct {
    std::string str;
    offset_t& offset;
    uint32_t& length;
} BufferChunk;

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

MeipuruResource* meipuruMakeResource(const char* filePath) { return new MeipuruResource(filePath); }

void meipuruFreeResource(MeipuruResource* resource) { delete resource; }

MeipuruBaseTagBuffer* meipuruGetReadonlyBaseTag(MeipuruResource* resource) {
    const auto file = resource->f;
    if (file == nullptr) {
        return nullptr;
    }

    const auto baseTag = file->tag();
    if (baseTag == nullptr) {
        return nullptr;
    }

    auto header = MeipuruBaseTagHeader();
    memset(&header, 0x00, sizeof(MeipuruBaseTagHeader));

    header.year = baseTag->year();
    header.track = baseTag->track();

    const auto propertyMap = file->properties();

    if (const auto it = propertyMap.find("TRACKNUMBER"); it != propertyMap.end()) {
        const auto trackNumberString = it->second.toString();
        if (!trackNumberString.isEmpty()) {
            const auto pos = trackNumberString.split("/");
            if (pos.size() == 2) {
                // "1/20"
                header.track = static_cast<uint32_t>(std::stoul(pos[0].to8Bit(true)));
                header.albumTotalTrack = static_cast<uint32_t>(std::stoul(pos[1].to8Bit(true)));
            } else if (trackNumberString[0] == '/') {
                // "/20"
                header.albumTotalTrack = static_cast<uint32_t>(std::stoul(pos[1].to8Bit(true)));
            } else {
                // "1", do nothing because track is already assigned.
            }
        }
    }

    const auto audioProperties = resource->f->audioProperties();
    if (audioProperties != nullptr) {
        header.bitRate = audioProperties->bitrate();
        header.sampleRate = audioProperties->sampleRate();
        header.channels = audioProperties->channels();
        header.length = audioProperties->length();
    }

    const auto filePath = resource->filePath;
    const auto fileName = resource->fileName;
    const auto title = baseTag->title().to8Bit(true);
    const auto artist = baseTag->artist().to8Bit(true);
    const auto albumTitle = baseTag->album().to8Bit(true);
    std::string albumArtist = {};
    if (auto it = propertyMap.find("ALBUMARTIST"); it != propertyMap.end()) {
        albumArtist = it->second.toString().to8Bit(true);
    }
    const auto genre = baseTag->genre().to8Bit(true);
    const auto comment = baseTag->comment().to8Bit(true);

    uint32_t bufferSize = 0;
    auto buffer = buildBuffer(header,
                              std::array<BufferChunk, 8>{{
                                  {filePath, header.filePathOffset, header.filePathLength},
                                  {fileName, header.fileNameOffset, header.fileNameLength},
                                  {title, header.titleOffset, header.titleLength},
                                  {artist, header.artistOffset, header.artistLength},
                                  {albumTitle, header.albumTitleOffset, header.albumTitleLength},
                                  {albumArtist, header.albumArtistOffset, header.albumArtistLength},
                                  {genre, header.genreOffset, header.genreLength},
                                  {comment, header.commentOffset, header.commentLength},
                              }},
                              bufferSize);

    auto tagBuffer = new MeipuruBaseTagBuffer{buffer, bufferSize};
    return tagBuffer;
}

void meipuruFreeBaseTag(MeipuruBaseTagBuffer* tagBuffer) {
    free(tagBuffer->buffer);
    delete tagBuffer;
}

MeipuruID3v2TagBuffer* readId3V2Tag(MeipuruResource* resource) {
    const auto* baseTag = resource->f->tag();
    TagLib::MPEG::File* id3v2File = dynamic_cast<TagLib::MPEG::File*>(resource->f->file());
    if (id3v2File == nullptr) {
        return nullptr;
    }

    // TODO: Generate buffer.
    return nullptr;
}

int setTitle(MeipuruResource* resource, const char* title) {
    // TODO: Save title to file.
    return 0;
}

int setArtist(MeipuruResource* resource, const char* artist) {
    // TODO: Save artist to file.
    return 0;
}

int setID3v2lyrics(MeipuruResource* resource, const char* lyrics) { return 0; }
