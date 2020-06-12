#include "AltClient.h"

int main(int argc, char *argv[]){

  char  action;
  printf("------- Menu Client -------\n0 - Rechercher une chanson \n1 - Ajouter une chanson \n2 - Jouer une chanson \n3 - Quitter \n");
  fgets(&action,LIGNE_MAX,stdin);
  
  if (action == '0'){
    int sfd =0, b;
    char* rbuff = (char*) malloc(1024*sizeof(char));
    char* SongName = (char*) malloc(100*sizeof(char)); 

    struct sockaddr_in serv_addr;

    memset(rbuff, '0', sizeof(1024*sizeof(char)));
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    b = connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    
    if (b==-1) {
        perror("Connect");
        return 1;
    }
      printf("Rechercher sur MusicMine le fichier (mettre ../ devant le nom du fichier): ");
      scanf("%s",SongName);
      
      FILE* Present;
      Present = fopen(SongName, "r");

      if (Present != NULL){
        printf(" La chanson est dans le repertoire.\n");
      }
      else {
        printf(" La chanson n'est pas dans le repertoire. \n");
      }
  }

  else if (action == '1'){
    int sfd =0, b;
    char* rbuff = (char*) malloc(1024*sizeof(char));
    char* sendbuffer = (char*) malloc(100*sizeof(char));
    char* SongName = (char*) malloc(LIGNE_MAX*sizeof(char)); 
    struct sockaddr_in serv_addr;

    memset(rbuff, '0', sizeof(1024*sizeof(char)));
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    b = connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (b==-1) {
        perror("Connect");
        return 1;
    }
    
    printf("Ajouter sur MusicMine le fichier : ");
    gets(SongName, LIGNE_MAX, stdin);
    printf("%s\n", SongName);

    FILE *fp = fopen(SongName, "rb");
    
    send(sfd, SongName, strlen(SongName), 0);
    printf("%s: Ajout en cours.\n", CMD);

    if(fp == NULL){
      perror("File");
      return 2;
    }

    while( (b = fread(sendbuffer, 1, sizeof(sendbuffer), fp))>0 ){
      send(sfd, sendbuffer, b, 0);
    }
    fclose(fp);
    printf("%s: Fin de l'ajout.\n", CMD);
  } 

  else if (action == '2'){
    int sfd =0, b;
    char* rbuff = (char*) malloc(1024*sizeof(char));
    char* SongName = (char*) malloc(100*sizeof(char)); 

    struct sockaddr_in serv_addr;

    memset(rbuff, '0', sizeof(1024*sizeof(char)));
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    b = connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (b==-1) {
        perror("Connect");
        return 1;
    }
    printf("Jouer depuis MusicMine le fichier (ajouter ../ devant le nom du fichier): ");
    scanf("%s", SongName);
    PlaySong(SongName);
  }
}


/*int SearchSong( char *SongName){
  if (recv() != NULL){
    printf(" La chanson est dans le repertoire.\n");
    return 0;
  }
  else {
    printf(" La chanson n'est pas dans le repertoire. \n");
    return 1;
  }
}*/

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
   musique = Mix_LoadMUS(SongName); //Chargement de la musique
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
