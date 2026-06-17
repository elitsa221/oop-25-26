#pragma once
#include <string>
#include <vector>
#include <iostream>
class Command
{
public:
    virtual ~Command() = default;
    virtual void execute(const std::vector<std::string>& args) = 0;
    virtual std::string name() const = 0;
    virtual std::string helpText() const = 0;

protected:
    void ok(const std::string& msg) { std::cout << "[OK] " << msg << "\n"; }
    void err(const std::string& msg) { std::cout << "[ERROR] " << msg << "\n"; }
    void out(const std::string& msg) { std::cout << msg << "\n"; }
};

