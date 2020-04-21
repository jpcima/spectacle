#pragma once
#include <SimpleIni.h>
#include <string>
#include <memory>

const std::string &get_configuration_dir();
std::string get_configuration_file(const std::string &name);

std::unique_ptr<CSimpleIniA> create_configuration();
std::unique_ptr<CSimpleIniA> load_configuration(const std::string &name);
bool save_configuration(const std::string &name, const CSimpleIniA &ini);
