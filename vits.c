/*********************************************************************/
/* vtpeak:     Threshold image between two most sig. hgram peaks     */
/*********************************************************************/

#include "VisXV4.h"          /* VisionX structure include file       */
#include "Vutil.h"           /* VisionX utility header files         */
VisXfile_t *VXin,            /* input file structure                 */
           *VXout;           /* output file structure                */
VisXelem_t *VXlist,*VXptr;   /* VisionX data structure               */
VXparam_t par[] =            /* command line structure               */
{
{  "if=",   0,   " input file, vtpeak: threshold between hgram peaks"},
{  "of=",   0,   " output file "},
{  "d=",    0,   " min dist between hgram peaks (default 10)"},
{  "-v",    0,   "(verbose) print threshold information"},
{   0,      0,   0} /* list termination */
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  DVAL   par[2].val
#define  VFLAG  par[3].val

main(argc, argv)
int argc;
char *argv[];
{

VisXimage_t im;                    /* input image structure          */
int        i,j;                    /* index counters                 */

    int hist[256];                 /* histogram bins                 */
    int thresh;                    /* threshold                      */
    int maxbin;                    /* maximum histogram bin          */
    int nxtbin;                    /* second maximum bin             */
    int minbin;                    /* minumim histogram bin          */
    int maxa, maxb;       /* second maximum bin above/below maxbin   */
    int dist;                   /* minimum distance between maxima   */
			     
    VXparse(&argc, &argv, par);    /* parse the command line         */
    VXin  = VXopen(IVAL, 0);       /* open input file                */
    VXout = VXopen(OVAL, 1);       /* open the output file           */
    int iterative=1000; /* control the number of iterative */
    int avg1=0;
    int avg2=0;
    int sum1=0;
    int sum2=0;
    int count1=0;
    int count2=0;

/************ Parameter and initialization section *******************/

    dist = 10;                    /* default dist */
    if (DVAL) dist = atoi(DVAL);  /* if d= was specified, get value */
    if (dist < 0 || dist > 255) {
	fprintf(stderr, "d= must be between 0 and 255\nUsing d=10\n");
        dist = 10;
    }
/************ End of Parameter and initialization section ************/

    while((VXlist = VXptr = VXreadframe(VXin)) != VXNIL){ /* every frame */
        VXfupdate(VXout, VXin); /* update global constants */
	/* find next byte image */
        while (VXNIL != (VXptr = VXfind(VXptr, VX_PBYTE)))  { 
            VXsetimage(&im, VXptr, VXin); /* initialize input structure */

/***************** Application specific section **********************/

            /* clear the histogram */
            for (i = 0; i < 256; i++) hist[i] = 0;
 
            /* compute the histogram */
            for (i = im.ylo; i <= im.yhi; i++)
                for (j = im.xlo; j <= im.xhi; j++)
                    hist[im.u[i][j]]++;
  
            /* compute the threshold */
	    /*initialize the threshold with a random integer between 0 and 255*/
  	    thresh =10;
  	    
	    /*iterative threshold selection*/
	    for(i=0; i<iterative; i++) {
		sum1=0;
		sum2=0;
		count1=0;
		count2=0;
		for(j=0; j< thresh; j++){
		   sum1+=j*hist[j];
                   count1+=hist[j];
		}
		for(j=255; j>= thresh; j--){
		   sum2+=j*hist[j];
                   count2+=hist[j];
		}
		if(count1==0)
		   avg1=0;
                else
		   avg1=sum1/count1;
                if(count2==0)
                   avg2=0;
                else
                   avg2=sum2/count2;
                if(thresh==(avg1+avg2)/2)
		   break;
		else
                   thresh=(avg1+avg2)/2;
            }
  
            /* apply the threshold */
            for (i = im.ylo; i <= im.yhi; i++) {
                for (j = im.xlo; j <= im.xhi; j++) {
                    if (im.u[i][j] >= thresh) im.u[i][j] = 255;
                    else                      im.u[i][j] = 0;
                }
            }
 
/************** End of the Application specific section **************/

            VXresetimage(&im); /* free the im image structure  */
            VXptr = VXptr->next; /* move to the next image */
        } /* end of every image section */
        VXwriteframe(VXout,VXlist); /* write frame */
        VXdellist(VXlist);          /* delete the frame */
    } /* end of every frame section */
    VXclose(VXin);  /* close files */
    VXclose(VXout);
    exit(0);
}
