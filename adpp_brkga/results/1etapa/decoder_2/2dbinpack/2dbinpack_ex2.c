#include "2dbinpack.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>

int tamanhoaleatorio(int T,double fat)
{
  return((int) ((((double) drand48() * 
		  fat * T * (1-0.1)))+((int) (T*0.1))));
}


int main()
{
  rectangletype r[MAXITEMS],*pr[MAXITEMS];
  
  int i,n,L,W,area,nplacas,minplacas;
  twodbinpacktype p;

  long tempoinicio,tempo;
                      // tempo = 1101261685; 
                      // Este eh uma excelente instancia. 
                      // Da exatamente 25 placas
                      // com n=300, L,W=100+(int) (drand48()*1000);
                      // tol.alt=0.05, tol.area=0.005,maiornivel=4,
                      // tol.ord=tol.alt * W/2, NTALT = 10, NTAREA=10
                      // cada dimensao gerada com 
                      // tamanhoaleatorio(L ou W,.5)
  tempoinicio = time(0);

  tempo = time(0);
  srand48((int) tempo);
  printf("Semente usada = %d\n",(int) tempo);
  n = 20;
  /*  L = 285;  W = 465;*/
  /*L = 100+(int) (drand48()*1000);*/
  L = 100;
  /*W = 100+(int) (drand48()*1000);*/
  W = 100;
  area = 0;
  for (i=0;i<n;i++){
    r[i].x = tamanhoaleatorio(L,.4);
    r[i].y = tamanhoaleatorio(W,.4);
    r[i].np = -1;
    r[i].ind = i;
    area += r[i].x * r[i].y;
    printf("Rectangle (%d , %d ) ind=%d, value= %d \n", r[i].x, r[i].y, r[i].ind, area);
  }

  minplacas = INT_MAX;
  /*minplacas = 1;*/

  init2dbinpack(&p,L,W,n,r, 0.0);
  if(empacotaumaplaca(&p, 0, &area)){
    
      printf("Empacotamento gerado nao eh viavel\n");
  
  /*if (empacotaplacas(&p,&nplacas)) {*/
    /*if (!ehempacotamentoviavel(&p)){*/
      /*printf("Empacotamento gerado nao eh viavel\n");*/
    /*}else{*/
      /*printf("Empacotou em %d placas (LB=%d)\n",*/
		 /*nplacas,(int) ceil((double) area/((double) L*W)));*/
      desenhaempacotamento(&p);
    
  } else {
    printf("Nao conseguiu empacotar todos os itens\n");
    printf("Retangulos nao empacotados na placa %d x %d\n",L,W);
    for (i=0;i<n;i++) {
      if (pr[i]->np==-1) 
	printf("(%3d,%3d) \n",pr[i]->x,pr[i]->y);
    }
  }
  return(0);
}

