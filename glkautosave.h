#ifndef ZTERP_GLKAUTOSAVE_H
#define ZTERP_GLKAUTOSAVE_H

enum class StreamRock : glui32 {
    None = 0,
    BlorbStream = 1,
    TranscriptStream = 2,
};

bool glkautosave_library_autosave();
bool glkautosave_library_autorestore();

#endif

