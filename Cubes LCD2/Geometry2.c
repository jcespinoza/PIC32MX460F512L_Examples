/*
**
** 3D geometry engine
**
** LDJ 8/03/07 v2.0 regrouping objects
** LDJ 1/05/10 v3.0 x256
*/


#include "geometry2.h"
#include "graphic.h"
    
// precomputed sin table * 256
const short tsin[91]= { 0,4,9,13,18,22,27,31,36,40,44,49,53,58,62,66,71,75,
                        79,83,88,92,96,100,104,108,112,116,120,124,128,132,
                        136,139,143,147,150,154,158,161,165,168,171,175,178,
                        181,184,187,190,193,196,199,202,204,207,210,212,215,
                        217,219,222,224,226,228,230,232,234,236,237,239,241,
                        242,243,245,246,247,248,249,250,251,252,253,254,254,
                        255,255,255,256,256,256,256
                        };


int fsin( short alpha)
{   
    int i = 1;

    // normalize the angle
    if ( alpha < 0)
        alpha = 360 - ((-alpha) % 360);
    else
        alpha %= 360;

	if ( alpha >= 180)
	{   // sin(a+180) == - sin(a)
		alpha -= 180;
        i = -1; 
    }
    if ( alpha > 90)
    { // sin(180-a) == sin (a); when a<180
        alpha = 180 - alpha;
    }
    
    // use the table to approximate the angle
    return ( i * tsin[ alpha]);

} // fsin 


int fcos( short alpha)
{   // cos(a ) == sin (a+90)
    int i;
    i = fsin( alpha + 90);
    return i;

} // fcos
    

// define a world transformation matrix
tmatrix m; 

void matrixProd( point pi, point po) 
{   // point and transformation-matrix multiplication
    short i, j;
    int k;
    for( i=0; i<3; i++)
    { // do not calculate the 4th element (will be normalized)
        k = 0;
        for( j=0; j<3; j++)
        {  // simplified pi[3] is always 1
            k += pi[ j] * m[ i][ j];
        }
        po[ i] = (k + m[i][3])>>8;
    }          
} //matrix prod



void initWorld( short alpha, short beta, short gamma, int x0, int y0, int z0)
{
    m[0][0] = (+fcos( gamma) * fcos( beta)) >>8;
    m[0][1] = (-fsin( gamma) * fcos( beta)) >>8;
    m[0][2] = fsin( beta);
    m[0][3] = x0 << 8;
    m[1][0] = ((fcos( gamma) * fsin( beta) * fsin( alpha)) >>16) + ((fsin( gamma)* fcos(alpha)) >>8);
    m[1][1] = ((fcos( gamma) * fcos( alpha)) >>8) - ((fsin( gamma) * fsin(beta) * fsin(alpha)) >>16);
    m[1][2] = (-fcos( beta) * fsin( alpha)) >>8;
    m[1][3] = y0 << 8;
    m[2][0] = ((fsin( gamma) * fsin( alpha)) >>8) - ((fcos(gamma) * fsin( beta) * fcos(alpha)) >>16);
    m[2][1] = ((fsin( gamma) * fsin( beta) * fcos( alpha)) >>16) + ((fsin( alpha) * fcos( gamma)) >>8);
    m[2][2] = (fcos( beta) * fcos( alpha)) >>8;
    m[2][3] = z0 << 8;
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1 << 8;

} // init World



