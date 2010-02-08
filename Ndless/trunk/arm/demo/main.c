/*****************************************************************************
 * @(#) Ndless - Particle System Demo (main file)
 *
 * Copyright (C) 2010 by ANNEHEIM Geoffrey
 * Contact: geoffrey.anneheim@gmail.com
 *
 * Original code by BoneSoft:
 * http://www.codeproject.com/KB/GDI-plus/FunWithGravity.aspx
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * RCSID $Id$
 *****************************************************************************/

#include "os.h"
#include "utils.h"
#include "gravity_particles.h"

asm(".string \"PRG\"\n");

int main(void) {
  t_gravity_particles gravity_particles;
  t_particle_system* particle_system;
  t_particle* p;
  int i;

  showSimpleDialogBox(
    "Ndless - Particle System Demo",
    "-------------------------------\n"
    "                Particle System Demo\n" \
    "      Copyright© 2010 by Ndless Team\n"
    "-------------------------------\n"
    "+  Add a particle\n"
    "-  Remove a particle\n"
    "*  Increase gravity\n"
    "/  Decrease gravity\n"
    "C  Enable/Disable collision detection\n"
    "T  Enable/Disable trace mode\n"
    "S  Clear screen\n"
    "ESC - Exit"
  );

  for (i = 0; i < 0x0F; ++i) {
    fondu(1);
    WAIT("0x1FFFFF", "fondu");
  }

  gravity_particles_construct(&gravity_particles, 0.00006672f, 100);
  particle_system = &(gravity_particles.particleSystem);
  particle_system->detectCol = false;
  particle_system->trace = false;
  particle_system_start(particle_system);

  p = particle_construct3(&(t_vector){0.f, 0.f, 0.f}, &(t_vector){0.f, 0.f, 0.f}, 1500.f, 4, BLACK);
  (void) particle_system_addParticle(particle_system, p);
  add_particle(&gravity_particles);

  while (!isKeyPressed(KEY_NSPIRE_ESC)) {
    gravity_particles_draw(&gravity_particles);

    // Add a particule (+)
    if (isKeyPressed(KEY_NSPIRE_PLUS)) {
      if (particle_system->nParticles == 0) {
        p = particle_construct3(&(t_vector){0.f, 0.f, 0.f}, &(t_vector){0.f, 0.f, 0.f}, 1500.f, 4, BLACK);
        (void) particle_system_addParticle(particle_system, p);
      }
      add_particle(&gravity_particles);
      while (isKeyPressed(KEY_NSPIRE_PLUS));
    }

    // Remove a particle (-)
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
      particle_system->detectCol = !particle_system->detectCol;
      while (isKeyPressed(KEY_NSPIRE_C));
    }

    // Collision (T)
    if (isKeyPressed(KEY_NSPIRE_T)) {
      particle_system->trace = !particle_system->trace;
      while (isKeyPressed(KEY_NSPIRE_T));
    }

    // Collision (S)
    if (isKeyPressed(KEY_NSPIRE_S)) {
      clearScreen();
      while (isKeyPressed(KEY_NSPIRE_S));
    }
  }

  gravity_particles_destruct(&gravity_particles);

  return 0;
}
