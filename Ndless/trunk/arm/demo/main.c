#include "os.h"
#include "utils.h"
#include "gravity_particles.h"

asm(".string \"PRG\"\n");

int main(void) {
  t_gravity_particle gravity_particles;
  t_particle* p;
  int i;

  showSimpleDialogBox(
    "Ndless - Particle System Demo",
    "-------------------------\n"
    "        Particle System Demo\n" \
    "Copyright© 2010 by Ndless Team\n"
    "-------------------------\n"
    "+  Add a particle\n"
    "-  Delete a particle\n"
    "*  Increase gravity\n"
    "/  Decrease gravity\n"
    "C  Enable/Disable collision detection\n"
    "ESC - Exit"
  );

  for (i = 0; i < 0x0F; ++i) {
    fondu(1);
    WAIT("0x1FFFFF", "fondu");
  }

  gravity_particles_construct(&gravity_particles, 0.00006672f, 100);
  gravity_particles.particleSystem.detectCol = false;
  particle_system_start(&gravity_particles.particleSystem);

  p = particle_construct3(&(t_vector){0.f, 0.f, 0.f}, &(t_vector){0.f, 0.f, 0.f}, 1500.f, 4, BLACK);
  (void) particle_system_addParticle(&gravity_particles.particleSystem, p);
  add_particle(&gravity_particles);

  while (!isKeyPressed(KEY_NSPIRE_ESC)) {
    gravity_particles_draw(&gravity_particles);

    // Add particule (+)
    if (isKeyPressed(KEY_NSPIRE_PLUS)) {
      if (gravity_particles.particleSystem.nParticles == 0) {
        p = particle_construct3(&(t_vector){0.f, 0.f, 0.f}, &(t_vector){0.f, 0.f, 0.f}, 1500.f, 4, BLACK);
        (void) particle_system_addParticle(&gravity_particles.particleSystem, p);
      }
      add_particle(&gravity_particles);
      while (isKeyPressed(KEY_NSPIRE_PLUS));
    }

    // Remove particle (-)
    if (isKeyPressed(KEY_NSPIRE_MINUS)) {
      remove_particle(&gravity_particles);
      while (isKeyPressed(KEY_NSPIRE_MINUS));
    }

    // High gravity (*)
    if (isKeyPressed(KEY_NSPIRE_MULTIPLY)) {
      gravity_particles.gravitationalConstant *= 10.f;
      while (isKeyPressed(KEY_NSPIRE_MULTIPLY));
    }

    // Low gravity (/)
    if (isKeyPressed(KEY_NSPIRE_DIVIDE)) {
      gravity_particles.gravitationalConstant /= 10.f;
      while (isKeyPressed(KEY_NSPIRE_DIVIDE));
    }

    // Collision (C)
    if (isKeyPressed(KEY_NSPIRE_C)) {
      gravity_particles.particleSystem.detectCol = !gravity_particles.particleSystem.detectCol;
      while (isKeyPressed(KEY_NSPIRE_C));
    }
  }

  gravity_particles_destruct(&gravity_particles);

  return 0;
}
