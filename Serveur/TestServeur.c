#include "TestServeur.h"

DataSpec dataSpec[NB_WORKERS];
sem_t semWorkersLibres;
pthread_mutex_t mutexCanal[NB_WORKERS];

int main(int argc, char *argv[]){

    int fd =0, confd = 0, b, tot, ret, numWorkerLibre;
    struct sockaddr_in serv_addr, adrClient;
    unsigned int lgAdrClient;
    char buff[1025];

    creerCohorteWorkers();

    ret = sem_init(&semWorkersLibres, 0, NB_WORKERS);
    if (ret == -1){
        erreur_IO("init semaphore workers libres");
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("%s: Socket cree\n", CMD);

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(buff, '0', sizeof(buff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    ret = bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret < 0){
        erreur_IO("bind");
    }

    printf("%s: Ecoute le socket\n", CMD);
    ret = listen(fd, 10);
    if (ret < 0){
        erreur_IO("listen");
    }

    while(1){
        printf("%s: Accepte une connexion\n", CMD);
        lgAdrClient = sizeof(adrClient);
        confd = accept(fd, (struct sockaddr*)&adrClient, &lgAdrClient);
        if (confd==-1) {
            perror("Accept");
            continue;
        }
        printf("%s: Connexion du client acceptee.\n", CMD);
        char ligne[LIGNE_MAX];
        lireLigne(confd, ligne);

        ret = sem_wait(&semWorkersLibres);
        if (ret == -1){
            erreur_IO("wait semaphore workers libres");
        }
        numWorkerLibre = chercherWorkerLibre();

        dataSpec[numWorkerLibre].canal = confd;
        ret = sem_post(&dataSpec[numWorkerLibre].sem);
        if (ret == -1){
            erreur_IO("post semaphore worker");
        }

        FILE* fp = (FILE*) malloc(sizeof(FILE));
        fp = fopen( ligne, "wb");
        tot=0;
        if(fp != NULL){
            while( (b = recv(confd, buff, 1024,0))> 0 ) {
                tot+=b;
                fwrite(buff, 1, b, fp);
            }

            printf("%s: Fichier envoye a MusicMine \n Bytes recus: %d\n", CMD, tot);
            if (b<0){
               perror("Receiving");
            }

            fclose(fp);
        }
        else{
            perror("File");
        }
        printf("%s: Deconnexion du client.\n", CMD);
        close(confd);
    }

    return 0;
}

void creerCohorteWorkers(void) {
  int i, ret;

  for (i = 0; i < NB_WORKERS; i++) {
    dataSpec[i].canal = -1;
    dataSpec[i].tid = i;

    ret = sem_init(&dataSpec[i].sem, 0, 0);
    if (ret == -1)
      erreur_IO("init semaphore worker");

    ret = pthread_create(&dataSpec[i].id, NULL, threadWorker, &dataSpec[i]);
    if (ret != 0)
      erreur_IO("creation worker");
  }
}

int chercherWorkerLibre(void) {
  int i, canal;

  for (i = 0; i < NB_WORKERS; i++) {
    lockMutexCanal(i);
    canal = dataSpec[i].canal;
    unlockMutexCanal(i);
    if (canal < 0)
      return i;
  }

  return -1;
}

void *threadWorker(void *arg) {
  DataSpec *dataSpec = (DataSpec *)arg; 
  int ret;

  while (VRAI) {
    ret = sem_wait(&dataSpec->sem);
    if (ret == -1){
      erreur_IO("wait semaphore worker");
    }

    sessionClient(dataSpec->canal);

    lockMutexCanal(dataSpec->tid);
    dataSpec->canal = -1;
    unlockMutexCanal(dataSpec->tid);

    ret = sem_post(&semWorkersLibres);
    if (ret == -1){
      erreur_IO("post semaphore workers libres");
    }
  }

  pthread_exit(NULL);
}

void sessionClient(int canal) {
  int fin = FAUX;
  //char ligne[LIGNE_MAX];
  char* ligne = (char*) malloc(LIGNE_MAX*sizeof(char));
  int lgLue;

  while (!fin) {
    lgLue = recv(canal, ligne, sizeof(ligne), 0);
    if (lgLue == -1)
      erreur_IO("lecture canal");

    else if (lgLue == 0) {  // arret du client
      fin = VRAI;
      printf("%s: arret du client\n", CMD);
    }
  }

}

void lockMutexCanal(int numWorker)
{
  int ret;

  ret = pthread_mutex_lock(&mutexCanal[numWorker]);
  if (ret != 0)
    erreur_IO("lock mutex canal");
}

void unlockMutexCanal(int numWorker)
{
  int ret;

  ret = pthread_mutex_unlock(&mutexCanal[numWorker]);
  if (ret != 0)
    erreur_IO("unlock mutex canal");
}