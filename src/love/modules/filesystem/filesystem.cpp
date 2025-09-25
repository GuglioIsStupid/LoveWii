#include <sol/sol.hpp>
#include <string>
#include <physfs/physfs.h>
#include <fstream>
#include <chrono>
#include <ctime>

#include "filesystem.hpp"
extern "C" {
    #include <lua.h>
}

namespace love {
    namespace filesystem {
        void __init(sol::state &luastate, int argc, char **argv) {
            PHYSFS_init(argv[0]);
            savePath = "sd:/lovewii";
            PHYSFS_mount(savePath.c_str(), nullptr, 1);
            PHYSFS_mount((savePath + "/game").c_str(), nullptr, 1);
            PHYSFS_mount((savePath + "/save").c_str(), nullptr, 1);
        }

        void createDirectories(const std::string& path) {
            /* std::filesystem::path dir(path);
            if (!std::filesystem::exists(dir)) {
                std::filesystem::create_directories(dir);
            } */
        }

        std::string getFilePath(const std::string& file) {
            std::string gamePath = "game/" + file;
            std::string savePathLocal = "save/" + file;

            if (doesPreferSaveDirectory) {
                if (PHYSFS_exists(savePathLocal.c_str()))
                    return savePathLocal;
                return gamePath;
            } else {
                if (PHYSFS_exists(gamePath.c_str()))
                    return gamePath;
                return savePathLocal;
            }
        }

        bool fileExists(const std::string& file) {
            std::string path = getFilePath(file);
            return PHYSFS_exists(path.c_str());
        }

        void init(std::string identity) { // Lua accesible
            if (identity.empty())
                identity = "sd:/lovewii";

            savePath = identity;

            PHYSFS_deinit();
            PHYSFS_init(nullptr);

            PHYSFS_mount(savePath.c_str(), nullptr, 1);
            PHYSFS_mount((savePath + "/game").c_str(), nullptr, 1);
            PHYSFS_mount((savePath + "/save").c_str(), nullptr, 1);
        }


        sol::protected_function load(const std::string& file, sol::this_state state) {
            sol::state_view luastate(state);
            std::string path = getFilePath(file);

            PHYSFS_File* f = PHYSFS_openRead(path.c_str());
            if (!f)
                throw std::runtime_error("Could not open file: " + path);

            PHYSFS_sint64 length = PHYSFS_fileLength(f);
            std::string buffer(length, '\0');
            PHYSFS_readBytes(f, buffer.data(), length);
            PHYSFS_close(f);

            return luastate.load(buffer, path);
        }

        sol::table getInfo(const std::string& file, sol::this_state s) {
            sol::state_view lua(s);
            sol::table info = lua.create_table();

            std::string path = getFilePath(file);
            if (!PHYSFS_exists(path.c_str()))
                return info; // return empty

            PHYSFS_Stat statbuf;
            if (PHYSFS_stat(path.c_str(), &statbuf)) {
                info["filetype"] = (statbuf.filetype == PHYSFS_FILETYPE_DIRECTORY) ? "directory" : "file";
            }

            PHYSFS_File* f = PHYSFS_openRead(path.c_str());
            if (f) {
                PHYSFS_sint64 size = PHYSFS_fileLength(f);
                info["size"] = static_cast<double>(size);
                PHYSFS_close(f);
            }
            
            info["modtime"] = 0.0;

            return info;
        }

        bool exists(const std::string& file, sol::this_state lua) {
            return PHYSFS_exists(getFilePath(file).c_str());
        }

        void preferSaveDirectory(const bool preferSave) {
            doesPreferSaveDirectory = preferSave;
        }
    }
}

int luaopen_love_filesystem(lua_State *L)  {
    sol::state_view luastate(L);

    luastate["love"]["filesystem"] = luastate.create_table_with(
        "init", love::filesystem::init,
        "load", love::filesystem::load,
        "getInfo", love::filesystem::getInfo,
        "exists", love::filesystem::exists,
        "preferSaveDirectory", love::filesystem::preferSaveDirectory
    );

    return 1;
}