#pragma once
#include <SimpleIni.h>
#include <vector>
#include <string>
#include <memory>

const std::string &get_configuration_dir();
std::string get_configuration_file(const std::string &name);

std::unique_ptr<CSimpleIniA> create_configuration();
std::unique_ptr<CSimpleIniA> load_configuration(const std::string &name);
bool save_configuration(const std::string &name, const CSimpleIniA &ini);

///

const std::string &get_theme_dir();
std::string get_theme_file(const std::string &name);

std::unique_ptr<CSimpleIniA> create_theme();
std::unique_ptr<CSimpleIniA> load_theme(const std::string &name);
bool save_theme(const std::string &name, const CSimpleIniA &ini);
std::vector<std::string> list_themes();
