#include "ini_parser.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

namespace qh
{
    INIParser::INIParser() : comment_symbol("#"), section_start_symbol("["), section_end_symbol("]"), default_retVal("") {}
    INIParser::~INIParser() {}

    bool INIParser::Parse(const std::string& ini_file_path) {
        if (ini_file_path.empty()) {
            std::cout << "Error: [" + ini_file_path + "] file path is empty" << std::endl;
            return false;
        }
        
        std::ifstream fin(ini_file_path.c_str());
        if (!fin.is_open()) {
            std::cout << "Error: [" + ini_file_path + "] open file failure" << std::endl;
            return false;
        }

        std::stringstream ss;
        std::string buf;
        while (!fin.eof()) {
            std::getline(fin, buf); 
            ss << buf << "\n";
        }
        fin.close();
        
        buf = ss.str();
        return Parse(buf.c_str(), buf.size());    
    }

    bool INIParser::Parse(const char* ini_data, size_t ini_data_len, const std::string& line_seperator, const std::string& key_value_seperator) {
        std::string str_data(ini_data);
        return ParseLines(str_data, line_seperator, key_value_seperator);
    }

    const std::string& INIParser::Get(const std::string& key, bool* found) {
        if (global_key_value_map.find(key) != global_key_value_map.end()) {

            if (found)
                *found = true;
            return global_key_value_map[key];
        }
        if (found) 
            *found = false;
        return default_retVal;
    }

    const std::string& INIParser::Get(const std::string& section, const std::string& key, bool* found) {
        if (section_key_value_map.find(section) != section_key_value_map.end()) {
            if (section_key_value_map[section].find(key) != section_key_value_map[section].end()) {
                if (found) 
                    *found = true;
                return section_key_value_map[section][key]; 
            }
        } 
        
        if (found) 
            *found = false;
        return default_retVal;
    }

    bool INIParser::ParseLines(std::string& lines, const std::string& line_seperator, const std::string& key_value_seperator) {
        if (lines.size() == 0)
            return true;
        size_t pos = lines.find(line_seperator);
        if (pos == std::string::npos) 
            return ParseProperty(lines, key_value_seperator);

        std::string left = lines.substr(0, pos);
        std::string right = lines.substr(pos + line_seperator.size());

        bool succ = ParseProperty(left, key_value_seperator);
        if (!succ)
            return false;

        return ParseLines(right, line_seperator, key_value_seperator);
    }

    void INIParser::TrimLine(std::string& line, const std::string& symbol, bool all) const {
        if (line.empty())
           return;
        // Delete from head
        while (line.find(symbol) == 0) {
            line.erase(0, symbol.size());
            if (!all)
                break;
        } 

        // Delete from tail
        while (!line.empty() && (line.rfind(symbol) == (line.size() - symbol.size()))) {
            line.erase(line.size() - symbol.size(), symbol.size());
            if (!all)
                break;
        }
    }

    void INIParser::TrimLineComment(std::string& line, const std::string& symbol) const {
        if (line.empty()) 
            return;
        if (line[0] == '#')
            return line.clear();
        size_t pos;
        if ((pos = line.find(symbol)) != std::string::npos)
            line.erase(pos, line.size());
    }

    bool INIParser::ParseProperty(std::string& property, const std::string& key_value_seperator) {
        if (property.empty())
            return true;
        
        TrimLine(property, std::string(" "), true);
        if (property.empty())
            return true;

        TrimLineComment(property, comment_symbol);
        if (property.empty())
            return true;

        if (property[0] == section_start_symbol[0]) {
            std::string section_name = GetSectionName(property, section_start_symbol, section_end_symbol);
            TrimLine(section_name, std::string(" "), true);
            if (section_name.empty()) {
                // Invalid section format
                std::cout << "Error: [" + property + "] invalid section format" << std::endl;
                return false;
            }

            // Set current section
            curr_active_section = section_name;
        } else {
            std::string::size_type pos = property.find(key_value_seperator);
            if (pos == 0) {
                // No key, just return
                std::cout << "Error: [" + property  + "] has no key" << std::endl;
                return false;
            }

            if (pos == std::string::npos) {
                // invalid format
                std::cout << "Error: [" + property + "] invalid format" << std::endl;
                return false;
            }

            std::string left = property.substr(0, pos);
            TrimLine(left, std::string(" "), true);
            std::string right = property.substr(pos + key_value_seperator.size());
            TrimLine(right, std::string(" "), true);

            if (!curr_active_section.empty()) 
                section_key_value_map[curr_active_section][left] = right;
            else
                global_key_value_map[left] = right;
        }
        return true;
    }
    
    std::string INIParser::GetSectionName(std::string& section_line, const std::string& begin_symbol, const std::string& end_symbol) {
        TrimLine(section_line, std::string(" "), true);
        if (section_line.empty())
            return "";

        if (section_line.find(begin_symbol) != 0) {
            std::cout << "Error: [" + section_line+ "] must begin with '['" << std::endl;
            return "";
        }

        if (section_line.rfind(end_symbol) != section_line.size() - end_symbol.size()) {
            std::cout << "Error: [" + section_line+ "] must end with ']'" << std::endl;
            return "";
        }

        return section_line.substr(1, section_line.size() - 2);
    }
}
