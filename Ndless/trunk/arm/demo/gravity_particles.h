#ifndef _GRAVITY_PARTICLES_H_
#define _GRAVITY_PARTICLES_H_

#include "particle_system.h"
#include "utils.h"

typedef struct {
  t_particle_system particleSystem;
  float gravitationalConstant;
  //private ParticleComparer pc = new ParticleComparer();
  int accelerationMultiplier;

} t_gravity_particle;

extern void gravity_particles_construct(t_gravity_particle* t_this, float gravity, int accelerationMultiplier);
extern void gravity_particles_destruct(t_gravity_particle* t_this);
extern t_particle* gravity_particles_merge(t_gravity_particle* t_this, t_particle* i, t_particle* j);
extern void gravity_particles_draw(t_gravity_particle* t_this);

extern void add_particle(t_gravity_particle* gravity_particles);
extern void remove_particle(t_gravity_particle* gravity_particles);

#endif
