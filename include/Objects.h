#ifndef CFGAAPROGRAMMINGASSIGNMENT_OBJECTS_H
#define CFGAAPROGRAMMINGASSIGNMENT_OBJECTS_H

#include "ngl/Vec3.h"
#include "ngl/Vec4.h"

struct Object
{
    Object()=default;

    Object(ngl::Vec3 _pos, ngl::Vec3 _dir, float _size=0.1f) :
            pos{_pos},dir{_dir},size{_size}
    {

    }

    ngl::Vec3 pos;
    ngl::Vec3 dir;
    ngl::Vec4 colour;
    float size=0.1f;
};

#endif //CFGAAPROGRAMMINGASSIGNMENT_OBJECTS_H
