/*
**  Cubes.c    Double buffered 3D animation demo
**
**  6/21/07 v2.0 Lucio Di Jasio
**  8/03/07 v2.1 LDJ automated generation of vertexes
**  8/10/09 v3.0 LDJ TG port and new graphics
**  1/05/10 v4.0 LDJ porting on MikroE MMB
**
**  HW Req: PIC32MX4 MMB MikroE
**  SW Req: MMB.c Geometry3.c Graphic.c MMB.c primitive.c displaydriver.c
**  Ref:    www.exploringpic32.com
** 
**  This demo uses integer math to perform 3D drawing and 
**  animation with Isometric perspective
**  The accelerometer is used to provide rotation inputs 
**  38 cubes forming the digits "32" are shown always standing "up"
**      - press once more (any) button to start (scramble)
**      - press and keep pressed (any) button to have message
**          gravitate back in place
*/


// configuration bit settings
#pragma config POSCMOD=XT, FNOSC=PRIPLL
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_1
#pragma config FWDTEN=OFF, CP=OFF, BWP=OFF

#include <plib.h>
#include <math.h>
#include "Graphics/Graphics.h"

#include "MMB.h"
#include "graphic.h"
#include "Geometry2.h"


extern const char Font8x8[];
extern short VCount;

// define the animation step (angle)
#define STEP    5  

// define the points table
#define MAXP    100 //1100
point plist[ MAXP];           
short pc = MAXP;                    // points counter

// define the polygons table
#define MAXPOLY 100
poly pylist[ MAXPOLY];
short pyc = MAXPOLY;                  // polygons counter

// define the objects table
#define MAXOBJ 2
object olist[ MAXOBJ];
short objc = MAXOBJ;                  // object counter



void drawPolygons( short p)
{   // draws all vectors in the 4 point poligon of index p
    point a, b;
    
    // compute the first vector coordinates 
    matrixProd( plist[pylist[p][0]], a);
    matrixProd( plist[pylist[p][1]], b);
    // draw the vector
    gLine( a[0], a[1], b[0], b[1] );

    // compute the first vector coordinates 
    matrixProd( plist[pylist[p][2]], a);
    // draw the vector
    gLine( a[0], a[1], b[0], b[1] );

    // compute the first vector coordinates 
    matrixProd( plist[pylist[p][3]], b);
    // draw the vector
    gLine( a[0], a[1], b[0], b[1] );

    // compute the first vector coordinates 
    matrixProd( plist[pylist[p][0]], a);
    // draw the vector
    gLine( a[0], a[1], b[0], b[1] );
        
} // drawPolygons


void drawObject( short obj)
{
    short i;
    setColor( MAXOBJ - obj);       

    for( i=0; i< olist[ obj].n; i++)
        drawPolygons( olist[ obj].p+i);

}// drawObject



short appPoint( short x, short y, short z)
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
} // appPoint


short appPoly( short a, short b, short c, short d)
{   
    poly *p = &pylist[ --pyc];
    (*p)[0] = a; (*p)[1] = b; (*p)[2] = c; (*p)[3] = d;
    return pyc;
} // appPoly    


short cube( short x, short y, short z, short s)
{ // generates a cube at x,y,z coordinates, of side s

  short a, b ,c, d, e, f, g, h, p;
  // verify there is room in the tables
    if ((pyc>3)  && (pc>7))
    {
      // generate all vertexes of the cube
        a = appPoint( x, y, z);        // A
        b = appPoint( x, y+s, z);      // B
        c = appPoint( x+s, y+s, z);    // C
        d = appPoint( x+s, y, z);      // D
        e = appPoint( x+s, y, z+s);    // E
        f = appPoint( x+s, y+s, z+s);  // F
        g = appPoint( x, y+s, z+s);    // G
        h = appPoint( x, y, z+s);      // H

      // generate all the polygons with proper orientation (normal outbound)
        appPoly( a, b, c, d);
        appPoly( e, f, g, h);
        appPoly( b, g, f, c); 
        p = appPoly( d, e, h, a);       
        // appPoly( d, c, f, e);
        // appPoly( g, b, a, h);

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
                    n+=4;
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

int ax[16], ay[16], az[16];
int ox, oy, oz;

void AccAverage(int *x, int* y, int* z)
{   // average accelerometer reading over last 16 samples
    static int j=0;
    int i, sx, sy, sz;
    
    MMBAccRead( &ax[j], &ay[j], &az[j]);
    j++;
    j %=16;
    
    // sum
    sx = sy= sz = 0;
    for( i=0; i<16; i++)
    {
        sx += ax[i];
        sy += ay[i];
        sz += az[i];
    }
    // average
    *x = sx>>4;     
    *y = sy>>4;
    *z = sz>>4;    
}

char s[32];


int qatan( int z, int y)
{   // four quadrant atan
    double r, alpha;
    
    // remove singularities
    if (z == 0)
        r = 0.0;
    else if (y == 0)
        r = (double)z / 0.0001;            
    else
        r = (double)z / (double)y;                        
        
    // find quadrant
    if ( z >= 0)    // 1st or 2nd quadrant
    {
        if ( y >= 0)    // Q1
            alpha = atan(r);
        else            // Q2
            alpha = M_PI+atan(r);
    }
    else    // Q3 or Q4
    {
        if ( y < 0)     // Q3
            alpha = M_PI+atan(r);
        else            // Q4
            alpha = atan(r);
    }        
            
    // compute angle in deg
    return (alpha * 180 / M_PI) + 0.5;     
} // qatan
     
int main( void)
{
    double t;       // profiling
    double alpha, beta, gamma;
    short  i, j;
    char c;
    char start;     // wait before rotate
    char roll;      // keep looping 
    int x, y, z;    // accelerometer input
    
    // init offsets
    x0 = 128;
    y0 = 50;
    z0 = 50;
    
    // hardware and video initialization 
    MMBInit();          // init processor and MMB peripherals
    MMBAccInit();       // configure the accelerometer
    initVideo();        // init the animation graphics module
    
        
    // 3. main loop
    while( 1)
    {
        roll = 1;
        start = 0;
        gClearScreen(); // clean the hidden screen
    
        // splash screen
        setColor( 1);  // red
        AT(4,10); putsV( "Accelerometer + 3D Animation");
            AT(8,12); putsV( "Lucio Di Jasio v1.0");
        AT(4,14); putsV( "for MikroE PIC32MMB Board");
        
        copyV();                        // update visible screen
        while( !MMBReadKey());
        srand( ReadCoreTimer());
        
        
        // calibrate acc
        for (i=0; i<32; i++)
            AccAverage( &x, &y, &z);
        ox = x; oy = y; oz = z + 256;    
        
        // select one of three demo modes
        c = JOY_DOWN ; 
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
            createObject( 0, "2", 200);                 
        else 
            createObject( 0, "\\", 400); // single cube
    
    
        while( roll)
        {
            double r;
            
            // get the accelerometer inputs
            AccAverage( &x, &y, &z);
            // subtract the calibration offsets
            x -= ox; y -= 2*oy;  z -= oz;

            //perform the update
            a[0] = -qatan( z, y);
            b[0] = 0;
            g[0] = 90-qatan( y, x);
            
            // show the totals
            sprintf( s, "%d x  %d y  %d z", a[0], b[0], g[0]);
                            
            j = 1;
            // repeat rotation j times
            while ( j-- > 0)
            {  
                gClearScreen();                 // clear the hidden screen
                setColor( 2);  // green
                AT( 0, 0); putsV( s);
    
                for( i=objc; i<MAXOBJ; i++)
                {
                    initWorld( a[0], b[0], g[0], x0, y0, z0);
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






