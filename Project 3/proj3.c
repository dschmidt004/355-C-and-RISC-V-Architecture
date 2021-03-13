#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int shift(int val, int scale) {
  return (val << scale);
}

void matrix_opr(int *mat, int *out) {
  
  int i, j;
  
  for (i=0;i<4;i++) {
    
    out[i] = 0;
    
    for (j=0; j<4; j++) {
    
      out[i] += shift(mat[i*4+j], i);
    }
  }
}

void main(void) {
  int mat[15], out[4];
  int k;
  
  for (k=0;k<16;k++)
    mat[k] = k;
  
  matrix_opr(mat, out);
   
  // fprintf(stdout, "");
  fprintf(stdout, "contents of mat[]:\n");
  for (k=0;k<16;k++)
    fprintf(stdout, "mat[%d] = %d\n", k, mat[k]);
    
  fprintf(stdout, "\ncontents of out[]:\n");
  for (k=0;k<4;k++)
    fprintf(stdout, "out[%d] = %d\n", k, out[k]);
  
}