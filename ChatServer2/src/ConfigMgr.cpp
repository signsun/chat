#include "ConfigMgr.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
// #include <map>
#include <string>

ConfigMgr::ConfigMgr()
{
    std::filesystem::path current_path = std::filesystem::current_path();
    std::filesystem::path config_path = current_path / "config/config.ini";
    std::cout << "Config path: " << config_path << std::endl;

    boost::property_tree::ptree pt;

    // 使用标准库文件流读取
    std::ifstream config_file(config_path);
    if (!config_file.is_open())
    {
        throw std::runtime_error("Cannot open config file: " + config_path.string());
    }

    // 使用 property_tree 的 read_ini 函数读取流
    boost::property_tree::read_ini(config_file, pt);
    config_file.close();

    for (const auto &section_pair : pt)
    {
        const std::string &section_name = section_pair.first;
        const boost::property_tree::ptree &section_tree = section_pair.second;
        std::map<std::string, std::string> section_config;
        for (const auto &key_value_pair : section_tree)
        {
            const std::string &key = key_value_pair.first;
            const std::string &value = key_value_pair.second.get_value<std::string>();
            section_config[key] = value;
        }

        SectionInfo sectionInfo;
        sectionInfo._section_datas = section_config;
        _config_map[section_name] = sectionInfo;
    }

    // 输出所有的section和key-value对
    for (const auto &section_entry : _config_map)
    {
        const std::string &section_name = section_entry.first;
        SectionInfo section_config = section_entry.second;
        std::cout << "[" << section_name << "]" << std::endl;
        for (const auto &key_value_pair : section_config._section_datas)
        {
            std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
        }
    }
}

std::string ConfigMgr::GetValue(const std::string &section, const std::string &key)
{
    if (_config_map.find(section) == _config_map.end())
    {
        return "";
    }

    return _config_map[section].GetValue(key);
}