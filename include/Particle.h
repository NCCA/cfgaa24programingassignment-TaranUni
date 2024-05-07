#ifndef PARTICLE_H_
#define PARTICLE_H_

#include "ngl/Vec3.h"

struct Particle
{
  Particle()=default;

  Particle(ngl::Vec3 _pos, ngl::Vec3 _dir, int _life, float _size=0.1f, ngl::Vec3 _colour={1,1,1}) :
      pos{_pos},dir{_dir},life{_life},size{_size},colour{_colour}
      {

      }

  ngl::Vec3 pos;
  ngl::Vec3 dir;
  ngl::Vec3 colour;
  int life=100;
  float size=0.01f;
  bool isAlive = false;
};


#endif
