
#define PETSCDM_DLL
#include <petsc/private/dmswarmimpl.h>    /*I   "petscdmswarm.h"   I*/
#include "data_bucket.h"

const char* DMSwarmTypeNames[] = { "basic", "pic", 0 };
const char* DMSwarmMigrateTypeNames[] = { "basic", "dmcellnscatter", "dmcellexact", "user", 0 };
const char* DMSwarmCollectTypeNames[] = { "basic", "boundingbox", "general", "user", 0 };

const char DMSwarmField_pid[] = "DMSwarm_pid";
const char DMSwarmField_rank[] = "DMSwarm_rank";
const char DMSwarmPICField_coor[] = "DMSwarmPIC_coor";


PetscErrorCode DMSwarmMigrate_Push_Basic(DM dm,PetscBool remove_sent_points);

/*@C

  DMSwarmVectorDefineField - Sets the field from which to define a Vec object
 
  Collective on DM
 
  Input parameters:
. dm - a DMSwarm
. fieldname - The textual name given to a registered field
 
  Level: beginner

  Notes:
  The field with name fieldname must be defined as having a data type of PetscScalar
  This function must be called prior to calling DMCreateLocalVector(), DMCreateGlobalVector().
  Mutiple calls to DMSwarmVectorDefineField() are permitted.
 
. seealso: DMSwarmRegisterPetscDatatypeField()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmVectorDefineField"
PETSC_EXTERN PetscErrorCode DMSwarmVectorDefineField(DM dm,const char fieldname[])
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  PetscInt bs,n;
  PetscScalar *array;
  PetscDataType type;

  if (!swarm->issetup) { ierr = DMSetUp(dm);CHKERRQ(ierr); }
  ierr = DataBucketGetSizes(swarm->db,&n,NULL,NULL);CHKERRQ(ierr);
  ierr = DMSwarmGetField(dm,fieldname,&bs,&type,(void**)&array);CHKERRQ(ierr);

  /* Check all fields are of type PETSC_REAL or PETSC_SCALAR */
  if (type != PETSC_REAL) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_SUP,"Only valid for PETSC_REAL");
  
  PetscSNPrintf(swarm->vec_field_name,PETSC_MAX_PATH_LEN-1,"%s",fieldname);
  swarm->vec_field_set = PETSC_TRUE;
  swarm->vec_field_bs = bs;
  swarm->vec_field_nlocal = n;
  ierr = DMSwarmRestoreField(dm,fieldname,&bs,&type,(void**)&array);CHKERRQ(ierr);
  
  PetscFunctionReturn(0);
}

/* requires DMSwarmDefineFieldVector has been called */
#undef __FUNCT__
#define __FUNCT__ "DMCreateGlobalVector_Swarm"
PetscErrorCode DMCreateGlobalVector_Swarm(DM dm,Vec *vec)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  Vec x;
  char name[PETSC_MAX_PATH_LEN];

  if (!swarm->issetup) { ierr = DMSetUp(dm);CHKERRQ(ierr); }
  if (!swarm->vec_field_set) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"Must call DMSwarmVectorDefineField first");
  if (swarm->vec_field_nlocal != swarm->db->L) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"DMSwarm sizes have changed since last call to VectorDefineField first"); /* Stale data */
  
  PetscSNPrintf(name,PETSC_MAX_PATH_LEN-1,"DMSwarmField_%s",swarm->vec_field_name);
  ierr = VecCreate(PetscObjectComm((PetscObject)dm),&x);CHKERRQ(ierr);
  ierr = PetscObjectSetName((PetscObject)x,name);CHKERRQ(ierr);
  ierr = VecSetSizes(x,swarm->db->L*swarm->vec_field_bs,PETSC_DETERMINE);CHKERRQ(ierr);
  ierr = VecSetBlockSize(x,swarm->vec_field_bs);CHKERRQ(ierr);
  ierr = VecSetFromOptions(x);CHKERRQ(ierr);
  *vec = x;
  
  PetscFunctionReturn(0);
}

/* requires DMSwarmDefineFieldVector has been called */
#undef __FUNCT__
#define __FUNCT__ "DMCreateLocalVector_Swarm"
PetscErrorCode DMCreateLocalVector_Swarm(DM dm,Vec *vec)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  Vec x;
  char name[PETSC_MAX_PATH_LEN];
  
  if (!swarm->issetup) { ierr = DMSetUp(dm);CHKERRQ(ierr); }
  if (!swarm->vec_field_set) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"Must call DMSwarmVectorDefineField first");
  if (swarm->vec_field_nlocal != swarm->db->L) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"DMSwarm sizes have changed since last call to VectorDefineField first"); /* Stale data */

  PetscSNPrintf(name,PETSC_MAX_PATH_LEN-1,"DMSwarmField_%s",swarm->vec_field_name);
  ierr = VecCreate(PETSC_COMM_SELF,&x);CHKERRQ(ierr);
  ierr = PetscObjectSetName((PetscObject)x,name);CHKERRQ(ierr);
  ierr = VecSetSizes(x,swarm->db->L*swarm->vec_field_bs,swarm->db->L);CHKERRQ(ierr);
  ierr = VecSetBlockSize(x,swarm->vec_field_bs);CHKERRQ(ierr);
  ierr = VecSetFromOptions(x);CHKERRQ(ierr);
  *vec = x;
  
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmCreateGlobalVectorFromField - Creates a Vec object sharing the array associated with a given field
 
 Collective on DM
 
 Input parameters:
 . dm - a DMSwarm
 . fieldname - the textual name given to a registered field
 
 Output parameters:
 . vec - the vector
 
 Level: beginner
 
 . seealso: DMSwarmRegisterPetscDatatypeField()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmCreateGlobalVectorFromField"
PETSC_EXTERN PetscErrorCode DMSwarmCreateGlobalVectorFromField(DM dm,const char fieldname[],Vec *vec)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  PetscInt bs,n;
  PetscScalar *array;
  Vec x;
  PetscDataType type;
  char name[PETSC_MAX_PATH_LEN];
  PetscMPIInt commsize;
  
  if (!swarm->issetup) { ierr = DMSetUp(dm);CHKERRQ(ierr); }

  ierr = DataBucketGetSizes(swarm->db,&n,NULL,NULL);CHKERRQ(ierr);
  ierr = DMSwarmGetField(dm,fieldname,&bs,&type,(void**)&array);CHKERRQ(ierr);

  /* Check all fields are of type PETSC_REAL or PETSC_SCALAR */
  if (type != PETSC_REAL) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_SUP,"Only valid for PETSC_REAL");

  ierr = MPI_Comm_size(PetscObjectComm((PetscObject)dm),&commsize);CHKERRQ(ierr);
  if (commsize == 1) {
    ierr = VecCreateSeqWithArray(PetscObjectComm((PetscObject)dm),bs,n*bs,array,&x);CHKERRQ(ierr);
  } else {
    ierr = VecCreateMPIWithArray(PetscObjectComm((PetscObject)dm),bs,n*bs,PETSC_DETERMINE,array,&x);CHKERRQ(ierr);
  }
  PetscSNPrintf(name,PETSC_MAX_PATH_LEN-1,"DMSwarmSharedField_%s",fieldname);
  ierr = PetscObjectSetName((PetscObject)x,name);CHKERRQ(ierr);

  /* Set guard */
  PetscSNPrintf(name,PETSC_MAX_PATH_LEN-1,"DMSwarm_VecFieldInPlace_%s",fieldname);
  ierr = PetscObjectComposeFunction((PetscObject)x,name,DMSwarmDestroyGlobalVectorFromField);CHKERRQ(ierr);
  *vec = x;
  PetscFunctionReturn(0);
}


/*@C
 
 DMSwarmDestroyGlobalVectorFromField - Destroys the Vec object which share the array associated with a given field
 
 Collective on DM
 
 Input parameters:
 . dm - a DMSwarm
 . fieldname - the textual name given to a registered field
 
 Output parameters:
 . vec - the vector
 
 Level: beginner
 
 . seealso: DMSwarmRegisterPetscDatatypeField()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmDestroyGlobalVectorFromField"
PETSC_EXTERN PetscErrorCode DMSwarmDestroyGlobalVectorFromField(DM dm,const char fieldname[],Vec *vec)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  DataField gfield;
  char name[PETSC_MAX_PATH_LEN];
  void (*fptr)(void);
  PetscInt bs,nlocal;
  
  ierr = VecGetLocalSize(*vec,&nlocal);CHKERRQ(ierr);
  ierr = VecGetBlockSize(*vec,&bs);CHKERRQ(ierr);
  if (nlocal/bs != swarm->db->L) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"DMSwarm sizes have changed since vector was created - cannot ensure pointers are valid"); /* Stale data */

  /* get data field */
  ierr = DataBucketGetDataFieldByName(swarm->db,fieldname,&gfield);CHKERRQ(ierr);
  
  /* check vector is an inplace array */
  PetscSNPrintf(name,PETSC_MAX_PATH_LEN-1,"DMSwarm_VecFieldInPlace_%s",fieldname);
  ierr = PetscObjectQueryFunction((PetscObject)(*vec),name,&fptr);CHKERRQ(ierr);
  if (!fptr) SETERRQ1(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"Vector being destroyed was not created from DMSwarm field(%s)",fieldname);
  
  /* restore data field */
  ierr = DataFieldRestoreAccess(gfield);CHKERRQ(ierr);
  
  ierr = VecDestroy(vec);CHKERRQ(ierr);
  
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "DMSwarmCreateGlobalVector"
PETSC_EXTERN PetscErrorCode DMSwarmCreateGlobalVector(DM dm, const char fieldname[], Vec *vec)
{
  DM_Swarm      *swarm = (DM_Swarm *) dm->data;
  PetscScalar   *array;
  PetscInt       bs,n;
  PetscMPIInt    commsize;
  PetscErrorCode ierr;

  if (!swarm->issetup) {ierr = DMSetUp(dm);CHKERRQ(ierr);}

  ierr = DataBucketGetSizes(swarm->db,&n,NULL,NULL);CHKERRQ(ierr);
  ierr = DMSwarmGetField(dm,fieldname,&bs,NULL,(void**) &array);CHKERRQ(ierr);
  ierr = DMSwarmRestoreField(dm,fieldname,&bs,NULL,(void**) &array);CHKERRQ(ierr);
  ierr = MPI_Comm_size(PetscObjectComm((PetscObject)dm),&commsize);CHKERRQ(ierr);
  if (commsize == 1) {
    ierr = VecCreateSeq(PetscObjectComm((PetscObject)dm),n*bs,vec);CHKERRQ(ierr);
    ierr = VecSetBlockSize(*vec,bs);CHKERRQ(ierr);
  } else {
    ierr = VecCreateMPI(PetscObjectComm((PetscObject)dm),n*bs,PETSC_DETERMINE,vec);CHKERRQ(ierr);
    ierr = VecSetBlockSize(*vec,bs);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

/*
PETSC_EXTERN PetscErrorCode DMSwarmCreateGlobalVectorFromFields(DM dm,const PetscInt nf,const char *fieldnames[],Vec *vec)
{
  PetscFunctionReturn(0);
}

PETSC_EXTERN PetscErrorCode DMSwarmRestoreGlobalVectorFromFields(DM dm,Vec *vec)
{
  PetscFunctionReturn(0);
}
*/
 

/*@C
 
 DMSwarmInitializeFieldRegister - Initiates the registration of fields to a DMSwarm
 
 Collective on DM
 
 Input parameter:
 . dm - a DMSwarm
 
 Level: beginner
 
 Notes:
 After all fields have been registered, users should call DMSwarmFinalizeFieldRegister()
 
 . seealso: DMSwarmFinalizeFieldRegister(), DMSwarmRegisterPetscDatatypeField(), 
 DMSwarmRegisterUserStructField(), DMSwarmRegisterUserDatatypeField()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmInitializeFieldRegister"
PETSC_EXTERN PetscErrorCode DMSwarmInitializeFieldRegister(DM dm)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;

  if (!swarm->field_registration_initialized) {
    swarm->field_registration_initialized = PETSC_TRUE;
    ierr = DMSwarmRegisterPetscDatatypeField(dm,DMSwarmField_pid,1,PETSC_LONG);CHKERRQ(ierr); /* unique identifer */
    ierr = DMSwarmRegisterPetscDatatypeField(dm,DMSwarmField_rank,1,PETSC_INT);CHKERRQ(ierr); /* used for communication */
  }
  
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmFinalizeFieldRegister - Finalizes the registration of fields to a DMSwarm
 
 Collective on DM
 
 Input parameter:
 . dm - a DMSwarm
 
 Level: beginner
 
 Notes:
 After DMSwarmFinalizeFieldRegister() has been called, no new fields can be defined
 on the DMSwarm
 
 . seealso: DMSwarmInitializeFieldRegister(), DMSwarmRegisterPetscDatatypeField(),
 DMSwarmRegisterUserStructField(), DMSwarmRegisterUserDatatypeField()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmFinalizeFieldRegister"
PETSC_EXTERN PetscErrorCode DMSwarmFinalizeFieldRegister(DM dm)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;

  if (!swarm->field_registration_finalized) {
    ierr = DataBucketFinalize(swarm->db);CHKERRQ(ierr);
  }
  swarm->field_registration_finalized = PETSC_TRUE;
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmSetLocalSizes - Sets the length of all registered fields on the DMSwarm
 
 Not collective
 
 Input parameters:
 . dm - a DMSwarm
 . nlocal - the length of each registered field
 . buffer - the length of the buffer used to efficient dynamic re-sizing
 
 Level: beginner
 
 . seealso: DMSwarmGetLocalSize()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmSetLocalSizes"
PETSC_EXTERN PetscErrorCode DMSwarmSetLocalSizes(DM dm,PetscInt nlocal,PetscInt buffer)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  
  ierr = DataBucketSetSizes(swarm->db,nlocal,buffer);CHKERRQ(ierr);
  
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmSetCellDM - Attachs a DM to a DMSwarm
 
 Collective on DM
 
 Input parameters:
 . dm - a DMSwarm
 . dmcell - the DM to attach to the DMSwarm
 
 Level: beginner
 
 Notes:
 The attached DM (dmcell) will be queried for pointlocation and 
 neighbor MPI-rank information if DMSwarmMigrate() is called
 
 . seealso: DMSwarmGetCellDM(), DMSwarmMigrate()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmSetCellDM"
PETSC_EXTERN PetscErrorCode DMSwarmSetCellDM(DM dm,DM dmcell)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  swarm->dmcell = dmcell;
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmGetCellDM - Fetches the attached cell DM
 
 Collective on DM
 
 Input parameter:
 . dm - a DMSwarm

 Output parameter:
 . dmcell - the DM which was attached to the DMSwarm

 Level: beginner
 
 Notes:
 The attached DM (dmcell) will be queried for pointlocation and
 neighbor MPI-rank information if DMSwarmMigrate() is called
 
 . seealso: DMSwarmSetCellDM()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmGetCellDM"
PETSC_EXTERN PetscErrorCode DMSwarmGetCellDM(DM dm,DM *dmcell)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  *dmcell = swarm->dmcell;
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmGetLocalSize - Retrives the local length of fields registered
 
 Not collective
 
 Input parameter:
 . dm - a DMSwarm
 
 Output parameter:
 . nlocal - the length of each registered field
 
 Level: beginner
 
 . seealso: DMSwarmSetLocalSizes()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmGetLocalSize"
PETSC_EXTERN PetscErrorCode DMSwarmGetLocalSize(DM dm,PetscInt *nlocal)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  
  if (nlocal) {
    ierr = DataBucketGetSizes(swarm->db,nlocal,NULL,NULL);CHKERRQ(ierr);
  }
  
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmGetSize - Retrives the total length of fields registered
 
 Collective on DM
 
 Input parameter:
 . dm - a DMSwarm
 
 Output parameter:
 . n - the total length of each registered field
 
 Level: beginner

 Note:
 This calls MPI_Allreduce upon each call (inefficient but safe)
 
 . seealso: DMSwarmGetLocalSize()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmGetSize"
PETSC_EXTERN PetscErrorCode DMSwarmGetSize(DM dm,PetscInt *n)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  PetscInt nlocal,ng;
  
  ierr = DataBucketGetSizes(swarm->db,&nlocal,NULL,NULL);CHKERRQ(ierr);
  ierr = MPI_Allreduce(&nlocal,&ng,1,MPIU_INT,MPI_SUM,PetscObjectComm((PetscObject)dm));CHKERRQ(ierr);
  if (n) { *n = ng; }
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmRegisterPetscDatatypeField - Register a field to a DMSwarm
 
 Collective on DM
 
 Input parameters:
 . dm - a DMSwarm
 . fieldname - the textual name to identify this field
 . blocksize - the number of each data type
 . type - a valid PETSc data type (PETSC_CHAR, PETSC_SHORT, PETSC_INT, PETSC_FLOAT, PETSC_REAL, PETSC_LONG)
 
 Level: beginner
 
 Notes:
 The textual name for each registered field must be unique

 . seealso: DMSwarmRegisterUserStructField(), DMSwarmRegisterUserDatatypeField()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmRegisterPetscDatatypeField"
PETSC_EXTERN PetscErrorCode DMSwarmRegisterPetscDatatypeField(DM dm,const char fieldname[],PetscInt blocksize,PetscDataType type)
{
  PetscErrorCode ierr;
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  size_t size;
  
  if (!swarm->field_registration_initialized) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"Must call DMSwarmInitializeFieldRegister() first");
  if (swarm->field_registration_finalized) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"Cannot register additional fields after calling DMSwarmFinalizeFieldRegister() first");
  
  if (type == PETSC_OBJECT) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_SUP,"Valid for {char,short,int,long,float,double}");
  if (type == PETSC_FUNCTION) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_SUP,"Valid for {char,short,int,long,float,double}");
  if (type == PETSC_STRING) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_SUP,"Valid for {char,short,int,long,float,double}");
  if (type == PETSC_STRUCT) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_SUP,"Valid for {char,short,int,long,float,double}");
  if (type == PETSC_DATATYPE_UNKNOWN) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_SUP,"Valid for {char,short,int,long,float,double}");

  ierr = PetscDataTypeGetSize(type, &size);CHKERRQ(ierr);
  
  /* Load a specific data type into data bucket, specifying textual name and its size in bytes */
	ierr = DataBucketRegisterField(swarm->db,"DMSwarmRegisterPetscDatatypeField",fieldname,blocksize*size,NULL);CHKERRQ(ierr);
  {
    DataField gfield;
    
    ierr = DataBucketGetDataFieldByName(swarm->db,fieldname,&gfield);CHKERRQ(ierr);
    ierr = DataFieldSetBlockSize(gfield,blocksize);CHKERRQ(ierr);
  }
  swarm->db->field[swarm->db->nfields-1]->petsc_type = type;
  
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmRegisterUserStructField - Register a user defined struct to a DMSwarm
 
 Collective on DM
 
 Input parameters:
 . dm - a DMSwarm
 . fieldname - the textual name to identify this field
 . size - the size in bytes of the user struct of each data type
 
 Level: beginner
 
 Notes:
 The textual name for each registered field must be unique

 . seealso: DMSwarmRegisterPetscDatatypeField(), DMSwarmRegisterUserDatatypeField()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmRegisterUserStructField"
PETSC_EXTERN PetscErrorCode DMSwarmRegisterUserStructField(DM dm,const char fieldname[],size_t size)
{
  PetscErrorCode ierr;
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  
	ierr = DataBucketRegisterField(swarm->db,"DMSwarmRegisterUserStructField",fieldname,size,NULL);CHKERRQ(ierr);
  swarm->db->field[swarm->db->nfields-1]->petsc_type = PETSC_STRUCT ;
  
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmRegisterUserDatatypeField - Register a user defined data type to a DMSwarm
 
 Collective on DM
 
 Input parameters:
 . dm - a DMSwarm
 . fieldname - the textual name to identify this field
 . size - the size in bytes of the user data type
 . blocksize - the number of each data type
 
 Level: beginner
 
 Notes:
 The textual name for each registered field must be unique
 
 . seealso: DMSwarmRegisterPetscDatatypeField(), DMSwarmRegisterUserStructField(), DMSwarmRegisterUserDatatypeField()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmRegisterUserDatatypeField"
PETSC_EXTERN PetscErrorCode DMSwarmRegisterUserDatatypeField(DM dm,const char fieldname[],size_t size,PetscInt blocksize)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;

	ierr = DataBucketRegisterField(swarm->db,"DMSwarmRegisterUserDatatypeField",fieldname,blocksize*size,NULL);CHKERRQ(ierr);
  {
    DataField gfield;
    
    ierr = DataBucketGetDataFieldByName(swarm->db,fieldname,&gfield);CHKERRQ(ierr);
    ierr = DataFieldSetBlockSize(gfield,blocksize);CHKERRQ(ierr);
  }
  swarm->db->field[swarm->db->nfields-1]->petsc_type = PETSC_DATATYPE_UNKNOWN;
  
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmGetField - Get access to the underlying array storing all entries associated with a registered field
 
 Not collective
 
 Input parameters:
 . dm - a DMSwarm
 . fieldname - the textual name to identify this field

 Output parameters:
 . blocksize - the number of each data type
 . type - the data type
 . data - pointer to raw array
 
 Level: beginner
 
 Notes:
 The user must call DMSwarmRestoreField()
 
 . seealso: DMSwarmRestoreField()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmGetField"
PETSC_EXTERN PetscErrorCode DMSwarmGetField(DM dm,const char fieldname[],PetscInt *blocksize,PetscDataType *type,void **data)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  DataField gfield;
  PetscErrorCode ierr;
  
  if (!swarm->issetup) { ierr = DMSetUp(dm);CHKERRQ(ierr); }
  
  ierr = DataBucketGetDataFieldByName(swarm->db,fieldname,&gfield);CHKERRQ(ierr);
  ierr = DataFieldGetAccess(gfield);CHKERRQ(ierr);
  ierr = DataFieldGetEntries(gfield,data);CHKERRQ(ierr);
  if (blocksize) {*blocksize = gfield->bs; }
  if (type) { *type = gfield->petsc_type; }
  
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmRestoreField - Restore access to the underlying array storing all entries associated with a registered field
 
 Not collective
 
 Input parameters:
 . dm - a DMSwarm
 . fieldname - the textual name to identify this field
 
 Output parameters:
 . blocksize - the number of each data type
 . type - the data type
 . data - pointer to raw array
 
 Level: beginner
 
 Notes:
 The user must call DMSwarmGetField() prior to calling DMSwarmRestoreField()
 
 . seealso: DMSwarmGetField()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmRestoreField"
PETSC_EXTERN PetscErrorCode DMSwarmRestoreField(DM dm,const char fieldname[],PetscInt *blocksize,PetscDataType *type,void **data)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  DataField gfield;
  PetscErrorCode ierr;
  
  ierr = DataBucketGetDataFieldByName(swarm->db,fieldname,&gfield);CHKERRQ(ierr);
  ierr = DataFieldRestoreAccess(gfield);CHKERRQ(ierr);
  if (data) *data = NULL;
  
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmAddPoint - Add space for one new point in the DMSwarm
 
 Not collective
 
 Input parameter:
 . dm - a DMSwarm
 
 Level: beginner
 
 Notes:
 The new point will have all fields initialized to zero
 
 . seealso: DMSwarmAddNPoints()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmAddPoint"
PETSC_EXTERN PetscErrorCode DMSwarmAddPoint(DM dm)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  
  if (!swarm->issetup) { ierr = DMSetUp(dm);CHKERRQ(ierr); }
  ierr = DataBucketAddPoint(swarm->db);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmAddNPoints - Add space for a number of new points in the DMSwarm
 
 Not collective
 
 Input parameters:
 . dm - a DMSwarm
 . npoints - the number of new points to add
 
 Level: beginner
 
 Notes:
 The new point will have all fields initialized to zero
 
 . seealso: DMSwarmAddPoint()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmAddNPoints"
PETSC_EXTERN PetscErrorCode DMSwarmAddNPoints(DM dm,PetscInt npoints)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  PetscInt nlocal;

  ierr = DataBucketGetSizes(swarm->db,&nlocal,NULL,NULL);CHKERRQ(ierr);
  nlocal = nlocal + npoints;
  ierr = DataBucketSetSizes(swarm->db,nlocal,-1);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmRemovePoint - Remove the last point from the DMSwarm
 
 Not collective
 
 Input parameter:
 . dm - a DMSwarm
 
 Level: beginner
 
 . seealso: DMSwarmRemovePointAtIndex()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmRemovePoint"
PETSC_EXTERN PetscErrorCode DMSwarmRemovePoint(DM dm)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;

  ierr = DataBucketRemovePoint(swarm->db);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmRemovePointAtIndex - Removes a specific point from the DMSwarm
 
 Not collective
 
 Input parameters:
 . dm - a DMSwarm
 . idx - index of point to remove
 
 Level: beginner
 
 . seealso: DMSwarmRemovePoint()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmRemovePointAtIndex"
PETSC_EXTERN PetscErrorCode DMSwarmRemovePointAtIndex(DM dm,PetscInt idx)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;

  ierr = DataBucketRemovePointAtIndex(swarm->db,idx);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "DMSwarmMigrate_Basic"
PetscErrorCode DMSwarmMigrate_Basic(DM dm,PetscBool remove_sent_points)
{
  PetscErrorCode ierr;
  ierr = DMSwarmMigrate_Push_Basic(dm,remove_sent_points);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

PetscErrorCode DMSwarmMigrate_CellDMScatter(DM dm,PetscBool remove_sent_points);
PetscErrorCode DMSwarmMigrate_CellDMExact(DM dm,PetscBool remove_sent_points);

/*@C
 
 DMSwarmMigrate - Relocates points defined in the DMSwarm to other MPI-ranks
 
 Collective on DM
 
 Input parameters:
 . dm - the DMSwarm
 . remove_sent_points - flag indicating if sent points should be removed from the current MPI-rank

 Notes:
 The DM wil be modified to accomodate received points.
 If remove_sent_points = PETSC_TRUE, send points will be removed from the DM
 Different styles of migration are supported. See DMSwarmSetMigrateType()
 
 Level: advanced

 . seealso: DMSwarmSetMigrateType()

@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmMigrate"
PETSC_EXTERN PetscErrorCode DMSwarmMigrate(DM dm,PetscBool remove_sent_points)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;

  switch (swarm->migrate_type) {

    case DMSWARM_MIGRATE_BASIC:
      ierr = DMSwarmMigrate_Basic(dm,remove_sent_points);CHKERRQ(ierr);
      break;
    
    case DMSWARM_MIGRATE_DMCELLNSCATTER:
      ierr = DMSwarmMigrate_CellDMScatter(dm,remove_sent_points);CHKERRQ(ierr);
      break;
    
    case DMSWARM_MIGRATE_DMCELLEXACT:
      SETERRQ(PETSC_COMM_WORLD,PETSC_ERR_SUP,"DMSWARM_MIGRATE_DMCELLEXACT not implemented");
      //ierr = DMSwarmMigrate_CellDMExact(dm,remove_sent_points);CHKERRQ(ierr);
      break;
    
    case DMSWARM_MIGRATE_USER:
      SETERRQ(PETSC_COMM_WORLD,PETSC_ERR_SUP,"DMSWARM_MIGRATE_USER not implemented");
      //ierr = swarm->migrate(dm,remove_sent_points);CHKERRQ(ierr);
      break;
    
    default:
      SETERRQ(PETSC_COMM_WORLD,PETSC_ERR_SUP,"DMSWARM_MIGRATE type unknown");
      break;
  }
  PetscFunctionReturn(0);
}

PetscErrorCode DMSwarmMigrate_GlobalToLocal_Basic(DM dm,PetscInt *globalsize);

/*
 DMSwarmCollectViewCreate
 
 * Applies a collection method and gathers point neighbour points into dm

 Notes:
 - Users should call DMSwarmCollectViewDestroy() after 
 they have finished computations associated with the collected points
*/

/*@C
 
 DMSwarmCollectViewCreate - Applies a collection method and gathers points 
 in neighbour MPI-ranks into the DMSwarm
 
 Collective on DM
 
 Input parameter:
 . dm - the DMSwarm
 
 Notes:
 Users should call DMSwarmCollectViewDestroy() after
 they have finished computations associated with the collected points
 Different collect methods are supported. See DMSwarmSetCollectType()

 Level: advanced
 
 . seealso: DMSwarmCollectViewDestroy(), DMSwarmSetCollectType()

@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmCollectViewCreate"
PETSC_EXTERN PetscErrorCode DMSwarmCollectViewCreate(DM dm)
{
  PetscErrorCode ierr;
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscInt ng;

  if (swarm->collect_view_active) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"CollectView currently active");
  
  ierr = DMSwarmGetLocalSize(dm,&ng);CHKERRQ(ierr);
  switch (swarm->collect_type) {

    case DMSWARM_COLLECT_BASIC:
      ierr = DMSwarmMigrate_GlobalToLocal_Basic(dm,&ng);CHKERRQ(ierr);
      break;
    
    case DMSWARM_COLLECT_DMDABOUNDINGBOX:
      SETERRQ(PETSC_COMM_WORLD,PETSC_ERR_SUP,"DMSWARM_COLLECT_DMDABOUNDINGBOX not implemented");
      //ierr = DMSwarmCollect_DMDABoundingBox(dm,&ng);CHKERRQ(ierr);
      break;
    
    case DMSWARM_COLLECT_GENERAL:
      SETERRQ(PETSC_COMM_WORLD,PETSC_ERR_SUP,"DMSWARM_COLLECT_GENERAL not implemented");
      //ierr = DMSwarmCollect_General(dm,..,,..,&ng);CHKERRQ(ierr);
      break;
      
    default:
      SETERRQ(PETSC_COMM_WORLD,PETSC_ERR_SUP,"DMSWARM_COLLECT type unknown");
      break;
  }

  swarm->collect_view_active = PETSC_TRUE;
  swarm->collect_view_reset_nlocal = ng;
  
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmCollectViewDestroy - Resets the DMSwarm to the size prior to calling DMSwarmCollectViewCreate()
 
 Collective on DM
 
 Input parameters:
 . dm - the DMSwarm
 
 Notes:
 Users should call DMSwarmCollectViewCreate() before this function is called.
 
 Level: advanced
 
 . seealso: DMSwarmCollectViewCreate(), DMSwarmSetCollectType()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmCollectViewDestroy"
PETSC_EXTERN PetscErrorCode DMSwarmCollectViewDestroy(DM dm)
{
  PetscErrorCode ierr;
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  
  if (!swarm->collect_view_active) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"CollectView is currently not active");
  ierr = DMSwarmSetLocalSizes(dm,swarm->collect_view_reset_nlocal,-1);CHKERRQ(ierr);
  swarm->collect_view_active = PETSC_FALSE;
  
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "DMSwarmSetUpPIC"
PetscErrorCode DMSwarmSetUpPIC(DM dm)
{
  PetscInt dim;
  PetscErrorCode ierr;
  
  ierr = DMGetDimension(dm,&dim);CHKERRQ(ierr);
  if (dim < 1) SETERRQ1(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"Dimension must be 1,2,3 - found %D",dim);
  if (dim > 3) SETERRQ1(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"Dimension must be 1,2,3 - found %D",dim);
  ierr = DMSwarmRegisterPetscDatatypeField(dm,DMSwarmPICField_coor,dim,PETSC_DOUBLE);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/*@C
 
 DMSwarmSetType - Set particular flavor of DMSwarm
 
 Collective on DM
 
 Input parameters:
 . dm - the DMSwarm
 . stype - the DMSwarm type (e.g. DMSWARM_PIC)
 
 Level: advanced
 
 . seealso: DMSwarmSetMigrateType(), DMSwarmSetCollectType()
 
@*/
#undef __FUNCT__
#define __FUNCT__ "DMSwarmSetType"
PETSC_EXTERN PetscErrorCode DMSwarmSetType(DM dm,DMSwarmType stype)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  
  swarm->swarm_type = stype;
  if (swarm->swarm_type == DMSWARM_PIC) {
    ierr = DMSwarmSetUpPIC(dm);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "DMSetup_Swarm"
PetscErrorCode DMSetup_Swarm(DM dm)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;
  PetscMPIInt rank;
  PetscInt p,npoints,*rankval;
  
  if (swarm->issetup) PetscFunctionReturn(0);
  
  swarm->issetup = PETSC_TRUE;

  if (swarm->swarm_type == DMSWARM_PIC) {
    /* check dmcell exists */
    if (!swarm->dmcell) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"DMSWARM_PIC requires you call DMSwarmSetCellDM");

    if (swarm->dmcell->ops->locatepointssubdomain) {
      /* check methods exists for exact ownership identificiation */
      PetscPrintf(PetscObjectComm((PetscObject)dm),"  DMSWARM_PIC: Using method CellDM->ops->LocatePointsSubdomain\n");
      swarm->migrate_type = DMSWARM_MIGRATE_DMCELLEXACT;
    } else {
      /* check methods exist for point location AND rank neighbor identification */
      if (swarm->dmcell->ops->locatepoints) {
        PetscPrintf(PetscObjectComm((PetscObject)dm),"  DMSWARM_PIC: Using method CellDM->LocatePoints\n");
      } else SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"DMSWARM_PIC requires the method CellDM->ops->locatepoints be defined");

      if (swarm->dmcell->ops->getneighbors) {
        PetscPrintf(PetscObjectComm((PetscObject)dm),"  DMSWARM_PIC: Using method CellDM->GetNeigbors\n");
      } else SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"DMSWARM_PIC requires the method CellDM->ops->getneighbors be defined");

      swarm->migrate_type = DMSWARM_MIGRATE_DMCELLNSCATTER;
    }
  }
  
  ierr = DMSwarmFinalizeFieldRegister(dm);CHKERRQ(ierr);

  /* check some fields were registered */
  if (swarm->db->nfields <= 2) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"At least one field user must be registered via DMSwarmRegisterXXX()");

  /* check local sizes were set */
  if (swarm->db->L == -1) SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_USER,"Local sizes must be set via DMSwarmSetLocalSizes()");

  /* initialize values in pid and rank placeholders */
  /* TODO: [pid - use MPI_Scan] */
  
  ierr = MPI_Comm_rank(PetscObjectComm((PetscObject)dm),&rank);CHKERRQ(ierr);
  ierr = DataBucketGetSizes(swarm->db,&npoints,NULL,NULL);CHKERRQ(ierr);
  ierr = DMSwarmGetField(dm,DMSwarmField_rank,NULL,NULL,(void**)&rankval);CHKERRQ(ierr);
  for (p=0; p<npoints; p++) {
    rankval[p] = (PetscInt)rank;
  }
  ierr = DMSwarmRestoreField(dm,DMSwarmField_rank,NULL,NULL,(void**)&rankval);CHKERRQ(ierr);
  
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "DMDestroy_Swarm"
PetscErrorCode DMDestroy_Swarm(DM dm)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = DataBucketDestroy(&swarm->db);CHKERRQ(ierr);
  ierr = PetscFree(swarm);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "DMSwarmView_Draw"
PetscErrorCode DMSwarmView_Draw(DM dm, PetscViewer viewer)
{
  DM             cdm;
  PetscDraw      draw;
  PetscReal     *coords, oldPause;
  PetscInt       Np, p, bs;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = PetscViewerDrawGetDraw(viewer, 0, &draw);CHKERRQ(ierr);
  ierr = DMSwarmGetCellDM(dm, &cdm);CHKERRQ(ierr);
  ierr = PetscDrawGetPause(draw, &oldPause);CHKERRQ(ierr);
  ierr = PetscDrawSetPause(draw, 0.0);CHKERRQ(ierr);
  ierr = DMView(cdm, viewer);CHKERRQ(ierr);
  ierr = PetscDrawSetPause(draw, oldPause);CHKERRQ(ierr);

  ierr = DMSwarmGetLocalSize(dm, &Np);CHKERRQ(ierr);
  ierr = DMSwarmGetField(dm, DMSwarmPICField_coor, &bs, NULL, (void **) &coords);CHKERRQ(ierr);
  for (p = 0; p < Np; ++p) {
    const PetscInt i = p*bs;

    ierr = PetscDrawEllipse(draw, coords[i], coords[i+1], 0.01, 0.01, PETSC_DRAW_BLUE);CHKERRQ(ierr);
  }
  ierr = DMSwarmRestoreField(dm, DMSwarmPICField_coor, &bs, NULL, (void **) &coords);CHKERRQ(ierr);
  ierr = PetscDrawFlush(draw);CHKERRQ(ierr);
  ierr = PetscDrawPause(draw);CHKERRQ(ierr);
  ierr = PetscDrawSave(draw);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "DMView_Swarm"
PetscErrorCode DMView_Swarm(DM dm, PetscViewer viewer)
{
  DM_Swarm *swarm = (DM_Swarm*)dm->data;
  PetscBool      iascii,ibinary,ishdf5,isvtk,isdraw;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(dm,DM_CLASSID,1);
  PetscValidHeaderSpecific(viewer,PETSC_VIEWER_CLASSID,2);
  ierr = PetscObjectTypeCompare((PetscObject)viewer, PETSCVIEWERASCII, &iascii);CHKERRQ(ierr);
  ierr = PetscObjectTypeCompare((PetscObject)viewer, PETSCVIEWERBINARY,&ibinary);CHKERRQ(ierr);
  ierr = PetscObjectTypeCompare((PetscObject)viewer, PETSCVIEWERVTK,   &isvtk);CHKERRQ(ierr);
  ierr = PetscObjectTypeCompare((PetscObject)viewer, PETSCVIEWERHDF5,  &ishdf5);CHKERRQ(ierr);
  ierr = PetscObjectTypeCompare((PetscObject)viewer, PETSCVIEWERDRAW,  &isdraw);CHKERRQ(ierr);
  if (iascii) {
    ierr = DataBucketView(PetscObjectComm((PetscObject)dm),swarm->db,NULL,DATABUCKET_VIEW_STDOUT);CHKERRQ(ierr);
  } else if (ibinary) {
    SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_SUP,"NO Binary support");
  } else if (ishdf5) {
#if defined(PETSC_HAVE_HDF5)
    SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_SUP,"NO HDF5 support");
#else
    SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_SUP,"HDF5 not supported. Please reconfigure using --download-hdf5");
#endif
  } else if (isvtk) {
    SETERRQ(PetscObjectComm((PetscObject)dm),PETSC_ERR_SUP,"NO VTK support");
  } else if (isdraw) {
    ierr = DMSwarmView_Draw(dm, viewer);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

/*MC
 
 DMSWARM = "swarm" - A DM object used to represent arrays of data (fields) of arbitrary data type.
 This implementation was designed for particle-in-cell type methods in which the underlying
 data required to be represented is both (i) dynamic in length, (ii) and of arbitrary data type.
 
 User data can be represented by DMSwarm through a registring "fields". 
 To register a field, the user must provide:
 (a) a unique name
 (b) the data type (or size in bytes)
 (c) the block size of the data
 
 For example, suppose the application requires a unique id, energy, momentum and density to be stored
 on a set of of particles. Then the following application could be used
 
 DMSwarmInitializeFieldRegister(dm)
 DMSwarmRegisterPetscDatatypeField(dm,"uid",1,PETSC_LONG);
 DMSwarmRegisterPetscDatatypeField(dm,"energy",1,PETSC_REAL);
 DMSwarmRegisterPetscDatatypeField(dm,"momentum",3,PETSC_REAL);
 DMSwarmRegisterPetscDatatypeField(dm,"density",1,PETSC_FLOAT);
 DMSwarmFinalizeFieldRegister(dm)
 
 The fields represented by DMSwarm are dynamic and can be re-sized at any time.
 The only restriction imposed by DMSwarm is that all fields contain the same number of points

 To support particle methods, "migration" techniques are provided. These methods migrate data
 between MPI-ranks.
 
 DMSwarm supports the methods DMCreateGlobalVector() and DMCreateLocalVector(). 
 As a DMSwarm may internally define and store values of different data types, 
 before calling DMCreate{Global/Local}Vector() the user must inform DMSwarm which 
 fields should be used to define a Vec object via
   DMSwarmVectorDefineField()
 The specified field can can changed be changed at any time - thereby permitting vectors 
 compatable with different fields to be created.
 
 A dual representation of fields in the DMSwarm and a Vec object are permitted via 
   DMSwarmCreateGlobalVectorFromField()
 Here the data defining the field in the DMSwarm is shared with a Vec. 
 This is inherently unsafe if you alter the size of the field at any time between
 calls to DMSwarmCreateGlobalVectorFromField() and DMSwarmDestroyGlobalVectorFromField().
 If the local size of the DMSwarm does not match the localsize of the global vector
 when DMSwarmDestroyGlobalVectorFromField() is called, an error is thrown.
 
 Level: beginner
 
 .seealso: DMType, DMCreate(), DMSetType()
 
M*/
#undef __FUNCT__
#define __FUNCT__ "DMCreate_Swarm"
PETSC_EXTERN PetscErrorCode DMCreate_Swarm(DM dm)
{
  DM_Swarm      *swarm;
  PetscErrorCode ierr;
  
  PetscFunctionBegin;
  PetscValidHeaderSpecific(dm, DM_CLASSID, 1);
  ierr     = PetscNewLog(dm,&swarm);CHKERRQ(ierr);
  dm->data = swarm;
  
  ierr = DataBucketCreate(&swarm->db);CHKERRQ(ierr);
  ierr = DMSwarmInitializeFieldRegister(dm);CHKERRQ(ierr);

  swarm->vec_field_set = PETSC_FALSE;
  swarm->issetup = PETSC_FALSE;
  swarm->swarm_type = DMSWARM_BASIC;
  swarm->migrate_type = DMSWARM_MIGRATE_BASIC;
  swarm->collect_type = DMSWARM_COLLECT_BASIC;
  swarm->migrate_error_on_missing_point = PETSC_FALSE;
  
  swarm->dmcell = NULL;
  swarm->collect_view_active = PETSC_FALSE;
  swarm->collect_view_reset_nlocal = -1;
  
  dm->dim  = 0;
  dm->ops->view                            = DMView_Swarm;
  dm->ops->load                            = NULL;
  dm->ops->setfromoptions                  = NULL;
  dm->ops->clone                           = NULL;
  dm->ops->setup                           = DMSetup_Swarm;
  dm->ops->createdefaultsection            = NULL;
  dm->ops->createdefaultconstraints        = NULL;
  dm->ops->createglobalvector              = DMCreateGlobalVector_Swarm;
  dm->ops->createlocalvector               = DMCreateLocalVector_Swarm;
  dm->ops->getlocaltoglobalmapping         = NULL;
  dm->ops->createfieldis                   = NULL;
  dm->ops->createcoordinatedm              = NULL;
  dm->ops->getcoloring                     = NULL;
  dm->ops->creatematrix                    = NULL;
  dm->ops->createinterpolation             = NULL;
  dm->ops->getaggregates                   = NULL;
  dm->ops->getinjection                    = NULL;
  dm->ops->refine                          = NULL;
  dm->ops->coarsen                         = NULL;
  dm->ops->refinehierarchy                 = NULL;
  dm->ops->coarsenhierarchy                = NULL;
  dm->ops->globaltolocalbegin              = NULL;
  dm->ops->globaltolocalend                = NULL;
  dm->ops->localtoglobalbegin              = NULL;
  dm->ops->localtoglobalend                = NULL;
  dm->ops->destroy                         = DMDestroy_Swarm;
  dm->ops->createsubdm                     = NULL;
  dm->ops->getdimpoints                    = NULL;
  dm->ops->locatepoints                    = NULL;
  
  PetscFunctionReturn(0);
}
