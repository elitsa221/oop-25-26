#pragma once
#include <string>

class FileSerializer {
public:
    static void save(const std::string& path);
    static void load(const std::string& path);

private:
    static constexpr char SEP = '|';
    static constexpr char LIST_SEP = ';';
};

