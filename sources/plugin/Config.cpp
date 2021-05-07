#include "Config.h"
#include "DistrhoPluginInfo.h"
#include <sys/stat.h>
#include <sys/types.h>
#if defined(_WIN32)
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif
#include <dirent.h>
#include <cstdlib>
#include <cstring>

#if !defined(_WIN32)
static std::string get_home_directory()
{
    std::string home;

    const char *value = std::getenv("HOME");
    if (value && value[0] != '\0') {
        home.assign(value);
        if (home.back() != '/')
            home.push_back('/');
    }

    return home;
}
#endif

const std::string &get_configuration_dir()
{
#if defined(_WIN32)
    #pragma message("TODO: not implemented")
    static const std::string empty;
    return empty;
#else
    static const std::string path = []() -> std::string {
        std::string path;
#if defined(__HAIKU__)
        path = get_home_directory();
        if (path.empty())
            return std::string();
        path.append("config/");
        mkdir(path.c_str(), 0755);
        path.append("settings/");
        mkdir(path.c_str(), 0755);
        path.append(DISTRHO_PLUGIN_NAME "/");
        mkdir(path.c_str(), 0755);
#else
        path = get_home_directory();
        if (path.empty())
            return std::string();
        path.append(".config/");
        mkdir(path.c_str(), 0755);
        path.append(DISTRHO_PLUGIN_NAME "/");
        mkdir(path.c_str(), 0755);
#endif
        path.shrink_to_fit();
        return path;
    }();
    return path;
#endif
}

std::string get_configuration_file(const std::string &name)
{
    std::string path = get_configuration_dir();
    if (path.empty())
        return std::string{};
    path.append(name.data(), name.size());
    path.append(".ini");
    return path;
}

std::unique_ptr<CSimpleIniA> create_configuration()
{
    bool iniIsUtf8 = true, iniMultiKey = true, iniMultiLine = true;
    return std::unique_ptr<CSimpleIniA>(
        new CSimpleIniA(iniIsUtf8, iniMultiKey, iniMultiLine));
}

std::unique_ptr<CSimpleIniA> load_configuration(const std::string &name)
{
    std::unique_ptr<CSimpleIniA> ini = create_configuration();
    if (ini->LoadFile(get_configuration_file(name).c_str()) != SI_OK)
        return nullptr;

    return ini;
}

bool save_configuration(const std::string &name, const CSimpleIniA &ini)
{
    return ini.SaveFile(get_configuration_file(name).c_str()) == SI_OK;
}

///

const std::string &get_theme_dir()
{
    static const std::string path = []() -> std::string {
        std::string path = get_configuration_dir();
        if (path.empty())
            return std::string();
        path.append("themes/");
        mkdir(path.c_str(), 0755);
        return path;
    }();
    return path;
}

std::string get_theme_file(const std::string &name)
{
    std::string path = get_theme_dir();
    if (path.empty())
        return std::string{};
    path.append(name.data(), name.size());
    path.append(".ini");
    return path;
}

std::unique_ptr<CSimpleIniA> create_theme()
{
    return create_configuration();
}

std::unique_ptr<CSimpleIniA> load_theme(const std::string &name)
{
    std::unique_ptr<CSimpleIniA> ini = create_theme();
    if (ini->LoadFile(get_theme_file(name).c_str()) != SI_OK)
        return nullptr;

    return ini;
}

bool save_theme(const std::string &name, const CSimpleIniA &ini)
{
    return ini.SaveFile(get_theme_file(name).c_str()) == SI_OK;
}

std::vector<std::string> list_themes()
{
    std::vector<std::string> list;

    const std::string &path = get_theme_dir();
    std::unique_ptr<DIR, int(*)(DIR *)> dir {opendir(path.c_str()), &closedir};

    if (!dir)
        return list;

    while (dirent *ent = readdir(dir.get())) {
        char *name = ent->d_name;
        size_t namelen = std::strlen(name);
        if (namelen > 4 && !std::strcmp(name + namelen - 4, ".ini"))
            list.emplace_back(name, name + namelen - 4);
    }

    return list;
}
