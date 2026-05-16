#pragma once

#include <stdint.h>

#ifdef _WIN32
#define MEIPURU_API __declspec(dllexport)
#else
#define MEIPURU_API __attribute__((visibility("default")))
#endif

typedef uint32_t offset_t;
typedef uint8_t byte_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MeipuruResource MeipuruResource;

MEIPURU_API MeipuruResource* meipuruMakeResource(const char* filePath);

MEIPURU_API void meipuruFreeResource(MeipuruResource* resource);

typedef struct {
    // sizeof(*buffer) = this.size
    byte_t* buffer;

    // size = sizeof(SomeTypeHeader) + sumOf(all_offset_fields_length)
    uint32_t size;
} MeipuruTagBuffer;

MEIPURU_API void meipuruFreeTagBuffer(MeipuruTagBuffer* tagBuffer);

typedef struct {
    uint32_t year;
    uint32_t track;
    uint32_t albumTotalTrack;
    int32_t bitRate;
    int32_t sampleRate;
    int32_t channels;
    int32_t length;

    offset_t filePathOffset;
    uint32_t filePathLength;

    offset_t fileNameOffset;
    uint32_t fileNameLength;

    offset_t titleOffset;
    uint32_t titleLength;

    offset_t artistOffset;
    uint32_t artistLength;

    offset_t albumTitleOffset;
    uint32_t albumTitleLength;

    offset_t albumArtistOffset;
    uint32_t albumArtistLength;

    offset_t genreOffset;
    uint32_t genreLength;

    offset_t commentOffset;
    uint32_t commentLength;
} MeipuruBaseTagHeader;

/**
 * Read tag data in the file resource `resource`.
 *
 * Returns a buffer contains:
 *
 * 1. `MeipuruBaseTagHeader` in the front of buffer.
 * 2. Data (whose offset specified in header) after header.
 *
 * The returned buffer is unmodifiable.
 */
MEIPURU_API MeipuruTagBuffer* meipuruGetReadonlyBaseTag(MeipuruResource* resource);

typedef struct {
    MeipuruBaseTagHeader baseHeader;

    offset_t lyricsOffset;
    uint32_t lyricsLength;

    offset_t albumCoverOffset;
    uint32_t albumCoverLength;
} MeipuruID3v2TagHeader;

/**
 * Read ID3v2 tag data in the file resource `resource`.
 *
 * Returns a buffer contains:
 *
 * 1. `MeipuruID3v2TagHeader` in the front of buffer.
 * 2. Data (whose offset specified in header) after header.
 *
 * The returned buffer is unmodifiable.
 */
MEIPURU_API MeipuruTagBuffer* meipuruGetReadonlyID3v2Tag(MeipuruResource* resource);

#ifdef __cplusplus
}
#endif