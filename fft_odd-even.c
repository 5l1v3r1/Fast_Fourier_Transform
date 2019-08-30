#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

double *get_data_one_column(char* fName, int *N)
{
  FILE *f;
  char linebuf[1024];
  double *data;
  int c, n_lines, got_data;

  f = fopen(fName, "r");
  if (!f)
  {
    fprintf(stderr, "%s %s\n", "Error, couldn't open file", fName );
    return (NULL);
  }

  n_lines = 0;
  while (fgets(linebuf, 1024, f))
  {
    n_lines++;
  }
  rewind(f);
  data = (double *)malloc(n_lines * sizeof(double));

  c = 0;
  for (int i = 0; i < n_lines; i++)
  {
    if (!fgets(linebuf, 1024, f)){return (NULL);}
    got_data = sscanf(linebuf, "%lf", &data[c++]);
    if (got_data == 0){c--;}
  }
  printf("read %i floats\n", c);
  *N = c;
  return data;
}




double * fourier(double *f, double *F, int N, double *temp)
{
  double *f_Re, *f_Im, *fe, *fo, *FE, *FO;
  double wn_Re, wn_Im, w_Re, w_Im, multi_Re, multi_Im;
  double tmp;
  fe = temp;
  fo = temp;
  FO = temp;
  FE = temp;



  if (N == 1) {return f;}




  for (int i = 0; i<N/2; i++)
  {
    // Real part
    fe[i] = f[2*i];
    fo[i] = f[2*i];
    // Imaginary part
    fe[i+1] = f[2*i + 1];
    fo[i+1] = f[2*i + 1];
  }

  FE = fourier(fe, F, N/2, temp);
  FO = fourier(fo, F, N/2, temp);

  wn_Re = cos(-2*M_PI / N);
  wn_Im = sin(-2*M_PI / N);

  w_Re = 1;
  w_Im = 0;
  for (int j = 0; j < N/2; j+= 2)
  {

    /* Re(w*fo[j]) */
    multi_Re = w_Re*fo[j] - w_Im*fo[j+1];
    /* Im(w*fo[j]) */
    multi_Im = w_Re*fo[j] + w_Im*fo[j+1];


    f_Re[j/2] = fe[j/2] + multi_Re;
    f_Im[j/2] = fe[j+1] + multi_Im;
    f_Re[j/2 +N/4] = fe[j] - multi_Re;
    f_Im[j/2 +N/4] = fe[j + 1] - multi_Im;


    tmp = w_Re;
    w_Re = w_Re * wn_Re - w_Im * wn_Im;
    w_Im = tmp * wn_Im + wn_Re * w_Im;

  }

  for(int k = 0; k < 2*N; k+=2)
  {
    F[k] = f_Re[k];
		F[k+1] = f_Im[k];
  }

  return f;

}

int main(int argc, char** argv)
{
  int N, i, k;
  double *x, *f, w, *F, *F1, *temp;
  double *fe, *fo, *FE, *FO;
  FILE *ofile;

  if (argc >=2)
  {
    f = get_data_one_column(argv[1], &N);
  }
  else
  {
    /*fprinf(stderr, "require single-column infile\n");*/
    exit(1);
  }


  temp = (double *)malloc(N * sizeof(double));
  F = (double *)malloc(2*N * sizeof(double));
  F1 = (double *)malloc(2*N * sizeof(double));
  F1 = fourier(f, F, N, temp);




  ofile = fopen("Output_ref.dat", "w");
  for (int k = 0; k<N-1; k++)
  {
    fprintf(ofile, "%.08f  %.08f\n", F1[k], F1[k+1]);
  }
  fclose(ofile);

  return(EXIT_SUCCESS);
}
