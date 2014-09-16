/*
**  Cubes.c    Double buffered 3D animation demo
**
**  6/21/07 v2.0 Lucio Di Jasio
**  8/03/07 v2.1 LDJ automated generation of vertexes
**  8/10/09 v3.0 LDJ TG port and new graphics
**  1/05/10 v4.0 LDJ porting on MikroE MMB
**
**  HW Req: PIC32MX4 MMB MikroE
**  SW Req: MMB.c Geometry3.c Graphic.c
**  Ref:    www.exploringpic32.com
** 
**  This demo uses integer math to perform 3D drawing and 
**  animation with Isometric perspective
*/


// configuration bit settings
#pragma config POSCMOD=XT, FNOSC=PRIPLL
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_1
#pragma config FWDTEN=OFF, CP=OFF, BWP=OFF

#include <plib.h>
#include "Graphics/Graphics.h"

#include "MMB.h"
#include "graphic.h"
#include "geometry3.h"


extern const char Font8x8[];
extern short VCount;

// define the animation step (angle)
#define STEP    5  

// define the points table
#define MAXP    100 
point plist[ MAXP];           
short pc = MAXP;                    // points counter

// define the polygons table
#define MAXPOLY 100
poly pylist[ MAXPOLY];
short pyc = MAXPOLY;                  // polygons counter

// define the objects table
#define MAXOBJ 10
object olist[ MAXOBJ];
short objc = MAXOBJ;                  // object counter

// define the vertex space
#define MAXV    11
vertex vlist[ MAXV];
short vxc = MAXV;                        // vertex counter


short newVertex( int x, int y)
{
    vertex *p;
    
    p = &vlist[ --vxc];
    p->x = x;   p->y = y;
    
    return vxc;
} //new vertex 


void FillFlatTrig( int vA, int vB, int vC)
{
    // vA and Vb are the bottom/top (flat) vertexes of the triangle
    // vC is the top/bottom vertex
    int xa, xb, dxa, dxb, y, yb, yt,deltay;

    // compute deltay
    deltay = vlist[vC].y - vlist[vA].y;

    // compute dxa and dxb
    dxa = (vlist[vC].x - vlist[vA].x);
    dxb = (vlist[vC].x - vlist[vB].x); 

    // separate flat top from flat bottom
    if (vlist[vA].y > vlist[vC].y)
    { // flat top
        yt = vlist[vA].y;
        // clip top vertex 
        if (yt > VRES - 1)
            yt = VRES - 1;

        yb = vlist[vC].y;
        // clip bottom part
        if (yb < 0)
            yb = 0;
    } // flat top
    else // flat bottom
    {
        yt = vlist[vC].y;
        // clip top vertex 
        if (yt > VRES - 1)
            yt = VRES - 1;

        yb = vlist[vA].y;
        // clip bottom part
        if (yb < 0)
            yb = 0;

    } // flat bottom

    // drawing loop from bottom up
    for( y = yb; y <= yt; y++)
    {
        xa = vlist[ vA].x + (y - vlist[vA].y) * dxa / deltay;
        xb = vlist[ vB].x + (y - vlist[vA].y) * dxb / deltay;
        gHLine( xa, xb, y);
    } // for y

} // Fill Flat Trig


void SplitTrig( int vA, int vB, int vC)
{   // breaks a trig in two flat top/bottom trigs and paints them
    // assume vB is the middle height vertex
    // use vertex 0 as working space

    // generate a new intermediate vertex
    vlist[ 0].y = vlist[ vB].y;
    vlist[ 0].x = vlist[vC].x + (vlist[vB].y-vlist[vC].y)*(vlist[vA].x - vlist[vC].x) / (vlist[vA].y - vlist[vC].y);
    
    // draw two flat triangles
    FillFlatTrig( 0, vB, vA);
    FillFlatTrig( 0, vB, vC);
} // Split Trig


void FillTrig( int vA, int vB, int vC)
{   // fills a generic triangle
    int y;

    // check if triangle is collapsed in a point or single row
    // check if flat already
    if (vlist[vA].y == vlist[vB].y)
    {
        if (vlist[vA].y == vlist[vC].y)
            return;
        else
            FillFlatTrig(vA, vB, vC);
    }
    else if (vlist[vA].y == vlist[vC].y)
        FillFlatTrig(vA, vC, vB);
    else if (vlist[vB].y == vlist[vC].y)
        FillFlatTrig(vB, vC, vA);
    else
    {
        // otherwise split the generic triangle in two flat triangles
        // 1. find the middle vertex
        y = vlist[vA].y;      // let's assume it is vA
        if (vlist[vB].y < y)  // if vB is lower (A B) 
        {
            if (vlist[vC].y < vlist[vB].y) // and vC is even lower
                // B is the middle  (A B C)
                SplitTrig(vA, vB, vC);

            else if (vlist[vC].y < y)
                // C is the middle  (A C B)
                SplitTrig(vA, vC, vB);
            else
                // A is the middle (C A B)
                SplitTrig(vC, vA, vB);

        }
        else                // vb is higher (B A)
        {
            if (vlist[vC].y < y)
                // A is the middle (B A C)
                SplitTrig(vB, vA, vC);
            else if (vlist[vC].y < vlist[vB].y)
                // C is the middle (B C A)
                SplitTrig(vB, vC, vA);
            else
                // B is the middle (C B A)
                SplitTrig(vC, vB, vA);
        } // vB > vA
    } // not flat
} // FillTrig



void drawPolygons( short p)
{   // draws all vectors in the 4 point poligon of index p
    point pa, pb, pc, pd;
    short z, va, vb, vc, vd;
    vxc = MAXV;      // clear list of vertexes
    
    // compute 3 poly vertexes first 
    matrixProd( plist[pylist[p][0]], pa);    
    matrixProd( plist[pylist[p][1]], pb);
    matrixProd( plist[pylist[p][2]], pc);
    
    // first check if the poy is visible 
    z = vectorZProduct( pa, pb, pc);
    if (z <= 0)
        return;     // poly is not facing viewer
    
    // compute last point
    matrixProd( plist[pylist[p][3]], pd);

    va = newVertex( pa[0], pa[1]);
    vb = newVertex( pb[0], pb[1]);
    vc = newVertex( pc[0], pc[1]);
    vd = newVertex( pd[0], pd[1]);

    // draw the poly in two triangles
    FillTrig( va, vb, vc);
    FillTrig( va, vc, vd); 
        
} // drawPolygons


void drawObject( short obj)
{
    short i;

    for( i=0; i< olist[ obj].n; i++)
    {
        setColor( i+1);
        drawPolygons( olist[ obj].p+i);
    }
}// drawObject



short newPoint( short x, short y, short z)
{ // add a point to the list and return its index
  // optimized to search for an existing point first
    int i;
    point *p = &plist[pc];
    for ( i=pc; i<MAXP; i++, p++)
    {
        if ((*p)[0] == x)
            if ((*p)[1] == y)
               if ((*p)[2] == z)
                    return i;       // found
    }    
    
    // not found create a new one  
    p = &plist[--pc];

    (*p)[0] = x; (*p)[1] = y; (*p)[2] = z;
    
    return pc;
} // newPoint


short newPoly( short a, short b, short c, short d)
{   
    poly *p = &pylist[ --pyc];
    (*p)[0] = a; (*p)[1] = b; (*p)[2] = c; (*p)[3] = d;
    return pyc;
} // newPoly    


short cube( short x, short y, short z, short s)
{ // generates a cube at x,y,z coordinates, of side s

  short a, b ,c, d, e, f, g, h, p;
  // verify there is room in the tables
    if ((pyc>3)  && (pc>7))
    {
      // generate all vertexes of the cube
        a = newPoint( x, y, z);        // A
        b = newPoint( x, y+s, z);      // B
        c = newPoint( x+s, y+s, z);    // C
        d = newPoint( x+s, y, z);      // D
        e = newPoint( x+s, y, z+s);    // E
        f = newPoint( x+s, y+s, z+s);  // F
        g = newPoint( x, y+s, z+s);    // G
        h = newPoint( x, y, z+s);      // H

      // generate all the polygons with proper orientation (normal outbound)
        newPoly( a, b, c, d);
        newPoly( e, f, g, h);
        newPoly( b, g, f, c); 
        newPoly( d, e, h, a);       
        newPoly( d, c, f, e);
        p = newPoly( g, b, a, h);

      // returns the polygon index
        return p;
    } // if 
    else 
        while( 1);
} // cube


void createObject( int m, char *s, int w)
{  // creates a pattern of cubes from a (short) text string
    // w is the desired total string width
    // m mode: 0- single cube
    //         1- multiple objects
    char c;
    const char*p;
    int i, j, wc, x0, x, y;
    short po, n;

    // determine maximum allowed size of each dot/cube
    wc = w /(8*strlen( s));

    // determine initial horizontal position
    x0 = - w/2;

    // start counting the cubes
    n = 0;

    // map each character in the string
    while(*s)
    { // determine intial position
        y = + 4*wc;
        // find character in font
        p = &Font8x8[(*s-0x20)<<3];
        for(i=0; i<8; i++)
        {   // one line at a time
            c = *p++;
            x = x0;
            for(j=0; j<8; j++)
            { // one pixel at a time
                if ( c & 0x80)
                {
                    po = cube( x, y, 0, wc);
                    n+=6;
                }
                x+=wc;
                c<<=1;
            } // forj
            y -= wc;
        } // for i
        s++;
        x0 += 8 * wc;

        if ( m)
        { // generate an object for each character in the string
          olist[ --objc].p = po;  // the last cube/polygon inserted 
          olist[ objc].n = n;     // the number of cubes created
          n = 0;                  // reset counter of cubes
        }
    } // while

    if ( !m)
    { // generate a single object for all characters
      olist[ --objc].p = po;  // the last cube/polygon inserted 
      olist[ objc].n = n;     // the number of cubes created
    }            
} // creatObject     
   

short x0, y0, z0;
short a[MAXOBJ], b[MAXOBJ], g[MAXOBJ];
short r[MAXOBJ], k[MAXOBJ];


int main( void)
{
    double t;
    short v, i, j;
    char start;
    char roll;
    char s[32], c;
 
    // init offsets
    x0 = 128;
    y0 = 50;
    z0 = 50;
    
    // hardware and video initialization 
    MMBInit();
    initVideo();
    
        
    // 3. main loop
    while( 1)
    {
        roll = 1;
        start = 0;
        gClearScreen();          
    
        // splash screen
        setColor( 2);  // red
        AT(8, 1); putsV( "3D Filling Demo");
        AT(8, 3); putsV( "   LDJ v1.0");
        AT(8, 5); putsV( " for PIC32MX4 MMB");
        
        copyV();                        // update visible screen
        while( !MMBReadKey());
        srand( ReadCoreTimer());
        
        // select one of three demo modes
        c = JOY_RIGHT ; 
        //do { c = MMBGetKey();
        //} while (c & (JOY_UP | JOY_LEFT ));
     
        // clear all objects
        objc = MAXOBJ;
        pyc = MAXPOLY;
        pc = MAXP;
        
        // init all objects angles
        for( i=0; i<MAXOBJ; i++)
            a[i] = b[i] = g[i] = 0;
     
        if( c == JOY_SELECT)             // complex demo
        {                               
            createObject( 1, "I`32", 200);
            start = 1;
        }
        else if( c == JOY_DOWN)         // 32
            createObject( 0, "32", 256);                 
        else 
            createObject( 0, "\\", 500); // single cube
    
        // show the totals
        sprintf( s, "%d Points  %d Poly  %d Obj", MAXP-pc, MAXPOLY-pyc, MAXOBJ-objc);
    
        while( roll)
        {
            for( i=objc; i<MAXOBJ; i++)
            {
                // pick random movements
                r[i] = ( rand() & 1) ? STEP : -STEP;  // random verse
                k[i] = ( rand() % 3);           // random axis
            }
            j = 5 + (rand() & 0x3f);            // random rotation
    
            // animate
            while ( j-- > 0)
            {  
                gClearScreen();                 // clear the hidden screen
                
                if ( !start)                    // summary 
                {
                    setColor( 2);  // green
                    AT( 0, 0); putsV( s);
                }
                
                for( i=objc; i<MAXOBJ; i++)
                {
                    initWorld( a[i], b[i], g[i], x0, y0, z0);
                    drawObject( i);
                    
                    if ( c = MMBReadKey())
                    {   // joystick pressed
                        if (c == JOY_LEFT)
                        {
                            // return all objects to base position
                            if ( a[i] >0 )
                                a[i]-= STEP;
                            else if ( a[i] <0)
                                a[i]+= STEP;
                            if ( b[i] >0 )
                                b[i]-= STEP;
                            else if ( b[i] <0)
                                b[i]+= STEP;
                            if ( g[i] >0 )
                                g[i]-= STEP;
                            else if ( g[i] <0)
                                g[i]+= STEP;
                        } // left
                        else if (c == JOY_UP) // return to menu
                            roll = 0;
                    }
                    else 
                    //perform the update
                    switch ( k[i]){
                    case 0:
                        a[i] += r[i];
                        break;
                    case 1: 
                        b[i] += r[i];
                        break;
                    case 2:
                        g[i] += r[i];
                        break;
                    } // switch
                } // for
    
                //MMBStartStopwatch();
                copyV();                        // update visible screen
                //t = MMBReadStopwatch();
                
                // wait for button to start the animation
                if ( !start)
                    start = MMBGetKey();
                //else
                
            } // while animation           
        
        } // roll
    } // main loop
} // main






