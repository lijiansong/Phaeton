// ----- Autogen kernel by Phaeton -----

void omp_func(
float *a,
float *b,
float *c,
float *d)
{
  float t0;
  for (unsigned i0 = 0; i0 < 4; ++i0) {
    for (unsigned i1 = 0; i1 < 5; ++i1) {
      for (unsigned i2 = 0; i2 < 6; ++i2) {
        for (unsigned i3 = 0; i3 < 4; ++i3) {
          for (unsigned i4 = 0; i4 < 5; ++i4) {
            for (unsigned i5 = 0; i5 < 6; ++i5) {
              t0 = a[(i2 + 6*(i1 + 5*(i0)))] * b[(i5 + 6*(i4 + 5*(i3)))];
              for (unsigned i6 = 0; i6 < 1; ++i6) {
                for (unsigned i7 = 0; i7 < 2; ++i7) {
                  for (unsigned i8 = 0; i8 < 3; ++i8) {
                    for (unsigned i9 = 0; i9 < 4; ++i9) {
                      d[(i9 + 4*(i8 + 3*(i7 + 2*(i6 + 1*(i5 + 6*(i4 + 5*(i3 + 4*(i2 + 6*(i1 + 5*(i0))))))))))] = t0 * c[(i9 + 4*(i8 + 3*(i7 + 2*(i6))))];
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
