//---------------------------------------------------------------//
//                                                               //
//                            AUDIO.CPP                          //
//                                                               //
//                              Source code by Marko             //
//                              http://www.gamedesign-online.com //
//---------------------------------------------------------------//


#include "audio.h"

audio::audio()
{
     for (int i=0; i<AUDIO_SOUND_COUNT; i++)
         wavSounds[i] = NULL;
         
     if ( Mix_OpenAudio(AUDIO_SAMPLE_FRQ, AUDIO_S16SYS, 2, AUDIO_CHUNK_SIZE) != 0 )
           printf("\tUnable to initialize audio: %s\n", Mix_GetError());
}

audio::~audio()
{
    Mix_CloseAudio();
}

void audio::playSound(int id)
{
     if ( wavSounds[id] )
     {
        if( Mix_PlayChannel(-1,wavSounds[id],0) == -1) 
	        printf("\n***ERROR*** Unable to play WAV file: %s\n", Mix_GetError());
     } 
}


// This function is used to set the volume of a specific sound sample
// Values below 0 won't be taken into account, and values above 128 will
// set the volume to its max value (128)
void audio::setSoundVolume(int id, int volume)
{
     if ( wavSounds[id] )
        Mix_VolumeChunk( wavSounds[id], volume );
}



void audio::loadWavSoundData()
{
         FILE *f_conf = fopen("data/sounds_wav.dat","r");
    if ( !f_conf )
       printf("*** ERROR *** can't load data/sounds_wav.dat config file\n");
     else 
     {
          printf("Opening WAV audio config file\n");
          printf("==========================================\n");
          char* line = new char[256];
          char* path = new char[256];
          char* data = new char[256];
          
          int nb   = 0;

          while (fgets(line,256,f_conf)!=NULL)
          {
              nb   = -1;
                
              if ( !strstr( line, "#" ) && (strlen(line)>3) )
              {
                    char* r = line;
                    char* w = data;
                    
                    // Load cell id
                    while (r && *r!='\n' && *r!='\r' && *r!='[')
                          r++;
                    r++;
                    while (r && *r!='\n' && *r!='\r' && *r!=']')
                          *w++ = *r++;
                    *w = 0;
                    nb = atoi(data);
                    w = path;
                    
                    // Load sound path 
                    while (r && *r!='\n' && *r!='\r' && *r!='[' )
                          r++;
                    r++;
                    while (r && *r!='\n' && *r!='\r' && *r!=']' )
                          *w++ = *r++;
                    *w = 0;
                    w = data;
                    
                    if ( nb < AUDIO_SOUND_COUNT && nb > -1)
                    {
                            if ( ! wavSounds[nb] )
                            {
                                  printf ("\tLoading sound '%s' to cell [%d]\n", path, nb);
                                  wavSounds[nb] = Mix_LoadWAV(path);
                                  if ( !wavSounds[nb] )
                                     printf("\tUnable to load sound : %s\n", Mix_GetError());
                             }
                             else
                                  printf ("\t***ERROR*** cell [%d] has already been allocated... can't load sound '%s'\n",nb,path); 
                    }
                    else
                        printf ("\t***ERROR*** cell [%d] does not exist... can't load sound '%s'\n",nb,path); 
              }
          }
          delete line;
          delete path;
          delete data;
          fclose(f_conf);
          printf("\n\n");
     }

}
