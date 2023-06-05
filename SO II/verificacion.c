/*JOSEP GABRIEL FORNÉS REYNÉS, JORDI FLORIT ENSENYAT, PAU GIRÓN RODRÍGUEZ*/
#include "verificacion.h"

int main(int argc, char **argv){

  struct STAT stat;
  int numeroEntradas = 0;
  int error = 0;
  int numeroBytes = 0;
  int nEntrada = 0;

  if (argc != 3){
    fprintf(stderr, ROJO_T "Sintaxis: ./verificacion <nombre_dispositivo> <directorio_simulación>\n" RESET);
    exit(FALLO);
  }

  if (bmount(argv[1]) == FALLO){
    fprintf(stderr, ROJO_T "Error montar dispositivo\n" RESET);
    return FALLO;
  }

  mi_stat(argv[2], &stat);
  fprintf(stderr, "Directorio de simulación: %s\n",argv[2]);

  numeroEntradas = (stat.tamEnBytesLog / sizeof(struct entrada)); // obtenemos numero de entradas
  if (numeroEntradas != NUMPROCESOS){
    fprintf(stderr, "Numero entradas: %i ,numProcesos: %d", numeroEntradas, NUMPROCESOS);
    fprintf(stderr, ROJO_T "Numero de entradas distinto a numero de procesos\n" RESET);
    return FALLO;
  }

  

  char direccion_fichero[100];
  sprintf(direccion_fichero, "%s%s", argv[2], "informe.txt");

  if (mi_creat(direccion_fichero, 7) < 0){
    bumount();
    exit(0);
  }

  struct entrada entradas[numeroEntradas];
  error = mi_read(argv[2], entradas, 0, sizeof(entradas));
  if(error < 0){
    mostrar_error_buscar_entrada(error);
    return FALLO;
  }

  fprintf(stderr,"Numero de entradas: %i\n", numeroEntradas);

  for(nEntrada = 0; nEntrada < numeroEntradas; nEntrada++){

    //leer la entrada y extrear el pid correspondiente
    pid_t pid = atoi(strchr(entradas[nEntrada].nombre, '_') + 1);
    struct INFORMACION informacion;
    informacion.pid = pid;
    informacion.nEscrituras = 0;

    char ficheroPrueba[128];
    sprintf(ficheroPrueba, "%s%s/%s", argv[2],entradas[nEntrada].nombre,"prueba.dat");

    //buffer de registros de escrituras
    int cantidad = 256*24; //multiple de blocksize
    struct REGISTRO bEscrituras[cantidad];
    memset(bEscrituras, 0, sizeof(bEscrituras));

    int offset = 0;

    //mirar si quedan escrituras en prueba.dat
    while(mi_read(ficheroPrueba, bEscrituras, offset, sizeof(bEscrituras)) > 0){

      int numeroRegistro = 0;

      while(numeroRegistro < cantidad){

        if(bEscrituras[numeroRegistro].pid == informacion.pid){

          if(!informacion.nEscrituras){
            informacion.MenorPosicion = bEscrituras[numeroRegistro];
            informacion.MayorPosicion = bEscrituras[numeroRegistro];
            informacion.PrimeraEscritura = bEscrituras[numeroRegistro];
            informacion.UltimaEscritura = bEscrituras[numeroRegistro];
            informacion.nEscrituras++;
            //fprintf(stderr, "nEscrituras: %i\n",informacion.nEscrituras);
          }else{

            if ((difftime(bEscrituras[numeroRegistro].fecha, informacion.PrimeraEscritura.fecha)) <= 0 &&
                bEscrituras[numeroRegistro].nEscritura < informacion.PrimeraEscritura.nEscritura)
            {
              informacion.PrimeraEscritura = bEscrituras[numeroRegistro];
            }

            if ((difftime(bEscrituras[numeroRegistro].fecha, informacion.UltimaEscritura.fecha)) >= 0 &&
                bEscrituras[numeroRegistro].nEscritura > informacion.UltimaEscritura.nEscritura)
            {
              informacion.UltimaEscritura = bEscrituras[numeroRegistro];
            }

            if (bEscrituras[numeroRegistro].nRegistro < informacion.MenorPosicion.nRegistro)
            {
              informacion.MenorPosicion = bEscrituras[numeroRegistro];
            }

            if (bEscrituras[numeroRegistro].nRegistro > informacion.MayorPosicion.nRegistro)
            {

              informacion.MayorPosicion = bEscrituras[numeroRegistro];
            }

            
            informacion.nEscrituras++;
            //fprintf(stderr, "nEscrituras: %i\n",informacion.nEscrituras);

          }

        }
        numeroRegistro++;
      }
      memset(&bEscrituras, 0, sizeof(bEscrituras));
      offset += sizeof(bEscrituras);
    }

    fprintf(stderr, "[%i) %i escrituras validadas en %s]\n", nEntrada + 1, informacion.nEscrituras, ficheroPrueba);

    //Añadir la información del struct info al fichero informe.txt por el final
    char tiempoPrimero[100];
    char tiempoUltimo[100];
    char tiempoMenor[100];
    char tiempoMayor[100];
    struct tm *tm;

    tm = localtime(&informacion.PrimeraEscritura.fecha);
    strftime(tiempoPrimero, sizeof(tiempoPrimero), "%a %Y-%m-%d %H:%M:%S", tm);
    tm = localtime(&informacion.UltimaEscritura.fecha);
    strftime(tiempoUltimo, sizeof(tiempoUltimo), "%a %Y-%m-%d %H:%M:%S", tm);
    tm = localtime(&informacion.MenorPosicion.fecha);
    strftime(tiempoMenor, sizeof(tiempoMenor), "%a %Y-%m-%d %H:%M:%S", tm);
    tm = localtime(&informacion.MayorPosicion.fecha);
    strftime(tiempoMayor, sizeof(tiempoMayor), "%a %Y-%m-%d %H:%M:%S", tm);

    char buffer[BLOCKSIZE];
    memset(buffer, 0, BLOCKSIZE);

    sprintf(buffer, "PID: %i\nNumero de escrituras: %i\n", pid, informacion.nEscrituras);
    sprintf(buffer + strlen(buffer), "%s %i %i %s",
            "Primera escritura",
            informacion.PrimeraEscritura.nEscritura,
            informacion.PrimeraEscritura.nRegistro,
            asctime(localtime(&informacion.PrimeraEscritura.fecha)));

    sprintf(buffer + strlen(buffer), "%s %i %i %s",
            "Ultima escritura",
            informacion.UltimaEscritura.nEscritura,
            informacion.UltimaEscritura.nRegistro,
            asctime(localtime(&informacion.UltimaEscritura.fecha)));

    sprintf(buffer + strlen(buffer), "%s %i %i %s",
            "Menor posicion",
            informacion.MenorPosicion.nEscritura,
            informacion.MenorPosicion.nRegistro,
            asctime(localtime(&informacion.MenorPosicion.fecha)));

    sprintf(buffer + strlen(buffer), "%s %i %i %s",
            "Mayor posicion",
            informacion.MayorPosicion.nEscritura,
            informacion.MayorPosicion.nRegistro,
            asctime(localtime(&informacion.MayorPosicion.fecha)));

    sprintf(buffer,
            "PID: %d\nNumero de escrituras:\t%d\n"
            "Primera escritura:\t%d\t%d\t%s\n"
            "Ultima escritura:\t%d\t%d\t%s\n"
            "Menor posición:\t\t%d\t%d\t%s\n"
            "Mayor posición:\t\t%d\t%d\t%s\n\n",
            informacion.pid, informacion.nEscrituras,
            informacion.PrimeraEscritura.nEscritura,
            informacion.PrimeraEscritura.nRegistro,
            tiempoPrimero,
            informacion.UltimaEscritura.nEscritura,
            informacion.UltimaEscritura.nRegistro,
            tiempoUltimo,
            informacion.MenorPosicion.nEscritura,
            informacion.MenorPosicion.nRegistro,
            tiempoMenor,
            informacion.MayorPosicion.nEscritura,
            informacion.MayorPosicion.nRegistro,
            tiempoMayor);
    // Escribimos en prueba.dat y actualizamos offset
    if ((numeroBytes += mi_write(direccion_fichero, &buffer, numeroBytes, strlen(buffer))) < 0){
      printf("verifiacion.c: Error al escribir el fichero: '%s'\n", direccion_fichero);
      bumount();
      return FALLO;
    }
  }
  bumount();

}

