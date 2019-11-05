#!/bin/bash

function menu()
{
	repetir=1
	until $repetir -eq 1
	do
		echo Seleccione una opcion:
		echo 1 - Mostrar el codigo del programa
		echo 2 - Compilar el programa
		echo 3 - Ejecutar el programa
		echo 4 - Salir del menu
		read opcion
		if opcion -lt 1 && opcion -gt 4
		then
			echo Opcion no valida, el valor debe ser un entero entre 1 y 4.
			echo Relanzando menu.
			echo -e
		else 
			repetir=0
		fi	
	done
	echo $opcion
}

function obtenerNumPinches()
{
	valido=0
	until $repetir -eq 0
	do
		echo Introduza el numero de pinches
		read opcion
		if opcion -lt 1
		then
			echo Opcion no valida, el valor debe ser un entero mayor que 0.
			echo Relanzando menu.
			echo -e
		else 
			repetir=0
		fi	
	done
	echo $opcion	
}


if test -f PracticaIntermedia.c
then
until $proc -eq 4
	do
		proc=`menu`
		case $proc in
			valor1)	
				`cat PracticaIntermedia.c`
			valor2)
				`gcc -o PracticaIntermedia ./PracticaIntermedia.c`
			valor3)
				if `test -f PracticaIntermedia`
				then
					numPinches=`obtenerNumPinches`
				else
					echo El archivo ejecutable no existe o no hay permisos de ejecucion
				fi
	done
else
	echo No se encuentra el codigo del programa
fi
echo Saliendo del programa
