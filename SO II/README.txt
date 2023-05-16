PRÀCTICA SISTEMAS OPERATIVOS 2
-JORDI FLORIT ENSENYAT
-PAU GIRÓN RODRÍGUEZ
-JOSEP GABRIEL FORNÉS REYNÉS

Mejoras realizadas:
-mi_write/mi_read_
    En estas funciones hemos implementado la mejora de la caché de directorios que se basa en una array 
    del struct UltimaEntrada el qual contiene el camino de la entrada y su inodo, por lo tanto esta array 
    actua como una cache que guarda als ultimas entradas usadas y si queremos leer o escribir miramos si 
    ya lo tenemos guardado, y si es asi ya tenmos su informacion sin necesitar hacer un buscar_entrada(). 
    Hemos implementado que se base en una gestión FIFO, es decir cuando una entrada nueva que no estaba en 
    la cache se introduce, se elimina la mas antigua es decir movemos todas las entradas hacia la izquierda 
    (eliminado el mas aniguo y dejando espacio para la nueva entrada).

-mi_touch.c;
    En este programa hemos realizado la creación de ficheros, y en el mi_mkdir solo va a crear directorios. 
    Lo hemos realizado ya que hemos considerado oportino que haya un programa que cree los directorios y otro 
    los ficheros y no tenerlo junto. Para realizarlo, solo hemos tenido que mirar si en la última posición de 
    la ruta se encuentra una / o no.

-mi_escribir_varios.c:
    Hemos añadido este programa que nos lo ha dado la profesora para poder comprobar el nivel 9.