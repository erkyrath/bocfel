// Copyright 2010-2021 Chris Spiegel.
//
// SPDX-License-Identifier: MIT

#include <initializer_list>
#include <string>

#include "options.h"
#include "osdep.h"
#include "screen.h"
#include "types.h"
#include "zterp.h"
#include "autosave.h"

extern "C" {
#include <glk.h>
}

#ifdef ZTERP_GLK_BLORB
extern "C" {
#include <gi_blorb.h>
}
#endif

static void load_resources();

#ifdef ZTERP_GLK_UNIX
extern "C" {
#include <glkstart.h>
}

// The “standard” function (provided by remglk as an extension) for
// getting a filename from a fileref is glkunix_fileref_get_filename;
// Gargoyle has always had the function garglk_fileref_get_name for this
// purpose, but has now adopted the remglk name for greater
// compatibility. If building under garglk, and the newer name is not
// available, fall back to the known-to-exist original name. Otherwise,
// use the new name.
#if defined(GARGLK) && !defined(GLKUNIX_FILEREF_GET_FILENAME)
#define glkunix_fileref_get_filename garglk_fileref_get_name
#endif

// Filled in by the Options constructor.
glkunix_argumentlist_t glkunix_arguments[128] = {
    { nullptr, glkunix_arg_End, nullptr }
};

#ifdef ZTERP_GLK_BLORB
static strid_t load_file(const std::string &file)
{
    return glkunix_stream_open_pathname(const_cast<char *>(file.c_str()), 0, 0);
}
#endif

bool glkstart_library_autosave()
{
    //### extra_state?

    auto autosavepath = zterp_os_autosave_name();
    std::string pathname = *autosavepath + ".json";
    
    strid_t jsavefile = glkunix_stream_open_pathname_gen(const_cast<char *>(pathname.c_str()), TRUE, FALSE, 1);
    if (!jsavefile) {
        return false;
    }
    
    glkunix_save_library_state(jsavefile, jsavefile, NULL, NULL);

    glk_stream_close(jsavefile, NULL);
    jsavefile = NULL;

    return true;
}

bool glkstart_library_autorestore()
{
    printf("### autorestore, load_library_state\n");
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

    /* We're done with the library state object. */
    glkunix_library_state_free(library_state);
    library_state = NULL;

    /* Let the interpreter recover window IDs. */
    recover_glk_windows();
    
    return true;
}

int glkunix_startup_code(glkunix_startup_t *data)
{
#ifdef GARGLK
    garglk_set_program_name("Bocfel");
#endif
    options.process_arguments(data->argc, data->argv);

    if (arg_status.any() || options.show_version || options.show_help) {
        return 1;
    }

#ifdef GARGLK
    if (!game_file.empty()) {
        auto story_name = game_file;
        auto slash = story_name.rfind('/');

        if (slash != std::string::npos) {
            story_name.erase(0, slash + 1);
        }

        garglk_set_story_name(story_name.c_str());
    } else {
        frefid_t ref;

        ref = glk_fileref_create_by_prompt(fileusage_Data | fileusage_BinaryMode, filemode_Read, 0);
        if (ref != nullptr) {
            const char *filename = glkunix_fileref_get_filename(ref);
            if (filename != nullptr) {
                game_file = filename;
            }
            glk_fileref_destroy(ref);
        }
    }
#endif

    if (!game_file.empty()) {
#ifndef ZTERP_OS_DOS
        glkunix_set_base_file(&game_file[0]);
#endif
        load_resources();
    }

    return 1;
}
#elif defined(ZTERP_GLK_WINGLK)
#include <cstdlib>

#include <WinGlk.h>

using namespace std::literals;

extern "C" {
int InitGlk(unsigned int);
}

#ifdef ZTERP_GLK_BLORB
static strid_t load_file(const std::string &file)
{
    frefid_t ref = winglk_fileref_create_by_name(fileusage_BinaryMode | fileusage_Data, const_cast<char *>(file.c_str()), 0, 0);

    if (ref == nullptr) {
        return nullptr;
    }

    strid_t stream = glk_stream_open_file(ref, filemode_Read, 0);
    glk_fileref_destroy(ref);
    return stream;
}
#endif

static void startup()
{
    winglk_app_set_name("Bocfel");
    winglk_set_menu_name("&Bocfel");
    winglk_set_about_text("Windows Bocfel " ZTERP_VERSION);
    winglk_show_game_dialog();

    options.process_arguments(__argc, __argv);

    if (arg_status.any() || options.show_version || options.show_help) {
        return;
    }

    if (game_file.empty()) {
        const std::string patterns = "*.z1;*.z2;*.z3;*.z4;*.z5;*.z6;*.z7.*.z8;*.zblorb;*.zlb;*.blorb;*.blb";
        std::string filter;
        const char *filename;

        filter = "Z-Code Files (" + patterns + ")|" + patterns + "|All Files (*.*)|*.*||";

        filename = winglk_get_initial_filename(nullptr, "Choose a Z-Code Game", filter.c_str());
        if (filename != nullptr) {
            game_file = filename;
        }
    }

    if (!game_file.empty()) {
        auto slash = game_file.find_last_of("/\\");
        std::string game_dir, filename;

        if (slash != std::string::npos) {
            game_dir = game_file.substr(0, slash);
            filename = game_file.substr(slash + 1);
        } else {
            game_dir = ".";
            filename = game_file;
        }

        winglk_set_resource_directory(game_dir.c_str());

        if (!filename.empty()) {
            auto dot = filename.rfind('.');
            if (dot != std::string::npos) {
                filename.resize(dot);
            }

            sglk_set_basename(&filename[0]);
        }

        load_resources();
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    if (!InitGlk(0x00000700)) {
        std::exit(EXIT_FAILURE);
    }

    startup();

    glk_main();
    glk_exit();

    return 0;
}
#else
#ifdef ZTERP_GLK_BLORB
#define load_file(file) nullptr
#endif
#error Glk on this platform is not supported.
#endif

// A Blorb file can contain the story file, or it can simply be an
// external package of resources. Try loading the main story file first;
// if it contains Blorb resources, use it. Otherwise, try to find a
// Blorb file that goes with the selected story.
static void load_resources()
{
#ifdef ZTERP_GLK_BLORB
    auto set_map = [](const std::string &blorb_file) {
        strid_t file = load_file(blorb_file);
        if (file != nullptr) {
            if (giblorb_set_resource_map(file) == giblorb_err_None) {
                screen_load_scale_info(blorb_file);

                return true;
            }
            glk_stream_close(file, nullptr);
        }

        return false;
    };

    if (set_map(game_file)) {
        return;
    }

    for (const auto &ext : {".blb", ".blorb"}) {
        std::string blorb_file = game_file;
        auto dot = blorb_file.rfind('.');
        if (dot != std::string::npos) {
            blorb_file.replace(dot, std::string::npos, ext);
        } else {
            blorb_file += ext;
        }

        if (set_map(blorb_file)) {
            return;
        }
    }
#endif
}
