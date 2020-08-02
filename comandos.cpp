#include "comandos.h"

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
    Ruta
};


Comandos::Comandos()
{
    segunda_pasada=false;
}

bool Comandos::Existe_archivo(string file){
    FILE * archivo;
     if (archivo = fopen(file.c_str(), "r"))
     {
      fclose(archivo);
      return true;
     }
     else
     {
      return false;
     }
}

int Comandos::Numero_Aleatorio(){
    srand(time(NULL));
    int test = rand() % 1000;
    return test;
}

QString Comandos::Quitar_Comillas(QString v){
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

bool Comandos::Existe_directorio(QString ruta){
    QStringList lista_carpetas=ruta.split("/");

    QString raiz="/home";

    for(int i=2;i<lista_carpetas.size();i++){
        raiz=raiz+"/"+lista_carpetas.at(i);
        DIR *directorio;
        if(directorio=opendir(raiz.toStdString().c_str())){
            closedir(directorio);

        }else{
            if(mkdir(raiz.toStdString().c_str(),0777)==0){
                cout<<"Carpeta creada"<<endl;
            }else{
                return false;
            }
        }

    }
    return true;
}


QString Comandos::Leer_Principales(Nodo *raiz){
    for(int y=0;y<raiz->hijos.size();y++){
        Nodo tem=raiz->hijos.at(y).hijos.at(0);
        int tipo=raiz->hijos.at(y).tipo_;

        switch (tipo) {
        case EXEC:
                //Nodo tem1=raiz->hijos.at(y).hijos.at(0);
               return Leer_Archivo(&tem);

            //break;
         case MKDISK:
                Ejecutar_Mkdisk(&tem);
            break;


        case RMDISK://Eliminar disco
                Ejecutar_Rmdisk(&tem);
            break;



        case FDISK:
                Ejecutar_fdisk(&tem);
            break;

        case MOUNT:
                Ejecutar_Montaje(&tem);
            break;

        case UNMOUNT:
                Ejecutar_Desmontaje(&tem);
            break;

        case REP:
            Ejecutar_Reporte(&tem);
            break;

        default:
            tem=raiz->hijos.at(y);
            Comandos_F2 *comf=new Comandos_F2();

            comf->Leer_Principales(&tem);
            //cout<<"INFO:: Comando principal no valido"<<endl;

            delete comf;

            break;
        }
    }
    return "";
}

QString Comandos::Leer_Archivo(Nodo *ext){
    for(int y=0;y<ext->hijos.size();y++){
        int tipo_=ext->hijos.at(y).tipo_;

        switch (tipo_) {
        case path:
                return Quitar_Comillas(ext->hijos.at(y).valor);
            break;
        default:
            //Error, parametro no permitido
            break;
        }
    }
    return "";
}





QString Comandos::Ejecutar_Mkdisk(Nodo *mk){
    bool is_size=false;
    bool is_path=false;
    int tamanio=0;
    int unidades=1024*1024;
    QString ruta="";
    char c='F';
    QString salida_consola="Mkdisk ";
    bool error_parametros=false;
    for(int i=0;i<mk->hijos.size();i++){
        Nodo tem=mk->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case size:
            try {
                tamanio=tem.valor.toInt();
            } catch (...) {

            }
            salida_consola=salida_consola+"-size="+tem.valor+" ";
            if(tamanio<=0){
                is_size=false;
            }else{
                 is_size=true;
            }

            break;
        case path:
            ruta=Quitar_Comillas(tem.valor);
            is_path=true;
            salida_consola=salida_consola+"-path="+tem.valor+" ";
            break;

        case fit:
            if(QString::compare(tem.valor,"bf",Qt::CaseInsensitive)==0){
                c='B';
            }else if(QString::compare(tem.valor,"ff",Qt::CaseInsensitive)==0){
                c='F';
            }else if(QString::compare(tem.valor,"wf",Qt::CaseInsensitive)==0){
                c='W';
            }else{
                cout<<"Ajuste no valido"<<endl;
                error_parametros=true;
            }
            salida_consola=salida_consola+"-fit="+tem.valor+" ";
            break;

        case unit:
            if(QString::compare(tem.valor,"k",Qt::CaseInsensitive)==0){
                unidades=1024;
            }else if(QString::compare(tem.valor,"m",Qt::CaseInsensitive)==0){
                unidades=1024*1024;
            }else{
                cout<<"Unidades no validas, ingrese k o m"<<endl;
                error_parametros=true;
            }
            salida_consola=salida_consola+"-unit="+tem.valor+" ";
            break;

        default:
            break;
        }
    }
    std::cout<<salida_consola.toStdString()<<endl;
    if(is_size==true && is_path==true  ){

        if(error_parametros==false){
            QStringList lista=ruta.split("/");
            QString value="/home";
            for(int i=2;i<lista.size()-1;i++){//separar nombre y directorio
                value=value+"/"+lista.at(i);
            }

            //verifica o crea el directorio

            if(Existe_directorio(value)==true){
                Crear_disco(tamanio,unidades,c,ruta.toStdString());


                QString nombre_ext=lista.at(lista.size()-1);
                QStringList solo_nombre=nombre_ext.split(".");
                try {
                    QString ruta_raid=value+"/"+solo_nombre.at(0)+"_ra1.disk";
                    Crear_disco(tamanio,unidades,c,ruta_raid.toStdString());
                } catch (...) {
                    cout<<"Error: no se puede crear el raid "<<nombre_ext.toStdString()<<endl;
                }




            }else{
                cout<<"Direccion no valida o no fue posible la creacion del directorio"<<endl;
            }
        }else{
            //Todo: ya se notificaron los errores arriba
        }

    }else{
        cout<<"Falto comando ruta o tamanio que debe ser mayor a 0"<<endl;
    }
    return "";
}

void Comandos::Crear_disco(int tamanio, int unidades, char ajuste, string ruta){
    unidades=tamanio*unidades;
    mbr nuevo_disco;

    nuevo_disco.mbr_tamano=unidades;
    nuevo_disco.disk_fit=ajuste;
    nuevo_disco.mbr_signatura=Numero_Aleatorio();
    nuevo_disco.mbr_fecha=time(0);

    for(int i=0;i<4;i++){
        nuevo_disco.part[i].part_size=0;
        nuevo_disco.part[i].part_status='0';
        nuevo_disco.part[i].part_type='0';
        nuevo_disco.part[i].part_fit='0';
        nuevo_disco.part[i].part_start=unidades;
        //lo unico que no inicio es part_name
    }

    char val[]={'0'};
    FILE *archivo_mkdisk;
    const char *path=ruta.c_str();
    archivo_mkdisk=fopen(path,"wb+");

    //Me coloco al principio del archivo y me voy a mover 0 posiciones
    fseek(archivo_mkdisk,0,SEEK_SET);
    fwrite(val,sizeof(char),1,archivo_mkdisk);//escribo en la primera posicion


    //Me coloco al principio del archivo y me voy a mover unidad-1 posiciones, asi en el ultimo
    //escribo el valor nulo
    fseek(archivo_mkdisk,unidades-1,SEEK_SET);
    fwrite(val,sizeof(char),1,archivo_mkdisk);

    //Ahora si tengo que escribir la estructura inicial del disco
    fseek(archivo_mkdisk,0,SEEK_SET);
    fwrite(&nuevo_disco,sizeof(mbr),1,archivo_mkdisk);

    fclose(archivo_mkdisk);
    cout<<"Disco creado con exito"<<endl;



}


QString Comandos::Ejecutar_Rmdisk(Nodo *rm){
    bool is_path=false;
    QString ruta="";
    QString salida_consola="rmdisk ";
    for(int i=0;i<rm->hijos.size();i++){
        Nodo tem=rm->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case path:
            ruta=Quitar_Comillas(tem.valor);
            salida_consola=salida_consola+"-path="+tem.valor;
            is_path=true;
            break;
        default:
            cout<<"Comando no valido para rmdisk"<<endl;
            break;
        }
    }
    cout<<salida_consola.toStdString()<<endl;

    if(is_path==true){
        Eliminar_disco(ruta.toStdString());
    }else{
        cout<<"No se especifico el comando path en rmdisk por lo que no se puede operar"<<endl;
    }

    return "";
}

void Comandos::Eliminar_disco(string ruta){

    if(Existe_archivo(ruta)){
        remove(ruta.c_str());
        cout<<"Archivo eliminando con exito"<<endl;
    }else{
        cout<<"El archivo no existe"<<ruta<<endl;
    }

}



void Comandos::Ejecutar_fdisk(Nodo *fd){
    bool is_size=false;
    bool is_path=false;
    bool is_name=false;
    bool errores_general=false;
    int tamanio=0;
    int unidades=1024;
    int unidades2=-1;
    QString ruta="";
    QString nombre="";
    char tipo_par='P';
    char ajuste='W';
    int tipo_eliminacion=0;

    bool is_delete=false;

    bool is_add=false;

    for(int i=0;i<fd->hijos.size();i++){
        Nodo tem=fd->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case size:
            try {
                tamanio=tem.valor.toInt();
                is_size=true;
            } catch (...) {
                is_size=false;
            }
            if(tamanio<=0){
                cout<<"El tamanio de la particion debe ser mayor a 0"<<endl;
                errores_general=true;
            }

            break;
        case path:
            ruta=Quitar_Comillas(tem.valor);
            is_path=true;
            break;
        case name:
             nombre=tem.valor;
             is_name=true;
            break;

        case unit:
                if(QString::compare(tem.valor,"b",Qt::CaseInsensitive)==0){
                    unidades=1;
                    unidades2=1;
                }else if(QString::compare(tem.valor,"k",Qt::CaseInsensitive)==0){
                    unidades=1024;
                    unidades2=1024;
                }else if(QString::compare(tem.valor,"m",Qt::CaseInsensitive)==0){
                    unidades=1024*1024;
                    unidades2=unidades;
                }else{
                    cout<<"La unidad especificada es invalida, debe ser b,k,m"<<endl;
                    errores_general=true;
                }
            break;
        case type:
                if(QString::compare(tem.valor,"p",Qt::CaseInsensitive)==0){
                    tipo_par='P';
                }else if(QString::compare(tem.valor,"e",Qt::CaseInsensitive)==0){
                    tipo_par='E';
                }else if(QString::compare(tem.valor,"L",Qt::CaseInsensitive)==0){
                    tipo_par='L';
                }else{
                    cout<<"El tipo de particion es invalida, debe ser p,e,l"<<endl;
                    errores_general=true;
                }
            break;

        case fit:
            if(QString::compare(tem.valor,"bf",Qt::CaseInsensitive)==0){
                ajuste='B';
            }else if(QString::compare(tem.valor,"ff",Qt::CaseInsensitive)==0){
                ajuste='F';
            }else if(QString::compare(tem.valor,"wf",Qt::CaseInsensitive)==0){
                ajuste='W';
            }else{
                cout<<"El ajuste especificado es invalido,debe ser bf,ff,wf"<<endl;
                errores_general=true;
            }
            break;
        case DELETE:
                is_delete=true;
                if(QString::compare(tem.valor,"fast",Qt::CaseInsensitive)==0){
                    tipo_eliminacion=1;
                }else if(QString::compare(tem.valor,"full",Qt::CaseInsensitive)==0){
                    tipo_eliminacion=2;
                }else{
                    cout<<"El comando de delete no es valido"<<endl;
                    is_delete=false;
                }
            break;

        case add:

            try {
                tamanio=tem.valor.toInt();
                is_add=true;
            } catch (...) {
                is_add=false;
            }

            break;
        default:
            break;
        }
    }
    if(is_name && is_path && is_size){
        if(!errores_general){
            Crear_particion(tamanio,unidades,ajuste,tipo_par,ruta,nombre);

            QStringList lista=ruta.split("/");
                        QString value="/home";
                        for(int i=2;i<lista.size()-1;i++){//separar nombre y directorio
                            value=value+"/"+lista.at(i);
                        }

                            QString nombre_ext=lista.at(lista.size()-1);
                            try {
                                QStringList solo_nombre=nombre_ext.split(".");
                                QString ruta_raid=value+"/"+solo_nombre.at(0)+"_ra1.disk";

                 Crear_particion(tamanio,unidades,ajuste,tipo_par,ruta_raid,nombre);
                            } catch (...) {
                                cout<<"Error: no se encontro el raid"<<nombre_ext.toStdString()<< endl;
                            }
        }else{
            //Todo: Errores fueron ya notificados
        }
    }else if(is_delete){
        if(is_name && is_path){
            Eliminar_Particion(tipo_eliminacion,nombre,ruta);
            QStringList lista=ruta.split("/");
                        QString value="/home";
                        for(int i=2;i<lista.size()-1;i++){//separar nombre y directorio
                            value=value+"/"+lista.at(i);
                        }

                            QString nombre_ext=lista.at(lista.size()-1);
                            try {
                                QStringList solo_nombre=nombre_ext.split(".");
                                QString ruta_raid=value+"/"+solo_nombre.at(0)+"_ra1.disk";

                 Eliminar_Particion(tipo_eliminacion,nombre,ruta_raid);
                            } catch (...) {
                                cout<<"Error: no se encontro el raid"<<nombre_ext.toStdString()<< endl;
                            }

        }else{
            cout<<"Hacen falta parametros de name o path en delete"<<endl;
        }
    }else if(is_add){
        if(unidades2>-1){
            if(is_name && is_path){
                Agregar_Particion(unidades,tamanio,nombre,ruta);
                QStringList lista=ruta.split("/");
                            QString value="/home";
                            for(int i=2;i<lista.size()-1;i++){//separar nombre y directorio
                                value=value+"/"+lista.at(i);
                            }

                                QString nombre_ext=lista.at(lista.size()-1);
                                try {
                                    QStringList solo_nombre=nombre_ext.split(".");
                                    QString ruta_raid=value+"/"+solo_nombre.at(0)+"_ra1.disk";

                     Agregar_Particion(unidades,tamanio,nombre,ruta_raid);
                                } catch (...) {
                                    cout<<"Error: no se puede agregar"<<nombre_ext.toStdString()<< endl;
                                }
            }else{
                cout<<"Hacen falta parametro de name o path en comando add"<<endl;
            }
        }else{
            cout<<"El comando en units es invalido"<<endl;
        }
    }
    else{
        cout<<"Hacen falta parametros obligatorios, name, path o size"<<endl;
    }
}

bool Comandos::Verificar_Particiones(mbr disco, char tipo_part){
    int contador_primarias=0;
    int contador_exten=0;
    if(tipo_part=='P'){
        for(int i=0;i<4;i++){
            if(disco.part[i].part_type=='P'){
                contador_primarias++;
            }
        }
        if(contador_primarias==3){
            cout<<"Error: ya existen 3 particiones primarias,no se pueden crear mas"<<endl;
            return false;
        }else{
            return true;
        }
    }else if(tipo_part=='E'){
        for(int i=0;i<4;i++){
            if(disco.part[i].part_type=='E'){
                contador_exten++;
            }
        }
        if(contador_exten==1){
            cout<<"Error: ya existe 1 particiones extendida, no se pueden crear mas"<<endl;
            return false;
        }else{
            return true;
        }
    }
    return true;
}

bool Comandos::Verificar_Espacio(mbr disco, int espacio){
    int ocupado=0;
    for(int i=0;i<4;i++){
        if(disco.part[i].part_status=='1' ){
            ocupado=ocupado+disco.part[i].part_size;
        }
    }
    int libre=disco.mbr_tamano-ocupado;
    if(libre>=espacio){

        return true;
    }
    else{
        cout<<"No se puede crear la particion debido a que no queda espacio disponible"<<endl;
        return false;
    }
}


bool Comandos::Primera_Particon(mbr disco){
    int contador=0;
    for(int i=0;i<4;i++){
        if(disco.part[i].part_status=='0'){
            contador++;
        }
    }
    if(contador==4){
        return true;
    }
    return false;
}


int Comandos::Crear_particion(int tamanio, int unidades, char ajuste, char tipo_part, QString ruta, QString nombre){

    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    int size_particion=tamanio*unidades;
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    mbr disco;
    fseek(archivo,0,SEEK_SET);
    fread(&disco,sizeof(mbr),1,archivo);

    fclose(archivo);

    //convertir nombre a char *

    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();

    if(tipo_part=='p' || tipo_part=='P' ||tipo_part=='E' || tipo_part=='e' ){
        if(!Verificar_Espacio(disco,size_particion)){
            return 0;
            //Ya no se puede por espacio en general no tomo las logicas
        }
    }


    if(!Verificar_Particiones(disco,tipo_part)){
        return 0;
        //Si regresa false, es porque ya no se pueden crear mas
    }



    for(int i=0;i<4;i++){
        if(strcasecmp(nom,disco.part[i].part_name)==0){
            cout<<"Error: ya existe el nombre en una particion"<<endl;
            return 0;
        }
    }

    char ajuste_disco=disco.disk_fit;
    //int inicio_particiones=sizeof(mbr)+1;



    if(tipo_part=='P' || tipo_part=='E'){
        if(ajuste_disco=='B'|| ajuste_disco=='b' ){
             disco=Mejor_ajuste(disco,size_particion,ajuste,tipo_part,nom,ruta);
        }else if(ajuste_disco=='F' || ajuste_disco=='f'){
            disco=Primer_Ajuste(disco,size_particion,ajuste,tipo_part,nom,ruta);
        }else if(ajuste_disco=='W' || ajuste_disco=='w'){
            //Peor ajuste;
            disco=Peor_Ajuste(disco,size_particion,ajuste,tipo_part,nom,ruta);
        }
    }else{
        //operaciones con logicas, espacio, nombre iguales, creacion
        int pos_extendida=-1;
        for(int i=0;i<4;i++){
            if(disco.part[i].part_type=='E' || disco.part[i].part_type=='e'){
                pos_extendida=i;
            }
        }
        //Si existe una extendida
        if(pos_extendida==-1){
            cout<<"INFO:: No se pueden crear particiones logicas debido a que no existe una particion extendida"<<endl;
            return 0;
        }


        //Aca verifico nombres iguales
        if(Nombres_Iguales_Ext(ruta,disco.part[pos_extendida].part_start,nombre)){
            cout<<"INFO:: No se pueden crear a particion "<<nombre.toStdString()<<" debido a que ya existe una con el mismo nombre"<<endl;
            return 0;
        }

        //Aca verifico si existe espacio en la extendida
        if(Espacio_Ext(ruta,disco.part[pos_extendida].part_start,disco.part[pos_extendida].part_size,size_particion)){
            cout<<"INFO:: No se puede crear la particion "<<nombre.toStdString()<<" por falta de espacio"<<endl;
            return 0;
        }

        if(disco.part[pos_extendida].part_fit=='B' || disco.part[pos_extendida].part_fit=='b'){
            Mejor_Ajuste_Ext(size_particion,ajuste,nombre,ruta,disco.part[pos_extendida].part_start,disco.part[pos_extendida].part_size);
        }else if(disco.part[pos_extendida].part_fit=='F'|| disco.part[pos_extendida].part_fit=='f'){
            Peor_Ajuste_Ext(size_particion,ajuste,nombre,ruta,disco.part[pos_extendida].part_start,disco.part[pos_extendida].part_size);
        }else{
            Primer_Ajuste_Ext(size_particion,ajuste,nombre,ruta,disco.part[pos_extendida].part_start,disco.part[pos_extendida].part_size);
        }
    }
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }

    //Escribir nuevamente disco en el archivo
    fseek(archivo,0,SEEK_SET);
    fwrite(&disco,sizeof(mbr),1,archivo);
    fclose(archivo);

    return 0;
}

mbr Comandos::Ordenamiento_particiones(mbr disco){
    //ordenamiento burbuja
    int i;
    int j;
    partitio part_tem;
    for(i=0;i<4;i++)
    {
        for(j=i+1;j<4;j++)
        {
            if(disco.part[j].part_start<disco.part[i].part_start)
            {
                part_tem=disco.part[j];

                disco.part[j]=disco.part[i];
                disco.part[i]=disco.part[j];
                disco.part[i]=part_tem;
            }
        }
    }
    return disco;
}

mbr Comandos::Primer_Ajuste(mbr disco, int size_particion, char ajuste_part, char tipo_part, char *nombre,QString ruta){

    if(Primera_Particon(disco)){
        disco.part[0].part_fit=ajuste_part;
        disco.part[0].part_size=size_particion;
        disco.part[0].part_start=sizeof(mbr)+1;
        disco.part[0].part_status='1';
        disco.part[0].part_type=tipo_part;
        strcpy(disco.part[0].part_name,nombre);

        if(tipo_part=='E' || tipo_part=='e'){
            Crear_EBR_Inicial(ruta,sizeof(mbr)+1);
        }else{
            cout<<"INFO:: se ha creado la particion primaria con exito"<<endl;
        }

        return disco;
    }else{
        int inicio1=sizeof(mbr)+1;
        int inicio2=0;
        int espacio_disponible=0;
        int posicion_disponible=0;
        for(int i=0;i<4;i++){
            if(disco.part[i].part_status=='1'){//posiciones del arreglo ocupadas,estaran en orden
                inicio2=disco.part[i].part_start;
                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    /*Aca ya existe espacio disponbible por lo que voy a buscar
                     * una una posicion del arreglo de particiones que su status sea 0
                     */
                    for(int j=i;j<4;j++){
                        if(disco.part[j].part_status=='0'){
                            posicion_disponible=j;
                            break;
                        }
                    }
                    break;
                }else{

                    inicio1=disco.part[i].part_start+disco.part[i].part_size;
                }
            }else if(disco.part[i].part_status=='0'){
                inicio2=disco.part[i].part_start;//una posicion libre le voy a setear el tamanio del disco

                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    //Todo: espacio esta bien
                    posicion_disponible=i;
                    break;
                }else{
                    cout<<"No se puede crear la particion por falta de tamanio"<<endl;
                    return disco;
                }
            }
        }
        //Aca seteo la informacion de la particion donde le corresponde
        disco.part[posicion_disponible].part_fit=ajuste_part;
        disco.part[posicion_disponible].part_status='1';
        disco.part[posicion_disponible].part_type=tipo_part;
        strcpy(disco.part[posicion_disponible].part_name,nombre);
        disco.part[posicion_disponible].part_start=inicio1;
        disco.part[posicion_disponible].part_size=size_particion;
        if(tipo_part=='E' || tipo_part=='e'){
            Crear_EBR_Inicial(ruta,inicio1);
        }
        cout<<"Particion creada con exito"<<endl;
        disco=Ordenamiento_particiones(disco);
        return disco;
    }


}

mbr Comandos::Mejor_ajuste(mbr disco, int size_particion, char ajuste_part, char tipo_part, char *nombre, QString ruta){
    int espacio_anterior=0;
    int inicio=0;
    bool encontro_espacio=false;
    if(Primera_Particon(disco)){
        disco.part[0].part_fit=ajuste_part;
        disco.part[0].part_size=size_particion;
        disco.part[0].part_start=sizeof(mbr)+1;
        disco.part[0].part_status='1';
        disco.part[0].part_type=tipo_part;
        strcpy(disco.part[0].part_name,nombre);
        if(tipo_part=='E' || tipo_part=='e'){
            Crear_EBR_Inicial(ruta,sizeof(mbr)+1);
        }
        return disco;
    }else{
        int inicio1=sizeof(mbr)+1;
        int inicio2=0;
        int espacio_disponible=0;
        int posicion_disponible=0;
        for(int i=0;i<4;i++){
            if(disco.part[i].part_status=='1'){//posiciones del arreglo ocupadas,estaran en orden
                inicio2=disco.part[i].part_start;
                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    if(espacio_anterior==0){
                        espacio_anterior=espacio_disponible;
                        inicio=inicio1;
                        encontro_espacio=true;
                    }
                    else{
                        if(espacio_anterior<=espacio_disponible){
                            encontro_espacio=true;
                        }else{
                            espacio_anterior=espacio_disponible;
                            inicio=inicio1;
                            encontro_espacio=true;
                        }
                    }

                }
                inicio1=disco.part[i].part_start+disco.part[i].part_size;

            }else if(disco.part[i].part_status=='0'){
                inicio2=disco.part[i].part_start;//una posicion libre le voy a setear el tamanio del disco
                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    if(espacio_anterior==0){
                        espacio_anterior=espacio_disponible;
                        inicio=inicio1;
                        encontro_espacio=true;
                    }else{
                        if(espacio_anterior<=espacio_disponible){
                            encontro_espacio=true;
                        }else{
                            espacio_anterior=espacio_disponible;
                            inicio=inicio1;
                            encontro_espacio=true;
                        }
                    }
                }else{

                }
                posicion_disponible=i;
                break;
            }

        }
        if(!encontro_espacio){
            cout<<"No se puede crear la particion debido a que no se encontro un espacio disponible"<<endl;
            return disco;
        }

        //Aca seteo la informacion de la particion donde le corresponde
        disco.part[posicion_disponible].part_fit=ajuste_part;
        disco.part[posicion_disponible].part_status='1';
        disco.part[posicion_disponible].part_type=tipo_part;
        strcpy(disco.part[posicion_disponible].part_name,nombre);
        disco.part[posicion_disponible].part_start=inicio;
        disco.part[posicion_disponible].part_size=size_particion;
        if(tipo_part=='E' || tipo_part=='e'){
            Crear_EBR_Inicial(ruta,inicio);
        }
        cout<<"Particion creada con exito"<<endl;
        disco=Ordenamiento_particiones(disco);
        return disco;
    }
}

mbr Comandos::Peor_Ajuste(mbr disco, int size_particion, char ajuste_parte, char tipo_part, char *nombre, QString ruta){
    int espacio_anterior=0;
    int inicio=0;
    bool encontro_espacio=false;
    if(Primera_Particon(disco)){
        disco.part[0].part_fit=ajuste_parte;
        disco.part[0].part_size=size_particion;
        disco.part[0].part_start=sizeof(mbr)+1;
        disco.part[0].part_status='1';
        disco.part[0].part_type=tipo_part;
        strcpy(disco.part[0].part_name,nombre);
        if(tipo_part=='E' || tipo_part=='e'){
            Crear_EBR_Inicial(ruta,sizeof(mbr)+1);
        }
        return disco;
    }else{
        int inicio1=sizeof(mbr)+1;
        int inicio2=0;
        int espacio_disponible=0;
        int posicion_disponible=0;
        for(int i=0;i<4;i++){
            if(disco.part[i].part_status=='1'){//posiciones del arreglo ocupadas,estaran en orden
                inicio2=disco.part[i].part_start;
                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    if(espacio_anterior==0){
                        espacio_anterior=espacio_disponible;
                        inicio=inicio1;
                        encontro_espacio=true;
                    }else{
                        if(espacio_anterior>=espacio_disponible){
                            encontro_espacio=true;
                        }else{
                            espacio_anterior=espacio_disponible;
                            inicio=inicio1;
                            encontro_espacio=true;
                        }
                    }

                }
                 inicio1=disco.part[i].part_start+disco.part[i].part_size;

            }else if(disco.part[i].part_status=='0'){
                inicio2=disco.part[i].part_start;//una posicion libre le voy a setear el tamanio del disco

                espacio_disponible=inicio2-inicio1;
                if(size_particion<=espacio_disponible){
                    if(espacio_anterior==0){
                        espacio_anterior=espacio_disponible;
                        inicio=inicio1;
                        encontro_espacio=true;
                    }else{
                        if(espacio_anterior>=espacio_disponible){
                            encontro_espacio=true;
                        }else{
                            espacio_anterior=espacio_disponible;
                            inicio=inicio1;
                            encontro_espacio=true;
                        }
                    }
                }else{

                }
                posicion_disponible=i;
                break;
            }

        }
        if(!encontro_espacio){
            cout<<"No se puede crear la particion debido a que no se encontro un espacio disponible"<<endl;
            return disco;
        }

        //Aca seteo la informacion de la particion donde le corresponde
        disco.part[posicion_disponible].part_fit=ajuste_parte;
        disco.part[posicion_disponible].part_status='1';
        disco.part[posicion_disponible].part_type=tipo_part;
        strcpy(disco.part[posicion_disponible].part_name,nombre);
        disco.part[posicion_disponible].part_start=inicio;
        disco.part[posicion_disponible].part_size=size_particion;
        if(tipo_part=='E' || tipo_part=='e'){
            Crear_EBR_Inicial(ruta,inicio);
        }
        cout<<"Particion creada con exito"<<endl;
        disco=Ordenamiento_particiones(disco);//Ordeno la particion
        return disco;
    }
}

int Comandos::Eliminar_Particion(int tipo_eliminacion, QString nombre, QString ruta){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    mbr disco;
    fseek(archivo,0,SEEK_SET);
    fread(&disco,sizeof(mbr),1,archivo);
    fclose(archivo);
    //convertir nombre a char *

    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();

    int posicion=-1;

    for(int i=0;i<4;i++){
        if(strcasecmp(disco.part[i].part_name,nom)==0){
            posicion=i;
        }
    }

    if(posicion==-1){
        for(int i=0;i<4;i++){
            if(disco.part[i].part_type=='E' || disco.part[i].part_type=='e'){
                posicion=i;
            }
        }
        if(posicion!=-1){
            Eliminar_Logica(tipo_eliminacion,nombre,ruta,disco.part[posicion].part_start,disco.part[posicion].part_size);
        }else{
            cout<<"INFO:: La particion no existe"<<endl;
            return 0;
        }

    }else{
        if(tipo_eliminacion==1){
            disco.part[posicion].part_fit='0';
            disco.part[posicion].part_status='0';
            disco.part[posicion].part_type='0';
            disco.part[posicion].part_size=0;
            disco.part[posicion].part_start=disco.mbr_tamano;
            strcpy(disco.part[posicion].part_name,"");
        }else{
            int inicio=disco.part[posicion].part_start;
            int tamanio=disco.part[posicion].part_size;
            //Aca amando a escribir el caracter \0
            char val[]={'\0'};
            fseek(archivo,inicio,SEEK_SET);
            fwrite(&val,sizeof(char),1,archivo);
            fseek(archivo,tamanio-1,SEEK_SET);
            fwrite(&val,sizeof(char),1,archivo);

            disco.part[posicion].part_fit='0';
            disco.part[posicion].part_status='0';
            disco.part[posicion].part_type='0';
            disco.part[posicion].part_size=0;
            disco.part[posicion].part_start=disco.mbr_tamano;
            strcpy(disco.part[posicion].part_name,"");
        }
        cout<<"Particion Elimina con exito"<<endl;
        disco=Ordenamiento_particiones(disco);
    }
    //Escribir estructura

    archivo=fopen(ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    fseek(archivo,0,SEEK_SET);
    fwrite(&disco,sizeof(mbr),1,archivo);
    fclose(archivo);
    return 0;
}

int Comandos::Agregar_Particion(int unidad, int tamanio, QString nombre, QString ruta){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    mbr disco;
    fseek(archivo,0,SEEK_SET);
    fread(&disco,sizeof(mbr),1,archivo);

    fclose(archivo);
    int tamanio_modif=unidad*tamanio;
    //convertir nombre a char *

    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();
    int posicion=-1;
    for(int i=0;i<4;i++){
        if(strcasecmp(disco.part[i].part_name,nom)==0){
            posicion=i;
        }
    }
    if(posicion==-1){
        for(int i=0;i<4;i++){
            if(disco.part[i].part_type=='E' || disco.part[i].part_type=='e'){
                posicion=i;
            }
        }
        if(posicion!=-1){
            Agregar_Logica(unidad,tamanio,nombre,ruta,disco.part[posicion].part_start,disco.part[posicion].part_size);
        }else{
             cout<<"La particion no existe en el disco"<<endl;
        }


    }else{
        if(tamanio_modif>0){//se va a agregar
            if(posicion==3){//esta en la ultima posicion
                int final_particion=disco.part[posicion].part_start+disco.part[posicion].part_size;
                int espacio_disponible=disco.mbr_tamano-final_particion;
                if(tamanio_modif<=espacio_disponible){
                    disco.part[posicion].part_size=disco.part[posicion].part_size+tamanio_modif;
                    cout<<"Se agrego espacio a la particion"<<endl;
                }else{
                    cout<<"No existe espacio suficiente para agregar"<<endl;
                }
            }else{
                int final_particion=disco.part[posicion].part_start+disco.part[posicion].part_size;
                int espacio_disponible=disco.part[posicion+1].part_start-final_particion;
                if(tamanio_modif<=espacio_disponible){
                    disco.part[posicion].part_size=disco.part[posicion].part_size+tamanio_modif;
                    cout<<"Se agrego espacio a la particion"<<endl;
                }else{
                    cout<<"No existe espacio suficiente para agregar"<<endl;
                }
            }
        }else if(tamanio_modif<0){//se va a quitar
            //int final_part=disco.part[posicion].part_start+disco.part[posicion].part_size;
            tamanio_modif=tamanio_modif*-1;
            if(tamanio_modif<disco.part[posicion].part_size){
                disco.part[posicion].part_size=disco.part[posicion].part_size-tamanio_modif;
                cout<<"Se quito espacio a la particion"<<endl;
            }else{
                cout<<"No se puede quitar espacio debido a que no queda espacio"<<endl;
            }
        }else{//No se va a agregar nada

        }
    }
    archivo=fopen(ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    fseek(archivo,0,SEEK_SET);
    fwrite(&disco,sizeof(mbr),1,archivo);
    fclose(archivo);

    return 0;
}


void Comandos::Ejecutar_Montaje(Nodo *montaje){
    bool is_error=false;
    bool is_path=false;
    bool is_name=false;
    QString ruta="";
    QString name_part="";
    for(int i=0;i<montaje->hijos.size();i++){
        Nodo tem=montaje->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case path:
             ruta=Quitar_Comillas(tem.valor);
             is_path=true;
            break;
        case name:
            name_part=tem.valor;
            is_name=true;
            break;

        default:
            cout<<"Error:Comando invalido en mount"<<endl;
            is_error=true;
        }
    }
    if(is_name && is_path){
        Hacer_Montaje(ruta,name_part);
    }else if(is_error){
        //pues nada wey, ya notifique
    }
    else{
        cout<<"Error: hacen falta parametros obligatorios en mount"<<endl;
    }

}


int Comandos::Hacer_Montaje(QString ruta, QString nombre){
    QStringList lista=ruta.split("/");
    int total_lista=lista.size();
    QString nombre_disco=lista.at(total_lista-1);

    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: No se encontro el disco"<<endl;
        return 0;
    }
    fseek(archivo,0,SEEK_SET);
    mbr disco;
    int part_exte=-1;

    fread(&disco,sizeof(mbr),1,archivo);

    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();

    bool encontro=false;


    for(int i=0;i<4;i++){
        if(disco.part[i].part_status=='1'){
            if(strcasecmp(disco.part[i].part_name,nom)==0){
                encontro=true;
                //part_mon=disco.part[i];
            }
            if(disco.part[i].part_type=='E' || disco.part[i].part_type=='e'){
                //extendida=i;
                part_exte=i;
            }
        }
    }
    if(encontro==false && part_exte!=-1){
        //Buscar el nombre en las logicas
    }

    if(encontro==false){
        cout<<"Error: no se encontro la particion especifica en el disco"<<endl;
        return 0;
    }

    //Verifico que la particion no este montada
    for(int i=0;i<50;i++){
        if(montar_repo[i].estado==1){
            if(QString::compare(montar_repo[i].nombre_disco,nombre_disco)==0 && QString::compare(montar_repo[i].nombre_part,nombre)==0){
                cout<<"Alerta:: La particion ya fue montada"<<endl;
                fclose(archivo);
                return 0;
            }
        }
    }

    //verificar si ya fue montada una particion
    //con el nombre del disco
    int valor;
    int posicion=-1;
    int contador=0;

    bool disco_usado=false;
    for(int i=0;i<50;i++){
        if(montar_repo[i].estado==1){
            if(QString::compare(montar_repo[i].nombre_disco,nombre_disco)==0){
                disco_usado=true;//se esta usando el disco
                contador++;
                valor=montar_repo[i].numero_letra;
            }
        }
    }

    if(disco_usado==true){
        //Cuando ya se esta usando el disco
        for(int i=0;i<50;i++){//Buscar una posicion libre
            if(montar_repo[i].estado==0){
                posicion=i;
            }
        }
        QString vd=Return_ID_reporte(valor);//numero id de montaje
        int numero_monta=contador+1;
        montar_repo[posicion].disco_ruta=ruta;
        montar_repo[posicion].estado=1;
        montar_repo[posicion].id_mont=vd+QString::number(numero_monta);
        montar_repo[posicion].nombre_disco=nombre_disco;
        montar_repo[posicion].nombre_part=nombre;
        montar_repo[posicion].numero=numero_monta;
        montar_repo[posicion].numero_letra=valor;
        cout<<"Particion montada con exito"<<endl;

    }else{
        QString vd="";

        int disponible=-1;

        for(int i=0;i<50;i++){
            if(montar_repo[i].estado==0){
                disponible=i;
            }
        }
        varLetra++;
        valor=varLetra;
        vd=Return_ID_reporte(valor)+"1";
        montar_repo[disponible].disco_ruta=ruta;
        montar_repo[disponible].nombre_part=nombre;
        montar_repo[disponible].nombre_disco=nombre_disco;
        montar_repo[disponible].numero_letra=valor;
        montar_repo[disponible].estado=1;
        montar_repo[disponible].id_mont=vd;
        montar_repo[disponible].numero=1;
        cout<<"Particion Montada con exito"<<endl;
    }
    fclose(archivo);
    return 0;
}

void Comandos::Ejecutar_Desmontaje(Nodo *desmontaje){
    bool is_error=false;
    bool is_name=false;
    QString id_part="";
    for(int i=0;i<desmontaje->hijos.size();i++){
        Nodo tem=desmontaje->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case id:
            id_part=tem.valor;
            is_name=true;
            break;
        default:
            is_error=true;
            cout<<"Error: Parametros no validos en la instruccion unmount"<<endl;
            break;
        }
    }

    if(is_name && !is_error){
        Hacer_Desmontaje(id_part);
    }else if(!is_name){
        cout<<"Error: No se encontro el parametro id en la instruccion unmount"<<endl;
    }

}

int Comandos::Hacer_Desmontaje(QString id_part){
    cout<<"INFO:: id particion a desmontar"<<id_part.toStdString()<<endl;

    for(int i=0;i<50;i++){
        if(montar_repo[i].estado==1){
            if(QString::compare(montar_repo[i].id_mont,id_part,Qt::CaseInsensitive)==0){
                montar_repo[i].estado=0;
                montar_repo[i].disco_ruta="";
                montar_repo[i].id_mont="";
                montar_repo[i].nombre_disco="";
                montar_repo[i].nombre_part="";
                montar_repo[i].numero=-1;
                montar_repo[i].numero_letra=-1;
                cout<<"INFO:: la particion se ha desmontado"<<endl;
                return 0;
            }
        }
    }
    cout<<"INFO:: La particion no existe montada"<<endl;
    return 0;
}


QString Comandos::Return_ID_reporte(int value){
    switch (value) {
    case 1:
        return "vda";
        break;
    case 2:
        return "vdb";
        break;
    case 3:
        return "vdc";
        break;
    case 4:
        return "vdd";
        break;
    case 5:
        return "vde";
        break;
    case 6:
        return "vdf";
        break;
    case 7:
        return "vdg";
        break;
    case 8:
        return "vdh";
        break;
        return "vdi";
    case 9:
        return "vdj";
        break;
    case 10:
        return "vdk";
        break;
    case 11:
        return "vdl";
        break;
    case 12:
        return "vdm";
        break;
    default:

        break;
    }
    return "";
}



void Comandos::Ejecutar_Reporte(Nodo *repo){
    bool is_name=false;
    bool is_path=false;
    bool is_id=false;
    int tipo_report=0;
    QString ruta="";
    QString id_part="";
    QString ruta_archivo="";
    bool is_archivo=false;
    for(int i=0;i<repo->hijos.size();i++){
        Nodo tem=repo->hijos.at(i);
        int tipo_=tem.tipo_;

        switch (tipo_) {
        case name:
            if(QString::compare(tem.valor,"mbr",Qt::CaseInsensitive)==0){
                tipo_report=1;
                is_name=true;
            }else if(QString::compare(tem.valor,"disk",Qt::CaseInsensitive)==0){
                tipo_report=2;
                is_name=true;
            }else if(QString::compare(tem.valor,"sb",Qt::CaseInsensitive)==0){
                    tipo_report=3;
                    is_name=true;
            }else if(QString::compare(tem.valor,"bm_inode",Qt::CaseInsensitive)==0){
                tipo_report=4;
                is_name=true;
            }else if(QString::compare(tem.valor,"bm_block",Qt::CaseInsensitive)==0){
                tipo_report=5;
                is_name=true;
            }else if(QString::compare(tem.valor,"inode",Qt::CaseInsensitive)==0){
                tipo_report=6;
                is_name=true;
            }else if(QString::compare(tem.valor,"block",Qt::CaseInsensitive)==0){
                tipo_report=7;
                is_name=true;
            }else if(QString::compare(tem.valor,"tree",Qt::CaseInsensitive)==0){
                tipo_report=8;
                is_name=true;
            }else if(QString::compare(tem.valor,"journaling",Qt::CaseInsensitive)==0){
                tipo_report=9;
                is_name=true;
            }else if(QString::compare(tem.valor,"file",Qt::CaseInsensitive)==0){
                tipo_report=10;
                is_name=true;
            }else if(QString::compare(tem.valor,"ls",Qt::CaseInsensitive)==0){
                tipo_report=11;
                is_name=true;
            }
            else{
                cout<<"Valor para comando name no valido"<<endl;
            }
            break;
        case path:
            ruta=Quitar_Comillas(tem.valor);
             is_path=true;
            break;
        case Ruta:
                ruta_archivo=Quitar_Comillas(tem.valor);
                is_archivo=true;
            break;
        case id:
            id_part=tem.valor;
            is_id=true;
            break;

        default:
            cout<<"Comando no valido"<<endl;
        }
    }
    if(is_name && is_path && is_id){

        //Aca mando a llamar para hacer el reporte
        Verificar_Reporte(tipo_report,ruta,id_part,ruta_archivo);

    }else{
        if(is_name){cout<<"Error: no se encontro el comando name"<<endl;}
        else if(is_path){cout<<"Error: no se encontro el comando path"<<endl;}
        else if(is_id){cout<<"Error: no se encontro el comando id"<<endl;}

    }
}

void Comandos::Verificar_Reporte(int tipo, QString ruta,QString id,QString ruta_archivo){
    QStringList lista=ruta.split("/");
    QString value="/home"; //value tendra el directorio
    for(int i=2;i<lista.size()-1;i++){//separar nombre y directorio
        value=value+"/"+lista.at(i);
    }
    QString nombre_ext=lista.at(lista.size()-1);

    Reporte *rep=new Reporte();

    if(Existe_directorio(value)){//aca ya se creo el directorio donde se guardara
        if(tipo==1){
            rep->Reporte_MBR(id,value,nombre_ext);
        }else if(tipo==2){
            rep->Reporte_DISK(id,value,nombre_ext);
        }else if(tipo==3){
            rep->Reporte_SB(id,value,nombre_ext);
        }else if(tipo==4){
            rep->Reporte_BM_Inode(id,value,nombre_ext);
        }else if(tipo==5){
            rep->Reporte_BM_Bloque(id,value,nombre_ext);
        }else if(tipo==6){
            rep->Reporte_Inode(id,value,nombre_ext);
        }else if(tipo==7){
            rep->Reporte_Bloque(id,value,nombre_ext);
        }else if(tipo==8){
            rep->ReporteTree(id,value,nombre_ext);
        }else if(tipo==9){
            rep->Reporte_Journaling(id,value,nombre_ext);
        }else if(tipo==10){
            if(QString::compare(ruta_archivo,"")!=0){
                rep->Reporte_File(id,value,nombre_ext,ruta_archivo);
            }else{
                cout<<"Erro:: No se puede ejecutar el reporte file, hace fala el parametro de ruta"<<endl;
            }

        }else if(tipo==11){
            if(QString::compare(ruta_archivo,"")!=0){
                  rep->Reporte_Ls(id,value,nombre_ext,ruta_archivo);
            }else{
                cout<<"Erro:: No se puede ejecutar el reporte file, hace fala el parametro de ruta"<<endl;
            }
        }
    }else{
        cout<<"Error: No se puede crear el directorio o no existe"<<endl;
    }
    delete rep;
}

//Manejo de logicas
int Comandos::Crear_EBR_Inicial(QString ruta, int pos_inicial){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr inicial_ebr;
    inicial_ebr.part_fit='0';
    strcpy(inicial_ebr.part_name,"");
    inicial_ebr.part_next=-1;
    inicial_ebr.part_size=0;
    inicial_ebr.part_start=pos_inicial;
    inicial_ebr.part_status='0';
    cout<<"INFO:: La particion extendida fue creada con exito"<<endl;
    fseek(archivo,pos_inicial,SEEK_SET);
    fwrite(&inicial_ebr,sizeof(ebr),1,archivo);
    fclose(archivo);
    //Escibir el primer ebr
    return 0;
}

bool Comandos::Nombres_Iguales_Ext(QString ruta, int pos_inicial, QString nombre_part){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return true;
    }
    ebr tem_ebr;
    fseek(archivo,pos_inicial,SEEK_SET);
    fread(&tem_ebr,sizeof(ebr),1,archivo);


    QByteArray array = nombre_part.toLocal8Bit();
    char* nom = array.data();
    int pos_siguiente=0;
    while(pos_siguiente!=-1){
        if(strcasecmp(tem_ebr.part_name,nom)==0){
            fclose(archivo);
            return true;
        }
        pos_siguiente=tem_ebr.part_next;
        if(pos_siguiente!=-1){

            fseek(archivo,pos_siguiente,SEEK_SET);
            fread(&tem_ebr,sizeof(ebr),1,archivo);
        }else{
            fclose(archivo);
            return false;
        }
    }
    fclose(archivo);
    return false;
}

bool Comandos::Espacio_Ext(QString ruta, int pos_inicial, int size_part,int size_logica){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return false;
    }
    ebr tem_ebr;
    fseek(archivo,pos_inicial,SEEK_SET);
    fread(&tem_ebr,sizeof(ebr),1,archivo);

    int pos_siguiente=0;
    int ocupado=0;
    while(pos_siguiente!=-1){
        ocupado=ocupado+tem_ebr.part_size;
        pos_siguiente=tem_ebr.part_next;
        if(pos_siguiente!=-1){
            fseek(archivo,pos_siguiente,SEEK_SET);
            fread(&tem_ebr,sizeof(ebr),1,archivo);
        }
    }
    int libre=size_part-ocupado;
    if(libre>=size_logica){
        fclose(archivo);
        return false;
    }
    fclose(archivo);
    return true;
}

int Comandos::Primer_Ajuste_Ext(int size_part, char ajuste_part, QString nombre_part, QString ruta, int inicial_ext, int size_ext){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr tem_ebr;
    ebr tem_ebr_anterior;
    ebr nuevo;
    fseek(archivo,inicial_ext,SEEK_SET);
    fread(&tem_ebr,sizeof(ebr),1,archivo);

    QByteArray array = nombre_part.toLocal8Bit();
    char* nom = array.data();

    if(tem_ebr.part_status=='0' && tem_ebr.part_next==-1){
        tem_ebr.part_fit=ajuste_part;
        strcpy(tem_ebr.part_name,nom);
        tem_ebr.part_next=-1;
        tem_ebr.part_size=size_part;
        tem_ebr.part_start=inicial_ext;
        tem_ebr.part_status='1';
        fseek(archivo,inicial_ext,SEEK_SET);
        fwrite(&tem_ebr,sizeof(ebr),1,archivo);
        fclose(archivo);
        return 0;
    }

    int inicio1=inicial_ext;
    int inicio2=0;
    int pos_siguiente=0;
    int espacio_disponible=0;
    bool encontro_espacio=false;
    int contador=0;
    int caso=0;
    while(pos_siguiente!=-1){
        if(tem_ebr.part_status=='1'){
           inicio2=tem_ebr.part_start;
           espacio_disponible=inicio2-inicio1;

           if(espacio_disponible>=size_part){
               caso=1;
               nuevo.part_fit=ajuste_part;
               strcpy(nuevo.part_name,nom);
               nuevo.part_next=inicio2;
               nuevo.part_size=size_part;
               nuevo.part_start=inicio1;
               nuevo.part_status='1';
               if(contador==0){

               }else{
                    tem_ebr_anterior.part_next=nuevo.part_start;
               }

               encontro_espacio=true;
               //Caso en donde se va a insertar en medio de dos logicas
           }
               contador++;
               inicio1=tem_ebr.part_start+tem_ebr.part_size;
               pos_siguiente=tem_ebr.part_next;
               if(pos_siguiente!=-1){
                   tem_ebr_anterior=tem_ebr;
                   fseek(archivo,pos_siguiente,SEEK_SET);
                   fread(&tem_ebr,sizeof(ebr),1,archivo);
               }else{
                   espacio_disponible=inicio1-(size_ext+inicial_ext);
                   if(espacio_disponible>=size_part){
                       caso=2;
                       nuevo.part_fit=ajuste_part;
                       strcpy(nuevo.part_name,nom);
                       nuevo.part_next=-1;
                       nuevo.part_size=size_part;
                       nuevo.part_start=inicio1;
                       nuevo.part_status='1';
                       tem_ebr.part_next=nuevo.part_start;
                       encontro_espacio=true;
                   }
               }


        }else{
            fseek(archivo,tem_ebr.part_next,SEEK_SET);
            fread(&tem_ebr,sizeof(ebr),1,archivo);
        }

    }
    if(encontro_espacio){
        if(caso==1){
            if(contador<=0){
                ebr tem1;
                fseek(archivo,inicial_ext,SEEK_SET);
                fread(&tem1,sizeof(ebr),1,archivo);
                tem1.part_fit=nuevo.part_fit;
                strcpy(tem1.part_name,nuevo.part_name);
                tem1.part_next=nuevo.part_next;
                tem1.part_size=nuevo.part_size;
                tem1.part_start=nuevo.part_start;
                tem1.part_status=nuevo.part_status;
                fseek(archivo,inicial_ext,SEEK_SET);
                fwrite(&tem1,sizeof(ebr),1,archivo);
            }else{
                fseek(archivo,tem_ebr_anterior.part_start,SEEK_SET);
                fwrite(&tem_ebr_anterior,sizeof(ebr),1,archivo);
                fseek(archivo,nuevo.part_start,SEEK_SET);
                fwrite(&nuevo,sizeof(ebr),1,archivo);
                fseek(archivo,tem_ebr.part_start,SEEK_SET);
                fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            }
        }else if(caso==2){
            fseek(archivo,tem_ebr.part_start,SEEK_SET);
            fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            fseek(archivo,nuevo.part_start,SEEK_SET);
            fwrite(&nuevo,sizeof(ebr),1,archivo);
        }
        cout<<"Particion Logica creada con exito"<<endl;
    }else{
        cout<<"INFO:: la particion no fue creado por falta de espacio"<<endl;
    }
    fclose(archivo);
    return 0;
}

int Comandos::Mejor_Ajuste_Ext(int size_part, char ajuste_part, QString nombre, QString ruta, int inicial_ext, int size_ext){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr tem_ebr;
    ebr tem_ebr_anterior;



    fseek(archivo,inicial_ext,SEEK_SET);
    fread(&tem_ebr,sizeof(ebr),1,archivo);

    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();

    if(tem_ebr.part_status=='0' && tem_ebr.part_next==-1){
        tem_ebr.part_fit=ajuste_part;
        strcpy(tem_ebr.part_name,nom);
        tem_ebr.part_next=-1;
        tem_ebr.part_size=size_part;
        tem_ebr.part_start=inicial_ext;
        tem_ebr.part_status='1';
        fseek(archivo,inicial_ext,SEEK_SET);
        fwrite(&tem_ebr,sizeof(ebr),1,archivo);
        fclose(archivo);
        return 0;
    }

    int inicio1=inicial_ext;
    int inicio2=0;
    int nuevo_inicio=0;
    int nuevo_siguiente=0;
    int inicio_anterior=0;
    int espacio_anterior=0;
    int pos_siguiente=0;
    int espacio_disponible=0;
    int contador=0;
    int caso=0;
    bool encontro_espacio=false;
    while(pos_siguiente!=-1){
        if(tem_ebr.part_status=='1'){
            inicio2=tem_ebr.part_start;
            espacio_disponible=inicio2-inicio1;
            if(espacio_disponible>=size_part){
                if(espacio_anterior==0){
                    espacio_anterior=espacio_disponible;

                    caso=1;
                    nuevo_inicio=inicio1;
                    nuevo_siguiente=inicio2;
                    encontro_espacio=true;
                    if(contador==0){
                        inicio_anterior=inicio1;
                    }else {
                        inicio_anterior=tem_ebr_anterior.part_start;
                    }



                }else{
                    if(espacio_anterior<=espacio_disponible){
                        encontro_espacio=true;

                    }else{
                        espacio_anterior=espacio_disponible;

                        caso=1;
                        nuevo_inicio=inicio1;
                        nuevo_siguiente=inicio2;
                        if(contador==0){
                            inicio_anterior=inicio1;
                        }else {
                            inicio_anterior=tem_ebr_anterior.part_start;
                        }
                        encontro_espacio=true;
                    }
                }
            }
            contador++;
            //habia un else
                inicio1=tem_ebr.part_start+tem_ebr.part_size;
                pos_siguiente=tem_ebr.part_next;
                if(pos_siguiente!=-1){
                    tem_ebr_anterior=tem_ebr;
                    fseek(archivo,pos_siguiente,SEEK_SET);
                    fread(&tem_ebr,sizeof(ebr),1,archivo);

                }else{
                    int pos_final=size_ext+inicial_ext;
                    espacio_disponible=pos_final-inicio1;
                    if(espacio_disponible>=size_part){
                        if(espacio_anterior==0){
                            caso=2;
                            espacio_anterior=espacio_disponible;
                            nuevo_inicio=inicio1;
                            nuevo_siguiente=-1;
                            inicio_anterior=tem_ebr.part_start;
                            encontro_espacio=true;
                        }else{
                            if(espacio_anterior<=espacio_disponible){

                                encontro_espacio=true;
                            }else{
                                encontro_espacio=true;
                                caso=2;
                                espacio_anterior=espacio_disponible;
                                nuevo_inicio=inicio1;
                                nuevo_siguiente=-1;
                               inicio_anterior=tem_ebr.part_start;
                                encontro_espacio=true;
                            }
                        }
                    }
                }

            \
        }else{
            fseek(archivo,tem_ebr.part_next,SEEK_SET);
            fread(&tem_ebr,sizeof(ebr),1,archivo);
        }
    }
    if(encontro_espacio){
        if(caso==1){
            if(contador<=1){
                fseek(archivo,inicial_ext,SEEK_SET);
                fread(&tem_ebr,sizeof(ebr),1,archivo);//el primero
                tem_ebr.part_fit=ajuste_part;
                strcpy(tem_ebr.part_name,nom);
                tem_ebr.part_next=nuevo_siguiente;
                tem_ebr.part_size=size_part;
                tem_ebr.part_start=nuevo_inicio;
                tem_ebr.part_status='1';
                fseek(archivo,nuevo_inicio,SEEK_SET);
                fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            }else{
                fseek(archivo,inicio_anterior,SEEK_SET);
                fread(&tem_ebr_anterior,sizeof(ebr),1,archivo);
                tem_ebr_anterior.part_next=nuevo_inicio;
                fseek(archivo,inicio_anterior,SEEK_SET);
                fwrite(&tem_ebr_anterior,sizeof(ebr),1,archivo);
                ebr nuevo;
                nuevo.part_fit=ajuste_part;
                strcpy(nuevo.part_name,nom);
                nuevo.part_next=nuevo_siguiente;
                nuevo.part_size=size_part;
                nuevo.part_start=nuevo_inicio;
                nuevo.part_status='1';
                fseek(archivo,nuevo_inicio,SEEK_SET);
                fwrite(&nuevo,sizeof(ebr),1,archivo);
                fseek(archivo,nuevo_siguiente,SEEK_SET);
                fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            }
        }else if(caso==2){
            ebr nuevo;
            nuevo.part_fit=ajuste_part;
            strcpy(nuevo.part_name,nom);
            nuevo.part_next=-1;
            nuevo.part_size=size_part;
            nuevo.part_start=nuevo_inicio;
            nuevo.part_status='1';

            tem_ebr.part_next=nuevo_inicio;
            fseek(archivo,tem_ebr.part_start,SEEK_SET);
            fwrite(&tem_ebr,sizeof(ebr),1,archivo);

            fseek(archivo,nuevo_inicio,SEEK_SET);
            fwrite(&nuevo,sizeof(ebr),1,archivo);

        }
        cout<<"Particion Logica creada con exito"<<endl;
    }else{
        cout<<"INFO:: No se encontro espacio para la particion"<<endl;

    }
    fclose(archivo);
    return 0;
}

int Comandos::Peor_Ajuste_Ext(int size_part, char ajuste_part, QString nombre, QString ruta, int inicial_ext, int size_ext){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr tem_ebr;
    ebr tem_ebr_anterior;



    fseek(archivo,inicial_ext,SEEK_SET);
    fread(&tem_ebr,sizeof(ebr),1,archivo);

    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();

    if(tem_ebr.part_status=='0' && tem_ebr.part_next==-1){
        tem_ebr.part_fit=ajuste_part;
        strcpy(tem_ebr.part_name,nom);
        tem_ebr.part_next=-1;
        tem_ebr.part_size=size_part;
        tem_ebr.part_start=inicial_ext;
        tem_ebr.part_status='1';
        fseek(archivo,inicial_ext,SEEK_SET);
        fwrite(&tem_ebr,sizeof(ebr),1,archivo);
        fclose(archivo);
        return 0;
    }

    int inicio1=inicial_ext;
    int inicio2=0;
    int nuevo_inicio=0;
    int nuevo_siguiente=0;
    int inicio_anterior=0;
    int espacio_anterior=0;
    int pos_siguiente=0;
    int espacio_disponible=0;
    int contador=0;
    int caso=0;
    bool encontro_espacio=false;
    while(pos_siguiente!=-1){
        if(tem_ebr.part_status=='1'){
            inicio2=tem_ebr.part_start;
            espacio_disponible=inicio2-inicio1;
            if(espacio_disponible>=size_part){
                if(espacio_anterior==0){
                    espacio_anterior=espacio_disponible;

                    caso=1;
                    nuevo_inicio=inicio1;
                    nuevo_siguiente=inicio2;
                    encontro_espacio=true;
                    if(contador==0){
                        inicio_anterior=inicio1;
                    }else {
                        inicio_anterior=tem_ebr_anterior.part_start;
                    }


                }else{
                    if(espacio_anterior>=espacio_disponible){
                        encontro_espacio=true;

                    }else{
                        espacio_anterior=espacio_disponible;

                        caso=1;
                        nuevo_inicio=inicio1;
                        nuevo_siguiente=inicio2;
                        if(contador==0){
                            inicio_anterior=inicio1;
                        }else {
                            inicio_anterior=tem_ebr_anterior.part_start;
                        }
                        encontro_espacio=true;
                    }
                }
            }
                contador++;
                inicio1=tem_ebr.part_start+tem_ebr.part_size;
                pos_siguiente=tem_ebr.part_next;
                if(pos_siguiente!=-1){
                    tem_ebr_anterior=tem_ebr;
                    fseek(archivo,pos_siguiente,SEEK_SET);
                    fread(&tem_ebr,sizeof(ebr),1,archivo);

                }else{
                    int pos_final=size_ext+inicial_ext;
                    espacio_disponible=pos_final-inicio1;
                    if(espacio_disponible>=size_part){
                        if(espacio_anterior==0){
                            espacio_anterior=espacio_disponible;
                            nuevo_inicio=inicio1;
                            nuevo_siguiente=-1;
                            encontro_espacio=true;
                            caso=2;
                        }else{
                            if(espacio_anterior>=espacio_disponible){

                                encontro_espacio=true;
                            }else{
                                encontro_espacio=true;
                                espacio_anterior=espacio_disponible;
                                nuevo_inicio=inicio1;
                                nuevo_siguiente=-1;
                                inicio_anterior=tem_ebr.part_start;
                                caso=2;
                            }
                        }
                    }
                }


        }else{
            fseek(archivo,tem_ebr.part_next,SEEK_SET);
            fread(&tem_ebr,sizeof(ebr),1,archivo);
        }
    }

    if(encontro_espacio){
        if(caso==1){
            if(contador<=1){
                fseek(archivo,inicial_ext,SEEK_SET);
                fread(&tem_ebr,sizeof(ebr),1,archivo);//el primero
                tem_ebr_anterior.part_fit=ajuste_part;
                strcpy(tem_ebr.part_name,nom);
                tem_ebr.part_next=nuevo_siguiente;
                tem_ebr.part_size=size_part;
                tem_ebr.part_start=nuevo_inicio;
                tem_ebr.part_status='1';
                fseek(archivo,nuevo_inicio,SEEK_SET);
                fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            }else{
                fseek(archivo,inicio_anterior,SEEK_SET);
                fread(&tem_ebr_anterior,sizeof(ebr),1,archivo);
                tem_ebr_anterior.part_next=nuevo_inicio;
                fseek(archivo,inicio_anterior,SEEK_SET);
                fwrite(&tem_ebr_anterior,sizeof(ebr),1,archivo);
                ebr nuevo;
                nuevo.part_fit=ajuste_part;
                strcpy(nuevo.part_name,nom);
                nuevo.part_next=nuevo_siguiente;
                nuevo.part_size=size_part;
                nuevo.part_start=nuevo_inicio;
                nuevo.part_status='1';
                fseek(archivo,nuevo_inicio,SEEK_SET);
                fwrite(&nuevo,sizeof(ebr),1,archivo);
                fseek(archivo,nuevo_siguiente,SEEK_SET);
                fwrite(&tem_ebr,sizeof(ebr),1,archivo);
            }
        }else if(caso==2){
            ebr nuevo;
            nuevo.part_fit=ajuste_part;
            strcpy(nuevo.part_name,nom);
            nuevo.part_next=-1;
            nuevo.part_size=size_part;
            nuevo.part_start=nuevo_inicio;
            nuevo.part_status='1';

            tem_ebr.part_next=nuevo_inicio;
            fseek(archivo,tem_ebr.part_start,SEEK_SET);
            fwrite(&tem_ebr,sizeof(ebr),1,archivo);

            fseek(archivo,nuevo_inicio,SEEK_SET);
            fwrite(&nuevo,sizeof(ebr),1,archivo);

        }
        cout<<"Particion Logica creada con exito"<<endl;
    }else{
        cout<<"INFO:: No se encontro espacio para la particion"<<endl;

    }


    fclose(archivo);
    return 0;
}


int Comandos::Eliminar_Logica(int tipo_eliminacion, QString nombre_part, QString ruta, int inicio_ext, int size_ext){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");
    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr tem;
    ebr tem_anterior;
    fseek(archivo,inicio_ext,SEEK_SET);
    fread(&tem,sizeof(ebr),1,archivo);

    if(tem.part_status=='0' && tem.part_size==-1){
        cout<<"No existe la particion con el nombre "<<nombre_part.toStdString()<<endl;
        return 0;
    }

    int pos_siguiente=0;
    bool existe_particion=false;
    int next_siguiente=0;

    QByteArray array = nombre_part.toLocal8Bit();
    char* nom = array.data();
    int contador=0;
    while(pos_siguiente!=-1){
        if(tem.part_status=='1'){
            if(strcasecmp(tem.part_name,nom)==0){
                if(contador==0){
                    tem.part_status='0';
                    tem.part_fit='0';
                    strcpy(tem.part_name,"");
                    tem.part_size=0;
                    tem.part_start=inicio_ext;
                    fseek(archivo,inicio_ext,SEEK_SET);
                    fwrite(&tem,sizeof(ebr),1,archivo);
                    cout<<"Se elimino la particion correctamente"<<endl;
                }else{
                    next_siguiente=tem.part_next;
                    pos_siguiente=tem.part_next;
                    if(next_siguiente==-1){
                        //limpio actual
                        tem_anterior.part_next=next_siguiente;
                    }else{
                        //enlazo
                        tem_anterior.part_next=next_siguiente;
                        pos_siguiente=-1;
                    }
                    fseek(archivo,tem_anterior.part_start,SEEK_SET);
                    fwrite(&tem_anterior,sizeof(ebr),1,archivo);
                    cout<<"Se Elimino la particion corrrectamente"<<endl;

                }
                existe_particion=true;
            }else{
                tem_anterior=tem;
                pos_siguiente=tem.part_next;
                if(pos_siguiente!=-1){
                    fseek(archivo,pos_siguiente,SEEK_SET);
                    fread(&tem,sizeof(ebr),1,archivo);
                }

            }

            contador++;
        }else{
            tem_anterior=tem;
            fseek(archivo,tem.part_next,SEEK_SET);
            fread(&tem,sizeof(ebr),1,archivo);
            contador++;
        }
    }
    fclose(archivo);
    if(!existe_particion){
        cout<<"La particion no se encuentra en el disco"<<endl;
        return 0;
    }

    return 0;

}
int Comandos::Agregar_Logica(int unidad, int tamanio, QString nombre, QString ruta, int inicio_ext, int size_ext){
    FILE *archivo;
    archivo=fopen(ruta.toStdString().c_str(),"rb+");

    if(archivo==NULL){
        cout<<"Error: no existe el disco especificado"<<endl;
        return 0;
    }
    ebr tem;
    fseek(archivo,inicio_ext,SEEK_SET);
    fread(&tem,sizeof(ebr),1,archivo);

    //fclose(archivo);

    QByteArray array = nombre.toLocal8Bit();
    char* nom = array.data();

    int agrega=unidad*tamanio;

    int pos_siguiente=0;
    int disponible=0;
    bool encontre=0;
    while(pos_siguiente!=-1){
        if(tem.part_status=='1'){
            if(strcasecmp(tem.part_name,nom)==0){
                pos_siguiente=tem.part_next;
                if(pos_siguiente!=-1){
                    ebr siguiente;
                    fseek(archivo,pos_siguiente,SEEK_SET);
                    fread(&siguiente,sizeof(ebr),1,archivo);
                    disponible=siguiente.part_start-(tem.part_start+tem.part_size);
                    pos_siguiente=-1;

                }else{
                    disponible=(inicio_ext+size_ext)-(tem.part_start+tem.part_size);

                }
                encontre=true;
                break;
            }
        }
        pos_siguiente=tem.part_next;
        if(pos_siguiente!=-1){
            fseek(archivo,pos_siguiente,SEEK_SET);
            fread(&tem,sizeof(ebr),1,archivo);
        }
    }
   if(encontre==true){
       if(agrega>0){
           if(agrega<disponible){
               tem.part_size=tem.part_size+agrega;
               fseek(archivo,tem.part_start,SEEK_SET);
               fwrite(&tem,sizeof(ebr),1,archivo);
               cout<<"Espacio agrega con exito"<<endl;
           }else{
               cout<<"No se puede agregar espacio debido a que no es suficiente"<<endl;
               return 0;
           }
       }else if(agrega<0){
           agrega=agrega*-1;
           if(agrega<tem.part_size){
               tem.part_size=tem.part_size-agrega;
               fseek(archivo,tem.part_start,SEEK_SET);
               fwrite(&tem,sizeof(ebr),1,archivo);
               cout<<"Espacio quitado con exito"<<endl;
           }else{
               cout<<"No se puede quitar el espacio debido a que la particion queda con espacio negativo"<<endl;
               return 0;
           }
       }else{
           return 0;
       }

   }else{
       cout<<"La particion especificada para agregar o quitar espacio no existe en el disco"<<endl;
       return 0;
   }
   // fclose(archivo);

   archivo=fopen(ruta.toStdString().c_str(),"rb+");

   if(archivo==NULL){
       cout<<"Error: no existe el disco especificado"<<endl;
       return 0;
   }
   fseek(archivo,tem.part_start,SEEK_SET);
   fwrite(&tem,sizeof(ebr),1,archivo);
   fclose(archivo);
   return 0;

}


//-------------------------- SEGUNDA FASE DE ARCHIVOS----------------------------------------------


