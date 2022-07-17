#pragma once

#ifndef SML_MAT_VEC_UTILS_H
#define SML_MAT_VEC_UTILS_H


#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

namespace SmartLib
{
template<typename T = double>
class MatVecUtils
{
public:
    /*inline*/ static constexpr glm::tmat4x4<T> MatE{T{1}};
    /*inline*/ static constexpr glm::tvec3<T> Vec1s{T{1}, T{1}, T{1}};
    /*inline*/ static constexpr glm::tvec3<T> Vec0s{T{0}, T{0}, T{0}};
    /*inline*/ static constexpr glm::tvec3<T> VecX{T{1}, T{0}, T{0}};
    /*inline*/ static constexpr glm::tvec3<T> VecY{T{0}, T{1}, T{0}};
    /*inline*/ static constexpr glm::tvec3<T> VecZ{T{0}, T{0}, T{1}};

public:
    static glm::tvec3<T> V4ToV3(const glm::tvec4<T>& v4)
    {
        if(glm::epsilonNotEqual(v4[3], T{0}, glm::epsilon<T>()*T{1000}))
        {
            T inverse = T{1}/v4[3];
            return glm::tvec3<T>{v4 * inverse};
        }
        else
        {
            return glm::tvec3<T>{v4};
        }
    }

    static glm::tvec4<T> V3ToP4(const glm::tvec3<T>& v3)
    {
        return glm::tvec4<T>{v3, T{1}}; //positional point
    }

    static glm::tvec4<T> V3ToV4(const glm::tvec3<T>& v3)
    {
        return glm::tvec4<T>{v3, T{0}}; //directional vector
    }

    static glm::tvec3<T> M4xP3(const glm::tmat4x4<T>& m4, const glm::tvec3<T>& v3)
    {
        return glm::tvec3<T>{m4 * glm::tvec4<T>{v3, T{1}}} ; //positional point
    }

    static glm::tvec3<T> M4xV3(const glm::tmat4x4<T>& m4, const glm::tvec3<T>& v3)
    {
        return glm::tvec3<T>{m4 * glm::tvec4<T>{v3, T{0}}} ; //directional vector
    }

    static glm::tvec3<T> M4xV4(const glm::tmat4x4<T>& m4, const glm::tvec4<T>& v4)
    {
        auto vec = m4 * v4;
        if(glm::epsilonNotEqual(vec[3], T{0}, glm::epsilon<T>()*T{1000}))
        {
            T inverse = T{1}/vec[3];
            vec *= inverse;
        }
        return glm::tvec3<T>(vec);
    }

public:
    static glm::tmat2x3<T> CalcTangentBitangent(
            const glm::tvec3<T>& p0,
            const glm::tvec3<T>& p1,
            const glm::tvec3<T>& p2,
            const glm::tvec2<T>& q0,
            const glm::tvec2<T>& q1,
            const glm::tvec2<T>& q2 )
    {
        glm::tmat2x3<T> matp{
            p1 - p0,
                    p2 - p0,
        };

        glm::tmat2x2<T> matq
        {
            q1 - q0,
                    q2 - q0,
        };

        glm::tmat2x3<T> result = matp * glm::inverse(matq);

        return result;
    }

    static glm::tmat2x3<T> CalcTangentBitangentByHand(
            const glm::tvec3<T>& p0,
            const glm::tvec3<T>& p1,
            const glm::tvec3<T>& p2,
            const glm::tvec2<T>& q0,
            const glm::tvec2<T>& q1,
            const glm::tvec2<T>& q2 )
    {
        //from https://learnopengl.com/Advanced-Lighting/Normal-Mapping
        glm::tvec3<T> edge1 = p1 - p0;
        glm::tvec3<T> edge2 = p2 - p0;
        glm::tvec2<T> deltaUV1 = q1 - q0;
        glm::tvec2<T> deltaUV2 = q2 - q0;


        T f = T{1} / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::tmat2x3<T> result;
        result[0].x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        result[0].y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        result[0].z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        result[1].x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        result[1].y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        result[1].z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        return result;
    }


    static glm::tmat3x3<T> CalcTangentBitangentNormal(
            const glm::tvec3<T>& normalp0, //vertex p0 normal
            const glm::tvec3<T>& p0, //vertex p0
            const glm::tvec3<T>& p1, //vertex p1
            const glm::tvec3<T>& p2, //vertex p2
            const glm::tvec3<T>& normalq0, //texture q0 normal, (0,0,1) or normal in normal map
            const glm::tvec3<T>& q0, //texture q0
            const glm::tvec3<T>& q1, //texture q1
            const glm::tvec3<T>& q2 )//texture q2
    {
        glm::tmat3x3<T> matp{
            p1 - p0,
                    p2 - p0,
                    normalp0
        };

        glm::tmat3x3<T> matq
        {
            q1 - q0,
                    q2 - q0,
                    normalq0
        };

        glm::tmat3x3<T> result = matp * glm::inverse(matq);

        return result;
    }


public:
    static glm::tmat3x3<T> RotateMat(T radians, const glm::tvec3<T>& rotationAxis)
    {
        T cc = glm::cos(radians);
        T ss = glm::sin(radians);
        glm::tmat3x3<T> matRX
        {
            //glm::tvec3<T>{1, 0, 0},     //colunm vector x axis
            VecX,
                    glm::tvec3<T>{0, cc, ss},   //colunm vector y axis
            glm::tvec3<T>{0, -ss, cc}  //colunm vector z axis
        };

        //static const glm::tvec3<T> vecx{1, 0, 0};

        auto rz = glm::cross(VecX, rotationAxis);
        if(glm::epsilonEqual(glm::length(rz), T{0}, glm::epsilon<T>()*1000))
        {
            static const  glm::tmat3x3<T> matE{1};
            return matE;
        }
        auto ry = glm::cross(rz, rotationAxis);

        glm::tmat3x3<T> matR
        {
            glm::normalize(rotationAxis),
                    glm::normalize(ry),
                    glm::normalize(rz),
        };

        glm::tmat3x3<T> matInv = glm::transpose(matR);


        return matR * matRX * matInv;
    }
};
}


#endif // SMLMATHUTILS_H
