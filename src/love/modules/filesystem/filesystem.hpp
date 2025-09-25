#pragma once

#include <sol/sol.hpp>
#include <string>

extern "C" {
    #include <lua.h>
}

namespace love {
    namespace filesystem {
        void __init(sol::state &state, int argc, char **argv);
        
        // Internal Use
        void         createDirectories(const std::string& path);
        std::string  getFilePath(const std::string& file);
        bool         fileExists(const std::string& file);

        inline bool  doesPreferSaveDirectory = false;

        // API Accessible
        void                          init(std::string identity);
        sol::protected_function       load(const std::string& file, sol::this_state lua);
        sol::table                    getInfo(const std::string& file, sol::this_state s);
        bool                          exists(const std::string& file, sol::this_state lua);
        sol::protected_function       lines(const std::string& file, sol::this_state lua); // TODO
        int                           getSize(const std::string& file, sol::this_state lua); // IN BYTES!! TODO
        sol::table                    getDirectoryItems(const std::string& dir, sol::this_state lua); // TODO
        std::optional<std::string>    read(const std::string& file, sol::this_state lua); // TODO
        std::pair<bool, std::string>  write(const std::string& file, const std::string& content, sol::this_state lua); // TODO
        std::pair<bool, std::string>  remove(const std::string& file, sol::this_state lua); // TODO
        std::pair<bool, std::string>  mkdir(const std::string& dir, sol::this_state lua); // TODO
        std::pair<bool, std::string>  newFile(const std::string& file, sol::this_state lua); // TODO
        std::pair<bool, std::string>  newFileData(const std::string& file, const std::string& data, sol::this_state lua); // TODO
        
        void                          setSource(const std::string &source); // TODO
        void                          setRequirePath(const std::string &paths); // TODO

        void                          preferSaveDirectory(bool preferSave);

        inline std::string savePath = "sd:/lovewii";
    }
}


int luaopen_love_filesystem(lua_State *L);