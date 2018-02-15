#ifndef FIGURE_DEFINE
#define FIGURE_DEFINE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "boolean.h"

typedef int fig_colortype;
#define BLACK 0
#define NOCOLOR 0
#define BLUE  1
#define GREEN 2
#define RED   4
#define WHITE 7
#define COL0 0
#define COL1 1
#define COL2 2
#define COL3 3
#define COL4 4
#define COL5 5
#define COL6 6
#define COL7 7
#define COL8 8
#define COL9 9
#define COL10 10
#define COL11 11
#define COL12 12
#define COL13 13
#define COL14 14
#define COL15 15
#define COL16 16 
#define COL17 17
#define COL18 18
#define COL19 19
#define COL20 20
#define COL21 21
#define COL22 22
#define COL23 23
#define COL24 24
#define COL25 25
#define COL26 26
#define COL27 27
#define COL28 28
#define COL29 29
#define COL30 30
#define COL31 31



typedef enum {COURIER } fig_fonttype;
#define FIG_TEXT_SIZE_4   120
#define FIG_TEXT_SIZE_6   150
#define FIG_TEXT_SIZE_8   180
#define FIG_TEXT_SIZE_10  210
#define FIG_TEXT_SIZE_12  240
#define FIG_TEXT_SIZE_14  270
#define FIG_TEXT_SIZE_16  300
#define FIG_TEXT_SIZE_18  330
#define FIG_TEXT_SIZE_20  360
#define FIG_TEXT_SIZE_22  390
#define FIG_TEXT_SIZE_24  420
#define FIG_TEXT_SIZE_26  450
#define FIG_TEXT_SIZE_28  480
#define FIG_TEXT_SIZE_30  510
#define FIG_TEXT_SIZE_32  540

typedef enum {POINT, LINE, TEXT , RECTANGLE} fig_objecttype;

#define FIG_MAXOBJECTS 10000
#define FIG_MAXTEXTSIZE 100
#define FIG_MAXPOINTPOSITION 6000

/* dados para desenhar um ponto */
typedef struct 
{
  double x,y;
  int raio;
  fig_colortype cor;
} fig_pointtype;

/* dados para desenhar uma reta */
typedef struct
{
  double x1,y1; /* primeiro ponto */
  double x2,y2; /* segundo ponto */
  int espessura; /* espessura da linha */
  fig_colortype cor;
} fig_linetype;

/* dados para desenhar um retangulo */
typedef struct
{
  double x1,y1; /* ponto inferior esquerdo*/
  double x2,y2; /* ponto superior direito */
  int espessura; /* espessura da linha */
  fig_colortype cor;
} fig_rectangletype;

/* dados para desenhar um texto */
typedef struct
{
  double x,y;
  char text[FIG_MAXTEXTSIZE]; /* tamanho maximo do texto 
		  (ideal seria alocar para permitir tamanhos mais variados)*/
  int size; /* tamanho do texto */
  fig_fonttype font;
  fig_colortype cor;
} fig_texttype;

typedef struct
{
  int n;
  int ngrid; /* numero de linhas horiz/vert na grade (iniciado com 0) */
  struct 
  {
    fig_objecttype type;
    union 
    {
      fig_pointtype p;
      fig_linetype l;
      fig_texttype t;
      fig_rectangletype r;
    }u;
  }object[FIG_MAXOBJECTS];
} figuretype;

void init_figure(figuretype *f);
void insert_figure_rectangle(figuretype *f,double x1,double y1,
			double x2, double y2,
			     fig_colortype cor);
void insert_figure_line(figuretype *f,double x1,double y1,
			double x2, double y2,
			fig_colortype cor, int espessura);
void insert_figure_point(figuretype *f,double x,double y,
			fig_colortype cor, int raio);
void insert_figure_text(figuretype *f,double x,double y,
			fig_fonttype font, int size, fig_colortype cor,
			char *text);
void save_epsfigure(figuretype *f,char *filename);
void view_figure(figuretype *f);
void min_max_figure_coordinates(figuretype *f,
			      double *minx, double *maxx,
				double *miny, double *maxy);
double fig_max(double a,double b);
#endif
