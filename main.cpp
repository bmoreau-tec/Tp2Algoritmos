#include <iostream>
#include <fstream>
#include <string.h>
#include <iostream>

#include <ctime>
typedef char str30[31];
typedef char str25[26];
typedef char str20[21];
typedef char str11[12];
typedef char str9[10];
typedef char str8[9];
typedef char str4[5];
typedef char str3[4];


using namespace std;

const int CANT_AEROP = 57;
struct sAerop {
    str3 codIATA;        // Código IATA del aeropuerto
    int ubicacion;       // Ubicación en el archivo original
    void* ptr;       // Campo NULL (puntero)
};

struct sVue {
    str9 idVue;
    short distKM;
    short velCru;
    short cantPas;
    str8 empresa;
    str11 marNave;
    int fecSale;
    short horaSale;
};

struct sAeropCompleto {  // Struct auxiliar para leer el archivo completo
    str20 provincia;
    str25 ciudad;
    str30 nomAerop;
    str4 codOACI;
    str3 codIATA;
};

struct tinfo {
    str3 nroVue;        // Número de vuelo
    int posArchivo;     // Posición en el archivo de vuelos
};

struct sNodo {
    tinfo info;
    sNodo* sig;
};

typedef sNodo* tLista;
typedef sAerop tvrAerop [CANT_AEROP]; 
//typedef struct sNodo* _____________;

// Funciones para obtener fecha y hora del sistema
long GetTime(int &hh, int &mm, int &ss) {
  time_t     rawtime;
  struct tm *timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  hh = timeinfo->tm_hour;
  mm = timeinfo->tm_min;
  ss = timeinfo->tm_sec;
  return timeinfo->tm_hour * 10000 + timeinfo->tm_min * 100 + timeinfo->tm_sec;
} 

long GetDate(int &year, int &mes, int &dia, int &ds) {
  time_t     rawtime;
  struct tm *timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  year = 1900 + timeinfo->tm_year;
  mes  = 1 + timeinfo->tm_mon;
  dia  = timeinfo->tm_mday;
  ds   = 1 + timeinfo->tm_wday;
  return (1900 + timeinfo->tm_year) * 10000 + (1 + timeinfo->tm_mon) * 100
          + timeinfo->tm_mday;
} 

// Función para replicar un carácter n veces
string replicate(char car, short n) {
  string cad = "";
  for (short i = 1; i <= n; i++)
    cad += car;
  return cad;
} 

char * SubCad(char cad[], short dsd, short hst) {
  static char sCad[4];
  short j;

  strcpy(sCad,"");
  for (short i = dsd, j = 0; i <= hst; i++,j++)
    sCad[j] = cad[i];
  sCad[j] = '\0';
  return sCad;
} // SubCad




void IntCmb(sAerop &elem1, sAerop &elem2) {
    sAerop aux = elem1;

    elem1 = elem2;
    elem2 = aux;
} // IntCmb

void OrdxBur(tvrAerop vrI, short Card) {
  bool  ordenado;
  short k = 0,
          i;

  do {
    ordenado = true;
    k++;
    for (i = 0; i < Card - k; i++)
      if (strcmp(vrI[i].codIATA,vrI[i+1].codIATA) > 0) {
          IntCmb(vrI[i],vrI[i+1]);
      ordenado = false;
    }
  } while (!ordenado);
} // OrdxBur

void convertirNroVuelo(str9 idVue, str3 &aerSal, str3 &aerLlega, str3 &nroVue) {
    strcpy(aerSal, SubCad(idVue, 0, 2));      // Posiciones 0-2: aeropuerto salida
    strcpy(aerLlega, SubCad(idVue, 6, 8));    // Posiciones 6-8: aeropuerto llegada  
    strcpy(nroVue, SubCad(idVue, 3, 5));      // Posiciones 3-5: número de vuelo
}



// MANEJO DE ARCHIVOS
void Abrir(FILE* &archivoAeropuertos, FILE* &archivoVuelos, FILE* &archivoConsultas) {
    archivoAeropuertos = fopen("aeropuertos.dat", "rb");
    archivoVuelos = fopen("vuelos.dat", "rb");
    archivoConsultas = fopen("consultas.dat", "rb");
}

void Cerrar(FILE* &archivoAeropuertos, FILE* &archivoVuelos, FILE* &archivoConsultas) {
    fclose(archivoAeropuertos);
    fclose(archivoVuelos);
    fclose(archivoConsultas);
}

void ProcAeropuertos(FILE* &archivoAeropuertos, tvrAerop vrAerop) {
    sAeropCompleto tempAerop;
    int contador = 0;
    
    // Leer cada registro completo del archivo
    while (fread(&tempAerop, sizeof(sAeropCompleto), 1, archivoAeropuertos) == 1 && contador < CANT_AEROP) {
        // Extraer solo codIATA, ubicación y NULL
        strcpy(vrAerop[contador].codIATA, tempAerop.codIATA);
        vrAerop[contador].ubicacion = contador;  // Posición en el archivo (0-based)
        vrAerop[contador].ptr = NULL;     // Campo NULL
        contador++;
    }
    
    // Ordenar por codIATA
    OrdxBur(vrAerop, contador);
}

void InsertarOrdenado(tLista &lista, str3 nroVue, int posArchivo) {
    sNodo* nuevo = new sNodo;
    strcpy(nuevo->info.nroVue, nroVue);
    nuevo->info.posArchivo = posArchivo;
    nuevo->sig = NULL;
    
    // Si la lista está vacía o el nuevo elemento va al principio
    if (lista == NULL || strcmp(nroVue, lista->info.nroVue) < 0) {
        nuevo->sig = lista;
        lista = nuevo;
        return;
    }
    
    // Buscar la posición correcta
    sNodo* actual = lista;
    while (actual->sig != NULL && strcmp(actual->sig->info.nroVue, nroVue) < 0) {
        actual = actual->sig;
    }
    
    nuevo->sig = actual->sig;
    actual->sig = nuevo;
}

int BuscarAeropuerto(tvrAerop vrAerop, int cantAerop, str3 codIATA) {
    int inicio = 0, fin = cantAerop - 1, medio;
    
    while (inicio <= fin) {
        medio = (inicio + fin) / 2;
        int comp = strcmp(vrAerop[medio].codIATA, codIATA);
        
        if (comp == 0) {
            return medio;  // Encontrado
        } else if (comp < 0) {
            inicio = medio + 1;
        } else {
            fin = medio - 1;
        }
    }
    return -1;  // No encontrado
}

void ProcVuelos(FILE* &archivoVuelos, tvrAerop vrAerop, int cantAerop) {
    sVue vuelo;
    int posicion = 0;
    str3 aerSal, aerLlega, nroVue;
    
    // Leer cada vuelo del archivo
    while (fread(&vuelo, sizeof(sVue), 1, archivoVuelos) == 1) {
        // Obtener codIATA del aeropuerto de origen del nroVuelo
        convertirNroVuelo(vuelo.idVue, aerSal, aerLlega, nroVue);
        
        // Localizar el aeropuerto de salida en vrAerop
        int posAerop = BuscarAeropuerto(vrAerop, cantAerop, aerSal);
        
        if (posAerop != -1) {
            // Convertir el ptr a lista y insertar el vuelo ordenado
            tLista* listaVuelos = (tLista*)&(vrAerop[posAerop].ptr);
            InsertarOrdenado(*listaVuelos, nroVue, posicion);
        }
        
        posicion++;
    }
}

void ConsultasVuelos() {
    //
}
void ListVueAeropSld() {
    //
}


void test(){
    str9 idVue = "ABC123DEF";
    str3 aerSal, aerLlega, nroVue;
    convertirNroVuelo(idVue, aerSal, aerLlega, nroVue);
    cout << "Aeropuerto de Salida: " << aerSal << endl;
    cout << "Aeropuerto de Llegada: " << aerLlega << endl;
    cout << "Número de Vuelo: " << nroVue << endl;
    cin >> idVue;

}

int main() {
    FILE* archivoAeropuertos;
    FILE* archivoVuelos;
    FILE* archivoConsultas;
    tvrAerop vrAerop;
    int cantAerop = 0;

    Abrir(archivoAeropuertos, archivoVuelos, archivoConsultas);
    
    // Procesar aeropuertos y obtener la cantidad real
    ProcAeropuertos(archivoAeropuertos, vrAerop);
    
    
    ProcVuelos(archivoVuelos, vrAerop, CANT_AEROP);

    

    ConsultasVuelos();
    ListVueAeropSld();
    
    Cerrar(archivoAeropuertos, archivoVuelos, archivoConsultas);
    return 0;
}