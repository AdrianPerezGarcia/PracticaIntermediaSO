#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int calculaAleatorios(int min, int max);
void manejadoraSomelier(int s);
void manejadoraJefeSala(int s);
void manejadoraMozo(int s);

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
	srand(time(NULL));
	int numPinches = atoi(argv[1]);

	pid_t pidSomelier, pidJefeSala;
	pid_t *pidPinches = (pid_t*)malloc(sizeof(pid_t)*numPinches);

	struct sigaction ssom, sjef;

	pidJefeSala = fork();
	if(pidSomelier == 0){
		sjef.sa_handler = manejadoraJefeSala;
		if(-1 == sigaction(SIGUSR1, &ssom, NULL)){
			perror("Jefe de Sala: sigaction");
			return 1;
		}
	}
	else{
		pidSomelier = fork();
		if(pidSomelier == 0){
			ssom.sa_handler = manejadoraSomelier;
			int sigaction(SIGUSR1, &ssom, NULL);
			int sigaction(SIGUSR2, &ssom, NULL);
			pause();
		}
		else{
			//Hay que crear a los pinches)
		/*
			Codigo pinches
			int i;
			for(i = 0; i<numPinches; i++){
				*(numPinches+i) = fork();
				if(*(numPinches+i) == 0){
					break;
				}
			}
		*/
		//Codigo chef
		sleep(3);
		int ingredientes = calculaAleatorios(0,1);
		if(ingredientes == 0){
			// Faltan ingredientes
			printf("Chef: Me faltan ingredientes.\n");
			kill(pidSomelier, SIGUSR1);
		} else{
			// Falta el vino
			printf("Chef: Me falta vino.\n");
			kill(pidSomelier, SIGUSR2);
		}
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
		//Hay que poner a trabajar a los pinches y cada plato bueno 
		if(platosCreados == 0){
			printf("Chef: No hay platos, cerramos.\n");
			kill(pidJefeSala, SIGTERM);
			return 0;
		} else{
			kill(pidJefeSala, SIGUSR1);
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
	printf("Somelier: He llegado a la manejadora.\n");
	pid_t pidMozo = fork();
	if (pidMozo == 0){
		/* code */
	} else{
		if(s == SIGUSR1){
			printf("Somelier: ¿Has oido mozo? Necesitamos ingredientes.\n");
		}
		else{
			printf("Somelier: ¿Has oido mozo? Necesitamos vino.\n");
		}
	}
	// El fork y que el hijo genere un aleatorio que devuelva al padre
	// El padre devuelve 1,2,3 depende de lo que le diga el hijo y se muere
}	

void manejadoraJefeSala(int s){
	sleep(3);
	printf("Jefe de Sala: Se han montado todas las mesas.\n");
}

void manejadoraMozo(int s){
	printf("Mozo: Voy a ver que encuentro.\n");
	sleep(2);
	int queHeEncontrado = calculaAleatorios(0,1);
	if(queHeEncontrado == 1){
		printf("Mozo (a lo lejos): Aqui hay si.\n");
	} else{
		printf("Mozo (a lo lejos): Que va, no queda nada.\n");
	}
	exit(queHeEncontrado);
}

void manejadoraPinche(int s){
	
}