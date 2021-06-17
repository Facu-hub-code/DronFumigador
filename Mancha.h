//
// Created by facul on 12/06/2021.
//

#ifndef PARCIAL3_MANCHA_H
#define PARCIAL3_MANCHA_H
//Definicion de estructuras
struct Posicion{
    int x;
    int y;
};

class Mancha {

private:
    Posicion posicion;

public:
    Mancha(const Posicion &posicion1){
        posicion = posicion1;
    }

    const Posicion &getPosicion() {
        return posicion;
    }

    void setPosicion(const Posicion &posicion1){
        posicion = posicion1;
    }
};


#endif //PARCIAL3_MANCHA_H
