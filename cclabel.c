/*******************************************************************/
/* vtemp      Compute local max operation on a single byte image   */
/*******************************************************************/

#include "VisXV4.h"           /* VisionX structure include file    */
#include "Vutil.h"            /* VisionX utility header files      */
VisXfile_t *VXin,             /* input file structure              */
           *VXout;            /* output file structure             */
VisXelem_t *VXlist;           /* VisionX data structure            */
VisXelem_t *VXptr;            /* VisionX pixel data pointer        */
VXparam_t par[] =             /* command line structure            */
{
{    "if=",    0,   " input file  vtemp: local max filter "},
{    "of=",    0,   " output file "},
{     0,       0,   0}  /* list termination */
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val

VisXimage_t im;                     /* i/o image structure          */
VisXimage_t tm;                     /* temp image structure         */
void setlabel(int i, int j, int n);

main(argc, argv)
int argc;
char *argv[];
{
int label=1;
int        i,j;                     /* index counters               */
  VXparse(&argc, &argv, par);       /* parse the command line       */
  VXin  = VXopen(IVAL, 0);          /* open input file              */
  VXout = VXopen(OVAL, 1);          /* open the output file         */
  VXlist = VXread(VXin);            /* read file                    */
  if (VXNIL != (VXptr = VXfind(VXlist, VX_PBYTE))){ /* find image */
     VXsetimage(&im, VXptr, VXin);  /* initialize input structure   */
     VXembedimage(&tm,&im,1,1,1,1); /* image structure with border  */


     for (i = im.ylo ; i <= im.yhi ; i++)   /* compute the function */
        for (j = im.xlo; j <= im.xhi; j++)
		im.u[i][j]=0;
     for (i = im.ylo ; i <= im.yhi ; i++)   /* compute the function */
        for (j = im.xlo; j <= im.xhi; j++){
	    if((im.u[i][j]==0)&&(tm.u[i][j]!=0))
		{setlabel(i, j, label);
			label++;}
}

     VXwrite(VXout, VXlist);     /* write data                   */
  }else{
     fprintf(stderr, "vtemp: no byte image data in input file\n");
     exit(-1);
  }
  VXclose(VXin);                  /* close files                  */
  VXclose(VXout);
  exit(0);
}
void setlabel(int x, int y, int n)
{
     im.u[x][y]=n;
     if((tm.u[x][y+1]!=0)&&(im.u[x][y+1]==0))
	setlabel(x,y+1,n);
   
     if((tm.u[x][y-1]!=0)&&(im.u[x][y-1]==0))
	setlabel(x,y-1,n);
          
     if((tm.u[x-1][y]!=0)&&(im.u[x-1][y]==0))
	setlabel(x-1,y,n);
     if((tm.u[x+1][y]!=0)&&(im.u[x+1][y]==0))
	setlabel(x+1,y,n);
}
