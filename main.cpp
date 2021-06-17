/*
 * Este programa buscara la ruta optima de vuelo de un dron fumigador.
 * A partir de una imagen satelital dada en bits, detectaremos donde hay manchas importantes de malezas.
 * Las recorreremos evitando las "barreras" que puedan llegar a surgir en el campo, y tratando de que el camino sea lo
 * mas corto posible para ahorrar gastos.
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include "bitset"
#include "queue"
#include "vector"
#include "Mancha.h"
#include "math.h"
#include "Hamilton.h"

#define INFI 9999
#define FILAS 100
#define COLUMNAS 100
using namespace std;
struct Barrera{
    Posicion p1;
    Posicion p2;
};

//Variables Globales:
bitset<2> matrizDeBits[FILAS][COLUMNAS];//Matriz que guarda el arreglo de bit<b1, b0> b1 indica si hay barrera || b0:indica si hay maleza
int matrizPesos[CANTIDADVERTICES][CANTIDADVERTICES];
int matrizAdyacencia[CANTIDADVERTICES][CANTIDADVERTICES];
vector<Barrera*> vectorBarreras;
vector<Mancha*> arrayManchas;

//Prototipo de funciones:
bool doIntersect(Posicion p1, Posicion q1, Posicion p2, Posicion q2);       //Funciones para controlar intersecciones
int orientation(Posicion p, Posicion q, Posicion r);
bool onSegment(Posicion p, Posicion q, Posicion r);

void llenarMatrizDeBits();                                                      //Funciones para completar matrices
void completarMatrizAdyacencia();
int calcularDistancia(Posicion, Posicion);

void filtrar();                                                  //Funciones para filtrar donde hay manchas y barreras
void revisarManchas(int, int);
void revisarBarreras(int, int );

void printMatrizAdyacencia();
void printMatrizBits();

int main(){
    llenarMatrizDeBits();
    filtrar();
    completarMatrizAdyacencia();

    Hamilton *hamiltonian = new Hamilton();
    cout<<"\n\nCaminos posibles: "<<endl;

    hamiltonian->hamCycle(matrizAdyacencia);
    int matriz[hamiltonian->cantCaminos][hamiltonian->cantCaminos];

    for (int i = 0; i < hamiltonian->cantCaminos; ++i) {
        for (int j = 0; j < hamiltonian->cantCaminos - 1; ++j) {
            matriz[i][j]=hamiltonian->MatrizConCaminos[i][j];
        }
    }

    //Encuentra el camino mas corto.
    vector<int> caminoActual, caminoMasCorto;
    int distanciaMenor = 9999999;
    for (int i = 0; i < hamiltonian->cantCaminos - 1; ++i) {
        int distancia=0;
        for (int j = 0; j < CANTIDADVERTICES; ++j) {
            //llenar el vector camino, y la distancia de ese vector
            caminoActual.push_back(matriz[i][j]);
            int aux1 = matriz[i][j], aux2 = matriz[i][j+1];
            distancia += matrizPesos[aux1][aux2];
        }
        int actual = distancia;
        if (actual < distanciaMenor ){
            distanciaMenor = actual;
            caminoMasCorto = caminoActual;
            caminoActual.erase(caminoActual.begin(), caminoActual.end());
        }
    }
    cout<<"\n\nEl camino mas corto es:  ";
    caminoMasCorto.erase(caminoMasCorto.begin() + CANTIDADVERTICES + 1, caminoMasCorto.end());
    for (int i = 0; i < caminoMasCorto.size()-1; ++i) {
        cout<<" -> "<<caminoMasCorto[i];
    }
    cout << "\n\nCon una distancia de: " << distanciaMenor*10 << "  metros" << endl;
    return 0;
}


void completarMatrizAdyacencia(){
    //Inicializo la matriz como si en ningun lugar hubiese paso.
    for (int i = 0; i < arrayManchas.size(); ++i){
        for (int j = 0; j < arrayManchas.size(); ++j){
            matrizAdyacencia[i][j] = 0;
            matrizPesos[i][j] = 0;
        }
    }

    //A medida que encuentro camino, lo asigno con la distancia euclideana.
    Posicion p1, p2, q1, q2;
    for (int i = 0; i < arrayManchas.size(); ++i) {
        p1 = arrayManchas[i]->getPosicion();                         //Tomo la posicion de una primer mancha
        for (int j = i+1; j < arrayManchas.size(); ++j) {
            q1 = arrayManchas[j]->getPosicion();                   //Tomo la posicion de otra mancha

            vector<Barrera*> aux=vectorBarreras;

            while(!aux.empty()){
                p2 = aux.back()->p1;                                //Tomo la posicion inicial de una barrera
                q2 = aux.back()->p2;                               //Tomo la posicion fianl de una barrera
                aux.pop_back();
                /*
                * Si no hay interseccion entre la recta que forma la barrera y la recta que forman ambas manchas
                * agrego a la matriz de adyacencia.
                */
                if (!doIntersect(p1, q1, p2, q2)){
                    matrizPesos[i][j] = calcularDistancia(p1, q1);
                    matrizAdyacencia[i][j] = matrizAdyacencia[j][i] = 1;

                } else if(doIntersect(p1, q1, p2, q2)){
                    matrizPesos[i][j] = INFI;
                    matrizAdyacencia[i][j] = matrizAdyacencia[j][i] = 0;
                    aux.erase(aux.begin(),aux.end()); //Borro el vector entero
                }
            }
        }
    }
    //Imprime a ver como quedo la matriz
    printMatrizAdyacencia();
}

void printMatrizAdyacencia(){
    cout<<endl<<endl<<endl;
    cout<<"Matriz de adyacencia: "<<endl;
    for (int x = 0; x < arrayManchas.size(); ++x) {
        for (int y = 0; y < arrayManchas.size(); ++y) {
            cout<<"\t"<<matrizAdyacencia[x][y];
        }cout<<endl;
    }
}

void printMatrizBits(){
    cout<<"\n"<<endl;
    for (int x = 0; x < FILAS; ++x) {
        for (int y = 0; y < COLUMNAS; ++y) {
            cout<<" "<<matrizDeBits[x][y];
        }cout<<endl;
    }
}

void revisarBarreras(int y, int x){

    Barrera *barrera = new Barrera;        //guardo la posicion inicial de la barrera
    barrera->p1.x = x;                    //La barrera esta modelada como una recta.
    barrera->p1.y = y;

    int contadorAuxiliar = 2;
    bool flag = true;

    if (matrizDeBits[y][x+1].test(1)){          //Si la barrera se extiende hacia la derecha
        while(flag){
            if (matrizDeBits[y][x+contadorAuxiliar].test(1))
                contadorAuxiliar++;
            else{
                barrera->p2.x = x + contadorAuxiliar;    //Guardo la posicion2
                barrera->p2.y = y;
                flag = false;
                //Una vez guardada la barrera la saco del mapa para no tomarla de nuevo
                for (int i = barrera->p1.x; i < barrera->p2.x; ++i) {
                    matrizDeBits[barrera->p1.y][i].reset(1);
                }
            }
        }

    } else if (matrizDeBits[y+1][x].test(1)){                //Si la barrera se extiende hacia abajo
        while(flag){
            if (matrizDeBits[y+contadorAuxiliar][x].test(1))
                contadorAuxiliar++;
            else{
                //Guardo la posicion2
                barrera->p2.x = x ;
                barrera->p2.y = y + contadorAuxiliar;
                flag = false;
                //Una vez guardada la barrera la saco del mapa para no tomarla de nuevo
                for (int i = barrera->p1.y; i < barrera->p2.y; ++i) {
                    matrizDeBits[i][barrera->p1.x].reset(1);//Borro la barrera para que no la vuelva a tomar
                }
            }
        }
    }
    vectorBarreras.push_back(barrera);                //Guardo la barrera
}

void revisarManchas(int y, int x){
    //Prueba que efectivamente sea una mancha de al menos 3x3
    bool prueba = matrizDeBits[y][x+1].test(0) && matrizDeBits[y][x+2].test(0) &&
                  matrizDeBits[y+1][x].test(0) && matrizDeBits[y+1][x+1].test(0) && matrizDeBits[y+1][x+2].test(0) &&
                  matrizDeBits[y+2][x].test(0) && matrizDeBits[y+2][x+1].test(0) && matrizDeBits[y+2][x+2].test(0);
    if(prueba){
        Posicion posicion;                                     //Creo una posicion al centro
        posicion.x = x+1; posicion.y = y+1;
        Mancha *mancha = new Mancha(posicion);
        arrayManchas.push_back(mancha);                 //Guardo la mancha
        matrizDeBits[y][x+2].reset(0);  //Una vez marcada la mancha, borro sus esquinas para no confundir el centro.
        matrizDeBits[y+2][x].reset(0);
    }
}

void filtrar(){
    for (int filas = 0; filas < FILAS-1; ++filas){
        for (int columnas = 0; columnas < COLUMNAS-1; ++columnas){
            if (matrizDeBits[filas][columnas].test(0)){
                revisarManchas(filas, columnas);
            }
            else if (matrizDeBits[filas][columnas].test(1)){
                revisarBarreras(filas, columnas);
            }
        }
    }
}

void llenarMatrizDeBits(){
    // Se usan los metodos while con contadores simulando un for, para poder recuperar la posicion de la matriz.
    char dato;  int i=0, j=0;    fstream myFile;
    string line, direccion = "C:\\Users\\facul\\Desktop\\text100.txt";
    myFile.open(direccion, ios::in);
    if (myFile.is_open()){
        while (getline(myFile, line)){
            istringstream isstream(line);
            while(!isstream.eof() && j<COLUMNAS ) {
                isstream >> dato;
                if (dato == '1'){
                    matrizDeBits[i][j].set(0);  //  Seteo si hay maleza
                } else if (dato == 'x' || dato == 'X'){
                    matrizDeBits[i][j].set(1);  //Seteo si hay barrera
                }j++;
            }j=0;i++;
        }i=0;
    }
    myFile.close();

    //cout<<endl<<endl;
    //cout<<"Matriz de arreglos de bits:"<<endl<<
    //"Donde <b1,b0>: b1 indica si existe una barrera, y b0 indica si existe maleza."<<endl<<endl;

    /*for (int x = 0; x < FILAS; ++x) {
        for (int y = 0; y < COLUMNAS; ++y) {
            cout<<" "<<matrizDeBits[x][y];
        }cout<<endl;
    }
     */
}

/*
 * Esta funcion retorna verdadero si existe un interseccion entre las dos rectas.
 * VER CARPETA RESOURCE, ejemploDirecciones.png para comprender dichos metodos.
 */
bool doIntersect(Posicion p1, Posicion q1, Posicion p2, Posicion q2){
    //Encuentra 4 orientaciones necesarios para los casos generales y especiales
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // Caso general
    if (o1 != o2 && o3 != o4)
        return true;

    // Caso especial
    // p1, q1 y p2 son colineales y p2 se encuentra en el segmento p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // p1, q1 y q2 son colineales y q2 se encuentra en el segmento p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // p2, q2 y p1 son colineales y p1 se encuentra en el segmento p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

    // p2, q2 y q1 son colineales y q1 se encuentra en el segmento p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false; // Ninguno de los casos anteriores
}

/*
 * Para encontrar la orientacion de la 3-tupla (p, q, r).
 * La funcion retorna los siguientes valores:
 * 0 --> p, q y r son colineales
 * 1 --> Sentido horario
 * 2 --> Sentido anti-horario
 */
int orientation(Posicion p, Posicion q, Posicion r){
    //Algoritmo de reconocimiento:
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // colineal

    return (val > 0)? 1: 2; // sentido horario o anti-horario
}

/*
 * Dados tres puntos colineales [p,q,r] la funcion chequea si:
 * el punto q esta en el segmento pr
 */
bool onSegment(Posicion p, Posicion q, Posicion r){
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
        return true;
    return false;
}

/*
 * Metodo que calcula la distancia euclidiana entre dos puntos de una superficie de dos dimensiones.
 */
int calcularDistancia(Posicion p1, Posicion q1){
    return sqrt( pow(q1.x-p1.x, 2) + pow(q1.y-p1.y, 2) );
}