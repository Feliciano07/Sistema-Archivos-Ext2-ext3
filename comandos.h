#ifndef COMANDOS_H
#define COMANDOS_H
#include "nodo.h"
#include "QString"
#include <iostream>
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include "QStringList"
#include "estructuras.h"
#include "reporte.h"
#include "comandos_f2.h"
using namespace std;
extern int varLetra;
extern mount montar_repo[50];




class Comandos
{
public:
    Comandos();

    bool segunda_pasada;

    void Tipos_Comandos();




    QString Leer_Principales(Nodo *raiz);
    QString Leer_Archivo(Nodo *ext);


    QString Quitar_Comillas(QString entrada);
    int Numero_Aleatorio();


    QString Ejecutar_Mkdisk(Nodo *mk);
    void Crear_disco(int tamanio,int unidades,char ajuste,string ruta);

    bool Existe_directorio(QString ruta);

    QString Ejecutar_Rmdisk(Nodo *rm);
    void Eliminar_disco(string ruta);

    bool Existe_archivo(string ruta);

    void Ejecutar_fdisk(Nodo *fd);
    int Crear_particion(int tamanio,int unidades,char ajuste,char part,QString ruta, QString nombre);
    bool Verificar_Particiones(mbr disco,char tipo);
    bool Verificar_Espacio(mbr disco,int espacio);
    bool Primera_Particon(mbr disco);

    mbr Mejor_ajuste(mbr disco,int size_particion,char ajuste_part,char tipo_part,char *nombre,QString ruta);
    mbr Primer_Ajuste(mbr disco,int size_particion,char ajuste_part,char tipo_part,char *nombre,QString ruta);
    mbr Peor_Ajuste(mbr disco,int size_particion,char ajuste_parte,char tipo_part,char *nombre,QString ruta);

    int Eliminar_Particion(int tipo_eliminacion,QString nombre,QString ruta);

    int Agregar_Particion(int unidad,int tamanio,QString nombre,QString ruta);

    mbr Ordenamiento_particiones(mbr disco);

    //Montar
    void Ejecutar_Montaje(Nodo *montaje);
    int Hacer_Montaje(QString ruta,QString name);

    void Ejecutar_Desmontaje(Nodo *desmontaje);
    int Hacer_Desmontaje(QString nombre_part);


    //Reportes
    void Ejecutar_Reporte(Nodo *repo);
    void Verificar_Reporte(int tipo, QString ruta, QString id, QString ruta_archivo);
    QString Return_ID_reporte(int value);


    //Manipulacion de particiones logicas
    int Crear_EBR_Inicial(QString ruta,int pos_inicial);
    bool Nombres_Iguales_Ext(QString ruta,int pos_inicial,QString nombre_part);
    int Montar_Logicas();

    bool Espacio_Ext(QString ruta, int pos_inicial,int size_part,int size_logica);

    int Primer_Ajuste_Ext(int size_part,char ajuste_part,QString nombre,QString ruta,int inicial_ext, int size_ext);
    int Peor_Ajuste_Ext(int size_part,char ajuste_part,QString nombre,QString ruta,int inicial_ext, int size_ext);
    int Mejor_Ajuste_Ext(int size_part,char ajuste_part,QString nombre,QString ruta,int inicial_ext, int size_ext);

    int Eliminar_Logica(int tipo_eliminacion,QString nombre_part, QString ruta,int inicio_ext,int size_ext);

    int Agregar_Logica(int unidad,int tamanio,QString nombre,QString ruta,int inicio_ext,int size_ext);

};

#endif // COMANDOS_H
