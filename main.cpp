#include <QCoreApplication>

#include <iostream>
#include<string>
#include<fstream>
#include "Analizadores/parser.h"
#include "Analizadores/scanner.h"
#include "comandos.h"
#include <stdlib.h>
#include "QString"
#include "QFile"
#include <QTextStream>
#include <fstream>

using namespace std;

int varLetra;
mount montar_repo[50];
login usuario;

mount temporal;
partitio tem_part;

extern int yyparse();
extern Nodo *raiz;
extern int linea; // Linea del token
extern int columna; // Columna de los tokens
extern int yylineno;


int valor=1;
char cadena1[500];


void Ejecutar_Resto(QString entrada){
    QFile archivo(entrada);
    QString datos="";
    if (archivo.open(QIODevice::ReadOnly|QIODevice::Text)) {
       //ui->entrada->setPlainText(QString::fromUtf8(archivo.readAll()));
       datos=QString::fromUtf8(archivo.readAll());

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

    }else{
        cout<<"ERROR:: NO se puede ejecutar exec porque la ruta no existe"<<endl;
    }



}


void Ejecutar_exec(){

    ofstream archivo;
    archivo.open("tem.txt",ios::out);

    if(archivo.fail()!=true){
        archivo<<cadena1;
        archivo.close();

        //Archivo para mandarlo al parse

        const char *x="tem.txt";

        FILE *input=fopen(x,"r");
        yyrestart(input);

        if(yyparse()==0){

            //cout<<"Se procedera a leer la ruta"<<endl;
            Comandos *exej=new Comandos();
            QString salida=exej->Leer_Principales(raiz);

            if(QString::compare(salida,"",Qt::CaseInsensitive)!=0){// se verifica la ejecucion de exec
                Ejecutar_Resto(salida);
            }
            delete exej;

        }else{
            cout<<"Se encontraron errores en el comando ingresado"<<endl;
        }
    }

}



int main(int argc, char *argv[])
{
    varLetra=0;
    usuario.sesion='0';



    cout<<"-----------Sistema de archivos---------------"<<endl;

        cout<<"Fernando Feliciano Chajon del Cid 201701089"<<endl<<endl<<endl;



        while(valor!=0){
            cout<<"Ingrese el comando para ejecutar"<<endl;
            cin.getline(cadena1,sizeof(cadena1));
            Ejecutar_exec();

        }

        return 0;
}
