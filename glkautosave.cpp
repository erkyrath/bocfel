// Copyright 2010-2021 Chris Spiegel.
//
// SPDX-License-Identifier: MIT

#include <initializer_list>
#include <string>

#include "options.h"
#include "osdep.h"
#include "glkautosave.h"

extern "C" {
#include <gi_blorb.h>
#include <glkstart.h>
}

bool glkautosave_library_autosave()
{
#ifdef GLKUNIX_AUTOSAVE_FEATURES

    auto autosavepath = zterp_os_autosave_name();
    std::string pathname = *autosavepath + ".json";
    
    strid_t jsavefile = glkunix_stream_open_pathname_gen(const_cast<char *>(pathname.c_str()), TRUE, FALSE, 1);
    if (!jsavefile) {
        return false;
    }
    
    glkunix_save_library_state(jsavefile, jsavefile, NULL, NULL);

    glk_stream_close(jsavefile, NULL);
    jsavefile = NULL;
    
#endif /* GLKUNIX_AUTOSAVE_FEATURES */

    return true;
}

bool glkautosave_library_autorestore()
{
#ifdef GLKUNIX_AUTOSAVE_FEATURES
    
    auto autosavepath = zterp_os_autosave_name();
    std::string pathname = *autosavepath + ".json";

    strid_t jsavefile = glkunix_stream_open_pathname_gen(const_cast<char *>(pathname.c_str()), FALSE, FALSE, 1);
    if (!jsavefile) {
        return false;
    }
    
    glkunix_library_state_t library_state = glkunix_load_library_state(jsavefile, NULL, NULL);

    glk_stream_close(jsavefile, NULL);
    jsavefile = NULL;
    
    if (!library_state) {
        return false;
    }

    /* The interpreter has already opened its windows. This will close them and open new ones, which is clunky but that's the sequence we're working with. */
    if (!glkunix_update_from_library_state(library_state)) {
        glkunix_library_state_free(library_state);
        return false;
    }

    //### recover_extra_state, which will drop and reopen the giblorb_map_t.

    /* We're done with the library state object. */
    glkunix_library_state_free(library_state);
    library_state = NULL;

    /* Let the interpreter recover window IDs. */
    recover_glk_windows();
    
#endif /* GLKUNIX_AUTOSAVE_FEATURES */

    return true;
}
