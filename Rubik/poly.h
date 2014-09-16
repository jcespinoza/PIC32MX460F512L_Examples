/*
**
** Poly.h
** 
** 8/4/07 LDJ v2.0 porting to 32-bit
** 1/6/10 LDJ v3.0 added triangle fill 
**
*/

#define MAXP    160     // points
#define MAXPOLY  60     // polys
#define MAXV      5     // vertexes

#define EZ        150   // observer distance
					
typedef short poly[4];  // a, b, c, d
typedef struct { int x; int y; } vertex;

extern tmatrix mo, mw;

// define the points table
extern point plist[ MAXP];           
extern short pc;                    // points counter

// define the polygons table
extern poly pylist[ MAXPOLY];
extern short pyc;                  // polygons counter


// define the vertex space
extern vertex vlist[ MAXV];
extern short vxc;                   // vertex counter


short newPoint( short x, short y, short z);
short newPoly( short a, short b, short c, short d);
short newVertex( int x, int y);

void drawPolygons( short);
void FillTrig( int vA, int vB, int vC);



