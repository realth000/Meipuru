#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binding.h"

typedef struct {
    /**
     * Buffer to walk through.
     */
    byte_t* buffer;

    /**
     * Buffer size in bytes.
     */
    uint32_t size;

    /**
     * Current offset.
     */
    uint32_t offset;
} BufferWalker;

bool hasSize(BufferWalker* walker, uint32_t size) { return walker->size > (walker->offset + size); }

uint32_t* readUint32(BufferWalker* walker) {
    if (!hasSize(walker, sizeof(uint32_t))) {
        return NULL;
    }

    uint32_t* value = (uint32_t*)(walker->buffer + walker->offset);
    walker->offset += sizeof(uint32_t);

    return value;
}

int32_t* readInt32(BufferWalker* walker) {
    if (!hasSize(walker, sizeof(int32_t))) {
        return NULL;
    }

    int32_t* value = (int32_t*)(walker->buffer + walker->offset);
    walker->offset += sizeof(int32_t);

    return value;
}

offset_t* readOffset(BufferWalker* walker) {
    if (!hasSize(walker, sizeof(offset_t))) {
        return NULL;
    }

    offset_t* value = (offset_t*)(walker->buffer + walker->offset);
    walker->offset += sizeof(offset_t);

    return value;
}

/**
 * The returned string is not '\0' terminated. To print it, use `printf("%.*s")`.
 */
const char* readStringNotTerminated(BufferWalker* walker, uint32_t* length) {
    if (!length || !hasSize(walker, sizeof(offset_t) + sizeof(uint32_t))) {
        return NULL;
    }

    offset_t* stringOffset = readOffset(walker);
    if (!stringOffset) {
        return NULL;
    }

    uint32_t* stringLength = readUint32(walker);
    if (!stringLength) {
        return NULL;
    }

    *length = *stringLength;

    offset_t offset = *stringOffset;
    const char* contentPtr = (const char*)(walker->buffer) + offset;
    return contentPtr;
}

void printUint32Field(BufferWalker* walker, const char* fieldName) {
    uint32_t* value = readUint32(walker);
    if (value) {
        printf("%s=%u\n", fieldName, *value);
    } else {
        printf("%s=<NULL>\n", fieldName);
        exit(1);
    }
}

void printInt32Field(BufferWalker* walker, const char* fieldName) {
    int32_t* value = readInt32(walker);
    if (value) {
        printf("%s=%d\n", fieldName, *value);
    } else {
        printf("%s=<NULL>\n", fieldName);
        exit(1);
    }
}

void printStringField(BufferWalker* walker, const char* fieldName) {
    uint32_t fieldLength = 0;
    const char* fieldValue = readStringNotTerminated(walker, &fieldLength);
    if (fieldValue) {
        if (fieldLength == 0) {
            printf("%s=<zero-size>\n", fieldName);
        } else {
            printf("%s=%.*s\n", fieldName, (int)fieldLength, fieldValue);
        }
    } else {
        printf("%s=<NULL>\n", fieldName);
        exit(1);
    }
}

int printBaseTag(MeipuruBaseTagBuffer* tagBuffer) {
    if (!tagBuffer) {
        printf("null tag buffer\n");
        return 1;
    }

    byte_t* buffer = tagBuffer->buffer;
    if (!buffer) {
        printf("null buffer\n");
        return 1;
    }

    BufferWalker walker = {.buffer = tagBuffer->buffer, .offset = 0, .size = tagBuffer->size};
    printUint32Field(&walker, "year");
    printUint32Field(&walker, "track");
    printUint32Field(&walker, "albumTotalTrack");
    printInt32Field(&walker, "bitRate");
    printInt32Field(&walker, "sampleRate");
    printInt32Field(&walker, "channels");
    printInt32Field(&walker, "length");
    printStringField(&walker, "filePath");
    printStringField(&walker, "fileName");
    printStringField(&walker, "title");
    printStringField(&walker, "artist");
    printStringField(&walker, "albumTitle");
    printStringField(&walker, "albumArtist");
    printStringField(&walker, "genre");
    printStringField(&walker, "comment");

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s [audio file]", argv[0]);
        return 0;
    }

    MeipuruResource* resource = meipuruMakeResource(argv[1]);
    if (!resource) {
        printf("failed to make resource\n");
        return 1;
    }

    MeipuruBaseTagBuffer* tagBuffer = meipuruGetReadonlyBaseTag(resource);
    int ret = printBaseTag(tagBuffer);
    meipuruFreeResource(resource);
}
