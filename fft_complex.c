#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>



typedef float real;
typedef struct{real Re; real Im;} complex;



double *get_data_one_column(char* fName, int *N){

   FILE   *f;
   char    linebuf[1024];
   double *data;
   int     i, c, n_lines, got_data;


   f = fopen(fName, "r");
   if( !f ) {
       fprintf(stderr, "Error, couldn't open file %s\n", fName);
       return (NULL);
   }

   n_lines = 0;
   while( fgets(linebuf, 1024, f) ){
        n_lines++;
   }
   rewind(f);
   data = (double *)malloc( n_lines * sizeof(double) );

   c = 0;
   for( i = 0; i < n_lines; i++ ){
       if( !fgets(linebuf, 1024, f) ){
            return(NULL);
       }
       got_data = sscanf(linebuf, "%lf", &data[c++]);
       if( got_data == 0 ){
          c--;
       }
   }
   printf("read %i floats\n", c);
  *N = c;
   return( data );
}



complex *fft( complex *f, int N, complex *tmp )
{
  if(N>1)
	{
		complex multi, w, *fo, *fe, wn, *Fo, *Fe, *F;
		double  temp;


    F  = calloc(N,   sizeof(complex));
    fo = calloc(N/2, sizeof(complex));
    fe = calloc(N/2, sizeof(complex));

		for(int i=0; i<N/2; i++)
		{
      fe[i].Re = f[2*i].Re;
      fe[i].Im = f[2*i].Im;
      fo[i].Re = f[2*i+1].Re;
      fo[i].Im = f[2*i+1].Im;
    }
    Fe = fft( fe, N/2, NULL );
    Fo = fft( fo, N/2, NULL );
		wn.Re =  cos(2*M_PI/(double)N);
		wn.Im = -sin(2*M_PI/(double)N);
		w.Re = 1;
		w.Im = 0;
    for(int j=0; j<N/2; j++)
		{
			/* Re(w*fo[j]) */
			multi.Re = w.Re*Fo[j].Re - w.Im*Fo[j].Im;
			/* Im(w*fo[j]) */
      multi.Im = w.Re*Fo[j].Im + w.Im*Fo[j].Re;

      F[j].Re = Fe[j].Re + multi.Re;
      F[j].Im = Fe[j].Im + multi.Im;
      F[j+N/2].Re = Fe[j].Re - multi.Re;
      F[j+N/2].Im = Fe[j].Im - multi.Im;

			temp = w.Re;
	    w.Re = w.Re * wn.Re - w.Im * wn.Im;
	    w.Im = temp * wn.Im + wn.Re * w.Im;
    }
      if( N >2 ){
        free( Fe );
        free( Fo );
      }
        free( fe );
        free( fo );

    return F;
  }else{
    return f;
  }
}

int main(int argc, char** argv)
{
  int N;
  int k;
  double *f;
  FILE    *ofile;
  complex *F, *ff, *tmp;

  if( argc >= 2 ){
    f  = get_data_one_column(argv[1], &N);
  }else{
    fprintf(stderr, "require single-column infile\n");
    exit(1);
  }

  //F   = calloc(N, sizeof(complex));
  ff  = calloc(N, sizeof(complex));
  //tmp = calloc(N, sizeof(complex));


  for (int j = 0; j<N; j++)
  {
    ff[j].Re = f[j];
    ff[j].Im = 0.;
  }

  F = fft( ff, N, tmp );


  ofile = fopen("complex-output.dat", "w");
  for (int k = 0; k<N; k++)
  {
    fprintf(ofile, "%.08f  %.08f\n", F[k].Re, F[k].Im);
  }
  fclose(ofile);


  exit(EXIT_SUCCESS);
}
