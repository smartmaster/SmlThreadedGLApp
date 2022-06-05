#pragma once
#include "SmlAxisCoord.h"
#include <vector>
#include <random>
#include <string>
#include <iostream>
#include <cassert>
#include <QDebug>
#include <glm/gtx/string_cast.hpp>


namespace SmartLib
{

using namespace ::std;

class AxisCoordTest
{
private:
    template<typename ITER>
    static void FillRandom(ITER begin, ITER end, remove_cvref_t<decltype(*begin)> scalar, long long minval, long long maxval, long long invalidVal)
    {
        random_device randDevice;
        mt19937_64 randEngine(randDevice());
        uniform_int_distribution<long long> randDistrib(minval, maxval);

        while (begin != end)
        {
            long long randVal = randDistrib(randEngine);
            if (randVal != invalidVal)
            {
                *begin = randVal * scalar;
                ++begin;
            }
        }
    }

    template<typename M, typename T>
    static bool CompareMat(const M& mat1, const M& mat2, T const eps)
    {
        auto str1 = glm::to_string(mat1);
        auto str2 = glm::to_string(mat2);

        qDebug() << str1.c_str() << Qt::endl;
        qDebug() << str2.c_str() << Qt::endl << Qt::endl;

        bool ok = true;
        for (int ii = 0; ii < mat1.length(); ++ii)
        {
            auto bv4 = glm::epsilonEqual(mat1[ii], mat2[ii], eps);
            ok = glm::all(bv4);
            assert(ok);
            if (!ok)
            {
                break;
            }
        }
        return ok;
    }

public:

    template<typename T>
    static void Test3Vertices(const glm::tvec3<T>& p0,
                                                                const glm::tvec3<T>& p1,
                                                                const glm::tvec3<T>& p2,
                                                                const glm::tvec2<T>& q0,
                                                                const glm::tvec2<T>& q1,
                                                                const glm::tvec2<T>& q2)
    {
        const glm::tvec3<T>* vv3[] = {&p0, &p1, &p2};
        const glm::tvec2<T>* vv2[] = {&q0, &q1, &q2};

        vector<glm::tmat2x3<T>> vecmat;
        for(int offset = 0; offset < 3; ++ offset)
        {
            glm::tmat2x3<T> matTB = AxisCoord<T>::CalcTangentBitangent(
                                             *vv3[(offset)%3],
                                             *vv3[(offset+1)%3],
                                             *vv3[(offset+2)%3],
                                             *vv2[(offset)%3],
                                             *vv2[(offset+1)%3],
                                             *vv2[(offset+2)%3]);
            qDebug() << glm::to_string(matTB).c_str();
            T dotpct = glm::dot(matTB[0], matTB[1]);
            qDebug() << "The dot product is " << dotpct << Qt::endl<< Qt::endl;

            vecmat.push_back(matTB);
            if(3 == vecmat.size())
            {
                const T eps = 1e-5;
                assert(CompareMat(vecmat[0], vecmat[1], eps));
                assert(CompareMat(vecmat[1], vecmat[2], eps));
            }
        }
    }

    static void Case2_TBN()
    {
        using T = double;

        const size_t dataSize = 10000;
        vector<T> data(dataSize);
        FillRandom(data.begin(), data.end(), 0.001, -99999, 99999, 0);

        const int loopCount = 20000;
        int index = 0;
        for (int ii = 0; ii < loopCount; ++ii)
        {
            glm::tvec3<T> p0{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};
            glm::tvec3<T> p1{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};
            glm::tvec3<T> p2{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};
            glm::tvec2<T> q0{data[index++%dataSize], data[index++%dataSize]};
            glm::tvec2<T> q1{data[index++%dataSize], data[index++%dataSize]};
            glm::tvec2<T> q2{data[index++%dataSize], data[index++%dataSize]};
//            qDebug() << glm::to_string(p0).c_str();
//            qDebug() << glm::to_string(p1).c_str();
//            qDebug() << glm::to_string(p2).c_str();
//            qDebug() << glm::to_string(q0).c_str();
//            qDebug() << glm::to_string(q1).c_str();
//            qDebug() << glm::to_string(q2).c_str();

            Test3Vertices(                                             p0,
                                                                       p1,
                                                                       p2,
                                                                       q0,
                                                                       q1,
                                                                       q2);

            glm::tmat2x3<T> matTB = AxisCoord<T>::CalcTangentBitangent(
                                             p0,
                                             p1,
                                             p2,
                                             q0,
                                             q1,
                                             q2 );
            qDebug() << glm::to_string(matTB).c_str();
            T dotpct = glm::dot(matTB[0], matTB[1]);
            qDebug() << "The dot product is " << dotpct << Qt::endl<< Qt::endl;

            glm::tmat2x3<T> matTB2 = AxisCoord<T>::CalcTangentBitangentByHand(
                                             p0,
                                             p1,
                                             p2,
                                             q0,
                                             q1,
                                             q2 );
            qDebug() << glm::to_string(matTB2).c_str();
            T dotpct2 = glm::dot(matTB2[0], matTB2[1]);
            qDebug() << "The dot product is " << dotpct2 << Qt::endl<< Qt::endl;

            const T eps = 1e-5;
            assert(CompareMat(matTB, matTB2, eps));
        }
    }

    static void Case1_glm_colum_row()
    {
        using T = double;
        glm::tmat2x3<T> m2x3{
            0, 1, 2,
            3, 4, 5
        };

        qDebug() << glm::to_string(m2x3).c_str() << Qt::endl;

        for(int ii = 0; ii < m2x3.length(); ++ ii)
        {
            auto vec = m2x3[ii];
            qDebug() << glm::to_string(vec).c_str() << Qt::endl;
        }
    }

    static void Case0_projection()
    {
        using T = double;

        const size_t dataSize = 10000;
        vector<T> data(dataSize);
        FillRandom(data.begin(), data.end(), 0.001, -99999, 99999, 0);

        const int loopCount = 20000;
        int index = 0;
        for (int ii = 0; ii < loopCount; ++ii)
        {
            T left = data[index++ % dataSize];
            T right = data[index++ % dataSize];
            T bottom = data[index++ % dataSize];
            T top = data[index++ % dataSize];
            T znear = data[index++ % dataSize];
            T zfar = data[index++ % dataSize];

            {
                auto mat = AxisCoord<T>::Ortho(left, right, bottom, top, znear, zfar);
                auto matGlm = glm::ortho(left, right, bottom, top, znear, zfar);
                const T eps = 1e-5;
                assert(CompareMat(mat, matGlm, eps));
            }
            {
                auto mat = AxisCoord<T>::Frustum(left, right, bottom, top, znear, zfar);
                auto matGlm = glm::frustum(left, right, bottom, top, znear, zfar);
                const T eps = 1e-5;
                assert(CompareMat(mat, matGlm, eps));
            }
            {
                glm::tvec3<T> eye{data[index++ % dataSize], data[index++ % dataSize], data[index++ % dataSize]};
                glm::tvec3<T> center{data[index++ % dataSize], data[index++ % dataSize], data[index++ % dataSize]};
                glm::tvec3<T> up{data[index++ % dataSize], data[index++ % dataSize], data[index++ % dataSize]};
                auto mat = AxisCoord<T>::LookAt(eye, center, up);
                auto matGlm = glm::lookAt(eye, center, up);
                const T eps = 1e-5;
                assert(CompareMat(mat, matGlm, eps));
            }
            {
                T radian = data[index++ % dataSize];
                glm::tvec3<T> rotationAxis{data[index++ % dataSize], data[index++ % dataSize], data[index++ % dataSize]};

                auto mat = AxisCoord<T>::RotateMat(radian, rotationAxis);

                static glm::tmat4x4<T> me{T{1}};
                auto matGlm = glm::tmat3x3<T>{glm::rotate(me, radian, rotationAxis)};

                const T eps = 1e-5;
                assert(CompareMat(mat, matGlm, eps));
            }

        }
    }
};
}
