// ----- Autogen kernel by Phaeton -----

void omp_func(
float *a,
float *b)
{
  for (unsigned i0 = 0; i0 < 3; ++i0) {
    for (unsigned i1 = 0; i1 < 3; ++i1) {
      b[(i1 + 3*(i0))] = a[(i1 + 3*(i0))] + b[(i1 + 3*(i0))];
    }
  }
  for (unsigned i2 = 0; i2 < 3; ++i2) {
    for (unsigned i3 = 0; i3 < 3; ++i3) {
      b[(i3 + 3*(i2))] = a[(i3 + 3*(i2))] - b[(i3 + 3*(i2))];
    }
  }
  for (unsigned i4 = 0; i4 < 3; ++i4) {
    for (unsigned i5 = 0; i5 < 3; ++i5) {
      b[(i5 + 3*(i4))] = a[(i5 + 3*(i4))] * b[(i5 + 3*(i4))];
    }
  }
  for (unsigned i6 = 0; i6 < 3; ++i6) {
    for (unsigned i7 = 0; i7 < 3; ++i7) {
      b[(i7 + 3*(i6))] = a[(i7 + 3*(i6))] / b[(i7 + 3*(i6))];
    }
  }
  float t0;
  float t1;
  float t2;
  float t3;
  for (unsigned i8 = 0; i8 < 3; ++i8) {
    for (unsigned i9 = 0; i9 < 3; ++i9) {
      t3 = b[(i9 + 3*(i8))] + a[(i9 + 3*(i8))];
      t2 = t3 + a[(i9 + 3*(i8))];
      t1 = a[(i9 + 3*(i8))] + t2;
      float t4;
      float t5;
      t5 = a[(i9 + 3*(i8))] * b[(i9 + 3*(i8))];
      t4 = a[(i9 + 3*(i8))] - t5;
      t0 = t1 * t4;
      b[(i9 + 3*(i8))] = t0 * b[(i9 + 3*(i8))];
    }
  }
}
