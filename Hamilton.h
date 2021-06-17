//
// Created by facul on 16/06/2021.
//
#ifndef HAMILTONEANOCPP_GFG_H
#define HAMILTONEANOCPP_GFG_H
#define CANTIDADVERTICES 8
#include "vector"
#include "iostream"


using namespace std;

class Hamilton {

public:
    int cantCaminos=0;
    int MatrizConCaminos[1000][1000];

    // Funcion que verifica si un vertice v puede ser agregado en la posicion pos en el ciclo Hamiltoneano.
    bool isSafe(int v, int graph[CANTIDADVERTICES][CANTIDADVERTICES], vector<int> path , int pos){
        // Si el vertice es adyacente al vertice anteriormente agregado
        if ( graph[path[pos - 1]][v] == 0 ){
            return false;
        }
        // Si el vertice ya ha sido incluido en el camino.
        for (int i = 0; i < pos; i++){
            if (path[i] == v){
                return false;
            }
        }
        // Si las condiciones de arriba no son verdaderas, devuelve true
        return true;
    }
    // Para chequear si existe al menos un ciclo hamiltoneano
    bool hasCycle; //todo poner privado
    // Funcion que encuentra todos los posibles ciclos
    void hamCycle(int graph[CANTIDADVERTICES][CANTIDADVERTICES]) {
        // Inicializa la flag en falso.
        hasCycle = false;

        // Guarda el camino obtenido
        vector<int> path;  //todo filtrar memoria
        path.push_back(0);

        //Mantiene el seguimiento de los vértices visitados
        bool visited[CANTIDADVERTICES];

        for (int i = 0; i < CANTIDADVERTICES; i++){
            visited[i] = false;
        }

        visited[0] = true;

        // Llamada recursiva a la funcion
        FindHamCycle(graph, 1, path, visited);

        if (!hasCycle) {
            // Si no hay un ciclo hamiltoneano posible para el grafo dado
            cout<<"\tNo Hamiltonian Cycle possible ";
            return;
        }
    }
    // Funcion recursiva para encontrar todos los ciclos hamiltoneanos posibles
    void FindHamCycle(int graph[CANTIDADVERTICES][CANTIDADVERTICES], int pos, vector<int> path, bool visited[]) {
        // Si todos los vertices estan incluidos en el ciclo hamiltoneano.
        if (pos == CANTIDADVERTICES) {

            // Si hay una arista del ultimo vertice hacia el vertice inicial
            if (graph[path[path.size() - 1]] [path[0]] != 0) {
                // Incluimos el vertice inicial al camino, y lo imprimimos.
                path.push_back(0);
                cout<<"\t\t";
                for (int i = 0; i < path.size(); i++) {
                    cout<<" "<<path[i];
                        for (int j = 0; j < path.size(); ++j) {
                            MatrizConCaminos[cantCaminos][j]=path[j];
                        }


                }
                cantCaminos++; //Aumento el cantCaminos cada vez que encuentro un camino
                cout<<"\n";

                // Quita el vertice inicial agregado.
                path.erase(path.begin()+(path.size()-1));
                // Actualiza el flag
                hasCycle = true;
            }
            return;
        }

        // Prueba diferentes vertices como siguientes del ciclo.
        for (int v = 0; v < CANTIDADVERTICES; v++) {
            // Revisa si se puede agregar dicho vertice
            if (isSafe(v, graph, path, pos) && !visited[v]) {
                path.push_back(v);
                visited[v] = true;
                // Llamada recursiva para el resto del camino.
                FindHamCycle(graph, pos + 1,path, visited);
                // Quita el vertice actual del cmaino y procesa otros vertices
                visited[v] = false;
                path.erase(path.begin()+(path.size()-1));

            }
        }
    }
};


#endif //HAMILTONEANOCPP_GFG_H
