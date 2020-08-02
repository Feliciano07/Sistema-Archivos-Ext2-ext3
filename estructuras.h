#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H
#include<time.h>
#include<QString>
using namespace std;




//El ebr se crea solo cuando se hace una logica
typedef struct EBR{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
}ebr;


typedef struct PARTITION{
    char part_status;//0
    char part_type;//0
    char part_fit;//0
    int part_start;//0
    int part_size;//0
    char part_name[16];

}partitio;


typedef struct MBR{
    int mbr_tamano;
    time_t mbr_fecha;
    int mbr_signatura;
    char disk_fit;
    partitio part [4];
}mbr;

typedef struct mount{
    QString disco_ruta;
    QString nombre_disco;
    QString nombre_part;
    QString id_mont;
    int estado;
    int numero;//numero
    int numero_letra;
}mount;

typedef struct SUPERBLOQUE{
    int filesystem_type;
    int inodes_count;
    int bloques_count;
    int free_bloques_count;
    int free_inodes_count;
    time_t mount_time;
    time_t unmount_time;
    int mount_count;
    int magic;
    int size_inodo;
    int size_bloque;
    int primer_inodo_free;
    int primer_bloque_free;
    int bm_start_inodo;
    int bm_start_bloque;
    int start_inodo;
    int start_bloque;
}superBloque;

typedef struct INODO{
    int usuario_id;
    int grupo_id;
    int size_file;
    time_t ulti_time;
    time_t crear_time;
    time_t modif_time;
    int apuntador[15];
    char type_file;
    int permiso;
}inodo;

typedef struct CONTENT{
    char nombre_dir_file[12];
    int apuntador_inodo;
}content;

typedef struct CARPETA{
    content contenido[4];
}carpeta;

typedef struct BLOQUE_ARCHIVO{
    char contenido[64];
}bloque_archivo;

typedef struct BLOQUE_APUNTADORES{
    int apuntador[16];
}bloque_pointers;


typedef struct LOGIN{
    char sesion;
    char l_usuario[10];
    char l_pass[10];
    QString id_part;
    int id_user;
    int id_grupo;
    int permisos;
}login;

typedef struct JOURNALING{
   char operacion[250];
   time_t fecha;
   char status=0;
}journaling;

class Estructuras
{
public:
    Estructuras();
};

#endif // ESTRUCTURAS_H
