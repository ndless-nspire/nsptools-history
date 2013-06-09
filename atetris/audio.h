//---------------------------------------------------------------//
//                                                               //
//                            AUDIO.H                            //
//                                                               //
//                              Source code by Marko             //
//                              http://www.gamedesign-online.com //
//---------------------------------------------------------------//


// The aim of this file is to provide an audio framework, and 
// make the sound handling much easier


#include "config.h"

class audio
{
      public:
             audio();
             ~audio();
             
             void loadWavSoundData();
             void playSound(int id);
             void setSoundVolume(int id, int volume);
      private:
              Mix_Chunk *wavSounds[AUDIO_SOUND_COUNT];
};
