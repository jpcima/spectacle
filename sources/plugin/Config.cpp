#include "Config.h"
#include "DistrhoPluginInfo.h"
#include <sys/stat.h>
#include <sys/types.h>
#if defined(_WIN32)
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif
#include <cstdlib>

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
    return std::string();
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
