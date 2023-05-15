# prac_SO 
Mejoras realizadas:
-mi_write/mi_read_
    En estas funciones hemos implementado la mejora de la caché de directorios que se basa en una array del struct UltimaEntrada el qual contiene el camino de la entrada y su inodo, por lo tanto esta array actua como una cache que guarda als ultimas entradas usadas y si queremos leer o escribir miramos si ya lo tenemos guardado, y si es asi ya tenmos su informacion sin necesitar hacer un buscar_entrada(). Hemos implementado que se base en una gestión FIFO, es decir cuando una entrada nueva que no estaba en la cache se introduce, se elimina la mas antigua es decir movemos todas las entradas hacia la izquierda (eliminado el mas aniguo y dejando espacio para la nueva entrada).
