//
// Created by zhoujg on 2022/12/15.
//

#ifndef SPLINES_CURVE_H
#define SPLINES_CURVE_H

#include "Splines/Spline.h"

//namespace baltam::splines {
//    template<int Dim, int Order, SplineType t>
//    struct Curve: public extern_obj_base {
//        Spline<Dim,Order,t> sp_;
//        BALTAM_LOCAL static int ID;
//        [[nodiscard]] extern_obj_base *dup() const override;
//        [[nodiscard]] std::string to_string() const override;
//        ~Curve() override;
//    };
//
//    template<int Dim, int Order, SplineType t>
//    int Curve<Dim,Order,t>::ID = 0;
//
//    template<int Dim, int Order, SplineType t>
//    extern_obj_base *Curve<Dim,Order,t>::dup() const {
//        auto *ret = new Curve<Dim,Order,t>(*this);
//        ret->sp_ = sp_;
//        return ret;
//    }
//
//    template<int Dim, int Order, SplineType t>
//    std::string Curve<Dim,Order,t>::to_string() const {
//        return "tets_to_string()";
//    }
//
//    template<int Dim, int Order, SplineType t>
//    Curve<Dim,Order,t>::~Curve()= default;
//}

#endif //SPLINES_CURVE_H

