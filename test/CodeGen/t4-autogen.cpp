// ----- Autogen kernel by Phaeton -----

void omp_func(
float *a,
float *b,
float *c,
float *d)
{
  d[0] = a[0] + b[0];
  d[0] = a[0] - b[0];
  d[0] = a[0] * b[0];
  d[0] = a[0] / b[0];
  float t0;
  float t1;
  float t2;
  t2 = a[0] + b[0];
  float t3;
  t3 = c[0] - d[0];
  t1 = t2 * t3;
  t0 = t1 + a[0];
  float t4;
  t4 = c[0] * d[0];
  d[0] = t0 + t4;
}
