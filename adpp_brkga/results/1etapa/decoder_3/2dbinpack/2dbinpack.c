#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include "boolean.h"
#include "figura.h"
#include <time.h>
#include "2dbinpack.h"

#define MAXPOINTS   (2*MAXITEMS)
int maiornivel=4,glob=0,testaviabilidade;
double toleranciaordenacaoglobal;

typedef struct 
{
  int x,y;
  int next, previous; /* tem valor -1 se não tem next ou previous */
  /* forma uma lista duplamente ligada */
} tipopontofronteira;

typedef struct 
{
  int L,W; /* dimensão da placa */
  tipopontofronteira p[MAXPOINTS];
  int firstpoint; /* primeiro ponto da lista duplamente ligada */
  int nextfreepoint; /* primeiro ponto da lista ligada (simples) de 
			pontos livres */
  /* toleranciaaltura: parametro em (0,1) que verifica a tolerancia de um 
     item usar a regra de busca pelo ponto mais baixo. Se o 
     parametro for muito baixo, ele deve privilegiar colocar o retangulo
     mais baixo possivel. Se for 0, ele vai necessariamente colocar o
     retangulo mais baixo (vira o BL - Bottom Leftmost). Numeros bons sao
     valores abaixo de 0.1 */
  double toleranciaaltura; 

  /* toleranciaarea: parametro em (0,1) que verifica a tolerancia de 
     um item deixar area desperdicada logo abaixo dele. Dah preferencia
     a empacotar um retangulo onde o desperdicio de area seja menor.
     Se for 0, soh empacota um item se o desperdicio de area for menor
     que o desperdicio do ponto anterior. Numeros bons são valores abaixo
     de 0.05 */
  double toleranciaarea;

  double toleranciaordenacao; /* quando for ordenar os retangulos,
				 coloca os mais altos primeiro, mas se
				 dois retangulos tiverem altura menor
				 que toleranciaaltura * W, entao o
				 que vem primeiro eh o com maior x */
} tipofronteira;




void init2dbinpack_p(twodbinpacktype *p,
		     int L,int W,
		     int n, /* numero de retangulos */
		     rectangletype *r, /* vetor de retanglos */
		     int maiornivel, 
		     double toleranciaaltura,
		     double toleranciaarea,
		     double toleranciaordenacao)
{
  p->n = n;
  p->r = r;
  p->L = L;
  p->W = W;
  p->maiornivel = maiornivel;
  p->toleranciaaltura = toleranciaaltura;
  p->toleranciaarea = toleranciaarea;
  p->toleranciaordenacao = toleranciaordenacao;
}

void init2dbinpack(twodbinpacktype *p,
		   int L,int W,
		   int n, /* numero de retangulos */
		   rectangletype *r,
           double toleranciaaltura) /* vetor de retanglos */
{
  init2dbinpack_p(p,L,W,n,r,4,toleranciaaltura,005,0.025);
}



void printfronteira(tipofronteira *f)
{
  int i;
  printf("Pontos da fronteira:\n");
  for (i=f->firstpoint;i!=-1;i=f->p[i].next) 
    printf("[%d]=(%d,%d) ",i,f->p[i].x,f->p[i].y);
  printf("\n");
  printf("Indices de pontos livres:\n");
  for (i=f->nextfreepoint;i!=-1;i=f->p[i].next)
    printf("%d ",i);
  printf("\n");
}




void desenhafronteira(tipofronteira *f,
		      int n, /* numero de retangulos */
		      rectangletype **r,
		      int placa) /* numero da placa a ser desenhada */
{
  int i,MaxW,d,iant;
  char numero[100];
  figuretype fig;

  if (f->W==INT_MAX) {
    MaxW = 0;
    for (i=f->firstpoint;i!=-1;i=f->p[i].next){
      if (MaxW < f->p[i].y) MaxW = f->p[i].y;
    }
    MaxW = (5*MaxW)/4;  /* só para aumentar de um fator */
  } else MaxW = f->W;
  init_figure(&fig);

  /* desenha a borda */
  insert_figure_line(&fig,0,0,0,MaxW,RED,1); /* lado esquerdo */
  insert_figure_line(&fig,0,0,f->L,0,RED,1); /* lado inferior */
  insert_figure_line(&fig,f->L,0,f->L,MaxW,RED,1); /* lado direito */
  insert_figure_line(&fig,f->L,MaxW,0,MaxW,RED,1); /* lado superior */

  /* desenha a fronteira*/
  d = 5; /* valor de d soh para desenhar a fronteira um pouquinho acima
	   dos retangulos */
  iant = f->firstpoint;
  i = f->p[iant].next;
  while (i!=-1){
    insert_figure_line(&fig,f->p[iant].x,f->p[iant].y+d,
		            f->p[i].x,   f->p[i].y+d,
   		            BLUE,30);
    iant = i;
    i = f->p[i].next;
  }
  /* desenha os pontos*/
  for (i=f->firstpoint;i!=-1 ; i=f->p[i].next){
    insert_figure_point(&fig,f->p[i].x,f->p[i].y,BLUE,20);
  }
  /* desenha os retangulos */
   for (i=0;i<n ; i++){
     if (r[i]->np == placa) {
       insert_figure_rectangle(&fig,(double) r[i]->px,(double) r[i]->py,
	     (double) r[i]->px + r[i]->x,(double) r[i]->py + r[i]->y,GREEN);
       sprintf(numero,"%d",i+1);
       insert_figure_text(&fig,r[i]->px+r[i]->x/20,r[i]->py+r[i]->y/20,
			  COURIER,FIG_TEXT_SIZE_4,RED,numero);
     }
   }
   view_figure(&fig);
}
void desenhaempacotamento(twodbinpacktype *p)
{
  int i,MaxW,maxplacas,indplaca,area,areaacum,areatotal,m,macum;
  char numero[100];
  figuretype fig;

  /* pega o numero de placas usadas pelo empacotamento */
  maxplacas = -1;
  areatotal = 0;
  for (i=0;i<p->n;i++) {
    if (p->r[i].np > maxplacas) 
      maxplacas = p->r[i].np;
    areatotal += p->r[i].x * p->r[i].y;
  }
  maxplacas++;
  

  if (p->W==INT_MAX) {
    MaxW = 0;
    for (i=0;i<p->n;i++){
      if (MaxW < p->r[i].y) MaxW = p->r[i].y;
    }
    MaxW = (5*MaxW)/4;  /* só para aumentar de um fator */
  } else MaxW = p->W;

  areaacum = 0;
  macum=0;
  for (indplaca=0;indplaca < maxplacas; indplaca++) {
    init_figure(&fig);

    /* desenha a borda */
    insert_figure_line(&fig,0,0,0,MaxW,RED,2); /* lado esquerdo */
    insert_figure_line(&fig,0,0,p->L,0,RED,2); /* lado inferior */
    insert_figure_line(&fig,p->L,0,p->L,MaxW,RED,2); /* lado direito */
    insert_figure_line(&fig,p->L,MaxW,0,MaxW,RED,2); /* lado superior */

    area = 0;
    m = 0;
    /* desenha os retangulos */
    for (i=0;i<p->n ; i++){
      if (p->r[i].np == indplaca) {
	insert_figure_rectangle(&fig,(double) p->r[i].px,(double) p->r[i].py,
	     (double) p->r[i].px + p->r[i].x,(double) p->r[i].py + p->r[i].y,GREEN);
	sprintf(numero,"%d(%dx%d)",p->r[i].ind,p->r[i].x,p->r[i].y);
	insert_figure_text(&fig,p->r[i].px+p->r[i].x/20,p->r[i].py+p->r[i].y/20,
			   COURIER,FIG_TEXT_SIZE_4,RED,numero);
	area += p->r[i].x * p->r[i].y;
	area += p->r[i].value;
	m++;
      }
    }
    areaacum += area;
    macum += m;
    sprintf(numero,"Placa %d",indplaca+1);
    insert_figure_text(&fig,10,-30,COURIER,FIG_TEXT_SIZE_8,RED,numero);
    sprintf(numero,"Area ocupada %d (%6.2lf), No. retangulos = %d",area,(double) area/(p->L*p->W),m);
    insert_figure_text(&fig,10,-60,COURIER,FIG_TEXT_SIZE_8,RED,numero);
    sprintf(numero,"Tamanho da placa: %d x %d",p->L,p->W);
    insert_figure_text(&fig,10,-90,COURIER,FIG_TEXT_SIZE_8,RED,numero);
    //sprintf(numero,"Semente usada: %d",(int) tempo);
    //insert_figure_text(&fig,10,-120,COURIER,FIG_TEXT_SIZE_8,RED,numero);
    /*view_figure(&fig, indplaca+1);*/
    char *filename;
    asprintf(&filename, "figure_%d.eps", indplaca+1);
    save_epsfigure(&fig,filename); 
  }
  /*if ((macum!=p->n) || (areaacum != areatotal)) {*/
    /*printf("Erro no empacotamento (conferencia nao bate)\n");*/
    /*printf("macum = %d, n=%d, areaacum = %d, areatotal = %d\n",*/
	   /*macum,p->n,areaacum,areatotal);*/
  /*}*/
}


boolean empacotamentoviavel(int n, /* numero de retangulos */
			  int L,int W,
			  rectangletype **r,
			    int origx,int origy)
{
  int i,j;
  rectangletype rr[MAXITEMS];

  if (!testaviabilidade) return(true);

  for (i=0;i<n;i++) {
    rr[i] = *r[i];
  }
  

  /* pega o numero de placas usadas pelo empacotamento */
  for (i=0;i<n;i++) {
    if (r[i]->np != -1) {
      if (r[i]->px < 0) goto resultadofalso1;
      if (r[i]->py < 0) goto resultadofalso1;
      if (r[i]->px+r[i]->x - origx > L) goto resultadofalso1;
      if (r[i]->py+r[i]->y - origy > W) goto resultadofalso1;
    }
  }
  
  for (i=0;i<n;i++) {
    for (j=i+1;j<n;j++) {
      if ((r[i]->np==r[j]->np) && (r[i]->np != -1)) {
	if (!((r[i]->px + r[i]->x <= r[j]->px) || // i esta a esquerda de j
	    (r[j]->px + r[j]->x <= r[i]->px) || // j esta a esquerda de i
	    (r[i]->py + r[i]->y <= r[j]->py) || // i esta abaixo de j
	    (r[j]->py + r[j]->y <= r[i]->py)))  // j esta abaixo de i
	  goto resultadofalso2;
      }
    }
  }
  return(true);
 resultadofalso1:
  printf("Empacotamento de (ind=%d %dx%d np=%d) (%d,%d) em placa %dx%d\n",
	 r[i]->ind,r[i]->x,r[i]->y,r[i]->np,r[i]->px,r[i]->py,L,W);
  return(false);
 resultadofalso2:
  printf("Empacotamento de (%d %dx%d np=%d) (%d,%d) e "
	 "(%d %dx%d np=%d) (%d,%d) interseptam\n",
	 r[i]->ind,r[i]->x,r[i]->y,r[i]->np,r[i]->px,r[i]->py,
	 r[j]->ind,r[j]->x,r[j]->y,r[j]->np,r[j]->px,r[j]->py);
  return(false);
}


boolean ehempacotamentoviavel(twodbinpacktype *p)
{
  int i,j;

  for (i=0;i<p->n;i++) {
    if (p->r[i].np != -1) {
      if (p->r[i].px < 0) goto resultadofalso1;
      if (p->r[i].py < 0) goto resultadofalso1;
      if (p->r[i].px + p->r[i].x > p->L) goto resultadofalso1;
      if (p->r[i].py + p->r[i].y > p->W) goto resultadofalso1;
    }
  }
  
  for (i=0;i<p->n;i++) {
    for (j=i+1;j<p->n;j++) {
      if ((p->r[i].np==p->r[j].np) && (p->r[i].np != -1)) {
	if (!((p->r[i].px + p->r[i].x <= p->r[j].px) || // i esta a esquerda de j
	    (p->r[j].px + p->r[j].x <= p->r[i].px) || // j esta a esquerda de i
	    (p->r[i].py + p->r[i].y <= p->r[j].py) || // i esta abaixo de j
	    (p->r[j].py + p->r[j].y <= p->r[i].py)))  // j esta abaixo de i
	  goto resultadofalso2;
      }
    }
  }
  return(true);
 resultadofalso1:
  printf("Empacotamento de (ind=%d %dx%d np=%d) (%d,%d) em placa %dx%d\n",
	 p->r[i].ind,p->r[i].x,p->r[i].y,p->r[i].np,p->r[i].px,p->r[i].py,p->L,p->W);
  desenhaempacotamento(p);
  return(false);
 resultadofalso2:
  printf("Empacotamento de (%d %dx%d np=%d) (%d,%d) e "
	 "(%d %dx%d np=%d) (%d,%d) interseptam\n",
	 p->r[i].ind,p->r[i].x,p->r[i].y,p->r[i].np,p->r[i].px,p->r[i].py,
	 p->r[j].ind,p->r[j].x,p->r[j].y,p->r[j].np,p->r[j].px,p->r[j].py);
  desenhaempacotamento(p);
  return(false);
}


boolean initfronteira(tipofronteira *f,int L,int W,
		      double toleranciaaltura,
		      double toleranciaarea)
{
  int i;
  f->L = L;
  f->W = W;
  f->firstpoint = 0;
  f->toleranciaaltura = toleranciaaltura;
  f->toleranciaarea = toleranciaarea;
  /* primeiro ponto */
  f->p[0].x = 0;
  f->p[0].y = 0;
  f->p[0].next = 1;
  f->p[0].previous = -1;
  /* segundo ponto */
  f->p[1].x = L;
  f->p[1].y = 0;
  f->p[1].next = -1;
  f->p[1].previous = 0;
  /* construindo a lista ligada de pontos livres */
  f->nextfreepoint = 2;
  for (i=2;i<MAXPOINTS-1;i++) f->p[i].next = i+1;
  f->p[MAXPOINTS-1].next = -1;
  return(true);
}


void liberapontofronteira(tipofronteira *f,int i)
{
  /*  if (i==f->firstpoint) {
    printf("Tentando desalocar o primeiro ponto\n");
    exit(0);
    }*/
  f->p[i].next = f->nextfreepoint;
  f->nextfreepoint = i;
}
int alocapontofronteira(tipofronteira *f)
{
  int i;
  i = f->nextfreepoint;
  if (i==-1) {
    printf("Numero de pontos na fronteira atingiu o maximo.\n");
    exit(0);
  }
  f->nextfreepoint = f->p[i].next;
  return(i);
}

/* retira a redundancia em torno de i. Neste caso, mantem o 
   ponto de indice i */

boolean ehprimeiroponto(tipofronteira *f,int i)
{
  if (f->p[i].previous == -1) return(true);
  return(false);
}

void retiraredundanciacomanterior(tipofronteira *f,int i)
{
  int prev,prevprev,next;
  if (i==-1) return;
  prev = f->p[i].previous;
  next = f->p[i].next;
  if (!ehprimeiroponto(f,i)) { /* se nao for o primeiro ponto */
    /* se o ponto em i for igual ao ponto anterior */
    if ((f->p[i].x == f->p[prev].x) && (f->p[i].y == f->p[prev].y)){
      if (ehprimeiroponto(f,prev)) { /* se anterior eh primeiro ponto */
	liberapontofronteira(f,prev); /* libera o primeiro ponto */
	f->firstpoint = i; /* faz o ponto em i ser o primeiro ponto */
	f->p[i].previous = -1;
      } else {
	prevprev = f->p[prev].previous;
	liberapontofronteira(f,prev);
	f->p[prevprev].next = i;
	f->p[i].previous = prevprev;
      }
    }
  }
}

/* devolve quanto de area estah abaixo da linha
   definida pelos pontos
   (f->p[indponto].x , alt) e (f->p[indponto].x+len , alt)
   e acima da fronteira. 
   Isto permite dizer se o empacotamento de um retangulo
   de comprimento len no ponto indponto vai desperdicar muito
   espaco ou nao. */

int areaperdida(tipofronteira *f,int len,int indponto,int alt) 
{
  int px;
  int i,imax,fimx,areaperd,xant,yant;
  areaperd = f->L*f->W;
  if (indponto==-1) return(areaperd); /* como devolver infinito */
  px = f->p[indponto].x;
  /* enquanto os pontos a direita tiverem a mesma coord. x, pego
     o proximo ponto. Pode estar mudando para um ponto tanto para
     cima como para baixo  */
  i = f->p[indponto].next;
  while ((i!=-1)&&(f->p[i].x==px)) {
    indponto = i;
    i = f->p[i].next;
  }
  if (f->p[indponto].next==-1) { 
    /* se for o ultimo ponto, nao tem como empacotar para a direita */
    return(areaperd); /* como devolver infinito */
  }
  if ((f->L - f->p[indponto].x) < len) {
    /* espaco do ponto ateh a lateral direita nao aguenta o retangulo a
       empacotar */
    return(areaperd); /* como devolver infinito */
  }
  /* percorre os pontos a direita, enquanto o retangulo a ser 
     inserido ficar acima destes pontos */
  imax = f->p[indponto].next; /* pega o proximo ponto a direita */
  fimx = f->p[indponto].x + len;
  areaperd = 0;
  xant = f->p[indponto].x;
  yant = f->p[indponto].y;
  while ((imax!=-1) && (f->p[imax].x <= fimx)) {
    areaperd += (f->p[imax].x - xant) * (alt - f->p[imax].y);
    xant = f->p[imax].x;
    yant = f->p[imax].y;
    imax = f->p[imax].next;
  }
  if (imax!=-1) areaperd += (fimx - xant)*(alt - f->p[imax].y);
  return(areaperd);
}
  


/* devolve a altura corrigida do ponto, quando for empacotar um
   retangulo de comprimento len no ponto indponto */
int alturapontocomprimento(tipofronteira *f,int len,int wid,int indponto)
{
  int px;
  int i,imax,max_wid,fimx;
  if (indponto==-1) return(-1);
  px = f->p[indponto].x;
  /* enquanto os pontos a direita tiverem a mesma coord. x, pego
     o proximo ponto. Pode estar mudando para um ponto tanto para
     cima como para baixo  */
  i = f->p[indponto].next;
  while ((i!=-1)&&(f->p[i].x==px)) {
    indponto = i;
    i = f->p[i].next;
  }
  if (f->p[indponto].next==-1) { 
    /* se for o ultimo ponto, nao tem como empacotar para a direita */
    return(-1);
  }
  if ((f->L - f->p[indponto].x) < len) {
    /* espaco do ponto ateh a lateral direita nao aguenta o retangulo a
       empacotar */
    return(-1);
  }
  if ((f->W - f->p[indponto].y) < wid) {
    /* espaco do ponto ateh o lado superior nao aguenta o retangulo a
       empacotar */
    return(-1);
  }
  /* percorre os pontos a direita, enquanto o retangulo a ser 
     inserido ficar acima destes pontos */
  imax = f->p[indponto].next; /* pega o proximo ponto a direita */
  max_wid = f->p[indponto].y;
  fimx = f->p[indponto].x + len;
  while ((imax!=-1) && (f->p[imax].x <= fimx)) {
    if (f->p[imax].y > max_wid) max_wid = f->p[imax].y;
    imax = f->p[imax].next;
  }
  if (max_wid + wid > f->W) return(-1);
  return(max_wid);
}



/* insere um retangulo de dimensoes (len x wid) numa fronteira.
   f        : fronteira (entrada)
   indponto : indice do ponto onde o retangulo será empacotado (entrada)
   x,y      : coordenadas do ponto onde o retângulo foi empacotado 
   nextpoint: indice do proximo ponto na fronteira onde o item serah
              empacotado.
*/
boolean inserefronteiradireita(tipofronteira *f,int indponto,
			int len,int wid,int *x,int *y,int *nextpoint)
{
  int px;
  int i,imax,max_wid,j,newpoint,newpoint2,fimx;
  if (indponto==-1) return(false);
  px = f->p[indponto].x;
  /* enquanto os pontos a direita tiverem a mesma coord. x, pego
     o proximo ponto. Pode estar mudando para um ponto tanto para
     cima como para baixo  */
  i = f->p[indponto].next;
  while ((i!=-1)&&(f->p[i].x==px)) {
    indponto = i;
    i = f->p[i].next;
  }
  if (f->p[indponto].next==-1) { 
    /* se for o ultimo ponto, nao tem como empacotar para a direita */
    return(false);
  }
  if ((f->L - f->p[indponto].x) < len) {
    /* espaco do ponto ateh a lateral direita nao aguenta o retangulo a
       empacotar */
    return(false);
  }
  /* percorre os pontos a direita, enquanto o retangulo a ser 
     inserido ficar acima destes pontos */
  imax = f->p[indponto].next; /* pega o proximo ponto a direita */
  max_wid = f->p[indponto].y;
  fimx = f->p[indponto].x + len;
  while ((imax!=-1) && (f->p[imax].x <= fimx)) {
    if (f->p[imax].y > max_wid) max_wid = f->p[imax].y;
    imax = f->p[imax].next;
  }
  if (max_wid + wid > f->W) {
    /* espaco para colocar do ponto ateh a borda superior da placa
       eh insuficiente para colocar o retangulo */
    return(false);
  }
  /* atualiza os parametros de retorno, com valores das posicoes
     de onde o retangulo vai ser empacotado */
  *x = f->p[indponto].x;
  *y = max_wid;
  i = f->p[indponto].next; 
  while (i!=imax) { /* se o imax = -1, o ultimo ponto tambem eh liberado */
    j = i;
    i = f->p[i].next;
    liberapontofronteira(f,j);
  }
  /* atualiza o primeiro ponto (canto superior esquerdo 
     do retangulo empacotado) */
  /* a coordenada x continua a mesma */
  f->p[indponto].y = max_wid + wid;

  /* canto superior direito do retangulo empacotado */
  newpoint = alocapontofronteira(f); 
  f->p[indponto].next = newpoint; 
  f->p[newpoint].previous = indponto;
  f->p[newpoint].x = f->p[indponto].x + len; 
  f->p[newpoint].y = f->p[indponto].y;

  if (imax==-1) { /* o empacotamento deste retangulo atingiu
		     a lateral direita do retangulo */
    f->p[newpoint].next = -1;
  } else {
    /* canto inferior direito do retangulo empacotado */
    newpoint2 = alocapontofronteira(f); 
    f->p[newpoint].next = newpoint2; 
    f->p[newpoint2].previous = newpoint;
    f->p[newpoint2].x = f->p[newpoint].x; 
    f->p[newpoint2].y = f->p[imax].y;
    f->p[newpoint2].next = imax;
  
    f->p[imax].previous = newpoint2;
  }
  
  //  retiraredundanciacomanterior(f,indponto);
  //  retiraredundanciacomanterior(f,f->p[indponto].next);
  //  retiraredundanciacomanterior(f,f->p[f->p[indponto].next].next);
  
  return(true);
} 


int pontomaisbaixo(tipofronteira *f,int len,int wid)
{
  int i,ymin,imin,alt,desperdicio,desperdicioimin;
  imin = -1;
  ymin = INT_MAX;
  desperdicioimin = f->L * f->W;
  for (i=f->firstpoint ; i!=-1 ; i=f->p[i].next) {
    alt = alturapontocomprimento(f,len,wid,i);
    /* calcula o desperdicio do ponto */
    desperdicio = areaperdida(f,len,i,alt);
    // printf("Ponto (%d,%d), comprimento %d, altura = %d, "
    // "desperdicio = %lf\n",
    // f->p[i].x,f->p[i].y,len,alt,
    // (double) desperdicio/((double) f->L*f->W));
    if ((alt!=-1) && (alt < ymin- f->toleranciaaltura *f->W)) { 
	/* Valores bons: 0.025 (inst. grandes), 0.07 (inst.pequenas) */
      if (desperdicio < desperdicioimin + f->toleranciaarea*f->L*f->W )
	/* Valores bons: 0.001 (inst. grandes), 0.005 (inst.pequenas) */
	{ /* se o desperdicio for muito grande, nao troco*/
	desperdicioimin = desperdicio;
	ymin = alt;
	imin = i;
      }
    }
  }
  return(imin);
}

int difereeps(int a,int b)
{
  if ((a-toleranciaordenacaoglobal < b) && 
      (a+toleranciaordenacaoglobal > b)) return(1);
  return(0);
}


/* compara dois retangulos, decrescente em y */    
/* Diferente dos outros, este eh usado para ordenar vetor de pointer
   para retangulos */
int  reverserectanglecomparey(void const *c1,void const *c2)
{
	rectangletype *r1,*r2;
	r1 = *((rectangletype **) c1);
	r2 = *((rectangletype **) c2);
	if (difereeps(r1->y,r2->y)) {
	  if (r1->x < r2->x) return(1);
	  if (r1->x > r2->x) return(-1);
	}
	if (r1->y < r2->y) return (1);
	if (r1->y > r2->y) return (-1);
	return (0);
}
/* compara dois retangulos, decrescente em x */    
int  reverserectanglecomparex(void const *c1,void const *c2)
{
	rectangletype *r1,*r2;
	r1 = (rectangletype *) c1;
	r2 = (rectangletype *) c2;
	if (r1->x < r2->x) return (1);
	if (r1->x > r2->x) return (-1);
	return (0);
}
/* compara dois retangulos, decrescente na area */    
int  reverserectanglecomparearea(void const *c1,void const *c2)
{
	rectangletype *r1,*r2;
	r1 = (rectangletype *) c1;
	r2 = (rectangletype *) c2;
	if (r1->x * r1->y < r2->x * r2->y) return (1);
	if (r1->x * r1->y > r2->x * r2->y) return (-1);
	return (0);
}


boolean viavel(int x,int y,int L,int W)
{
  if (x>L) return(false);
  if (y>W) return(false);
  return(true);
}
boolean empacotavel(int x,int y,int L,int W)
{
  if (viavel(x,y,L,W)) return(true);
  /*if (viavel(y,x,L,W)) return(true);*/
  return(false);
}



void trocainteiros(int *a,int *b)
{
  /*int c;*/
  /*c = *a;*/
  /**a = *b;*/
  /**b = c;*/
}



/* empacota em apenas uma placa, a rotina se vira para ver em
 qual orientacao fica melhor. Considera apenas uma estrategia
 bottom-leftmost (com parametros de tolerancia) */
boolean empacotaumaplacabottomleftmost(
	 int n, /* numero de retangulos total no vetor */
	 rectangletype **r, /* vetor de pont. para retang. soh
			     tem retangulos que podem entrar na placa 
			     e que nao foram empacotados */
	 int L,int W, /* dimensoes da placa */
	 int np, /* numero da placa a ser empacotada */
	 double toleranciaaltura,
	 double toleranciaarea,
	 double toleranciaordenacao,
	 int *areaempacotada,
	 int origx,int origy)
{
  int i,nextpoint,ind,empacotados=0;
  tipofronteira f;
  boolean empacotoualgum;
  /* gira itens para ficarem mais altos */
  /*for (i=0;i<n;i++) {*/
    /*if ((r[i]->np==-1) && (r[i]->x > r[i]->y) &&  [> x > y <]*/
	/*((viavel(r[i]->y,r[i]->x,L,W))))  [> eh viavel girar <]*/
      /*trocainteiros(&r[i]->y,&r[i]->x);*/
  /*}*/
  /*toleranciaordenacaoglobal = toleranciaordenacao * W;*/
  /* ordena por altura, as maiores alturas primeiro */
  /*qsort(r,n,sizeof(rectangletype *),reverserectanglecomparey);*/

  initfronteira(&f,L,W,toleranciaaltura,toleranciaarea);
  empacotoualgum = false;
  *areaempacotada = 0;
  for (i=0;i<n;i++){
    //    if (r[i]->np==-1) {
      ind = pontomaisbaixo(&f,r[i]->x,r[i]->y);
      if (inserefronteiradireita(&f,ind,r[i]->x,r[i]->y,
				 &r[i]->px,&r[i]->py,&nextpoint)) {
	r[i]->np = np;
	r[i]->px += origx;
	r[i]->py += origy;
	empacotoualgum = true;
	empacotados++;
	*areaempacotada += r[i]->x * r[i]->y;
	*areaempacotada += r[i]->value;
      } else r[i]->np = -1;
      //    }
  }
  if (empacotoualgum) return(true);
  return(false); /* nao conseguiu emp. nenhum ret. numa placa vazia */
  
  // desenhafronteira(&f,n,r,np);
}



/* empacota em apenas uma placa, mas tenta ver as duas orientacoes
 possiveis para crescer o empacotamento. */
boolean empacotaumaplacasimples(
	 int n, /* numero de retangulos total no vetor */
	 rectangletype **r, /* vetor de pont. para retang. soh
			     tem retangulos que podem entrar na placa 
			     e que nao foram empacotados */
	 int L,int W, /* dimensoes da placa */
	 int np, /* numero da placa a ser empacotada */
	 double toleranciaaltura,
	 double toleranciaarea,
	 double toleranciaordenacao,
	 int *areaempacotada,
	 int origx,int origy)
{
  rectangletype best[MAXITEMS];
  int i,area,bestarea;

  if (!empacotamentoviavel(n,L,W,r,origx,origy)) {
    printf("Problemas. Empacotamento nao eh viavel xx\n");
  }
  
  bestarea = 0;
  empacotaumaplacabottomleftmost(n,r,L,W,np,
				 toleranciaaltura,
				 toleranciaarea,
				 toleranciaordenacao,
				 &area,origx,origy);
  if (!empacotamentoviavel(n,L,W,r,origx,origy)) {
    printf("Problemas. Empacotamento nao eh viavel yy\n");
  }
  
  /* isto pode ser otimizado, pois nao precisa copiar todos */
  if (bestarea < area) {
    bestarea = area;
    for (i=0;i<n;i++) 
      best[i] = *(r[i]);
  }
  /* volta todos os itens como nao empacotado */
  for (i=0;i<n;i++) 
    r[i]->np = -1;

  /* experimenta empacotar com a placa girada */
  /*empacotaumaplacabottomleftmost(n,r,W,L,np,*/
				 /*toleranciaaltura,*/
				 /*toleranciaarea,*/
				 /*toleranciaordenacao,*/
				 /*&area,origx,origy);*/

  /* Se obteve solucao melhor, atualiza */
  /*if (bestarea < area) {*/
    /*for (i=0;i<n;i++) {*/
      /*trocainteiros(&r[i]->x,&r[i]->y);*/
      /*r[i]->px -= origx;*/
      /*r[i]->py -= origy;*/
      /*trocainteiros(&r[i]->px,&r[i]->py);*/
      /*r[i]->px += origx;*/
      /*r[i]->py += origy;*/
    /*}*/
    /**areaempacotada = area;*/
  /*} else {*/
    *areaempacotada = bestarea;
    for (i=0;i<n;i++) 
      *(r[i]) = best[i];
  /*}*/

  if (*areaempacotada > 0) return(1);
  return(0);
}

/* remove do vetor de ponteiros, os retangulos jah empacotados */    
void removeempacotados(int *n, rectangletype **r,int L,int W)
{
  int i,j;
  j=0;
  for (i=0;i<*n;i++) {
    if ((r[i]->np==-1) && (empacotavel(r[i]->x,r[i]->y,L,W))) {
      r[j] = r[i];
      j++;
    } 
  }
  *n = j;
}

int escolheretangulofixo(int n,rectangletype **r)
{
  int ind,i,maiorarea,area;
  ind = -1;
  maiorarea = 0;
  for (i=0;i<n;i++) {
    if (r[i]->np == -1) {
      /*area = r[i]->x * r[i]->y;*/
      area = r[i]->value ;
      if (area > maiorarea) {
	ind = i;
	maiorarea = area;
      }
    }
  }
  return(ind);
}

/* empacota em apenas uma placa, a estrategia eh fixar um dos retangulos
   grandes em um dos cantos e tentar empacotar o resto gilhotinando
   ao lado deste retangulo grande. */
boolean empacotaumaplacanivel(
	 int n, /* numero de retangulos total no vetor */
	 rectangletype **r, /* vetor de pont. para retang. soh
			     tem retangulos que podem entrar na placa 
			     e que nao foram empacotados */
	 int L,int W, /* dimensoes da placa */
	 int np, /* numero da placa a ser empacotada */
	 double toleranciaaltura,
	 double toleranciaarea,
	 double toleranciaordenacao,
	 int *areaempacotada,
	 int origx,int origy,int nivel);


boolean empacotaumaplacacomretangulofixo(
	 int n, /* numero de retangulos total no vetor */
	 rectangletype **r, /* vetor de pont. para retang. soh
			     tem retangulos que podem entrar na placa 
			     e que nao foram empacotados */
	 int L,int W, /* dimensoes da placa */
	 int np, /* numero da placa a ser empacotada */
	 double toleranciaaltura,
	 double toleranciaarea,
	 double toleranciaordenacao,
	 int *areaempacotada,
	 int origx,int origy,int nivel)
{
  int i,m,ind,areaesquerda,areadireita;
  rectangletype best[MAXITEMS],*copiar[MAXITEMS];
  int area,bestarea,xind,yind,rotacao;

  bestarea = 0;
  for (i=0;i<n;i++) {
    if (r[i]->np!=-1) 
      printf("\n\n\nLIXO\n\n\n\n");
  }
  
  if (!empacotamentoviavel(n,L,W,r,origx,origy)) {
    printf("Problemas. Empacotamento de entrada nao eh viavel 0\n");
  }

  ind = escolheretangulofixo(n,r);
  if (ind==-1) return (false);


  /* testa as duas possibilidades de rotacao */
  for (rotacao=0;rotacao<1;rotacao++) {
    /* Faz uma copia do vetor original */
    m = n; /* vou trabalhar com m e com copiar */
    for (i=0;i<n;i++) {
      copiar[i] = r[i];
      r[i]->np = -1;
    }
    if (!empacotamentoviavel(n,L,W,r,origx,origy)) {
      printf("Problemas. Empacotamento nao eh viavel xxxyyy\n");
    }

    if (rotacao==0) { /* empacota o retangulo na orientacao original */
      if ((copiar[ind]->x > L) || (copiar[ind]->y > W)) {
	continue;
      }
    } else { /* rotacao==1 empacota o retangulo girando previamente */
      if ((copiar[ind]->y > L) || (copiar[ind]->x > W)) {
	continue;
      }
      glob++;
      //printf("glob = %d\n",glob);
      trocainteiros(&copiar[ind]->x,&copiar[ind]->y);
    }

    copiar[ind]->px = origx;
    copiar[ind]->py = origy;
    copiar[ind]->np = np;
    xind = copiar[ind]->x;
    yind = copiar[ind]->y;

    /* remove o item que ja foi empacotado e os que nao cabem */
    removeempacotados(&m,copiar,L - xind,W);

    empacotaumaplacanivel(m,copiar,L - xind,W,np,
			  toleranciaaltura,
			  toleranciaarea,
			  toleranciaordenacao,
			  &areaesquerda,
			    origx+xind,origy,nivel+1);

    if (!empacotamentoviavel(n,L,W,r,origx,origy)) {
      printf("Problemas. Empacotamento nao eh viavel 11\n");
    }


    /* volta a ter todos os itens */
    m = n;
    for (i=0;i<n;i++)
      copiar[i] = r[i];

    if (!empacotamentoviavel(n,L,W,r,origx,origy)) {
      printf("Problemas. Empacotamento nao eh viavel 999\n");
    }
    /* retira todos os que foram empacotados no passo 
       anterior e os que nao cabem */
    removeempacotados(&m,copiar,xind,W - yind);

    if (!empacotamentoviavel(n,L,W,r,origx,origy)) {
      printf("Problemas. Empacotamento nao eh viavel 22\n");
    }

    empacotaumaplacanivel(m,copiar,xind,W - yind,np,
			  toleranciaaltura,
			  toleranciaarea,
			  toleranciaordenacao,
			  &areadireita,
			    origx,origy+yind,nivel+1);

    if (!empacotamentoviavel(n,L,W,r,origx,origy)) {
      printf("Problemas. Empacotamento nao eh viavel 2\n");
    }

    /* computa a area resultante */
    area = xind * yind + areaesquerda + areadireita;

    /* isto pode ser otimizado, pois nao precisa copiar todos */
    if (bestarea < area) {
      //printf("Empacotamento anterior. Area = %d\n",bestarea);
      //desenhaempacotamento(n,L,W,best);
      bestarea = area;
      for (i=0;i<n;i++) 
	best[i] = *(r[i]);
      //printf("Numero da placa = %d fixo = (%d,%d)\n",np,xind,yind);
      //printf("Empacotamento novo. Area = %d\n",bestarea);
      //desenhaempacotamento(n,L,W,best);
    }
    /* -------------------------------------------------------- */
    /* Volta os dados originais */
    m = n;
    for (i=0;i<n;i++) {
      copiar[i] = r[i];
      r[i]->np = -1;
    }
    /* Testa outros cortes */
    copiar[ind]->px = origx;
    copiar[ind]->py = origy;
    copiar[ind]->np = np;
    xind = copiar[ind]->x;
    yind = copiar[ind]->y;

    /* remove o item que ja foi empacotado e os que nao cabem */
    removeempacotados(&m,copiar,L,W-yind);

    empacotaumaplacanivel(m,copiar,L,W-yind,np,
			  toleranciaaltura,
			  toleranciaarea,
			  toleranciaordenacao,
			  &areaesquerda,
			    origx,origy+yind,nivel+1);
    if (!empacotamentoviavel(m,L,W-yind,copiar,origx,origy+yind)) {
      printf("Problemas. Empacotamento nao eh viavel 3\n");
    }

    /* volta a ter todos os itens */
    m = n;
    for (i=0;i<n;i++) {
      copiar[i] = r[i];
    }
    
    /* retira todos os que foram empacotados no passo 
       anterior e os que nao cabem */
    removeempacotados(&m,copiar,L-xind,yind);
    if (!empacotamentoviavel(n,L,W,r,origx,origy)) {
      printf("Problemas. Empacotamento nao eh viavel aa\n");
    }
    empacotaumaplacanivel(m,copiar,L-xind,yind,np,
			  toleranciaaltura,
			  toleranciaarea,
			  toleranciaordenacao,
			  &areadireita,
			    origx+xind,origy,nivel+1);

    if (!empacotamentoviavel(m,L-xind,yind,copiar,origx+xind,origy)) {
      printf("Problemas. Empacotamento nao eh viavel 4\n");
    }

    /* computa a area resultante */
    area = xind * yind + areaesquerda + areadireita;
    
    /* isto pode ser otimizado, pois nao precisa copiar todos */
    if (bestarea < area) {
      //printf("Empacotamento anterior. Area = %d\n",bestarea);
      //desenhaempacotamento(n,L,W,best);
      bestarea = area;
      for (i=0;i<n;i++) 
	best[i] = *(r[i]);
      //printf("Numero da placa = %d fixo = (%d,%d)\n",np,xind,yind);
      //printf("Empacotamento novo. Area = %d\n",bestarea);
      //desenhaempacotamento(n,L,W,best);
    }
    /* -------------------------------------------------------- */
  }
  /* Copia a melhor solucao */
  *areaempacotada = bestarea;
  for (i=0;i<n;i++) 
    *(r[i]) = best[i];

  if (!empacotamentoviavel(n,L,W,r,origx,origy)) {
    printf("Problemas. Empacotamento nao eh viavel 5\n");
  }

  if (*areaempacotada > 0) return(1);
  return(0);
}

  

/* empacota em apenas uma placa, a estrategia eh fixar um dos retangulos
   grandes em um dos cantos e tentar empacotar o resto gilhotinando
   ao lado deste retangulo grande. */
boolean empacotaumaplacanivel(
	 int n, /* numero de retangulos total no vetor */
	 rectangletype **r, /* vetor de pont. para retang. soh
			     tem retangulos que podem entrar na placa 
			     e que nao foram empacotados */
	 int L,int W, /* dimensoes da placa */
	 int np, /* numero da placa a ser empacotada */
	 double toleranciaaltura,
	 double toleranciaarea,
	 double toleranciaordenacao,
	 int *areaempacotada,
	 int origx,int origy,int nivel)
{
  int i;
  rectangletype best[MAXITEMS],copia[MAXITEMS],*copiar[MAXITEMS];
  int area,bestarea;
  for (i=0;i<n;i++) {
    if (!(viavel(r[i]->x,r[i]->y,L,W))) 
      trocainteiros(&r[i]->x,&r[i]->y);
    if (!(empacotavel(r[i]->x,r[i]->y,L,W))){
      printf("Entrada na rotina empacotaumaplaca com itens inviaveis\n");
    }
  }

  bestarea = 0;
  /* faz uma copia dos dados de entrada */
  for (i=0;i<n;i++) 
      copia[i] = *(r[i]);

  /* -------------------------------------------------------- */
  empacotaumaplacasimples(n,r,L,W,np,
			  toleranciaaltura,
			  toleranciaarea,
			  toleranciaordenacao,
			  &area,origx,origy);
  /* isto pode ser otimizado, pois nao precisa copiar todos */
  if (bestarea < area) {
    bestarea = area;
    for (i=0;i<n;i++) 
      best[i] = *(r[i]);
  }
  /* volta os dados originais */
  for (i=0;i<n;i++) {
    //    r[i]->np = -1;
    //    r[i]->px = -1; 
    //    r[i]->py = -1;
    *(r[i]) = copia[i];
    copiar[i] = r[i];
  }
  if (nivel < maiornivel) {
    empacotaumaplacacomretangulofixo(n,r,L,W,np,
				     toleranciaaltura,
				     toleranciaarea,
				     toleranciaordenacao,
				     &area,origx,origy,nivel);
    /* isto pode ser otimizado, pois nao precisa copiar todos */
    if (bestarea < area) {
      bestarea = area;
      for (i=0;i<n;i++) 
	best[i] = *(r[i]);
    }
  }
  /* Copia a melhor solucao */
  *areaempacotada = bestarea;
  for (i=0;i<n;i++) 
    *(r[i]) = best[i];
  if (*areaempacotada > 0) return(1);
  return(0);
}

boolean empacotaumaplaca_p(
	 int n, /* numero de retangulos total no vetor */
	 rectangletype **r, /* vetor de pont. para retang. soh
			     tem retangulos que podem entrar na placa 
			     e que nao foram empacotados */
	 int L,int W, /* dimensoes da placa */
	 int np, /* numero da placa a ser empacotada */
	 double toleranciaaltura,
	 double toleranciaarea,
	 double toleranciaordenacao,
	 int *areaempacotada)
{
  return(empacotaumaplacanivel(n,r,L,W,np,toleranciaaltura,
			       toleranciaarea,
			       toleranciaordenacao,
			       areaempacotada,0,0,0));
}


boolean empacotaumaplaca(
	 twodbinpacktype *p,
	 int np, /* numero da placa a ser empacotada */
	 int *areaempacotada)
{
  rectangletype *pr[MAXITEMS];
  int i,n;
  n=0;
  int value=0;
  for (i=0;i<p->n;i++) {
    if (p->r[i].np==-1) {
      pr[n] = &p->r[i];
      n++;
    }
   
  }
  boolean resul =empacotaumaplaca_p(n,pr,p->L,p->W,np,
			    p->toleranciaaltura,
			    p->toleranciaarea,
			    p->toleranciaordenacao,
                areaempacotada);

  p->value = 0;
  for (i=0;i<p->n;i++) {
    if(p->r[i].np == np){
        /*value+= p->r[i].value; */
    //added to return value total packed
        p->value += p->r[i].value;
   }
  }

  /*printf("total value: %d \n", value);*/
  return(resul);
}





/* Algoritmo devolve true se conseguiu fazer um empacotamento que
   empacota todos os itens, false caso contrario */
boolean empacotaplacas(twodbinpacktype *p,
		       int *numeroplacas) /*num. de placas usadas pelo emp.*/
{
  int i,area,nplacas,total,n;
  rectangletype *rcopia[MAXITEMS];
  boolean empacotoualgum;
  n = 0;
  for (i=0;i<p->n;i++) {
    if (p->r[i].np == -1) {
      rcopia[n] = &p->r[i];
      n++;
    }
  }
  
  nplacas = 0;
  total = n;
  do {
    empacotoualgum = empacotaumaplaca_p(n,rcopia,
					p->L,p->W,
					nplacas,
					p->toleranciaaltura,
					p->toleranciaarea,
					p->toleranciaordenacao,
					&area);
    if (!empacotamentoviavel(n,p->L,p->W,rcopia,0,0)) {
      printf("Problemas. Empacotamento nao eh viavel loop\n");
    }

    if (empacotoualgum) {
      removeempacotados(&n,rcopia,p->L,p->W);
      nplacas++;
      printf("Empacotados: %3d (faltam %d)\n",total-n,n);
    }
  }while (empacotoualgum);
  *numeroplacas = nplacas;
  if (n==0) return(true);
  return(false);
} 



