#ifndef REPORTE_H
#define REPORTE_H

#include<QString>
#include<QStringList>
#include "estructuras.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
extern mount montar_repo[50];
using namespace std;
class Reporte
{
public:
    Reporte();

    int exclusivo_archivos;
    QString contenidos_archivos;

    int Reporte_MBR(QString id_part,QString ruta, QString value);
    int Reporte_DISK(QString nombre,QString ruta,QString id);
    int Reporte_SB(QString id_part,QString ruta, QString value);

    int Reporte_BM_Inode(QString id_part,QString ruta, QString value);

    int Reporte_BM_Bloque(QString id_part,QString ruta, QString value);

    int Reporte_Inode(QString id_part,QString ruta, QString value);
    int Reporte_Bloque(QString id_part,QString ruta, QString value);

    int ReporteTree(QString id_part,QString ruta, QString value);

    int Tree_Inodo(int padre, int actual,int pos,ofstream &reporte,FILE *archivo,superBloque sb);

    int Tree_Bloque(int padre, int actual, int pos, ofstream &reporte, FILE *archivo, superBloque sb, int tipo);

    int Indirecto_Simple(int padre, int actual, int pos, ofstream &reporte, FILE *archivo, superBloque sb, int tipo);

    int Indirecto_Doble(int padre, int actual, int pos, ofstream &reporte, FILE *archivo, superBloque sb, int tipo);

    int Indirecto_Triple(int padre, int actual, int pos, ofstream &reporte, FILE *archivo, superBloque sb, int tipo);

    int Tree_Indirecto(int padre, int actual, int pos, ofstream &reporte, FILE *archivo, superBloque sb, int tipo);

    int Reporte_Journaling(QString id_part,QString ruta, QString value);

    int Reporte_File(QString id_part,QString ruta,QString value,QString ruta_archivo);

    int Buscar_Carpeta_Archivo(int pos_bmi,superBloque sb, QStringList rutas,int pos);

    int Buscar_Indirectos_C_A(QStringList rutas, superBloque sb, int indirecto, int pos_ruta, int block, FILE *archivo);

    int Recorrer_Bloque_Archivos(int pos_inodo,superBloque sb);

    int Reporte_Ls(QString id_part,QString ruta,QString value,QString ruta_archivo);

    int Buscar_Usuario(int id_usuario, int id_grupo, ofstream &reporte);

    int Datos_Usuario(int id_usuario, int id_grupo, QString contenido, ofstream &reporte);

    int Verificar_montaje(QString id_part);
    mount temporal_mount;
    partitio temporal_part;

};

#endif // REPORTE_H
