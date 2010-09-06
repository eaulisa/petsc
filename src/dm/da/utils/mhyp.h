
#if !defined(_MHYP_H)
#define _MHYP_H

#include "petscda.h"   /*I "petscda.h" I*/
#include "HYPRE_struct_mv.h"
#include "HYPRE_struct_ls.h"
#include "_hypre_struct_mv.h"
#include "HYPRE_sstruct_mv.h"
#include "HYPRE_sstruct_ls.h"
#include "_hypre_sstruct_mv.h"

typedef struct {
  MPI_Comm            hcomm;
  DA                  da;
  HYPRE_StructGrid    hgrid;
  HYPRE_StructStencil hstencil;
  HYPRE_StructMatrix  hmat;
  HYPRE_StructVector  hb,hx;
  hypre_Box           hbox;

  PetscTruth          needsinitialization;

  /* variables that are stored here so they need not be reloaded for each MatSetValuesLocal() or MatZeroRowsLocal() call */
  PetscInt            *gindices,rstart,gnx,gnxgny,xs,ys,zs,nx,ny,nxny;
} Mat_HYPREStruct;

typedef struct {
  MPI_Comm               hcomm;
  DA                     da;
  HYPRE_SStructGrid      ss_grid;
  HYPRE_SStructGraph     ss_graph;
  HYPRE_SStructStencil   ss_stencil;
  HYPRE_SStructMatrix    ss_mat;
  HYPRE_SStructVector    ss_b, ss_x;
  hypre_Box              hbox;

  int                    ss_object_type;
  int                    nvars;
  int                    dofs_order;

  PetscTruth             needsinitialization;

  /* variables that are stored here so they need not be reloaded for each MatSetValuesLocal() or MatZeroRowsLocal() call */
  PetscInt              *gindices,rstart,gnx,gnxgny,gnxgnygnz,xs,ys,zs,nx,ny,nz,nxny,nxnynz;
} Mat_HYPRESStruct;


EXTERN PetscErrorCode MatHYPRE_IJMatrixCreate(Mat,HYPRE_IJMatrix*);
EXTERN PetscErrorCode MatHYPRE_IJMatrixCopy(Mat,HYPRE_IJMatrix);
EXTERN PetscErrorCode MatHYPRE_IJMatrixFastCopy(Mat,HYPRE_IJMatrix);
EXTERN PetscErrorCode VecHYPRE_IJVectorCreate(Vec,HYPRE_IJVector*);

/*
    PetscStackCallHypre - Calls a hypre library routine after pushing the name of the routine on the stack.

   Input Parameters:
+   name - string that gives the name of the function being called
-   routine - actual call to the routine

   Developer Note: this is so that when a hypre routine results in a crash or corrupts memory, they get blamed instead of PETSc.

*/
#define PetscStackCallHypre(name,routine) PetscStackPush(name);ierr = routine;if (ierr) SETERRQ1(PETSC_COMM_SELF,PETSC_ERR_LIB,"Error in %s()",name);PetscStackPop;

#endif
