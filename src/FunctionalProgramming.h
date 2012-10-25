#include "Platform.h"

#include <functional>
#include <string>

EXAMPLES_LIB_API bool nonMemberFunction(int arg);

class EXAMPLES_LIB_API ResourceManager {
public:
    
};

class EXAMPLES_LIB_API SomeClass {
public:
    std::string memberFunction(double arg);
};

class EXAMPLES_LIB_API MapLoader {
public:
    bool loadResources(const ResourceManager& mgr, const std::string& path);
};