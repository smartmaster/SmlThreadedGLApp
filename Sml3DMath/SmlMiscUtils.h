#pragma once

#ifndef SML_MISC_UTILS_H
#define SML_MISC_UTILS_H


#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

namespace SmartLib
{
template<typename T = double>
class MiscUtils
{
public:
    static T MixRatio(T p0, T p1, T p)
    {
        return (p - p0) / (p1 - p0);
    }

    static T Mix(T q0, T q1, T p0, T p1, T p)
    {
        return glm::mix(q0, q1, MixRatio(p0, p1, p));
    }
};
}


#endif // SMLMATHUTILS_H
