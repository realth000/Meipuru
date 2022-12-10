#ifndef MEIPURU_MEIPURUEXPORT_H
#define MEIPURU_MEIPURUEXPORT_H

#ifdef MEIPURU_LIB
#if (defined(_WIN32) || defined(_WIN64))
#define MEIPURU_EXPORT __declspec(dllexport)
#else
#define MEIPURU_EXPORT __declspec(dllimport)
#endif // MSVC
#elif defined(__GNUC__)
#define MEIPURU_EXPORT __attribute__ ((visibility("default")))
#else
#define MEIPURU_EXPORT
#endif

#endif //MEIPURU_MEIPURUEXPORT_H
