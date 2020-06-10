// client.c
#include "pse.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>

#define CMD   "client"

int Search(char SongName);
void Add(char SongName,const char *FileName);
int PlaySong(const char* SongName );


int main(int argc, char *argv[]) {
  int sock, ret;
  struct sockaddr_in *adrServ;
  int fin = FAUX;
  char ligne[LIGNE_MAX];

  signal(SIGPIPE, SIG_IGN);

  if (argc != 3)
    erreur("usage: %s machine port\n", argv[0]);

  printf("%s: creating a socket\n", CMD);
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    erreur_IO("socket");

  printf("%s: DNS resolving for %s, port %s\n", CMD, argv[1], argv[2]);
  adrServ = resolv(argv[1], argv[2]);
  if (adrServ == NULL)
    erreur("adresse %s port %s inconnus\n", argv[1], argv[2]);

  printf("%s: adr %s, port %hu\n", CMD,
	        stringIP(ntohl(adrServ->sin_addr.s_addr)),
	        ntohs(adrServ->sin_port));

  printf("%s: connecting the socket\n", CMD);
  ret = connect(sock, (struct sockaddr *)adrServ, sizeof(struct sockaddr_in));
  if (ret < 0)
    erreur_IO("connect");

  while (!fin) {
    printf("ligne> ");
    if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
      // sortie par CTRL-D
      fin = VRAI;
    else {
      if (ecrireLigne(sock, ligne) == -1)
        erreur_IO("ecriture socket");

      if (strcmp(ligne, "fin\n") == 0)
        fin = VRAI;
    }
  }

  if (close(sock) == -1)
    erreur_IO("fermeture socket");

  exit(EXIT_SUCCESS);
}


int Search(const char *SongName){
  int res = fseek(SongName);
  if (res == 0){
    printf(" La chanson est dans le repertoire.\n");
    return 0;
  }
  else {
    printf("La chanson n'est pas dans le repertoire. \n");
    return 1;
  }
}

int PlaySong(const char* SongName)
{
   int continuer = 1;
   SDL_Init(SDL_INIT_VIDEO);
   SDL_Surface *ecran = NULL;
   ecran = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
   SDL_Event event;
   SDL_WM_SetCaption("SDL_Mixer", NULL);
   SDL_Flip(ecran);
   if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
   {
      printf("%s", Mix_GetError());
   }
   Mix_VolumeMusic(MIX_MAX_VOLUME / 2); //Mettre le volume à la moitié
   Mix_Music *musique; //Création d'un pointeur de type Mix_Music
   musique = Mix_LoadMUS(strcat(SongName,".mp3")); //Chargement de la musique
   Mix_PlayMusic(musique, -1); //Jouer infiniment la musique
   while(continuer)
   {
      SDL_WaitEvent(&amp;event);
      switch(event.type)
      {
         case SDL_QUIT:
            continuer = 0;
            break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
                case SDLK_p:
                    if(Mix_PausedMusic() == 1)//Si la musique est en pause
                    {
                        Mix_ResumeMusic(); //Reprendre la musique
                    }
                    else
                    {
                        Mix_PauseMusic(); //Mettre en pause la musique
                    }
                    break;
                case SDLK_BACKSPACE:
                    Mix_RewindMusic(); //Revient au début de la musique
                    break;
                case SDLK_ESCAPE:
                    Mix_HaltMusic(); //Arrête la musique
                    break;
            }
            break;
      }
   }
   Mix_FreeMusic(musique); //Libération de la musique
   Mix_CloseAudio(); //Fermeture de l'API
   SDL_Quit();
   return EXIT_SUCCESS;
}
