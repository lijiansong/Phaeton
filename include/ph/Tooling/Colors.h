//===--- Colors.h - Colors for text output at Linux terminal ----*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines some common color macro for terminal text output.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_TOOLING_COLORS_H
#define PHAETON_TOOLING_COLORS_H

// More details, checkout this page:
// https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal

// Foreground
#define RST "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST

#endif // PHAETON_TOOLING_COLORS_H
