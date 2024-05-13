#ifndef EMITTER_H_
#define EMITTER_H_

#include <vector>
#include <string_view>
#include "Particle.h"
#include "ngl/Vec3.h"
#include <ngl/AbstractVAO.h>
#include <memory>

class Emitter
{
public :
  Emitter(int _numParticles, int _maxAlive);
  void update();
  void render() const;
  void writeToGeo(std::string_view _fname);
private :
    ngl::Vec3 randomVectorOnSphere();
  void createDefaultParticle(Particle &_p);
  void createZeroParticle(Particle &_p);
  void handleBoundaryCollisions(Particle &_p);
  void handleParticleCollisions(Particle &_p);
  std::vector<Particle> m_particles;
  ngl::Vec3 m_position={0,0,0};
  float m_spread = 15.0f; // how far particles are ejected
  ngl::Vec3 m_emitDir = {0,20.0f,0};
  int m_maxAlive;
  std::unique_ptr<ngl::AbstractVAO> m_vao;
};

// create a temp vector for position that updates at the end of each loop, this vector will represent the previous instance and allow the co

#endif