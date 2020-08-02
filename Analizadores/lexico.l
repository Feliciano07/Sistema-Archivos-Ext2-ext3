/*Area defines que acepta el lenguaje*/

%option case-insensitive 
%option yylineno
%option header-file="scanner.h"
%option outfile="scanner.cpp"
%option noyywrap
%option nounput

%{
    //Area de definicion de cabeceras y pariables

    #include "parser.h"
    #include <cstdlib>
    #include <string>
    #include <iostream>
    #include "nodo.h"
    #include <cstdlib>
    #include <stdio.h>

    //Variables para llevar el control de la parte lexica
    int yylex();
    void yyerror(const char *s);
    int linea=1;
    int columna=1;
    int anterior=0;

    Nodo *raiz;//Raiz donde empieza la parte lexica;

    void imprimirEnConsola (const char *mensaje,const char *tkn){
       // std::cout<<mensaje<<" : "<<tkn<<std::endl;
    }

%}

/* expresiones regulares*/

numero [0-9]+
numeroN "-"{numero}

iden [a-zñA-ZÑ]+

cadena "\""[^"\""]*"\""
cadena2 ([a-zA-Z]\:)?[a-zA-Z0-9\/s_\/.\/-\/(\/):]+


otros \t|\r|\f

%x COM

%%
INITIAL
{
    "#"             {BEGIN COM;}
    {otros}         {/*igore white espace*/}
    

    {iden}        {imprimirEnConsola ("iden",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return iden;}
  

    {numero}      {imprimirEnConsola ("numero",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return numero;}

    {cadena}     {imprimirEnConsola("cadena",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return cadena;}

    {cadena2}       {imprimirEnConsola("cadena2",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return cadena2;}

    {numeroN}      {imprimirEnConsola ("numeroN",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return numeroN;}



     "="         {imprimirEnConsola("igual",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return Tigual;}

     "-"              {imprimirEnConsola("guion",yytext);
                   columna=columna+anterior;  anterior=strlen(yytext);    strcpy(yylval.TEXT,yytext); return Tguion;}

      " "             {columna=columna+anterior; anterior=strlen(yytext);  strcpy(yylval.TEXT,yytext); }

     "\n"            {linea++; columna=0;anterior=0;}

}

<COM>"\n"      {linea++; columna=0; BEGIN INITIAL;}
<COM>.;      
%%
