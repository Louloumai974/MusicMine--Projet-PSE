#include "pse.h"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#define CMD         "serveur"
#define NB_WORKERS 2

void creerCohorteWorkers(void);
int chercherWorkerLibre(void);
void *threadWorker(void *arg);
void sessionClient(int canal);
void lockMutexCanal(int numWorker);
void unlockMutexCanal(int numWorker);