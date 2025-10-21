#include <iostream>
#include <fstream>
#include <string.h>
#include "defs.hpp"
#include <ctime>
//#include <ssp/string.h>

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
typedef sAerop tvrAerop [57]; 
//typedef struct sNodo* _____________;

void convertirNroVuelo(str9 idVue, str3 &aerSal, str3 &aerLlega, int &nroVue) {
    strncpy(aerSal, idVue, 3);
    aerSal[3] = '\0'; // terminar cadena

    // Copiar los últimos 3 caracteres (aeropuerto de llegada)
    strncpy(aerLlega, idVue + 6, 3);
    aerLlega[3] = '\0'; // terminar cadena

    // Extraer los 3 caracteres del medio y convertirlos a int
    char nro[4];
    strncpy(nro, idVue + 3, 3);
    nro[3] = '\0';
    nroVue = atoi(nro);
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
  return (1900 + timeinfo->tm_year) * 10000 + (1 + timeinfo->tm_mon) * 100 +
                                                          timeinfo->tm_mday;
} // GetDate

void Abrir() {
    //
}

int main() {

    
    str9 texto = "MDQ123ABC";
    str3 aerSal; str3 aerLlega; int nroVue;
    convertirNroVuelo(texto, aerSal, aerLlega, nroVue);

    cout << nroVue;

    return 0;
}