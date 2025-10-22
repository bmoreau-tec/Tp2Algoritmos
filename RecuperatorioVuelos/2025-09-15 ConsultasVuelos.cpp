/*
  id.Pograma: 2025-09-15 ConsultasVuelos.cpp
  Autor.....: Lic. Hugo Cuello
  Fecha.....: 2025-septiembre
  Comentario: Consultas de nros. de Vuelos
                1) Volcar todos los datos del archivo de texto Aeropuertos a la
                  tabla vrAeropuertos[57], luego ordenarla por el campo codIATA.
                2) Generar desde archivo de texto Viajes la tabla
                    vrVuelos[MAX_VIAJES] el nro. de vuelo de 9 cars. y su posViaje
                    luego ordenarla por el campo nroVuelo.
                3) Recorrer el archivo de texto Consultas cuyo contenido de cada
                    linea es un nro. de viaje, hacer una busqueda binaria BB en la
                    tabla vrVuelos, encontrada la posicion tomar el campo posViaje
                    y hacer un Direccionamiento Directo (DD) en la tabla vrVuelos
                    ademas tomar los 3 primeros cars. del nroVuelo que indica un
                    codIATA, hacer BB en la tabla de vrAeropuertos, emitir los
                    datos requeridos Ciudad y nomAeropuerto, volvera tomar ahora
                    los 3 ultimos cas. del nroVuelo que indica un codIATA, hacer
                    BB en la tabla de vrAeropuertos, emitir los datos requeridos
                    Ciudad y nomAeropuerto, ademas emitir los datos requeridos
                    desde la tabla vrVuelos emitir Empresa y Marca.
                4) Listado de Salidas de c/u. de los Aeropuertos ordenado por
                    codIATA, a otros destinos. Utilizando la tecnica de Corte de
                    Control recorriendo la tabla vrIndVue por el campo nroVuelo.
                    Requiere leer datos en el archivo Vuelos.Txt (DD) para cada
                    Aeropuerto de destino y acceso a la tabla de vrAerop uno para
                    el AeropOrig (BB) y otros para c/u. de los AeropDest (BB).
  Compilador: Borland V.5.5
*/

#define record struct
#define AND &&
#define OR  ||
#include <fstream>
#include <iomanip>
using namespace std;

const CANT_AEROP       =    57,
      MAX_VUELOS       = 20000,
      TAM_LINEA_VUELOS =    60;

typedef char str3 [ 4],
             str4 [ 5],
             str8 [ 9],
             str9 [10],
             str11[12],
             str20[21],
             str25[26],
             str30[31];

record sAerop {
  str20 pcia;
  str25 ciudad;
  str30 nomAerop;
  str4  codOACI;
  str3  codIATA;
};

record sVue {
  str9  nroVue; // formato 3cAeropOrig3cdig3cAeropDest Ej NQN281AEP
  short distKm,
        velCruc,
        cantPasaj;
  str8  empresa;
  str11 marca;
  int   fechaSale;
  short horaSale;
};

record sIndVue {
  str9  nroVue;
  short posVue;
};

typedef sAerop  tvrAerop [CANT_AEROP];
typedef sIndVue tvrIndVue[MAX_VUELOS];

long GetTime(int &hh, int &mm, int &ss) {
  time_t     rawtime;
  struct tm *timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  hh = timeinfo->tm_hour;
  mm = timeinfo->tm_min;
  ss = timeinfo->tm_sec;
  return timeinfo->tm_hour * 10000 + timeinfo->tm_min * 100 + timeinfo->tm_sec;
} // GetTime

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
} // GetDate

string replicate(char car, short n) {
  string cad = "";

  for (short i = 1; i <= n; i++)
    cad += car;
  return cad;
} // replicate

char * SubCad(char cad[], short dsd, short hst) {
  static char sCad[4];

  strcpy(sCad,"");
  for (short i = dsd, j = 0; i <= hst; i++,j++)
    sCad[j] = cad[i];
  sCad[hst + 1] = '\0';
  return sCad;
} // SubCad

void Abrir(ifstream &Aerop, ifstream &Vue, ifstream &Cons) {
  Aerop.open("Aeropuertos.Txt");
  Vue.open("Vuelos.Txt");
  Cons.open("Consultas.Txt");
} // Abrir

bool LeerAerop(ifstream &Aerop, sAerop &rAerop) {
  Aerop.get(rAerop.pcia,21);
  Aerop.ignore();
  Aerop.get(rAerop.ciudad,26);
  Aerop.ignore();
  Aerop.get(rAerop.nomAerop,31);
  Aerop.ignore();
  Aerop.get(rAerop.codOACI,5);
  Aerop.ignore();
  Aerop.get(rAerop.codIATA,4);
  Aerop.ignore();
  return Aerop.good();
} // LeerAerop

bool LeerVuelos(ifstream &Vuelos, sVue &rVuelo) {
  Vuelos.get(rVuelo.nroVue,10);
  Vuelos >> rVuelo.distKm >> rVuelo.velCruc >> rVuelo.cantPasaj;
  Vuelos.ignore();
  Vuelos.get(rVuelo.empresa,9);
  Vuelos.ignore();
  Vuelos.get(rVuelo.marca,12);
  Vuelos >> rVuelo.fechaSale >> rVuelo.horaSale;
  Vuelos.ignore();
  return Vuelos.good();
} // LeerVuelos

bool LeerCons(ifstream &Cons, str9 nroVue) {
  Cons.get(nroVue,10);
  Cons.ignore();
  return Cons.good();
} // LeerCons

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

void InsertarEnOrden(tvrIndVue vE, sIndVue elem, short card) {
	while (card > 0 AND strcmp(elem.nroVue,vE[card - 1].nroVue) < 0) {
		vE[card] = vE[card - 1];
		card--;
	}
	vE[card] = elem;
} // InsertarEnOrden

int BusBinVec(tvrIndVue vCI, char clv[], short ult) {
  short pri = 0,
        med;

  ult -= 1;
  while (pri <= ult) {
    med = (pri + ult) / 2;
	if (strcmp(vCI[med].nroVue,clv) == 0)
	  return vCI[med].posVue;
  else
	  if (strcmp(vCI[med].nroVue,clv) < 0)
		  pri = med + 1;
    else
		  ult = med - 1;
  }
  return -1;
} // BusBinVec

int BusBinVec(tvrAerop vCI, char clv[], short ult) {
  short pri = 0,
        med;

  ult -= 1;
  while (pri <= ult) {
    med = (pri + ult) / 2;
	  if (strcmp(vCI[med].codIATA,clv) == 0)
	    return med;
    else
	    if (strcmp(vCI[med].codIATA,clv) < 0)
		    pri = med + 1;
      else
		    ult = med - 1;
  }
  return -1;
} // BusBinVec

int HoraLlega(short horaSale, short distKm, short velCruc, float &tiempo) {
  short horaMinLlega,
        minLlega,
        hora,
        mnt;

  tiempo = (float) distKm / velCruc;
  hora = tiempo; // asigna solo la parte entera.
  mnt = (tiempo - hora) * 60; // convierte parte decimal a minutos.
  tiempo = hora * 100 + mnt;
  horaMinLlega = horaSale + tiempo;
  minLlega = horaMinLlega - (horaMinLlega / 100) * 100;
  if (minLlega >= 60) {
     hora += 1;
     mnt -= 60;
  }
  return horaSale + hora * 100 + mnt;//retorna la hora de llegada: horaSale+tmpVue
} // HoraLlega

void FormatoHoraMin(int horaLlega, string &horaEstima, string &minEstima) {
  str30  num2cad;

  horaEstima = (string) itoa(horaLlega / 100,num2cad,10);
  minEstima = (string) itoa(horaLlega % 100,num2cad,10);
  if (horaEstima.size() == 1)
    horaEstima = '0' + horaEstima;
    if (minEstima.size() == 1)
      minEstima = '0' + minEstima;
} // FormatoHoraMin

void VerifEstado(int &horaAct, int &mnAct, int &horaLlega, sVue rVue,
                 float &tiempoVue, short &nroDia, int nDiaHoy, string &estado ) {
  int    seg;
  string horaEstima,
         minEstima;

  GetTime(horaAct,mnAct,seg);
  horaLlega = HoraLlega(rVue.horaSale,rVue.distKm,rVue.velCruc,tiempoVue);
  nroDia = rVue.fechaSale % 100;
  if (nroDia < nDiaHoy)
    estado = "Arribo";
  else
    if (nroDia > nDiaHoy)
      estado = "Programado";
    else
      if (rVue.horaSale > horaAct * 100 + mnAct)
        estado = "Programado";
      else
        if(horaLlega <= horaAct * 100 + mnAct)
          estado = "Arribo";
        else {
          FormatoHoraMin(horaLlega,horaEstima,minEstima);
          estado = "Estimado " + horaEstima + ':' + minEstima;
        }
  if (horaLlega >= 2400) { //OR rVue.fechaSale % 100 + 1 == nDiaHoy) {
    horaLlega = horaLlega - 2400;
    FormatoHoraMin(horaLlega,horaEstima,minEstima);
    estado = "Estimado " + horaEstima + ':' + minEstima;
  }
  if (horaLlega < horaAct * 100 + mnAct AND nroDia < nDiaHoy)
    estado = "Arribo";
  if (rVue.horaSale > horaAct * 100 + mnAct)
    estado = "Programado";
} // VerifEstado

void SepararHoraMin(short &horaSale, float tiempoVue, short &minSale,
                    short &minTmpVue, short &horaTmpVue,
                    int &minLlega, int &horaLlega) {
  minSale = horaSale % 100;
  horaSale = horaSale / 100;
  minTmpVue = (int) tiempoVue % 100;
  horaTmpVue = tiempoVue / 100;
  minLlega = horaLlega % 100;
  horaLlega = horaLlega / 100;
} // SepararHoraMin

void CortarCadenas(char nAerop[], char nomAerop[], char nCiu[], char ciudad[],
                   char nAeropD[], char nomAeropD[], char nCiuD[],
                   char nomCiudadD[]) {
  strncpy(nAerop,nomAerop,15);
  nAerop[15] = '\0';
  strncpy(nCiu,ciudad,15);
  nCiu[15] = '\0';
  strncpy(nAeropD,nomAeropD,15);
  nAeropD[15] = '\0';
  strncpy(nCiuD,nomCiudadD,15);
  nCiuD[15] = '\0';
} // CortarCadenas

void EmiteTitCab(short frmtTit) {
  cout << endl << replicate('=',140) << endl;
  if (frmtTit == 1)
    cout << "NroVue.   Ciu.Orig.       Nom.Aerop.Orig. ";
  else
    cout << "NroVue.   ";
  cout << "Empresa  Marca       ";
  cout << "Ciu.Dest.        ";
  cout << "Nom.Aerop.Dest.  Estado          dia hhAct hhSa  t.V.  hhLl" << endl;
  cout << replicate('=',140) << endl;
} // EmiteTitCab

void EmiteLinDet(sVue rVue,char nomAerop[], char ciudad[], char nomAeropD[],
                 char nomCiudadD[], int nDiaHoy) {
  short  horaSale,
         minSale,
         horaTmpVue,
         minTmpVue,
         nroDia;
  char   nAerop[15],
         nAeropD[15],
         nCiu[15],
         nCiuD[15];
  string estado;
  int    horaAct,
         mnAct,
         horaLlega,
         minLlega;
  float  tiempoVue;

  VerifEstado(horaAct,mnAct,horaLlega,rVue,tiempoVue,nroDia, nDiaHoy,estado);
  SepararHoraMin(rVue.horaSale,tiempoVue,minSale,minTmpVue,horaTmpVue,
                 minLlega,horaLlega);
  CortarCadenas(nAerop,nomAerop,nCiu,ciudad,nAeropD,nomAeropD,nCiuD,nomCiudadD);
  cout << setw(9) << rVue.nroVue << ' ' << left
       << (strlen(nCiu) == 0 ? setw(0) : setw(16)) << nCiu << ""
       << (strlen(nAerop) == 0 ? setw(0) : setw(16)) << nAerop << "" << left
       << setw(8) << rVue.empresa << ' ' << setw(11) << rVue.marca << ' '
       << setw(16) << nCiuD << ' ' << setw(16) << nAeropD << ' '
       << setw(16) << left << estado << setfill('0') << right
       << setw(2) << nroDia << "  " << setw(2) << horaAct << ':'
       << setw(2) << setfill('0') << mnAct << ' ' << setw(2)
       << rVue.horaSale << ':' << setw(2) << minSale << ' '
       << setw(2) << horaTmpVue << ':' << setw(2)
       << minTmpVue << ' ' << setw(2) << horaLlega << ':' << setw(2)
       << minLlega << ' ' << ' ' << setfill(' ') << endl;
} // EmiteLinDet

void VolcarAeropuertos(ifstream &Aerop, tvrAerop vrAerop) {
  short  i = 0;

  while (LeerAerop(Aerop,vrAerop[i])) // Vuelca todos los datos a la tabla.
    i++;
  OrdxBur(vrAerop,i); // Ordena por el campo codIATA.VolcarAeropuertosv
} // VolcarAeropuertos (1 punto)

void GenIndVuelos(ifstream &Vuelos, tvrIndVue vrIndVuelos, short &cantVue) {
  sVue rVuelo;

  while (LeerVuelos(Vuelos,rVuelo)) {
    strcpy(vrIndVuelos[cantVue].nroVue,rVuelo.nroVue);
    vrIndVuelos[cantVue].posVue = cantVue;
    InsertarEnOrden(vrIndVuelos,vrIndVuelos[cantVue],cantVue); // x nro.Vuelo.
    cantVue++;
  }
  Vuelos.clear();
} // GenIndVuelos (2 puntos)

void ConsultasVuelos(ifstream &Cons, ifstream &Vue, tvrIndVue vrIndVue,
                     tvrAerop vrAerop, short cantVue, short &diaHoy) {
  str9   nroVue;
  short  posAeropOrig,
         posAeropDest;
  sVue   rVue;
  str3   codAeropOrig,
         codAeropDest;
  int    year,mes,diaAct,ds;

  GetDate(year,mes,diaAct,ds);
  diaHoy = diaAct;
  freopen("Listado Vuelos.Txt","w",stdout);
  char * mesStr[] = {"","enero","febrero","marzo","abril","mayo","junio",
                 "julio","agosto","septiembre","octubre","noviembre","diciembre"};
  cout << replicate(' ',45) << "Consultas de Vuelos del " << diaAct
       << " de " << mesStr[mes] << " de " << year;
  EmiteTitCab(1);
  while (LeerCons(Cons,nroVue)) {
    Vue.seekg(BusBinVec(vrIndVue,nroVue,cantVue) * TAM_LINEA_VUELOS);
    LeerVuelos(Vue,rVue);
    strcpy(codAeropOrig,SubCad(nroVue,0,2));
    strcpy(codAeropDest,SubCad(nroVue,6,8));
    posAeropOrig = BusBinVec(vrAerop,codAeropOrig,CANT_AEROP);
    posAeropDest = BusBinVec(vrAerop,codAeropDest,CANT_AEROP);
    EmiteLinDet(rVue,vrAerop[posAeropOrig].nomAerop,
                vrAerop[posAeropOrig].ciudad,vrAerop[posAeropDest].nomAerop,
                vrAerop[posAeropDest].ciudad,diaAct);
  }
  cout << replicate('-',140) << endl;
} // ConsultasVuelos

void ListVueAeropSld(ifstream &Vue, tvrIndVue vrIndVue, tvrAerop vrAerop,
                     short cantVue, int diaAct) {
  short i = 0,
        posAeropOrig,
        posAeropDest;
  str3  cAeropOrigAnt,
        codAeropDest;
  sVue  rVue;

  cout << replicate(' ',21)
       << "Listado Salidas Aerop. Origen a otros Aerop. Llegada del dia "
       << diaAct << endl;
  while (i < cantVue) {
    strcpy(cAeropOrigAnt,SubCad(vrIndVue[i].nroVue,0,2));
    posAeropOrig = BusBinVec(vrAerop,cAeropOrigAnt,CANT_AEROP);
    cout << "Aerop. origen : " << cAeropOrigAnt << ' '
         << vrAerop[posAeropOrig].nomAerop << ' '
         << "Ciudad: " << vrAerop[posAeropOrig].ciudad;
    EmiteTitCab(2);
    while (i < cantVue AND
           strcmp(SubCad(vrIndVue[i].nroVue,0,2),cAeropOrigAnt) == 0) {
      Vue.seekg(vrIndVue[i].posVue * TAM_LINEA_VUELOS);
      LeerVuelos(Vue,rVue);
      strcpy(codAeropDest,SubCad(vrIndVue[i++].nroVue,6,8));
      posAeropDest = BusBinVec(vrAerop,codAeropDest,CANT_AEROP);
      EmiteLinDet(rVue,"","",vrAerop[posAeropDest].nomAerop,
                  vrAerop[posAeropDest].ciudad,diaAct);
    }
    cout << replicate('-',140) << endl;
  }
  freopen("CON","w",stdout);
} // ListadoVuelosAeropSalida

void Cerrar(ifstream &Aerop, ifstream &Vue, ifstream &Cons) {
  Aerop.close();
  Vue.close();
  Cons.close();
} // Cerrar

main() {
  ifstream  Aeropuertos,
            Vuelos,
            Consultas;
  tvrAerop  vrAeropuertos;
  tvrIndVue vrIndVuelos;
  short     diaHoy,
            cantVuelos = 0;

  Abrir(Aeropuertos,Vuelos,Consultas);
  VolcarAeropuertos(Aeropuertos,vrAeropuertos);
  GenIndVuelos(Vuelos,vrIndVuelos,cantVuelos);
  ConsultasVuelos(Consultas,Vuelos,vrIndVuelos,vrAeropuertos,cantVuelos,diaHoy);
  ListVueAeropSld(Vuelos,vrIndVuelos,vrAeropuertos,cantVuelos,diaHoy);
  Cerrar(Aeropuertos,Vuelos,Consultas);
  return 0;
}