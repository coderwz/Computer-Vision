/******************************************************************************/
/* vssum:     Compute mean of image frames in a sequence                      */
/******************************************************************************/
#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */
VXparam_t par[] =  
{
{ "if=",    0, " input file  vssum: compute temporal iamge running average"},
{ "of=",    0,   " output file  "},
{ "n=",     0,   " number of frames "},
{ "th=",    0,   0},
{  0,       0,   0}
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  NVAL   par[2].val
#define  MVAL   par[3].val

VisXfile_t *VXin,            /* input file structure            */
           *VXout;           /* output file structure           */
VisXelem_t *VXlist,*VXpt;    /* VisX data structure             */

main(argc, argv)
int argc;
char *argv[];
{
VisXimage_t *ib;                   /* multiframe image buffer                 */
VisXimage_t tm;                    /* temp image  structure                   */
int         i,j,k;                 /* index counters                          */
int         n;                     /* number of frames to average             */
int         told;                  /* temp integer                            */
int         first;
int         threshold;

    VXparse(&argc, &argv, par);    /* parse the command line                  */
    VXin  = VXopen(IVAL, 0);       /* open input file                         */
    VXout = VXopen(OVAL, 1);       /* open the output file                    */
    n = (NVAL ? atoi(NVAL) : 2);   /* read n, default is n=1                  */
    ib = VXsbufimage(VXin, n);     /* allocate buffer and read n-1 frames     */
    threshold = atoi(MVAL);

    first = 1;
    while(VXrbufimage(ib, VXin, n)) /* every frame */
    {
        VXfupdate(VXout, VXin);     /* update global constants */
/********************* Application specific section ***************************/

        if (first) /* initialize buffer */
        { 
            /* make temp int image */
            VXmakeimage(&tm, VX_PINT, ib[0].bbx, ib[0].chan); 
            first = 0;
        }
        for (i = tm.ylo; i <= tm.yhi; i++)
        {
            for (j = tm.xlo; j <= tm.xhi; j++)
            {
		if(abs(ib[0].u[i][j]-ib[1].u[i][j])<threshold)
			tm.i[i][j]=128;
		else
  			tm.i[i][j]=255;
                ib[n-1].u[i][j] = tm.i[i][j]; /* update oldest frame      */
            }
        }
        VXwriteframe(VXout,ib[n-1].list);     /* write oldest frame           */

/******************* End of the Application specific section ******************/
    }                                         /* end of every frame section   */
    if (first)
    {
        fprintf(stderr,"vssum: not enough frames in image set\n");
        exit(1);
    }
    VXclose(VXin);                            /* close files                  */
    VXclose(VXout);
    exit(0);
}
