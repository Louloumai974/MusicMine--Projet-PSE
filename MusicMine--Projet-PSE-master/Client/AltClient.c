#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <sys/stat.h>


#define LIGNE_MAX 300 

int SearchSong( char *SongName);
int PlaySong( char* SongName );


int main(int argc, char *argv[]){

    int sfd =0, b;
    char rbuff[1024];
    char sendbuffer[100];
	char SongName[100]; 

    struct sockaddr_in serv_addr;

    memset(rbuff, '0', sizeof(rbuff));
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    b=connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (b==-1) {
        perror("Connect");
        return 1;
    }
    char  action;
    printf("---------- Menu----------\n0 - Rechercher une chanson \n1 - Ajouter une chanson \n2 - Jouer une chanson \n3 - Quitter \n");
    fgets(&action,LIGNE_MAX,stdin);
    switch(action){
      case '0':
        printf("Rechercher sur MusicMine le fichier : ");
        fgets(SongName,LIGNE_MAX,stdin);
        SearchSong(SongName);
        break;

      case '1':
        printf("Ajouter sur MusicMine le fichier : ");
        fgets(SongName,LIGNE_MAX,stdin);
        printf("%s", SongName);
        if (SearchSong(strcat(SongName, ".mp3")) == 1){
          printf("on ajoute la chanson\n");
          FILE *fp = fopen(SongName, "r");
          if(fp == NULL){
            perror("File");
            return 2;
          }
          while( (b = fread(sendbuffer, 1, sizeof(sendbuffer), fp))>0 ){
              send(sfd, sendbuffer, b, 0);
          }
        fclose(fp);
        }
        break;
        
      case '2':
        printf("Jouer depuis MusicMine le fichier :");
        fgets(SongName,LIGNE_MAX,stdin);
        break;
      
      case '3' :
        break;
    }
}


int SearchSong( char *SongName){
  struct stat *buf; 
  buf = (struct stat *) malloc(sizeof(struct stat)); 
  int res = stat(strcat(SongName, ".mp3"), buf);
  if (res == 0){
    printf(" La chanson est dans le repertoire.\n");
    return 0;
  }
  else {
    printf("La chanson n'est pas dans le repertoire. \n");
    return 1;
  }
}

int PlaySong( char* SongName)
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
   musique = Mix_LoadMUS(strcat(SongName, ".mp3")); //Chargement de la musique
   Mix_PlayMusic(musique, -1); //Jouer infiniment la musique
   while(continuer)
   {
      SDL_WaitEvent(&event);
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
