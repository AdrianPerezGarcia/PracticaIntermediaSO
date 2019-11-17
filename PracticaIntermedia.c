#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int calculaAleatorios(int min, int max);
void manejadoraSomelier(int s);

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

	pidSomelier = fork();
	if(pidSomelier == 0){
		//Codigo Somelier
	}
	else{
		//Hay que crear por aqui al jefe de sala y ponerlo a dormir (Y a los pinches)
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
		int queFalta = waitpid(); // ? La idea es parar hasta que se muera el sommelier y recoger el 1,2 o 3 que devuelve
		switch(queFalta){
			case 1:
				//No hay vino, se acaba el programa, todos sus hijos estan muertos
				printf("Chef: No hay vino, así no hay quien trabaje.\n");
				return 0;
			break;
			case 2:
				//No hay algun ingrediente, no se acaba el programa pero hay que avisar
				printf("Chef: Falta algun ingrediente, pero vamos alla.\n");
			break;
		}
		//Hay que poner a trabajar a los pinches
		if(platosCreados == 0){
			printf("Chef: No hay platos, cerramos.\n");
			kill(pidJefeSala, SIGTERM);
			return 0;
		} else{
			kill(pidJefeSala, SIGUSR1);
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
	// El fork y que el hijo genere un aleatorio que devuelva al padre
	// El padre devuelve 1,2,3 depende de lo que le diga el hijo y se muere
}	

void manejadoraJefeSala(int s){
	printf("Jefe de Sala: He llegado a la manejadora.\n");
	sleep(3);
	printf("Jefe de Sala: Se han montado todas las mesas.\n"); 
	// Muere, no se si esto se hace aqui o en el main
}
