# Taran Jones CFGAA Assignment
## What it looks like
![AssignmentScreenie](https://github.com/NCCA/cfgaa24programingassignment-TaranUni/assets/159461684/536ca8e1-cf1f-4da7-a006-d5aad25b4445)

[This is a video of the program running](https://github.com/NCCA/cfgaa24programingassignment-TaranUni/blob/main/Program%20Footage.mp4)

## Assignment Ideas

- Flip Fluid Sim
- Eulerian Fluid Sim
- Fire Sim
- **Particle Based Fluid Sim**

## What i'm planning on doing

I am using the particle system made during John Macey's workshops/ lessons as the basis for the code/ system.

### ~~Eulerian Fluid Sim, if this is too simple I will extend it to being a Flip fluid Sim.~~

~~In the demo by [TenMinutePhysics](https://matthias-research.github.io/pages/tenMinutePhysics/index.html "Link to TenMinutePhysics")
on [Eulerian fluid](https://matthias-research.github.io/pages/tenMinutePhysics/17-fluidSim.html "Fluid Sim") he has multiple test enviroments for the simulation,
I think if I start with creating the wind tunnel enviroment this should be the simplest, then I can potentially add the paint test enviroment as I think this is the nicest one to interact with.
I will also just stick with showing just the smoke view.~~

### Particle Based Fluid Sim

After some thinking and looking through the Euler method, I think I will switch to using the particle based method instead as we have been working on something similar in workshops.
I have decided to use this video as referance [Sebastian Lague](https://www.youtube.com/watch?v=rSKMYc1CQHE&) on how to create a realistic particle based fluid sim. I think this method will be easier to implement with current code.

I found it hard to use Sebastian Lagues video to implement the optimization step as I found some parts had explanations which made the problem seem more confusing than it was, for instance the collision radius effect was overwhelming to look at and understand, when the effect radius of every particle is shown at once, instead of 2 or a few of them.

To keep the video short I only simulated 5000 particles at once allowing the box to fill quicker, although because of this the start is a bit slow on the FPS because of the way the optimization I implemented works, I found this video by [Pixel Physics](https://www.youtube.com/watch?v=J1thcSAT9Dc&list=PLzLQjQOvhO0bBCWPsERzBpuXG7HboGumv&index=14&) the best at explaining how it works. Essentially though the optimization works by creating a grid in this case 3D and use it as a way to index every particles general position.

### ~~**Fire VFX** I am now just using the particles for this.~~
I gave up on the fluid sim due to time constraints and complexity in optimising the code.


#### Old Collision code
```C++
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
```
Because of this I am going to try making a first person scene with a campfire without particle inter-particle interaction.

## Checklist

~~Particle generation~~

~~Bounding box~~

set up particle spheres of influence/ smoothing

### Shader handling 
-- Partially working currently working on intergrating with camera controls

This picuture is before implementing the PBR shader, in this version I was using custom shaders.
![This picuture is before implementing the PBR shader, in this version I was using custom shaders.][logo]

[logo]: https://github.com/NCCA/cfgaa24programingassignment-TaranUni/assets/159461684/435d290b-4761-4bf0-a9c2-0dc7e90e4d53"

**inter particle interaction**

Improve camera controls
-- Made Field of View Adjustable

Add user interaction with particles

Added VAOPrimitive with transforms, shaders and camera controls
-- I had the particles and disk in the same scene, but the disk would slowly move upwards instead of staying still, currently trying to fix

Create a UI

## Design of classes/ Data structures

I am using GLSL, C++, CMake and [NGL](https://github.com/NCCA/NGL.git) for this program.

Currently using ngl VAO GL_POINTS to create simple particles

The Particle data structure is stored in "Particle.h" as part of a struct. This also contains the default particle attribute declaration.

The Emmiters parameters and functions are stored in "Emmiter.h" as part of a class.

## How the program flows
The program starts with Main.cpp then opens NGLScene (this is where the main code loop occurs).

In NGLScene the window is set to grey the light's attributes are set, the emmiter and 
