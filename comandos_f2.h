#ifndef COMANDOS_F2_H
#define COMANDOS_F2_H

#include "nodo.h"
#include "qstring.h"
#include <iostream>
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include "QStringList"
#include "estructuras.h"
#include "reporte.h"
#include <tgmath.h>
#include "QFile"
#include "QTextStream"
#include "Analizadores/parser.h"
#include "Analizadores/scanner.h"
#include "comandos.h"
#include <stdio.h>
using namespace std;



extern int yyparse();
extern Nodo *raiz;
extern int linea; // Linea del token
extern int columna; // Columna de los tokens
extern int yylineno;

extern login usuario;
extern mount temporal;
extern partitio tem_part;

class Comandos_F2
{
public:
    Comandos_F2();
    int bloque_con_espacio;
    int pos_bloque_con_espacio;
    int inodo_con_espacio;
    int pos_inodo_con_espacio;
    int padre;
    int bloque;
    int block_indirecto;
    int pos_block_indirecto;
    int tipo_op_archivo;
    //QString ruta_contenido;
    int indirecto_archivo_carpeta;

    int exclusivo_archivos;


    QString comando_ejec;

    int global_size;
    QString global_cont;


    QString contenidos_archivos;
    int tamanio_archivo;


    QString valor;
    superBloque global_sb;
    int posicion_inodo_archivo;
    //para indirectos


    QString Quitar_Comillas(QString v);

    void Leer_Principales(Nodo *raiz);
    int Ejecutar_MKFS(Nodo *mkfs);
    int Formatear_Particion_Ext2(QString id_parte,int tipo_system,int tipo_formateo);

    int Creacion_Carpeta_Raiz(superBloque S_b_temporal);
    int Creacion_Archivo_Usuario(superBloque s_b_temporal);

    int Calculo_de_N(int tipo_system);

    bool Buscar_Montada(QString id_montaje);
    bool Buscar_particion();

    int LLenar_Bitmap_Inodos(int inicio_inodo, int final_inodo, int inicio_bloque, int final_bloque);
    int LLenar_Journal(int inicio_jor,int fin_jor);

    int Ejecutar_Login(Nodo *log);
    int Iniciar_Sesion(QString user,QString pass, QString id_part);

    //cierre de sesion
    int Ejecutar_Logout();

    bool Sesion_Activa();

    //para crear archivos
    int Ejecutar_Mkdir(Nodo * dir);
    int Setear_Inicio(QString ruta, bool parametro_p, int tipo_operacion, QString ruta_count, int size, int size_o_cont);

    int Crear_Carpeta(QStringList rutas,bool parametro_p,superBloque sb,int pos_bmi,int pos);
    int Operar_Carpeta(superBloque sb, char *nom_carpeta, int enlace);


    inodo Crear_Inodo(int bloque_libre);
    carpeta Crear_Bloque_C(int padre, int actual);

    superBloque Cambiar_Libre_Inodo(superBloque sb);
    superBloque Cambiar_Libre_Bloque(superBloque sb);

    //MAnejo de archivos
    int Ejecutar_Mkfile(Nodo *arc);
    int Crear_Archivo(int pos_bmi, superBloque sb, QString nombre_ar, QString ruta_cont, int size, int tipo);
    int Operar_Archivo(superBloque sb, char *nom_carpeta, int tamanio, QString ruta);
    superBloque Crear_Bloque_A(int tamanio, inodo tem_i, superBloque sb, QString ruta_contenido, int pos_inodo);


    int Buscar_Indirectos(QStringList rutas, bool parametro_p, superBloque sb, int indirecto, int pos_ruta, int block, FILE *archivo);

    void Setear_Variables(int n1,int n2,int n3,int n4,int n5,int n6);
    void Reset_Variables();

    int Crear_Indirectos_Simples(int pos_inodo, int block, superBloque sb, char *nom_carpeta);

    int Crear_Indirectos_Dobles(int pos_inodo, int block, superBloque sb,char *nom_carpeta);

    int Crear_Indirectos_Triples(int pos_inodo, int block, superBloque sb, char *nom_carpeta);

    int Crear_Indrecto(superBloque sb, inodo inodo_padre, char * nom_carpeta, int inode_bm);

    int Ejecutar_Loos(Nodo *lo);
    int Comando_Loss(QString  id_part);
    int LLenar_Nulos(int inicio, int final, FILE *archivo, int tipo);

    int Ejecutar_Recovery(Nodo *rec);
    int Comando_Recovery(QString id_part);
    QString Leer_Jor(FILE *archivo);


    int Escribir_Jor();

    //----cat

    int Ejecutar_Cat(Nodo *cat);

    int Leer_Archivo(QString ruta);

    int Buscar_Carpeta_Archivo(int pos_bmi, superBloque sb, QStringList rutas, int pos);

    int Buscar_Indirectos_C_A(QStringList rutas, superBloque sb, int indirecto, int pos_ruta, int block, FILE *archivo);

    int Recorrer_Bloque_Archivos(int pos_inodo,superBloque sb);

    int Ejecutar_Mkgrp(Nodo *grupo);

    int Agregar_Grupos(QString nombre_grupo);
    int Recorrer_Grupos(QString con_archivo, QString nombre, QString nombre_usuario, int iniciar);

    int Agregar_Eliminar_Contenido_Archivos(QString contenido_nuevo, int pos_inodo, superBloque sb);

    //crear usuarios
    int Ejecutar_Mkuser(Nodo *usuario);
    int Agregar_Usuario(QString nombre_usuario, QString password, QString nombre_grupo);


    int Usuario_Sesion(QString nombre_usuario, QString pass, QString contenido, QString id_part);

    int Buscar_Usuario(QString id_particion, QString nombre_usuario, QString pass);

};

#endif // COMANDOS_F2_H
