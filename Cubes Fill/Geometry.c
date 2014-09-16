/*
**
** 3D geometry engine
**
*/


#include "Geometry.h"
#include "Graphic.h"
 

// world transformation matrix
tmatrix m; 

   
// precomputed sin table * 100
const short tsin[91]= { 0, 1, 3, 5, 6, 8,10,12,13,15,17,19,20,22,24,25,27,29,30,32,
                     34,35,37,39,40,42,43,45,46,48,49,51,52,54,55,57,58,60,61,62,
                     64,65,66,68,69,70,71,73,74,75,76,77,78,79,80,81,82,83,84,85,
                     86,87,88,89,89,90,91,92,92,93,93,94,95,95,96,96,97,97,97,98,
                     98,98,99,99,99,99,99,99,99,99,100};

int fsin( short alpha)
{   
    int i =1;

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


int ftan( short alpha)
{   // tan(a) = sin(a)/cos(a)
    return ( fsin( alpha)/fcos( alpha));
} //ftan
    

void matrixProd( point pi, point po) 
{   // point and transformation-matrix multiplication
    short i, j;
    int k, a, b;
    for( i=0; i<4; i++)
    {
        k = 0;
        for( j=0; j<4; j++)
        {  a = pi[j];
            b = m[i][j];
            k += pi[ j] * m[ i][ j];
        }
        po[ i] = k;
    }          
} //matrix prod


void normalize( point p)
{   // normalize the result
    short i;
    for( i=0; i<3; i++)
        p[ i] /= 200;
    p[3] = 1;
} // normalize


void initWorld( short alpha, short beta, short gamma, int x0, int y0, int z0)
{
    m[0][0] = +fcos( gamma)* fcos( beta)/100;
    m[0][1] = -fsin( gamma) * fcos( beta)/100;
    m[0][2] = fsin( beta);
    m[0][3] = x0*100;
    m[1][0] = fcos( gamma) * fsin( beta) * fsin( alpha) /10000+ fsin( gamma)* fcos(alpha)/100;
    m[1][1] = fcos( gamma) * fcos( alpha)/100 - fsin( gamma) * fsin(beta) * fsin(alpha)/10000;
    m[1][2] = -fcos( beta) * fsin( alpha)/100;
    m[1][3] = y0*100;
    m[2][0] = fsin( gamma) * fsin( alpha)/100 - fcos(gamma) * fsin( beta) * fcos(alpha)/10000;
    m[2][1] = fsin( gamma) * fsin( beta) * fcos( alpha)/10000 + fsin( alpha) * fcos( gamma)/100;
    m[2][2] = fcos( beta) * fcos( alpha)/100;
    m[2][3] = z0*100;
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 100;

} // init World


void perspective( point p)
{   // basic isometric perspective distortion
    p[0] += p[2]/2;     
    p[1] += p[2]/2;
    
} // perspective 


short vectorZProduct( point c, point d, point e)
{ // a and b are vectors defining the poligon plane
    int a0, a1, b0, b1;
    // find a plane containing three points
    // create a vector containing d-c
    a0 = d[0]-c[0];
    a1 = d[1]-c[1];
    // create a vector containing e-c
    b0 = e[0]-d[0];
    b1 = e[1]-d[1];
    // return the z component to determine if the poligon is visible
    return ( a0*b1 - a1*b0); 
} //vectorZProduct


void drawPolygons(  point *p)
{   // assume minumum two segments
    point a, b, c;
    while ( (*p)[3] == 1)
    { // start a new series of points (poligon)
        // compute the first point coordinates 
        matrixProd( *p, a);
        p++;
        matrixProd( *p, b);
        p++;
        // analyze the plane orientation
        matrixProd( *p, c);
        p++;
        if ( 1) //vectorZProduct( a, b, c)>0)    
        { // visible, draw the first two vectors
            normalize( a);
            perspective( a);
            normalize( b);
            perspective( b);
            line( a[0], a[1], b[0], b[1] );
            normalize( c);
            perspective( c);
            line (c[0], c[1], b[0], b[1] );

            // continue from c 
              a[0] = c[0]; 
              a[1] = c[1];

            while ( (*p)[3] == 1)
            { // continue poligon
                matrixProd( *p, b);
                p++;
              // draw the a-b vector
                normalize( b);
                perspective( b);
                line( a[0], a[1], b[0], b[1] );
              // continue from b 
                a[0] = b[0]; 
                a[1] = b[1];
            }// polygon
        }
        else    // discard rest of the poligon
            while ( (*p)[3] == 1)
                p++;
        
        // advance to next series of points
        p++;
    } // drawing list
} // draw polygons




