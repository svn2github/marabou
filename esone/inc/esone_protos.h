extern "C" int ccopen(char*, void*);
extern "C" int cdreg(void*, long, int, int, int);
extern "C" int cdctrl(int);
extern "C" int cfsa(int, long, int*, int*);
extern "C" int cssa(int, long, int*, int*);
extern "C" int ctstat(int*);
extern "C" char *cerror(int, int);
extern "C" int cccz(long);
extern "C" int cccc(long);
extern "C" int ccci(long, int);
extern "C" int ctci(long, int*);
extern "C" int cccd(long, int);
extern "C" int ctcd(long, int*);
extern "C" int ctgl(long, int*);
extern "C" int cgreg(long, void*, int*, int*, int*);
extern "C" int cfga(int*, void*, int*, int*, void*);
extern "C" int csga(int*, void*, int*, int*, void*);
extern "C" int cfmad(int, void*, int*, void*);
extern "C" int csmad(int, void*, int*, void*);
extern "C" int cfubc(int, long, int*, void*);
extern "C" int csubc(int, long, int*, void*);
extern "C" int cfubr(int, long, int*, void*);
extern "C" int csubr(int, long, int*, void*);
