#include <iostream>
#include <fstream>
#include <string.h>
#include "defs.hpp"
#include <ctime>

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


//struct tinfo;
struct sNodo;
struct sTblAerop;


bool leerAeropuertos(ifstream & aeropFile, sAerop & rAerop) { // Lee un aeropuerto
    aeropFile.get(rAerop.provincia, 21); // Lee provincia (máx 20 + '\0')
    aeropFile.ignore(); // Consumir separador
    aeropFile.get(rAerop.ciudad, 26); // Lee ciudad (máx 25 + '\0')
    aeropFile.ignore(); // Consumir separador
    aeropFile.get(rAerop.nomAerop, 31); // Lee nombre aeropuerto (máx 30 + '\0')
    aeropFile.ignore(); // Consumir separador
    aeropFile.get(rAerop.codOACI, 5); // Lee código OACI (4 + '\0')
    aeropFile.ignore(); // Consumir separador
    aeropFile.get(rAerop.codIATA, 4); // Lee código IATA (3 + '\0')
    aeropFile.ignore(); // Consumir separador/fin de línea
    return aeropFile.good();
}


bool leerVuelos(ifstream & vueFile, sVue & rVue) { // Lee un vuelo
    vueFile.get(rVue.idVue, 10); // Lee ID vuelo (9 + '\0')
    vueFile.ignore(); // Consumir separador
    vueFile >> rVue.distKM; // Lee distancia en KM
    vueFile.ignore(); // Consumir separador
    vueFile >> rVue.velCru; // Lee velocidad crucero
    vueFile.ignore(); // Consumir separador
    vueFile >> rVue.cantPas; // Lee cantidad de pasajeros
    vueFile.ignore(); // Consumir separador
    vueFile.get(rVue.empresa, 9); // Lee empresa (máx 8 + '\0')
    vueFile.ignore(); // Consumir separador
    vueFile.get(rVue.marNave, 12); // Lee marca nave (máx 11 + '\0')
    vueFile.ignore(); // Consumir separador
    vueFile >> rVue.fecSale; // Lee fecha salida
    vueFile.ignore(); // Consumir separador
    vueFile >> rVue.horaSale; // Lee hora salida
    vueFile.ignore(); // Consumir fin de línea
    return vueFile.good();
}

bool leerConsultas(ifstream & consFile, str9 & idVue) { // Lee id de consulta (idVuelo)
    consFile.get(idVue, 10); // Lee ID vuelo (máx 9 + '\0')
    consFile.ignore(); // Consumir separador/fin de línea
    return consFile.good();
}

int generateAerop() {
    ifstream aeropFile("aeropuertos.txt");
    FILE* binAerop = fopen("aeropuertos.dat", "wb");
    sAerop rAerop;

    while (leerAeropuertos(aeropFile, rAerop)) {
        fwrite(&rAerop, sizeof(sAerop), 1, binAerop);
    }
}

int generateVuelos() {
    ifstream vueFile("vuelos.txt");
    FILE* binVue = fopen("vuelos.dat", "wb");
    sVue rVue;

    while (leerVuelos(vueFile, rVue)) {
        fwrite(&rVue, sizeof(sVue), 1, binVue);
    }
}

int generateConsultas() {
    ifstream consFile("Consultas.Txt");
    FILE* binCons = fopen("consultas.dat", "wb");
    str9 idVue;

    while (leerConsultas(consFile, idVue)) {

        fwrite(&idVue, sizeof(str9), 1, binCons);
    }
}


int main() {
    generateAerop();
    generateVuelos();
    generateConsultas();
    return 0;
}