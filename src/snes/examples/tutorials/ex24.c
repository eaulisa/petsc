/*$Id: ex24.c,v 1.1 2001/01/02 21:42:39 bsmith Exp bsmith $*/

static char help[] = "Solves PDE optimization problem of ex22.c with finite differences for adjoint\n\n";

#include "petscda.h"
#include "petscpf.h"
#include "petscsnes.h"

/*

       w - design variables (what we change to get an optimal solution)
       u - state variables (i.e. the PDE solution)
       lambda - the Lagrange multipliers

            U = (w u lambda)

       fu, fw, flambda contain the gradient of L(w,u,lambda)

            FU = (fw fu flambda)

       In this example the PDE is 
                             Uxx = 2, 
                            u(0) = w(0), thus this is the free parameter
                            u(1) = 0
       the function we wish to minimize is 
                            \integral u^{2}

       The exact solution for u is given by u(x) = x*x - 1.25*x + .25

       Use the usual centered finite differences.

       Note we treat the problem as non-linear though it happens to be linear

       The lambda and u are NOT interlaced.
*/

extern int FormFunction(SNES,Vec,Vec,void*);
extern int PDEFormFunction(SNES,Vec,Vec,void*);

#undef __FUNC__
#define __FUNC__ "main"
int main(int argc,char **argv)
{
  int     ierr,N = 5;
  DA      da;
  DMMG    *dmmg;
  VecPack packer;

  PetscInitialize(&argc,&argv,PETSC_NULL,help);
  ierr = OptionsGetInt(PETSC_NULL,"-N",&N,PETSC_NULL);CHKERRQ(ierr);

  /* Hardwire several options; can be changed at command line */
  ierr = OptionsSetValue("-dmmg_grid_sequence",PETSC_NULL);CHKERRQ(ierr);
  ierr = OptionsSetValue("-ksp_type","fgmres");CHKERRQ(ierr);
  ierr = OptionsSetValue("-ksp_max_it","5");CHKERRQ(ierr);
  ierr = OptionsSetValue("-pc_mg_type","full");CHKERRQ(ierr);
  ierr = OptionsSetValue("-mg_coarse_ksp_type","gmres");CHKERRQ(ierr);
  ierr = OptionsSetValue("-mg_levels_ksp_type","gmres");CHKERRQ(ierr);
  ierr = OptionsSetValue("-mg_coarse_ksp_max_it","6");CHKERRQ(ierr);
  ierr = OptionsSetValue("-mg_levels_ksp_max_it","3");CHKERRQ(ierr);
  ierr = OptionsSetValue("-snes_mf_type","wp");CHKERRQ(ierr);
  ierr = OptionsSetValue("-snes_mf_compute_norma","no");CHKERRQ(ierr);
  ierr = OptionsSetValue("-snes_mf_compute_normu","no");CHKERRQ(ierr);
  ierr = OptionsSetValue("-snes_eq_ls","basicnonorms");CHKERRQ(ierr);
  ierr = OptionsInsert(&argc,&argv,PETSC_NULL);CHKERRQ(ierr); 
  
  /* Create a global vector from a da arrays */
  ierr = DACreate1d(PETSC_COMM_WORLD,DA_NONPERIODIC,N,1,1,PETSC_NULL,&da);CHKERRQ(ierr);
  ierr = VecPackCreate(PETSC_COMM_WORLD,&packer);CHKERRQ(ierr);
  ierr = VecPackAddArray(packer,1);CHKERRQ(ierr);
  ierr = VecPackAddDA(packer,da);CHKERRQ(ierr);
  ierr = VecPackAddDA(packer,da);CHKERRQ(ierr);

  /* create nonlinear multi-level solver */
  ierr = DMMGCreate(PETSC_COMM_WORLD,2,PETSC_NULL,&dmmg);CHKERRQ(ierr);
  ierr = DMMGSetUseMatrixFree(dmmg);CHKERRQ(ierr);
  ierr = DMMGSetDM(dmmg,(DM)packer);CHKERRQ(ierr);
  ierr = DMMGSetSNES(dmmg,FormFunction,PETSC_NULL);CHKERRQ(ierr);
  ierr = DMMGSolve(dmmg);CHKERRQ(ierr);
  ierr = DMMGDestroy(dmmg);CHKERRQ(ierr);

  ierr = DADestroy(da);CHKERRQ(ierr);
  ierr = VecPackDestroy(packer);CHKERRQ(ierr);
  PetscFinalize();
  return 0;
}
 
/*
      Evaluates FU = Gradiant(L(w,u,lambda))

     This local function acts on the ghosted version of U (accessed via VecPackGetLocalVectors() and
   VecPackScatter()) BUT the global, nonghosted version of FU (via VecPackAccess()).

     This function uses PDEFormFunction() to enforce the PDE constraint equations and its adjoint
   for the Lagrange multiplier equations

*/
int FormFunction(SNES snes,Vec U,Vec FU,void* dummy)
{
  DMMG    dmmg = (DMMG)dummy;
  int     ierr,xs,xm,i,N,nredundant;
  Scalar  *u,*w,*fw,*fu,*lambda,*flambda,d,h;
  Vec     vu,vlambda,vfu,vflambda;
  DA      da,dadummy;
  VecPack packer = (VecPack)dmmg->dm;

  PetscFunctionBegin;
  ierr = VecPackGetEntries(packer,&nredundant,&da,&dadummy);CHKERRQ(ierr);
  ierr = VecPackGetLocalVectors(packer,&w,&vu,&vlambda);CHKERRQ(ierr);
  ierr = VecPackScatter(packer,U,w,vu,vlambda);CHKERRQ(ierr);
  ierr = VecPackAccess(packer,FU,&fw,&vfu,&vflambda);CHKERRQ(ierr);

  ierr = DAGetCorners(da,&xs,PETSC_NULL,PETSC_NULL,&xm,PETSC_NULL,PETSC_NULL);CHKERRQ(ierr);
  ierr = DAGetInfo(da,0,&N,0,0,0,0,0,0,0,0,0);CHKERRQ(ierr);
  ierr = DAVecGetArray(da,vu,(void**)&u);CHKERRQ(ierr);
  ierr = DAVecGetArray(da,vfu,(void**)&fu);CHKERRQ(ierr);
  ierr = DAVecGetArray(da,vlambda,(void**)&lambda);CHKERRQ(ierr);
  ierr = DAVecGetArray(da,vflambda,(void**)&flambda);CHKERRQ(ierr);
  d    = (N-1.0);
  h    = 1.0/d;

  /* derivative of L() w.r.t. w */
  if (xs == 0) { /* only first processor computes this */
    fw[0] = -2.*d*lambda[0];
  }

  /* derivative of L() w.r.t. u */
  for (i=xs; i<xs+xm; i++) {
    if      (i == 0)   flambda[0]   = (2.*h*u[0]   + 2.*d*lambda[0]   + d*lambda[1]);
    else if (i == 1)   flambda[1]   = -(2.*h*u[1]   - 2.*d*lambda[1]   + d*lambda[2]);
    else if (i == N-1) flambda[N-1] = (2.*h*u[N-1] + 2.*d*lambda[N-1] + d*lambda[N-2]);
    else if (i == N-2) flambda[N-2] = -(2.*h*u[N-2] - 2.*d*lambda[N-2] + d*lambda[N-3]);
    else               flambda[i]   = -((2.*h*u[i]   + d*(lambda[i+1] - 2.0*lambda[i] + lambda[i-1])));
  } 

  /* derivative of L() w.r.t. lambda */
  for (i=xs; i<xs+xm; i++) {
    if      (i == 0)   fu[0]   = 2.0*d*(u[0] - w[0]);
    else if (i == N-1) fu[N-1] = 2.0*d*u[N-1];
    else               fu[i]   = -(d*(u[i+1] - 2.0*u[i] + u[i-1]) - 2.0*h);
  } 

  ierr = DAVecRestoreArray(da,vu,(void**)&u);CHKERRQ(ierr);
  ierr = DAVecRestoreArray(da,vfu,(void**)&fu);CHKERRQ(ierr);
  ierr = DAVecRestoreArray(da,vlambda,(void**)&lambda);CHKERRQ(ierr);
  ierr = DAVecRestoreArray(da,vflambda,(void**)&flambda);CHKERRQ(ierr);

  ierr = VecPackRestoreLocalVectors(packer,&w,&vu,&vlambda);CHKERRQ(ierr);
  PLogFlops(13*N);
  PetscFunctionReturn(0);
}


/*
     This local function acts on the ghosted version of U (accessed via DAGetLocalVector())
     BUT the global, nonghosted version of FU

*/
int PDEFormFunction(SNES snes,Vec U,Vec FU,void* dummy)
{
  DMMG    dmmg = (DMMG)dummy;
  int     ierr,xs,xm,i,N;
  Scalar  *u,*fu,d,h;
  Vec     vu;
  DA      da = (DA) dmmg->dm;

  PetscFunctionBegin;
  ierr = DAGetLocalVector(da,&vu);CHKERRQ(ierr);
  ierr = DAGlobalToLocalBegin(da,U,INSERT_VALUES,vu);CHKERRQ(ierr);
  ierr = DAGlobalToLocalEnd(da,U,INSERT_VALUES,vu);CHKERRQ(ierr);

  ierr = DAGetCorners(da,&xs,PETSC_NULL,PETSC_NULL,&xm,PETSC_NULL,PETSC_NULL);CHKERRQ(ierr);
  ierr = DAGetInfo(da,0,&N,0,0,0,0,0,0,0,0,0);CHKERRQ(ierr);
  ierr = DAVecGetArray(da,vu,(void**)&u);CHKERRQ(ierr);
  ierr = DAVecGetArray(da,FU,(void**)&fu);CHKERRQ(ierr);
  d    = N-1.0;
  h    = 1.0/d;

  for (i=xs; i<xs+xm; i++) {
    if      (i == 0)   fu[0]   = 2.0*d*(u[0] - .25);
    else if (i == N-1) fu[N-1] = 2.0*d*u[N-1];
    else               fu[i]   = -(d*(u[i+1] - 2.0*u[i] + u[i-1]) - 2.0*h);
  } 

  ierr = DAVecRestoreArray(da,vu,(void**)&u);CHKERRQ(ierr);
  ierr = DAVecRestoreArray(da,FU,(void**)&fu);CHKERRQ(ierr);
  ierr = DARestoreLocalVector(da,&vu);CHKERRQ(ierr);
  PLogFlops(6*N);
  PetscFunctionReturn(0);
}
