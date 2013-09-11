#ifndef LINT_H
#define LINT_H

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef struct LIntS {
    int fd;
    int vector;
  } LInt;
  
  LInt* newLInt(int vector);
  void delLInt(LInt *my);
  int LInt_wait(LInt *my);
  int LInt_enable(LInt *my, int level);
  
#ifdef __cplusplus
}
#endif

#endif
