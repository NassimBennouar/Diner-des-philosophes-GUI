/*Nassim Bennouar TP App*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>

#define PENSE 0
#define MANGE 1
#define FAIM 2
typedef struct setDeTable{
	sem_t * fourchettes;
	int n;
} SetDeTable;

struct sigaction action;

int stop=0;
pthread_t * threads;
int n_philosophes;

void handler(int signum){
	switch(signum){
		case SIGINT:
			stop=1;
			fflush(stdout);
			printf("Fin du repas des philosophes\n");
			int i;
			for(i=0;i<n_philosophes;i++){
				pthread_cancel(threads[i]);
			}
			return;
		default:
			printf("Cogitum Ergosum\n");
			return;
	}
}

void * philosophe(void * arg){

	SetDeTable * setTable = (SetDeTable *) arg; 

	int n=setTable->n;
	int nmax=n_philosophes;

	int suivant=(n+1)%nmax;
	int precedent=n-1;
	if(precedent<0)
		precedent=(nmax-1)-n;


	
	while(!stop){
		printf("Je pense, donc je suis le philosophe %d\n", n);
		sleep(1);
		//setTable->status[n]=FAIM;
		printf("En tant que philosophe %d, je souhaite manger\n", n);
		sleep(1);
		//if(setTable->status[suivant]!=MANGE && setTable->status[precedent]!=MANGE){
			sem_wait(&(setTable->fourchettes[n]));
			sem_wait(&(setTable->fourchettes[suivant]));
			//etTable->status[n]=MANGE;
			printf("Il était temps que moi, philosophe %d, puisse manger\n", n);
			sleep(1);
			sem_post(&(setTable->fourchettes[n]));
			sem_post(&(setTable->fourchettes[suivant]));
			printf("Philosophe %d a bien mange, et vous salue\n", n);
			sleep(1);
			//setTable->status[n]=PENSE;
		//}
	}

	pthread_exit(NULL);
}

void philosophons(int nmax){
	n_philosophes=nmax;
	action.sa_handler=handler;
	action.sa_flags=0;
	sigemptyset(&action.sa_mask);
	/*action2.sa_handler=handlerThread;
	action2.sa_flags=SA_SIGINFO;
	sigemptyset(&action2.sa_mask);*/

	threads = NULL;
	threads=(pthread_t *) malloc(nmax*sizeof(pthread_t));
	sem_t * fourchettes=NULL;
	fourchettes=(sem_t *) malloc(nmax*sizeof(sem_t));
	//sem_t * global_sem=NULL;
	//global_sem=(sem_t *) malloc(sizeof(sem_t));
	//int * status=NULL;
	//status=(int *) calloc(nmax,sizeof(int));
	SetDeTable ** setDeSets=NULL;
	setDeSets=(SetDeTable **) malloc(nmax*sizeof(SetDeTable *));
	if(!threads||!fourchettes||!setDeSets){
		fprintf(stderr,"Erreur malloc");
		exit(EXIT_FAILURE);
	}

	int i;
	//sem_init(global_sem,0,1);

	for(i=0;i<nmax;i++){
		//status[i]=PENSE;
		sem_init(&fourchettes[i],0,1);
	}

	for(i=0;i<nmax;i++){
		setDeSets[i]=(SetDeTable *) malloc(sizeof(SetDeTable));
		//setDeSets[i]->global_sem=global_sem;
		setDeSets[i]->fourchettes=fourchettes;
		setDeSets[i]->n=i;
		//setDeSets[i]->status=status;
		pthread_create(&threads[i], NULL, philosophe, setDeSets[i]);
	}

	sigaction(SIGINT,&action,NULL);

	while(!stop){
		pause();
		//printf("hehe %d\n", stop);
	}

	
	//fflush(stdout);
	for(i=0;i<nmax;i++){
		pthread_join(threads[i], NULL);
		free(setDeSets[i]);
	}

	free(threads);
	free(setDeSets);
	free(fourchettes);

	//Faut éviter les use after free
	threads=NULL;
	setDeSets=NULL;
	fourchettes=NULL;
	
	printf("La main est l'outil de l'outil :)\n");
}

int main(int argc, char *argv[]){
	if(argc>1 && atoi(argv[1])>2)
		n_philosophes=atoi(argv[1]);
	else
		n_philosophes=5;

	philosophons(n_philosophes);

	exit(EXIT_SUCCESS);
}