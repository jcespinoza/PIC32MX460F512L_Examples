/*
**
** Geometry3.h
** 
** 8/4/07 LDJ v2.0 porting to 32-bit
** 1/6/10 LDJ v3.0 added triangle fill 
*/
			
typedef short point[3];
typedef short poly[4];
typedef struct { short p; short n; } object;
typedef struct { int x; int y; } vertex;


typedef int tmatrix[4][4]; 
extern tmatrix m;

int fsin( short);
int fcos( short);
int ftan( short);

void initWorld( short, short, short, int, int, int );
void drawPolygons( short);



