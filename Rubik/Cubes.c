/*
**  Cubes.c    Double buffered 3D animation demo
**
**  6/21/07 v2.0 Lucio Di Jasio
**  8/03/07 v2.1 LDJ automated generation of vertexes
**  8/10/09 v3.0 LDJ TG port and new graphics
**  1/07/10 v4.0 MIkroE MMB port and 3D fill
**
**  HW Req: PIC32MX4 MMB MikroE
**  SW Req: MMB.c Geometry3.c Graphic.c
**  Ref:    www.exploringpic32.com
** 
**  This demo uses integer math to perform 3D drawing
**  the joystick is pressed after start up:
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
#include "Graphics/Graphics.h"

#include "MMB.h"
#include "graphic.h"
#include "geometry4.h"
#include "poly.h"

// Cube faces color
#define R_R     1
#define R_Y     3
#define R_G     2
#define R_B     4
#define R_O     5
#define R_W     7

#define U        28         // cubicle unitary side
#define G        4          // cursor size
#define MAXF     54+3       // faces 
#define MAXOBJ   26+1       // objects

extern const char Font8x8[];

// define the animation step (angle deg)
#define STEP    2  
#define AOFFS   20
#define BOFFS   -20
#define GOFFS   0

typedef struct { short p; short color; }face; 
typedef struct { short f; short n; } object;

// define a list of cube facets
face flist[ MAXF];                      // list of faces
short facec = MAXF;                     // face counter
char flaglist[ MAXP];                   // additional flags for each point

// define the objects table
object olist[ MAXOBJ];                  // list of objects
short objc = MAXOBJ;                    // object counter

// define the cube matrix
char c[3][3][3];   // each element points to an object in olist [0..MAXOBJ]

// define origin coordinates 
short x0, y0, z0;

// define cursor
int qx, qy;         // current cursor
char cursor;        // 1 = on; 0= off
vertex grid[3][3];  // define cursor grid


// define object orientation matrix
short a[MAXOBJ], b[MAXOBJ], g[MAXOBJ];

//define random animation matrix
short r[MAXOBJ], k[MAXOBJ];

short newFace( short p, short c)
{
   flist[ --facec].p = p;
   flist[ facec].color = c;
   return facec;
} // new face

short newObj( short f, short n)
{
    olist[ --objc].f = f;  // the last face inserted 
    olist[ objc].n = n;     // the number of faces inserted
    return objc;
} // new obj
    
void rotatePoint( tmatrix mo, short p)
{
    point pt;
    
    // rotate using given matrix
    matrixProd( mo, plist[ p], pt);   
    // make it permanent
    memcpy( plist[ p], pt, sizeof(point));    
} // rotatePoint

    
void rotatePoly( tmatrix mo, short p)
{   // rotates (permanently) the 4 points of poligon of index p 
    point pa, pb, pc, pd, pt;
    
    // check if not rotated already (as part of previous poly)
    if ( !flaglist[ pylist[p][0]]++)
        rotatePoint( mo, pylist[p][0]);
    if ( !flaglist[pylist[p][1]]++)
        rotatePoint( mo, pylist[p][1]);
    if ( !flaglist[pylist[p][2]]++)
        rotatePoint( mo, pylist[p][2]);
    if ( !flaglist[pylist[p][3]]++)
        rotatePoint( mo, pylist[p][3]);
            
} // rotatePoly


void rotateObject( tmatrix mo, short obj)
{  // rotates (permanently) all faces of an object
    short i, pf;
    // clear the flags 
    for (i=pc; i<MAXP; i++)
        flaglist[i] = 0;
        
    // rotate each face(poly)    
    for( i=0; i< olist[ obj].n; i++)
    {
        pf = olist[ obj].f+i;
        rotatePoly( mo, flist[ pf].p);
    }
} // rotate object
    

void initGridP( int x, int y)
{ 
    point pt, pa;
    
    // define point coordinates in 3D space 
    pt[0] = (x-1)* (U+2); 
    pt[1] = (y-1)* (U+2); ; 
    pt[2] = U+U/2+3;
    
    // world rotate point (projection)
    matrixProd( mw, pt, pa);   
    
    // get grid vertex  2D coord
    grid[ x][ y].x = pa[0];   
    grid[ x][ y].y = pa[1];
    
} // initGridPoint     


void initGrid( void)
{ // init the cursor grid coordinate matrix
    initGridP( 0, 2);     initGridP( 1, 2);     initGridP( 2, 2); 

    initGridP( 0, 1);     initGridP( 1, 1);     initGridP( 2, 1); 

    initGridP( 0, 0);     initGridP( 1, 0);     initGridP( 2, 0); 
    
} // initGrid


void drawCursor( int x, int y)
{ // paint the cursor on the front face
    int gx, gy, va, vb, vc, vd;
    
    // get the 2D coords
    gx = grid[x][y].x; gy = grid[x][y].y;
    
    // clear list of vertexes
    vxc = MAXV; 

    // put the cursor vertex on the vlist
    va = newVertex( gx-G, gy);
    vb = newVertex( gx+G, gy);
    vc = newVertex( gx, gy+G);
    vd = newVertex( gx, gy-G);
    
    // draw cursor as a purple triangle
    setColor( 6);
    FillTrig( va, vb, vc);
    FillTrig( va, vb, vd);
} // drawCursor
        
    
short newSide( short alpha, short beta, short gamma, short c1)
{ // generates a cube center side (1 x face) 
  // alpha beta gamma indicate the rotation
  // c1 is the colors 
  // return index to face table  (containing 1 new face)
  
  short f, po;
  short x, y, z;
  short a, b ,c, d;
  
  // determin vertex coordinates  
  x = (+ U/2 );
  y = (+ U/2 );
  z = (U + U/2 +3);
  
  // rotation
  initMatrix( mo, alpha, beta, gamma, 0, 0, 0);

  // verify there is room in the tables
  if ((facec>=1) && (pyc>=1) && (pc>=4))
  {
      // generate all points necessary for the cube corner
      // in base position
      a = newPoint( x,   y,   z);        // A        
      b = newPoint( x,   y-U, z);        // B
      c = newPoint( x-U, y-U, z);        // C
      d = newPoint( x-U, y,   z);        // D
        
      // generate all the polygons and faces 
      // with proper orientation (normal outbound)
      f = newFace ( newPoly( a, b, c, d), c1);
        
      // add an object with 1 face
      po = newObj( f, 1);

      // rotate as required
      rotateObject( mo, po);
              
      // returns the object index
      return po;
  } // if 
    else 
        while( 1);
} // new cube side


short newEdge( short alpha, short beta, short gamma, 
                 short c1, short c2)
{ // generates a cube edge (2 x face) 
  // alpha beta gamma indicate the rotation
  // c1, c2 are the colors for the faces in order
  // return index to face table  (containing 2 new faces)
  
  short fo, po;
  short x, y, z;
  short a, b ,c, d, f, g;
  
  // determin vertex coordinates  
  x = (+ U/2 );
  y = (U + U/2 +3);
  z = (U + U/2 +3);
  
  // rotation
  initMatrix( mo, alpha, beta, gamma, 0, 0, 0);

  // verify there is room in the tables
    if ((facec>=2) && (pyc>=2) && (pc>=6))
    {
      // generate all points necessary for the cube corner
      // in base position
        a = newPoint( x,   y,   z);        // A        
        b = newPoint( x,   y-U, z);        // B
        c = newPoint( x-U, y-U, z);        // C
        d = newPoint( x-U, y,   z);        // D
        f = newPoint( x,   y,   z-U);      // F
        g = newPoint( x-U, y,   z-U);      // G
        
        // rotate as required
        
      // generate all the polygons and faces 
      // with proper orientation (normal outbound)
        newFace ( newPoly( a, b, c, d), c1);
        fo = newFace ( newPoly( d, g, f, a), c2);

      // add an object with 3 faces
        po = newObj( fo, 2);
        
      // rotate as required
        rotateObject( mo, po);

      // returns the object index
        return po;
    } // if 
    else 
        while( 1);
} // new cube edge


short newCorner( short alpha, short beta, short gamma, 
                 short c1, short c2, short c3)
{ // generates a cube corner (3 x face) 
  // alpha beta gamma indicate the rotation
  // c1, c2, c3 are the colors for the three faces in order
  // return index to face table  (containing 3 new faces)
  
  short fo, po;
  short x, y, z;
  short a, b ,c, d, e, f, g;
  
  // determin vertex coordinates  
  x = (U + U/2 +3);
  y = (U + U/2 +3);
  z = (U + U/2 +3);
  
  // rotation
  initMatrix( mo, alpha, beta, gamma, 0, 0, 0);

  // verify there is room in the tables
    if ((facec>=3) && (pyc>=3) && (pc>=7))
    {
      // generate all points necessary for the cube corner
      // in base position
        a = newPoint( x,   y,   z);        // A        
        b = newPoint( x,   y-U, z);        // B
        c = newPoint( x-U, y-U, z);        // C
        d = newPoint( x-U, y,   z);        // D
        e = newPoint( x,   y-U, z-U);      // E
        f = newPoint( x,   y,   z-U);      // F
        g = newPoint( x-U, y,   z-U);      // G
        
      // generate all the polygons and faces 
      // with proper orientation (normal outbound)
        newFace ( newPoly( a, b, c, d), c1);
        newFace ( newPoly( d, g, f, a), c2);
        fo = newFace( newPoly( a, f, e, b), c3); 
        
      // add an object with 3 faces
        po = newObj( fo, 3);
        
      // rotate as required
        rotateObject( mo, po);

      // returns the object index
        return po;
    } // if 
    else 
        while( 1);
} // new cube corner


void newCube( void)
{  // creates a new 3x3x3 Rubik cube 
    short po, n;
    int u = U + U/2;
    
    facec = MAXF;
    objc = MAXOBJ;
    
    // generate 8 cube corners
    c[2][2][2] = newCorner( +0, +0,  +00, R_Y, R_O, R_B); // A
    c[2][0][2] = newCorner( +0, +0,  +270, R_Y, R_B, R_R); // B
    c[0][0][2] = newCorner( +0, +0,  180, R_Y, R_R, R_G); // C
    c[0][2][2] = newCorner( +0, +0,  +90, R_Y, R_G, R_O); // D
    c[0][2][0] = newCorner( +0, 180, +00, R_W, R_O, R_G); // G
    c[0][0][0] = newCorner( +0, 180, -90, R_W, R_G, R_R); // H
    c[2][0][0] = newCorner( +0, 180, 180, R_W, R_R, R_B); // E
    c[2][2][0] = newCorner( +0, 180, +90, R_W, R_B, R_O); // F
   
    // generate 12 cube edges
    c[1][2][2] = newEdge(   0,   0,   0, R_Y, R_O); // DA
    c[2][1][2] = newEdge(   0,   0, -90, R_Y, R_B); // AB
    c[1][0][2] = newEdge(   0,   0, 180, R_Y, R_R); // BC
    c[0][1][2] = newEdge(   0,   0,  90, R_Y, R_G); // CD

    c[1][2][0] = newEdge(   0, 180,   0, R_W, R_O); // FG
    c[0][1][0] = newEdge(   0, 180, -90, R_W, R_G); // GH
    c[1][0][0] = newEdge(   0, 180, 180, R_W, R_R); // HE
    c[2][1][0] = newEdge(   0, 180, +90, R_W, R_B); // EF

    c[2][2][1] = newEdge(   0,  90,   0, R_B, R_O); 
    c[2][0][1] = newEdge(   0,  90, 180, R_B, R_R);
    c[0][2][1] = newEdge(   0, -90,   0, R_G, R_O); // DG
    c[0][0][1] = newEdge(   0, -90, 180, R_G, R_R);
    
    // add 6 more sides
    c[1][1][2] = newSide(   0,   0,   0, R_Y);
    c[2][1][1] = newSide(   0,  90,   0, R_B);
    c[1][1][0] = newSide(   0, 180,   0, R_W);
    c[0][1][1] = newSide(   0, -90,   0, R_G);
    c[1][2][1] = newSide( -90,   0,   0, R_O);
    c[1][0][1] = newSide( +90,   0,   0, R_R);
    
    olist[0].f = 0; // object 0 is empty (will be used as a temp)   
    olist[0].n = 0;  
    
} // newObject     
   

void drawObject( tmatrix mo, short obj)
{
    short i, pf;
    
    for( i=0; i< olist[ obj].n; i++)
    {
        pf = olist[ obj].f+i;
        setColor( flist[ pf].color);
        drawPolygons( flist[ pf].p);
    }
//    // the last poly drawn is the key face 
//    // vertexes are still in the stack
//    // circle them in red if the object is in focus
//    if (( obj == c[qx][qy][2]) && cursor)
//    {
//        setColor( 13);
//        gLine( vlist[1].x, vlist[1].y, vlist[2].x, vlist[2].y);
//        gLine( vlist[2].x, vlist[2].y, vlist[3].x, vlist[3].y);
//        gLine( vlist[3].x, vlist[3].y, vlist[4].x, vlist[4].y);
//        gLine( vlist[4].x, vlist[4].y, vlist[1].x, vlist[1].y);
//        gLine( vlist[1].x, vlist[1].y, vlist[3].x, vlist[3].y);
//        gLine( vlist[2].x, vlist[2].y, vlist[4].x, vlist[4].y);
//    }
}// drawObject


void rotateC( int x, int y, int z, int v, int s)
{
    // rotate the C matrix ob object pointers
    int i, j, t;
    for( i=0; i<((s>0)?1:3); i++)
    {
        if ( x)
        {   // rotate corners (use object zero as a temp)
            t = c[v][0][0]; 
            c[v][0][0]= c[v][0][2];
            c[v][0][2]= c[v][2][2]; 
            c[v][2][2]= c[v][2][0];
            c[v][2][0]= t;
            // rotate edges
            t = c[v][0][1]; 
            c[v][0][1]= c[v][1][2];
            c[v][1][2]= c[v][2][1]; 
            c[v][2][1]= c[v][1][0];
            c[v][1][0]= t;
        }
        else if ( y)
        {   // rotate corners
            t = c[0][v][0]; 
            c[0][v][0]=c[2][v][0];
            c[2][v][0]=c[2][v][2]; 
            c[2][v][2]=c[0][v][2];
            c[0][v][2]= t;
            // rotate edges
            t = c[2][v][1]; 
            c[2][v][1]=c[1][v][2];
            c[1][v][2]=c[0][v][1]; 
            c[0][v][1]=c[1][v][0];
            c[1][v][0]= t;
        }
        else if ( z)
        {   // rotate corners
            t = c[0][0][v]; 
            c[0][0][v]= c[0][2][v];
            c[0][2][v]= c[2][2][v]; 
            c[2][2][v]= c[2][0][v];
            c[2][0][v]= t;
            // rotate edges
            t = c[0][1][v]; 
            c[0][1][v]= c[1][2][v];
            c[1][2][v]= c[2][1][v]; 
            c[2][1][v]= c[1][0][v];
            c[1][0][v]= t;
        }
    } // for i
} // rotateC

        
void rotateRow( int x, int y, int z, int v, int s)
{
    // rotate all objects matching x, y, z mask  with value v
    // s verse of rotation
    int j, i, ix, iy, iz;
    
    // animation loop 0-90 deg
    for(j = 0; j <= 90; j+= STEP)
    {
        // clear painting scren
        gClearScreen();                 // clear the hidden screen

        // search all objects for matching pattern
        for( iz=0; iz<3; iz++)
            for( iy=0; iy<3; iy++)
                for(ix=0; ix<3; ix++)
                {   
                    if ( ix*iy*iz == 1) continue; // ignore c[1][1][1]
                    i = c[ix][iy][iz];

                    // check if requires rotation
                    if (( (ix+1)*x == v+1) || ((iy+1)*y == v+1) || ((iz+1)*z == v+1))
                    {  // rotate the corresponding axis
                        // transform and draw the object                        
                        initMatrix( mo, x*s*j, y*s*j, z*s*j, 0, 0, 0);
                        drawObject( mo, i);

                        // if final rotation
                        if ( j == 90)
                            rotateObject( mo, i);
                    }
                    else 
                    {
                        // transform and draw the object                        
                        initMatrix( mo, 0, 0, 0, 0, 0, 0);
                        drawObject( mo, i);
                    }    
                    
                }
        
        
        // update visible screen
        copyV();            
    } // for j steps

    // update pointer matrix c[][][]
    rotateC( x, y, z, v, s);    
        
} // animate rotation of one row/face   


void rotateCube( int alpha, int beta, int gamma)
{
    int j, i, ar, br, gr;
    
    for(j = 0; j <= 90; j+= STEP)
    {                        
        // clear painting screen
        gClearScreen();                 // clear the hidden screen
        
       // rotate the corresponding axis
        ar = alpha * j;
        br = beta * j;
        gr = gamma * j ;

        for( i=objc; i<MAXOBJ; i++)
        {
           // transform and draw the object                        
            initMatrix( mo, ar, br, gr, 0, 0, 0);
            drawObject( mo, i);

            // if final rotation
            if ( j == 90)
                rotateObject( mo, i);
        }
                
        // update visible screen
        copyV();

    } // for j steps

    // update pointer matrix c[][][]
    
    for ( i=0; i<3; i++)
    {
        if (( alpha<0)||( beta<0)||( gamma<0))
            rotateC( ( alpha!=0), ( beta!=0), ( gamma!=0), i, -1);    
        else
            rotateC( ( alpha!=0), ( beta!=0), ( gamma!=0), i, +1);    
    }
} // animate rotation of entire cube


// ************************************************************
int main( void)
{
//    double t;
    short v, i, j;
    char start;
    char roll;
    char s[32], kj;

    // init offsets
    x0 = 125;
    y0 = 100;
    z0 = 200;
    
    // hardware and video initialization 
    MMBInit();
    initVideo();
    initMatrix( mw, AOFFS, BOFFS, GOFFS, x0, y0, z0);
        
    // 3. main loop
    while( 1)
    {
        roll = 1;
        start = 0;
        gClearScreen();          
    
        // splash screen
        setColor( 2);  // red
        AT(8, 1); putsV( "Rubik's Cube Demo");
        AT(8, 3); putsV( "   LDJ v1.0");
        AT(8, 5); putsV( " for PIC32MX4 MMB");
        copyV();                        // update visible screen
        while( !MMBReadKey());
        srand( ReadCoreTimer());
             
        // clear all objects
        objc = MAXOBJ;
        pyc = MAXPOLY;
        pc = MAXP;
        
        // init all objects angles
        for( i=0; i<MAXOBJ; i++)
            a[i] = b[i] = g[i] = 0;
     
        // create the rubik cube
        newCube();
        
        // init cursor and define grid
        qx = 1; qy = 1;
        initGrid();        
        
        while ( roll)
        {
            // paint cube in current position (with cursor)
            gClearScreen();                 // clear the hidden screen
            for( i=objc; i<MAXOBJ; i++)
            {
                initMatrix( mo, 0, 0, 0, 0, 0, 0);
                drawObject( mo, i);
            }
            drawCursor( qx, qy);
            copyV();
            
            // read the joystick and rotate center rows 
            kj = MMBGetKey();
            
            if ( kj & 0x80)         // long pressure 
            {
                switch( kj & 0x7F){
                  case JOY_RIGHT:   // rotate whole cube
                        rotateCube( 0, +1, 0);
                    break;
                  case JOY_LEFT:    // rotate whole cube
                        rotateCube( 0, -1, 0);
                    break;
                  case JOY_UP:      // rotate whole cube
                        rotateCube( -1, 0, 0);
                    break;
                  case JOY_DOWN:    // rotate whole cube
                        rotateCube( +1, 0, 0);
                    break;
                  case JOY_SELECT:  // rotate face counter clockwise
                        rotateRow( 0, 0, 1, 2, +1);
                    break;
                }
            }
            else                    // short pressure
            {                       
                switch( kj){
                  case JOY_RIGHT:   // rotate only the current row 
                    if (qx == 2)
                        rotateRow( 0, 1, 0, qy, +1);
                    else qx++;
                    break;
                  case JOY_LEFT:    // rotate only the current row 
                    if (qx == 0)
                        rotateRow( 0, 1, 0, qy, -1);
                    else qx--;
                    break;
                  case JOY_UP:      // rotate only the current row 
                    if (qy == 2)
                        rotateRow( 1, 0, 0, qx, -1);
                    else qy++;
                    break;
                  case JOY_DOWN:    // rotate only the current row 
                    if (qy == 0)
                        rotateRow( 1, 0, 0, qx, +1);
                    else qy--;
                    break;
                  case JOY_SELECT:  // rotate face clockwise
                        rotateRow( 0, 0, 1, 2, -1);
                    break;
                } // switch
            } // short pressure     
        } //roll        
    } // main loop
} // main





/*
// show the totals
        //sprintf( s, "%d Points  %d Poly  %d Obj", MAXP-pc, MAXPOLY-pyc, MAXOBJ-objc);
    
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
                
                //if ( !start)                    // summary 
                //{
                //    setColor( 2);  // green
                //    AT( 0, 0); putsV( s);
                //}
                
                for( i=objc; i<MAXOBJ; i++)
                {
                    initWorld( a[MAXOBJ-1]+0, b[MAXOBJ-1]+0, g[MAXOBJ-1]+0, x0, y0, z0);
                    drawObject( i);
                                        
                    if ( kj = MMBReadKey())
                    {   // joystick pressed
                        if (kj == JOY_LEFT)
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
                        else if (kj == JOY_UP) // return to menu
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
*/
