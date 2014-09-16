/*
** fill.c
** 
*/



/*


void ClipLine(int x0, int y0, int x1, int y1)
{
    bool steep;
    int t ;
    int deltax, deltay, error;
    int x, y;
    int ystep;

    // advanced clipping 

    // 1. remove extreme cases when no part of the line is visible
    if (( x0<0)&&(x1<0))      
    return;         // entire line outside left edge
    if (( y0<0)&&(y1<0))       
    return;         // entire line below bottom
    if (( x0>clipx)&&(x1>=clipx))
    return;         // entire line outside right edge
    if (( y0>=clipy)&&(y1>-clipy))
    return;         // entire line above the top
         
     
    // 2. reduction to shallow lines (swap x and y)
    steep = ( Math.Abs(y1 - y0) > Math.Abs(x1 - x0));

    if ( steep )
    { // swap x and y
     t = x0; x0 = y0; y0 = t;
     t = x1; x1 = y1; y1 = t;
     t = clipx; clipx=clipy; clipy=t;
    }

    // 3. reduction to left to right drawing (swap ends)
    if (x0 > x1) 
    {  // swap ends
     t = x0; x0 = x1; x1 = t;
     t = y0; y0 = y1; y1 = t;
    }

    // 4. get the deltas
    deltax = x1 - x0;
    deltay = y1 - y0;

    // 5. check intersection with the y axis (only x0 could)
    if ( x0 < 0)
    {
    x0 = 0;                             // clip left edge
    // intercept y axis (x=0)
    y0=(y1*deltax-deltay*x1)/deltax;    
    }   

    // 4. check intersection with right edge (only x1 could)
    if (x1>clipx)			
    {                                      // clip right edge
     x1 = clipx;
     // intercept right edge (x=clipx)
     y1=(y1*deltax-deltay*x1+deltay*clipx)/deltax;
    }

    // 5. depending on line inclination  
    if (y0 < y1)       // rising
    {
     ystep = 1; 
     // 5.1 check y0 against bottom and y1 against top
     if (y0<0)
     {
         y0 = 0;
         // intercept x axis (y=0)
         x0=(deltay*x1-deltax*y1)/deltay;
     }    
     if (y1>clipy)
     {
         y1=0;
         // intercept top (y=clipy)
         x1=(deltay*x1-deltax*y1+deltax*clipy)/deltay;
     }    
    }    
    else               // falling
    {
     ystep = -1;
     // 5.2 check y1 against bottom and y0 against top
     if (y1<0)
     {
         y1 = 0;
         // intercept x axis (y=0)
         x1=(deltay*x1-deltax*y1)/deltay;
     }    
     if (y0>clipy)
     {
         y0=0;
         // intercept top (y=clipy)
         x0=(deltay*x1-deltax*y1+deltax*clipy)/deltay;
     }    
    }

    // 6. init the drawing loop
    deltay = Math.Abs( deltay);
    error = 0;
    y = y0;

    // 7. the drawing loop (left to right) 
    for (x = x0; x < x1; x++)
    {
     if ( steep)
         gF.DrawLine( myPen, y + 10, x + 10, y + 3 + 10, x + 10);
     else
         gF.DrawLine( myPen, x + 10, y + 10, x + 10, y + 10 + 3);
     
     error += deltay;
     
     if ( (error<<1) >= deltax)
     {
         y += ystep;
         error -= deltax;
     } // if 
    } // for
} // ClipLine 


*/


