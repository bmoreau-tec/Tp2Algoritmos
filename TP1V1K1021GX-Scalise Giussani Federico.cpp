#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cstring>
#include <vector>
#include <ctime>

using namespace std;

// =============================================================================
// CONSTANTES GLOBALES
// =============================================================================
const int CANT_PROMOS = 14; // Cantidad total de promociones disponibles
const int MAX_ARTICULOS = 100; // Máximo de artículos que puede manejar el sistema
const int MAX_RUBROS = 15; // Máximo de rubros (categorías) que se pueden registrar
const int ANCHO_ARTICULOS = 104; // Ancho máximo (en caracteres) para la descripción de artículos

// =============================================================================
// DEFINICIONES DE TIPOS
// =============================================================================
typedef char str30[31];
typedef char str10[11];
typedef char str4[5];

// Definición de tipos para búsqueda genérica
typedef struct IndiceDescripcion * tbl; // 'tbl' será un puntero a un IndiceDescripcion
typedef
const char * tid; // 'tid' será un puntero a cadena constante (C-string)

// =============================================================================
// ESTRUCTURAS DE DATOS
// =============================================================================
struct Articulo {
  int codigo;
  short rubro;
  str30 descripcion;
  unsigned short stock;
  float precio;
  str10 unidad;
  short promociones[14];
};

struct Rubro {
  short codigo; // Código del rubro (2 caracteres + '\0')
  str30 descripcion; // Descripción del rubro (máx. 30 caracteres + '\0')
};

struct IndiceDescripcion {
  str30 descripcion; // Descripción para indexar (máx. 30 caracteres + '\0')
  int posArt; // Posición del artículo en el archivo (0-based)
  int estado; // Estado del artículo (1 = activo)
};

struct ItemCompra {
  str30 descripcion; // Descripción del artículo (máx. 30 caracteres + '\0')
  int cantidad; // Cantidad comprada
};

struct RubroPosicion {
  short codigoRubro; // Código numérico del rubro
  int posicionArticulo; // Posición del artículo dentro del archivo (número de registro o línea)
};

// =============================================================================
// VARIABLES GLOBALES
// =============================================================================
ifstream ArticulosFile; // Streams globales
ifstream RubrosFile;
ifstream IndDescripArtFile;
ifstream ListaComprasFile;

// =============================================================================
// FUNCIONES DE UTILIDAD GENERAL
// =============================================================================

// Función que obtiene la hora actual y la devuelve como número entero HHMMSS
long GetTime(int & hh, int & mm, int & ss) {
  time_t rawtime;
  struct tm * timeinfo;
  time( & rawtime);
  timeinfo = localtime( & rawtime);
  hh = timeinfo -> tm_hour;
  mm = timeinfo -> tm_min;
  ss = timeinfo -> tm_sec;
  return timeinfo -> tm_hour * 10000 + timeinfo -> tm_min * 100 + timeinfo -> tm_sec;
} // GetTime

// Función que obtiene la fecha actual y la devuelve como número entero AAAAMMDD
long GetDate(int & year, int & mes, int & dia, int & ds) {
  time_t rawtime;
  struct tm * timeinfo;
  time( & rawtime);
  timeinfo = localtime( & rawtime);
  year = 1900 + timeinfo -> tm_year;
  mes = 1 + timeinfo -> tm_mon;
  dia = timeinfo -> tm_mday;
  ds = 1 + timeinfo -> tm_wday;
  return (1900 + timeinfo -> tm_year) * 10000 + (1 + timeinfo -> tm_mon) * 100 +
    timeinfo -> tm_mday;
} // GetDate

// Devuelve n veces el carácter 'c'
char * Replicate(char c, int n) {
  static char buffer[256]; // Buffer estático local
  if(n >= 256) n = 255; // Limita a 255
  for(int i = 0; i < n; ++i) // Rellena
    buffer[i] = c;
  buffer[n] = '\0'; // Termina en '\0'
  return buffer; // Retorna puntero al buffer
}

// Devuelve día abreviado según número (1-7 = Dom-Sab)
void ObtenerNombreDia(int ds, str4 nombreDia) {
  static
  const str4 dias[] = { // 1..7 => Dom..Sab
    "Dom.",
    "Lun.",
    "Mar.",
    "Mie.",
    "Jue.",
    "Vie.",
    "Sab."
  };
  strcpy(nombreDia, dias[ds - 1]); // Índice base 0
}

// =============================================================================
// FUNCIONES DE LECTURA DE ARCHIVOS
// =============================================================================

bool LeerIndDescripArt(ifstream & Ind, IndiceDescripcion & rInd) { // Lee un índice
  Ind.get(rInd.descripcion, 31); // Lee descripción (máx 30 + '\0')
  Ind.ignore(); // Consumir separador 
  int numLinea; // Variable temporal para número de línea
  Ind >> numLinea; // Lee número de línea (1-based)
  rInd.posArt = numLinea - 1; // Convierte a posición 0-based
  Ind.ignore(); // Consumir separador
  Ind >> rInd.estado; // Lee estado (1=activo)
  Ind.ignore(); // Consumir separador de fin de campo
  return true; // Lectura OK
}

bool LeerArt(ifstream & Art, Articulo & rArt) { // Lee un artículo completo
  Art >> rArt.codigo >> rArt.rubro; // Lee código y rubro (enteros)
  Art.ignore(); // Consumir separador
  Art.get(rArt.descripcion, 31); // Lee descripción (30 + '\0')
  Art >> rArt.stock >> rArt.precio;
  Art.ignore(); // Consumir separador
  Art.get(rArt.unidad, 11); // Lee unidad (10 + '\0')
  for(int i = 0; i < CANT_PROMOS; ++i) { // Lee los 14 shorts de promos
    Art >> rArt.promociones[i];
  }
  Art.ignore(); // Consumir separador/fin de línea
  return true; // Lectura OK
}

bool LeerRubros(ifstream & Rub, Rubro & rRub) { // Lee un rubro
  Rub >> rRub.codigo; // Lee código como short
  Rub.ignore(); // Consumir separador
  Rub.get(rRub.descripcion, 31); // Lee descripción (30 + '\0')
  Rub.ignore(); // Consumir separador
  return true; // OK
}

bool LeerListaCompras(ifstream & List, ItemCompra & rItem) { // Lee item compra
  List.get(rItem.descripcion, 31); // Descripción (30 + '\0')
  List.ignore(); // Consumir separador
  List >> rItem.cantidad; // Cantidad pedida
  List.ignore(); // Consumir separador/fin de línea
  return true; // OK
}

// Lee un artículo por posición de registro (0-based) del archivo de texto
bool leerArticuloPorPosicion(int posicion, Articulo & art) {
  // Calcular offset: cada línea tiene ANCHO_ARTICULOS caracteres
  long int linea = posicion * ANCHO_ARTICULOS;
  ArticulosFile.seekg(linea, ios::beg); // Posiciona al inicio del registro
  LeerArt(ArticulosFile, art); // Lee el artículo
  return true; // OK
}

// =============================================================================
// FUNCIONES DE GESTIÓN DE ARCHIVOS
// =============================================================================

bool Abrir(ifstream & ArticulosFile, ifstream & RubrosFile, // Abre
  ifstream & IndDescripArtFile, ifstream & ListaComprasFile) { // archivos
  IndDescripArtFile.open("IndDescripArt.Txt"); // Índice por descripción
  ArticulosFile.open("Articulos.Txt"); // Tabla de artículos
  RubrosFile.open("Rubros.Txt"); // Tabla de rubros
  ListaComprasFile.open("ListaCompras.Txt"); // Lista de compras

  return true; // Todo abierto
}

int VolcarArchivos(ifstream & IndDescripArtFile, IndiceDescripcion indices[],
  int & cantIndices, ifstream & RubrosFile, Rubro rubros[],
  int & cantRubros, ifstream & ListaComprasFile,
  ItemCompra listaCompras[], int & cantidadCompras,
  RubroPosicion rubrosPos[], int & cantRubrosPos) {
  cantIndices = 0; // Reset contador
  while(cantIndices < MAX_ARTICULOS && // Carga índices hasta tope
    LeerIndDescripArt(IndDescripArtFile, indices[cantIndices])) {
    cantIndices++; // Acumula
  }

  cantRubros = 0; // Reset contador rubros
  while(cantRubros < MAX_RUBROS && // Carga rubros hasta tope
    LeerRubros(RubrosFile, rubros[cantRubros])) {
    cantRubros++; // Acumula
  }

  cantidadCompras = 0; // Reset contador compras
  while(cantidadCompras < MAX_ARTICULOS && // Carga lista compras
    LeerListaCompras(ListaComprasFile, listaCompras[cantidadCompras])) {
    cantidadCompras++; // Acumula
  }

  // Generar RubroPosicion leyendo Articulos.Txt (posición = línea/registro)
  ifstream artFile("Articulos.Txt"); // Reabrir artículos
  if(!artFile) {
    return 0; // No se pudo leer
  }

  cantRubrosPos = 0; // Reset contador
  int posicionLinea = 0; // Índice de registro
  Articulo tempArt; // Buffer temporal

  while(cantRubrosPos < MAX_ARTICULOS && // Lee cada artículo
    LeerArt(artFile, tempArt)) {
    rubrosPos[cantRubrosPos].codigoRubro = tempArt.rubro; // Rubro short a int
    rubrosPos[cantRubrosPos].posicionArticulo = posicionLinea; // N° registro
    cantRubrosPos++; // Siguiente
    posicionLinea++; // Avanza línea
  }
  artFile.close(); // Cerrar archivo

  // Ordenar rubrosPos por código de rubro (burbujeo simple)
  for(int i = 0; i < cantRubrosPos - 1; ++i) {
    for(int j = i + 1; j < cantRubrosPos; ++j) {
      if(rubrosPos[i].codigoRubro > rubrosPos[j].codigoRubro) {
        RubroPosicion temp = rubrosPos[i]; // Swap
        rubrosPos[i] = rubrosPos[j];
        rubrosPos[j] = temp;
      }
    }
  }

  return 1; // Éxito
}

// Actualiza sólo el stock en el archivo de texto usando seekp
void actualizarStockArticulo(int posicion, unsigned short nuevoStock) {
  fstream archivo("Articulos.Txt", ios::in | ios::out); // Abre R/W

  // Cálculo de offset: campos antes de STOCK suman 43 chars (según layout)
  long int posStock = posicion * ANCHO_ARTICULOS + 43; // Offset absoluto
  archivo.seekp(posStock, ios::beg); // Mueve puntero de escritura

  // Escribe stock con ancho 4, ceros a la izquierda (p.ej. 0042)
  archivo << right << setw(4) << setfill('0') << nuevoStock;

  archivo.close(); // Cierra
}

// =============================================================================
// FUNCIONES DE BÚSQUEDA Y PROCESAMIENTO
// =============================================================================

// Búsqueda binaria sobre vector de IndiceDescripcion por 'descripcion'
int BusBinVec(tbl tabla, tid clv, int ult) {
  int ini = 0, fin = ult; // Rango de búsqueda
  while(ini <= fin) { // Itera mientras haya rango
    int medio = (ini + fin) / 2; // Punto medio

    int cmp = strcmp(tabla[medio].descripcion, clv); // Comparación directa
    if(cmp == 0) {
      return medio; // Coincidencia exacta
    }
    else if(cmp < 0) {
      ini = medio + 1; // Buscar en mitad derecha
    }
    else {
      fin = medio - 1; // Buscar en mitad izquierda
    }
  }
  return -1; // No encontrado
}

void procesarCompra(ItemCompra compras[], int cantidadCompras,
  IndiceDescripcion indices[], int cantIndices) {
  for(int i = 0; i < cantidadCompras; ++i) { // Recorre pedidos
    int indicePos = BusBinVec(indices, compras[i].descripcion,
      cantIndices - 1); // Busca en índice

    if(indicePos == -1) { // No existe en índice
      compras[i].cantidad = 0; // Anula cantidad
      continue; // Siguiente
    }

    if(indices[indicePos].estado != 1) { // Inactivo
      compras[i].cantidad = 0; // Anula
      continue; // Siguiente
    }

    int posArt = indices[indicePos].posArt; // Pos en archivo (ya está 0-based)

    Articulo art; // Lee artículo real
    if(!leerArticuloPorPosicion(posArt, art)) {
      compras[i].cantidad = 0; // Error al leer
      continue; // Siguiente
    }

    if(art.stock >= compras[i].cantidad) { // Stock suficiente
      art.stock -= compras[i].cantidad; // Descuenta
    }
    else if(art.stock > 0) { // Parcial
      compras[i].cantidad = art.stock; // Vende lo que hay
      art.stock = 0; // Agota
    }
    else {
      compras[i].cantidad = 0; // Sin stock
    }

    actualizarStockArticulo(posArt, art.stock); // Persiste stock
  }
}

// =============================================================================
// FUNCIONES DE PROMOCIONES
// =============================================================================

// Nombre fijo por tipo de promoción
const char * GetNombrePromo(int tipoPromo) {
  switch(tipoPromo) {
    case 1:
      return "Promo       ";
    case 2:
      return "Marca       ";
    case 3:
      return "Jubilado    ";
    case 4:
      return "Comunidad   ";
    case 5:
      return "MercadoPago ";
    case 6:
      return "ANSES       ";
    case 7:
      return "SinPromo    ";
    default:
      return "Tipo desconocido";
  }
}

// =============================================================================
// FUNCIONES DE GENERACIÓN DE REPORTES
// =============================================================================

void EmitirArt_x_Rubro(RubroPosicion rubrosPos[], int cantRubrosPos,
  Rubro rubros[], int cantRubros) { // Reporte por rubro

  cout << "\n\n\n\n\n"; // Saltos antes del reporte

  cout << Replicate('-', 105) << "\n"; // Línea separadora
  cout << setw(80) // Centrar aprox. título
    <<
    "Listado de Articulos ordenados por Código de Rubro" << "\n";
  cout << Replicate('=', 105) << "\n\n"; // Línea doble

  int rubroActual = -1; // Para detectar cambios de rubro
  const char * descRubro = "DESCONOCIDO"; // Descripción por defecto

  for(int i = 0; i < cantRubrosPos; ++i) { // Recorre artículos ordenados
    if(rubrosPos[i].codigoRubro != rubroActual) { // Cambio de rubro
      rubroActual = rubrosPos[i].codigoRubro; // Actualiza rubro

      descRubro = "DESCONOCIDO"; // Busca descripción de rubro
      for(int r = 0; r < cantRubros; ++r) {
        if(rubros[r].codigo == rubroActual) { // Comparación directa short
          descRubro = rubros[r].descripcion;
          break;
        }
      }

      cout << "\nCod. Rubro: " << right << setw(2) << setfill(' ') <<
        rubroActual << " " << descRubro << endl; // Encabezado
      cout << "Cod.Art. Descripcion                    Stk.   Pre.Uni. "
      "U.Med.    TD % TD % TD % TD % TD % TD % TD %\n";
      cout << Replicate('-', 105) << "\n"; // Línea separadora
    }

    Articulo art; // Leer el artículo
    if(!leerArticuloPorPosicion(rubrosPos[i].posicionArticulo, art)) {
      continue; // Si falla, salta
    }

    cout << right << setw(8) << setfill('0') << art.codigo << " "; // Código 
    cout << left << setw(30) << setfill(' ') << art.descripcion << " "; // Desc
    cout << right << setw(5) << art.stock << "   "; // Stock 5 cols
    cout << right << setw(8) << fixed << setprecision(2) << art.precio << " ";
    cout << left << setw(7) << art.unidad << "  "; // Unidad 7 cols

    for(int j = 0; j < CANT_PROMOS; j += 2) { // Imprime tipo/porcentaje
      int tipo = art.promociones[j];
      int valor = art.promociones[j + 1];
      int valorPar = valor & ~1; // Fuerza par (borra bit 0)

      cout << tipo << " " // Tipo promo
        <<
        setw(2) << setfill('0') // % 2 dígitos con 0 izq
        <<
        valorPar << " ";
    }
    cout << endl; // Fin de línea
  }
}

// =============================================================================
// FUNCIONES DE GENERACIÓN DE TICKETS
// =============================================================================

void CabeceraTicket(int & ds) { // Imprime cabecera
  int year, mes, dia; // Fecha actual
  GetDate(year, mes, dia, ds); // También obtiene ds

  int hh, mm, ss; // Hora actual
  GetTime(hh, mm, ss);

  cout << "K O T T O\n"; // Datos fijos del local
  cout << "Yo te reconozco\n";
  cout << "SUC 170\n";
  cout << "XXXXXX...X 9999\n";
  cout << "XX....X\n";
  cout << "C.U.I.T. 99-99999999-9\n";

  str4 nombreDia; // Buffer para nombre del día
  ObtenerNombreDia(ds, nombreDia); // Obtiene nombre del día

  cout << "Fecha: " << nombreDia << " " // dd/mm/yyyy con día
    <<
    setw(2) << setfill('0') << dia << "/" <<
    setw(2) << setfill('0') << mes << "/" <<
    year << "\n";

  cout << "Hora: " // hh:mm:ss con ceros
    <<
    setw(2) << setfill('0') << hh << ":" <<
    setw(2) << setfill('0') << mm << ":" <<
    setw(2) << setfill('0') << ss << "\n";

  cout << "Nro. Ticket: 9999-99999999\n"; // Placeholders
  cout << "Nro. Caja: 9999\n";
  cout << Replicate('-', 40) << "\n"; // Separador
  cout << "F A C T U R A - B\n"; // Tipo comp.
  cout << "ORIGINAL\n";
  cout << Replicate('-', 40) << "\n"; // Separador
}

void PieTicket(float impTot, float impTotDesto, float impTotConDesto) {
  cout << "SubTot. sin descuentos....:  $" // Subtotal bruto
    <<
    fixed << setprecision(2) << impTot << "\n";
  cout << "Descuentos por promociones:  $" // Total descuentos
    <<
    fixed << setprecision(2) << impTotDesto << "\n";
  cout << Replicate('=', 40) << "\n"; // Separador
  cout << "T O T A L                    $" // Total neto
    <<
    fixed << setprecision(2) << (impTot - impTotDesto) << "\n";

  cout << Replicate('=', 40) << "\n"; // Separador
  cout << "Su pago con Tipo Pago:       $" // Monto abonado
    <<
    fixed << setprecision(2) << impTotConDesto << "\n";
  cout << "Su vuelto:                   $0.00\n"; // Vuelto (placeholder)
  cout << "G R A C I A S  P O R  S U  C O M P R A\n";
  cout << "Para consultas, sugerencias o reclamos\n";
  cout << "comunicarse al correo infoKotto.com.ar\n";
  cout << "****************************************\n";
}

void EmitirTicket(ItemCompra lista[], int cantItems, // Emite ticket
  IndiceDescripcion indices[], int cantIndices) {
  int year, mes, dia, ds; // Tomar fecha y día semana
  GetDate(year, mes, dia, ds);

  CabeceraTicket(ds); // Imprime cabecera

  float subtotalSinDescuentos = 0.0f; // Acumula bruto
  float totalDescuentos = 0.0f; // Acumula descuentos

  for(int i = 0; i < cantItems; ++i) { // Para cada ítem pedido
    if(lista[i].cantidad <= 0) continue; // Ignora cantidades <= 0

    int indicePos = BusBinVec(indices, lista[i].descripcion,
      cantIndices - 1); // Busca en índice
    if(indicePos == -1) continue; // No existe -> salta

    int posArt = indices[indicePos].posArt; // Pos en archivo (ya está 0-based)
    Articulo art; // Lee artículo
    if(!leerArticuloPorPosicion(posArt, art)) continue;

    float importeItem = art.precio * lista[i].cantidad; // Bruto item
    float descuento = 0.0f; // Desc. del item

    cout << lista[i].cantidad << " x " // Línea precio unitario
      <<
      fixed << setprecision(2) << art.precio << "\n";
    cout << art.descripcion << " " << right << art.unidad << "\n"; // Desc y UM
    cout << right << setw(8) << setfill('0') << art.codigo << " " <<
      setw(22) << setfill(' ') << "$" << fixed << setprecision(2) <<
      importeItem << "\n";

    int indiceBase = (ds - 1) * 2; // 0,2,4,...,12

    if(indiceBase >= 0 && indiceBase + 1 < CANT_PROMOS) {
      short tipoPromo = art.promociones[indiceBase]; // Tipo promo
      short porcPromo = art.promociones[indiceBase + 1]; // % promo

      if(porcPromo > 0 && tipoPromo != 7) { // Aplica si no es SinPromo
        float desc = (porcPromo / 100.0f) * importeItem; // Importe desc
        descuento += desc; // Acumula desc

        cout << GetNombrePromo(tipoPromo) // Muestra promo y %
          <<
          porcPromo << "%" << setw(17) <<
          "-$" << setprecision(2) << desc << "\n";
      }
    }

    cout << "\n"; // Línea en blanco

    subtotalSinDescuentos += importeItem; // Suma bruto
    totalDescuentos += descuento; // Suma descuento
  }

  PieTicket(subtotalSinDescuentos, totalDescuentos, // Imprime pie
    subtotalSinDescuentos - totalDescuentos);
  cout.flush(); // Vacía buffer salida
}

// =============================================================================
// FUNCIÓN PRINCIPAL
// =============================================================================

int main() { // Punto de entrada
  // Variables locales del main
  IndiceDescripcion indices[100]; // Índices por desc
  ItemCompra listaCompras[100]; // Pedidos
  Rubro rubros[MAX_RUBROS]; // Rubros
  RubroPosicion rubrosPos[MAX_ARTICULOS]; // (rubro,pos)

  int cantIndices = 0, cantRubros = 0; // Contadores
  int cantidadCompras = 0, cantRubrosPos = 0;

  Abrir(ArticulosFile, RubrosFile, IndDescripArtFile, ListaComprasFile);

  VolcarArchivos(IndDescripArtFile, indices, cantIndices, // Carga tablas
    RubrosFile, rubros, cantRubros,
    ListaComprasFile, listaCompras, cantidadCompras,
    rubrosPos, cantRubrosPos);

  procesarCompra(listaCompras, cantidadCompras, indices, cantIndices);

  freopen("Ticket.txt", "w", stdout); // Redirige stdout al archivo

  EmitirTicket(listaCompras, cantidadCompras, indices, cantIndices);
  EmitirArt_x_Rubro(rubrosPos, cantRubrosPos, rubros, cantRubros);

  // Restauración de salida estándar
  freopen("CON", "w", stdout); // Restaura stdout a consola

  return 0; // Fin OK
}