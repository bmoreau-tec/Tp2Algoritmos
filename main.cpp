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
    str20 provincia;
    str25 ciudad;
    str30 nomAerop;
    str4 codOACI;
    str3 codIATA;
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

struct tinfo;
struct sNodo;
struct sTblAerop;
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

  strcpy(sCad,"");
  for (short i = dsd, j = 0; i <= hst; i++,j++)
    sCad[j] = cad[i];
  sCad[hst + 1] = '\0';
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
    fread(vrAerop, sizeof(sAerop), CANT_AEROP, archivoAeropuertos);
    OrdxBur(vrAerop, CANT_AEROP); // EVIDENTEMENTE SAEROP ESTÁ MAL. TIENE QUE TENER OTROS CAMPOS
}

void ProcVuelos() {
    //
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
    test();
    FILE* archivoAeropuertos = fopen("aeropuertos.dat", "rb");
    FILE* archivoVuelos = fopen("vuelos.dat", "rb");
    FILE* archivoConsultas = fopen("consultas.dat", "rb");
    tvrAerop vrAerop;

    Abrir (archivoAeropuertos, archivoVuelos, archivoConsultas);
    ProcAeropuertos (archivoAeropuertos, vrAerop);
    ProcVuelos (  );
    ConsultasVuelos (  );
    ListVueAeropSld ( );
    Cerrar (archivoAeropuertos, archivoVuelos, archivoConsultas);
    return 0;
}