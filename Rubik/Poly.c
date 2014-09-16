/*
** Poly.c
**  
** 2/21/10 LDJ created (updated for geometry4 style)
*/

#include "graphic.h"
#include "geometry4.h"
#include "poly.h"

// define the points table
point plist[ MAXP];           
short pc = MAXP;                    // points counter

// define the polygons table
poly pylist[ MAXPOLY];
short pyc = MAXPOLY;                  // polygons counter


// define the vertex space
vertex vlist[ MAXV];
short vxc = MAXV;                        // vertex counter

tmatrix mo, mw;

short newPoint( short x, short y, short z)
{ // add a point to the list and return its index
    point *p = &plist[--pc];

    (*p)[0] = x; (*p)[1] = y; (*p)[2] = z;
    
    return pc;
} // newPoint


short newPoly( short a, short b, short c, short d)
{   // add a poly to the list and return its index
    poly *p = &pylist[ --pyc];
    (*p)[0] = a; (*p)[1] = b; (*p)[2] = c; (*p)[3] = d;
    return pyc;
} // newPoly    


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
    point pa, pb, pc, pd, pt;
    short z, va, vb, vc, vd;
    vxc = MAXV;      // clear list of vertexes
    
    // compute 3 poly vertexes first 
    matrixProd( mo, plist[pylist[p][0]], pt);   
    matrixProd( mw, pt, pa); 
    matrixProd( mo, plist[pylist[p][1]], pt);
    matrixProd( mw, pt, pb); 
    matrixProd( mo, plist[pylist[p][2]], pt);
    matrixProd( mw, pt, pc); 
    
    // first check if the poy is visible 
    z = vectorZProduct( pa, pb, pc);
    if (z <= 0)
        return;     // poly is not facing viewer
    
    // compute last point
    matrixProd( mo, plist[pylist[p][3]], pt);
    matrixProd( mw, pt, pd); 

    va = newVertex( pa[0], pa[1]);
    vb = newVertex( pb[0], pb[1]);
    vc = newVertex( pc[0], pc[1]);
    vd = newVertex( pd[0], pd[1]);

    // draw the poly in two triangles
    FillTrig( va, vb, vc);
    FillTrig( va, vc, vd); 
        
} // drawPolygons
