#include <iostream>
#include <cstdio>   // para FILE, fread, fopen
#include <cstring>
#include "defs.hpp"

using namespace std;

typedef char str20[21];
typedef char str25[26];
typedef char str30[31];
typedef char str4[5];
typedef char str3[4];
typedef char str9[10];
typedef char str5[6];


struct sAerop {
    str20 provincia;
    str25 ciudad;
    str30 nomAerop;
    str4 codOACI;
    str3 codIATA;
};

struct sVuelo {
    str9 idVue;
    short distKM;
    short velCru;
    short cantPas;
    str8 empresa;
    str11 marNave;
    int fecSale;
    short horaSale;
};

int main() {
    FILE* bin = fopen("aeropuertos.dat", "rb");  // abrir en modo lectura binaria
    if (!bin) {
        cerr << "No se pudo abrir Aerop.dat\n";
        return 1;
    }

    sAerop a;

    cout << "=== LISTA DE AEROPUERTOS ===\n\n";

    // Leer mientras haya registros
    while (fread(&a, sizeof(sAerop), 1, bin) == 1) {
        cout << "Provincia: " << a.provincia << endl;
        cout << "Ciudad:    " << a.ciudad << endl;
        cout << "Aerop.:    " << a.nomAerop << endl;
        cout << "OACI:      " << a.codOACI << endl;
        cout << "IATA:      " << a.codIATA << endl;
        cout << "-----------------------------\n";
        break;
    }

    fclose(bin);



    //read the vuelos.dat file

    FILE* binVue = fopen("vuelos.dat", "rb");  // abrir en modo lectura binaria
    if (!binVue) {
        cerr << "No se pudo abrir vuelos.dat\n";
        return 1;
    }

    sVuelo v;
    cout << "=== LISTA DE VUELOS ===\n\n";
    // Leer mientras haya registros
    while (fread(&v, sizeof(sVuelo), 1, binVue)
              == 1) {
          cout << "ID Vuelo:      " << v.idVue << endl;
          cout << "Distancia KM:  " << v.distKM << endl;
          cout << "Velocidad Cru: " << v.velCru << endl;
          cout << "Cant. Pasajeros: " << v.cantPas << endl;
          cout << "Empresa:       " << v.empresa << endl;
          cout << "Marca Nave:    " << v.marNave << endl;
          cout << "Fecha Salida:  " << v.fecSale << endl;
          cout << "Hora Salida:   " << v.horaSale << endl;
          cout << "-----------------------------\n";
          break;
     }
        
    fclose(binVue);

    //read the consultas.dat file
    FILE* binCons = fopen("consultas.dat", "rb");  // abrir en modo lectura binaria
    if (!binCons) {
        cerr << "No se pudo abrir consultas.dat\n";
        return 1;
    }
    str9 idVue;
    cout << "=== LISTA DE CONSULTAS ===\n\n";
    // Leer mientras haya registros
    while (fread(&idVue, sizeof(str9), 1, binCons)
              == 1) {
          cout << "ID Vuelo Consulta: " << idVue << endl;
          cout << "-----------------------------\n";
          break;
     }
    fclose(binCons);
    

    return 0;
}
