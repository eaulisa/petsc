#ifdef PETSC_RCS_HEADER
"$Id: petscconf.h,v 1.14 1999/11/05 14:42:55 bsmith Exp bsmith $"
"Defines the configuration for this machine"
#endif

#if !defined(INCLUDED_PETSCCONF_H)
#define INCLUDED_PETSCCONF_H

#define PARCH_freebsd
#define PETSC_ARCH_NAME "freebsd"

#define PETSC_HAVE_LIMITS_H
#define PETSC_HAVE_PWD_H 
#define PETSC_HAVE_STDLIB_H 
#define PETSC_HAVE_STRING_H 
#define PETSC_HAVE_DRAND48  
#define PETSC_HAVE_GETDOMAINNAME 
#define PETSC_HAVE_UNISTD_H  
#define PETSC_HAVE_UNAME 
#define PETSC_HAVE_SYS_TIME_H

#define PETSC_HAVE_READLINK
#define PETSC_HAVE_MEMMOVE
#define PETSC_USE_DYNAMIC_LIBRARIES

#define PETSC_HAVE_FORTRAN_UNDERSCORE_UNDERSCORE
#define PETSC_HAVE_FORTRAN_UNDERSCORE

#if (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#define PETSC_HAVE_VPRINTF_CHAR
#endif
#define PETSC_HAVE_SYS_RESOURCE_H
#define PETSC_SIZEOF_VOIDP 4
#define PETSC_SIZEOF_INT 4
#define PETSC_SIZEOF_DOUBLE 8

#endif
