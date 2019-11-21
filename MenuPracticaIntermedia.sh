#!/bin/bash

if test -f PracticaIntermedia.c
then
	entrada=0
	salida=4
	while test $entrada -ne $salida
	do
		echo Mete una opcion
		echo 1 - Mostrar el codigo del programa
		echo 2 - Compilar el programa
		echo 3 - Ejecutarlo
		echo 4 - Salir
		read entrada
		case $entrada in
			1)	
				`cat ./PracticaIntermedia.c`
			;;
			2)
				`gcc PracticaIntermedia.c -o PracticaIntermedia`
			;;
			3)
				sol=`umask`
				if test -f PracticaIntermedia && umask >= 100
				then
					echo Introduce el numero de pinches
					read pinches
					`./PracticaIntermedia $pinches`
				else 
					echo Error: No existe el ejecutable o no se encuentra en el mismo directorio
				fi
			;;
			4)
				echo Saliendo del programa
			;;
			*)
				echo Introduce un valor entre 1 y 4
				echo Relanzando menu
		esac
	done
else
	echo Error: No se encuentra el codigo del programa
fi
