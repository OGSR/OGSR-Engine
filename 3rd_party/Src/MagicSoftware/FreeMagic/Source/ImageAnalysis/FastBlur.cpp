// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2002.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#include <math.h>

//---------------------------------------------------------------------------
void Blur1D (int xdim, float* im, float scale)
{
    // Precondition for stability:  0 < scale < exp(0.5)
    // Good choice for iterative blurring is scale = exp(0.25)

    float logscale = float(log(scale));
    int x;
    float* temp = new float[xdim];

    for (x = 0; x < xdim; x++) 
    {
        float rxp = x+scale, rxm = x-scale;
        int xp = (int) floor(rxp), xm = (int) ceil(rxm);

        float xsum = -2*im[x];
        if ( xp >= xdim-1 )  // use boundary value
            xsum += im[xdim-1];
        else  // linearly interpolate
            xsum += im[xp]+(rxp-xp)*(im[xp+1]-im[xp]);
        if ( xm <= 0 )  // use boundary value
            xsum += im[0];
        else  // linearly interpolate
            xsum += im[xm]+(rxm-xm)*(im[xm]-im[xm-1]);

        temp[x] = im[x]+logscale*xsum;
    }

    for (x = 0; x < xdim; x++)
        im[x] = temp[x];

    delete[] temp;
}
//---------------------------------------------------------------------------
void Blur2D (int xdim, int ydim, float** im, float scale)
{
    // Precondition for stability:  0 < scale < exp(0.25)
    // Good choice for iterative blurring is scale = exp(0.125)

    float logscale = float(log(scale));
    int x, y;
    float** temp = new float*[ydim];
    for (y = 0; y < ydim; y++)
        temp[y] = new float[xdim];

    for (y = 0; y < ydim; y++) 
    {
        float ryp = y+scale, rym = y-scale;
        int yp = (int) floor(ryp), ym = (int) ceil(rym);

        for (x = 0; x < xdim; x++) 
        {
            float rxp = x+scale, rxm = x-scale;
            int xp = (int) floor(rxp), xm = (int) ceil(rxm);

            // x portion of second central difference
            float xsum = -2*im[y][x];
            if ( xp >= xdim-1 )  // use boundary value
                xsum += im[y][xdim-1];
            else  // linearly interpolate
                xsum += im[y][xp]+(rxp-xp)*(im[y][xp+1]-im[y][xp]);
            if ( xm <= 0 )  // use boundary value
                xsum += im[y][0];
            else  // linearly interpolate
                xsum += im[y][xm]+(rxm-xm)*(im[y][xm]-im[y][xm-1]);

            // y portion of second central difference
            float ysum = -2*im[y][x];
            if ( yp >= ydim-1 )  // use boundary value
                ysum += im[ydim-1][x];
            else  // linearly interpolate
                ysum += im[yp][x]+(ryp-yp)*(im[yp+1][x]-im[yp][x]);
            if ( ym <= 0 )  // use boundary value
                ysum += im[0][x];
            else  // linearly interpolate
                ysum += im[ym][x]+(rym-ym)*(im[ym][x]-im[ym-1][x]);

            temp[y][x] = im[y][x]+logscale*(xsum+ysum);
        }
    }

    for (y = 0; y < ydim; y++)
        for (x = 0; x < xdim; x++)
            im[y][x] = temp[y][x];

    for (y = 0; y < ydim; y++)
        delete[] temp[y];
    delete[] temp;
}
//---------------------------------------------------------------------------
void Blur3D (int xdim, int ydim, int zdim, float*** im, float scale)
{
    // Precondition for stability:  0 < scale < exp(0.125)
    // Good choice for iterative blurring is scale = exp(0.0625)

    float logscale = float(log(scale));
    int x, y, z;
    float*** temp = new float**[zdim];
    for (z = 0; z < zdim; z++)
    {
        temp[z] = new float*[ydim];
        for (y = 0; y < ydim; y++)
            temp[z][y] = new float[xdim];
    }

    for (z = 0; z < zdim; z++) {
        float rzp = z+scale, rzm = z-scale;
        int zp = (int) floor(rzp), zm = (int) ceil(rzm);

        for (y = 0; y < ydim; y++) 
        {
            float ryp = y+scale, rym = y-scale;
            int yp = (int) floor(ryp), ym = (int) ceil(rym);

            for (x = 0; x < xdim; x++) 
            {
                float rxp = x+scale, rxm = x-scale;
                int xp = (int) floor(rxp), xm = (int) ceil(rxm);

                // x portion of second central difference
                float xsum = -2*im[z][y][x];
                if ( xp >= xdim-1 )  // use boundary value
                    xsum += im[z][y][xdim-1];
                else  // linearly interpolate
                    xsum += im[z][y][xp]+(rxp-xp)*(im[z][y][xp+1]-im[z][y][xp]);
                if ( xm <= 0 )  // use boundary value
                    xsum += im[z][y][0];
                else  // linearly interpolate
                    xsum += im[z][y][xm]+(rxm-xm)*(im[z][y][xm]-im[z][y][xm-1]);

                // y portion of second central difference
                float ysum = -2*im[z][y][x];
                if ( yp >= ydim-1 )  // use boundary value
                    ysum += im[z][ydim-1][x];
                else  // linearly interpolate
                    ysum += im[z][yp][x]+(ryp-yp)*(im[z][yp+1][x]-im[z][yp][x]);
                if ( ym <= 0 )  // use boundary value
                    ysum += im[z][0][x];
                else  // linearly interpolate
                    ysum += im[z][ym][x]+(rym-ym)*(im[z][ym][x]-im[z][ym-1][x]);

                // z portion of second central difference
                float zsum = -2*im[z][y][x];
                if ( zp >= zdim-1 )  // use boundary value
                    zsum += im[zdim-1][y][x];
                else  // linearly interpolate
                    zsum += im[zp][y][x]+(rzp-zp)*(im[zp+1][y][x]-im[zp][y][x]);
                if ( zm <= 0 )  // use boundary value
                    zsum += im[0][y][x];
                else  // linearly interpolate
                    zsum += im[zm][y][x]+(rzm-zm)*(im[zm][y][x]-im[zm-1][y][x]);

                temp[z][y][x] = im[z][y][x]+logscale*(xsum+ysum+zsum);
            }
        }
    }

    for (z = 0; z < zdim; z++)
        for (y = 0; y < ydim; y++)
            for (x = 0; x < xdim; x++)
                im[z][y][x] = temp[z][y][x];

    for (z = 0; z < zdim; z++)
    {
        for (y = 0; y < ydim; y++)
            delete[] temp[z][y];
        delete[] temp[z];
    }
    delete[] temp;
}
//---------------------------------------------------------------------------

#ifdef FASTBLUR_TEST

void main ()
{
    const int xdim = 256, ydim = 256;
    int y;

    float** image = new float*[ydim];
    for (y = 0; y < ydim; y++)
        image[y] = new float[xdim];

    // initialization of image[y][x] goes here...

    const int imax = 8;
    float scale = 1.0f, dScale = float(exp(0.125));
    for (int i = 1; i <= imax; i++, scale *= dScale)
    {
        Blur2D(xdim,ydim,image,scale);

        // save blurred image here...
    }

    for (y = 0; y < ydim; y++)
        delete[] image[y];
    delete[] image;
}

#endif


