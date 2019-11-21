/* Inclusion de las librerias necesarias */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

/* Definicion de las funciones */
int calculaAleatorios(int min, int max);
void manejadoraSomelier(int s);
void manejadoraJefeSala(int s);
void manejadoraMozo(int s);
void manejadoraPinche(int s);

/* Funcion principal */
int main(int argc, char const *argv[]){
	/* Compruebo que se ha introducido un solo valor positivo como argumento */
	if (argc != 2){
		printf("Error: Parametros incorrectos.\n");
		return -1;
	} 
	else if(atoi(argv[1]) < 1){
		printf("Error: El número de pinches no puede ser menor que 1.\n");
		return -1;
	}
	printf("Se ha creado el chef con pid %d hijo de %d.\n" ,getpid(), getppid());
	int numPinches = atoi(argv[1]);
	srand(getpid());

	pid_t pidSomelier, pidJefeSala;
	pid_t *pidPinches = (pid_t*)malloc(sizeof(pid_t)*numPinches);

	struct sigaction ssom, sjef, spin;

	pidJefeSala = fork();
	if(pidJefeSala == 0){
		printf("Se ha creado el jefe de sala con pid %d hijo de %d.\n" ,getpid(), getppid());
		sjef.sa_handler = manejadoraJefeSala;
		if(-1 == sigaction(SIGUSR1, &sjef, NULL)){
			perror("Jefe de Sala: sigaction");
		}
		pause();
		exit(1);
	}
	else if(pidJefeSala == -1){
		perror("Error en la llamada fork");
	}else{
		pidSomelier = fork();
		if(pidSomelier == 0){
			printf("Se ha creado el sommelier con pid %d hijo de %d.\n" ,getpid(), getppid());
			ssom.sa_handler = manejadoraSomelier;
			if(-1 == sigaction(SIGUSR1, &ssom, NULL)  || -1 == sigaction(SIGUSR2, &ssom, NULL)){
				perror("Jefe de Sala: sigaction");
			}
			pause();
		} 
		else if(pidSomelier == -1){
			perror("Error en la llamada fork");
		} else{
			int i;
			for(i = 0; i<numPinches; i++){
				*(pidPinches+i) = fork();
				if(*(pidPinches+i) == 0){
					printf("Se ha creado el pinche %d con pid %d hijo de %d.\n" ,i+1, getpid(), getppid());
					spin.sa_handler = manejadoraPinche;
					if(-1 == sigaction(SIGUSR1, &spin, NULL)){
						perror("Pinche: sigaction");
					}
					pause();
					break;
				}
			}

			sleep(3);
			int ingredientes = calculaAleatorios(0,1);
			if(ingredientes == 0){
				printf("Chef: Me faltan ingredientes.\n");
				kill(pidSomelier, SIGUSR1);
			} else{
				printf("Chef: Me falta vino.\n");
				kill(pidSomelier, SIGUSR2);
			}
			int status;
			wait(&status);
			int queFalta = WEXITSTATUS(status);
			switch(queFalta){
				case 1:
					//No hay vino, se acaba el programa.
					kill(SIGTERM, pidJefeSala);
					//Matar a los pinches
					printf("Chef: No hay vino, así no hay quien trabaje.\n");
					return 0;
				break;
				case 2:
					//No hay algun ingrediente, no se acaba el programa pero hay que avisar
					printf("Chef: Falta algun ingrediente, pero vamos allá.\n");
				break;
			}
			//Hay que poner a trabajar a los pinches y contabilizar los platos
			int platosCreados = 0;
			for( i=0; i<numPinches; i++){
				printf("Chef: Haz un plato pinche %d.\n", i+1);
				kill(*(pidPinches+i) , SIGUSR1);
				int status;
				wait(&status);
				int seCocino = WEXITSTATUS(status);
				if(seCocino == 1){
					platosCreados++;
				}
			}
			if(platosCreados == 0){
				printf("Chef: No hay platos, cerramos.\n");
				kill(pidJefeSala, SIGTERM);
				return 0;
			} else{
				int status;
				kill(pidJefeSala, SIGUSR1);
				wait(&status);
			}
		}
	}

	printf("PUEDE ABRIRSE EL RESTAURANTE.");
	return 0;
}

int calculaAleatorios(int min, int max){
	return rand() % (max-min+1) + min;
}

void manejadoraSomelier(int s){
	printf("Somelier: Recibido, ahora mismo te digo Chef.\n");
	struct sigaction smoz;
	pid_t pidMozo = fork();
	if (pidMozo == 0){
		printf("Se ha creado el mozo con pid %d hijo de %d.\n" ,getpid(), getppid());
		smoz.sa_handler = manejadoraMozo;
		if(-1 == sigaction(SIGPIPE, &smoz, NULL)){
			perror("Mozo: sigaction");
		}
		printf("El mozo ha enmascarado la pipe y se va a la camique.\n");
		pause();
		printf("Al mozo le ha llegado una señal.\n");
	} else{
		sleep(5);
		int status, loEncontro;
		if(s == SIGUSR1){
			printf("Somelier: ¿Has oido mozo? Necesitamos ingredientes.\n");
			kill(pidMozo, SIGPIPE);
			wait(&status);
			loEncontro = WEXITSTATUS(status);
			if (loEncontro == 1){
				printf("Sommelier: Tenemos los ingredientes Chef.\n");
				exit(3);
			} else{
				printf("Sommelier: No tenemos los ingredientes Chef.\n");
				exit(2);
			}
		} else{
			printf("Somelier: ¿Has oido mozo? Necesitamos vino.\n");
			kill(pidMozo, SIGPIPE);
			wait(&status);
			loEncontro = WEXITSTATUS(status);
			if (loEncontro == 1){
				printf("Sommelier: Tenemos el vino Chef.\n");
				exit(3);
			} else{
				printf("Sommelier: No tenemos el vino Chef.\n");
				exit(1);
			}
		}
	}
}	

void manejadoraJefeSala(int s){
	printf("Jefe de Sala: Recibido Chef, me pongo a ello.\n");
	sleep(3);
	printf("Jefe de Sala: Se han montado todas las mesas.\n");
}

void manejadoraMozo(int s){
	srand(getpid());
	printf("Mozo (a lo lejos): Voy a ver que encuentro.\n");
	sleep(2);
	int loEncontre = calculaAleatorios(0,1);
	if(loEncontre == 1){
		printf("Mozo (a lo lejos): Aqui hay si.\n");
	} else{
		printf("Mozo (a lo lejos): Que va, no queda nada.\n");
	}
	exit(loEncontre);
}

void manejadoraPinche(int s){
	srand(getpid());
	printf("Pinche: Me pongo a preparar el plato.\n");
	sleep(calculaAleatorios(2,5));
	int sePreparo = calculaAleatorios(0,1);
	if(sePreparo == 1){
		printf("Pinche: Plato preparado.\n");
	} else{
		printf("Pinche: No se ha podido preparar el plato.\n");
	}
	exit(sePreparo);
}
