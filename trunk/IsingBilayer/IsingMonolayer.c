//2D Ising simulation of Lipid Membrane
//Waipot Ngamsaad (waipotn@yahoo.com)
//16 Nov. 2008

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "mygraph.h"

//Maximum Simulation Size
#define LMAX (256)
#define XMAX (LMAX)
#define YMAX (XMAX)
#define NMAX (XMAX*YMAX)
#define TEMPINIT (1)

//Maximum History Size
#define HMAX (100000)
//Initial simulation params
#define XDIMINIT (100)
#define YDIMINIT (100)
#define BORDINIT (5)
#define REPEATINIT (1)
//Maximum random number size
#define RANDMAX (2147483647)
#define PI (3.14159265358979323846264338327)
//Searchparams
#define VELMAX  (0.010)
#define VELMIN  (0.002)
#define VELMLT  (2.0)
#define VELSTP  (0.001)

#define TMAX  (0.10)
#define TMIN1 (0.01)
#define TSTP1 (0.01)
#define TMIN2 (0.001)
#define TSTP2 (0.001)

#ifndef Error
#define Error(Str) Fatal_error(Str)
#define Fatal_error(Str) fprintf(stderr,"%s\n",Str), exit(1)
#endif

#ifndef rnd
#define rnd(void) (double)rand()/(double)RANDMAX
#endif

//Main inputs
double Amp1 = 1E-4;
double a_ = -1., b_ = 1., kappa_  = 0.9, PARAM = 0.;
double T = TEMPINIT, Told=TEMPINIT, ratio = 0.5;
double tau0 = 1., tau1 = 1.;
double UX1 = 0.0, UY1 = 0.0;
double r_zero[2] = {0.,0.};

//Secondary inputs
int x_zero = 0;
int xdim = XMAX;
int ydim = YMAX;
int hdim = 1000;
int border = BORDINIT;

//Stability factors
double s = 0.1;

//Simulation book-keeping
int iterations = REPEATINIT;
int repeat = REPEATINIT;
int saverepeat = REPEATINIT;
int stabilize = 100000;
int iteration_max = 1000000;

//Flow controls
int graphics = 0;
int Pause = 1;
int singlestep = 0;
int done = 0;
int automate = 0;

//Set init
int setinit = 0;
int initvert = 10;

//Global Variables
double rho[XMAX][YMAX];
double phi[YMAX][YMAX];
double mu[XMAX][YMAX];
double w[17];

double timepassed[HMAX];
double totalmass[HMAX];
double domainwidth_vert[HMAX];
double domainwidth_vert_SD[HMAX];
double domainwidth_horiz[HMAX];
double domainwidth_horiz_SD[HMAX];

void init_spin(double spin[XMAX][YMAX]) {
  int i,j;
  //Set initial spin configuration
  for (i=0;i<xdim;i++) {
    for (j=0;j<ydim;j++) {
      if ( rnd()<=ratio )
	spin[i][j] = 1;
      else 
	spin[i][j] = -1;	 
    }
  }
}

void init(void) {
  int i, j, n, dE;

  char name[256];
  FILE *out;

  //Zero arrays
  for (i=0;i<xdim;i++) {
    for (j=0;j<ydim;j++) {  
      phi[i][j] = 0;
      rho[i][j] = 0;   
      mu[i][j] = 0.; 
    }
  }

  //init spin
  init_spin(phi);
  init_spin(rho);

  //Compute Boltzmann probability ratios
  for (dE = -8; dE <= 8; dE=dE+4)
    w[dE+8] = exp(-(double)dE/T);

  for (n=0;n<HMAX;n++) {
    timepassed[n] = 0.0;
    totalmass[n] = 0.0;
    domainwidth_vert[n] = 0.0;
    domainwidth_vert_SD[n] = 0.0;
    domainwidth_horiz[n] = 0.0;
    domainwidth_horiz_SD[n] = 0.0;
  }

  //Reset book-keeping
  if (repeat==0) 
    repeat = saverepeat;
  
  iterations = 0;
  //x_zero = 0;

  sprintf(name,"data-%f.txt", PARAM);
  out = fopen(name,"w+");

  if (out==(FILE *)NULL) Error("\n Error: Cannot open data-*.txt!!! \n");

  fclose(out);
}

void write_file() {
  int i,j;
  char name[256];

  FILE *out;

  sprintf(name,"image/image-%f-%08i.txt",PARAM,iterations);
  out = fopen(name,"w+");

  if (out==(FILE *)NULL) Error("\n Error: Cannot open ./image!!! \n");

  for (i=0;i<xdim;i++){
    for (j=0;j<ydim;j++){
      fprintf(out,"%f ", phi[i][j]);
    }
    fprintf(out,"\n");
  }
  fclose(out);
  
}

const char* wtime_string(double sec)
{
#define BUF_SIZE 64
  static char buf[BUF_SIZE + 1];

  if (sec/60 < 1.0)
    snprintf(buf, BUF_SIZE, "%f sec", sec);
  else if (sec/(60*60) < 1.0) {
    const double min = sec/60;
    snprintf(buf, BUF_SIZE, "%f min", min);
  } else {
    const double hours = sec/(60*60);
    snprintf(buf, BUF_SIZE, "%f hours", hours);
  }

  return buf;
}

void check_unstable(void) {
  int c = isnormal(rho[xdim/2][ydim/2])&&isnormal(phi[xdim/2][ydim/2]);
  if (!c) {
    printf("It's not stable\n");
    s -= TSTP1;
    if (s<TMIN1) {
      s += TSTP1;
      s -= TSTP2;
    }
    if (s<TMIN2) {
      printf("\"t\" cannot be lowered enough to maintain stability.\n");
      done = 1;
    }
    saverepeat = repeat;
    repeat = 0;
  }
}

void OnTempChange()
{
  if (T!=Told) 
    {
      Told = T;
      int dE;
      //re-compute Boltzmann probability ratios
      for (dE = -8; dE <= 8; dE=dE+4)
	w[dE+8] = exp(-(double)dE/T);
    }
}

int DeltaE(double spin[XMAX][YMAX],int x,int y,int L)
{
  int xm,xp,ym,yp,dE,left,right,up,down,center;

  //Periodic BC.
  xm  = (x+xdim-1)%xdim;
  xp  = (x+1)%xdim;
  ym  = (y+ydim-1)%ydim;
  yp  = (y+1)%ydim;

  center = spin[x][y];
  left = spin[xm][y];
  right = spin[xp][y];
  up = spin[x][yp];
  down = spin[x][ym];
    
  dE = 2*center*(left + right + up + down);

  return dE;
}

void Metropolis(double spin[XMAX][YMAX],int N,int L,double *E,double *M,double *accept)
{
  /*one Monte Carlo step per spin  */
  int ispin,x,y,dE;
  //Check If Temperature
  OnTempChange();
  for (ispin=1; ispin <= N; ispin++)
    {
      /* random x and y coordinates for trial spin  */
      x = L*rnd();
      y = L*rnd();
      dE = DeltaE(spin,x,y,L);
      if (rnd() <= w[dE+8])
	{
	  spin[x][y] = -spin[x][y];
	  *accept = *accept + 1;
	  *M = *M + 2*spin[x][y];
	  *E = *E + dE;
	}
    }
}

void iteration1(void){
  double E,M,accept;

  Metropolis(phi,NMAX,LMAX,&E,&M,&accept);
  check_unstable();
}

void analysis(void){
}

//Graphics
void GetGraphics(void){
}

void GUI(void){	
  DefineGraphNxN_R("phi", &phi[0][0], &xdim, &ydim, NULL);
  DefineGraphNxN_R("rho", &rho[0][0], &xdim, &ydim, NULL);
  //DefineGraphNxN_R("mu", &mu[0][0], &xdim, &ydim, NULL);
  NewGraph();

  StartMenu("Ising Bilayer",1);

  SetActiveGraph(0);
  DefineGraph(contour2d_,"Visualize");

  //StartMenu("Main Inputs",0);
  //DefineDouble("phi_zero",&r_zero[0]);
  //DefineDouble("rho_zero",&r_zero[1]);
  //DefineDouble("Amp",&Amp1);
  //DefineDouble("a",&a_);
  //DefineDouble("b",&b_);
  //DefineDouble("kappa",&kappa_);
  //DefineDouble("PARAM",&PARAM);
  //DefineDouble("tau0",&tau0);
  //DefineDouble("tau1",&tau1);
  //DefineDouble("UX_tp",&UX1);
  //DefineDouble("UY_tp",&UY1);
  //EndMenu();

  //StartMenu("Controls",0);
  DefineInt("iterations", &iterations);
  DefineInt("repeat", &repeat);
  //DefineInt("stabilize", &stabilize);
  DefineDouble("Ratio", &ratio);
  DefineDouble("Temperature", &T);
  //EndMenu();

  StartMenu("Secondary Inputs",0);
  DefineInt("xdim", &xdim);
  DefineInt("ydim", &ydim);
  EndMenu();

  //StartMenu("Stability Factors",0);
  //DefineDouble("s",&s);
  //EndMenu();

  //StartMenu("Set Initializations",0);
  //DefineBool("Use Set Init", &setinit);
  //DefineInt("Num Vert Domains", &initvert);
  //EndMenu();
     
  DefineFunction("reinitialize",&init);

  DefineBool("Pause",&Pause);
  DefineBool("Single Step",&singlestep);
  DefineBool("Done",&done);	
  EndMenu();
}

// Never change if not neccesary
int 
main(int argc, char *argv[]){
  int n, newdata = 0;

  time_t BEGIN_T;
  char name[256], hname[128];
  FILE *out;
 
  srand(time(NULL));

  //printf("%d\n", argc);
  if(argc < 1) {
    printf("Usage: %s [GUI 1 or 0]\n", argv[0]);
  }
  else {
    //PARAM = atof(argv[1]);
    
    if (argc == 2)
      graphics = atoi(argv[1]);
  }

  if (graphics)
    GUI();
  
  if (!graphics)
    printf("Running PARAM = %f on %dx%d Lattice with %d iterations\n",PARAM,XMAX,YMAX,iteration_max);

  if (!graphics) Pause = 0;
  
  init(); 
   //test for speed
  BEGIN_T = time(NULL);
  for (n=0;n<10;n++)
    iteration1();

  if (!graphics)
    printf("sec/iteration: %s\n", wtime_string( (double)(time(NULL)-BEGIN_T)/10) );

  init();

  //Just reset
  sprintf(name,"data-%f.txt",PARAM);
  out = fopen(name,"w+");

 //Write some information
  gethostname(hname,(size_t)sizeof(hname));

  fprintf(out,"#Size: %dx%d\n#K: %f\n#PARAM: %f\n#itertions: %d\n#Repeat: %d\n#sec/iteration: %s\n#Host: %s\n#PID: %d\n#---BEGIN DATA---\n\n\n",XMAX,YMAX,kappa_,PARAM,iteration_max,repeat,wtime_string( (double)(time(NULL)-BEGIN_T)/10 ), hname, getpid() );

  fclose(out);  

  //Now start simulation
  BEGIN_T = time(NULL);

  while ( (iterations<iteration_max)&&(!done) ){//Begin Main Loop
    if (graphics) {
      Events(newdata);
      GetGraphics();
      DrawGraphs();
    } else {
      //done=1;
      //Pause=0;
    }

    if (!Pause||singlestep) {
      singlestep = 0;
      newdata = 1;
      
      for (n=0;n<repeat;n++) {
	iterations++;
	iteration1();
      }
      //analysis();
      
      if (!graphics)
	repeat *= 1.1;

      if (!graphics)
	printf("ETA: %s\n", wtime_string( (double)(time(NULL)-BEGIN_T)*(iteration_max-iterations)/iterations) );

      if (repeat==0) 
	init();
    }
    else 
      sleep(1);

  }//End Main Loop

  return 0;
}
