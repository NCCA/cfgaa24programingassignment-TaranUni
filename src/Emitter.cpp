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
  _p.size=0.1f;
  _p.dir.set(0,0,0);
  _p.isAlive = false;
}

void Emitter::createDefaultParticle(Particle &_p)
{
  _p.pos=m_position;
//  std::cout<<"update\n";
  _p.dir=m_emitDir * ngl::Random::randomPositiveNumber() + randomVectorOnSphere() * m_spread;
  _p.dir.m_y = std::abs(_p.dir.m_y);
  _p.colour = ngl::Vec3(1.0f,1.0f,1.0f);
  _p.life = static_cast<int>(2.0f+ngl::Random::randomPositiveNumber(150));
  _p.size= 0.1f;
  _p.randomness = 5;
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
    glPointSize(10);

    // Bind the VAO
    m_vao->bind();
    {
        // Set the vertex data for the particles
        m_vao->setData(ngl::AbstractVAO::VertexData(m_particles.size() * sizeof(Particle), m_particles[0].pos.m_x));

        // Position attribute
        m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(Particle), 0);

        // Color attribute (assuming it starts at offset 6 in Particle)
        m_vao->setVertexAttributePointer(1, 3, GL_FLOAT, sizeof(Particle), offsetof(Particle, colour));

        // Size attribute (assuming it starts at offset 12 in Particle)
        m_vao->setVertexAttributePointer(2, 1, GL_FLOAT, sizeof(Particle), offsetof(Particle, size));

        // Set the number of indices
        m_vao->setNumIndices(m_particles.size());

        // Draw the particles
        m_vao->draw();
    }
    // Unbind the VAO
    m_vao->unbind();
}


/*
void Emitter::render() const
{
//  ngl::Mat4 look=ngl::lookAt({0,150,150},{0,0,0},{0,1,0});
//  ngl::Mat4 persp=ngl::perspective(45.0f,1.0,0.1,200);
  glPointSize(10);
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
*/

void Emitter::initializeGrid(int sizeX, int sizeY, int sizeZ, float cellSize)
{
    m_gridSizeX = sizeX;
    m_gridSizeY = sizeY;
    m_gridSizeZ = sizeZ;
    m_cellSize = cellSize;

    m_grid.resize(m_gridSizeX);
    for (int x = 0; x < m_gridSizeX; ++x)
    {
        m_grid[x].resize(m_gridSizeY);
        for (int y = 0; y < m_gridSizeY; ++y)
        {
            m_grid[x][y].resize(m_gridSizeZ);
        }
    }
}

void Emitter::indexParticleInGrid(Particle& p)
{
    int x = static_cast<int>((p.pos.m_x + m_gridSizeX * m_cellSize / 2) / m_cellSize);
    int y = static_cast<int>((p.pos.m_y + m_gridSizeY * m_cellSize / 2) / m_cellSize);
    int z = static_cast<int>((p.pos.m_z + m_gridSizeZ * m_cellSize / 2) / m_cellSize);

    // Ensure particle is within grid bounds
    if (x >= 0 && x < m_gridSizeX && y >= 0 && y < m_gridSizeY && z >= 0 && z < m_gridSizeZ)
    {
        m_grid[x][y][z].particles.push_back(&p);
    }
}

void Emitter::update()
{
    // Clear the grid
    for (auto& plane : m_grid)
    {
        for (auto& row : plane)
        {
            for (auto& cell : row)
            {
                cell.particles.clear();
            }
        }
    }

    // Update particles
    float _dt = 0.1f; // Delta Time
    ngl::Vec3 gravity(0, -9.87, 0); // Gravity

    // Iterate over all particles
    for (auto& p : m_particles)
    {
        if (p.isAlive)
        {
            ngl::ShaderLib::setUniform("Colour",400*p.colour.m_r,400*p.colour.m_g,400*p.colour.m_b,1.0f);
            p.colour -= ngl::Vec3(0.0f,0.0f,0.05f);
            if (p.colour.m_z < 0)
            {
                p.colour -= ngl::Vec3(0.0f,0.02f,0.0f);
            }
            if (p.colour.m_y < 0)
            {
                p.colour -= ngl::Vec3(0.01f,0.0f,0.0f);
            }

            /// Redirects points to center
            if (p.pos.m_x > 10)
            {
                p.dir += ngl::Vec3(1,0,0);
            }
            if (p.pos.m_x < -10)
            {
                p.dir -= ngl::Vec3(1,0,0);
            }
            if (p.pos.m_z > 10)
            {
                p.dir += ngl::Vec3(0,0,1);
            }
            if (p.pos.m_z < -10)
            {
                p.dir -= ngl::Vec3(0,0,10);
            }

//        p.dir += gravity * _dt * 0.5 / p.randomness; // for campfire
            p.dir += gravity * _dt * 0.5; // for fluid
            p.pos += p.dir * _dt;
            p.size -= 0.1f;
            /// Kill particle
            if (--p.life <= 0 && p.pos.m_y >= 200.0)
            {
                createZeroParticle(p);
                p.isAlive = false;
            }
            /// Create Boundary Box Collision
            handleBoundaryCollisions(p);

            // Index particle in the grid
            indexParticleInGrid(p);
        }
    }

    // Iterate over grid cells to perform collision checks only for nearby particles
    for (int x = 0; x < m_gridSizeX; ++x)
    {
        for (int y = 0; y < m_gridSizeY; ++y)
        {
            for (int z = 0; z < m_gridSizeZ; ++z)
            {
                // Get nearby particles within a 2-grid radius
                auto& currentCell = m_grid[x][y][z];
                for (auto& particle : currentCell.particles)
                {
                    for (int offsetX = -2; offsetX <= 2; ++offsetX)
                    {
                        for (int offsetY = -2; offsetY <= 2; ++offsetY)
                        {
                            for (int offsetZ = -2; offsetZ <= 2; ++offsetZ)
                            {
                                // Calculate neighboring cell coordinates
                                int neighborX = x + offsetX;
                                int neighborY = y + offsetY;
                                int neighborZ = z + offsetZ;

                                // Check if neighbor cell is within grid bounds
                                if (neighborX >= 0 && neighborX < m_gridSizeX &&
                                    neighborY >= 0 && neighborY < m_gridSizeY &&
                                    neighborZ >= 0 && neighborZ < m_gridSizeZ)
                                {
                                    // Iterate over nearby particles in the neighbor cell
                                    auto& neighborCell = m_grid[neighborX][neighborY][neighborZ];
                                    for (auto& neighborParticle : neighborCell.particles)
                                    {
                                        // Perform collision check between current particle and neighbor particle
                                        handleParticleCollision(*particle, *neighborParticle);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

ngl::Vec3 normalized(const ngl::Vec3 &v)
{
    ngl::Vec3 result = v;
    result.normalize();
    return result;
}

void Emitter::handleParticleCollision(Particle &particleA, Particle &particleB)
{
    // Calculate the distance between the two particles
    ngl::Vec3 displacement = particleA.pos - particleB.pos;
    float distance = displacement.length();

    // If the distance is less than a threshold (e.g., sum of radii), they collide
    float collisionThreshold = particleA.size + particleB.size;
    if (distance < collisionThreshold)
    {
        // Handle collision effects (e.g., bounce off, merge, etc.)
        // Here's a simple example of particles bouncing off each other:
        ngl::Vec3 relativeVelocity = particleA.dir - particleB.dir; // Calculate relative velocity
        float relativeSpeed = relativeVelocity.dot(normalized(displacement)); // Calculate relative speed along the normal

        // Check if particles are moving towards each other
        if (relativeSpeed < 0)
        {
            // Apply collision response - reverse velocities along the collision normal
            ngl::Vec3 collisionNormal = normalized(displacement);
            particleA.dir -= 2 * relativeSpeed * collisionNormal;
            particleB.dir += 2 * relativeSpeed * collisionNormal;
        }
    }
}





/*
void Emitter::handleParticleCollision(Particle& particleA, Particle& particleB)
{
    // Calculate the distance between the two particles
    float distance = (particleA.pos - particleB.pos).length();

    // If the distance is less than a threshold (e.g., sum of radii), they collide
    float collisionThreshold = particleA.size + particleB.size;
    if (distance < collisionThreshold)
    {
        // Handle collision effects (e.g., bounce off, merge, etc.)
        // Here's a simple example of particles bouncing off each other:
        ngl::Vec3 relativeVelocity = particleA.dir - particleB.dir; // Calculate relative velocity
        float relativeSpeed = relativeVelocity.dot(relativeVelocity); // Calculate relative speed along the normal

        // Check if particles are moving towards each other
        if (relativeSpeed < (0,0,0))
        {
            // Apply collision response - reverse velocities along the collision normal
            particleA.dir -= 2 * relativeSpeed * particleA.dir;
            particleB.dir -= 2 * relativeSpeed * particleB.dir;
        }
    }
}*/


/*
void Emitter::update()
{
    // Clear the grid
    for (auto& plane : m_grid)
    {
        for (auto& row : plane)
        {
            for (auto& cell : row)
            {
                cell.particles.clear();
            }
        }
    }

//    usleep(50000); // Delays the update for Debugging purposes.
float _dt=0.1f; // Delta Time
ngl::Vec3 gravity(0,-9.87, 0); // Gravity
ngl::Vec3 random(0,0, 0); // random direction vector
static int numP =0;
bool collision = false;

// choose number to birth
// find first not alive and set as new particle
int numberToBirth=10+ngl::Random::randomPositiveNumber(10);

for(int i=0; i<numberToBirth; ++i)
{
  auto p = std::find_if(std::begin(m_particles),std::end(m_particles),
                        [](auto p)
                        {
                          return p.isAlive==false;
                        });
  createDefaultParticle(*p);
}
  for(auto &p : m_particles)
  {
    if (p.isAlive == true)
    {
        ngl::ShaderLib::setUniform("Colour",400*p.colour.m_r,400*p.colour.m_g,400*p.colour.m_b,1.0f);
        p.colour -= ngl::Vec3(0.0f,0.0f,0.05f);
        if (p.colour.m_z < 0)
        {
            p.colour -= ngl::Vec3(0.0f,0.02f,0.0f);
        }
        if (p.colour.m_y < 0)
        {
            p.colour -= ngl::Vec3(0.01f,0.0f,0.0f);
        }

        /// Redirects points to center
        if (p.pos.m_x > 10)
        {
            p.dir += ngl::Vec3(1,0,0);
        }
        if (p.pos.m_x < -10)
        {
            p.dir -= ngl::Vec3(1,0,0);
        }
        if (p.pos.m_z > 10)
        {
            p.dir += ngl::Vec3(0,0,1);
        }
        if (p.pos.m_z < -10)
        {
            p.dir -= ngl::Vec3(0,0,10);
        }

//        p.dir += gravity * _dt * 0.5 / p.randomness; // for campfire
        p.dir += gravity * _dt * 0.5; // for fluid
        p.pos += p.dir * _dt;
        p.size -= 0.1f;
        /// Kill particle
      if (--p.life <= 0 && p.pos.m_y >= 200.0)
      {
          createZeroParticle(p);
          p.isAlive = false;
      }
      /// Create Boundary Box Collision
      handleBoundaryCollisions(p);
    }
  }
}
*/

// Returns the color of the Particle.
ngl::Vec4 Particle::getColour() const
{
    return ngl::Vec4(colour,1.0f);
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