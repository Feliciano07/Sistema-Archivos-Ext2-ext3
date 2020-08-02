%{
//Area de encabezado y codigo

#include "scanner.h"
#include "nodo.h"
#include <iostream>


extern int yylineno; //linea actual donde se encuentra el parse (analisis lexico) lo maneja bison
extern int columna;//columna actual donde se encuentra el parse(analisi lexico) lo maneja bison
extern int linea;
extern char *yytext; //lexema actual donde esta el parser (analisis lexico) lo maneja BISON

extern Nodo *raiz;

int yyerror(const char* mens)
{

    std::cout << mens <<" "<<yytext<<" Linea: "<<linea<<" Columna: "<<columna<< std::endl;
    return 0;
}

%}

//error-verbose si se especifica la opcion los errores sintacticos son especificados por BISON
%defines "parser.h"
%output "parser.cpp"
%error-verbose
%locations

%union{
//se especifican los tipo de valores para los no terminales y lo terminales
char TEXT [256];
class Nodo *node;
}

//Terminales
%token<TEXT> numero;
%token<TEXT> numeroN;
%token<TEXT> cadena;
%token<TEXT> cadena2;
%token<TEXT> montar;
%token<TEXT> Tigual;
%token<TEXT> Tguion;
%token<TEXT> iden;

//Non terminales
%type<node> init;
%type<node> Lista_Comandos;
%type<node> Comando;
%type<node> Lista_Ejecutar;
%type<node> Parametro;

%start init
%%


init: Lista_Comandos{
        raiz=$1;
    };  

Lista_Comandos: Lista_Comandos Comando{
                    $$=$1;
                    $$->add(*$2);
                }
               |Comando{
                   $$=new Nodo(@1.first_line, @1.first_column,"Lista_Comandos","Lista_Comandos");
                   
                   $$->add(*$1);
               };

Comando: iden Lista_Ejecutar{
            $$=new Nodo(@1.first_line, @1.first_column,$1,$1);
            
            $$->add(*$2);
        }
        |iden{
             $$=new Nodo(@1.first_line, @1.first_column,$1,$1);
             Nodo *nodito=new Nodo(@1.first_line, @1.first_column,$1,$1);
             $$->add(*nodito);
        };


Lista_Ejecutar: Lista_Ejecutar Parametro{
                    $$=$1;
                    $$->add(*$2);
                }
                |Parametro{
                    $$=new Nodo(@1.first_line, @1.first_column,"Lista_Ejecutar","Lista_Ejecutar");
                    $$->add(*$1);
                };

Parametro: Tguion iden Tigual iden{
            $$=new Nodo(@1.first_line, @1.first_column,$2,$4);
            }
           |Tguion iden Tigual cadena{
               $$=new Nodo(@1.first_line, @1.first_column,$2,$4);
           }
           |Tguion iden Tigual cadena2{
               $$=new Nodo(@1.first_line, @1.first_column,$2,$4);
           }
           |Tguion iden Tigual numero{
               $$=new Nodo(@1.first_line, @1.first_column,$2,$4);
           }
           |
           Tguion iden Tigual numeroN{
            $$=new Nodo(@1.first_line, @1.first_column,$2,$4);
           }
           |Tguion iden{
               $$=new Nodo(@1.first_line, @1.first_column,$2,$2);
           }
           ;

