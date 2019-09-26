void omp_func(
float a[3][4],
float b[4][3],
float c[3][3])
{
  for (unsigned i0 = 0; i0 < 3; ++i0) {
    for (unsigned i1 = 0; i1 < 3; ++i1) {
      float t1 = 0.0;
      for (unsigned i2_contr = 0; i2_contr < 4; ++i2_contr) {
        t1 += a[i0][i2_contr] * b[i2_contr][i1];
      }
      c[i0][i1] = t1;
    }
  }
}
