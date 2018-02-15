#include <stdlib.h>
#include <stdio.h>
#include "figura.h"


/* Para compilar, execute

gcc figura_main.c figura.c -o exe

*/
int main()
{
  figuretype f;
  int i,n;
  char ss[1000];
  double x,y,x1,y1,x2,y2,maxx,minx,maxy,miny,delta;
  srand48(100);
  
  /* inicia figura */
  init_figure(&f);
  for (i=0;i<10;i++) {
    /* coordenadas aleatorias */
    x = (int) (drand48()*1000.0);
    y = (int) (drand48()*1000.0);
    insert_figure_point(&f,x,y,RED,20+100*drand48());
  }

  /* insere algum texto em posições aleatórias*/
  for (i=0;i<10;i++) {
    x = (int) (drand48()*1000.0);
    y = (int) (drand48()*1000.0);
    insert_figure_text(&f,x,y,COURIER,FIG_TEXT_SIZE_12,BLUE,"bla");
  }

  /* insere algumas linhas aleatórias */
  for (i=0;i<10;i++) {
    x1 = (int) (drand48()*1000.0);
    y1 = (int) (drand48()*500.0+500);
    x2 = (int) (drand48()*1000.0);
    y2 = (int) (drand48()*500.0+500);
    /* insere com espessuras aleatorias */
    insert_figure_line(&f,x1,y1,x2,y2,BLACK,1+(int) 20*drand48());
  }

  /* insere alguns retangulos aleatorios */
  for (i=0;i<5;i++) {
    x1 = (int) (drand48()*1000.0);
    y1 = (int) (drand48()*1000.0);
    x2 = (int) (drand48()*1000.0);
    y2 = (int) (drand48()*1000.0);
    insert_figure_rectangle(&f,x1,y1,x2,y2,RED);
  }

  /* simplesmente desenha uma moldura */
  min_max_figure_coordinates(&f,&minx,&maxx,&miny,&maxy);
  delta = fig_max((maxx-minx)/10.0,(maxy-miny)/10.0);
  insert_figure_line(&f,minx-delta,miny-delta,minx-delta,maxy+delta,BLUE,10);
  insert_figure_line(&f,minx-delta,maxy+delta,maxx+delta,maxy+delta,BLUE,10);
  insert_figure_line(&f,maxx+delta,maxy+delta,maxx+delta,miny-delta,BLUE,10);
  insert_figure_line(&f,minx-delta,miny-delta,maxx+delta,miny-delta,BLUE,10);

  /* ve a figura */
  view_figure(&f);
}
