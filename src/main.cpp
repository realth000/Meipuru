#include "fileref.h"
#include "tag.h"
#include <iostream>

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        std::cout << "Reading file" << argv[i] << std::endl;
        TagLib::FileRef fileRef(argv[i]);
        if (fileRef.isNull()) {
            std::cout << "File is NULL:" << argv[i] << std::endl;
            continue;
        }
        if (fileRef.tag() == nullptr) {
            std::cout << "Tag is NULL:" << argv[i] << std::endl;
            continue;
        }
        TagLib::Tag *tag = fileRef.tag();
        std::cout << "Title:" << tag->title() << "; Artist:" << tag->artist() << "; Album:" << tag->album() << "; Year:" << tag->year() << "; track:" << tag->track() << "; Genre:" << tag->genre() << "; comment:" << tag->comment() << std::endl;
    }
    return 0;
}
