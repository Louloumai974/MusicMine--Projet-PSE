#include "pse.h"
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

#define CMD         "client"

//int SearchSong( char *SongName);
int PlaySong( char* SongName );