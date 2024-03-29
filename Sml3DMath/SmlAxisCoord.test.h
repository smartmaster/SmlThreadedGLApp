#pragma once

#include <vector>
#include <random>
#include <string>
#include <iostream>
#include <cassert>
#include <QDebug>
#include <glm/gtx/string_cast.hpp>

#include "Sml3DMath/SmlGlmUtils.h"
#include "Sml3DMath/SmlMiscUtils.h"

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
            //assert(ok);
            if (!ok)
            {
                break;
            }
        }
        return ok;
    }

    template<typename V, typename T>
    static bool CompareVec(const V& v1, const V& v2, T const eps)
    {
        auto str1 = glm::to_string(v1);
        auto str2 = glm::to_string(v2);

        qDebug() << str1.c_str() << Qt::endl;
        qDebug() << str2.c_str() << Qt::endl << Qt::endl;

        bool ok = true;
        auto bv4 = glm::epsilonEqual(v1, v2, eps);
        ok = glm::all(bv4);
        //assert(ok);
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
            glm::tmat2x3<T> matTB = MatVecUtils<T>::CalcTangentBitangent(
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
            glm::tvec3<T> normalp0{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};
            glm::tvec3<T> p0{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};
            glm::tvec3<T> p1{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};
            glm::tvec3<T> p2{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};
            glm::tvec3<T> normalq0{0, 0, data[index++%dataSize]};
            glm::tvec3<T> q0{data[index++%dataSize], data[index++%dataSize], 0};
            glm::tvec3<T> q1{data[index++%dataSize], data[index++%dataSize], 0};
            glm::tvec3<T> q2{data[index++%dataSize], data[index++%dataSize], 0};
            //            qDebug() << glm::to_string(p0).c_str();
            //            qDebug() << glm::to_string(p1).c_str();
            //            qDebug() << glm::to_string(p2).c_str();
            //            qDebug() << glm::to_string(q0).c_str();
            //            qDebug() << glm::to_string(q1).c_str();
            //            qDebug() << glm::to_string(q2).c_str();

            Test3Vertices(p0,
                          p1,
                          p2,
                          glm::tvec2<T>{q0},
                          glm::tvec2<T>{q1},
                          glm::tvec2<T>{q2});

            glm::tmat2x3<T> matTB = MatVecUtils<T>::CalcTangentBitangent(
                        p0,
                        p1,
                        p2,
                        glm::tvec2<T>{q0},
                        glm::tvec2<T>{q1},
                        glm::tvec2<T>{q2});
            qDebug() << glm::to_string(matTB).c_str();
            T dotpct = glm::dot(matTB[0], matTB[1]);
            qDebug() << "The dot product is " << dotpct << Qt::endl<< Qt::endl;

            glm::tmat2x3<T> matTB2 = MatVecUtils<T>::CalcTangentBitangentByHand(
                        p0,
                        p1,
                        p2,
                        glm::tvec2<T>{q0},
                        glm::tvec2<T>{q1},
                        glm::tvec2<T>{q2} );
            qDebug() << glm::to_string(matTB2).c_str();
            T dotpct2 = glm::dot(matTB2[0], matTB2[1]);
            qDebug() << "The dot product is " << dotpct2 << Qt::endl<< Qt::endl;

            const T eps = 1e-5;
            assert(CompareMat(matTB, matTB2, eps));

            auto tbn = MatVecUtils<T>::CalcTangentBitangentNormal(
                        normalp0,
                        p0,
                        p1,
                        p2,
                        normalq0,
                        q0,
                        q1,
                        q2);
            auto strtbn = glm::to_string(tbn);

            qDebug() << strtbn.c_str();

            auto matq = glm::tmat3x3<T>{
                    glm::tvec3<T>{q1-q0},
                    glm::tvec3<T>{q2-q0},
                    glm::tvec3<T>{normalq0}
        };
            auto matq1 = glm::inverse(tbn) * glm::tmat3x3<T>{
                    glm::tvec3<T>{p1-p0},
                    glm::tvec3<T>{p2-p0},
                    glm::tvec3<T>{normalp0}
        };

            assert(CompareMat(matq, matq1, eps));

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

    static void CaseMisc()
    {
        using T = double;
        auto rr = MiscUtils<T>::Mix(-100.0, 100.0, -200, 200, 0);
        assert(rr == 0.0);
    }

    static void Case0_projection()
    {
        using T = double;

         CaseMisc();

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
            T fov = data[index++ % dataSize];
            T aspect = data[index++ % dataSize];

            {
                auto mat = GlmUtils<T>::Ortho(left, right, bottom, top, znear, zfar);
                auto matNZ = GlmUtils<T>::OrthoNegZ(left, right, bottom, top, -znear, -zfar);
                auto matGlm = glm::ortho(left, right, bottom, top, znear, zfar);
                const T eps = 1e-5;
                assert(CompareMat(mat, matGlm, eps));
                assert(CompareMat(matNZ, matGlm, eps));
            }
            {
                auto mat = GlmUtils<T>::Frustum(left, right, bottom, top, znear, zfar);
                auto matNZ = GlmUtils<T>::FrustumNegZ(left, right, bottom, top, -znear, -zfar);
                auto matGlm = glm::frustum(left, right, bottom, top, znear, zfar);
                const T eps = 1e-5;
                assert(CompareMat(mat, matGlm, eps));
                assert(CompareMat(-matNZ, matGlm, eps));
            }
            {
                auto mat = GlmUtils<T>::Perspective(fov, aspect, znear, zfar);
                auto matNZ = GlmUtils<T>::PerspectiveNegZ(fov, aspect, -znear, -zfar);
                auto matGlm = glm::perspective(fov, aspect, znear, zfar);
                const T eps = 1e-5;
                assert(CompareMat(mat, matGlm, eps));
                assert(CompareMat(-matNZ, matGlm, eps));
            }
            {
                glm::tvec3<T> eye{data[index++ % dataSize], data[index++ % dataSize], data[index++ % dataSize]};
                glm::tvec3<T> center{data[index++ % dataSize], data[index++ % dataSize], data[index++ % dataSize]};
                glm::tvec3<T> up{data[index++ % dataSize], data[index++ % dataSize], data[index++ % dataSize]};
                auto mat = GlmUtils<T>::LookAt(eye, center, up);
                auto matGlm = glm::lookAt(eye, center, up);
                const T eps = 1e-5;
                assert(CompareMat(mat, matGlm, eps));
            }
            {
                T radian = data[index++ % dataSize];
                glm::tvec3<T> rotationAxis{data[index++ % dataSize], data[index++ % dataSize], data[index++ % dataSize]};

                auto mat = MatVecUtils<T>::RotateMat(radian, rotationAxis);

                static glm::tmat4x4<T> me{T{1}};
                auto matGlm = glm::tmat3x3<T>{glm::rotate(me, radian, rotationAxis)};

                const T eps = 1e-5;
                assert(CompareMat(mat, matGlm, eps));
            }
            {
                glm::tvec3<T> OO{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};
                glm::tvec3<T> HH{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};
                glm::tvec3<T> VV{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};
                glm::tvec3<T> ZZ{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};




                AxisCoord<T> coord;
                coord.MakeFromOHVZ(OO, HH, VV, ZZ, false);
                coord.SetIsBaseAxis(false);
                {
                    const T eps = 1e-5;
                    auto&  coordAxis = coord.GetAxis();
                    auto& coordOrigin = coord.GetOrigin();
                    assert(glm::epsilonEqual(glm::length(OO), glm::length(coordOrigin), eps));
                    assert(glm::epsilonEqual(glm::length(coordAxis[0]), glm::length(HH), eps));
                    assert(glm::epsilonEqual(glm::length(coordAxis[1]), glm::length(VV), eps));
                    assert(glm::epsilonEqual(glm::length(coordAxis[2]), glm::length(ZZ), eps));
                }

                {
                    glm::tvec3<T> model{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};

                    auto world = coord.ModelToWorld(model);
                    auto model1 = coord.WorldToModel(world);
                    const T eps = 1e-5;
                    CompareVec(model, model1, eps);

                    auto m2w = coord.ModelToWorldMat();
                    auto world1 = MatVecUtils<T>::M4xP3(m2w, model);
                    CompareVec(world, world1, eps);

                    auto w2m = coord.WorldToModelMat();
                    auto model2 = MatVecUtils<T>::M4xP3(w2m, world);
                    CompareVec(model2, model1, eps);
                }

                {
                    glm::tvec3<T> world{data[index++%dataSize], data[index++%dataSize], data[index++%dataSize]};

                    auto model = coord.WorldToModel(world);
                    auto world1 = coord.ModelToWorld(model);
                    const T eps = 1e-5;
                    CompareVec(world, world1, eps);

                    auto m2w = coord.ModelToWorldMat();
                    auto world2 = MatVecUtils<T>::M4xP3(m2w, model);
                    CompareVec(world2, world, eps);

                    auto w2m = coord.WorldToModelMat();
                    auto model1 = MatVecUtils<T>::M4xP3(w2m, world);
                    CompareVec(model1, model, eps);
                }

            }

        }
    }
};
}
