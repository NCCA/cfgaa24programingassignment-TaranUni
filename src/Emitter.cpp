#include "Emitter.h"
#include "ngl/Random.h"
#include <fmt/format.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ngl/Util.h>
#include <ngl/Transformation.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/VAOFactory.h>
#include <unistd.h>

Emitter::Emitter(int _numParticles, int _maxAlive)
{
  m_particles.resize(_numParticles);
  m_maxAlive=_maxAlive;
  for(auto &p : m_particles)
  {
    createZeroParticle(p);
  }
  m_vao = ngl::VAOFactory::createVAO(ngl::simpleVAO,GL_POINTS); //create Vertex Array Object
}

void Emitter::createZeroParticle(Particle &_p)
{
  _p.pos.set(0.0f,0.0f,0.0f);
  _p.size=0.0f;
  _p.dir.set(0,0,0);
  _p.isAlive = false;
}

void Emitter::createDefaultParticle(Particle &_p)
{
  _p.pos=m_position;
//  std::cout<<"update\n";
  _p.dir=m_emitDir * ngl::Random::randomPositiveNumber() + randomVectorOnSphere() * m_spread;
  _p.dir.m_y = std::abs(_p.dir.m_y);
  _p.colour = ngl::Random::getRandomColour3();
  _p.life = static_cast<int>(2.0f+ngl::Random::randomPositiveNumber(150));
  _p.size= 0.01f;
  _p.radius = 1;
  _p.dist = 1;
  _p.isAlive = true;
}

ngl::Vec3 Emitter::randomVectorOnSphere()
{
    auto phi = ngl::Random::randomPositiveNumber(M_PI * 2.0f);
    auto costheta = ngl::Random::randomNumber();
    auto theta = acos(costheta);
    auto u = ngl::Random::randomPositiveNumber();
    auto r = 1.0f *std::cbrt(u);
    return ngl::Vec3(r* sinf(theta) * cosf(phi),
                     r* sinf(theta) * sinf(phi),
                     r* cosf(theta));
}

void Emitter::render() const
{
//  ngl::Mat4 look=ngl::lookAt({0,150,150},{0,0,0},{0,1,0});
//  ngl::Mat4 persp=ngl::perspective(45.0f,1.0,0.1,200);
//  glPointSize(10);
//  ngl::ShaderLib::setUniform("MVP", persp*look);
  m_vao ->bind();
  {
      m_vao->setData(ngl::AbstractVAO::VertexData(m_particles.size()*sizeof(Particle),m_particles[0].pos.m_x));
      m_vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(Particle),0);
      m_vao->setVertexAttributePointer(1,3,GL_FLOAT,sizeof(Particle),6);

      m_vao->setNumIndices(m_particles.size());
      m_vao->draw();
  }
  m_vao->bind();

//  ngl::Transformation tx;
//  for(auto p : m_particles)
//  {
//      tx.setPosition(p.pos);
//      tx.setScale(p.size,p.size,p.size);
//      ngl::ShaderLib::setUniform("MVP", persp*look*tx.getMatrix());
//      ngl::ShaderLib::setUniform("Colour",p.colour.m_r,p.colour.m_g,p.colour.m_b,1.0f);
//      ngl::VAOPrimitives::draw(ngl::buddah);
////    fmt::print("{} ,{}, {} \n",p.pos.m_x,
////               p.pos.m_y, p.pos.m_z);
//  }
}

//void Emitter::update()
//{
////    usleep(50000); // Delays the update for Debugging purposes.
//float _dt=0.1f; // Delta Time
//ngl::Vec3 gravity(0,-9.87, 0); // Gravity
//static int numP =0;
//bool collision = false;
//
//// choose number to birth
//// find first not alive and set as new particle
//int numberToBirth=10+ngl::Random::randomPositiveNumber(150);
//
//for(int i=0; i<numberToBirth; ++i)
//{
//  auto p = std::find_if(std::begin(m_particles),std::end(m_particles),
//                        [](auto p)
//                        {
//                          return p.isAlive==false;
//                        });
//  createDefaultParticle(*p);
//}
//  for(auto &p : m_particles)
//  {
//    if (p.isAlive == true)
//    {
//      p.dir += gravity * _dt * 0.5;
//      p.pos += p.dir * _dt;
//      p.size += 0.01f;
//
//      /// Kill particle
//      if (--p.life <= 0 && p.pos.m_y <= -4.0)
//      {
//         createZeroParticle(p);
//      }
//      /// Create Boundary Box Collision
//      if (p.pos.m_y <= -4.0)
//      {
//          p.dir.set(p.dir.m_x,p.dir.m_y *= -1,p.dir.m_z);
//      }
//      if (p.pos.m_x <= -100.0)
//      {
//          p.dir.set(p.dir.m_x *= -1,p.dir.m_y,p.dir.m_z);
//      }
//      if (p.pos.m_x >= 100.0)
//      {
//          p.dir.set(p.dir.m_x *= -1,p.dir.m_y,p.dir.m_z);
//      }
//      if (p.pos.m_z <= -100.0)
//      {
//          p.dir.set(p.dir.m_x,p.dir.m_y,p.dir.m_z *= -1);
//      }
//      if (p.pos.m_z >= 100.0)
//      {
//          p.dir.set(p.dir.m_x,p.dir.m_y,p.dir.m_z *= -1);
//      }
//
////      /// Inter-Particle collision handling
////      if (bool collision == true)
////      {
////          p.dir.set(0,0,0);
////      }
//    }
//  }
//}

void Emitter::update()
{
    // Define necessary variables
    float _dt = 0.1f; // Delta Time
    ngl::Vec3 gravity(0, -9.87, 0); // Gravity

    // choose number to birth
    // find first not alive and set as new particle
    int numberToBirth = 10 + ngl::Random::randomPositiveNumber(10);

    for (int i = 0; i < numberToBirth; ++i)
    {
        auto p = std::find_if(std::begin(m_particles), std::end(m_particles), [](auto p)
        {
            return p.isAlive == false;
        });
        createDefaultParticle(*p);
    }

    // Vector to store particle indices and positions
    std::vector<std::pair<size_t, ngl::Vec3>> particleIndicesAndPositions;

    // Loop through each particle
    for (size_t i = 0; i < m_particles.size(); ++i)
    {
        auto &p = m_particles[i];

        if (p.isAlive == true)
        {
            // Apply gravity
            p.dir += gravity * _dt * 0.5;

            // Update position
            p.pos += p.dir * _dt;
            p.size += 1.0f;

            // Kill particle if life ends and it reaches the bottom
            if (--p.life <= 0 && p.pos.m_y <= -4.0)
            {
                createZeroParticle(p);
            }

            // Handle boundary collisions
            handleBoundaryCollisions(p);

            // Store particle index and position
            particleIndicesAndPositions.push_back(std::make_pair(i, p.pos));
        }
    }

    // Now all particle indices and positions stored in particleIndicesAndPositions vector
    // Loop through this vector and compare positions to detect collisions
    for (const auto &pair : particleIndicesAndPositions)
    {
        size_t index = pair.first;
        ngl::Vec3 pos = pair.second;

        // Loop through all particles to check for collisions with current particle
        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            if (i != index && m_particles[i].isAlive)
            {
                // Calculate distance between particles
                float distSq = (pos - m_particles[i].pos).lengthSquared();

                // If particles collide
                if (distSq < (m_particles[index].size + m_particles[i].size) * (m_particles[index].size + m_particles[i].size))
                {
                    // Implement collision response
                    // For simplicity I'm just reversing their velocities
                    m_particles[index].dir = -m_particles[index].dir;
                    m_particles[i].dir = -m_particles[i].dir;
                }
            }
        }
    }
}


void Emitter::handleBoundaryCollisions(Particle &p)
{
    // Boundary conditions
    if (p.pos.m_y <= -4.0)
    {
        p.dir.set(p.dir.m_x, p.dir.m_y *= -1, p.dir.m_z); // Reverse y direction
    }
    if (p.pos.m_x <= -100.0 || p.pos.m_x >= 100.0)
    {
        p.dir.set(p.dir.m_x *= -1, p.dir.m_y, p.dir.m_z); // Reverse x direction
    }
    if (p.pos.m_z <= -100.0 || p.pos.m_z >= 100.0)
    {
        p.dir.set(p.dir.m_x, p.dir.m_y, p.dir.m_z *= -1); // Reverse z direction
    }
}

//void Emitter::handleParticleCollisions(Particle &p)
//{
//    // Loop through all particles to check for collisions with p
//    for (auto &other : m_particles)
//    {
//        if (&p != &other && other.isAlive == true)
//        {
//            // Calculate distance between particles
//            float distSq = (p.pos - other.pos).lengthSquared();
//            int maxCollisionDistance = 1.0f;
//
//            // Check if particles are within the collision range
//            if (distSq < maxCollisionDistance * maxCollisionDistance)
//            {
//                // If particles collide
//                if (distSq < (p.size + other.size) * (p.size + other.size))
//                {
//                    // Implement collision response
//                    // Here, you can adjust the direction or velocity of both particles
//                    // For simplicity, let's just reverse their velocities
//                    p.dir = -p.dir;
//                    other.dir = -other.dir;
//                }
//            }
//        }
//    }
//}

void Emitter::writeToGeo(std::string_view _fname)
{
  std::cout<<"writing "<<_fname<<'\n';
  auto file = std::ofstream(_fname.data());
  if(file.is_open())
  {
    file<<"PGEOMETRY V5\n";
    file << "NPoints "<< m_particles.size() << " NPrims 1 \n";
    file << "NPointGroups 0 NPrimGroups 0 \n";
    file << "NPointAttrib 2 NVertexAttrib 0 NPrimAttrib 1 NAttrib 0\n";
    file << "PointAttrib \n";
    file << "Cd 3 float 0 0 0\n";
    file << "pscale 1 float 0.1 \n";
    for(auto p : m_particles)
    {
      file << p.pos.m_x <<' '<<p.pos.m_y<<' '<<p.pos.m_z<<' '<<" 1 (";
      file<<p.colour.m_x<<' '<<p.colour.m_y<<' '<<p.colour.m_z<<' ';
      file<< p.size<<' ';
      file<<")\n";
    } // end of per particle data
    file << "PrimitiveAttrib \n";
    file << "generator 1 index 1 papi \n";
    file << "Part ";
    file << m_particles.size()<<' ';
    for(size_t i=0; i<m_particles.size(); ++i)
      file<< i<<' ';
    file <<"[0] \n";
    file<<"beginExtra\nendExtra\n";
  }
}