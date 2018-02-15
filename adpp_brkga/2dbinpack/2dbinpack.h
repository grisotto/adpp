#ifndef TWODBINPACK_DEFINE
#define TWODBINPACK_DEFINE

#include "maxitems.h"
#include "boolean.h"

typedef struct 
{
  int x,y;
  int ind; /* indice do retangulo no vetor original */
  int px,py; /* posicao onde foi empacotado */
  int np; /* numero da placa onde foi empacotado */
  /*  int girado; *//* 1 se o item foi empacotado girado, 0 caso contrario */
  int value; // usado para o valor do retangulo 
} rectangletype;

typedef struct 
{
  int n; /* numero de retangulos */
  rectangletype *r; /* vetor de retangulos (as rotinas principais
		       nao se preocupam com alocacao e desalocacao) */
  int L,W; /* dimensoes da placa */
  int maiornivel; /* maior nivel que tenta fixar um retangulo e chamar
		   a rotina para empacotar em uma placa recursivamente.
		  Numero tipico = 4 */
  double toleranciaaltura; /* tolerancia a empacotar na altura mais baixa.
			    Parametro em (0,1). 
			    Numero tipico = 0.05 */
  double toleranciaarea; /* Tolerancia de um item deixar muita area 
			    desperdicada logo abaixo dele. Dah preferencia
			 a empacotar um retangulo onde o desperdicio de area
			 seja menor que o desperdicio do ponto anterior. 
			 Numero bons igual a 0.005 */
  double toleranciaordenacao; /* quando for ordenar os retangulos,
				 coloca os mais altos primeiro, mas se
				 dois retangulos tiverem altura menor
				 que toleranciaaltura * W, entao o
				 que vem primeiro eh o com maior x.
			      Um numero interessante eh 
			      toleranciaaltura/2 (mas nao eh certo que eh
			      melhor.) */
  double value;
} twodbinpacktype;


void init2dbinpack(twodbinpacktype *p,
		   int L,int W,
		   int n, /* numero de retangulos */
		   rectangletype *r,/* vetor de retanglos */
           double toleranciaaltura);   

void desenhaempacotamento(twodbinpacktype *p);


boolean ehempacotamentoviavel(twodbinpacktype *p);

/* empacota itens que ainda nao foram empacotados em uma
   nova placa de numero np */
boolean empacotaumaplaca(twodbinpacktype *p,
	 int np, /* numero da placa a ser empacotada */
	 int *areaempacotada);

boolean empacotaplacas(twodbinpacktype *p,
		       int *numeroplacas);/*num. de placas usadas pelo emp.*/

#endif
