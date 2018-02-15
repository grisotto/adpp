
#include "figura.h"
#include <string.h>

double fig_max(double a,double b)
{
  if (a>b) return(a);
  return(b);
}


void init_figure(figuretype *f)
{
  f->n = 0;
  f->ngrid=0;
}

void insert_grid(figuretype *f,int ngrade)
{
  f->ngrid = ngrade;
}


void insert_figure_line(figuretype *f,double x1,double y1,
			double x2, double y2,
			fig_colortype cor, int espessura)
{
  if (f->n>=FIG_MAXOBJECTS) {
    printf("Numero maximo de objetos atingido.\n");
    return;
  }
  f->object[f->n].type = LINE;
  f->object[f->n].u.l.x1 = x1;
  f->object[f->n].u.l.y1 = y1;
  f->object[f->n].u.l.x2 = x2;
  f->object[f->n].u.l.y2 = y2;
  f->object[f->n].u.l.espessura = espessura;
  f->object[f->n].u.l.cor = cor;
  f->n++;
}

void insert_figure_rectangle(figuretype *f,double x1,double y1,
			double x2, double y2,
			fig_colortype cor)
{
  if (f->n>=FIG_MAXOBJECTS) {
    printf("Numero maximo de objetos atingido.\n");
    return;
  }
  f->object[f->n].type = RECTANGLE;
  f->object[f->n].u.r.x1 = x1;
  f->object[f->n].u.r.y1 = y1;
  f->object[f->n].u.r.x2 = x2;
  f->object[f->n].u.r.y2 = y2;
  f->object[f->n].u.r.cor = cor;
  f->n++;
}

void insert_figure_point(figuretype *f,double x,double y,
			fig_colortype cor, int raio)
{
  if (f->n>=FIG_MAXOBJECTS) {
    printf("Numero maximo de objetos atingido.\n");
    return;
  }
  f->object[f->n].type = POINT;
  f->object[f->n].u.p.x = x;
  f->object[f->n].u.p.y = y;
  f->object[f->n].u.p.raio = raio;
  f->object[f->n].u.p.cor = cor;
  f->n++;
}

void insert_figure_text(figuretype *f,double x,double y,
			fig_fonttype font, int size, fig_colortype cor,
			char *text)
{
  if (f->n>=FIG_MAXOBJECTS) {
    printf("Numero maximo de objetos atingido.\n");
    return;
  }
  f->object[f->n].type = TEXT;
  f->object[f->n].u.t.x = x;
  f->object[f->n].u.t.y = y;
  f->object[f->n].u.t.size = size;
  f->object[f->n].u.t.cor = cor;
  f->object[f->n].u.t.font = font;
  strncpy(f->object[f->n].u.t.text,text,FIG_MAXTEXTSIZE);
  f->n++;
}

void fig_getepscolor(char *epscolorname,fig_colortype cor)
{
  if ((cor<0) || (cor>31)) cor = BLACK;
  sprintf(epscolorname,"col%d",cor);
}

void fig_update_max(double *max,double *x)
{
  if (*x > *max) *max = *x;
}
void fig_update_min(double *min,double *x)
{
  if (*x < *min) *min = *x;
}


void min_max_figure_coordinates(figuretype *f,
			      double *minx, double *maxx,
			      double *miny, double *maxy)
{
  int i;
  *maxx = -10E20;
  *maxy = -10E20;
  *minx =  10E20;
  *miny =  10E20;
  for (i=0;i<f->n;i++){
    switch (f->object[i].type) {
    case POINT: 
	fig_update_max(maxx,&f->object[i].u.p.x);
	fig_update_min(minx,&f->object[i].u.p.x);
	fig_update_max(maxy,&f->object[i].u.p.y);
	fig_update_min(miny,&f->object[i].u.p.y);
	break;
    case TEXT: 
	fig_update_max(maxx,&f->object[i].u.p.x);
	fig_update_min(minx,&f->object[i].u.p.x);
	fig_update_max(maxy,&f->object[i].u.p.y);
	fig_update_min(miny,&f->object[i].u.p.y);
	break;
    case LINE: 
        fig_update_max(maxx,&f->object[i].u.l.x1);
	fig_update_min(minx,&f->object[i].u.l.x1);
	fig_update_max(maxx,&f->object[i].u.l.x2);
	fig_update_min(minx,&f->object[i].u.l.x2);
	fig_update_max(maxy,&f->object[i].u.l.y1);
	fig_update_min(miny,&f->object[i].u.l.y1);
	fig_update_max(maxy,&f->object[i].u.l.y2);
	fig_update_min(miny,&f->object[i].u.l.y2);
	break;
    case RECTANGLE: 
        fig_update_max(maxx,&f->object[i].u.r.x1);
	fig_update_min(minx,&f->object[i].u.r.x1);
	fig_update_max(maxx,&f->object[i].u.r.x2);
	fig_update_min(minx,&f->object[i].u.r.x2);
	fig_update_max(maxy,&f->object[i].u.r.y1);
	fig_update_min(miny,&f->object[i].u.r.y1);
	fig_update_max(maxy,&f->object[i].u.r.y2);
	fig_update_min(miny,&f->object[i].u.r.y2);
	break;
    }
  }
}


int fig_converte_x(double x,double minx,double maxx,double miny,double maxy)
{
  int sizex,sizey;
  double deltax,deltay;
  deltax = (double) maxx-minx;
  deltay = (double) maxy-miny;
  if (deltax > deltay) {
    sizex = (int) FIG_MAXPOINTPOSITION;
    sizey = (int) FIG_MAXPOINTPOSITION*deltay/deltax;
  } else {
    sizex = (int) FIG_MAXPOINTPOSITION*deltax/deltay;
    sizey = (int) FIG_MAXPOINTPOSITION;
  }
  return((int) (sizex*((double)(x-minx))/((double) (maxx-minx)))+500);
}

int fig_converte_y(double y,double minx,double maxx,double miny,double maxy)
{
  int sizex,sizey;
  double deltax,deltay;
  deltax = (double) maxx-minx;
  deltay = (double) maxy-miny;
  if (deltax > deltay) {
    sizex = (int) FIG_MAXPOINTPOSITION;
    sizey = (int) FIG_MAXPOINTPOSITION*deltay/deltax;
  } else {
    sizex = (int) FIG_MAXPOINTPOSITION*deltax/deltay;
    sizey = (int) FIG_MAXPOINTPOSITION;
  }
  return((int) (sizey-sizey*((double)(y-miny))/((double) (maxy-miny)))+500);
}



void save_epsfigure(figuretype *f,char *filename)
{
  FILE *fp;
  double maxx, maxy, minx, miny;
  int gap,posx,posy,
    telax,posxu,posxv,posyu,posyv,i,raio;
  char epscolor[100];
  fp = fopen(filename,"w");
  if (fp==NULL) {
    printf("Erro para abrir arquivo \"%s\"\n",filename);
    return;
  }
  if (f->n<1){
    printf("figura sem objetos\n");
    return;
  }
  min_max_figure_coordinates(f,&minx,&maxx,&miny,&maxy);

  telax = 500;
  fprintf(fp,"%%!PS-Adobe-2.0 EPSF-2.0\n");
  fprintf(fp,"%%%%Title: x.eps\n");
  fprintf(fp,"%%%%Creator: fig2dev Version 3.2 Patchlevel 3c\n");
  fprintf(fp,"%%%%CreationDate: Thu Sep 12 13:02:34 2002\n");
  fprintf(fp,"%%%%For: fkm@hobbes.dcc.unicamp.br ()\n");
  fprintf(fp,"%%%%BoundingBox: 0 0 %d %d\n",telax,telax);
  fprintf(fp,"%%%%Magnification: 1.0000\n");
  fprintf(fp,"%%%%EndComments\n");
  fprintf(fp,"/$F2psDict 200 dict def\n");
  fprintf(fp,"$F2psDict begin\n");
  fprintf(fp,"$F2psDict /mtrx matrix put\n");
  fprintf(fp,"/col-1 {0 setgray} bind def\n");
  fprintf(fp,"/col0 {0.000 0.000 0.000 srgb} bind def\n");
  fprintf(fp,"/col1 {0.000 0.000 1.000 srgb} bind def\n");
  fprintf(fp,"/col2 {0.000 1.000 0.000 srgb} bind def\n");
  fprintf(fp,"/col3 {0.000 1.000 1.000 srgb} bind def\n");
  fprintf(fp,"/col4 {1.000 0.000 0.000 srgb} bind def\n");
  fprintf(fp,"/col5 {1.000 0.000 1.000 srgb} bind def\n");
  fprintf(fp,"/col6 {1.000 1.000 0.000 srgb} bind def\n");
  fprintf(fp,"/col7 {1.000 1.000 1.000 srgb} bind def\n");
  fprintf(fp,"/col8 {0.000 0.000 0.560 srgb} bind def\n");
  fprintf(fp,"/col9 {0.000 0.000 0.690 srgb} bind def\n");
  fprintf(fp,"/col10 {0.000 0.000 0.820 srgb} bind def\n");
  fprintf(fp,"/col11 {0.530 0.810 1.000 srgb} bind def\n");
  fprintf(fp,"/col12 {0.000 0.560 0.000 srgb} bind def\n");
  fprintf(fp,"/col13 {0.000 0.690 0.000 srgb} bind def\n");
  fprintf(fp,"/col14 {0.000 0.820 0.000 srgb} bind def\n");
  fprintf(fp,"/col15 {0.000 0.560 0.560 srgb} bind def\n");
  fprintf(fp,"/col16 {0.000 0.690 0.690 srgb} bind def\n");
  fprintf(fp,"/col17 {0.000 0.820 0.820 srgb} bind def\n");
  fprintf(fp,"/col18 {0.560 0.000 0.000 srgb} bind def\n");
  fprintf(fp,"/col19 {0.690 0.000 0.000 srgb} bind def\n");
  fprintf(fp,"/col20 {0.820 0.000 0.000 srgb} bind def\n");
  fprintf(fp,"/col21 {0.560 0.000 0.560 srgb} bind def\n");
  fprintf(fp,"/col22 {0.690 0.000 0.690 srgb} bind def\n");
  fprintf(fp,"/col23 {0.820 0.000 0.820 srgb} bind def\n");
  fprintf(fp,"/col24 {0.500 0.190 0.000 srgb} bind def\n");
  fprintf(fp,"/col25 {0.630 0.250 0.000 srgb} bind def\n");
  fprintf(fp,"/col26 {0.750 0.380 0.000 srgb} bind def\n");
  fprintf(fp,"/col27 {1.000 0.500 0.500 srgb} bind def\n");
  fprintf(fp,"/col28 {1.000 0.630 0.630 srgb} bind def\n");
  fprintf(fp,"/col29 {1.000 0.750 0.750 srgb} bind def\n");
  fprintf(fp,"/col30 {1.000 0.880 0.880 srgb} bind def\n");
  fprintf(fp,"/col31 {1.000 0.840 0.000 srgb} bind def\n");
  fprintf(fp,"\n");
  fprintf(fp,"end\n");
  fprintf(fp,"save\n");
  fprintf(fp,"newpath 0 %d moveto 0 0 lineto %d 0 lineto %d %d "
	  "lineto closepath clip newpath\n",telax,telax,telax,telax);
  fprintf(fp,"%d %d translate\n",-10,telax+10);
  fprintf(fp,"1 -1 scale\n");
  fprintf(fp,"\n");
  fprintf(fp,"/cp {closepath} bind def\n");
  fprintf(fp,"/ef {eofill} bind def\n");
  fprintf(fp,"/gr {grestore} bind def\n");
  fprintf(fp,"/gs {gsave} bind def\n");
  fprintf(fp,"/sa {save} bind def\n");
  fprintf(fp,"/rs {restore} bind def\n");
  fprintf(fp,"/l {lineto} bind def\n");
  fprintf(fp,"/m {moveto} bind def\n");
  fprintf(fp,"/rm {rmoveto} bind def\n");
  fprintf(fp,"/n {newpath} bind def\n");
  fprintf(fp,"/s {stroke} bind def\n");
  fprintf(fp,"/sh {show} bind def\n");
  fprintf(fp,"/slc {setlinecap} bind def\n");
  fprintf(fp,"/slj {setlinejoin} bind def\n");
  fprintf(fp,"/slw {setlinewidth} bind def\n");
  fprintf(fp,"/srgb {setrgbcolor} bind def\n");
  fprintf(fp,"/rot {rotate} bind def\n");
  fprintf(fp,"/sc {scale} bind def\n");
  fprintf(fp,"/sd {setdash} bind def\n");
  fprintf(fp,"/ff {findfont} bind def\n");
  fprintf(fp,"/sf {setfont} bind def\n");
  fprintf(fp,"/scf {scalefont} bind def\n");
  fprintf(fp,"/sw {stringwidth} bind def\n");
  fprintf(fp,"/tr {translate} bind def\n");
  fprintf(fp,"/tnt {dup dup currentrgbcolor\n");
  fprintf(fp,"  4 -2 roll dup 1 exch sub 3 -1 roll mul add\n");
  fprintf(fp,"  4 -2 roll dup 1 exch sub 3 -1 roll mul add\n");
  fprintf(fp,"  4 -2 roll dup 1 exch sub 3 -1 roll mul add srgb}\n");
  fprintf(fp,"  bind def\n");
  fprintf(fp,"/shd {dup dup currentrgbcolor 4 -2 roll mul 4 -2 roll mul\n");
  fprintf(fp,"  4 -2 roll mul srgb} bind def\n");
  fprintf(fp," /DrawEllipse {\n");
  fprintf(fp,"	/endangle exch def\n");
  fprintf(fp,"	/startangle exch def\n");
  fprintf(fp,"	/yrad exch def\n");
  fprintf(fp,"	/xrad exch def\n");
  fprintf(fp,"	/y exch def\n");
  fprintf(fp,"	/x exch def\n");
  fprintf(fp,"	/savematrix mtrx currentmatrix def\n");
  fprintf(fp,"	x y tr xrad yrad sc 0 0 1 startangle endangle arc\n");
  fprintf(fp,"	closepath\n");
  fprintf(fp,"	savematrix setmatrix\n");
  fprintf(fp,"	} def\n");
  fprintf(fp,"\n");
  fprintf(fp,"/$F2psBegin {$F2psDict begin "
	  "/$F2psEnteredState save def} def\n");
  fprintf(fp,"/$F2psEnd {$F2psEnteredState restore end} def\n");
  fprintf(fp,"\n");
  fprintf(fp,"$F2psBegin\n");
  fprintf(fp,"%%%%Page: 1 1\n");
  fprintf(fp,"10 setmiterlimit\n");
  fprintf(fp," %10.8lf %10.8lf sc\n",(double) FIG_MAXPOINTPOSITION/100000,(double) FIG_MAXPOINTPOSITION/100000);
  fprintf(fp,"%%\n");
  fprintf(fp,"%% Fig objects follow\n");
  fprintf(fp,"%%\n");
  fprintf(fp,"10.000 slw\n"); /* espessura da linha */
  gap = 300;
  raio = 7;


  /* imprime os retas */
  for (i=0;i<f->n;i++){
    if (f->object[i].type==LINE) {
      posxu = fig_converte_x(f->object[i].u.l.x1,minx,maxx,miny,maxy);
      posyu = fig_converte_y(f->object[i].u.l.y1,minx,maxx,miny,maxy);
      posxv = fig_converte_x(f->object[i].u.l.x2,minx,maxx,miny,maxy);
      posyv = fig_converte_y(f->object[i].u.l.y2,minx,maxx,miny,maxy);
      fig_getepscolor(epscolor,f->object[i].u.l.cor);
      fprintf(fp,"%lf slw\n",f->object[i].u.l.espessura * 7.5); /* espessura da linha */
      fprintf(fp,"n %d %d m\n %d %d l gs %s s gr \n",
	    posxu,posyu,posxv,posyv,epscolor);
    }
  }
  /* imprime os retangulos */
  for (i=0;i<f->n;i++){
    if (f->object[i].type==RECTANGLE) {
      posxu = fig_converte_x(f->object[i].u.r.x1,minx,maxx,miny,maxy);
      posyu = fig_converte_y(f->object[i].u.r.y1,minx,maxx,miny,maxy);
      posxv = fig_converte_x(f->object[i].u.r.x2,minx,maxx,miny,maxy);
      posyv = fig_converte_y(f->object[i].u.r.y2,minx,maxx,miny,maxy);
      fig_getepscolor(epscolor,f->object[i].u.r.cor);
      fprintf(fp,"7.500 slw\n"); /* espessura da borda */
      fprintf(fp,"n %d %d m ",posxu,posyu);
      fprintf(fp,"%d %d l ",posxv,posyu);
      fprintf(fp,"%d %d l ",posxv,posyv);
      fprintf(fp,"%d %d l\n",posxu,posyv);
      fprintf(fp,"cp gs %s 1.00 shd ef gr gs col0 s gr\n",epscolor);
    }
  }

  /* imprime os pontos */
  for (i=0;i<f->n;i++){
    if (f->object[i].type==POINT) {
      posx = fig_converte_x(f->object[i].u.p.x,minx,maxx,miny,maxy);
      posy = fig_converte_y(f->object[i].u.p.y,minx,maxx,miny,maxy);
      fig_getepscolor(epscolor,f->object[i].u.p.cor);
      fprintf(fp,"n %d %d %d %d 0 360 DrawEllipse gs %s 1.00 shd ef gr \n",
	      posx,posy,f->object[i].u.p.raio,f->object[i].u.p.raio,epscolor);
    }
  }
  /* imprime os textos */
  for (i=0;i<f->n;i++){
    if (f->object[i].type==TEXT) {
      posx = fig_converte_x(f->object[i].u.t.x,minx,maxx,miny,maxy);
      posy = fig_converte_y(f->object[i].u.t.y,minx,maxx,miny,maxy);
      fig_getepscolor(epscolor,f->object[i].u.t.cor);
      fprintf(fp,"/Courier-Bold ff %d scf sf\n",f->object[i].u.t.size);
      fprintf(fp,"%d %d m\n",posx,posy);
      fprintf(fp,"gs 1 -1 sc (%s) %s sh gr\n",f->object[i].u.t.text,epscolor);
    }
  }

  fprintf(fp,"$F2psEnd\n");
  fprintf(fp,"rs\n");
  fclose(fp);
  free(filename);
  return;
}

void view_figure(figuretype *f)
{
  save_epsfigure(f,"TMP_FILE.eps");
  /*system("open TMP_FILE.eps");*/
}

