#include "FunctionalProgramming.h"

bool nonMemberFunction(int arg)
{
    return true;
}

std::string SomeClass::memberFunction(double arg)
{
    return "TRUE";
}

bool MapLoader::loadResources(const ResourceManager& mgr, const std::string& path)
{
    return true;
}