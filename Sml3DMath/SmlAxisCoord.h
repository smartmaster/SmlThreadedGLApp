#pragma once

#ifndef SML_AXIS_COORD_UTILS_H
#define SML_AXIS_COORD_UTILS_H


#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/epsilon.hpp>

#include "SmlMatVecUtils.h"

namespace SmartLib
{
template<typename T = double>
class AxisCoord
{
private:
    glm::tmat4x4<T> _axis{MatVecUtils<T>::MatE};             // the vector of x y z axis
    glm::tvec3<T> _unitLen{MatVecUtils<T>::Vec1s};  // unit length of the x y z axis
    glm::tvec3<T> _origin{MatVecUtils<T>::Vec0s}; //in world (absolute) system
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
        _axis = MatVecUtils<T>::MatE;
        _unitLen = MatVecUtils<T>::Vec1s;
        _origin = MatVecUtils<T>::Vec0s;
        return *this;
    }



    //move along the current axis directions and with the current unit length
    AxisCoord& Translate(const glm::tvec3<T>& offset)
    {
        _origin += MatVecUtils<T>::M4xP3(_axis, offset * _unitLen);
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
        auto rotAxisAbsolutely = MatVecUtils<T>::M4xV3(_axis, rotAxis * _unitLen);
        _axis = glm::rotate<T>(MatVecUtils<T>::MatE, radians, rotAxisAbsolutely) * _axis;
        return *this;
    }


    //rotate in absolute (world) coordinates system
    AxisCoord& RotateAbsolutely(T radians, const glm::tvec3<T>& rotAxisAbsolutely)
    {
        _axis = glm::rotate<T>(MatVecUtils<T>::MatE, radians, rotAxisAbsolutely) * _axis;
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
        return _origin + MatVecUtils<T>::M4xP3(_axis, model * _unitLen) ;
    }

    const glm::tvec3<T> WorldToModel(const glm::tvec3<T>& world) const
    {
        return MatVecUtils<T>::M4xP3(InverseAxis(), world - _origin)/_unitLen;
    }

    const glm::tmat4x4<T> ModelToWorldMat() const
    {
        auto matScale = glm::scale<T>(MatVecUtils<T>::MatE, _unitLen);

        auto matTrans = glm::translate<T>(MatVecUtils<T>::MatE, _origin);

        return matTrans * _axis * matScale;

    }

    const glm::tmat4x4<T> WorldToModelMat() const
    {
        auto matTrans = glm::translate(MatVecUtils<T>::MatE, -_origin);
        auto matRot = InverseAxis(); // is equal to glm::inverse<T>(_axis)
        auto matScale = glm::scale(MatVecUtils<T>::MatE, MatVecUtils<T>::Vec1s/_unitLen);
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

};
}


#endif //#ifndef SML_AXIS_COORD_UTILS_H
