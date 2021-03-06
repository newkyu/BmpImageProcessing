#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "esw_bmp.h"
#include "esw_process2.h"

//* Bit Plane Function *//
unsigned char **bitPlane(unsigned char **output,BITMAPINFOHEADER bitmapInfoHeader)
{
  int HEIGHT = bitmapInfoHeader.biHeight;
  int WIDTH = bitmapInfoHeader.biWidth;
  double c;
  int level;
  while(1)
  {
    printf(">> Enter the bit plane Level[0~7] : "); scanf("%d",&level);

    if (level >7 || level <-1)
       printf("Range of level 0~7\n");
    else
    {
      printf("start bitplane .. \n");
      for (int i = 0; i<HEIGHT;i++)
      {
        for (int j = 0; j<WIDTH;j++)
        {
          c = ((int)output[i][j]/(int)pow(2,level))%2;
 	  c = c*255;
	  output[i][j]=c;
        }
      }
      break;
    }   
  }

  return output;
}

//* Dithering Function *//
unsigned char **dithering(unsigned char **output,BITMAPINFOHEADER bitmapInfoHeader)
{
  
  while(1) 
  {
    int flag = 1;
    char option[2];
  
    printf(">> Enter the dithering option \n   $ d # d filter \n   $ d2 # d2 filter \n   $ f # Floyd-steinberg \n  : ");
    scanf("%s",option);

    switch(option[0])
    {    
      case 0x64: // D or D2 filter
         printf("start %s filter ..\n",option);
         output = Dfilter(output,option);
         break;
       
      case 0x66: // Floyd-steinberg
         printf("start Floyd-steinberg filter ..\n");
         output = floyd(output,bitmapInfoHeader);
         break;

      default:
         printf("Option Error \n");
         flag = 0;
         break;
    }

    if(flag)
      break;
  }
  return output;
}

unsigned char **floyd(unsigned char **output,BITMAPINFOHEADER bitmapInfoHeader)
{
   int HEIGHT = bitmapInfoHeader.biHeight;
   int WIDTH = bitmapInfoHeader.biWidth;

   unsigned char **y = ( unsigned char **)malloc(sizeof( unsigned char *)*HEIGHT);
   for (int i = 0; i<HEIGHT;i++)
   {   y[i] = (unsigned char *)malloc(sizeof(unsigned char)*WIDTH);  }
 

   float **z = (float **)malloc(sizeof( float *)*HEIGHT);
   for (int i = 0; i<HEIGHT+1;i++)
   {   z[i] = (float*)malloc(sizeof(float)*WIDTH+1);  }
  
   float e ;
   
   
   for(int i = 0;i<HEIGHT+1;i++)
   {
      for(int j = 0;j<WIDTH+1;j++)
      {
          if(i == 0 || j == 0)
             z[i][j] = 0;
          else
             z[i][j] = output[i-1][j-1];
          
      }
   }

   for(int i = 1;i<HEIGHT-1;i++)
   {
      for(int j = 1;j<WIDTH-1;j++)
      {
          if(z[i][j] < 128)
          {
             y[i-1][j-1] = 0;
             e = z[i][j];
          }
          else
          {
             y[i-1][j-1] = 255;
             e = z[i][j]-255; 
          }
          z[i][j+1] = z[i][j+1]+7*e/16;
          z[i+1][j-1] = z[i+1][j-1]+3*e/16;
          z[i+1][j] = z[i+1][j]+5*e/16;
          z[i+1][j+1] = z[i+1][j+1]+e/16;
      }
   }
    
 
   return y;
}

unsigned char **Dfilter(unsigned char **output, char opt[2])
{
    int size;
    int D[2][2] = {{0,128},{192,64}}; // filter D
    int D2[4][4] = {{0,128,32,160},{192,64,224,96}, \
                    {48,176,16,144},{240,112,208,80}}; // filter D2
                    
    if(opt[1] - '0' == 2)
       size = 4;
    else 
       size = 2;
       
    
    
    int R[128*size][128*size];

   for(int i = 0;i<size;i++)
   {   
      for(int j = 0;j<size;j++)
      {   
        for(int x = i;x<256;x =x+size)
        {
          for(int y = j;y<256;y = y+size)
          {
             if(size == 2) // D
                R[x][y] = D[i][j];
             else 
                R[x][y] = D2[i][j];
           }
         }
      }
    } 

   for(int i = 0;i<256;i++)
   {   
     for(int j = 0;j<256;j++)
     {   
        output[i][j] = (output[i][j] > R[i][j]) ? 255:0;
     }
  }
  
  return output;

}

//* Resolution Function *//
unsigned char **resolution(unsigned char **output,BITMAPINFOHEADER bitmapInfoHeader)
{
  int HEIGHT = bitmapInfoHeader.biHeight;
  int WIDTH = bitmapInfoHeader.biWidth;
  int level;
 
  while(1)
  {
    printf(">> Enter the resolution reduction Level[1~9] : "); scanf("%d",&level);
    if (level >9 || level <1)
        printf("Range of level 1~9\n");
    else
    {
      level = level+1;
       printf("start resolution reduction .. \n");
      // resolution reduction
      for(int i = 0; i<HEIGHT;i= i+level)
      {
         for(int j = 0;j<WIDTH;j= j+level)
         {
   	    int temp = output[i][j];
            for(int x = i;x<i+level;x++)
            {
               if(x>=HEIGHT) break;
	       for(int y = j;y<j+level;y++)
               {
                  if(y>=WIDTH) break;
 	  	  output[x][y] = temp;
               }
            }
         }
      }
      break;
    }
    
  }
  
 	
  return output;
}

