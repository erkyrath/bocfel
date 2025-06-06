#ifndef ZTERP_GLKAUTOSAVE_H
#define ZTERP_GLKAUTOSAVE_H

enum class StreamRock : glui32 {
    BlorbStream = 1,
};

bool glkautosave_library_autosave();
bool glkautosave_library_autorestore();

#endif

