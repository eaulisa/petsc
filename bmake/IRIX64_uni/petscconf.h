#ifdef PETSC_RCS_HEADER
"$Id: petscconf.h,v 1.20 1999/11/05 14:42:55 bsmith Exp bsmith $"
"Defines the configuration for this machine"
#endif

#if !defined(INCLUDED_PETSCCONF_H)
#define INCLUDED_PETSCCONF_H
 
#define PARCH_IRIX64 
#define PETSC_ARCH_NAME "IRIX64"
#define PETSC_HAVE_LIMITS_H
#define PETSC_HAVE_PWD_H 
#define PETSC_HAVE_STRING_H 
#define PETSC_HAVE_STROPTS_H 
#define PETSC_HAVE_MALLOC_H 
#define PETSC_HAVE_DRAND48 
#define PETSC_HAVE_GETDOMAINNAME 
#define PETSC_HAVE_UNAME 
#define PETSC_HAVE_UNISTD_H 
#define PETSC_HAVE_STDLIB_H
#define PETSC_HAVE_SYS_TIME_H 
#define PETSC_HAVE_SYS_UTSNAME_H
#define PETSC_USE_SHARED_MEMORY

#define PETSC_HAVE_FORTRAN_UNDERSCORE 
#define PETSC_SIZEOF_VOIDP 8
#define PETSC_SIZEOF_INT 4
#define PETSC_SIZEOF_DOUBLE 8

#define PETSC_HAVE_IRIXF90

#define PETSC_HAVE_IRIXF90

#define PETSC_WORDS_BIGENDIAN 1

#define PETSC_HAVE_MEMMOVE

#define PETSC_HAVE_DOUBLE_ALIGN
#define PETSC_HAVE_DOUBLE_ALIGN_MALLOC

#define PETSC_HAVE_MEMALIGN

#define PETSC_HAVE_FAST_MPI_WTIME

#define PETSC_USE_DBX_DEBUGGER
#define PETSC_HAVE_SYS_RESOURCE_H


#define PETSC_HAVE_RTLD_GLOBAL 1

#define PETSC_CAN_SLEEP_AFTER_ERROR

#define PETSC_HAVE_4ARG_SIGNAL_HANDLER
#define PETSC_USE_KBYTES_FOR_SIZE
#define PETSC_USE_P_FOR_DEBUGGER

#endif
