PRÀCTICA 2 SISTEMAS OPERATIVOS
-JORDI FLORIT ENSENYAT
-PAU GIRÓN RODRÍGUEZ
-JOSEP GABRIEL FORNÉS REYNÉS

Esta práctica consistirá en la implementación de un mini shell básico basado en el bash de Linux, 
y tendremos que superar 6 niveles para completar nuestra misión. 

El minishell implementara comandos internos y externos mediante la creación de procesos hijos encargados de su ejecucción y su 
debida gestión. Ademas permitira implementar procesos en segundo plano y la gestión de interrupciones.

Observaciones:
-cd_avanzado: Hemos implementado esta función que permite introduccir nombres de directorios entre comillas (puede ser
comillas dobles o simples), separados por espacio o bien con el simbolo '\ '.

    ejemplos:
        cd "directorio 1"
        cd 'directorio 2'
        cd directorio\3

-prompt: El prompt implementado en el nivel 1 simplemente es un ejemplo (mostramos el USER y el HOME) y no se puede modificar debido
a que no esta implementado el comando cd, en los niveles posteriores el prompt mostrado si es el real y se modifica el CWD correctamente
dependiendo del equipo y la ruta de directorios del usuario. 

-cabezera: No hemos implementado niguna cabezera para ninguno de los niveles.

-mensajes de debugging:
    -nivel 1: En este nivel adicionalmente imprimimos el numero total de tokens creados y un mensaje que nos indique si el comando
    introducido es externo o interno.

    -nivel 4/5: cuando mostramos el comando introducido que ejecuta el hijo, si se introducen comentarios se vera en el debugging aunque
    no se ejecute.

    -nivel 6/my_shell: tras esperar a que acabe un proceso en segundo plano no reaparece el prompt a no ser que se le introduzca un enter o otro comando.

-funcion init_jobslist: funcion implementada a partir del nivel4 para facilitar la inicializacion de la tabla o resetear los valores.

