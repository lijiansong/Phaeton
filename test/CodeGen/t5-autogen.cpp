// ----- Autogen kernel by Phaeton -----

void omp_func(
float *a,
float *b)
{
  float c[9];
  for (unsigned i0 = 0; i0 < 3; ++i0) {
    for (unsigned i1 = 0; i1 < 3; ++i1) {
      c[(i1 + 3*(i0))] = 0.0;
      for (unsigned i2_contr = 0; i2_contr < 4; ++i2_contr) {
        c[(i1 + 3*(i0))] += a[(i2_contr + 4*(i0))] * b[(i1 + 3*(i2_contr))];
      }
    }
  }
}
