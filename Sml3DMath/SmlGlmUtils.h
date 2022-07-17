#pragma once

#ifndef SML_GLM_UTILS_H
#define SML_GLM_UTILS_H

#include "SmlAxisCoord.h"


namespace SmartLib
{
template<typename T = double>
class GlmUtils
{
public:
    //equivilent to glm::lookAt
    static glm::tmat4x4<T> LookAt(const glm::tvec3<T> &eye, const glm::tvec3<T> &center, const glm::tvec3<T> &up)
    {
        //////////////////////////////////////
        auto zAxis = eye - center; //posive z direction pointing into eye
        auto xAxis = glm::cross(up, zAxis);
        auto yAxis = glm::cross(zAxis, xAxis);

        AxisCoord<T> axisSys;
        axisSys.MakeFromOHVZ(eye, xAxis, yAxis, zAxis, true);

        return axisSys.WorldToModelMat();
    }

    //equivilent to glm::ortho
    static glm::tmat4x4<T> Ortho(
            T const left, T const right,
            T const bottom, T const top,
            T const zNear, T const zFar
            )
    {
#if 0
        //////////////////////////////////////
        AxisCoord<T> axisSys;

        axisSys.Scale(glm::tvec3<T>{T{ 1 }, T{ 1 }, T{ -1 }})
                .Translate(glm::tvec3<T>{(left + right) / T{ 2 }, (bottom + top) / T{ 2 }, (zNear + zFar) / T{ 2 }})
                .Scale(glm::tvec3<T>{(right - left) / T{ 2 }, (top - bottom) / T{ 2 }, (zFar - zNear) / T{ 2 }});

        auto mat = axisSys.WorldToModelMat();

        return mat;

#else
        AxisCoord<T> axisSys;
        axisSys
                .Translate(glm::tvec3<T>{(left + right) / T{ 2 }, (bottom + top) / T{ 2 }, -(zNear + zFar) / T{ 2 }})
                .Scale(glm::tvec3<T>{(right - left) / T{ 2 }, (top - bottom) / T{ 2 }, (zNear - zFar) / T{ 2 }});

        auto mat = axisSys.WorldToModelMat();
        return mat;
#endif
    }




    static glm::tmat4x4<T> Frustum(
            T const left, T const right,
            T const bottom, T const top,
            T const zNear, T const zFar
            )
    {

#if 0
        //////////////////////////////////////

        //frustum to cubic
        T A = -zNear - zFar;
        T B = -zNear * zFar;
        glm::tmat4x4<T> matFrustum2Cubic
        {
            -zNear, 0, 0, 0,
                    0, -zNear, 0, 0,
                    0, 0, A, 1,
                    0, 0, B, 0,
        };

        //cubic to NDC
        glm::tmat4x4<T> matOrtho = Ortho(
                    left, right,
                    bottom, top,
                    zNear, zFar);

        glm::tmat4x4<T> rv = matOrtho * matFrustum2Cubic;

        return -rv;
#else
        //////////////////////////////////////

        //frustum to cubic
        T A = zNear + zFar;
        T B = - zNear * zFar;
        glm::tmat4x4<T> matFrustum2Cubic
        {
            zNear, 0, 0, 0,
                    0, zNear, 0, 0,
                    0, 0, A, -1,
                    0, 0, -B, 0,
        };

        //cubic to NDC
        glm::tmat4x4<T> matOrtho = Ortho(
                    left, right,
                    bottom, top,
                    zNear, zFar);

        glm::tmat4x4<T> rv = matOrtho * matFrustum2Cubic;
        return rv;
#endif

    }


    static glm::tmat4x4<T> Perspective(T fovy, T aspect, T zNear, T zFar)
    {
        T height = zNear * glm::tan(fovy/T{2});
        T width = height * aspect;
        return Frustum(-width, width, -height, height, zNear, zFar);
    }
};
}

#endif // SMLGLMUTILS_H
