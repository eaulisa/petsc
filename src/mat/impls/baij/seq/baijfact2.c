/*$Id: baijfact2.c,v 1.32 1999/11/10 03:19:27 bsmith Exp bsmith $*/
/*
    Factorization code for BAIJ format. 
*/

#include "src/mat/impls/baij/seq/baij.h"
#include "src/vec/vecimpl.h"
#include "src/inline/ilu.h"
#include "src/inline/dot.h"

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_1_NaturalOrdering"
int MatSolveTranspose_SeqBAIJ_1_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  int             ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz;
  int             *diag = a->diag;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,*x,*b;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);

  /* forward solve the U^T */
  for ( i=0; i<n; i++ ) {

    v     = aa + diag[i];
    /* multiply by the inverse of the block diagonal */
    s1    = (*v++)*b[i];
    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      x[*vi++]  -= (*v++)*s1;
    }
    x[i]   = s1;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + diag[i] - 1;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    s1   = x[i];
    while (nz--) {
      x[*vi--]   -=  (*v--)*s1;
    }
  }
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*(a->nz) - a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_2_NaturalOrdering"
int MatSolveTranspose_SeqBAIJ_2_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  int             ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt;
  int             *diag = a->diag,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,x1,x2;
  Scalar          *x,*b;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 4*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1 = b[idx];   x2 = b[1+idx];
    s1 = v[0]*x1  +  v[1]*x2;
    s2 = v[2]*x1  +  v[3]*x2;
    v += 4;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 2*(*vi++);
      x[oidx]   -= v[0]*s1  +  v[1]*s2;
      x[oidx+1] -= v[2]*s1  +  v[3]*s2;
      v  += 4;
    }
    x[idx]   = s1;x[1+idx] = s2;
    idx += 2;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 4*diag[i] - 4;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 2*i;
    s1   = x[idt];  s2 = x[1+idt];
    while (nz--) {
      idx   = 2*(*vi--);
      x[idx]   -=  v[0]*s1 +  v[1]*s2;
      x[idx+1] -=  v[2]*s1 +  v[3]*s2;
      v -= 4;
    }
  }
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*4*(a->nz) - 2*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_3_NaturalOrdering"
int MatSolveTranspose_SeqBAIJ_3_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  int             ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt;
  int             *diag = a->diag,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,s3,x1,x2,x3;
  Scalar          *x,*b;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 9*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1 = b[idx];   x2 = b[1+idx]; x3    = b[2+idx];
    s1 = v[0]*x1  +  v[1]*x2 +  v[2]*x3;
    s2 = v[3]*x1  +  v[4]*x2 +  v[5]*x3;
    s3 = v[6]*x1  +  v[7]*x2 + v[8]*x3;
    v += 9;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 3*(*vi++);
      x[oidx]   -= v[0]*s1  +  v[1]*s2 +  v[2]*s3;
      x[oidx+1] -= v[3]*s1  +  v[4]*s2 +  v[5]*s3;
      x[oidx+2] -= v[6]*s1 + v[7]*s2 + v[8]*s3;
      v  += 9;
    }
    x[idx]   = s1;x[1+idx] = s2; x[2+idx] = s3;
    idx += 3;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 9*diag[i] - 9;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 3*i;
    s1 = x[idt];  s2 = x[1+idt]; s3 = x[2+idt];
    while (nz--) {
      idx   = 3*(*vi--);
      x[idx]   -=  v[0]*s1 +  v[1]*s2 +  v[2]*s3;
      x[idx+1] -=  v[3]*s1 +  v[4]*s2 +  v[5]*s3;
      x[idx+2] -= v[6]*s1 + v[7]*s2 + v[8]*s3;
      v -= 9;
    }
  }
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*9*(a->nz) - 3*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_4_NaturalOrdering"
int MatSolveTranspose_SeqBAIJ_4_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  int             ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt;
  int             *diag = a->diag,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,s3,s4,x1,x2,x3,x4;
  Scalar          *x,*b;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 16*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1    = b[idx];   x2 = b[1+idx]; x3    = b[2+idx]; x4 = b[3+idx];
    s1 = v[0]*x1  +  v[1]*x2 +  v[2]*x3 +  v[3]*x4;
    s2 = v[4]*x1  +  v[5]*x2 +  v[6]*x3 +  v[7]*x4;
    s3 = v[8]*x1  +  v[9]*x2 + v[10]*x3 + v[11]*x4;
    s4 = v[12]*x1 + v[13]*x2 + v[14]*x3 + v[15]*x4;
    v += 16;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 4*(*vi++);
      x[oidx]   -= v[0]*s1  +  v[1]*s2 +  v[2]*s3 +  v[3]*s4;
      x[oidx+1] -= v[4]*s1  +  v[5]*s2 +  v[6]*s3 +  v[7]*s4;
      x[oidx+2] -= v[8]*s1 + v[9]*s2 + v[10]*s3 + v[11]*s4;
      x[oidx+3] -= v[12]*s1 + v[13]*s2 + v[14]*s3 + v[15]*s4;
      v  += 16;
    }
    x[idx]   = s1;x[1+idx] = s2; x[2+idx] = s3;x[3+idx] = s4;
    idx += 4;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 16*diag[i] - 16;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 4*i;
    s1 = x[idt];  s2 = x[1+idt]; s3 = x[2+idt];s4 = x[3+idt];
    while (nz--) {
      idx   = 4*(*vi--);
      x[idx]   -=  v[0]*s1 +  v[1]*s2 +  v[2]*s3 +  v[3]*s4;
      x[idx+1] -=  v[4]*s1 +  v[5]*s2 +  v[6]*s3 +  v[7]*s4;
      x[idx+2] -= v[8]*s1 + v[9]*s2 + v[10]*s3 + v[11]*s4;
      x[idx+3] -= v[12]*s1 + v[13]*s2 + v[14]*s3 + v[15]*s4;
      v -= 16;
    }
  }
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*16*(a->nz) - 4*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_5_NaturalOrdering"
int MatSolveTranspose_SeqBAIJ_5_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  int             ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt;
  int             *diag = a->diag,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,s3,s4,s5,x1,x2,x3,x4,x5;
  Scalar          *x,*b;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 25*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1    = b[idx];   x2 = b[1+idx]; x3    = b[2+idx]; x4 = b[3+idx]; x5 = b[4+idx];
    s1 = v[0]*x1  +  v[1]*x2 +  v[2]*x3 +  v[3]*x4 +  v[4]*x5;
    s2 = v[5]*x1  +  v[6]*x2 +  v[7]*x3 +  v[8]*x4 +  v[9]*x5;
    s3 = v[10]*x1 + v[11]*x2 + v[12]*x3 + v[13]*x4 + v[14]*x5;
    s4 = v[15]*x1 + v[16]*x2 + v[17]*x3 + v[18]*x4 + v[19]*x5;
    s5 = v[20]*x1 + v[21]*x2 + v[22]*x3 + v[23]*x4 + v[24]*x5;
    v += 25;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 5*(*vi++);
      x[oidx]   -= v[0]*s1  +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5;
      x[oidx+1] -= v[5]*s1  +  v[6]*s2 +  v[7]*s3 +  v[8]*s4 +  v[9]*s5;
      x[oidx+2] -= v[10]*s1 + v[11]*s2 + v[12]*s3 + v[13]*s4 + v[14]*s5;
      x[oidx+3] -= v[15]*s1 + v[16]*s2 + v[17]*s3 + v[18]*s4 + v[19]*s5;
      x[oidx+4] -= v[20]*s1 + v[21]*s2 + v[22]*s3 + v[23]*s4 + v[24]*s5;
      v  += 25;
    }
    x[idx]   = s1;x[1+idx] = s2; x[2+idx] = s3;x[3+idx] = s4; x[4+idx] = s5;
    idx += 5;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 25*diag[i] - 25;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 5*i;
    s1 = x[idt];  s2 = x[1+idt]; s3 = x[2+idt];s4 = x[3+idt]; s5 = x[4+idt];
    while (nz--) {
      idx   = 5*(*vi--);
      x[idx]   -=  v[0]*s1 +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5;
      x[idx+1] -=  v[5]*s1 +  v[6]*s2 +  v[7]*s3 +  v[8]*s4 +  v[9]*s5;
      x[idx+2] -= v[10]*s1 + v[11]*s2 + v[12]*s3 + v[13]*s4 + v[14]*s5;
      x[idx+3] -= v[15]*s1 + v[16]*s2 + v[17]*s3 + v[18]*s4 + v[19]*s5;
      x[idx+4] -= v[20]*s1 + v[21]*s2 + v[22]*s3 + v[23]*s4 + v[24]*s5;
      v -= 25;
    }
  }
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*25*(a->nz) - 5*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_6_NaturalOrdering"
int MatSolveTranspose_SeqBAIJ_6_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  int             ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt;
  int             *diag = a->diag,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,s3,s4,s5,s6,x1,x2,x3,x4,x5,x6;
  Scalar          *x,*b;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 36*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1    = b[idx];   x2 = b[1+idx]; x3    = b[2+idx]; x4 = b[3+idx]; x5 = b[4+idx];
    x6    = b[5+idx]; 
    s1 = v[0]*x1  +  v[1]*x2 +  v[2]*x3 +  v[3]*x4 +  v[4]*x5 +  v[5]*x6;
    s2 = v[6]*x1  +  v[7]*x2 +  v[8]*x3 +  v[9]*x4 + v[10]*x5 + v[11]*x6;
    s3 = v[12]*x1 + v[13]*x2 + v[14]*x3 + v[15]*x4 + v[16]*x5 + v[17]*x6;
    s4 = v[18]*x1 + v[19]*x2 + v[20]*x3 + v[21]*x4 + v[22]*x5 + v[23]*x6;
    s5 = v[24]*x1 + v[25]*x2 + v[26]*x3 + v[27]*x4 + v[28]*x5 + v[29]*x6;
    s6 = v[30]*x1 + v[31]*x2 + v[32]*x3 + v[33]*x4 + v[34]*x5 + v[35]*x6;
    v += 36;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 6*(*vi++);
      x[oidx]   -= v[0]*s1  +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5 +  v[5]*s6;
      x[oidx+1] -= v[6]*s1  +  v[7]*s2 +  v[8]*s3 +  v[9]*s4 + v[10]*s5 + v[11]*s6;
      x[oidx+2] -= v[12]*s1 + v[13]*s2 + v[14]*s3 + v[15]*s4 + v[16]*s5 + v[17]*s6;
      x[oidx+3] -= v[18]*s1 + v[19]*s2 + v[20]*s3 + v[21]*s4 + v[22]*s5 + v[23]*s6;
      x[oidx+4] -= v[24]*s1 + v[25]*s2 + v[26]*s3 + v[27]*s4 + v[28]*s5 + v[29]*s6;
      x[oidx+5] -= v[30]*s1 + v[31]*s2 + v[32]*s3 + v[33]*s4 + v[34]*s5 + v[35]*s6;
      v  += 36;
    }
    x[idx]   = s1;x[1+idx] = s2; x[2+idx] = s3;x[3+idx] = s4; x[4+idx] = s5;
    x[5+idx] = s6;
    idx += 6;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 36*diag[i] - 36;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 6*i;
    s1 = x[idt];  s2 = x[1+idt]; s3 = x[2+idt];s4 = x[3+idt]; s5 = x[4+idt];
    s6 = x[5+idt];
    while (nz--) {
      idx   = 6*(*vi--);
      x[idx]   -=  v[0]*s1 +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5 +  v[5]*s6;
      x[idx+1] -=  v[6]*s1 +  v[7]*s2 +  v[8]*s3 +  v[9]*s4 + v[10]*s5 + v[11]*s6;
      x[idx+2] -= v[12]*s1 + v[13]*s2 + v[14]*s3 + v[15]*s4 + v[16]*s5 + v[17]*s6;
      x[idx+3] -= v[18]*s1 + v[19]*s2 + v[20]*s3 + v[21]*s4 + v[22]*s5 + v[23]*s6;
      x[idx+4] -= v[24]*s1 + v[25]*s2 + v[26]*s3 + v[27]*s4 + v[28]*s5 + v[29]*s6;
      x[idx+5] -= v[30]*s1 + v[31]*s2 + v[32]*s3 + v[33]*s4 + v[34]*s5 + v[35]*s6;
      v -= 36;
    }
  }
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*36*(a->nz) - 6*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_7_NaturalOrdering"
int MatSolveTranspose_SeqBAIJ_7_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  int             ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt;
  int             *diag = a->diag,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,s3,s4,s5,s6,s7,x1,x2,x3,x4,x5,x6,x7;
  Scalar          *x,*b;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 49*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1    = b[idx];   x2 = b[1+idx]; x3    = b[2+idx]; x4 = b[3+idx]; x5 = b[4+idx];
    x6    = b[5+idx]; x7 = b[6+idx];
    s1 = v[0]*x1  +  v[1]*x2 +  v[2]*x3 +  v[3]*x4 +  v[4]*x5 +  v[5]*x6 +  v[6]*x7;
    s2 = v[7]*x1  +  v[8]*x2 +  v[9]*x3 + v[10]*x4 + v[11]*x5 + v[12]*x6 + v[13]*x7;
    s3 = v[14]*x1 + v[15]*x2 + v[16]*x3 + v[17]*x4 + v[18]*x5 + v[19]*x6 + v[20]*x7;
    s4 = v[21]*x1 + v[22]*x2 + v[23]*x3 + v[24]*x4 + v[25]*x5 + v[26]*x6 + v[27]*x7;
    s5 = v[28]*x1 + v[29]*x2 + v[30]*x3 + v[31]*x4 + v[32]*x5 + v[33]*x6 + v[34]*x7;
    s6 = v[35]*x1 + v[36]*x2 + v[37]*x3 + v[38]*x4 + v[39]*x5 + v[40]*x6 + v[41]*x7;
    s7 = v[42]*x1 + v[43]*x2 + v[44]*x3 + v[45]*x4 + v[46]*x5 + v[47]*x6 + v[48]*x7;
    v += 49;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 7*(*vi++);
      x[oidx]   -= v[0]*s1  +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5 +  v[5]*s6 +  v[6]*s7;
      x[oidx+1] -= v[7]*s1  +  v[8]*s2 +  v[9]*s3 + v[10]*s4 + v[11]*s5 + v[12]*s6 + v[13]*s7;
      x[oidx+2] -= v[14]*s1 + v[15]*s2 + v[16]*s3 + v[17]*s4 + v[18]*s5 + v[19]*s6 + v[20]*s7;
      x[oidx+3] -= v[21]*s1 + v[22]*s2 + v[23]*s3 + v[24]*s4 + v[25]*s5 + v[26]*s6 + v[27]*s7;
      x[oidx+4] -= v[28]*s1 + v[29]*s2 + v[30]*s3 + v[31]*s4 + v[32]*s5 + v[33]*s6 + v[34]*s7;
      x[oidx+5] -= v[35]*s1 + v[36]*s2 + v[37]*s3 + v[38]*s4 + v[39]*s5 + v[40]*s6 + v[41]*s7;
      x[oidx+6] -= v[42]*s1 + v[43]*s2 + v[44]*s3 + v[45]*s4 + v[46]*s5 + v[47]*s6 + v[48]*s7;
      v  += 49;
    }
    x[idx]   = s1;x[1+idx] = s2; x[2+idx] = s3;x[3+idx] = s4; x[4+idx] = s5;
    x[5+idx] = s6;x[6+idx] = s7;
    idx += 7;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 49*diag[i] - 49;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 7*i;
    s1 = x[idt];  s2 = x[1+idt]; s3 = x[2+idt];s4 = x[3+idt]; s5 = x[4+idt];
    s6 = x[5+idt];s7 = x[6+idt]; 
    while (nz--) {
      idx   = 7*(*vi--);
      x[idx]   -=  v[0]*s1 +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5 +  v[5]*s6 +  v[6]*s7;
      x[idx+1] -=  v[7]*s1 +  v[8]*s2 +  v[9]*s3 + v[10]*s4 + v[11]*s5 + v[12]*s6 + v[13]*s7;
      x[idx+2] -= v[14]*s1 + v[15]*s2 + v[16]*s3 + v[17]*s4 + v[18]*s5 + v[19]*s6 + v[20]*s7;
      x[idx+3] -= v[21]*s1 + v[22]*s2 + v[23]*s3 + v[24]*s4 + v[25]*s5 + v[26]*s6 + v[27]*s7;
      x[idx+4] -= v[28]*s1 + v[29]*s2 + v[30]*s3 + v[31]*s4 + v[32]*s5 + v[33]*s6 + v[34]*s7;
      x[idx+5] -= v[35]*s1 + v[36]*s2 + v[37]*s3 + v[38]*s4 + v[39]*s5 + v[40]*s6 + v[41]*s7;
      x[idx+6] -= v[42]*s1 + v[43]*s2 + v[44]*s3 + v[45]*s4 + v[46]*s5 + v[47]*s6 + v[48]*s7;
      v -= 49;
    }
  }
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*49*(a->nz) - 7*a->n);
  PetscFunctionReturn(0);
}

/*---------------------------------------------------------------------------------------------*/
#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_1"
int MatSolveTranspose_SeqBAIJ_1(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,*rout,*cout;
  int             *diag = a->diag;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,*x,*b,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout;

  /* copy the b into temp work space according to permutation */
  for ( i=0; i<n; i++ ) {
    t[i] = b[c[i]];
  } 

  /* forward solve the U^T */
  for ( i=0; i<n; i++ ) {

    v     = aa + diag[i];
    /* multiply by the inverse of the block diagonal */
    s1    = (*v++)*t[i];
    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      t[*vi++]  -= (*v++)*s1;
    }
    t[i]   = s1;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + diag[i] - 1;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    s1   = t[i];
    while (nz--) {
      t[*vi--]   -=  (*v--)*s1;
    }
  }

  /* copy t into x according to permutation */
  for ( i=0; i<n; i++ ) {
    x[r[i]]   = t[i];
  } 

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*(a->nz) - a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_2"
int MatSolveTranspose_SeqBAIJ_2(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,*rout,*cout;
  int             *diag = a->diag,ii,ic,ir,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,x1,x2;
  Scalar          *x,*b,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout;

  /* copy the b into temp work space according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ic      = 2*c[i];
    t[ii]   = b[ic];
    t[ii+1] = b[ic+1];
    ii += 2;
  } 

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 4*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1    = t[idx];   x2 = t[1+idx];
    s1 = v[0]*x1  +  v[1]*x2;
    s2 = v[2]*x1  +  v[3]*x2;
    v += 4;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 2*(*vi++);
      t[oidx]   -= v[0]*s1  +  v[1]*s2;
      t[oidx+1] -= v[2]*s1  +  v[3]*s2;
      v  += 4;
    }
    t[idx]   = s1;t[1+idx] = s2;
    idx += 2;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 4*diag[i] - 4;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 2*i;
    s1 = t[idt];  s2 = t[1+idt];
    while (nz--) {
      idx   = 2*(*vi--);
      t[idx]   -=  v[0]*s1 +  v[1]*s2;
      t[idx+1] -=  v[2]*s1 +  v[3]*s2;
      v -= 4;
    }
  }

  /* copy t into x according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ir      = 2*r[i];
    x[ir]   = t[ii];
    x[ir+1] = t[ii+1];
    ii += 2;
  } 

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*4*(a->nz) - 2*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_3"
int MatSolveTranspose_SeqBAIJ_3(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,*rout,*cout;
  int             *diag = a->diag,ii,ic,ir,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,s3,x1,x2,x3;
  Scalar          *x,*b,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout;

  /* copy the b into temp work space according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ic      = 3*c[i];
    t[ii]   = b[ic];
    t[ii+1] = b[ic+1];
    t[ii+2] = b[ic+2];
    ii += 3;
  } 

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 9*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1    = t[idx];   x2 = t[1+idx]; x3    = t[2+idx];
    s1 = v[0]*x1  +  v[1]*x2 +  v[2]*x3;
    s2 = v[3]*x1  +  v[4]*x2 +  v[5]*x3;
    s3 = v[6]*x1  +  v[7]*x2 + v[8]*x3;
    v += 9;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 3*(*vi++);
      t[oidx]   -= v[0]*s1  +  v[1]*s2 +  v[2]*s3;
      t[oidx+1] -= v[3]*s1  +  v[4]*s2 +  v[5]*s3;
      t[oidx+2] -= v[6]*s1 + v[7]*s2 + v[8]*s3;
      v  += 9;
    }
    t[idx]   = s1;t[1+idx] = s2; t[2+idx] = s3;
    idx += 3;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 9*diag[i] - 9;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 3*i;
    s1 = t[idt];  s2 = t[1+idt]; s3 = t[2+idt];
    while (nz--) {
      idx   = 3*(*vi--);
      t[idx]   -=  v[0]*s1 +  v[1]*s2 +  v[2]*s3;
      t[idx+1] -=  v[3]*s1 +  v[4]*s2 +  v[5]*s3;
      t[idx+2] -= v[6]*s1 + v[7]*s2 + v[8]*s3;
      v -= 9;
    }
  }

  /* copy t into x according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ir      = 3*r[i];
    x[ir]   = t[ii];
    x[ir+1] = t[ii+1];
    x[ir+2] = t[ii+2];
    ii += 3;
  } 

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*9*(a->nz) - 3*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_4"
int MatSolveTranspose_SeqBAIJ_4(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,*rout,*cout;
  int             *diag = a->diag,ii,ic,ir,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,s3,s4,x1,x2,x3,x4;
  Scalar          *x,*b,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout;

  /* copy the b into temp work space according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ic      = 4*c[i];
    t[ii]   = b[ic];
    t[ii+1] = b[ic+1];
    t[ii+2] = b[ic+2];
    t[ii+3] = b[ic+3];
    ii += 4;
  } 

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 16*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1    = t[idx];   x2 = t[1+idx]; x3    = t[2+idx]; x4 = t[3+idx];
    s1 = v[0]*x1  +  v[1]*x2 +  v[2]*x3 +  v[3]*x4;
    s2 = v[4]*x1  +  v[5]*x2 +  v[6]*x3 +  v[7]*x4;
    s3 = v[8]*x1  +  v[9]*x2 + v[10]*x3 + v[11]*x4;
    s4 = v[12]*x1 + v[13]*x2 + v[14]*x3 + v[15]*x4;
    v += 16;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 4*(*vi++);
      t[oidx]   -= v[0]*s1  +  v[1]*s2 +  v[2]*s3 +  v[3]*s4;
      t[oidx+1] -= v[4]*s1  +  v[5]*s2 +  v[6]*s3 +  v[7]*s4;
      t[oidx+2] -= v[8]*s1 + v[9]*s2 + v[10]*s3 + v[11]*s4;
      t[oidx+3] -= v[12]*s1 + v[13]*s2 + v[14]*s3 + v[15]*s4;
      v  += 16;
    }
    t[idx]   = s1;t[1+idx] = s2; t[2+idx] = s3;t[3+idx] = s4;
    idx += 4;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 16*diag[i] - 16;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 4*i;
    s1 = t[idt];  s2 = t[1+idt]; s3 = t[2+idt];s4 = t[3+idt];
    while (nz--) {
      idx   = 4*(*vi--);
      t[idx]   -=  v[0]*s1 +  v[1]*s2 +  v[2]*s3 +  v[3]*s4;
      t[idx+1] -=  v[4]*s1 +  v[5]*s2 +  v[6]*s3 +  v[7]*s4;
      t[idx+2] -= v[8]*s1 + v[9]*s2 + v[10]*s3 + v[11]*s4;
      t[idx+3] -= v[12]*s1 + v[13]*s2 + v[14]*s3 + v[15]*s4;
      v -= 16;
    }
  }

  /* copy t into x according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ir      = 4*r[i];
    x[ir]   = t[ii];
    x[ir+1] = t[ii+1];
    x[ir+2] = t[ii+2];
    x[ir+3] = t[ii+3];
    ii += 4;
  } 

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*16*(a->nz) - 4*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_5"
int MatSolveTranspose_SeqBAIJ_5(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,*rout,*cout;
  int             *diag = a->diag,ii,ic,ir,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,s3,s4,s5,x1,x2,x3,x4,x5;
  Scalar          *x,*b,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout;

  /* copy the b into temp work space according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ic      = 5*c[i];
    t[ii]   = b[ic];
    t[ii+1] = b[ic+1];
    t[ii+2] = b[ic+2];
    t[ii+3] = b[ic+3];
    t[ii+4] = b[ic+4];
    ii += 5;
  } 

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 25*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1    = t[idx];   x2 = t[1+idx]; x3    = t[2+idx]; x4 = t[3+idx]; x5 = t[4+idx];
    s1 = v[0]*x1  +  v[1]*x2 +  v[2]*x3 +  v[3]*x4 +  v[4]*x5;
    s2 = v[5]*x1  +  v[6]*x2 +  v[7]*x3 +  v[8]*x4 +  v[9]*x5;
    s3 = v[10]*x1 + v[11]*x2 + v[12]*x3 + v[13]*x4 + v[14]*x5;
    s4 = v[15]*x1 + v[16]*x2 + v[17]*x3 + v[18]*x4 + v[19]*x5;
    s5 = v[20]*x1 + v[21]*x2 + v[22]*x3 + v[23]*x4 + v[24]*x5;
    v += 25;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 5*(*vi++);
      t[oidx]   -= v[0]*s1  +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5;
      t[oidx+1] -= v[5]*s1  +  v[6]*s2 +  v[7]*s3 +  v[8]*s4 +  v[9]*s5;
      t[oidx+2] -= v[10]*s1 + v[11]*s2 + v[12]*s3 + v[13]*s4 + v[14]*s5;
      t[oidx+3] -= v[15]*s1 + v[16]*s2 + v[17]*s3 + v[18]*s4 + v[19]*s5;
      t[oidx+4] -= v[20]*s1 + v[21]*s2 + v[22]*s3 + v[23]*s4 + v[24]*s5;
      v  += 25;
    }
    t[idx]   = s1;t[1+idx] = s2; t[2+idx] = s3;t[3+idx] = s4; t[4+idx] = s5;
    idx += 5;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 25*diag[i] - 25;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 5*i;
    s1 = t[idt];  s2 = t[1+idt]; s3 = t[2+idt];s4 = t[3+idt]; s5 = t[4+idt];
    while (nz--) {
      idx   = 5*(*vi--);
      t[idx]   -=  v[0]*s1 +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5;
      t[idx+1] -=  v[5]*s1 +  v[6]*s2 +  v[7]*s3 +  v[8]*s4 +  v[9]*s5;
      t[idx+2] -= v[10]*s1 + v[11]*s2 + v[12]*s3 + v[13]*s4 + v[14]*s5;
      t[idx+3] -= v[15]*s1 + v[16]*s2 + v[17]*s3 + v[18]*s4 + v[19]*s5;
      t[idx+4] -= v[20]*s1 + v[21]*s2 + v[22]*s3 + v[23]*s4 + v[24]*s5;
      v -= 25;
    }
  }

  /* copy t into x according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ir      = 5*r[i];
    x[ir]   = t[ii];
    x[ir+1] = t[ii+1];
    x[ir+2] = t[ii+2];
    x[ir+3] = t[ii+3];
    x[ir+4] = t[ii+4];
    ii += 5;
  } 

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*25*(a->nz) - 5*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_6"
int MatSolveTranspose_SeqBAIJ_6(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,*rout,*cout;
  int             *diag = a->diag,ii,ic,ir,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,s3,s4,s5,s6,x1,x2,x3,x4,x5,x6;
  Scalar          *x,*b,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout;

  /* copy the b into temp work space according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ic      = 6*c[i];
    t[ii]   = b[ic];
    t[ii+1] = b[ic+1];
    t[ii+2] = b[ic+2];
    t[ii+3] = b[ic+3];
    t[ii+4] = b[ic+4];
    t[ii+5] = b[ic+5];
    ii += 6;
  } 

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 36*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1    = t[idx];   x2 = t[1+idx]; x3    = t[2+idx]; x4 = t[3+idx]; x5 = t[4+idx];
    x6    = t[5+idx]; 
    s1 = v[0]*x1  +  v[1]*x2 +  v[2]*x3 +  v[3]*x4 +  v[4]*x5 +  v[5]*x6;
    s2 = v[6]*x1  +  v[7]*x2 +  v[8]*x3 +  v[9]*x4 + v[10]*x5 + v[11]*x6;
    s3 = v[12]*x1 + v[13]*x2 + v[14]*x3 + v[15]*x4 + v[16]*x5 + v[17]*x6;
    s4 = v[18]*x1 + v[19]*x2 + v[20]*x3 + v[21]*x4 + v[22]*x5 + v[23]*x6;
    s5 = v[24]*x1 + v[25]*x2 + v[26]*x3 + v[27]*x4 + v[28]*x5 + v[29]*x6;
    s6 = v[30]*x1 + v[31]*x2 + v[32]*x3 + v[33]*x4 + v[34]*x5 + v[35]*x6;
    v += 36;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 6*(*vi++);
      t[oidx]   -= v[0]*s1  +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5 +  v[5]*s6;
      t[oidx+1] -= v[6]*s1  +  v[7]*s2 +  v[8]*s3 +  v[9]*s4 + v[10]*s5 + v[11]*s6;
      t[oidx+2] -= v[12]*s1 + v[13]*s2 + v[14]*s3 + v[15]*s4 + v[16]*s5 + v[17]*s6;
      t[oidx+3] -= v[18]*s1 + v[19]*s2 + v[20]*s3 + v[21]*s4 + v[22]*s5 + v[23]*s6;
      t[oidx+4] -= v[24]*s1 + v[25]*s2 + v[26]*s3 + v[27]*s4 + v[28]*s5 + v[29]*s6;
      t[oidx+5] -= v[30]*s1 + v[31]*s2 + v[32]*s3 + v[33]*s4 + v[34]*s5 + v[35]*s6;
      v  += 36;
    }
    t[idx]   = s1;t[1+idx] = s2; t[2+idx] = s3;t[3+idx] = s4; t[4+idx] = s5;
    t[5+idx] = s6;
    idx += 6;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 36*diag[i] - 36;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 6*i;
    s1 = t[idt];  s2 = t[1+idt]; s3 = t[2+idt];s4 = t[3+idt]; s5 = t[4+idt];
    s6 = t[5+idt];
    while (nz--) {
      idx   = 6*(*vi--);
      t[idx]   -=  v[0]*s1 +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5 +  v[5]*s6;
      t[idx+1] -=  v[6]*s1 +  v[7]*s2 +  v[8]*s3 +  v[9]*s4 + v[10]*s5 + v[11]*s6;
      t[idx+2] -= v[12]*s1 + v[13]*s2 + v[14]*s3 + v[15]*s4 + v[16]*s5 + v[17]*s6;
      t[idx+3] -= v[18]*s1 + v[19]*s2 + v[20]*s3 + v[21]*s4 + v[22]*s5 + v[23]*s6;
      t[idx+4] -= v[24]*s1 + v[25]*s2 + v[26]*s3 + v[27]*s4 + v[28]*s5 + v[29]*s6;
      t[idx+5] -= v[30]*s1 + v[31]*s2 + v[32]*s3 + v[33]*s4 + v[34]*s5 + v[35]*s6;
      v -= 36;
    }
  }

  /* copy t into x according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ir      = 6*r[i];
    x[ir]   = t[ii];
    x[ir+1] = t[ii+1];
    x[ir+2] = t[ii+2];
    x[ir+3] = t[ii+3];
    x[ir+4] = t[ii+4];
    x[ir+5] = t[ii+5];
    ii += 6;
  } 

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*36*(a->nz) - 6*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolveTranspose_SeqBAIJ_7"
int MatSolveTranspose_SeqBAIJ_7(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,*rout,*cout;
  int             *diag = a->diag,ii,ic,ir,oidx;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,s3,s4,s5,s6,s7,x1,x2,x3,x4,x5,x6,x7;
  Scalar          *x,*b,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout;

  /* copy the b into temp work space according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ic      = 7*c[i];
    t[ii]   = b[ic];
    t[ii+1] = b[ic+1];
    t[ii+2] = b[ic+2];
    t[ii+3] = b[ic+3];
    t[ii+4] = b[ic+4];
    t[ii+5] = b[ic+5];
    t[ii+6] = b[ic+6];
    ii += 7;
  } 

  /* forward solve the U^T */
  idx = 0;
  for ( i=0; i<n; i++ ) {

    v     = aa + 49*diag[i];
    /* multiply by the inverse of the block diagonal */
    x1    = t[idx];   x2 = t[1+idx]; x3    = t[2+idx]; x4 = t[3+idx]; x5 = t[4+idx];
    x6    = t[5+idx]; x7 = t[6+idx];
    s1 = v[0]*x1  +  v[1]*x2 +  v[2]*x3 +  v[3]*x4 +  v[4]*x5 +  v[5]*x6 +  v[6]*x7;
    s2 = v[7]*x1  +  v[8]*x2 +  v[9]*x3 + v[10]*x4 + v[11]*x5 + v[12]*x6 + v[13]*x7;
    s3 = v[14]*x1 + v[15]*x2 + v[16]*x3 + v[17]*x4 + v[18]*x5 + v[19]*x6 + v[20]*x7;
    s4 = v[21]*x1 + v[22]*x2 + v[23]*x3 + v[24]*x4 + v[25]*x5 + v[26]*x6 + v[27]*x7;
    s5 = v[28]*x1 + v[29]*x2 + v[30]*x3 + v[31]*x4 + v[32]*x5 + v[33]*x6 + v[34]*x7;
    s6 = v[35]*x1 + v[36]*x2 + v[37]*x3 + v[38]*x4 + v[39]*x5 + v[40]*x6 + v[41]*x7;
    s7 = v[42]*x1 + v[43]*x2 + v[44]*x3 + v[45]*x4 + v[46]*x5 + v[47]*x6 + v[48]*x7;
    v += 49;

    vi    = aj + diag[i] + 1;
    nz    = ai[i+1] - diag[i] - 1;
    while (nz--) {
      oidx = 7*(*vi++);
      t[oidx]   -= v[0]*s1  +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5 +  v[5]*s6 +  v[6]*s7;
      t[oidx+1] -= v[7]*s1  +  v[8]*s2 +  v[9]*s3 + v[10]*s4 + v[11]*s5 + v[12]*s6 + v[13]*s7;
      t[oidx+2] -= v[14]*s1 + v[15]*s2 + v[16]*s3 + v[17]*s4 + v[18]*s5 + v[19]*s6 + v[20]*s7;
      t[oidx+3] -= v[21]*s1 + v[22]*s2 + v[23]*s3 + v[24]*s4 + v[25]*s5 + v[26]*s6 + v[27]*s7;
      t[oidx+4] -= v[28]*s1 + v[29]*s2 + v[30]*s3 + v[31]*s4 + v[32]*s5 + v[33]*s6 + v[34]*s7;
      t[oidx+5] -= v[35]*s1 + v[36]*s2 + v[37]*s3 + v[38]*s4 + v[39]*s5 + v[40]*s6 + v[41]*s7;
      t[oidx+6] -= v[42]*s1 + v[43]*s2 + v[44]*s3 + v[45]*s4 + v[46]*s5 + v[47]*s6 + v[48]*s7;
      v  += 49;
    }
    t[idx]   = s1;t[1+idx] = s2; t[2+idx] = s3;t[3+idx] = s4; t[4+idx] = s5;
    t[5+idx] = s6;t[6+idx] = s7;
    idx += 7;
  }
  /* backward solve the L^T */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 49*diag[i] - 49;
    vi   = aj + diag[i] - 1;
    nz   = diag[i] - ai[i];
    idt  = 7*i;
    s1 = t[idt];  s2 = t[1+idt]; s3 = t[2+idt];s4 = t[3+idt]; s5 = t[4+idt];
    s6 = t[5+idt];s7 = t[6+idt]; 
    while (nz--) {
      idx   = 7*(*vi--);
      t[idx]   -=  v[0]*s1 +  v[1]*s2 +  v[2]*s3 +  v[3]*s4 +  v[4]*s5 +  v[5]*s6 +  v[6]*s7;
      t[idx+1] -=  v[7]*s1 +  v[8]*s2 +  v[9]*s3 + v[10]*s4 + v[11]*s5 + v[12]*s6 + v[13]*s7;
      t[idx+2] -= v[14]*s1 + v[15]*s2 + v[16]*s3 + v[17]*s4 + v[18]*s5 + v[19]*s6 + v[20]*s7;
      t[idx+3] -= v[21]*s1 + v[22]*s2 + v[23]*s3 + v[24]*s4 + v[25]*s5 + v[26]*s6 + v[27]*s7;
      t[idx+4] -= v[28]*s1 + v[29]*s2 + v[30]*s3 + v[31]*s4 + v[32]*s5 + v[33]*s6 + v[34]*s7;
      t[idx+5] -= v[35]*s1 + v[36]*s2 + v[37]*s3 + v[38]*s4 + v[39]*s5 + v[40]*s6 + v[41]*s7;
      t[idx+6] -= v[42]*s1 + v[43]*s2 + v[44]*s3 + v[45]*s4 + v[46]*s5 + v[47]*s6 + v[48]*s7;
      v -= 49;
    }
  }

  /* copy t into x according to permutation */
  ii = 0;
  for ( i=0; i<n; i++ ) {
    ir      = 7*r[i];
    x[ir]   = t[ii];
    x[ir+1] = t[ii+1];
    x[ir+2] = t[ii+2];
    x[ir+3] = t[ii+3];
    x[ir+4] = t[ii+4];
    x[ir+5] = t[ii+5];
    x[ir+6] = t[ii+6];
    ii += 7;
  } 

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*49*(a->nz) - 7*a->n);
  PetscFunctionReturn(0);
}

/* ----------------------------------------------------------- */
#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_N"
int MatSolve_SeqBAIJ_N(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j;
  int             nz,bs=a->bs,bs2=a->bs2,*rout,*cout;
  MatScalar       *aa=a->a,*v;
  Scalar          *x,*b,*s,*t,*ls;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout + (n-1);

  /* forward solve the lower triangular */
  ierr = PetscMemcpy(t,b+bs*(*r++),bs*sizeof(Scalar));CHKERRQ(ierr);
  for ( i=1; i<n; i++ ) {
    v   = aa + bs2*ai[i];
    vi  = aj + ai[i];
    nz  = a->diag[i] - ai[i];
    s = t + bs*i;
    ierr = PetscMemcpy(s,b+bs*(*r++),bs*sizeof(Scalar));CHKERRQ(ierr);
    while (nz--) {
      Kernel_v_gets_v_minus_A_times_w(bs,s,v,t+bs*(*vi++));
      v += bs2;
    }
  }
  /* backward solve the upper triangular */
  ls = a->solve_work + a->n;
  for ( i=n-1; i>=0; i-- ){
    v   = aa + bs2*(a->diag[i] + 1);
    vi  = aj + a->diag[i] + 1;
    nz  = ai[i+1] - a->diag[i] - 1;
    ierr = PetscMemcpy(ls,t+i*bs,bs*sizeof(Scalar));CHKERRQ(ierr);
    while (nz--) {
      Kernel_v_gets_v_minus_A_times_w(bs,ls,v,t+bs*(*vi++));
      v += bs2;
    }
    Kernel_w_gets_A_times_v(bs,ls,aa+bs2*a->diag[i],t+i*bs);
    ierr = PetscMemcpy(x + bs*(*c--),t+i*bs,bs*sizeof(Scalar));CHKERRQ(ierr);
  }

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*(a->bs2)*(a->nz) - a->bs*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_7"
int MatSolve_SeqBAIJ_7(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,idc,*rout,*cout;
  int             *diag = a->diag;
  MatScalar       *aa=a->a,*v;
  Scalar          s1,s2,s3,s4,s5,s6,s7,x1,x2,x3,x4,x5,x6,x7;
  Scalar          *x,*b,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout + (n-1);

  /* forward solve the lower triangular */
  idx    = 7*(*r++); 
  t[0] = b[idx];   t[1] = b[1+idx]; 
  t[2] = b[2+idx]; t[3] = b[3+idx]; t[4] = b[4+idx];
  t[5] = b[5+idx]; t[6] = b[6+idx]; 

  for ( i=1; i<n; i++ ) {
    v     = aa + 49*ai[i];
    vi    = aj + ai[i];
    nz    = diag[i] - ai[i];
    idx   = 7*(*r++); 
    s1  = b[idx];s2 = b[1+idx];s3 = b[2+idx];s4 = b[3+idx];
    s5  = b[4+idx];s6 = b[5+idx];s7 = b[6+idx];
    while (nz--) {
      idx   = 7*(*vi++);
      x1    = t[idx];  x2 = t[1+idx];x3 = t[2+idx];
      x4    = t[3+idx];x5 = t[4+idx];
      x6    = t[5+idx];x7 = t[6+idx];
      s1 -= v[0]*x1 + v[7]*x2  + v[14]*x3 + v[21]*x4 + v[28]*x5 + v[35]*x6 + v[42]*x7;
      s2 -= v[1]*x1 + v[8]*x2  + v[15]*x3 + v[22]*x4 + v[29]*x5 + v[36]*x6 + v[43]*x7;
      s3 -= v[2]*x1 + v[9]*x2  + v[16]*x3 + v[23]*x4 + v[30]*x5 + v[37]*x6 + v[44]*x7;
      s4 -= v[3]*x1 + v[10]*x2 + v[17]*x3 + v[24]*x4 + v[31]*x5 + v[38]*x6 + v[45]*x7;
      s5 -= v[4]*x1 + v[11]*x2 + v[18]*x3 + v[25]*x4 + v[32]*x5 + v[39]*x6 + v[46]*x7;
      s6 -= v[5]*x1 + v[12]*x2 + v[19]*x3 + v[26]*x4 + v[33]*x5 + v[40]*x6 + v[47]*x7;
      s7 -= v[6]*x1 + v[13]*x2 + v[20]*x3 + v[27]*x4 + v[34]*x5 + v[41]*x6 + v[48]*x7;
      v += 49;
    }
    idx = 7*i;
    t[idx]   = s1;t[1+idx] = s2;
    t[2+idx] = s3;t[3+idx] = s4; t[4+idx] = s5;
    t[5+idx] = s6;t[6+idx] = s7;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 49*diag[i] + 49;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 7*i;
    s1 = t[idt];  s2 = t[1+idt]; 
    s3 = t[2+idt];s4 = t[3+idt]; s5 = t[4+idt];
    s6 = t[5+idt];s7 = t[6+idt]; 
    while (nz--) {
      idx   = 7*(*vi++);
      x1    = t[idx];   x2 = t[1+idx];
      x3    = t[2+idx]; x4 = t[3+idx]; x5 = t[4+idx];
      x6    = t[5+idx]; x7 = t[6+idx];
      s1 -= v[0]*x1 + v[7]*x2  + v[14]*x3 + v[21]*x4 + v[28]*x5 + v[35]*x6 + v[42]*x7;
      s2 -= v[1]*x1 + v[8]*x2  + v[15]*x3 + v[22]*x4 + v[29]*x5 + v[36]*x6 + v[43]*x7;
      s3 -= v[2]*x1 + v[9]*x2  + v[16]*x3 + v[23]*x4 + v[30]*x5 + v[37]*x6 + v[44]*x7;
      s4 -= v[3]*x1 + v[10]*x2 + v[17]*x3 + v[24]*x4 + v[31]*x5 + v[38]*x6 + v[45]*x7;
      s5 -= v[4]*x1 + v[11]*x2 + v[18]*x3 + v[25]*x4 + v[32]*x5 + v[39]*x6 + v[46]*x7;
      s6 -= v[5]*x1 + v[12]*x2 + v[19]*x3 + v[26]*x4 + v[33]*x5 + v[40]*x6 + v[47]*x7;
      s7 -= v[6]*x1 + v[13]*x2 + v[20]*x3 + v[27]*x4 + v[34]*x5 + v[41]*x6 + v[48]*x7;
      v += 49;
    }
    idc = 7*(*c--);
    v   = aa + 49*diag[i];
    x[idc]   = t[idt]   = v[0]*s1+v[7]*s2+v[14]*s3+
                                 v[21]*s4+v[28]*s5+v[35]*s6+v[42]*s7;
    x[1+idc] = t[1+idt] = v[1]*s1+v[8]*s2+v[15]*s3+
                                 v[22]*s4+v[29]*s5+v[36]*s6+v[43]*s7;
    x[2+idc] = t[2+idt] = v[2]*s1+v[9]*s2+v[16]*s3+
                                 v[23]*s4+v[30]*s5+v[37]*s6+v[44]*s7;
    x[3+idc] = t[3+idt] = v[3]*s1+v[10]*s2+v[17]*s3+
                                 v[24]*s4+v[31]*s5+v[38]*s6+v[45]*s7;
    x[4+idc] = t[4+idt] = v[4]*s1+v[11]*s2+v[18]*s3+
                                 v[25]*s4+v[32]*s5+v[39]*s6+v[46]*s7;
    x[5+idc] = t[5+idt] = v[5]*s1+v[12]*s2+v[19]*s3+
                                 v[26]*s4+v[33]*s5+v[40]*s6+v[47]*s7;
    x[6+idc] = t[6+idt] = v[6]*s1+v[13]*s2+v[20]*s3+
                                 v[27]*s4+v[34]*s5+v[41]*s6+v[48]*s7;
  }

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*49*(a->nz) - 7*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_7_NaturalOrdering"
int MatSolve_SeqBAIJ_7_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a = (Mat_SeqBAIJ *)A->data;
  int             i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt;
  int             ierr,*diag = a->diag,jdx;
  MatScalar       *aa=a->a,*v;
  Scalar          *x,*b,s1,s2,s3,s4,s5,s6,s7,x1,x2,x3,x4,x5,x6,x7;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr); 
  /* forward solve the lower triangular */
  idx    = 0;
  x[0] = b[idx];   x[1] = b[1+idx]; x[2] = b[2+idx]; 
  x[3] = b[3+idx]; x[4] = b[4+idx]; x[5] = b[5+idx];
  x[6] = b[6+idx]; 
  for ( i=1; i<n; i++ ) {
    v     =  aa + 49*ai[i];
    vi    =  aj + ai[i];
    nz    =  diag[i] - ai[i];
    idx   =  7*i;
    s1  =  b[idx];   s2 = b[1+idx]; s3 = b[2+idx];
    s4  =  b[3+idx]; s5 = b[4+idx]; s6 = b[5+idx];
    s7  =  b[6+idx];
    while (nz--) {
      jdx   = 7*(*vi++);
      x1    = x[jdx];   x2 = x[1+jdx]; x3 = x[2+jdx];
      x4    = x[3+jdx]; x5 = x[4+jdx]; x6 = x[5+jdx];
      x7    = x[6+jdx];
      s1 -= v[0]*x1 + v[7]*x2  + v[14]*x3 + v[21]*x4 + v[28]*x5 + v[35]*x6 + v[42]*x7;
      s2 -= v[1]*x1 + v[8]*x2  + v[15]*x3 + v[22]*x4 + v[29]*x5 + v[36]*x6 + v[43]*x7;
      s3 -= v[2]*x1 + v[9]*x2  + v[16]*x3 + v[23]*x4 + v[30]*x5 + v[37]*x6 + v[44]*x7;
      s4 -= v[3]*x1 + v[10]*x2 + v[17]*x3 + v[24]*x4 + v[31]*x5 + v[38]*x6 + v[45]*x7;
      s5 -= v[4]*x1 + v[11]*x2 + v[18]*x3 + v[25]*x4 + v[32]*x5 + v[39]*x6 + v[46]*x7;
      s6 -= v[5]*x1 + v[12]*x2 + v[19]*x3 + v[26]*x4 + v[33]*x5 + v[40]*x6 + v[47]*x7;
      s7 -= v[6]*x1 + v[13]*x2 + v[20]*x3 + v[27]*x4 + v[34]*x5 + v[41]*x6 + v[48]*x7;
      v += 49;
     }
    x[idx]   = s1;
    x[1+idx] = s2;
    x[2+idx] = s3;
    x[3+idx] = s4;
    x[4+idx] = s5;
    x[5+idx] = s6;
    x[6+idx] = s7;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 49*diag[i] + 49;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 7*i;
    s1 = x[idt];   s2 = x[1+idt]; 
    s3 = x[2+idt]; s4 = x[3+idt]; 
    s5 = x[4+idt]; s6 = x[5+idt];
    s7 = x[6+idt];
    while (nz--) {
      idx   = 7*(*vi++);
      x1    = x[idx];   x2 = x[1+idx]; x3 = x[2+idx]; 
      x4    = x[3+idx]; x5 = x[4+idx]; x6 = x[5+idx]; 
      x7    = x[6+idx];  
      s1 -= v[0]*x1 + v[7]*x2  + v[14]*x3 + v[21]*x4 + v[28]*x5 + v[35]*x6 + v[42]*x7;
      s2 -= v[1]*x1 + v[8]*x2  + v[15]*x3 + v[22]*x4 + v[29]*x5 + v[36]*x6 + v[43]*x7;
      s3 -= v[2]*x1 + v[9]*x2  + v[16]*x3 + v[23]*x4 + v[30]*x5 + v[37]*x6 + v[44]*x7;
      s4 -= v[3]*x1 + v[10]*x2 + v[17]*x3 + v[24]*x4 + v[31]*x5 + v[38]*x6 + v[45]*x7;
      s5 -= v[4]*x1 + v[11]*x2 + v[18]*x3 + v[25]*x4 + v[32]*x5 + v[39]*x6 + v[46]*x7;
      s6 -= v[5]*x1 + v[12]*x2 + v[19]*x3 + v[26]*x4 + v[33]*x5 + v[40]*x6 + v[47]*x7;
      s7 -= v[6]*x1 + v[13]*x2 + v[20]*x3 + v[27]*x4 + v[34]*x5 + v[41]*x6 + v[48]*x7;
      v += 49;
    }
    v        = aa + 49*diag[i];
    x[idt]   = v[0]*s1 + v[7]*s2  + v[14]*s3 + v[21]*s4 
                         + v[28]*s5 + v[35]*s6 + v[42]*s7;
    x[1+idt] = v[1]*s1 + v[8]*s2  + v[15]*s3 + v[22]*s4 
                         + v[29]*s5 + v[36]*s6 + v[43]*s7;
    x[2+idt] = v[2]*s1 + v[9]*s2  + v[16]*s3 + v[23]*s4 
                         + v[30]*s5 + v[37]*s6 + v[44]*s7;
    x[3+idt] = v[3]*s1 + v[10]*s2  + v[17]*s3 + v[24]*s4 
                         + v[31]*s5 + v[38]*s6 + v[45]*s7;
    x[4+idt] = v[4]*s1 + v[11]*s2  + v[18]*s3 + v[25]*s4 
                         + v[32]*s5 + v[39]*s6 + v[46]*s7;
    x[5+idt] = v[5]*s1 + v[12]*s2  + v[19]*s3 + v[26]*s4 
                         + v[33]*s5 + v[40]*s6 + v[47]*s7;
    x[6+idt] = v[6]*s1 + v[13]*s2  + v[20]*s3 + v[27]*s4 
                         + v[34]*s5 + v[41]*s6 + v[48]*s7;
  }

  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr); 
  PLogFlops(2*36*(a->nz) - 6*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_6"
int MatSolve_SeqBAIJ_6(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,idc,*rout,*cout;
  int             *diag = a->diag;
  MatScalar       *aa=a->a,*v;
  Scalar          *x,*b,s1,s2,s3,s4,s5,s6,x1,x2,x3,x4,x5,x6,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout + (n-1);

  /* forward solve the lower triangular */
  idx    = 6*(*r++); 
  t[0] = b[idx];   t[1] = b[1+idx]; 
  t[2] = b[2+idx]; t[3] = b[3+idx]; 
  t[4] = b[4+idx]; t[5] = b[5+idx];
  for ( i=1; i<n; i++ ) {
    v     = aa + 36*ai[i];
    vi    = aj + ai[i];
    nz    = diag[i] - ai[i];
    idx   = 6*(*r++); 
    s1  = b[idx];s2 = b[1+idx];s3 = b[2+idx];s4 = b[3+idx];
    s5  = b[4+idx]; s6 = b[5+idx];
    while (nz--) {
      idx   = 6*(*vi++);
      x1    = t[idx];   x2 = t[1+idx]; x3 = t[2+idx];
      x4    = t[3+idx]; x5 = t[4+idx]; x6 = t[5+idx];
      s1 -= v[0]*x1 + v[6]*x2  + v[12]*x3 + v[18]*x4 + v[24]*x5 + v[30]*x6;
      s2 -= v[1]*x1 + v[7]*x2  + v[13]*x3 + v[19]*x4 + v[25]*x5 + v[31]*x6;
      s3 -= v[2]*x1 + v[8]*x2  + v[14]*x3 + v[20]*x4 + v[26]*x5 + v[32]*x6;
      s4 -= v[3]*x1 + v[9]*x2  + v[15]*x3 + v[21]*x4 + v[27]*x5 + v[33]*x6;
      s5 -= v[4]*x1 + v[10]*x2 + v[16]*x3 + v[22]*x4 + v[28]*x5 + v[34]*x6;
      s6 -= v[5]*x1 + v[11]*x2 + v[17]*x3 + v[23]*x4 + v[29]*x5 + v[35]*x6;
      v += 36;
    }
    idx = 6*i;
    t[idx]   = s1;t[1+idx] = s2;
    t[2+idx] = s3;t[3+idx] = s4; 
    t[4+idx] = s5;t[5+idx] = s6;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 36*diag[i] + 36;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 6*i;
    s1 = t[idt];  s2 = t[1+idt]; 
    s3 = t[2+idt];s4 = t[3+idt]; 
    s5 = t[4+idt];s6 = t[5+idt];
    while (nz--) {
      idx   = 6*(*vi++);
      x1    = t[idx];   x2 = t[1+idx];
      x3    = t[2+idx]; x4 = t[3+idx]; 
      x5    = t[4+idx]; x6 = t[5+idx];
      s1 -= v[0]*x1 + v[6]*x2  + v[12]*x3 + v[18]*x4 + v[24]*x5 + v[30]*x6;
      s2 -= v[1]*x1 + v[7]*x2  + v[13]*x3 + v[19]*x4 + v[25]*x5 + v[31]*x6;
      s3 -= v[2]*x1 + v[8]*x2  + v[14]*x3 + v[20]*x4 + v[26]*x5 + v[32]*x6;
      s4 -= v[3]*x1 + v[9]*x2  + v[15]*x3 + v[21]*x4 + v[27]*x5 + v[33]*x6;
      s5 -= v[4]*x1 + v[10]*x2 + v[16]*x3 + v[22]*x4 + v[28]*x5 + v[34]*x6;
      s6 -= v[5]*x1 + v[11]*x2 + v[17]*x3 + v[23]*x4 + v[29]*x5 + v[35]*x6;
      v += 36;
    }
    idc = 6*(*c--);
    v   = aa + 36*diag[i];
    x[idc]   = t[idt]   = v[0]*s1+v[6]*s2+v[12]*s3+
                                 v[18]*s4+v[24]*s5+v[30]*s6;
    x[1+idc] = t[1+idt] = v[1]*s1+v[7]*s2+v[13]*s3+
                                 v[19]*s4+v[25]*s5+v[31]*s6;
    x[2+idc] = t[2+idt] = v[2]*s1+v[8]*s2+v[14]*s3+
                                 v[20]*s4+v[26]*s5+v[32]*s6;
    x[3+idc] = t[3+idt] = v[3]*s1+v[9]*s2+v[15]*s3+
                                 v[21]*s4+v[27]*s5+v[33]*s6;
    x[4+idc] = t[4+idt] = v[4]*s1+v[10]*s2+v[16]*s3+
                                 v[22]*s4+v[28]*s5+v[34]*s6;
    x[5+idc] = t[5+idt] = v[5]*s1+v[11]*s2+v[17]*s3+
                                 v[23]*s4+v[29]*s5+v[35]*s6;
  }

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*36*(a->nz) - 6*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_6_NaturalOrdering"
int MatSolve_SeqBAIJ_6_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a = (Mat_SeqBAIJ *)A->data;
  int             i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt;
  int             ierr,*diag = a->diag,jdx;
  MatScalar       *aa=a->a,*v;
  Scalar          *x,*b,s1,s2,s3,s4,s5,s6,x1,x2,x3,x4,x5,x6;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr); 
  /* forward solve the lower triangular */
  idx    = 0;
  x[0] = b[idx];   x[1] = b[1+idx]; x[2] = b[2+idx]; 
  x[3] = b[3+idx]; x[4] = b[4+idx]; x[5] = b[5+idx];
  for ( i=1; i<n; i++ ) {
    v     =  aa + 36*ai[i];
    vi    =  aj + ai[i];
    nz    =  diag[i] - ai[i];
    idx   =  6*i;
    s1  =  b[idx];   s2 = b[1+idx]; s3 = b[2+idx];
    s4  =  b[3+idx]; s5 = b[4+idx]; s6 = b[5+idx];
    while (nz--) {
      jdx   = 6*(*vi++);
      x1    = x[jdx];   x2 = x[1+jdx]; x3 = x[2+jdx];
      x4    = x[3+jdx]; x5 = x[4+jdx]; x6 = x[5+jdx];
      s1 -= v[0]*x1 + v[6]*x2  + v[12]*x3 + v[18]*x4 + v[24]*x5 + v[30]*x6;
      s2 -= v[1]*x1 + v[7]*x2  + v[13]*x3 + v[19]*x4 + v[25]*x5 + v[31]*x6;
      s3 -= v[2]*x1 + v[8]*x2  + v[14]*x3 + v[20]*x4 + v[26]*x5 + v[32]*x6;
      s4 -= v[3]*x1 + v[9]*x2  + v[15]*x3 + v[21]*x4 + v[27]*x5 + v[33]*x6;
      s5 -= v[4]*x1 + v[10]*x2 + v[16]*x3 + v[22]*x4 + v[28]*x5 + v[34]*x6;
      s6 -= v[5]*x1 + v[11]*x2 + v[17]*x3 + v[23]*x4 + v[29]*x5 + v[35]*x6;
      v += 36;
     }
    x[idx]   = s1;
    x[1+idx] = s2;
    x[2+idx] = s3;
    x[3+idx] = s4;
    x[4+idx] = s5;
    x[5+idx] = s6;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 36*diag[i] + 36;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 6*i;
    s1 = x[idt];   s2 = x[1+idt]; 
    s3 = x[2+idt]; s4 = x[3+idt]; 
    s5 = x[4+idt]; s6 = x[5+idt];
    while (nz--) {
      idx   = 6*(*vi++);
      x1    = x[idx];   x2 = x[1+idx]; x3 = x[2+idx]; 
      x4    = x[3+idx]; x5 = x[4+idx]; x6 = x[5+idx];   
      s1 -= v[0]*x1 + v[6]*x2  + v[12]*x3 + v[18]*x4 + v[24]*x5 + v[30]*x6;
      s2 -= v[1]*x1 + v[7]*x2  + v[13]*x3 + v[19]*x4 + v[25]*x5 + v[31]*x6;
      s3 -= v[2]*x1 + v[8]*x2  + v[14]*x3 + v[20]*x4 + v[26]*x5 + v[32]*x6;
      s4 -= v[3]*x1 + v[9]*x2  + v[15]*x3 + v[21]*x4 + v[27]*x5 + v[33]*x6;
      s5 -= v[4]*x1 + v[10]*x2 + v[16]*x3 + v[22]*x4 + v[28]*x5 + v[34]*x6;
      s6 -= v[5]*x1 + v[11]*x2 + v[17]*x3 + v[23]*x4 + v[29]*x5 + v[35]*x6;
      v += 36;
    }
    v        = aa + 36*diag[i];
    x[idt]   = v[0]*s1 + v[6]*s2  + v[12]*s3 + v[18]*s4 + v[24]*s5 + v[30]*s6;
    x[1+idt] = v[1]*s1 + v[7]*s2  + v[13]*s3 + v[19]*s4 + v[25]*s5 + v[31]*s6;
    x[2+idt] = v[2]*s1 + v[8]*s2  + v[14]*s3 + v[20]*s4 + v[26]*s5 + v[32]*s6;
    x[3+idt] = v[3]*s1 + v[9]*s2  + v[15]*s3 + v[21]*s4 + v[27]*s5 + v[33]*s6;
    x[4+idt] = v[4]*s1 + v[10]*s2 + v[16]*s3 + v[22]*s4 + v[28]*s5 + v[34]*s6;
    x[5+idt] = v[5]*s1 + v[11]*s2 + v[17]*s3 + v[23]*s4 + v[29]*s5 + v[35]*s6;
  }

  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr); 
  PLogFlops(2*36*(a->nz) - 6*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_5"
int MatSolve_SeqBAIJ_5(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,idc,*rout,*cout;
  int             *diag = a->diag;
  MatScalar       *aa=a->a,*v;
  Scalar          *x,*b,s1,s2,s3,s4,s5,x1,x2,x3,x4,x5,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout + (n-1);

  /* forward solve the lower triangular */
  idx    = 5*(*r++); 
  t[0] = b[idx];   t[1] = b[1+idx]; 
  t[2] = b[2+idx]; t[3] = b[3+idx]; t[4] = b[4+idx];
  for ( i=1; i<n; i++ ) {
    v     = aa + 25*ai[i];
    vi    = aj + ai[i];
    nz    = diag[i] - ai[i];
    idx   = 5*(*r++); 
    s1  = b[idx];s2 = b[1+idx];s3 = b[2+idx];s4 = b[3+idx];
    s5  = b[4+idx];
    while (nz--) {
      idx   = 5*(*vi++);
      x1    = t[idx];  x2 = t[1+idx];x3 = t[2+idx];
      x4    = t[3+idx];x5 = t[4+idx];
      s1 -= v[0]*x1 + v[5]*x2 + v[10]*x3 + v[15]*x4 + v[20]*x5;
      s2 -= v[1]*x1 + v[6]*x2 + v[11]*x3 + v[16]*x4 + v[21]*x5;
      s3 -= v[2]*x1 + v[7]*x2 + v[12]*x3 + v[17]*x4 + v[22]*x5;
      s4 -= v[3]*x1 + v[8]*x2 + v[13]*x3 + v[18]*x4 + v[23]*x5;
      s5 -= v[4]*x1 + v[9]*x2 + v[14]*x3 + v[19]*x4 + v[24]*x5;
      v += 25;
    }
    idx = 5*i;
    t[idx]   = s1;t[1+idx] = s2;
    t[2+idx] = s3;t[3+idx] = s4; t[4+idx] = s5;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 25*diag[i] + 25;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 5*i;
    s1 = t[idt];  s2 = t[1+idt]; 
    s3 = t[2+idt];s4 = t[3+idt]; s5 = t[4+idt];
    while (nz--) {
      idx   = 5*(*vi++);
      x1    = t[idx];   x2 = t[1+idx];
      x3    = t[2+idx]; x4 = t[3+idx]; x5 = t[4+idx];
      s1 -= v[0]*x1 + v[5]*x2 + v[10]*x3 + v[15]*x4 + v[20]*x5;
      s2 -= v[1]*x1 + v[6]*x2 + v[11]*x3 + v[16]*x4 + v[21]*x5; 
      s3 -= v[2]*x1 + v[7]*x2 + v[12]*x3 + v[17]*x4 + v[22]*x5;
      s4 -= v[3]*x1 + v[8]*x2 + v[13]*x3 + v[18]*x4 + v[23]*x5;
      s5 -= v[4]*x1 + v[9]*x2 + v[14]*x3 + v[19]*x4 + v[24]*x5;
      v += 25;
    }
    idc = 5*(*c--);
    v   = aa + 25*diag[i];
    x[idc]   = t[idt]   = v[0]*s1+v[5]*s2+v[10]*s3+
                                 v[15]*s4+v[20]*s5;
    x[1+idc] = t[1+idt] = v[1]*s1+v[6]*s2+v[11]*s3+
                                 v[16]*s4+v[21]*s5;
    x[2+idc] = t[2+idt] = v[2]*s1+v[7]*s2+v[12]*s3+
                                 v[17]*s4+v[22]*s5;
    x[3+idc] = t[3+idt] = v[3]*s1+v[8]*s2+v[13]*s3+
                                 v[18]*s4+v[23]*s5;
    x[4+idc] = t[4+idt] = v[4]*s1+v[9]*s2+v[14]*s3+
                                 v[19]*s4+v[24]*s5;
  }

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*25*(a->nz) - 5*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_5_NaturalOrdering"
int MatSolve_SeqBAIJ_5_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a = (Mat_SeqBAIJ *)A->data;
  int             i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt;
  int             ierr,*diag = a->diag,jdx;
  MatScalar       *aa=a->a,*v;
  Scalar          *x,*b,s1,s2,s3,s4,s5,x1,x2,x3,x4,x5;;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr); 
  /* forward solve the lower triangular */
  idx    = 0;
  x[0] = b[idx]; x[1] = b[1+idx]; x[2] = b[2+idx]; x[3] = b[3+idx];x[4] = b[4+idx];
  for ( i=1; i<n; i++ ) {
    v     =  aa + 25*ai[i];
    vi    =  aj + ai[i];
    nz    =  diag[i] - ai[i];
    idx   =  5*i;
    s1  =  b[idx];s2 = b[1+idx];s3 = b[2+idx];s4 = b[3+idx];s5 = b[4+idx];
    while (nz--) {
      jdx   = 5*(*vi++);
      x1    = x[jdx];x2 = x[1+jdx];x3 = x[2+jdx];x4 = x[3+jdx];x5 = x[4+jdx];
      s1 -= v[0]*x1 + v[5]*x2 + v[10]*x3  + v[15]*x4 + v[20]*x5;
      s2 -= v[1]*x1 + v[6]*x2 + v[11]*x3  + v[16]*x4 + v[21]*x5;
      s3 -= v[2]*x1 + v[7]*x2 + v[12]*x3  + v[17]*x4 + v[22]*x5;
      s4 -= v[3]*x1 + v[8]*x2 + v[13]*x3  + v[18]*x4 + v[23]*x5;
      s5 -= v[4]*x1 + v[9]*x2 + v[14]*x3  + v[19]*x4 + v[24]*x5;
      v    += 25;
    }
    x[idx]   = s1;
    x[1+idx] = s2;
    x[2+idx] = s3;
    x[3+idx] = s4;
    x[4+idx] = s5;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 25*diag[i] + 25;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 5*i;
    s1 = x[idt];  s2 = x[1+idt]; 
    s3 = x[2+idt];s4 = x[3+idt]; s5 = x[4+idt];
    while (nz--) {
      idx   = 5*(*vi++);
      x1    = x[idx];   x2 = x[1+idx];x3    = x[2+idx]; x4 = x[3+idx]; x5 = x[4+idx];
      s1 -= v[0]*x1 + v[5]*x2 + v[10]*x3  + v[15]*x4 + v[20]*x5;
      s2 -= v[1]*x1 + v[6]*x2 + v[11]*x3  + v[16]*x4 + v[21]*x5;
      s3 -= v[2]*x1 + v[7]*x2 + v[12]*x3  + v[17]*x4 + v[22]*x5;
      s4 -= v[3]*x1 + v[8]*x2 + v[13]*x3  + v[18]*x4 + v[23]*x5;
      s5 -= v[4]*x1 + v[9]*x2 + v[14]*x3  + v[19]*x4 + v[24]*x5;
      v    += 25;
    }
    v        = aa + 25*diag[i];
    x[idt]   = v[0]*s1 + v[5]*s2 + v[10]*s3  + v[15]*s4 + v[20]*s5;
    x[1+idt] = v[1]*s1 + v[6]*s2 + v[11]*s3  + v[16]*s4 + v[21]*s5;
    x[2+idt] = v[2]*s1 + v[7]*s2 + v[12]*s3  + v[17]*s4 + v[22]*s5;
    x[3+idt] = v[3]*s1 + v[8]*s2 + v[13]*s3  + v[18]*s4 + v[23]*s5;
    x[4+idt] = v[4]*s1 + v[9]*s2 + v[14]*s3  + v[19]*s4 + v[24]*s5;
  }

  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr); 
  PLogFlops(2*25*(a->nz) - 5*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_4"
int MatSolve_SeqBAIJ_4(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a = (Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,idc,*rout,*cout;
  int             *diag = a->diag;
  MatScalar       *aa=a->a,*v;
  Scalar          *x,*b,s1,s2,s3,s4,x1,x2,x3,x4,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout + (n-1);

  /* forward solve the lower triangular */
  idx    = 4*(*r++); 
  t[0] = b[idx];   t[1] = b[1+idx]; 
  t[2] = b[2+idx]; t[3] = b[3+idx];
  for ( i=1; i<n; i++ ) {
    v     = aa + 16*ai[i];
    vi    = aj + ai[i];
    nz    = diag[i] - ai[i];
    idx   = 4*(*r++); 
    s1  = b[idx];s2 = b[1+idx];s3 = b[2+idx];s4 = b[3+idx];
    while (nz--) {
      idx   = 4*(*vi++);
      x1    = t[idx];x2 = t[1+idx];x3 = t[2+idx];x4 = t[3+idx];
      s1 -= v[0]*x1 + v[4]*x2 + v[8]*x3  + v[12]*x4;
      s2 -= v[1]*x1 + v[5]*x2 + v[9]*x3  + v[13]*x4;
      s3 -= v[2]*x1 + v[6]*x2 + v[10]*x3 + v[14]*x4;
      s4 -= v[3]*x1 + v[7]*x2 + v[11]*x3 + v[15]*x4;
      v    += 16;
    }
    idx        = 4*i;
    t[idx]   = s1;t[1+idx] = s2;
    t[2+idx] = s3;t[3+idx] = s4;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 16*diag[i] + 16;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 4*i;
    s1 = t[idt];  s2 = t[1+idt]; 
    s3 = t[2+idt];s4 = t[3+idt];
    while (nz--) {
      idx   = 4*(*vi++);
      x1    = t[idx];   x2 = t[1+idx];
      x3    = t[2+idx]; x4 = t[3+idx];
      s1 -= v[0]*x1 + v[4]*x2 + v[8]*x3   + v[12]*x4;
      s2 -= v[1]*x1 + v[5]*x2 + v[9]*x3   + v[13]*x4; 
      s3 -= v[2]*x1 + v[6]*x2 + v[10]*x3  + v[14]*x4;
      s4 -= v[3]*x1 + v[7]*x2 + v[11]*x3  + v[15]*x4;
      v += 16;
    }
    idc      = 4*(*c--);
    v        = aa + 16*diag[i];
    x[idc]   = t[idt]   = v[0]*s1+v[4]*s2+v[8]*s3+v[12]*s4;
    x[1+idc] = t[1+idt] = v[1]*s1+v[5]*s2+v[9]*s3+v[13]*s4;
    x[2+idc] = t[2+idt] = v[2]*s1+v[6]*s2+v[10]*s3+v[14]*s4;
    x[3+idc] = t[3+idt] = v[3]*s1+v[7]*s2+v[11]*s3+v[15]*s4;
  }

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*16*(a->nz) - 4*a->n);
  PetscFunctionReturn(0);
}


/*
      Special case where the matrix was ILU(0) factored in the natural
   ordering. This eliminates the need for the column and row permutation.
*/
#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_4_NaturalOrdering"
int MatSolve_SeqBAIJ_4_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a = (Mat_SeqBAIJ *)A->data;
  int             n=a->mbs,*ai=a->i,*aj=a->j;
  int             ierr,*diag = a->diag;
  MatScalar       *aa=a->a;
  Scalar          *x,*b;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr); 

#if defined(PETSC_USE_FORTRAN_KERNEL_SOLVEBAIJBLAS)
  {
    static Scalar w[2000]; /* very BAD need to fix */
    fortransolvebaij4blas_(&n,x,ai,aj,diag,aa,b,w);
  }
#elif defined(PETSC_USE_FORTRAN_KERNEL_SOLVEBAIJ)
  {
    static Scalar w[2000]; /* very BAD need to fix */
    fortransolvebaij4_(&n,x,ai,aj,diag,aa,b,w);
  }
#elif defined(PETSC_USE_FORTRAN_KERNEL_SOLVEBAIJUNROLL)
  fortransolvebaij4unroll_(&n,x,ai,aj,diag,aa,b);
#else
  {
    Scalar    s1,s2,s3,s4,x1,x2,x3,x4;
    MatScalar *v;
    int       jdx,idt,idx,nz,*vi,i;

  /* forward solve the lower triangular */
  idx    = 0;
  x[0]   = b[0]; x[1] = b[1]; x[2] = b[2]; x[3] = b[3];
  for ( i=1; i<n; i++ ) {
    v     =  aa      + 16*ai[i];
    vi    =  aj      + ai[i];
    nz    =  diag[i] - ai[i];
    idx   +=  4;
    s1  =  b[idx];s2 = b[1+idx];s3 = b[2+idx];s4 = b[3+idx];
    while (nz--) {
      jdx   = 4*(*vi++);
      x1    = x[jdx];x2 = x[1+jdx];x3 = x[2+jdx];x4 = x[3+jdx];
      s1 -= v[0]*x1 + v[4]*x2 + v[8]*x3  + v[12]*x4;
      s2 -= v[1]*x1 + v[5]*x2 + v[9]*x3  + v[13]*x4;
      s3 -= v[2]*x1 + v[6]*x2 + v[10]*x3 + v[14]*x4;
      s4 -= v[3]*x1 + v[7]*x2 + v[11]*x3 + v[15]*x4;
      v    += 16;
    }
    x[idx]   = s1;
    x[1+idx] = s2;
    x[2+idx] = s3;
    x[3+idx] = s4;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 16*diag[i] + 16;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 4*i;
    s1 = x[idt];  s2 = x[1+idt]; 
    s3 = x[2+idt];s4 = x[3+idt];
    while (nz--) {
      idx   = 4*(*vi++);
      x1    = x[idx];   x2 = x[1+idx];x3    = x[2+idx]; x4 = x[3+idx];
      s1 -= v[0]*x1 + v[4]*x2 + v[8]*x3   + v[12]*x4;
      s2 -= v[1]*x1 + v[5]*x2 + v[9]*x3   + v[13]*x4; 
      s3 -= v[2]*x1 + v[6]*x2 + v[10]*x3  + v[14]*x4;
      s4 -= v[3]*x1 + v[7]*x2 + v[11]*x3  + v[15]*x4;
      v    += 16;
    }
    v        = aa + 16*diag[i];
    x[idt]   = v[0]*s1 + v[4]*s2 + v[8]*s3  + v[12]*s4;
    x[1+idt] = v[1]*s1 + v[5]*s2 + v[9]*s3  + v[13]*s4;
    x[2+idt] = v[2]*s1 + v[6]*s2 + v[10]*s3 + v[14]*s4;
    x[3+idt] = v[3]*s1 + v[7]*s2 + v[11]*s3 + v[15]*s4;
  }
  }
#endif

  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr); 
  PLogFlops(2*16*(a->nz) - 4*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_3"
int MatSolve_SeqBAIJ_3(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,idc,*rout,*cout;
  int             *diag = a->diag;
  MatScalar       *aa=a->a,*v;
  Scalar          *x,*b,s1,s2,s3,x1,x2,x3,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout + (n-1);

  /* forward solve the lower triangular */
  idx    = 3*(*r++); 
  t[0] = b[idx]; t[1] = b[1+idx]; t[2] = b[2+idx];
  for ( i=1; i<n; i++ ) {
    v     = aa + 9*ai[i];
    vi    = aj + ai[i];
    nz    = diag[i] - ai[i];
    idx   = 3*(*r++); 
    s1  = b[idx]; s2 = b[1+idx]; s3 = b[2+idx];
    while (nz--) {
      idx   = 3*(*vi++);
      x1    = t[idx]; x2 = t[1+idx]; x3 = t[2+idx];
      s1 -= v[0]*x1 + v[3]*x2 + v[6]*x3;
      s2 -= v[1]*x1 + v[4]*x2 + v[7]*x3;
      s3 -= v[2]*x1 + v[5]*x2 + v[8]*x3;
      v += 9;
    }
    idx = 3*i;
    t[idx] = s1; t[1+idx] = s2; t[2+idx] = s3;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 9*diag[i] + 9;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 3*i;
    s1 = t[idt]; s2 = t[1+idt]; s3 = t[2+idt];
    while (nz--) {
      idx   = 3*(*vi++);
      x1    = t[idx]; x2 = t[1+idx]; x3 = t[2+idx];
      s1 -= v[0]*x1 + v[3]*x2 + v[6]*x3;
      s2 -= v[1]*x1 + v[4]*x2 + v[7]*x3;
      s3 -= v[2]*x1 + v[5]*x2 + v[8]*x3;
      v += 9;
    }
    idc = 3*(*c--);
    v   = aa + 9*diag[i];
    x[idc]   = t[idt]   = v[0]*s1 + v[3]*s2 + v[6]*s3;
    x[1+idc] = t[1+idt] = v[1]*s1 + v[4]*s2 + v[7]*s3;
    x[2+idc] = t[2+idt] = v[2]*s1 + v[5]*s2 + v[8]*s3;
  }
  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr); 
  PLogFlops(2*9*(a->nz) - 3*a->n);
  PetscFunctionReturn(0);
}

/*
      Special case where the matrix was ILU(0) factored in the natural
   ordering. This eliminates the need for the column and row permutation.
*/
#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_3_NaturalOrdering"
int MatSolve_SeqBAIJ_3_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a = (Mat_SeqBAIJ *)A->data;
  int             n=a->mbs,*ai=a->i,*aj=a->j;
  int             ierr,*diag = a->diag;
  MatScalar       *aa=a->a, *v;
  Scalar          *x,*b,s1,s2,s3,x1,x2,x3;
  int             jdx,idt,idx,nz,*vi,i;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr); 


  /* forward solve the lower triangular */
  idx    = 0;
  x[0]   = b[0]; x[1] = b[1]; x[2] = b[2];
  for ( i=1; i<n; i++ ) {
    v     =  aa      + 9*ai[i];
    vi    =  aj      + ai[i];
    nz    =  diag[i] - ai[i];
    idx   +=  3;
    s1  =  b[idx];s2 = b[1+idx];s3 = b[2+idx];
    while (nz--) {
      jdx   = 3*(*vi++);
      x1    = x[jdx];x2 = x[1+jdx];x3 = x[2+jdx];
      s1 -= v[0]*x1 + v[3]*x2 + v[6]*x3;
      s2 -= v[1]*x1 + v[4]*x2 + v[7]*x3;
      s3 -= v[2]*x1 + v[5]*x2 + v[8]*x3;
      v    += 9;
    }
    x[idx]   = s1;
    x[1+idx] = s2;
    x[2+idx] = s3;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 9*diag[i] + 9;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 3*i;
    s1 = x[idt];  s2 = x[1+idt]; 
    s3 = x[2+idt];
    while (nz--) {
      idx   = 3*(*vi++);
      x1    = x[idx];   x2 = x[1+idx];x3    = x[2+idx];
      s1 -= v[0]*x1 + v[3]*x2 + v[6]*x3;
      s2 -= v[1]*x1 + v[4]*x2 + v[7]*x3;
      s3 -= v[2]*x1 + v[5]*x2 + v[8]*x3;
      v    += 9;
    }
    v        = aa +  9*diag[i];
    x[idt]   = v[0]*s1 + v[3]*s2 + v[6]*s3;
    x[1+idt] = v[1]*s1 + v[4]*s2 + v[7]*s3;
    x[2+idt] = v[2]*s1 + v[5]*s2 + v[8]*s3;
  }

  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr); 
  PLogFlops(2*9*(a->nz) - 3*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_2"
int MatSolve_SeqBAIJ_2(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,idx,idt,idc,*rout,*cout;
  int             *diag = a->diag;
  MatScalar       *aa=a->a,*v;
  Scalar          *x,*b,s1,s2,x1,x2,*t;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout + (n-1);

  /* forward solve the lower triangular */
  idx    = 2*(*r++); 
  t[0] = b[idx]; t[1] = b[1+idx];
  for ( i=1; i<n; i++ ) {
    v     = aa + 4*ai[i];
    vi    = aj + ai[i];
    nz    = diag[i] - ai[i];
    idx   = 2*(*r++); 
    s1  = b[idx]; s2 = b[1+idx];
    while (nz--) {
      idx   = 2*(*vi++);
      x1    = t[idx]; x2 = t[1+idx];
      s1 -= v[0]*x1 + v[2]*x2;
      s2 -= v[1]*x1 + v[3]*x2;
      v += 4;
    }
    idx = 2*i;
    t[idx] = s1; t[1+idx] = s2;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 4*diag[i] + 4;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 2*i;
    s1 = t[idt]; s2 = t[1+idt];
    while (nz--) {
      idx   = 2*(*vi++);
      x1    = t[idx]; x2 = t[1+idx];
      s1 -= v[0]*x1 + v[2]*x2;
      s2 -= v[1]*x1 + v[3]*x2;
      v += 4;
    }
    idc = 2*(*c--);
    v   = aa + 4*diag[i];
    x[idc]   = t[idt]   = v[0]*s1 + v[2]*s2;
    x[1+idc] = t[1+idt] = v[1]*s1 + v[3]*s2;
  }
  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr); 
  PLogFlops(2*4*(a->nz) - 2*a->n);
  PetscFunctionReturn(0);
}

/*
      Special case where the matrix was ILU(0) factored in the natural
   ordering. This eliminates the need for the column and row permutation.
*/
#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_2_NaturalOrdering"
int MatSolve_SeqBAIJ_2_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a = (Mat_SeqBAIJ *)A->data;
  int             n=a->mbs,*ai=a->i,*aj=a->j;
  int             ierr,*diag = a->diag;
  MatScalar       *aa=a->a,*v;
  Scalar          *x,*b,s1,s2,x1,x2;
  int             jdx,idt,idx,nz,*vi,i;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr); 

  /* forward solve the lower triangular */
  idx    = 0;
  x[0]   = b[0]; x[1] = b[1];
  for ( i=1; i<n; i++ ) {
    v     =  aa      + 4*ai[i];
    vi    =  aj      + ai[i];
    nz    =  diag[i] - ai[i];
    idx   +=  2;
    s1  =  b[idx];s2 = b[1+idx];
    while (nz--) {
      jdx   = 2*(*vi++);
      x1    = x[jdx];x2 = x[1+jdx];
      s1 -= v[0]*x1 + v[2]*x2;
      s2 -= v[1]*x1 + v[3]*x2;
      v    += 4;
    }
    x[idx]   = s1;
    x[1+idx] = s2;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + 4*diag[i] + 4;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = 2*i;
    s1 = x[idt];  s2 = x[1+idt]; 
    while (nz--) {
      idx   = 2*(*vi++);
      x1    = x[idx];   x2 = x[1+idx];
      s1 -= v[0]*x1 + v[2]*x2;
      s2 -= v[1]*x1 + v[3]*x2;
      v    += 4;
    }
    v        = aa +  4*diag[i];
    x[idt]   = v[0]*s1 + v[2]*s2;
    x[1+idt] = v[1]*s1 + v[3]*s2;
  }

  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr); 
  PLogFlops(2*4*(a->nz) - 2*a->n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_1"
int MatSolve_SeqBAIJ_1(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a=(Mat_SeqBAIJ *)A->data;
  IS              iscol=a->col,isrow=a->row;
  int             *r,*c,ierr,i,n=a->mbs,*vi,*ai=a->i,*aj=a->j,nz,*rout,*cout;
  int             *diag = a->diag;
  MatScalar       *aa=a->a,*v;
  Scalar          *x,*b,s1,*t;

  PetscFunctionBegin;
  if (!n) PetscFunctionReturn(0);

  ierr = VecGetArray(bb,&b);CHKERRQ(ierr);
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr);
  t  = a->solve_work;

  ierr = ISGetIndices(isrow,&rout);CHKERRQ(ierr); r = rout;
  ierr = ISGetIndices(iscol,&cout);CHKERRQ(ierr); c = cout + (n-1);

  /* forward solve the lower triangular */
  t[0] = b[*r++];
  for ( i=1; i<n; i++ ) {
    v     = aa + ai[i];
    vi    = aj + ai[i];
    nz    = diag[i] - ai[i];
    s1  = b[*r++];
    while (nz--) {
      s1 -= (*v++)*t[*vi++];
    }
    t[i] = s1;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + diag[i] + 1;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    s1 = t[i];
    while (nz--) {
      s1 -= (*v++)*t[*vi++];
    }
    x[*c--] = t[i] = aa[diag[i]]*s1;
  }

  ierr = ISRestoreIndices(isrow,&rout);CHKERRQ(ierr);
  ierr = ISRestoreIndices(iscol,&cout);CHKERRQ(ierr);
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr);
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr);
  PLogFlops(2*1*(a->nz) - a->n);
  PetscFunctionReturn(0);
}
/*
      Special case where the matrix was ILU(0) factored in the natural
   ordering. This eliminates the need for the column and row permutation.
*/
#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqBAIJ_1_NaturalOrdering"
int MatSolve_SeqBAIJ_1_NaturalOrdering(Mat A,Vec bb,Vec xx)
{
  Mat_SeqBAIJ     *a = (Mat_SeqBAIJ *)A->data;
  int             n=a->mbs,*ai=a->i,*aj=a->j;
  int             ierr,*diag = a->diag;
  MatScalar       *aa=a->a;
  Scalar          *x,*b;
  Scalar          s1,x1;
  MatScalar       *v;
  int             jdx,idt,idx,nz,*vi,i;

  PetscFunctionBegin;
  ierr = VecGetArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecGetArray(xx,&x);CHKERRQ(ierr); 

  /* forward solve the lower triangular */
  idx    = 0;
  x[0]   = b[0];
  for ( i=1; i<n; i++ ) {
    v     =  aa      + ai[i];
    vi    =  aj      + ai[i];
    nz    =  diag[i] - ai[i];
    idx   +=  1;
    s1  =  b[idx];
    while (nz--) {
      jdx   = *vi++;
      x1    = x[jdx];
      s1 -= v[0]*x1;
      v    += 1;
    }
    x[idx]   = s1;
  }
  /* backward solve the upper triangular */
  for ( i=n-1; i>=0; i-- ){
    v    = aa + diag[i] + 1;
    vi   = aj + diag[i] + 1;
    nz   = ai[i+1] - diag[i] - 1;
    idt  = i;
    s1 = x[idt]; 
    while (nz--) {
      idx   = *vi++;
      x1    = x[idx];
      s1 -= v[0]*x1;
      v    += 1;
    }
    v        = aa +  diag[i];
    x[idt]   = v[0]*s1;
  }
  ierr = VecRestoreArray(bb,&b);CHKERRQ(ierr); 
  ierr = VecRestoreArray(xx,&x);CHKERRQ(ierr); 
  PLogFlops(2*(a->nz) - a->n);
  PetscFunctionReturn(0);
}

/* ----------------------------------------------------------------*/
/*
     This code is virtually identical to MatILUFactorSymbolic_SeqAIJ
   except that the data structure of Mat_SeqAIJ is slightly different.
   Not a good example of code reuse.
*/
extern int MatMissingDiagonal_SeqBAIJ(Mat);

#undef __FUNC__  
#define __FUNC__ "MatILUFactorSymbolic_SeqBAIJ"
int MatILUFactorSymbolic_SeqBAIJ(Mat A,IS isrow,IS iscol,MatILUInfo *info,Mat *fact)
{
  Mat_SeqBAIJ *a = (Mat_SeqBAIJ *) A->data, *b;
  IS          isicol;
  int         *r,*ic, ierr, prow, n = a->mbs, *ai = a->i, *aj = a->j;
  int         *ainew,*ajnew, jmax,*fill, *xi, nz, *im,*ajfill,*flev;
  int         *dloc, idx, row,m,fm, nzf, nzi,len,  realloc = 0, dcount = 0;
  int         incrlev,nnz,i,bs = a->bs,bs2 = a->bs2, levels, diagonal_fill;
  PetscTruth  col_identity, row_identity;
  double      f;

  PetscFunctionBegin;
  if (info) {
    f             = info->fill;
    levels        = (int) info->levels;
    diagonal_fill = (int) info->diagonal_fill;
  } else {
    f             = 1.0;
    levels        = 0;
    diagonal_fill = 0;
  }
  ierr = ISInvertPermutation(iscol,&isicol);CHKERRQ(ierr);

  /* special case that simply copies fill pattern */
  PetscValidHeaderSpecific(isrow,IS_COOKIE);
  PetscValidHeaderSpecific(iscol,IS_COOKIE);
  ierr = ISIdentity(isrow,&row_identity);CHKERRQ(ierr);
  ierr = ISIdentity(iscol,&col_identity);CHKERRQ(ierr);
  if (levels == 0 && row_identity && col_identity) {
    ierr = MatDuplicate_SeqBAIJ(A,MAT_DO_NOT_COPY_VALUES,fact);CHKERRQ(ierr);
    (*fact)->factor = FACTOR_LU;
    b               = (Mat_SeqBAIJ *) (*fact)->data;
    if (!b->diag) {
      ierr = MatMarkDiagonal_SeqBAIJ(*fact);CHKERRQ(ierr);
    }
    ierr = MatMissingDiagonal_SeqBAIJ(*fact);CHKERRQ(ierr);
    b->row        = isrow;
    b->col        = iscol;
    ierr          = PetscObjectReference((PetscObject)isrow);CHKERRQ(ierr);
    ierr          = PetscObjectReference((PetscObject)iscol);CHKERRQ(ierr);
    b->icol       = isicol;
    b->solve_work = (Scalar *) PetscMalloc((b->m+1+b->bs)*sizeof(Scalar));CHKPTRQ(b->solve_work);
   /*
        Blocksize 2, 3, 4, 5, 6 and 7 have a special faster factorization/solver 
        for ILU(0) factorization with natural ordering
   */
    switch (b->bs) {
    case 2:
      (*fact)->ops->lufactornumeric = MatLUFactorNumeric_SeqBAIJ_2_NaturalOrdering;
      (*fact)->ops->solve           = MatSolve_SeqBAIJ_2_NaturalOrdering;
      PLogInfo(A,"MatILUFactorSymbolic_SeqBAIJ:Using special in-place natural ordering factor and solve BS=2\n");
      break;
    case 3:
      (*fact)->ops->lufactornumeric = MatLUFactorNumeric_SeqBAIJ_3_NaturalOrdering;
      (*fact)->ops->solve           = MatSolve_SeqBAIJ_3_NaturalOrdering;
      PLogInfo(A,"MatILUFactorSymbolic_SeqBAIJ:sing special in-place natural ordering factor and solve BS=3\n");
      break; 
    case 4:
      (*fact)->ops->lufactornumeric = MatLUFactorNumeric_SeqBAIJ_4_NaturalOrdering;
      (*fact)->ops->solve           = MatSolve_SeqBAIJ_4_NaturalOrdering;
      PLogInfo(A,"MatILUFactorSymbolic_SeqBAIJ:Using special in-place natural ordering factor and solve BS=4\n"); 
      break;
    case 5:
      (*fact)->ops->lufactornumeric = MatLUFactorNumeric_SeqBAIJ_5_NaturalOrdering;
      (*fact)->ops->solve           = MatSolve_SeqBAIJ_5_NaturalOrdering;
      PLogInfo(A,"MatILUFactorSymbolic_SeqBAIJ:Using special in-place natural ordering factor and solve BS=5\n"); 
      break;
    case 6: 
      (*fact)->ops->lufactornumeric = MatLUFactorNumeric_SeqBAIJ_6_NaturalOrdering;
      (*fact)->ops->solve           = MatSolve_SeqBAIJ_6_NaturalOrdering;
      PLogInfo(A,"MatILUFactorSymbolic_SeqBAIJ:Using special in-place natural ordering factor and solve BS=6\n");
      break; 
    case 7:
      (*fact)->ops->lufactornumeric = MatLUFactorNumeric_SeqBAIJ_7_NaturalOrdering;
      (*fact)->ops->solve           = MatSolve_SeqBAIJ_7_NaturalOrdering;
      PLogInfo(A,"MatILUFactorSymbolic_SeqBAIJ:Using special in-place natural ordering factor and solve BS=7\n");
    break; 
  }
    PetscFunctionReturn(0);
  }

  ierr = ISGetIndices(isrow,&r);CHKERRQ(ierr);
  ierr = ISGetIndices(isicol,&ic);CHKERRQ(ierr);

  /* get new row pointers */
  ainew = (int *) PetscMalloc( (n+1)*sizeof(int) );CHKPTRQ(ainew);
  ainew[0] = 0;
  /* don't know how many column pointers are needed so estimate */
  jmax = (int) (f*ai[n] + 1);
  ajnew = (int *) PetscMalloc( (jmax)*sizeof(int) );CHKPTRQ(ajnew);
  /* ajfill is level of fill for each fill entry */
  ajfill = (int *) PetscMalloc( (jmax)*sizeof(int) );CHKPTRQ(ajfill);
  /* fill is a linked list of nonzeros in active row */
  fill = (int *) PetscMalloc( (n+1)*sizeof(int));CHKPTRQ(fill);
  /* im is level for each filled value */
  im = (int *) PetscMalloc( (n+1)*sizeof(int));CHKPTRQ(im);
  /* dloc is location of diagonal in factor */
  dloc = (int *) PetscMalloc( (n+1)*sizeof(int));CHKPTRQ(dloc);
  dloc[0]  = 0;
  for ( prow=0; prow<n; prow++ ) {

    /* copy prow into linked list */
    nzf        = nz  = ai[r[prow]+1] - ai[r[prow]];
    if (!nz) SETERRQ(PETSC_ERR_MAT_LU_ZRPVT,1,"Empty row in matrix");
    xi         = aj + ai[r[prow]];
    fill[n]    = n;
    fill[prow] = -1; /* marker for diagonal entry */
    while (nz--) {
      fm  = n;
      idx = ic[*xi++];
      do {
        m  = fm;
        fm = fill[m];
      } while (fm < idx);
      fill[m]   = idx;
      fill[idx] = fm;
      im[idx]   = 0;
    }

    /* make sure diagonal entry is included */
    if (diagonal_fill && fill[prow] == -1) {
      fm = n;
      while (fill[fm] < prow) fm = fill[fm];
      fill[prow] = fill[fm];  /* insert diagonal into linked list */
      fill[fm]   = prow;
      im[prow]   = 0;
      nzf++;
      dcount++;
    }

    nzi = 0;
    row = fill[n];
    while ( row < prow ) {
      incrlev = im[row] + 1;
      nz      = dloc[row];
      xi      = ajnew  + ainew[row] + nz + 1;
      flev    = ajfill + ainew[row] + nz + 1;
      nnz     = ainew[row+1] - ainew[row] - nz - 1;
      fm      = row;
      while (nnz-- > 0) {
        idx = *xi++;
        if (*flev + incrlev > levels) {
          flev++;
          continue;
        }
        do {
          m  = fm;
          fm = fill[m];
        } while (fm < idx);
        if (fm != idx) {
          im[idx]   = *flev + incrlev;
          fill[m]   = idx;
          fill[idx] = fm;
          fm        = idx;
          nzf++;
        } else {
          if (im[idx] > *flev + incrlev) im[idx] = *flev+incrlev;
        }
        flev++;
      }
      row = fill[row];
      nzi++;
    }
    /* copy new filled row into permanent storage */
    ainew[prow+1] = ainew[prow] + nzf;
    if (ainew[prow+1] > jmax) {

      /* estimate how much additional space we will need */
      /* use the strategy suggested by David Hysom <hysom@perch-t.icase.edu> */
      /* just double the memory each time */
      int maxadd = jmax;
      /* maxadd = (int) (((f*ai[n]+1)*(n-prow+5))/n); */
      if (maxadd < nzf) maxadd = (n-prow)*(nzf+1);
      jmax += maxadd;

      /* allocate a longer ajnew and ajfill */
      xi = (int *) PetscMalloc( jmax*sizeof(int) );CHKPTRQ(xi);
      ierr = PetscMemcpy(xi,ajnew,ainew[prow]*sizeof(int));CHKERRQ(ierr);
      ierr = PetscFree(ajnew);CHKERRQ(ierr);
      ajnew = xi;
      xi = (int *) PetscMalloc( jmax*sizeof(int) );CHKPTRQ(xi);
      ierr = PetscMemcpy(xi,ajfill,ainew[prow]*sizeof(int));CHKERRQ(ierr);
      ierr = PetscFree(ajfill);CHKERRQ(ierr);
      ajfill = xi;
      realloc++; /* count how many reallocations are needed */
    }
    xi          = ajnew + ainew[prow];
    flev        = ajfill + ainew[prow];
    dloc[prow]  = nzi;
    fm          = fill[n];
    while (nzf--) {
      *xi++   = fm;
      *flev++ = im[fm];
      fm      = fill[fm];
    }
    /* make sure row has diagonal entry */
    if (ajnew[ainew[prow]+dloc[prow]] != prow) {
      SETERRQ1(PETSC_ERR_MAT_LU_ZRPVT,1,"Row %d has missing diagonal in factored matrix\n\
    try running with -pc_ilu_nonzeros_along_diagonal or -pc_ilu_diagonal_fill",prow);
    }
  }
  ierr = PetscFree(ajfill);CHKERRQ(ierr);
  ierr = ISRestoreIndices(isrow,&r);CHKERRQ(ierr);
  ierr = ISRestoreIndices(isicol,&ic);CHKERRQ(ierr);
  ierr = PetscFree(fill);CHKERRQ(ierr);
  ierr = PetscFree(im);CHKERRQ(ierr);

  {
    double af = ((double)ainew[n])/((double)ai[n]);
    PLogInfo(A,"MatILUFactorSymbolic_SeqBAIJ:Reallocs %d Fill ratio:given %g needed %g\n",realloc,f,af);
    PLogInfo(A,"MatILUFactorSymbolic_SeqBAIJ:Run with -pc_ilu_fill %g or use \n",af);
    PLogInfo(A,"MatILUFactorSymbolic_SeqBAIJ:PCILUSetFill(pc,%g);\n",af);
    PLogInfo(A,"MatILUFactorSymbolic_SeqBAIJ:for best performance.\n");
    if (diagonal_fill) {
      PLogInfo(A,"MatILUFactorSymbolic_SeqBAIJ:Detected and replace %d missing diagonals",dcount);
    }
  }

  /* put together the new matrix */
  ierr = MatCreateSeqBAIJ(A->comm,bs,bs*n,bs*n,0,PETSC_NULL,fact);CHKERRQ(ierr);
  PLogObjectParent(*fact,isicol);
  b = (Mat_SeqBAIJ *) (*fact)->data;
  ierr = PetscFree(b->imax);CHKERRQ(ierr);
  b->singlemalloc = PETSC_FALSE;
  len = bs2*ainew[n]*sizeof(MatScalar);
  /* the next line frees the default space generated by the Create() */
  ierr = PetscFree(b->a);CHKERRQ(ierr);
  ierr = PetscFree(b->ilen);CHKERRQ(ierr);
  b->a          = (MatScalar *) PetscMalloc( len );CHKPTRQ(b->a);
  b->j          = ajnew;
  b->i          = ainew;
  for ( i=0; i<n; i++ ) dloc[i] += ainew[i];
  b->diag       = dloc;
  b->ilen       = 0;
  b->imax       = 0;
  b->row        = isrow;
  b->col        = iscol;
  ierr          = PetscObjectReference((PetscObject)isrow);CHKERRQ(ierr);
  ierr          = PetscObjectReference((PetscObject)iscol);CHKERRQ(ierr);
  b->icol       = isicol;
  b->solve_work = (Scalar *) PetscMalloc( (bs*n+bs)*sizeof(Scalar));CHKPTRQ(b->solve_work);
  /* In b structure:  Free imax, ilen, old a, old j.  
     Allocate dloc, solve_work, new a, new j */
  PLogObjectMemory(*fact,(ainew[n]-n)*(sizeof(int))+bs2*ainew[n]*sizeof(Scalar));
  b->maxnz          = b->nz = ainew[n];
  (*fact)->factor   = FACTOR_LU;

  (*fact)->info.factor_mallocs    = realloc;
  (*fact)->info.fill_ratio_given  = f;
  (*fact)->info.fill_ratio_needed = ((double)ainew[n])/((double)ai[prow]);

  PetscFunctionReturn(0); 
}




