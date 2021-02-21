#include "FountainEmitter.h"
#include <iostream>
#include <algorithm>

FountainEmitter::FountainEmitter(Drawable *_model, int number) : IntParticleEmitter(_model, number) {}

void FountainEmitter::updateParticles(float time, float dt, glm::vec3 camera_pos) {

    //This is for the fountain to slowly increase the number of its particles to the max amount
    //instead of shooting all the particles at once
    if (active_particles < number_of_particles) {
        int batch = 30;
        int limit = std::min(number_of_particles - active_particles, batch);
        for (int i = 0; i < limit; i++) {
            createNewParticle(active_particles);
            active_particles++;
        }
    }
    else {
        active_particles = number_of_particles; //In case we resized our ermitter to a smaller particle number
    }

    for(int i = 0; i < active_particles; i++){
        particleAttributes & particle = p_attributes[i];

        if(particle.position.y < emitter_pos.y - 30.0f || particle.life == 0.0f || checkForCollision(particle)){
            createNewParticle(i);
        }

        if (particle.position.y > height_threshold)
        {
            createNewParticle(i);
        }

        particle.accel = glm::vec3(-particle.position.x, 0.0f, -particle.position.z); //gravity force

        //particle.rot_angle += 90*dt; 

        particle.position = particle.position + particle.velocity*dt + particle.accel*(dt*dt)*0.5f;
        particle.velocity = particle.velocity + particle.accel*dt;

        //*
        auto bill_rot = calculateBillboardRotationMatrix(particle.position, camera_pos);
        //particle.rot_axis = glm::vec3(bill_rot.x, bill_rot.y, bill_rot.z);
        //particle.rot_angle = glm::degrees(bill_rot.w);
        
        particle.rot_axis = glm::normalize(glm::vec3(-wind_z * 1.0f, 1.0f, wind_x * 1.0f));
        particle.rot_angle = fabs(wind_x) * (27.5f/30.0f) + (wind_z >= 0 ? (fabs(wind_z) * (27.5f / 30.0f)) : (fabs(wind_z) * (27.5f / 20.0f)));
        
        
        //*/
        //particle.dist_from_camera = length(particle.position - camera_pos);
        //particle.life = (height_threshold - particle.position.y) / (height_threshold - emitter_pos.y);
    }
}

bool FountainEmitter::checkForCollision(particleAttributes& particle)
{
    return particle.position.y < 0.0f;
}

void FountainEmitter::createNewParticle(int index){
    particleAttributes & particle = p_attributes[index];

    particle.position = emitter_pos + glm::vec3(sin(RAND * 6.28f) * 70, RAND, 85 * sin(RAND * 6.28f)) + glm::vec3((wind_x + 10) * 0.5f - 10.0f, 0.0f, (wind_z + 10) * 0.7f - 50.0f);
    particle.velocity = -glm::vec3( 5 - RAND*(wind_x+10), 10, 5 - RAND*(wind_z+10))*4.0f;
    //particle.velocity = -glm::vec3(2, 2, 2)*4.0f;

    particle.mass = RAND + 0.5f;
    particle.rot_axis = glm::normalize(glm::vec3(1 - 2*RAND, 1 - 2*RAND, 1 - 2*RAND));
    //particle.rot_axis = glm::vec3(1.0f, 1.0f, 1.0f);
    
    particle.accel = glm::vec3(0.0f, -9.8f, 0.0f); //gravity force
    particle.rot_angle = RAND*360;
    
    particle.life = 1.0f; //mark it alive
}

