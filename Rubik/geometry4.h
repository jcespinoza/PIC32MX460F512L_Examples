/*
**
** Geometry4.h
** 
** 8/04/07 LDJ v2.0 porting to 32-bit
** 1/05/10 LDJ v3.0 x256
** 2/21/10 LDJ v4.0 rotation matrix passed as parameter and vector Z product added
*/
			
typedef int tmatrix[4][4]; 
typedef short point[3]; // x, y, z

int fsin( short);
int fcos( short);
int ftan( short);

void initMatrix( tmatrix, short, short, short, int, int, int );
void matrixProd( tmatrix m, const point pi, point po); 
short vectorZProduct( point c, point d, point e);




