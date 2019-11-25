/* Inclusion de las librerias necesarias */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/* Definicion de las funciones */
int calculaAleatorios(int min, int max);
void manejadoraSommelier(int s);
void manejadoraJefeSala(int s);
void manejadoraMozo(int s);
void manejadoraPinche(int s);

/* Funcion principal */
int main(int argc, char const *argv[]){

	printf("||| COMIENZA EL RESTAURANTE. |||\n");

	/* Compruebo que se ha introducido un solo valor positivo como argumento */
	if (argc != 2){
		printf("Error: Parametros incorrectos.\n");
		printf("||| SE CIERRA EL RESTAURANTE. |||\n");
		return -1;
	} 
	else if(atoi(argv[1]) < 1){
		printf("Error: El número de pinches no puede ser menor que 1.\n");
		printf("||| SE CIERRA EL RESTAURANTE. |||\n");
		return -1;
	}
	printf("Se ha creado el chef con pid %d hijo de %d.\n" ,getpid(), getppid());
	int numPinches = atoi(argv[1]);
	srand(getpid());

	/* Se crean los pid_t y se reserva memoria para el puntero de pinches */
	pid_t pidSommelier, pidJefeSala;
	pid_t *pidPinches = (pid_t*)malloc(sizeof(pid_t)*numPinches);

	/* Se definen las sigaction para el Sommelier, Jefe de Sala y Pinches (La del mozo está en la manejadora del Sommelier) */
	/* Anotación: Se inicializan a cero ya que si no la SIGPIPE a veces se bloquea. */
	struct sigaction ssom={0}, sjef={0}, spin={0};

	pidSommelier = fork();
	if(pidSommelier == 0){
		/* Código del Sommelier */

		/* Se crea, se le asigna SIGUSR1 y SIGUSR2 a su manejadora y se pone en pause (Muere en su manejadora) */
		printf("Se ha creado el sommelier con pid %d hijo de %d.\n" ,getpid(), getppid());
		ssom.sa_handler = manejadoraSommelier;
		if(-1 == sigaction(SIGUSR1, &ssom, NULL)  || -1 == sigaction(SIGUSR2, &ssom, NULL)){
			perror("Jefe de Sala: sigaction");
			exit(-1);
		}
		pause();
	} 
	else if(pidSommelier == -1){
		perror("Error en la llamada fork");
	}else{
		pidJefeSala = fork();
		if(pidJefeSala == 0){
			/* Código del Jefe de Sala*/

			/* Se crea, se le asigna SIGUSR1 a su manejadora y se pone en pause, cuando se ejecute esta manejadora el proceso morira dentro */
			printf("Se ha creado el jefe de sala con pid %d hijo de %d.\n" ,getpid(), getppid());
			sjef.sa_handler = manejadoraJefeSala;
			if(-1 == sigaction(SIGUSR1, &sjef, NULL)){
				perror("Jefe de Sala: sigaction");
				exit(-1);
			}
			pause();
		}
		else if(pidJefeSala == -1){
			perror("Error en la llamada fork");
		} else{
			/* Código del Chef */

			/* Se crean los numPinches pinches almacenando su Pid en un puntero y poniendolos en pause esperando SIGUSR1 */
			int i, status;
			for(i = 0; i<numPinches; i++){
				*(pidPinches+i) = fork();
				if(*(pidPinches+i) == 0){
					printf("Se ha creado un pinche con pid %d hijo de %d.\n" ,getpid(), getppid());
					spin.sa_handler = manejadoraPinche;
					if(-1 == sigaction(SIGUSR1, &spin, NULL)){
						perror("Pinche: sigaction");
						exit(-1);
					}
					pause();
				}
			}

			sleep(3);

			/* Se genera un aleatorio para saber que le falta al Chef y se avisa al Sommelier con dos señales distintas */
			int ingredientes = calculaAleatorios(0,1);
			if(ingredientes == 0){
				printf("Chef: Me faltan ingredientes.\n");
				kill(pidSommelier, SIGUSR1);
			} else{
				printf("Chef: Me falta vino.\n");
				kill(pidSommelier, SIGUSR2);
			}
			
			/* Se recoge el valor devuelto por el Sommelier y según este procede o no la ejecución */
			wait(&status);
			printf("Chef: Recibido Sommelier.\n");
			switch(WEXITSTATUS(status)){
				case 1:
					//No hay vino, se acaba el programa matando antes al hijo de sala y a los numPinches pinches.
					printf("Chef: No hay vino, así no hay quien trabaje.\n");
					kill(pidJefeSala, SIGTERM);
					for( i=0; i<numPinches; i++){
						kill(*(pidPinches+i) , SIGTERM);
					}	
					printf("||| SE CIERRA EL RESTAURANTE. |||\n");
					return 0;
				break;
				case 2:
					//No hay algun ingrediente, no se acaba el programa pero hay que avisar
					printf("Chef: Falta algun ingrediente, pero vamos allá.\n");
				break;
				case 3:
					//No falta nada
					printf("Chef: Perfecto, vamos allá.\n");
				break;
			}
			/* Se pone a todos los pinches a trabajar de forma simultanea y se espera por todos */
			int platosCreados = 0;
			for( i=0; i<numPinches; i++){
				printf("Chef: Haz un plato pinche %d.\n", i+1);
				kill(*(pidPinches+i) , SIGUSR1);
			}
			for( i=0; i<numPinches; i++){
				int status;
				wait(&status);
				int seCocino = WEXITSTATUS(status);
				if(seCocino == 1){
					platosCreados++;
				}
			}

			/* Si no se ha completado ningun plato se cierra la aplicación (matando al Jefe De Sala), en caso contrario se le manda montar las mesas */
			if(platosCreados == 0){
				printf("Chef: No hay platos, cerramos.\n");
				kill(pidJefeSala, SIGTERM);
				printf("||| SE CIERRA EL RESTAURANTE. |||\n");
				return 0;
			} else{
				printf("Chef: Tenemos %d platos cocinados, monta las mesas Jefe de Sala.\n", platosCreados);
				int status;
				kill(pidJefeSala, SIGUSR1);
				wait(&status);
			}
			/* Ejecucion correcta, se acaba el programa */
			printf("||| PUEDE ABRIRSE EL RESTAURANTE. |||\n");
		}
	}
	return 0;
}

int calculaAleatorios(int min, int max){
	return rand() % (max-min+1) + min;
}


/* Manejadora del Sommelier */
void manejadoraSommelier(int s){

	printf("Somelier: Recibido Chef, ahora mismo te digo.\n");
	
	/* Se crea el proceso Mozo con un fork */
	pid_t pidMozo;
	pidMozo = fork();

	if (pidMozo == 0){
		/* Código del Mozo */

		/* Se crea, se enmascara SIGPIPE y se pone el proceso en pausa, el proceso muere en su manejadora */
		struct sigaction smoz = {0};
		smoz.sa_handler = manejadoraMozo;
		if(sigaction(SIGPIPE, &smoz, NULL) == -1){
			perror("Mozo: sigaction");
			exit(-1);
		}
		printf("Se ha creado el mozo con pid %d hijo de %d.\n" ,getpid(), getppid());
		pause();
	} 
	else if(pidMozo == -1){
		perror("Error en la llamada al fork Mozo");
	}else{
		/* Código del Sommelier */

		/* Se duerme un tiempo para que al Mozo le de tiempo a enmascarar la señal y se le envia la señal SIGPIPE */ 
		int status;
		sleep(2);
		printf("Somelier: ¿Has oido mozo? Busca a ver que hay.\n");
		kill(pidMozo, SIGPIPE);

		/* Se recoge el valor que deje el Mozo y segun la señal que recibio el Sommelier se sale con un valor u otro para que lo recoja el main */
		wait(&status);
		printf("Sommelier: Recibido Mozo.\n");
		if (WEXITSTATUS(status) == 1){
			if(s == SIGUSR1){
				printf("Sommelier: Tenemos todos los ingredientes Chef.\n");
			} else{
				printf("Sommelier: Hemos encontrado el vino Chef.\n");
			}
			exit(3);
		} else if(s == SIGUSR1){
			printf("Sommelier: No tenemos los ingredientes Chef.\n");
			exit(2);
		} else{
			printf("Sommelier: No tenemos el vino Chef.\n");
			exit(1);
		}
	}
}	

/* Manejadora del Jefe de Sala */
void manejadoraJefeSala(int s){
	
	printf("Jefe de Sala: Recibido Chef, me pongo a ello.\n");

	/* Se realiza el procedimiento especificado en el enunciado y se sale con un valor 0 para que el main lo recoja */
	sleep(3);
	printf("Jefe de Sala: Se han montado todas las mesas.\n");
	exit(0);
}

/* Manejadora del Mozo */
void manejadoraMozo(int s){
	
	printf("Mozo (a lo lejos): Recibido Sommelier, voy a ver que encuentro.\n");

	/* Si se encontro o no se imprime un mensaje y se sale del programa con ese valor para que lo recoja el Sommelier */
	/* Anotación: El mozo no sabe lo que tiene que buscar, solo busca algo y devuelve si lo encontro o no */
	if(calculaAleatorios(0,1) == 1){
		printf("Mozo (a lo lejos): Aqui hay si.\n");
		exit(1);
	} else{
		printf("Mozo (a lo lejos): Que va, no queda nada.\n");
		exit(0);
	}
}

/* Manejadora del Pinche */
void manejadoraPinche(int s){
	
	printf("Pinche: Recibido Chef, me pongo a preparar el plato.\n");

	/* Se cambia la semilla al pid del pinche para generar los aleatorios del tiempo de sueño y si se ha preparado o no */
	srand(getpid());
	sleep(calculaAleatorios(2,5));
	int sePreparo = calculaAleatorios(0,1);

	/* Si se preparo o no se imprime un mensaje y se sale del programa con ese valor para que lo recoja el main */
	if(sePreparo == 1){
		printf("Pinche: Plato preparado.\n");
	} else{
		printf("Pinche: No se ha podido preparar el plato.\n");
	}
	exit(sePreparo);
}
