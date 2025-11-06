#define F (1 << 14)
 
static inline itofp(int n);
static inline int fptoi_r(int x);
static inline int fptoi(int x);
static inline int addfp(int x, int n);
static inline int subfp(int x, int n);
static inline int mulfp(int x, int y);
static inline int divfp(int x, int y);

int itofp(int n){
  int temp= n * F;
  return temp;
}
 
int fptoi_r(int x){
  if(x>=0){
    return (x + F / 2 ) / F;
  }
  else{
    return (x - F / 2 ) / F;
  }
}
 
int fptoi(int x){
  return x/F;
}
 
int addfp(int x, int n){
  return x+itofp(n);
}

int subfp(int x, int n){
  return x-itofp(n);
}
 
int mulfp(int x, int y){
  return (((int64_t)x) * y / F);
}
 
int divfp(int x, int y){
  return ((int64_t) x) * F / y;
}