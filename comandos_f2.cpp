#include "comandos_f2.h"
enum Estados{

    EXEC,
    MKDISK,
    RMDISK,
    FDISK,
    MOUNT,
    UNMOUNT,
    REP,

    size,
    fit,
    unit,
    path,
    type,
    DELETE,
    name,
    add,
    id,

    MKFS,
    fs,
    MKDIR,
    Login,
    usr,
    pwd,
    Logout,
    PP,
    MKFILE,
    Cont,
    LOSS,
    RECOVERY,
    CAT,
    File,
    PAUSE,
    Ruta,
    MKGRP,
    grp,
    MKUSER

};


Comandos_F2::Comandos_F2()
{
    temporal.estado=0;
    tem_part.part_status='0';
    this->pos_bloque_con_espacio=-1;
    this->pos_inodo_con_espacio=-1;
    this->bloque_con_espacio=-1;
    this->inodo_con_espacio=-1;
    this->padre=-1;
    this->bloque=-1;
    this->tipo_op_archivo=-1;
    this->block_indirecto=-1;
    this->pos_block_indirecto=-1;
    //this->ruta_contenido="";
    this->global_cont="";
    this->global_size=-1;
    this->valor="";
    this->comando_ejec="";

    this->contenidos_archivos="";
    this->exclusivo_archivos=-1;
    this->tamanio_archivo=0;
}

QString Comandos_F2::Quitar_Comillas(QString v){
    if(v.startsWith("\"")){
            if(v.endsWith("\"")){
                QString salida;
                for(int a=1;a<=v.size()-2;a++){
                    salida.append(v[a]);
                }
                v=salida;
            }
    }
    return v;
}

bool Comandos_F2::Sesion_Activa(){
    if(usuario.sesion=='1'){return true;}
    cout<<"INFO::No existe sesion Activa para ejecutar los comandos"<<endl;
    return false;
}

void Comandos_F2::Leer_Principales(Nodo *raiz){

        Nodo tem=raiz->hijos.at(0);
        int tipo=raiz->tipo_;
        switch (tipo) {
        case MKFS:
            Ejecutar_MKFS(&tem);
            break;
        case MKDIR:
            if(Sesion_Activa()){
                 Ejecutar_Mkdir(&tem);
            }

            break;
        case MKFILE:
            if(Sesion_Activa()){
                 Ejecutar_Mkfile(&tem);
            }
            break;
        case Login:
            Ejecutar_Login(&tem);
            break;

        case Logout:
            Ejecutar_Logout();
            break;

        case LOSS:
            if(Sesion_Activa()){
                 Ejecutar_Loos(&tem);
            }

            break;

        case RECOVERY:
            if(Sesion_Activa()){
                Ejecutar_Recovery(&tem);
            }
            break;

        case CAT:
            if(Sesion_Activa()){
                            Ejecutar_Cat(&tem);
            }
            break;
        case PAUSE:
            cout<<"Presione enter para continuar"<<endl;
            getchar();
            break;

        case MKGRP:
            Ejecutar_Mkgrp(&tem);
            break;
        case MKUSER:
            Ejecutar_Mkuser(&tem);
            break;
        default:
            cout<<"INFO::Comando no reconocido"<<tem.tipo.toStdString()<<endl;
            break;
        }


}


bool Comandos_F2::Buscar_Montada(QString id_montaje){
    temporal.estado=0;
    for(int i=0;i<50;i++){
        if(montar_repo[i].estado==1){
            if(QString::compare(id_montaje,montar_repo[i].id_mont)==0){
                   temporal=montar_repo[i];
            }
        }
    }
    if(temporal.estado==1){
        return Buscar_particion();//regresa true la encontro;
    }else{
        cout<<"ERROR:: la particion con id "<<id_montaje.toStdString()<<" No se encuentra montada"<<endl<<endl;
        return false;
    }
    return false;
}

bool Comandos_F2::Buscar_particion(){
    FILE *archivo;
    QString ar=temporal.disco_ruta;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: No se encontro el disco"<<endl;
        return false;
    }
    fseek(archivo,0,SEEK_SET);
    mbr disco;
    fread(&disco,sizeof(mbr),1,archivo);

    for(int i=0;i<4;i++){
        if(disco.part[i].part_status=='1'){
            if(strcasecmp(temporal.nombre_part.toStdString().c_str(),disco.part[i].part_name)==0){
                tem_part=disco.part[i];
                return true;
            }
        }
    }
    fclose(archivo);
    return false;
}

//---------- EJECUTAR COMANDO MKFS

int Comandos_F2::Ejecutar_MKFS(Nodo *mkfs){
    bool is_id=false;
    bool error_general=false;
    int tipo_formateo=1;
    int tipo_system=0;
    QString id_part="";
    for(int i=0;i<mkfs->hijos.size();i++){
        Nodo tem=mkfs->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case id:
            id_part=tem.valor;
            is_id=true;
            break;
        case type:
            if(QString::compare(tem.valor,"fast",Qt::CaseInsensitive)==0){
                tipo_formateo=0;
            }else if(QString::compare(tem.valor,"full",Qt::CaseInsensitive)==0){
                tipo_formateo=1;
            }else{
                tipo_formateo=-1;
                cout<<"ERROR:: el tipo de formateo especificado no es valido, verifique"<<endl<<endl;
                error_general=true;
            }
            break;

        case fs:
            if(QString::compare(tem.valor,"2fs",Qt::CaseInsensitive)==0){
                tipo_system=0;
            }else if(QString::compare(tem.valor,"3fs",Qt::CaseInsensitive)==0){
                tipo_system=1;
            }else{
                tipo_system=-1;
                cout<<"ERROR:: el tipo de sistema de archivos no es valido, verifique"<<endl<<endl;
                error_general=true;
            }
            break;
        default:
            cout<<"ERROR:: Parametro no valido dentro del mkfs,verifique "<<tem.valor.toStdString()<<endl;
            error_general=true;
            break;
        }
    }

    if(!error_general){
         if(is_id && tipo_system>=0){
             Formatear_Particion_Ext2(id_part,tipo_system,tipo_formateo);
         }else{
             cout<<"ERROR:: no se especifico el id de la particion a formatear"<<endl<<endl;
             return 0;
         }
    }
    return 0;
}

int Comandos_F2::Calculo_de_N(int tipo_system){
    if(tipo_system==0){
        int tamanio_particion=tem_part.part_size;
        //buscamos n;
        double n=(double)(tamanio_particion-sizeof(superBloque))/(4+sizeof(inodo)+(3*sizeof(bloque_archivo)));
        n=floor(n);
        return (int)n;
    }else{
        int tamanio_particion=tem_part.part_size;
        double n=(double)(tamanio_particion-sizeof(superBloque))/(4+sizeof(journaling)+sizeof(inodo)+(3*sizeof(bloque_archivo)));
        n=floor(n);
        return (int)n;
    }
    return 0;
}


int Comandos_F2::LLenar_Bitmap_Inodos(int inicio_inodo, int final_inodo, int inicio_bloque,int final_bloque){
    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    char x='0';
    if(archivo==NULL){
        cout<<"Error: No se encontro el disco"<<endl;
        return 0;
    }
    fseek(archivo,inicio_inodo,SEEK_SET);
    for(int i=0;i<final_inodo;i++){
        fwrite(&x,sizeof(char),1,archivo);
        inicio_inodo++;
        fseek(archivo,inicio_inodo,SEEK_SET);
    }
    fseek(archivo,inicio_bloque,SEEK_SET);
    for(int i=0;i<final_bloque;i++){
        fwrite(&x,sizeof(char),1,archivo);
        inicio_bloque++;
        fseek(archivo,inicio_bloque,SEEK_SET);
    }
    fclose(archivo);
    return 0;
}

int Comandos_F2::LLenar_Journal(int inicio_jor, int fin_jor){
    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    journaling tem_j;
    tem_j.status='0';
    if(archivo==NULL){
        cout<<"Error: No se encontro el disco"<<endl;
        return 0;
    }
    int contador=0;
    int apuntador=inicio_jor;
    fseek(archivo,apuntador,SEEK_SET);
    for(int i=0;i<fin_jor;i++){
        fwrite(&tem_j,sizeof(journaling),1,archivo);
        contador++;
        apuntador=inicio_jor+(contador*sizeof(journaling));
        fseek(archivo,apuntador,SEEK_SET);
    }
    return 0;
}

int Comandos_F2::Formatear_Particion_Ext2(QString id_parte, int tipo_system,int tipo_formateo){
    //Verificar el montaje y particion
    if(Buscar_Montada(id_parte)){ 
         int n=Calculo_de_N(tipo_system);
        if(tipo_system==0){
            int inicio_part=tem_part.part_start;
            superBloque tem_sb;
            tem_sb.filesystem_type=tipo_system;
            tem_sb.inodes_count=n;
            tem_sb.bloques_count=3*n;
            tem_sb.free_bloques_count=3*n;//esto lo cambio
            tem_sb.free_inodes_count=n;//esto lo cambio
            tem_sb.mount_time=time(0);
            tem_sb.unmount_time=time(0);
            tem_sb.mount_count=0;
            tem_sb.magic=201701089;
            tem_sb.size_inodo=sizeof(inodo);
            tem_sb.size_bloque=sizeof(bloque_archivo);
            tem_sb.primer_inodo_free=0;//esto lo cambio
            tem_sb.primer_bloque_free=0;//esto lo cambio
            tem_sb.bm_start_inodo=inicio_part+sizeof(superBloque);
            tem_sb.bm_start_bloque=inicio_part+sizeof(superBloque)+n;
            tem_sb.start_inodo=inicio_part+sizeof(superBloque)+n+(3*n);
            tem_sb.start_bloque=tem_sb.start_inodo+(sizeof(inodo)*n);

            //llenar los bitmap de inodos y bloques
            LLenar_Bitmap_Inodos(tem_sb.bm_start_inodo,tem_sb.inodes_count,tem_sb.bm_start_bloque,tem_sb.bloques_count);
            FILE *archivo;
            archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

            if(archivo==NULL){
                cout<<"Error: No se encontro el disco"<<endl;
                return 0;
            }
            fseek(archivo,tem_part.part_start,SEEK_SET);
            fwrite(&tem_sb,sizeof(superBloque),1,archivo);
            fclose(archivo);


            Creacion_Carpeta_Raiz(tem_sb);

             archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

            if(archivo==NULL){
                fclose(archivo);
                cout<<"Error: No se encontro el disco"<<endl;
                return 0;
            }
            fseek(archivo,tem_part.part_start,SEEK_SET);
            fread(&tem_sb,sizeof(superBloque),1,archivo);
            fclose(archivo);

            Creacion_Archivo_Usuario(tem_sb);


            cout<<"INFO:: se realizo el formateo de la particion a ext2"<<endl<<endl;
        }else{
            int inicio_part=tem_part.part_start;
            superBloque tem_sb;
            tem_sb.filesystem_type=tipo_system;
            tem_sb.inodes_count=n;
            tem_sb.bloques_count=3*n;
            tem_sb.free_bloques_count=3*n;//esto lo cambio
            tem_sb.free_inodes_count=n;//esto lo cambio
            tem_sb.mount_time=time(0);
            tem_sb.unmount_time=time(0);
            tem_sb.mount_count=0;
            tem_sb.magic=201701089;
            tem_sb.size_inodo=sizeof(inodo);
            tem_sb.size_bloque=sizeof(bloque_archivo);
            tem_sb.primer_inodo_free=0;//esto lo cambio
            tem_sb.primer_bloque_free=0;//esto lo cambio

            int inicio_jor=inicio_part+sizeof(superBloque);
            int fin_jor=inicio_jor+(n*sizeof(journaling));

            tem_sb.bm_start_inodo=fin_jor;
            tem_sb.bm_start_bloque=fin_jor+n;
            tem_sb.start_inodo=fin_jor+n+(3*n);
            tem_sb.start_bloque=tem_sb.start_inodo+(sizeof(inodo)*n);

            LLenar_Journal(inicio_jor,tem_sb.inodes_count);
            //llenar los bitmap de inodos y bloques
            LLenar_Bitmap_Inodos(tem_sb.bm_start_inodo,tem_sb.inodes_count,tem_sb.bm_start_bloque,tem_sb.bloques_count);



            FILE *archivo;
            archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

            if(archivo==NULL){
                cout<<"Error: No se encontro el disco"<<endl;
                return 0;
            }
            fseek(archivo,tem_part.part_start,SEEK_SET);
            fwrite(&tem_sb,sizeof(superBloque),1,archivo);
            fclose(archivo);


            Creacion_Carpeta_Raiz(tem_sb);

             archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

            if(archivo==NULL){
                fclose(archivo);
                cout<<"Error: No se encontro el disco"<<endl;
                return 0;
            }
            fseek(archivo,tem_part.part_start,SEEK_SET);
            fread(&tem_sb,sizeof(superBloque),1,archivo);
            fclose(archivo);

            Creacion_Archivo_Usuario(tem_sb);


            cout<<"INFO:: se realizo el formateo de la particion a ext3"<<endl<<endl;
        }



    }else{
        cout<<"Error:: No se logro ejecutar el formateo de "<<id_parte.toStdString()<<" tipo ext2 "<<endl<<endl;
        return 0;
    }
    return 0;
}

int Comandos_F2::Creacion_Carpeta_Raiz(superBloque S_b_temporal){
    int bm_libre_inodo=S_b_temporal.primer_inodo_free;//posicion inodo libre bm
    int posicion_libre_bmI=S_b_temporal.bm_start_inodo+(bm_libre_inodo);//posicion del archivo del inodo libre BM
    int posicion_libreI=S_b_temporal.start_inodo+(bm_libre_inodo*S_b_temporal.size_inodo);//posicion archivo del inodo libre

    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: No se encontro el disco"<<endl;
        return 0;
    }

    inodo nuevo_i;
    nuevo_i.usuario_id=1;
    nuevo_i.grupo_id=1;
    nuevo_i.crear_time=time(0);
    nuevo_i.modif_time=time(0);
    nuevo_i.permiso=777;
    nuevo_i.size_file=0;
    nuevo_i.type_file='0';
    nuevo_i.ulti_time=time(0);
    for(int i=0;i<15;i++){
        nuevo_i.apuntador[i]=-1;
    }
    char ocupado='1';
    fseek(archivo,posicion_libre_bmI,SEEK_SET);
    fwrite(&ocupado,sizeof(char),1,archivo);
    //Guardar el siguiente bm libre en el super_bloque
    fseek(archivo,posicion_libre_bmI,SEEK_SET);
    for(int i=bm_libre_inodo;i<S_b_temporal.inodes_count;i++){
        char estado='3';
        fread(&estado,sizeof(char),1,archivo);
        if(estado=='0'){
            S_b_temporal.primer_inodo_free=i;//cambio el inodo libre
            break;
        }
        posicion_libre_bmI++;
        fseek(archivo,posicion_libre_bmI,SEEK_SET);//sumando uno por uno
    }
    S_b_temporal.free_inodes_count=S_b_temporal.free_inodes_count-1;
    //buscar bloque

    int bm_libre_bloque=S_b_temporal.primer_bloque_free;//posicion inodo libre bm
    int posicion_libre_bmB=S_b_temporal.bm_start_bloque+(bm_libre_bloque);//posicion del archivo del inodo libre BM
    int posicion_libreB=S_b_temporal.start_bloque+(bm_libre_bloque*S_b_temporal.size_bloque);//posicion archivo del inodo libre

    //creacion del bloque carpeta
    carpeta nueva_carpeta;
    nueva_carpeta.contenido[0].apuntador_inodo=bm_libre_inodo;
    strcpy(nueva_carpeta.contenido[0].nombre_dir_file,".");
    nueva_carpeta.contenido[1].apuntador_inodo=bm_libre_inodo;
    strcpy(nueva_carpeta.contenido[1].nombre_dir_file,"..");
    nueva_carpeta.contenido[2].apuntador_inodo=-1;
    strcpy(nueva_carpeta.contenido[2].nombre_dir_file,"--");
    nueva_carpeta.contenido[3].apuntador_inodo=-1;
    strcpy(nueva_carpeta.contenido[3].nombre_dir_file,"--");

    //primer apuntador  a bm_libre_bloque
    nuevo_i.apuntador[0]=bm_libre_bloque;
    fseek(archivo,posicion_libre_bmB,SEEK_SET);
    fwrite(&ocupado,sizeof(char),1,archivo);

    //cambiar el bloque libre en el super_bloque
    fseek(archivo,posicion_libre_bmB,SEEK_SET);
    for(int i=bm_libre_bloque;i<S_b_temporal.bloques_count;i++){
        char estado='2';
        fread(&estado,sizeof(char),1,archivo);
        if(estado=='0'){
            S_b_temporal.primer_bloque_free=i;//cambio el inodo libre
            break;
        }
        posicion_libre_bmB++;
        fseek(archivo,posicion_libre_bmB,SEEK_SET);//sumando uno por uno
    }

    S_b_temporal.free_bloques_count=S_b_temporal.free_bloques_count-1;

    //ahora si escribir las el inodo carpeta y el bloque carpeta
    fseek(archivo,posicion_libreI,SEEK_SET);
    fwrite(&nuevo_i,sizeof(inodo),1,archivo);
    fseek(archivo,posicion_libreB,SEEK_SET);
    fwrite(&nueva_carpeta,sizeof(carpeta),1,archivo);

    fseek(archivo,tem_part.part_start,SEEK_SET);


    fwrite(&S_b_temporal,sizeof(superBloque),1,archivo);

    fclose(archivo);
    return 0;
}

int Comandos_F2::Creacion_Archivo_Usuario(superBloque S_b_temporal){
    int bm_libre_inodo=S_b_temporal.primer_inodo_free;//posicion inodo libre bm
    int posicion_libre_bmI=S_b_temporal.bm_start_inodo+(bm_libre_inodo);//posicion del archivo del inodo libre BM
    int posicion_libreI=S_b_temporal.start_inodo+(bm_libre_inodo*S_b_temporal.size_inodo);//posicion archivo del inodo libre

    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: No se encontro el disco"<<endl;
        return 0;
    }
    QString user="1,G,root\\n";
    user=user+"1,U,root,root,123\\n";
    QByteArray array = user.toLocal8Bit();
    char* nom = array.data();

    inodo nuevo_i;
    nuevo_i.usuario_id=1;
    nuevo_i.grupo_id=1;
    nuevo_i.crear_time=time(0);
    nuevo_i.modif_time=time(0);
    nuevo_i.permiso=777;
    nuevo_i.size_file=user.size();
    nuevo_i.type_file='1';
    nuevo_i.ulti_time=time(0);
    for(int i=0;i<15;i++){
        nuevo_i.apuntador[i]=-1;
    }
    char ocupado='1';
    fseek(archivo,posicion_libre_bmI,SEEK_SET);
    fwrite(&ocupado,sizeof(char),1,archivo);

    //cambiar en el super bloque el primer inodo libre
    fseek(archivo,posicion_libre_bmI,SEEK_SET);
    for(int i=bm_libre_inodo;i<S_b_temporal.inodes_count;i++){
        char estado='2';
        fread(&estado,sizeof(char),1,archivo);
        if(estado=='0'){
            S_b_temporal.primer_inodo_free=i;
            break;
        }
        posicion_libre_bmI++;
        fseek(archivo,posicion_libre_bmI,SEEK_SET);
    }
    S_b_temporal.free_inodes_count=S_b_temporal.free_inodes_count-1;

    int bm_libre_bloque=S_b_temporal.primer_bloque_free;//posicion inodo libre bm
    int posicion_libre_bmB=S_b_temporal.bm_start_bloque+(bm_libre_bloque);//posicion del archivo del inodo libre BM
    int posicion_libreB=S_b_temporal.start_bloque+(bm_libre_bloque*S_b_temporal.size_bloque);//posicion archivo del inodo libre


    char final[60];
    strcpy(final,nom);

    //modificar el tercer espacio del bloque de la carpeta raiz
    int bloque_carpeta_raiz=S_b_temporal.start_bloque;//es el primer bloque
    carpeta temporal_car;
    fseek(archivo,bloque_carpeta_raiz,SEEK_SET);
    fread(&temporal_car,sizeof(carpeta),1,archivo);
    temporal_car.contenido[2].apuntador_inodo=bm_libre_bloque;

    strcpy(temporal_car.contenido[2].nombre_dir_file,"user.txt");
    fseek(archivo,bloque_carpeta_raiz,SEEK_SET);
    fwrite(&temporal_car,sizeof(carpeta),1,archivo);//escritura del bloque carpeta de la raiz


    bloque_archivo nuevo_bloque;
    strcpy(nuevo_bloque.contenido,final);//copiar cadena

    nuevo_i.apuntador[0]=bm_libre_bloque;
    fseek(archivo,posicion_libre_bmB,SEEK_SET);
    fwrite(&ocupado,sizeof(char),1,archivo);

    //cambiar el bloque libre en el super_bloque
    fseek(archivo,posicion_libre_bmB,SEEK_SET);
    for(int i=bm_libre_bloque;i<S_b_temporal.bloques_count;i++){
        char estado='2';
        fread(&estado,sizeof(char),1,archivo);
        if(estado=='0'){
            S_b_temporal.primer_bloque_free=i;//cambio el inodo libre
            break;
        }
        posicion_libre_bmB++;
        fseek(archivo,posicion_libre_bmB,SEEK_SET);//sumando uno por uno
    }
    //escribir inodo y bloque creado
    S_b_temporal.free_bloques_count=S_b_temporal.free_bloques_count-1;

    fseek(archivo,posicion_libreI,SEEK_SET);
    fwrite(&nuevo_i,sizeof(inodo),1,archivo);
    fseek(archivo,posicion_libreB,SEEK_SET);
    fwrite(&nuevo_bloque,sizeof(bloque_archivo),1,archivo);

    fseek(archivo,tem_part.part_start,SEEK_SET);
    fwrite(&S_b_temporal,sizeof(superBloque),1,archivo);

    fclose(archivo);
    return 0;

}

int Comandos_F2::Ejecutar_Login(Nodo *log){
    QString name_user="";
    QString pass="";
    QString id_part="";
    bool error_general=false;
    bool is_user=false;
    bool is_pass=false;
    bool is_id=false;
    for(int i=0;i<log->hijos.size();i++){
        Nodo tem=log->hijos.at(i);
        int tipo_=tem.tipo_;
        switch (tipo_) {
        case usr:
            name_user=Quitar_Comillas(tem.valor);
            is_user=true;
            break;

        case pwd:
            pass=Quitar_Comillas(tem.valor);
            is_pass=true;
            break;

        case id:
            id_part=tem.valor;
            is_id=true;
            break;
        default:
            cout<<"ERROR::Comando no valido para la ejecucion de login"<<endl;
            error_general=true;
            break;
        }
    }
    if(!error_general){
        if(is_user && is_pass && is_id){
            Iniciar_Sesion(name_user,pass,id_part);
        }else{
            cout<<"ERROR:: Faltan comandos obligatorios de usuario, password o id partcion en login"<<endl;
            return 0;
        }
    }
    return 0;
}



int Comandos_F2::Iniciar_Sesion(QString user, QString pass, QString id_part){
    if(usuario.sesion=='0'){

        if(QString::compare("root",user)==0 && QString::compare("123",pass)==0){
            //es usuario root

            if(Buscar_Montada(id_part)){
                usuario.sesion='1';
                QByteArray array = user.toLocal8Bit();
                char* nom = array.data();
                strcpy(usuario.l_usuario,nom);
                array = pass.toLocal8Bit();
                char* p = array.data();
                strcpy(usuario.l_pass,p);
                usuario.id_part=id_part;
                usuario.id_grupo=1;
                usuario.id_user=1;
                usuario.permisos=777;
                cout<<"Sesion Iniciada con exito"<<endl;
            }
        }else{
            Buscar_Usuario(id_part,user,pass);
            return 0;
        }

    }else{
        cout<<"INFO::Existe una sesion activa, cierre la sesion para continuar"<<endl;
        return 0;
    }
    return 0;
}




int Comandos_F2::Buscar_Usuario(QString id_particion,QString nombre_usuario,QString pass){

    if(Buscar_Montada(id_particion)){
        QString ruta="/user.txt";
        QStringList lista=ruta.split("/");

        FILE *archivo;
        archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

        if(archivo==NULL){
            cout<<"Error: No se encontro el disco, por lo que no se puede crear carpeta o archivo"<<endl;
            return 0;
        }

        superBloque tem_sb;
        fseek(archivo,tem_part.part_start,SEEK_SET);
        fread(&tem_sb,sizeof(superBloque),1,archivo);
        fclose(archivo);

        if(tem_sb.magic==201701089){



            int inodo_encontrado=Buscar_Carpeta_Archivo(0,tem_sb,lista,1);//devuelve el inodo de lo que quiere agregar

            if(inodo_encontrado>=0){

                int salida=Recorrer_Bloque_Archivos(inodo_encontrado,tem_sb);
                if(salida==0){
                        Usuario_Sesion(nombre_usuario,pass,this->contenidos_archivos,id_particion);
                        this->contenidos_archivos="";
                }

            }else{
                cout<<"No se encontro el archivo especificado para hacer el inicio de sesion"<<endl;
                return 0;
            }


        }else if(tem_sb.magic==2017){
            cout<<"La particion sufrio un problema no se pueden mostrar archivos"<<endl;
            return 0;
        }

    }else{
        cout<<"La particion no se encuentra montada"<<endl;
        return 0;
    }

}

int Comandos_F2::Usuario_Sesion(QString nombre_usuario,QString pass,QString contenido,QString id_part){
    QStringList registro=contenido.split("\\n");

    bool no_existe=false;
    QString nombre_grupo="";

    for(int i=0;i<registro.size()-1;i++){

        QString registroN=registro.at(i);

        QStringList campos=registroN.split(",");

        QString numero=campos.at(0);
        QString is_usuario=campos.at(1);

        if(QString::compare(is_usuario,"U")==0){

            QString nombre_user=campos.at(2);
            nombre_grupo=campos.at(3);
            QString pass_user=campos.at(4);

            if(QString::compare(nombre_user,nombre_usuario)==0 && QString::compare(pass_user,pass)==0){
                usuario.sesion='1';
                QByteArray array = nombre_user.toLocal8Bit();
                char* nom = array.data();
                strcpy(usuario.l_usuario,nom);
                array = pass.toLocal8Bit();
                char* p = array.data();
                strcpy(usuario.l_pass,p);
                usuario.id_part=id_part;
                usuario.id_user=numero.toInt();
                usuario.permisos=664;
                cout<<"Sesion Iniciada con exito"<<endl;
                no_existe=true;
            }

        }

    }
    if(no_existe){
        for(int i=0;i<registro.size()-1;i++){
            QString registroN=registro.at(i);

            QStringList campos=registroN.split(",");

            QString numero=campos.at(0);
            QString is_usuario=campos.at(1);

            if(QString::compare(is_usuario,"G")==0){
                QString nombreG=campos.at(2);
                if(QString::compare(nombre_grupo,nombreG)==0){
                    usuario.id_grupo=numero.toInt();
                }
            }
        }
    }else{
        cout<<"El usuario no existe en la particion, no se puede iniciar sesion"<<endl;
        return -1;
    }
   return 0;
}






int Comandos_F2::Ejecutar_Logout(){
    if(usuario.sesion=='1'){
        usuario.sesion='0';
        strcpy(usuario.l_usuario,"");
        strcpy(usuario.l_pass,"");
        usuario.id_part="";
        usuario.id_user=-1;
        usuario.id_grupo=-1;
        cout<<"INFO:: Sesion cerra con exito"<<endl;
    }else{
        cout<<"ERROR:: No existen sesiones activas en el sistema"<<endl;
    }
    return 0;
}

int Comandos_F2::Ejecutar_Mkdir(Nodo *dir){
    this->comando_ejec="";
    QString ruta="";
    bool parametro_p=false;
    bool parametro_ruta=false;
    bool error_general=false;
    this->comando_ejec="mkdir";
    for(int i=0;i<dir->hijos.size();i++){
        Nodo tem=dir->hijos.at(i);
        int tipo_=tem.tipo_;
        switch (tipo_) {
        case path:
            ruta=Quitar_Comillas(tem.valor);
            parametro_ruta=true;
            this->comando_ejec=this->comando_ejec+" -path="+tem.valor;
            break;

        case PP:
            parametro_p=true;
            this->comando_ejec=this->comando_ejec+" -p";
            break;
        default:
            cout<<"Parametro no valido en el comando mkdir"<<endl;
            break;
        }
    }
    if(!error_general){
       if(parametro_ruta){
            Setear_Inicio(ruta,parametro_p,0,"",-1,0);
       }else{
           cout<<"ERROR:: no se puede ejecutar mkdir ya que no se especifico una ruta"<<endl;
           return 0;
       }

    }
    return 0;
}

int Comandos_F2::Setear_Inicio(QString ruta, bool parametro_p, int tipo_operacion,QString ruta_count,int size,int size_o_cont){
    QStringList lista=ruta.split("/");


    //aca debo acceder a la particion montada, en sesion activa

    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: No se encontro el disco, por lo que no se puede crear carpeta o archivo"<<endl;
        return 0;
    }

    superBloque tem_sb;
    fseek(archivo,tem_part.part_start,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);
    fclose(archivo);

    if(tem_sb.magic==201701089){
        int inodo_crear_Archivo=-1;//ultimo inodo accedido
        QString nombre_Archivo="";

        int inicio_bmi=0;
        if(tipo_operacion==0){//crear carpeta
            this->indirecto_archivo_carpeta=0;
            int salida=Crear_Carpeta(lista,parametro_p,tem_sb,inicio_bmi,1);//mando 1 para ignorar el espacio vacio
            if(salida>=0){
                //mando a escribir en jor
                Escribir_Jor();
            }
            return 0;
        }else if(tipo_operacion==1){//crear archivo
            QStringList lis_ar;
            for(int i=0;i<lista.size()-1;i++){
                lis_ar.append(lista.at(i));
            }
            nombre_Archivo=lista.at(lista.size()-1);
            this->indirecto_archivo_carpeta=0;
            inodo_crear_Archivo=Crear_Carpeta(lis_ar,parametro_p,tem_sb,inicio_bmi,1);//mando 1 para ignorar el espacio vacio

            if(inodo_crear_Archivo!=-1){
                this->indirecto_archivo_carpeta=1;
                int salida=Crear_Archivo(inodo_crear_Archivo,tem_sb,nombre_Archivo,ruta_count,size,size_o_cont);
                if(salida>=0){
                    //mando a escribir en jor
                    Escribir_Jor();
                }
                return 0;
            }else{
                cout<<"El archivo no se puede crear debido a que sus padres no existe"<<endl;
            }

        }
    }else if(tem_sb.magic==2017){
        cout<<"La particion sufrio una perdida, ejecute el comando recovery si es posible"<<endl;
        return 0;
    }else{
        cout<<"La particion aun no ha sido formateada, debo ejecutar el formateo para realizar operaciones"<<endl;
        return 0;
    }



    return 0;
}

inodo Comandos_F2::Crear_Inodo(int bloque_libre){

    inodo nuevo_i;
    nuevo_i.usuario_id=usuario.id_user;//cambiar
    nuevo_i.grupo_id=usuario.id_grupo;//cambiar
    nuevo_i.crear_time=time(0);
    nuevo_i.modif_time=time(0);
    nuevo_i.permiso=usuario.permisos;//cambiar
    nuevo_i.size_file=sizeof(carpeta);
    nuevo_i.type_file='0';
    nuevo_i.ulti_time=time(0);
    for(int i=0;i<15;i++){
        nuevo_i.apuntador[i]=-1;
    }
    nuevo_i.apuntador[0]=bloque_libre;

    return nuevo_i;
}

carpeta Comandos_F2::Crear_Bloque_C(int padre,int actual){
    carpeta nuevoC;
    nuevoC.contenido[0].apuntador_inodo=actual;
    strcpy(nuevoC.contenido[0].nombre_dir_file,".");

    nuevoC.contenido[1].apuntador_inodo=padre;
    strcpy(nuevoC.contenido[1].nombre_dir_file,"..");

    nuevoC.contenido[2].apuntador_inodo=-1;
    strcpy(nuevoC.contenido[2].nombre_dir_file,"--");

    nuevoC.contenido[3].apuntador_inodo=-1;
    strcpy(nuevoC.contenido[3].nombre_dir_file,"--");
    return nuevoC;
}

superBloque Comandos_F2::Cambiar_Libre_Inodo(superBloque sb){
    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    char estado='1';
    int apuntador=-1;
    apuntador=sb.bm_start_inodo+(sb.primer_inodo_free);
    fseek(archivo,apuntador,SEEK_SET);
    fwrite(&estado,sizeof(char),1,archivo);
    estado='0';
    fseek(archivo,apuntador,SEEK_SET);
    for(int i=sb.primer_inodo_free;i<sb.inodes_count;i++){
        fread(&estado,sizeof(char),1,archivo);
        if(estado=='0'){
            sb.primer_inodo_free=i;
            sb.free_inodes_count--;
            break;
        }
        apuntador++;
        fseek(archivo,apuntador,SEEK_SET);
    }
    fclose(archivo);
    return sb;
}


superBloque Comandos_F2::Cambiar_Libre_Bloque(superBloque sb){
    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    char estado='1';
    int apuntador=-1;

    estado='1';
    apuntador=sb.bm_start_bloque+(sb.primer_bloque_free);
    fseek(archivo,apuntador,SEEK_SET);
    fwrite(&estado,sizeof(char),1,archivo);
    estado='0';
    fseek(archivo,apuntador,SEEK_SET);

    for(int i=sb.primer_bloque_free;i<sb.bloques_count;i++){
        fread(&estado,sizeof(char),1,archivo);
        if(estado=='0'){
            sb.primer_bloque_free=i;
            sb.free_bloques_count--;
            break;
        }
        apuntador++;
        fseek(archivo,apuntador,SEEK_SET);
    }
    fclose(archivo);
    return sb;
}



int Comandos_F2::Crear_Carpeta(QStringList rutas, bool parametro_p, superBloque sb, int pos_bmi, int pos){
    //este metodo al final va a retornar
    //el ultimo inodo que se creo o se encontro

    if(pos>rutas.size()-1){return pos_bmi;} //retorno todo;

    int pos_bloque_con_espacio=-1;
    int pos_inodo_con_espacio=-1;
    int bloque_con_espacio=-1;
    int inodo_con_espacio=-1;
    int padre=-1;
    int bloque=-1;




    int existe_indirecto=-1;
    int posicion_indirecto=-1;

    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    fseek(archivo,tem_part.part_start,SEEK_SET);
    fread(&sb,sizeof(superBloque),1,archivo);
    //buscar el inodo y recorrer los apuntadores
    inodo tem_inodo;
    carpeta tem_car;
    bloque_pointers tem_pon;

    int apuntador=sb.start_inodo+(pos_bmi*sb.size_inodo);

    fseek(archivo,apuntador,SEEK_SET);
    fread(&tem_inodo,sizeof(inodo),1,archivo);
    QByteArray array = rutas[pos].toLocal8Bit();
    char* nom_carpeta = array.data();
    bool encontre=false;
    bool encontre_inodo=false;
    padre=pos_bmi;

    for(int i=0;i<15;i++){
        //Verificar si es inodo archivo o carpeta?
        if(i<12){//********************************** APUNTADORES DIRECTOS******************************
            if(tem_inodo.apuntador[i]!=-1){

                int pos_bloque=tem_inodo.apuntador[i];
                int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);

                //aca seria bloque de carpetas
                fseek(archivo,apuntador,SEEK_SET);
                fread(&tem_car,sizeof(carpeta),1,archivo);

                for(int j=0;j<4;j++){
                    if(tem_car.contenido[j].apuntador_inodo!=-1){
                        //existe la carpeta lo mando de manera recursiva
                        if(strcasecmp(tem_car.contenido[j].nombre_dir_file,nom_carpeta)==0){
                            pos++;
                            fclose(archivo);
                           // encontre=true;
                            return Crear_Carpeta(rutas,parametro_p,sb,tem_car.contenido[j].apuntador_inodo,pos);

                        }
                    }else{
                        if(bloque_con_espacio==-1){
                            bloque_con_espacio=pos_bloque;
                            pos_bloque_con_espacio=j;
                            inodo_con_espacio=pos_bmi; pos_inodo_con_espacio=i;
                        }
                    }
                }

            }else{
                if(inodo_con_espacio==-1){
                    inodo_con_espacio=pos_bmi; pos_inodo_con_espacio=i;
                    bloque_con_espacio=-1; pos_bloque_con_espacio=0;
                    encontre_inodo=true;
                }
            }
        }else{//************************************ APUNTADORES INDIRECTOS************************************
            posicion_indirecto=i;
            if(tem_inodo.apuntador[i]!=-1){
                int pos_bloque=tem_inodo.apuntador[i];
                int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);

                //aca seria bloque de apuntadores
                fseek(archivo,apuntador,SEEK_SET);
                fread(&tem_pon,sizeof(bloque_pointers),1,archivo);

                for(int j=0;j<16;j++){
                    if(tem_pon.apuntador[j]!=-1){//aca obtengo la posicion del bloque al que apunta
                        //se mando las posicion del inodo

                        existe_indirecto=Buscar_Indirectos(rutas,parametro_p,sb,i,pos,tem_pon.apuntador[j],archivo);
                        if(existe_indirecto>-1){
                           // encontre=true;
                            return existe_indirecto;//retonar un inodo
                        }
                    }
                }
            }
        }

    }
    //si no se encontro el inodo y sus respectivos bloques
    if(!encontre){

        if(pos==rutas.size()-1){//pos==tamanio_lista_rutas
            //creo el inodo y bloque
            if(bloque_con_espacio!=-1){//existe un bloque que tiene espacio

                Setear_Variables(pos_bloque_con_espacio,pos_inodo_con_espacio,bloque_con_espacio,inodo_con_espacio,padre,bloque);
                fclose(archivo);
                pos_bmi=Operar_Carpeta(sb,nom_carpeta,0);
                cout<<"INFO:: La(s) carpetas fueron creadas con exito"<<endl;
                Reset_Variables();
                return pos_bmi;
            }else if(inodo_con_espacio!=-1 && encontre_inodo){//no existe bloque con espacio, solo existe espacio en el inodo
                Setear_Variables(pos_bloque_con_espacio,pos_inodo_con_espacio,bloque_con_espacio,inodo_con_espacio,padre,bloque);
                this->bloque=sb.primer_bloque_free;
                //creo un bloque
                apuntador=sb.start_bloque+(this->bloque*sb.size_bloque);
                this->bloque_con_espacio=this->bloque;
                this->pos_bloque_con_espacio=0;

                tem_car=Crear_Bloque_C(-1,-1);

                fseek(archivo,apuntador,SEEK_SET);
                fwrite(&tem_car,sizeof(carpeta),1,archivo);
                sb=Cambiar_Libre_Bloque(sb);
                fclose(archivo);
                pos_bmi=Operar_Carpeta(sb,nom_carpeta,1);//hace lo mismo de arriba y lo enlaza ya con el bloque que recie cree y enlazar el bloque creado con el inodo con espacio
                //aca muere
                Reset_Variables();
                cout<<"INFO:: La(s) carpetas fueron creadas con exito"<<endl;
                return pos_bmi;

            }else if(posicion_indirecto!=-1){
                //tengo que crearla y muere
                pos_bmi=Crear_Indrecto(sb,tem_inodo,nom_carpeta,pos_bmi);
                if(pos_bmi<0){
                    return -1;
                }
                return pos_bmi;
            }
            else{
                cout<<"INFO::ya no se pueden crear mas archivos por espacio en:"<<nom_carpeta<<endl;
                return -1;
            }


        }else if(parametro_p && pos!=rutas.size()-1){

            if(bloque_con_espacio!=-1 ){
                Setear_Variables(pos_bloque_con_espacio,pos_inodo_con_espacio,bloque_con_espacio,inodo_con_espacio,padre,bloque);
                fclose(archivo);
                pos_bmi=Operar_Carpeta(sb,nom_carpeta,0);
                //tengo que mandar
                pos++;
                Reset_Variables();
                return Crear_Carpeta(rutas,parametro_p,sb,pos_bmi,pos);

            }else if(inodo_con_espacio!=-1 && encontre_inodo){
                Setear_Variables(pos_bloque_con_espacio,pos_inodo_con_espacio,bloque_con_espacio,inodo_con_espacio,padre,bloque);
                this->bloque=sb.primer_bloque_free;
                apuntador=sb.start_bloque+(this->bloque*sb.size_bloque);
                this->bloque_con_espacio=this->bloque;
                this->pos_bloque_con_espacio=0;
                tem_car=Crear_Bloque_C(-1,-1);
                sb=Cambiar_Libre_Bloque(sb);
                fseek(archivo,apuntador,SEEK_SET);
                fwrite(&tem_car,sizeof(carpeta),1,archivo);
                fclose(archivo);
                pos_bmi=Operar_Carpeta(sb,nom_carpeta,1);
                //vuelvo a mandar
                pos++;
                Reset_Variables();
                return Crear_Carpeta(rutas,parametro_p,sb,pos_bmi,pos);

            }else if(posicion_indirecto!=-1){
                fclose(archivo);
                pos_bmi=Crear_Indrecto(sb,tem_inodo,nom_carpeta,pos_bmi);
                pos++;
                Reset_Variables();
                if(pos_bmi<0){
                    return -1;
                }
                return Crear_Carpeta(rutas,parametro_p,sb,pos_bmi,pos);
            }
            else{
                cout<<"INFO:: ya no se pueden creas mas archivos por espacio: "<<nom_carpeta<<endl;
                return -1;
            }
        }
        else{
            cout<<"ERROR::No se puede crear la carpeta, debido a que sus padres no existe y no se especifico el parametro p"<<endl;
            return -1;
        }
    }
    return -1;
}

int Comandos_F2::Operar_Carpeta(superBloque sb, char *nom_carpeta,int enlace){
    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    int apuntador=0;
    int apuntador_inodo=-1;
    carpeta tem_car;
    inodo tem_inodo;

    apuntador=sb.start_bloque+(this->bloque_con_espacio*sb.size_bloque);
    fseek(archivo,apuntador,SEEK_SET);
    fread(&tem_car,sizeof(carpeta),1,archivo);

    // espacio del bloque libre le seteo el primer inodo libre y escribo
    tem_car.contenido[this->pos_bloque_con_espacio].apuntador_inodo=sb.primer_inodo_free;
    strcpy(tem_car.contenido[this->pos_bloque_con_espacio].nombre_dir_file,nom_carpeta);
    fseek(archivo,apuntador,SEEK_SET);
    fwrite(&tem_car,sizeof(carpeta),1,archivo);

    //donde voy a escribir el nuevo inodo
    apuntador_inodo=sb.primer_inodo_free;

    apuntador=sb.start_inodo+(sb.primer_inodo_free*sb.size_inodo);
    tem_inodo=Crear_Inodo(sb.primer_bloque_free);
    fseek(archivo,apuntador,SEEK_SET);
    fwrite(&tem_inodo,sizeof(inodo),1,archivo);

    apuntador=sb.start_bloque+(sb.primer_bloque_free*sb.size_bloque);
    tem_car=Crear_Bloque_C(this->padre,sb.primer_inodo_free);
    fseek(archivo,apuntador,SEEK_SET);
    fwrite(&tem_car,sizeof(carpeta),1,archivo);


    //cambio libre inodo y bloque
    sb=Cambiar_Libre_Inodo(sb);
    sb=Cambiar_Libre_Bloque(sb);
    fseek(archivo,tem_part.part_start,SEEK_SET);
    fwrite(&sb,sizeof(superBloque),1,archivo);//los cambios del super bloque ya estan aca

    if(enlace==1){
        apuntador=sb.start_inodo+(this->inodo_con_espacio*sb.size_inodo);
        fseek(archivo,apuntador,SEEK_SET);
        fread(&tem_inodo,sizeof(inodo),1,archivo);

        tem_inodo.apuntador[this->pos_inodo_con_espacio]=this->bloque;

        fseek(archivo,apuntador,SEEK_SET);
        fwrite(&tem_inodo,sizeof(inodo),1,archivo);
    }else if(enlace==2){
        apuntador=sb.start_bloque+(this->block_indirecto*sb.size_bloque);
        bloque_pointers punt;
        fseek(archivo,apuntador,SEEK_SET);
        fread(&punt,sizeof(bloque_pointers),1,archivo);
        punt.apuntador[this->pos_block_indirecto]=this->bloque;
        this->block_indirecto=-1; this->pos_block_indirecto=-1;
        fseek(archivo,apuntador,SEEK_SET);
        fwrite(&punt,sizeof(bloque_pointers),1,archivo);
    }

    fclose(archivo);
    return apuntador_inodo;
}


int Comandos_F2::Ejecutar_Mkfile(Nodo *arc){
    this->comando_ejec="mkfile";
    bool parametro_p=false;
    bool is_path=false;
    bool is_cont=false;
    int is_size=0;
    QString ruta="";
    QString ruta_cont="";
    bool error_general=false;
    for(int i=0;i<arc->hijos.size();i++){
        Nodo tem=arc->hijos.at(i);
        int tipo_=tem.tipo_;
        switch (tipo_) {
        case path:
            this->comando_ejec=this->comando_ejec+" -path="+tem.valor;
            ruta=Quitar_Comillas(tem.valor);
            is_path=true;
            break;

        case PP:
            parametro_p=true;
            this->comando_ejec=this->comando_ejec+" -p";
            break;

        case size:
            this->comando_ejec=this->comando_ejec+" -size="+tem.valor;
            is_size=tem.valor.toInt();
            this->global_size=is_size;
            break;

        case Cont:
            this->comando_ejec=this->comando_ejec+" -cont="+tem.valor;
            ruta_cont=Quitar_Comillas(tem.valor);
            this->global_cont=ruta_cont;
            is_cont=true;
            break;
        default:
            error_general=true;
            cout<<"ERROR:: Parametro no valido para mkfile "<<tem.valor.toStdString()<<endl;
            break;
        }

    }
    if(!error_general){
        if(is_path){
            if(is_cont){//solo se toma cont
                Setear_Inicio(ruta,parametro_p,1,ruta_cont,is_size,0);
            }else if(is_size>=0){//se toma size
                Setear_Inicio(ruta,parametro_p,1,ruta_cont,is_size,1);
            }else{
                cout<<"ERROR:: No se especeficio el cont o size en mkfile"<<endl;
            }
        }else{
            cout<<"ERROR:: No se especificio la ruta para crear el archivo"<<endl;
            return 0;
        }
    }
    return 0;
}

int Comandos_F2::Crear_Archivo(int pos_bmi,superBloque sb,QString nombre_ar,QString ruta_cont,int size,int tipo){

    this->pos_bloque_con_espacio=-1;
    this->pos_inodo_con_espacio=-1;
    this->bloque_con_espacio=-1;
    this->inodo_con_espacio=-1;
    this->padre=-1;
    this->bloque=-1;
    this->tipo_op_archivo=-1;
    this->tipo_op_archivo=tipo;

    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    fseek(archivo,tem_part.part_start,SEEK_SET);
    fread(&sb,sizeof(superBloque),1,archivo);

    inodo tem_inodo;
    carpeta tem_car;
    int apuntador=sb.start_inodo+(pos_bmi*sb.size_inodo);

    fseek(archivo,apuntador,SEEK_SET);
    fread(&tem_inodo,sizeof(inodo),1,archivo);
    QByteArray array = nombre_ar.toLocal8Bit();
    char* nom_carpeta = array.data();

    bool encontre_espacio=false;
    bool indirecto=false;
    bool encontre_inodo=false;

    for(int i=0;i<15;i++){
        if(i<12){

            if(tem_inodo.apuntador[i]!=-1){

                int pos_bloque=tem_inodo.apuntador[i];
                int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);

                //aca seria bloque de carpetas
                fseek(archivo,apuntador,SEEK_SET);
                fread(&tem_car,sizeof(carpeta),1,archivo);

                for(int j=0;j<4;j++){//verificar que no exista en la raiz
                    if(tem_car.contenido[j].apuntador_inodo==-1){
                        if(this->bloque_con_espacio==-1){
                            this->bloque_con_espacio=pos_bloque;
                            this->pos_bloque_con_espacio=j;
                            encontre_espacio=true;
                            break;
                        }
                    }else{
                        if(strcasecmp(tem_car.contenido[j].nombre_dir_file,nom_carpeta)==0){
                            cout<<"El archivo ya existe"<<endl;
                            fclose(archivo);
                            return 0;
                        }
                    }
                }

            }else{

              if(this->inodo_con_espacio==-1){
                  this->inodo_con_espacio=pos_bmi;
                  this->pos_inodo_con_espacio=i;
                  encontre_inodo=true;

              }
            }
            if(encontre_espacio){
                break;
            }

        }else{
            indirecto=true;
            if(tem_inodo.apuntador[i]!=-1){
                QStringList ruta_lista;
                ruta_lista.append(nombre_ar);

                int Regreso=Buscar_Indirectos(ruta_lista,true,sb,i,0,tem_inodo.apuntador[i],archivo);
                if(Regreso==-2){
                    cout<<"El archivo ya existe"<<endl;
                    fclose(archivo);
                    return 0;
                }
            }

        }
    }

    if(encontre_espacio){
        fclose(archivo);
        Operar_Archivo(sb,nom_carpeta,size,ruta_cont);
        return 0;
    }else if(encontre_inodo==true && this->inodo_con_espacio!=-1){
        tem_inodo.apuntador[this->pos_inodo_con_espacio]=sb.primer_bloque_free;
        apuntador=sb.start_inodo+(this->inodo_con_espacio*sb.size_inodo);
        fseek(archivo,apuntador,SEEK_SET);
        fwrite(&tem_inodo,sizeof(inodo),1,archivo);

        tem_car=Crear_Bloque_C(-1,-1);
        this->bloque_con_espacio=sb.primer_bloque_free;
        this->pos_bloque_con_espacio=0;
        apuntador=sb.start_bloque+(sb.primer_bloque_free*sb.size_bloque);
        fseek(archivo,apuntador,SEEK_SET);
        fwrite(&tem_car,sizeof(carpeta),1,archivo);
        sb=Cambiar_Libre_Bloque(sb);
        fclose(archivo);
        Operar_Archivo(sb,nom_carpeta,size,ruta_cont);
    }else{//aqui si es el indirecto
        fclose(archivo);
        Crear_Indrecto(sb,tem_inodo,nom_carpeta,pos_bmi);
    }
    return 0;
}

superBloque Comandos_F2::Crear_Bloque_A(int tamanio,inodo tem_i,superBloque sb,QString ruta_contenido,int pos_inodo){
    this->valor="";

    //verificar cont
    bool cont=false;
    char *aa=" ";
    QString datos_arc="";
    this->indirecto_archivo_carpeta=3;
    if(QString::compare(ruta_contenido,"")!=0){
        //como es distinto mando a buscar el archivo
        QFile contenido(ruta_contenido);

        if (contenido.open(QIODevice::ReadOnly|QIODevice::Text)) {
           datos_arc=QString::fromUtf8(contenido.readAll());
           tamanio=datos_arc.size();
           this->global_size=tamanio;
            cont=true;
            contenido.close();
        }else{
            cout<<"ERROR:: NO se puede crear el archivo con el comando cont porque la ruta no existe"<<endl;
            return sb;
        }
    }

    //crear solo con size
    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    int apuntador=0;
    QString valores="";


    char numero[11] ="0123456789";
    int contador=0;
    int posicion=0;
    bool pendiente=true;
    int i=0;
    bloque_archivo tem_ar;
    for(int contador_general=0;contador_general<tamanio;contador_general++){
        if(cont){
             valores.append(datos_arc.at(contador_general)) ;
        }else{
            valores.append(QChar(numero[i]));

            if(i==9){
                i=0;
            }else{
                i++;
            }
        }


        if(contador==63){
            contador=0;
            //aca creo
            if(posicion<12){
                apuntador=sb.start_bloque+(sb.primer_bloque_free*sb.size_bloque);
                strcpy(tem_ar.contenido,valores.toStdString().c_str());//copio
                fseek(archivo,apuntador,SEEK_SET);
                fwrite(&tem_ar,sizeof(bloque_archivo),1,archivo);
                tem_i.apuntador[posicion]=sb.primer_bloque_free;
                sb=Cambiar_Libre_Bloque(sb);
                pendiente=false;
                posicion++;

                //limpio
                 valores="";
                 strcpy(tem_ar.contenido,"");
                 i=0;
            }else{
                //aca mando a escribir el super bloque
                this->valor=valores;
                int val=Crear_Indrecto(sb,tem_i,aa,pos_inodo);
                posicion=this->posicion_inodo_archivo;
                sb=this->global_sb;
                if(val==10){
                    pendiente=false;
                    valores="";
                    strcpy(tem_ar.contenido,"");
                    i=0;
                }

            }
        }else{
            pendiente=true;
        }

        if(pendiente){
            contador++;
        }else{
            contador=0;
        }
    }


    if(pendiente){
        apuntador=sb.start_bloque+(sb.primer_bloque_free*sb.size_bloque);
        fseek(archivo,apuntador,SEEK_SET);
        strcpy(tem_ar.contenido,valores.toStdString().c_str());
        fwrite(&tem_ar,sizeof(bloque_archivo),1,archivo);
        tem_i.apuntador[posicion]=sb.primer_bloque_free;
        sb=Cambiar_Libre_Bloque(sb);
        pendiente=false;
        posicion++;
        //limpio
         valores="";
         strcpy(tem_ar.contenido,"");
    }else{
        this->valor=valores;
        int val=Crear_Indrecto(sb,tem_i,aa,pos_inodo);
        posicion=this->posicion_inodo_archivo;
        sb=this->global_sb;
        if(val==10){
            pendiente=false;
            valores="";
            strcpy(tem_ar.contenido,"");
            i=0;
        }
    }
    //se escrible el inodo
    apuntador=sb.start_inodo+(sb.primer_inodo_free*sb.size_inodo);
    tem_i.size_file=tamanio;
    fseek(archivo,apuntador,SEEK_SET);
    fwrite(&tem_i,sizeof(inodo),1,archivo);
    fclose(archivo);
    cout<<"El archivo fue creado con exito"<<endl;

    return sb;
}

int Comandos_F2::Operar_Archivo(superBloque sb, char *nom_carpeta, int tamanio,QString ruta){
    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    int apuntador=0;

    carpeta tem_car;

    inodo tem_inodo;

    apuntador=sb.start_bloque+(this->bloque_con_espacio*sb.size_bloque);
    fseek(archivo,apuntador,SEEK_SET);
    fread(&tem_car,sizeof(carpeta),1,archivo);

    // espacio del bloque libre le seteo el primer inodo libre y escribo
    tem_car.contenido[this->pos_bloque_con_espacio].apuntador_inodo=sb.primer_inodo_free;
    strcpy(tem_car.contenido[this->pos_bloque_con_espacio].nombre_dir_file,nom_carpeta);
    fseek(archivo,apuntador,SEEK_SET);
    fwrite(&tem_car,sizeof(carpeta),1,archivo);

    //donde voy a escribir el nuevo inodo

    //se crea el inodo
    apuntador=sb.start_inodo+(sb.primer_inodo_free*sb.size_inodo);
    tem_inodo=Crear_Inodo(sb.primer_bloque_free);
    tem_inodo.type_file='1';
    tem_inodo.size_file=tamanio;

    //crear bloques archivos
    fclose(archivo);

    sb=Crear_Bloque_A(tamanio,tem_inodo,sb,ruta,sb.primer_inodo_free); //crear inodos,tomar en cuenta tos indirectos

    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    //cambio libre inodo y bloque
    sb=Cambiar_Libre_Inodo(sb);

    fseek(archivo,tem_part.part_start,SEEK_SET);
    fwrite(&sb,sizeof(superBloque),1,archivo);

    fclose(archivo);

    return 0;
}

//QStringList rutas, bool parametro_p, superBloque sb, int pos_bmi, int pos(constructor de crear carpetas)
int Comandos_F2::Buscar_Indirectos(QStringList rutas, bool parametro_p, superBloque sb, int indirecto, int pos_ruta, int block, FILE *archivo){
   if(indirecto==12){
       QByteArray array = rutas[pos_ruta].toLocal8Bit();
       char* nom_carpeta = array.data();
       bool encontre=false;
       int apuntador=sb.start_bloque+(block*sb.size_bloque);
       //aca seria bloque de carpetas
       carpeta tem_car;
       fseek(archivo,apuntador,SEEK_SET);
       fread(&tem_car,sizeof(carpeta),1,archivo);
       for(int j=0;j<4;j++){
           if(tem_car.contenido[j].apuntador_inodo!=-1){
               //existe la carpeta lo mando de manera recursiva
               if(strcasecmp(tem_car.contenido[j].nombre_dir_file,nom_carpeta)==0){
                   pos_ruta++;
                   encontre=true;
                   if(this->indirecto_archivo_carpeta==1){
                       return -2;
                   }
                   return Crear_Carpeta(rutas,parametro_p,sb,tem_car.contenido[j].apuntador_inodo,pos_ruta);

               }
           }
       }
       if(!encontre){

           return -1;
       }
   }else if(indirecto==13){
       //en crear carpeta accedo al primer bloque de pointers
       //aca accedo a segundo bloque y estos ya van a tener bloques carpetas y mando restando a indirecto
       int pos_bloque=block;
       int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);
       bloque_pointers tem_pon;
       //aca seria bloque de carpetas
       fseek(archivo,apuntador,SEEK_SET);
       fread(&tem_pon,sizeof(bloque_pointers),1,archivo);

       for(int j=0;j<16;j++){
           if(tem_pon.apuntador[j]!=-1){//aca obtengo la posicion del bloque al que apunta
               //se mando las posicion del inodo
               int regreso= Buscar_Indirectos(rutas,parametro_p,sb,indirecto-1,pos_ruta,tem_pon.apuntador[j],archivo);
               if(regreso>-1){
                   return regreso;//trae una pos inodo
               }
           }
       }

   }else if(indirecto==14){
       //crear carpeta el primer bloque pointers
       //mando crear indirecto y le resto a indirecto para que sea 12
       //luego retorna el inodo
       int pos_bloque=block;
       int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);
       bloque_pointers tem_pon;
       //aca seria bloque de carpetas
       fseek(archivo,apuntador,SEEK_SET);
       fread(&tem_pon,sizeof(bloque_pointers),1,archivo);

       for(int j=0;j<16;j++){
           if(tem_pon.apuntador[j]!=-1){//aca obtengo la posicion del bloque al que apunta
               //se mando las posicion del inodo
               int regreso=Buscar_Indirectos(rutas,parametro_p,sb,indirecto-1,pos_ruta,tem_pon.apuntador[j],archivo);
               if(regreso>-1){
                   return regreso;//trae una pos de inodo
               }

           }
       }
   }
   return -1;
}

void Comandos_F2::Setear_Variables(int n1,int n2,int n3,int n4,int n5,int n6){
    this->pos_bloque_con_espacio=n1;
    this->pos_inodo_con_espacio=n2;
    this->bloque_con_espacio=n3;
    this->inodo_con_espacio=n4;
    this->padre=n5;
    this->bloque=n6;
}

void Comandos_F2::Reset_Variables(){
    this->pos_bloque_con_espacio=-1;
    this->pos_inodo_con_espacio=-1;
    this->bloque_con_espacio=-1;
    this->inodo_con_espacio=-1;
    this->padre=-1;
    this->bloque=-1;
}

//retornar el ultimo inodo creado
int Comandos_F2::Crear_Indrecto( superBloque sb, inodo inodo_padre, char *nom_carpeta,int inode_bm){
    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    if(inodo_padre.apuntador[12]==-1){
        int bloque_l=sb.primer_bloque_free;
        inodo_padre.apuntador[12]=bloque_l;
        bloque_pointers nuevoP;
        for(int i=0;i<16;i++){
            nuevoP.apuntador[i]=-1;
        }
        //escribir apuntadores

        int point=sb.start_bloque+(sb.primer_bloque_free*sb.size_bloque);
        fseek(archivo,point,SEEK_SET);
        fwrite(&nuevoP,sizeof(bloque_pointers),1,archivo);
        sb=Cambiar_Libre_Bloque(sb);

        fclose(archivo);
        int pos_bmi=Crear_Indirectos_Simples(inode_bm,inodo_padre.apuntador[12],sb,nom_carpeta);
        //enlazo el bloque creado aca con el inodo padre
        archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
        this->posicion_inodo_archivo=12;

        point=sb.start_inodo+(inode_bm*sb.size_inodo);
        fseek(archivo,point,SEEK_SET);
        fwrite(&inodo_padre,sizeof(inodo),1,archivo);
        fclose(archivo);
        return pos_bmi;

    }else{
        int pos_bmi=Crear_Indirectos_Simples(inode_bm,inodo_padre.apuntador[12],sb,nom_carpeta);
        this->posicion_inodo_archivo=12;
        //si retornar -1 quiere decir que se debe crear un doble
        if(pos_bmi==-1){
            if(inodo_padre.apuntador[13]==-1){
                int bloque_l=sb.primer_bloque_free;
                inodo_padre.apuntador[13]=bloque_l;
                bloque_pointers nuevoP;
                for(int i=0;i<16;i++){
                    nuevoP.apuntador[i]=-1;
                }
                //escribir apuntadores

                int point=sb.start_bloque+(sb.primer_bloque_free*sb.size_bloque);
                fseek(archivo,point,SEEK_SET);
                fwrite(&nuevoP,sizeof(bloque_pointers),1,archivo);
                sb=Cambiar_Libre_Bloque(sb);
                fclose(archivo);
                int pos_bmi=Crear_Indirectos_Dobles(inode_bm,inodo_padre.apuntador[13],sb,nom_carpeta);
                //enlazo el bloque creado aca con el inodo padre
                archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
                this->posicion_inodo_archivo=13;

                point=sb.start_inodo+(inode_bm*sb.size_inodo);
                fseek(archivo,point,SEEK_SET);
                fwrite(&inodo_padre,sizeof(inodo),1,archivo);
                fclose(archivo);
                return pos_bmi;
            }else{
                int pos_bmi=Crear_Indirectos_Dobles(inode_bm,inodo_padre.apuntador[13],sb,nom_carpeta);
                this->posicion_inodo_archivo=13;
                //si retornar -2 quiere decir que se debe ver el triple
                if(pos_bmi==-2){
                    if(inodo_padre.apuntador[14]==-1){
                        //se crea aca
                        int bloque_l=sb.primer_bloque_free;
                        inodo_padre.apuntador[14]=bloque_l;
                        bloque_pointers nuevoP;
                        for(int i=0;i<16;i++){
                            nuevoP.apuntador[i]=-1;
                        }
                        //escribir apuntadores

                        int point=sb.start_bloque+(sb.primer_bloque_free*sb.size_bloque);
                        fseek(archivo,point,SEEK_SET);
                        fwrite(&nuevoP,sizeof(bloque_pointers),1,archivo);
                        sb=Cambiar_Libre_Bloque(sb);
                        archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
                        fclose(archivo);

                        int pos_bmi=Crear_Indirectos_Triples(inode_bm,inodo_padre.apuntador[14],sb,nom_carpeta);
                        //enlazo el bloque creado aca con el inodo padre
                        this->posicion_inodo_archivo=14;
                        point=sb.start_inodo+(inode_bm*sb.size_inodo);
                        fseek(archivo,point,SEEK_SET);
                        fwrite(&inodo_padre,sizeof(inodo),1,archivo);
                        fclose(archivo);
                        return pos_bmi;
                    }else{
                        int pos_bmi=Crear_Indirectos_Triples(inode_bm,inodo_padre.apuntador[14],sb,nom_carpeta);
                        //si retornar -3 quiere decir que se no existe espacio
                        if(pos_bmi==-3){
                            cout<<"ERROR:: no se puede crear la carpeta debido a que no existe espacio"<<endl;
                            return pos_bmi;
                        }
                    }
                }
                return pos_bmi;
            }
        }
        return pos_bmi;
    }
    fclose(archivo);
    return -1;
}


//retorno mayor a 0 cuando si hizo se hizo bien
//



int Comandos_F2::Crear_Indirectos_Simples(int pos_inodo,int block,superBloque sb,char * nom_carpeta){
    bloque_pointers apuntadores;
    FILE *archivo;
    int point=-1;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    bool existen_simples=false;

    int posicion_carpeta_libre=-1;
    if(this->indirecto_archivo_carpeta<2){

        point=sb.start_bloque+(block*sb.size_bloque); //obtengo bloque
        fseek(archivo,point,SEEK_SET);
        fread(&apuntadores,sizeof(bloque_pointers),1,archivo);
        for(int i=0;i<16;i++){
            if(apuntadores.apuntador[i]!=-1){
                carpeta tem_car;
                int apun=sb.start_bloque+(apuntadores.apuntador[i]*sb.size_bloque);
                fseek(archivo,apun,SEEK_SET);
                fread(&tem_car,sizeof(carpeta),1,archivo);
                for(int j=0;j<4;j++){
                    if(tem_car.contenido[j].apuntador_inodo==-1){
                        if(this->indirecto_archivo_carpeta==0){
                            this->bloque_con_espacio=apuntadores.apuntador[i];
                            this->pos_bloque_con_espacio=j;
                            this->padre=pos_inodo;
                            int pos_bmi=Operar_Carpeta(sb,nom_carpeta,0);
                            return pos_bmi;//retorna el inodo
                        }else if(this->indirecto_archivo_carpeta==1){
                            this->bloque_con_espacio=apuntadores.apuntador[i];
                            this->pos_bloque_con_espacio=j;
                            return Operar_Archivo(sb,nom_carpeta,this->global_size,this->global_cont);
                        }

                    }
                }
            }else{
                if(posicion_carpeta_libre==-1){
                    posicion_carpeta_libre=i;
                }
            }
        }
        if(posicion_carpeta_libre!=-1){
            //mando a crear el bloque de carpeta que se guarda en ese apuntador
                        carpeta tem_car;
                       this->bloque=sb.primer_bloque_free;
                       //creo un bloque
                       point=sb.start_bloque+(this->bloque*sb.size_bloque);
                       this->bloque_con_espacio=bloque;
                       this->pos_bloque_con_espacio=0;

                       tem_car=Crear_Bloque_C(-1,-1);
                       fseek(archivo,point,SEEK_SET);
                       fwrite(&tem_car,sizeof(carpeta),1,archivo);
                       sb=Cambiar_Libre_Bloque(sb);
                       this->padre=pos_inodo;
                       this->block_indirecto=block;
                       this->pos_block_indirecto=posicion_carpeta_libre;
                       fclose(archivo);
                       //ppor aca tengo que verificar que se va a crear archivo o carpeta
                       if(this->indirecto_archivo_carpeta==0){
                           int pos_bmi=Operar_Carpeta(sb,nom_carpeta,2);
                            return pos_bmi; //retorno el inodo donde se crea el bloque de la carpeta nueva
                       }else{
                           archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
                           point=sb.start_bloque+(block*sb.size_bloque);
                           bloque_pointers punt;
                           fseek(archivo,point,SEEK_SET);
                           fread(&punt,sizeof(bloque_pointers),1,archivo);
                           punt.apuntador[posicion_carpeta_libre]=this->bloque;
                           this->block_indirecto=-1; this->pos_block_indirecto=-1;
                           fseek(archivo,point,SEEK_SET);
                           fwrite(&punt,sizeof(bloque_pointers),1,archivo);
                           int pos_bmi=Operar_Archivo(sb,nom_carpeta,this->global_size,this->global_cont);
                           return pos_bmi;
                       }



        }
        fclose(archivo);
        if(existen_simples==false){
            return -1; //quiere decir que tengo que crear otro indirecto,dobles o triple
        }
        return -1;
    }else{
        point=sb.start_bloque+(block*sb.size_bloque); //obtengo bloque
        fseek(archivo,point,SEEK_SET);
        fread(&apuntadores,sizeof(bloque_pointers),1,archivo);

        for(int i=0;i<16;i++){
            if(apuntadores.apuntador[i]==-1){
                bloque_archivo tem_ar;
                point=sb.start_bloque+(sb.primer_bloque_free*sb.size_bloque);
                strcpy(tem_ar.contenido,this->valor.toStdString().c_str());//copio
                fseek(archivo,point,SEEK_SET);
                fwrite(&tem_ar,sizeof(bloque_archivo),1,archivo);
                sb=Cambiar_Libre_Bloque(sb);
                existen_simples=true;
                this->global_sb=sb;
                return 10;
            }
        }
        if(existen_simples==false){
            return -1;
        }
    }

    return -1;
}

int Comandos_F2::Crear_Indirectos_Dobles(int pos_inodo, int block, superBloque sb, char *nom_carpeta){
    bloque_pointers apuntadores;
    FILE *archivo;
    int point=-1;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    int pos_bloque_libre=-1;
    //haciendo dobles
    point=sb.start_bloque+(block*sb.size_bloque);
    fseek(archivo,point,SEEK_SET);
    fread(&apuntadores,sizeof(bloque_pointers),1,archivo);
    for(int i=0;i<16;i++){
        if(apuntadores.apuntador[i]!=-1){
            int pos_bmi=Crear_Indirectos_Simples(pos_inodo,apuntadores.apuntador[i],sb,nom_carpeta); // van a retonar algo
            if(pos_bmi>-1){
                fclose(archivo);
                return pos_bmi;
            }

        }else{
            if(pos_bloque_libre==-1){
                pos_bloque_libre=i;
            }
        }
    }
    //creo un bloque apuntadores ,escribir y leer el super bloque
    if(pos_bloque_libre!=-1){
        bloque_pointers nuevoP;
        for(int i=0;i<16;i++){
            nuevoP.apuntador[i]=-1;
        }
        //escribir apuntadores
        apuntadores.apuntador[pos_bloque_libre]=sb.primer_bloque_free;
        point=sb.start_bloque+(sb.primer_bloque_free*sb.size_bloque);
        fseek(archivo,point,SEEK_SET);
        fwrite(&nuevoP,sizeof(bloque_pointers),1,archivo);

        sb=Cambiar_Libre_Bloque(sb);
        fclose(archivo);
        //cuando mando el sb ya va modificado
        int pos_bmi=Crear_Indirectos_Simples(pos_inodo,apuntadores.apuntador[pos_bloque_libre],sb,nom_carpeta);
        archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
        point=sb.start_bloque+(block*sb.size_bloque);
        fseek(archivo,point,SEEK_SET);
        fwrite(&apuntadores,sizeof(bloque_pointers),1,archivo);
        fclose(archivo);
        return pos_bmi;
    }
    fclose(archivo);
    return -2; //tengo que crear un triple
}

int Comandos_F2::Crear_Indirectos_Triples(int pos_inodo, int block, superBloque sb, char *nom_carpeta){
    bloque_pointers apuntadores;
    FILE *archivo;
    int point=-1;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    int pos_bloque_libre=-1;

    //haciendo dobles
    point=sb.start_bloque+(block*sb.size_bloque);
    fseek(archivo,point,SEEK_SET);
    fread(&apuntadores,sizeof(bloque_pointers),1,archivo);
    for(int i=0;i<16;i++){
        if(apuntadores.apuntador[i]!=-1){
            int pos_bmi=Crear_Indirectos_Dobles(pos_inodo,apuntadores.apuntador[i],sb,nom_carpeta); // van a retonar algo
            if(pos_bmi>-1){
                fclose(archivo);

                return pos_bmi;
            }
        }else{
            if(pos_bloque_libre==-1){
                pos_bloque_libre=i;
            }
        }
    }

    if(pos_bloque_libre!=-1){
        bloque_pointers nuevoP;
        for(int i=0;i<16;i++){
            nuevoP.apuntador[i]=-1;
        }
        //escribir apuntadores
        apuntadores.apuntador[pos_bloque_libre]=sb.primer_bloque_free;
        point=sb.start_bloque+(sb.primer_bloque_free*sb.size_bloque);
        fseek(archivo,point,SEEK_SET);
        fwrite(&nuevoP,sizeof(bloque_pointers),1,archivo);
        sb=Cambiar_Libre_Bloque(sb);
        fclose(archivo);
        int pos_bmi=Crear_Indirectos_Dobles(pos_inodo,apuntadores.apuntador[pos_bloque_libre],sb,nom_carpeta);
        point=sb.start_bloque+(block*sb.size_bloque);
        fseek(archivo,point,SEEK_SET);
        fwrite(&apuntadores,sizeof(bloque_pointers),1,archivo);
        fclose(archivo);
        return pos_bmi;
    }
    fclose(archivo);
    return -3; //ya no existe espacio
}




//Funcion que escribe en el jor
int Comandos_F2::Escribir_Jor(){
    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");


    superBloque sb;

    int inicio=tem_part.part_start+sizeof(superBloque);
    int fin=sb.inodes_count;

    fseek(archivo,tem_part.part_start,SEEK_SET);
    fread(&sb,sizeof(superBloque),1,archivo);

    if(sb.filesystem_type==1){
        journaling tem_j;
        int apuntador=inicio;
        int contador=0;
        fseek(archivo,apuntador,SEEK_SET);
        for(int i=0;i<fin;i++){
            fread(&tem_j,sizeof(journaling),1,archivo);
            if(tem_j.status=='0'){
                //aca se escribe y se sale

                QByteArray array = this->comando_ejec.toLocal8Bit();
                char* nom_carpeta = array.data();
                strcpy(tem_j.operacion,nom_carpeta);
                tem_j.status='1';
                tem_j.fecha=time(0);
                int apuntador=inicio+(contador*sizeof(journaling));
                fseek(archivo,apuntador,SEEK_SET);
                fwrite(&tem_j,sizeof(journaling),1,archivo);
                fclose(archivo);

                return 0;
            }
            contador++;
            apuntador=inicio+(contador*sizeof(journaling));
            fseek(archivo,apuntador,SEEK_SET);
        }
    }
    fclose(archivo);
    return 0;
}


int Comandos_F2::Ejecutar_Loos(Nodo *lo){
    QString id_part="";
    bool is_id=false;
    bool is_error=false;
    for(int i=0;i<lo->hijos.size();i++){
        Nodo tem=lo->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case id:
            id_part=tem.valor;
            is_id=true;
            break;
        default:
            cout<<"Comando no Reconocido para la ejecucion del loss"<<endl;
            is_error=true;
            break;
        }
    }
    if(!is_error){
        if(is_id){
            Comando_Loss(id_part);
        }else{
            cout<<"Falta la instruccion id de la particion a perder"<<endl;
        }
    }
    return 0;
}


int Comandos_F2::Comando_Loss(QString id_part){
    if(Buscar_Montada(id_part)){
        FILE *archivo;
        archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

        superBloque sb;
        int apuntador=tem_part.part_start;
        fseek(archivo,apuntador,SEEK_SET);
        fread(&sb,sizeof(superBloque),1,archivo);

        int final_part=tem_part.part_start+tem_part.part_size;
        int inicio=sb.bm_start_inodo;

        LLenar_Nulos(inicio,final_part,archivo,0);

        sb.primer_bloque_free=0;
        sb.primer_inodo_free=0;
        sb.free_bloques_count=sb.bloques_count;
        sb.free_inodes_count=sb.inodes_count;

        apuntador=tem_part.part_start;

        sb.magic=2017;

        fseek(archivo,apuntador,SEEK_SET);
        fwrite(&sb,sizeof(superBloque),1,archivo);
        fclose(archivo);


        cout<<"La simulacion de perdida se realizo "<<endl;
    }else{
        cout<<"La particion con id="<<id_part.toStdString()<<" No se encuentra montada"<<endl;

    }
    return 0;
}

int Comandos_F2::LLenar_Nulos(int inicio, int final,FILE *archivo,int tipo){
    if(tipo==0){
        char caracter='0';
        fseek(archivo,inicio,SEEK_SET);

        for(int i=inicio;i<final;i++){
            fwrite(&caracter,sizeof(char),1,archivo);
        }
        return 0;
    }else{
        char caracter='0';
        fseek(archivo,inicio,SEEK_SET);
        for(int i=inicio;i<final;i++){
            fwrite(&caracter,sizeof(char),1,archivo);
        }
        return 0;
    }
return 0;
}


int Comandos_F2::Ejecutar_Recovery(Nodo *rec){
    QString id_part="";
    bool is_id=false;
    bool is_error=false;
    for(int i=0;i<rec->hijos.size();i++){
        Nodo tem=rec->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case id:
            id_part=tem.valor;
            is_id=true;
            break;
        default:
            cout<<"Comando no Reconocido para la ejecucion del loss"<<endl;
            is_error=true;
            break;
        }
    }
    if(!is_error){
        if(is_id){
            Comando_Recovery(id_part);
        }else{
            cout<<"Falta la instruccion id de la particion a perder"<<endl;
        }
    }
    return 0;
}

int Comandos_F2::Comando_Recovery(QString id_part){
    if(Buscar_Montada(id_part)){
        FILE *archivo;
        archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

        superBloque sb;
        int apuntador=tem_part.part_start;
        fseek(archivo,apuntador,SEEK_SET);
        fread(&sb,sizeof(superBloque),1,archivo);

        int final_part=tem_part.part_start+tem_part.part_size;
        int inicio=sb.bm_start_inodo;

        sb.magic=201701089;

        LLenar_Nulos(inicio,final_part,archivo,1);


        fseek(archivo,tem_part.part_start,SEEK_SET);
        fwrite(&sb,sizeof(superBloque),1,archivo);
        fclose(archivo);


        Creacion_Carpeta_Raiz(sb);

        archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

        if(archivo==NULL){
            fclose(archivo);
            cout<<"Error: No se encontro el disco"<<endl;
            return 0;
        }
        fseek(archivo,tem_part.part_start,SEEK_SET);
        fread(&sb,sizeof(superBloque),1,archivo);
        fclose(archivo);

        Creacion_Archivo_Usuario(sb);
        archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

        QString datos=Leer_Jor(archivo);

        usuario.sesion='1';
        strcpy(usuario.l_usuario,"root");

        strcpy(usuario.l_pass,"123");
        usuario.id_part=temporal.id_mont;
        usuario.id_grupo=1;
        usuario.id_user=1;
        usuario.permisos=777;

        //datos="login -usr=root -pwd=123 -id=vda1 \n"+datos;

        fclose(archivo);



        QFile file("temp.txt"); //SE CREA UN ARCHIVO TEMPORAL PARA COMPILARLO
            if ( file.open( file.WriteOnly ) ) { //BUFFER PARA EL TEXTO QUE SE DESEA COMPILAR
                 QTextStream stream1( &file );
                  stream1 << datos;
        }

            const char* x = "temp.txt";
            FILE* input = fopen(x, "r");
            yyrestart(input);
            if(yyparse()==0){
                Comandos *ejecutas=new Comandos();
                ejecutas->Leer_Principales(raiz);
                delete ejecutas;
            }else{
                cout<<"Se encontraron errores en el comando especificado"<<endl;
            }

        cout<<"La recuperacion se hizo con exito"<<endl;

    }else{
        cout<<"La particion para la recuperacion no se encuentra montada"<<endl;
    }
    return 0;
}


QString Comandos_F2::Leer_Jor(FILE *archivo){
    QString contenido="";

    superBloque sb;
    journaling jng;

    fseek(archivo,tem_part.part_start,SEEK_SET);
    fread(&sb,sizeof(superBloque),1,archivo);

    int inicioJ=tem_part.part_start+sizeof(superBloque);
    int finJ=sb.inodes_count;
    int apuntador=inicioJ;
    fseek(archivo,apuntador,SEEK_SET);

    int contador=0;
    for(int i=0;i<finJ;i++){
        fread(&jng,sizeof(journaling),1,archivo);
        if(jng.status=='1'){
            QString con(jng.operacion);
            contenido=contenido+"\n"+con;
        }
        contador++;
        apuntador=inicioJ+(contador*sizeof(journaling));
        fseek(archivo,apuntador,SEEK_SET);
    }
    return contenido;
}



int Comandos_F2::Ejecutar_Cat(Nodo *cat){
    bool is_file=false;
    QString ruta_file="";
    bool is_error=false;
    for(int i=0;i<cat->hijos.size();i++){
        Nodo tem=cat->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case File:
            ruta_file=Quitar_Comillas(tem.valor);
            is_file=true;
            break;
        default:
            cout<<"Comando no valido para le ejecucion de cat"<<endl;
            is_error=true;
            break;
        }
    }

    if(!is_error){
        if(is_file){
            Leer_Archivo(ruta_file);
        }else{
            cout<<"Error:: hace falta el comando -file para ejecutar el cat"<<endl;
            return 0;
        }
    }
    return 0;
}

int Comandos_F2::Leer_Archivo(QString ruta){
    QStringList lista=ruta.split("/");

    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: No se encontro el disco, por lo que no se puede crear carpeta o archivo"<<endl;
        return 0;
    }

    superBloque tem_sb;
    fseek(archivo,tem_part.part_start,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);
    fclose(archivo);

    if(tem_sb.magic==201701089){



        int inodo_encontrado=Buscar_Carpeta_Archivo(0,tem_sb,lista,1);

        if(inodo_encontrado>=0){

            int salida=Recorrer_Bloque_Archivos(inodo_encontrado,tem_sb);

            //aca se muestra en pantalla
            if(salida==0){
                cout<<"Contenido de Archivo: "<<ruta.toStdString()<<endl;
                cout<<"=================================================================================="<<endl<<endl;
                cout<<"COntenido: "<<this->contenidos_archivos.toStdString()<<endl;
                cout<<"=================================================================================="<<endl<<endl;
            }
            this->contenidos_archivos="";
        }else{
            cout<<"No se encontro el archivo especificado para hacer el cat"<<endl;
            return 0;
        }


    }else if(tem_sb.magic==2017){
        cout<<"La particion sufrio un problema no se pueden mostrar archivos"<<endl;
        return 0;
    }else{

    }

    return 0;
}



//devuelve el inodo de carpeta o archivo a buscar
int Comandos_F2::Buscar_Carpeta_Archivo(int pos_bmi,superBloque sb, QStringList rutas,int pos){
    if(pos>rutas.size()-1){return pos_bmi;} //retorno todo;


    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");
    fseek(archivo,tem_part.part_start,SEEK_SET);
    fread(&sb,sizeof(superBloque),1,archivo);

    //buscar el inodo y recorrer los apuntadores
    inodo tem_inodo;
    carpeta tem_car;
    bloque_pointers tem_pon;

    int apuntador=sb.start_inodo+(pos_bmi*sb.size_inodo);

    fseek(archivo,apuntador,SEEK_SET);
    fread(&tem_inodo,sizeof(inodo),1,archivo);
    QByteArray array = rutas[pos].toLocal8Bit();
    char* nom_carpeta = array.data();



    for(int i=0;i<15;i++){
        //Verificar si es inodo archivo o carpeta?
        if(i<12){//********************************** APUNTADORES DIRECTOS******************************
            if(tem_inodo.apuntador[i]!=-1){

                int pos_bloque=tem_inodo.apuntador[i];
                int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);

                //aca seria bloque de carpetas
                fseek(archivo,apuntador,SEEK_SET);
                fread(&tem_car,sizeof(carpeta),1,archivo);

                for(int j=0;j<4;j++){
                    if(tem_car.contenido[j].apuntador_inodo!=-1){
                        //existe la carpeta lo mando de manera recursiva
                        if(strcasecmp(tem_car.contenido[j].nombre_dir_file,nom_carpeta)==0){
                            pos++;
                            fclose(archivo);
                           // encontre=true;
                            return Buscar_Carpeta_Archivo(tem_car.contenido[j].apuntador_inodo,sb,rutas,pos);

                        }
                    }
                }

            }
        }else{//************************************ APUNTADORES INDIRECTOS************************************

            if(tem_inodo.apuntador[i]!=-1){
                int pos_bloque=tem_inodo.apuntador[i];
                int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);

                //aca seria bloque de apuntadores
                fseek(archivo,apuntador,SEEK_SET);
                fread(&tem_pon,sizeof(bloque_pointers),1,archivo);

                for(int j=0;j<16;j++){
                    if(tem_pon.apuntador[j]!=-1){//aca obtengo la posicion del bloque al que apunta
                        //se mando las posicion del inodo

                        int salida= Buscar_Indirectos_C_A(rutas,sb,i,pos,tem_pon.apuntador[j],archivo);
                        if(salida>=0){
                            return salida;
                        }
                    }
                }
            }
        }

    }
    return -1;
}


int Comandos_F2::Buscar_Indirectos_C_A(QStringList rutas, superBloque sb, int indirecto, int pos_ruta, int block, FILE *archivo){

    if(indirecto==12){

        if(this->exclusivo_archivos==-1){
            QByteArray array = rutas[pos_ruta].toLocal8Bit();
            char* nom_carpeta = array.data();
            int apuntador=sb.start_bloque+(block*sb.size_bloque);
            //aca seria bloque de carpetas
            carpeta tem_car;
            fseek(archivo,apuntador,SEEK_SET);
            fread(&tem_car,sizeof(carpeta),1,archivo);
            for(int j=0;j<4;j++){
                if(tem_car.contenido[j].apuntador_inodo!=-1){
                    //existe la carpeta lo mando de manera recursiva
                    if(strcasecmp(tem_car.contenido[j].nombre_dir_file,nom_carpeta)==0){
                        pos_ruta++;
                        //fclose(archivo);
                        return Buscar_Carpeta_Archivo(tem_car.contenido[j].apuntador_inodo,sb,rutas,pos_ruta);
                    }
                }
            }
            return -1;
        }else{
            int apuntador=sb.start_bloque+(block*sb.size_bloque);
            bloque_archivo tem_archivo;
            fseek(archivo,apuntador,SEEK_SET);
            fread(&tem_archivo,sizeof(bloque_archivo),1,archivo);

            //leer contenido del archivo y mostrar en consola
            QString conte(tem_archivo.contenido);

            this->contenidos_archivos=this->contenidos_archivos+conte;
        }

    }else if(indirecto==13){
        //en crear carpeta accedo al primer bloque de pointers
        //aca accedo a segundo bloque y estos ya van a tener bloques carpetas y mando restando a indirecto
        int pos_bloque=block;
        int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);
        bloque_pointers tem_pon;
        //aca seria bloque de carpetas
        fseek(archivo,apuntador,SEEK_SET);
        fread(&tem_pon,sizeof(bloque_pointers),1,archivo);

        for(int j=0;j<16;j++){
            if(tem_pon.apuntador[j]!=-1){//aca obtengo la posicion del bloque al que apunta
                //se mando las posicion del inodo
                int regreso= Buscar_Indirectos_C_A(rutas,sb,indirecto-1,pos_ruta,tem_pon.apuntador[j],archivo);
                if(regreso>-1){
                    return regreso;//trae una pos inodo
                }
            }
        }

    }else if(indirecto==14){
        //crear carpeta el primer bloque pointers
        //mando crear indirecto y le resto a indirecto para que sea 12
        //luego retorna el inodo
        int pos_bloque=block;
        int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);
        bloque_pointers tem_pon;
        //aca seria bloque de carpetas
        fseek(archivo,apuntador,SEEK_SET);
        fread(&tem_pon,sizeof(bloque_pointers),1,archivo);

        for(int j=0;j<16;j++){
            if(tem_pon.apuntador[j]!=-1){//aca obtengo la posicion del bloque al que apunta
                //se mando las posicion del inodo
                int regreso= Buscar_Indirectos_C_A(rutas,sb,indirecto-1,pos_ruta,tem_pon.apuntador[j],archivo);
                if(regreso>-1){
                    return regreso;//trae una pos de inodo
                }

            }
        }
    }
    return -1;
}


int Comandos_F2::Recorrer_Bloque_Archivos(int pos_inodo,superBloque sb){

    this->exclusivo_archivos=1;
    this->tamanio_archivo=0;

    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    int apuntador=sb.start_inodo+(pos_inodo*sb.size_inodo);
    inodo tem_inodo;

    fseek(archivo,apuntador,SEEK_SET);
    fread(&tem_inodo,sizeof(inodo),1,archivo);

    bloque_archivo tem_arch;
    if(tem_inodo.type_file==0){
        cout<<"La ruta especificada es una carpeta"<<endl;
        return 2;
    }
    this->tamanio_archivo=tem_inodo.size_file;

    for(int i=0;i<15;i++){
        //Verificar si es inodo archivo o carpeta?
        if(i<12){//********************************** APUNTADORES DIRECTOS******************************
            if(tem_inodo.apuntador[i]!=-1){

                int pos_bloque=tem_inodo.apuntador[i];
                int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);

                //aca seria bloque de carpetas
                fseek(archivo,apuntador,SEEK_SET);
                fread(&tem_arch,sizeof(bloque_archivo),1,archivo);
                QString conte(tem_arch.contenido);

                this->contenidos_archivos=this->contenidos_archivos+conte;

            }
        }else{//************************************ APUNTADORES INDIRECTOS************************************

            if(tem_inodo.apuntador[i]!=-1){
                int pos_bloque=tem_inodo.apuntador[i];
                int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);
                bloque_pointers tem_pon;
                //aca seria bloque de apuntadores
                fseek(archivo,apuntador,SEEK_SET);
                fread(&tem_pon,sizeof(bloque_pointers),1,archivo);

                for(int j=0;j<16;j++){
                    if(tem_pon.apuntador[j]!=-1){//aca obtengo la posicion del bloque al que apunta
                        //se mando las posicion del inodo
                        QStringList vacia;
                        Buscar_Indirectos_C_A(vacia,sb,i,0,tem_pon.apuntador[j],archivo);
                    }
                }
            }
        }

    }
    fclose(archivo);
    this->exclusivo_archivos=-1;
    return 0;
}



//FUncion que se encarga de agregar contenido a los archivos user.txt
int Comandos_F2::Ejecutar_Mkgrp(Nodo *grupo){
    bool is_name=false;
    bool is_error=false;
    QString nombre_grupo="";
    for(int i=0;i<grupo->hijos.size();i++){
        Nodo tem=grupo->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case name:
            nombre_grupo=Quitar_Comillas(tem.valor);
            is_name=true;
            break;
        default:
            cout<<"Se encontro un error al ejecutar mkgrp, comando no valido: "<<tem.valor.toStdString()<<endl;
            is_error=true;
            break;
        }

    }
    if(!is_error){
        if(is_name){
            Agregar_Grupos(nombre_grupo);
        }else{
            cout<<"Error:: hace falta el comando de -name para ejecutar mkgrp"<<endl;
            return 0;
        }
    }
    return 0;
}


int Comandos_F2::Ejecutar_Mkuser(Nodo *usuario){
    QString nombre_usuario="";
    bool is_usuario=false;
    QString pass_usuario="";
    bool is_pass=false;
    QString nombre_grupo="";
    bool is_grupo=false;
    bool is_error=false;
    for(int i=0;i<usuario->hijos.size();i++){
        Nodo tem=usuario->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case usr:
            nombre_usuario=Quitar_Comillas(tem.valor);
            is_usuario=true;
            break;
        case pwd:
            pass_usuario=Quitar_Comillas(tem.valor);
            is_pass=true;
            break;
        case grp:
            nombre_grupo=Quitar_Comillas(tem.valor);
            is_grupo=true;
            break;
        default:
            cout<<"Error en ejecutar mkuser parametros no validos: "<<tem.valor.toStdString()<<endl;
            is_error=true;
            break;
        }

    }
    if(!is_error){
        if(is_usuario && is_pass && is_grupo){
            Agregar_Usuario(nombre_usuario,pass_usuario,nombre_grupo);
        }else{
            cout<<"Error:: parametro faltante en mkuser"<<endl;
            return 0;
        }
    }
    return 0;
}


int Comandos_F2::Agregar_Grupos(QString nombre_grupo){

    if(strcasecmp(usuario.l_usuario,"root")!=0){
        cout<<"No se pueden crear grupos ya que el usuario root no esta logueado"<<endl;
        return 0;
    }

    QString ruta="/user.txt";
    QStringList lista=ruta.split("/");

    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: No se encontro el disco, por lo que no se puede crear carpeta o archivo"<<endl;
        return 0;
    }

    superBloque tem_sb;
    fseek(archivo,tem_part.part_start,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);
    fclose(archivo);

    if(tem_sb.magic==201701089){



        int inodo_encontrado=Buscar_Carpeta_Archivo(0,tem_sb,lista,1);//devuelve el inodo de lo que quiere agregar

        if(inodo_encontrado>=0){

            int salida=Recorrer_Bloque_Archivos(inodo_encontrado,tem_sb);


            if(salida==0){
                QString tapar="";
                int se_puede_crear=Recorrer_Grupos(this->contenidos_archivos,nombre_grupo,tapar,0);

                if(se_puede_crear>=0){
                    QString nuevo_grupo=QString::number(se_puede_crear)+",G,"+nombre_grupo+"\\n";
                    this->contenidos_archivos=this->contenidos_archivos+nuevo_grupo;
                    Agregar_Eliminar_Contenido_Archivos(this->contenidos_archivos,inodo_encontrado,tem_sb);
                    cout<<"Se creo un nuevo grupo: "<<nombre_grupo.toStdString()<<endl;
                    this->contenidos_archivos="";
                }else{

                }
            }

        }else{
            cout<<"No se encontro el archivo especificado para hacer el cat"<<endl;
            return 0;
        }


    }else if(tem_sb.magic==2017){
        cout<<"La particion sufrio un problema no se pueden mostrar archivos"<<endl;
        return 0;
    }else{

    }

       return 0;
}


int Comandos_F2::Agregar_Usuario(QString nombre_usuario, QString password, QString nombre_grupo){
    if(strcasecmp(usuario.l_usuario,"root")!=0){
        cout<<"No se pueden crear grupos ya que el usuario root no esta logueado"<<endl;
        return 0;
    }

    QString ruta="/user.txt";
    QStringList lista=ruta.split("/");

    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: No se encontro el disco, por lo que no se puede crear carpeta o archivo"<<endl;
        return 0;
    }

    superBloque tem_sb;
    fseek(archivo,tem_part.part_start,SEEK_SET);
    fread(&tem_sb,sizeof(superBloque),1,archivo);
    fclose(archivo);

    if(tem_sb.magic==201701089){



        int inodo_encontrado=Buscar_Carpeta_Archivo(0,tem_sb,lista,1);//devuelve el inodo de lo que quiere agregar

        if(inodo_encontrado>=0){

            int salida=Recorrer_Bloque_Archivos(inodo_encontrado,tem_sb);


            if(salida==0){
                int se_puede_crear=Recorrer_Grupos(this->contenidos_archivos,nombre_grupo,nombre_usuario,0);

                if(se_puede_crear>=0){
                    QString nuevo_grupo=QString::number(se_puede_crear)+",U,"+nombre_usuario+","+nombre_grupo+","+password+"\\n";
                    this->contenidos_archivos=this->contenidos_archivos+nuevo_grupo;
                    Agregar_Eliminar_Contenido_Archivos(this->contenidos_archivos,inodo_encontrado,tem_sb);
                    cout<<"Se creo un nuevo usuario: "<<nombre_usuario.toStdString()<<endl;
                    this->contenidos_archivos="";
                }else{

                }
            }

        }else{
            cout<<"No se encontro el archivo especificado para hacer el cat"<<endl;
            return 0;
        }


    }else if(tem_sb.magic==2017){
        cout<<"La particion sufrio un problema no se pueden mostrar archivos"<<endl;
        return 0;
    }else{

    }
    return 0;
}


//es solo para grupos
int Comandos_F2::Recorrer_Grupos(QString con_archivo,QString nombre,QString nombre_usuario,int iniciar){

    QStringList registro=con_archivo.split("\\n");
    if(iniciar==0){
        //split con el caracter de salo
        int contador=1;
        if(QString::compare(nombre_usuario,"")==0){
            for(int i=0;i<registro.size()-1;i++){

                QString registroN=registro.at(i);

                QStringList campos=registroN.split(",");

                QString is_grupo=campos.at(1);
                QString nombre_g=campos.at(2);

                if(QString::compare(is_grupo,"G")==0){

                    if(QString::compare(nombre_g,nombre)==0){
                        cout<<"No se puede crear el grupo ya que existe"<<endl;
                        return -1;
                    }
                    contador++;
                }

            }
            return contador;
        }else{
            contador=1;
            bool existe_usuario=false;
            for(int i=0;i<registro.size()-1;i++){

                QString registroN=registro.at(i);

                QStringList campos=registroN.split(",");

                QString is_grupo=campos.at(1);
                QString nombre_g=campos.at(2);

                if(QString::compare(is_grupo,"G")==0){

                    if(QString::compare(nombre_g,nombre)==0){
                        existe_usuario=true;
                    }

                }

            }

            if(existe_usuario){
                for(int i=0;i<registro.size()-1;i++){

                    QString registroN=registro.at(i);

                    QStringList campos=registroN.split(",");

                    QString is_usuario=campos.at(1);
                    QString nombre_user=campos.at(2);

                    if(QString::compare(is_usuario,"U")==0){

                        if(QString::compare(nombre_user,nombre_usuario)==0){
                            cout<<"El usuario ya existe dentro de particion"<<endl;
                            return -1;
                        }
                        contador++;
                    }

                }

                return contador;
            }else{
                cout<<"El usuario no se puede crear porque no existe el grupo especificado"<<endl;
                return -1;
            }

        }
        return -1;
    }
    return -1;
}



int Comandos_F2::Agregar_Eliminar_Contenido_Archivos(QString contenido_nuevo,int pos_inodo,superBloque sb){



    FILE *archivo;
    archivo=fopen(temporal.disco_ruta.toStdString().c_str(),"rb+");

    int apuntador=sb.start_inodo+(pos_inodo*sb.size_inodo);
    inodo tem_inodo;

    fseek(archivo,apuntador,SEEK_SET);
    fread(&tem_inodo,sizeof(inodo),1,archivo);

    bloque_archivo tem_arch;
    if(tem_inodo.type_file==0){
        cout<<"La ruta especificada es una carpeta"<<endl;
        return 2;
    }
    this->tamanio_archivo=contenido_nuevo.size();
    int contador=0;

    string contenido=contenido_nuevo.toStdString();
    tem_inodo.size_file=this->tamanio_archivo;

    for(int i=0;i<15;i++){
        //Verificar si es inodo archivo o carpeta?
       if(contador<tamanio_archivo){
           if(i<12){//********************************** APUNTADORES DIRECTOS******************************
               if(tem_inodo.apuntador[i]!=-1){

                   int pos_bloque=tem_inodo.apuntador[i];
                   int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);

                   //aca seria bloque de carpetas
                   fseek(archivo,apuntador,SEEK_SET);
                   fread(&tem_arch,sizeof(bloque_archivo),1,archivo);

                   for(int j=0;j<63;j++){
                       if(contador<tamanio_archivo){
                           tem_arch.contenido[j]=contenido[contador];
                           contador++;
                       }else{
                           tem_arch.contenido[j]='\0';
                       }

                   }
                   tem_arch.contenido[63]='\0';
                   apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);
                   fseek(archivo,apuntador,SEEK_SET);
                   fwrite(&tem_arch,sizeof(bloque_archivo),1,archivo);
               }
               else{
                    bloque_archivo nuevo_bloque;
                   for(int j=0;j<63;j++){
                       if(contador<tamanio_archivo){
                           nuevo_bloque.contenido[j]=contenido[contador];
                           contador++;
                       }else{
                           nuevo_bloque.contenido[j]='\0';
                       }

                   }
                   nuevo_bloque.contenido[63]='\0';
                   apuntador=sb.start_bloque+(sb.primer_bloque_free*sb.size_bloque);
                   fseek(archivo,apuntador,SEEK_SET);
                   fwrite(&nuevo_bloque,sizeof(bloque_archivo),1,archivo);

                   tem_inodo.apuntador[i]=sb.primer_bloque_free;


                   sb=Cambiar_Libre_Bloque(sb);


                   apuntador=tem_part.part_start;
                   fseek(archivo,apuntador,SEEK_SET);
                   fwrite(&sb,sizeof(superBloque),1,archivo);
               }
           }else{//************************************ APUNTADORES INDIRECTOS************************************

               if(tem_inodo.apuntador[i]!=-1){
                   int pos_bloque=tem_inodo.apuntador[i];
                   int apuntador=sb.start_bloque+(pos_bloque*sb.size_bloque);
                   bloque_pointers tem_pon;
                   //aca seria bloque de apuntadores
                   fseek(archivo,apuntador,SEEK_SET);
                   fread(&tem_pon,sizeof(bloque_pointers),1,archivo);
               }
           }
       }

    }
    apuntador=sb.start_inodo+(pos_inodo*sb.size_inodo);
    fseek(archivo,apuntador,SEEK_SET);
    fwrite(&tem_inodo,sizeof(inodo),1,archivo);
    fclose(archivo);
    return 0;
}






