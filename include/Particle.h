#ifndef PARTICLE_H_
#define PARTICLE_H_

#include "ngl/Vec3.h"

struct Particle
{
  Particle()=default;

  Particle(ngl::Vec3 _pos, ngl::Vec3 _dir, ngl::Vec3 _randy,int _life, float _size=0.1f, ngl::Vec3 _colour={1,1,1},float _randomness = 5) :
      pos{_pos},dir{_dir},life{_life},size{_size},colour{_colour}
      {

      }

  ngl::Vec3 pos;
  ngl::Vec3 dir;
  ngl::Vec3 colour;
  ngl::Vec3 randy;
  float randomness = 5;
  int life=100;
  float size=0.1f;
  bool isAlive = false; // Note isAlive is not contained in the Constructor like the other parameters
};


#endif
