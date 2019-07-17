void C_func(
float *a,
float *b,
float *c)
{
  float d[1];
  d[(] = a[(] + b[(];
  float d[1];
  d[(] = a[(] - b[(];
  float d[1];
  d[(] = a[(] * b[(];
  float d[1];
  d[(] = a[(] / b[(];
  float d[1];
  float t0;
  float t1;
  float t2;
  t2 = a[(] + b[(];
  float t3;
  t3 = c[(] - d[(];
  t1 = t2 * t3;
  t0 = t1 + a[(];
  float t4;
  t4 = c[(] * d[(];
  d[(] = t0 + t4;
}
