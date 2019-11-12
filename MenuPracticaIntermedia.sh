#!/bin/bash

function menu()
{
	repetir=1
	until test $repetir -eq 1
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
	until test $repetir -eq 0
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
	proc=0
	until test $proc -eq 4
	do
		proc=`menu`
		case $proc in
			valor1)	
				echo Se ha entrado en la opcion 1
			;;
			valor2)
				echo Se ha entrado en la opcion 2
			;;
			valor3)
				echo Se ha entrado en la opcion 3
			;;
		esac
	done
else
	echo No se encuentra el codigo del programa
fi
echo Saliendo del programa
