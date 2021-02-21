#include "OrbitEmitter.h"
#include <iostream>
OrbitEmitter::OrbitEmitter(Drawable *_model, int number, float _radius_min, float _radius_max) : IntParticleEmitter(_model, number), radius_min(_radius_min), radius_max(_radius_max){
    particle_radius.resize(number_of_particles, 0.0f);
    for(int i = 0; i < number_of_particles; i++){
        createNewParticle(i);
    }
}

void OrbitEmitter::updateParticles(float time, float dt, glm::vec3 camera_pos) {
    for (int i = 0; i < number_of_particles; i++) {
        auto& p = p_attributes[i];

        p.rot_angle += dt;
        p.position = emitter_pos + glm::vec3(particle_radius[i] * 
            sin(p.rot_angle), 0.0f, particle_radius[i] * 
            cos(p.rot_angle));
    }
}

void OrbitEmitter::createNewParticle(int index) {
    particleAttributes& particle = p_attributes[index];

    particle_radius[index] = RAND * (radius_max - radius_min) + 
        radius_min;

    particle.rot_angle = 360 * RAND;
    particle.rot_axis = glm::normalize(glm::vec3(1 - 2 * RAND, 1 - 2 * RAND, 1 - 2 * RAND));
    particle.mass = RAND + 0.5f;
    particle.life = 1.0f; //mark it alive
}
