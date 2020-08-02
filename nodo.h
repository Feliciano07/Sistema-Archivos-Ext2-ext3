#ifndef NODO_H
#define NODO_H
#include "QString"
#include "QList"
#include <list>
using namespace std;

class Nodo
{
public:
    Nodo();

    int linea;
    int columna;
    QString tipo;
    QString valor;

    Nodo(int linea,int columna,QString t, QString v);

    void add(Nodo hijo);

    QList<Nodo> hijos;

    int tipo_;

    int Tipo_Comando();

};

#endif // NODO_H
