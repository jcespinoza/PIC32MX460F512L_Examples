/*
**
** Geometry.h
** 
** 8/4/07 LDJ v2.0 porting to 32-bit
*/
			
typedef short point[3];
typedef short poly[4];
typedef struct { short p; short n; } object;


typedef int tmatrix[4][4]; 
extern tmatrix m;

int fsin( short);
int fcos( short);
int ftan( short);

void initWorld( short, short, short, int, int, int );
void drawPolygons( short);
void matrixProd( point pi, point po);
void perspective( point p);



