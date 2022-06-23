#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/epsilon.hpp>

namespace SmartLib
{
template<typename T = double>
class AxisCoord
{
private:
    /*inline*/ static constexpr glm::tmat4x4<T> MatE{T{1}};
    /*inline*/ static constexpr glm::tvec3<T> Vec1s{T{1}, T{1}, T{1}};
    /*inline*/ static constexpr glm::tvec3<T> Vec0s{T{0}, T{0}, T{0}};
    /*inline*/ static constexpr glm::tvec3<T> VecX{T{1}, T{0}, T{0}};
    /*inline*/ static constexpr glm::tvec3<T> VecY{T{0}, T{1}, T{0}};
    /*inline*/ static constexpr glm::tvec3<T> VecZ{T{0}, T{0}, T{1}};

private:
    glm::tmat4x4<T> _axis{MatE};             // the vector of x y z axis
    glm::tvec3<T> _unitLen{Vec1s};  // unit length of the x y z axis
    glm::tvec3<T> _origin{Vec0s}; //in world (absolute) system
    bool _isBaseAxis{true};


private:
    glm::tmat4x4<T> InverseAxis() const
    {
        return _isBaseAxis? glm::transpose(_axis) : glm::inverse(_axis);
    }

public:

    AxisCoord()
    {
    }

    AxisCoord(const AxisCoord& ac) :
        _axis{ac._axis},
        _unitLen{ac._unitLen},
        _origin{ac._origin}
    {
    }

    AxisCoord(AxisCoord&& ac) :
        _axis{std::move(ac._axis)},
        _unitLen{std::move(ac._unitLen)},
        _origin{std::move(ac._origin)}
    {
    }

    const AxisCoord& operator=(const AxisCoord& ac)
    {
        _axis = ac._axis;
        _unitLen = ac._unitLen;
        _origin = ac._origin;
        return *this;
    }

    const AxisCoord& operator=(AxisCoord&& ac)
    {
        _axis = std::move(ac._axis);
        _unitLen = std::move(ac._unitLen);
        _origin = std::move(ac._origin);
        return *this;
    }

    AxisCoord& Reset()
    {
        _axis = MatE;
        _unitLen = Vec1s;
        _origin = Vec0s;
        return *this;
    }



    //move along the current axis directions and with the current unit length
    AxisCoord& Translate(const glm::tvec3<T>& offset)
    {
        _origin += M4xP3(_axis, offset * _unitLen);
        return *this;
    }

    //move along the absolute (world) axis directions
    AxisCoord& TranslateAbsolutely(const glm::tvec3<T>& offsetAbsolutely)
    {
        _origin += offsetAbsolutely;
        return *this;
    }

    //rotate in current coordinates system
    AxisCoord& Rotate(T radians, const glm::tvec3<T>& rotAxis)
    {
        auto rotAxisAbsolutely = M4xV3(_axis, rotAxis);
        _axis = glm::rotate<T>(MatE, radians, rotAxisAbsolutely) * _axis;
        return *this;
    }


    //rotate in absolute (world) coordinates system
    AxisCoord& RotateAbsolutely(T radians, const glm::tvec3<T>& rotAxisAbsolutely)
    {
        _axis = glm::rotate<T>(MatE, radians, rotAxisAbsolutely) * _axis;
        return *this;
    }


    //scale in current coordinates system
    AxisCoord& Scale(const glm::tvec3<T>& scalar)
    {
        _unitLen *= scalar;
        return *this;
    }


    AxisCoord& ScaleAbsolutely(const glm::tvec3<T>& scalar)
    {
        _unitLen = scalar;
        return *this;
    }


    AxisCoord& SetOrigin(const glm::tvec3<T>& origin)
    {
        _origin = origin;
        return *this;
    }

    //axis should be unit and orthogonal matrix
    AxisCoord& SetAxis(const glm::tmat4x4<T>& axis)
    {
        _axis = axis;
        return *this;
    }


    const glm::tvec3<T>& GetScale() const
    {
        return _unitLen;
    }


    const glm::tvec3<T>& GetOrigin() const
    {
        return _origin;
    }

    const glm::tmat4x4<T>& GetAxis() const
    {
        return _axis;
    }


    const glm::tvec3<T> ModelToWorld(const glm::tvec3<T>& model) const
    {
        return _origin + M4xP3(_axis, model * _unitLen) ;
    }

    const glm::tvec3<T> WorldToModel(const glm::tvec3<T>& world) const
    {
        return M4xP3(InverseAxis(), world - _origin)/_unitLen;
    }

    const glm::tmat4x4<T> ModelToWorldMat() const
    {
        auto matScale = glm::scale<T>(MatE, _unitLen);

        auto matTrans = glm::translate<T>(MatE, _origin);

        return matTrans * _axis * matScale;

    }

    const glm::tmat4x4<T> WorldToModelMat() const
    {
        auto matTrans = glm::translate(MatE, -_origin);
        auto matRot = InverseAxis(); // is equal to glm::inverse<T>(_axis)
        auto matScale = glm::scale(MatE, Vec1s/_unitLen);
        return matScale * matRot * matTrans;
    }

    void MakeFromOHV(const glm::tvec3<T>& originPos, const glm::tvec3<T>& horizontalV, const glm::tvec3<T>& verticalV)
    {
        _origin = originPos;
        auto zV = glm::cross<T>(horizontalV, verticalV);
        auto yV = glm::cross<T>(zV, horizontalV);

        _axis[0] = glm::tvec4<T>{glm::normalize(horizontalV), T{0}};
        _axis[1] = glm::tvec4<T>{glm::normalize(yV), T{0}};
        _axis[2] = glm::tvec4<T>{glm::normalize(zV), T{0}};
    }


    void MakeFromOHVPos(const glm::tvec3<T>& originPos, const glm::tvec3<T>& horizontalPos, const glm::tvec3<T>& verticalPos)
    {
        MakeFromOHV(originPos, horizontalPos - originPos, verticalPos - originPos);
    }


    //note: horizontalV verticalV and zV should be orthogonal to each other
    //if not above, call SetIsBaseAxis(false)
    void MakeFromOHVZ(
            const glm::tvec3<T>& originPos,
            const glm::tvec3<T>& horizontalV,
            const glm::tvec3<T>& verticalV,
            const glm::tvec3<T>& zV,
            bool toNormalize)
    {
        _origin = originPos;
        _axis[0] = glm::tvec4<T>{toNormalize ? glm::normalize(horizontalV) : horizontalV, T{0}};
        _axis[1] = glm::tvec4<T>{toNormalize ? glm::normalize(verticalV) : verticalV, T{0}};
        _axis[2] = glm::tvec4<T>{toNormalize ? glm::normalize(zV) : zV, T{0}};
    }

    void SetIsBaseAxis(bool isBaseAxis)
    {
        _isBaseAxis = isBaseAxis;
    }


    void MakeFromCamera(const glm::tvec3<T> &eye,
                        const glm::tvec3<T> &center,
                        const glm::tvec3<T> &up)
    {
        //////////////////////////////////////
        auto zAxis = eye - center; //posive z direction pointing into eye
        auto xAxis = glm::cross(up, zAxis);
        auto yAxis = glm::cross(zAxis, xAxis);
        MakeFromOHVZ(eye, xAxis, yAxis, zAxis, true);
    }


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
