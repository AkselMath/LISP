#pragma once

#include <sstream>
#include <string>
#include "parser.h"

class Scheme {
public:
    Scheme();

    std::string Evaluate(const std::string& expression);

private:
    std::shared_ptr<Scope> scope_;
};
