#ifdef PETSC_RCS_HEADER
"$Id: petscconf.h,v 1.17 1999/09/16 19:02:40 balay Exp bsmith $"
"Defines the configuration for this machine"
#endif

#if !defined(INCLUDED_PETSCCONF_H)
#define INCLUDED_PETSCCONF_H

#define PARCH_rs6000 
#define PETSC_ARCH_NAME "rs6000"
#define PETSC_USE_IBM_ASM_CLOCK

#define PETSC_HAVE_LIMITS_H
#define PETSC_HAVE_STROPTS_H 
#define PETSC_HAVE_SEARCH_H 
#define PETSC_HAVE_PWD_H 
#define PETSC_HAVE_STDLIB_H
#define PETSC_HAVE_STRING_H 
#define PETSC_HAVE_STRINGS_H 
#define PETSC_HAVE_MALLOC_H 
#define _POSIX_SOURCE
#define PETSC_HAVE_DRAND48  
#define PETSC_HAVE_GETDOMAINNAME  
#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 
#endif
#define PETSC_HAVE_UNISTD_H 
#define PETSC_HAVE_SYS_TIME_H 
#define PETSC_NEEDS_UTYPE_TYPEDEFS 
#define _XOPEN_SOURCE_EXTENDED 1
#define PETSC_HAVE_UNAME  
#define PETSC_HAVE_BROKEN_REQUEST_FREE 
#define PETSC_HAVE_TEMPLATED_COMPLEX
#define PETSC_HAVE_DOUBLE_ALIGN_MALLOC

#define PETSC_HAVE_FORTRAN_UNDERSCORE 
#define PETSC_HAVE_FORTRAN_UNDERSCORE_UNDERSCORE

#define PETSC_HAVE_READLINK
#define PETSC_HAVE_MEMMOVE
#define PETSC_HAVE_SYS_RESOURCE_H

#define PETSC_SIZEOF_VOIDP 4
#define PETSC_SIZEOF_INT 4
#define PETSC_SIZEOF_DOUBLE 8

#define PETSC_WORDS_BIGENDIAN 1
#define PETSC_NEED_SOCKET_PROTO
#define PETSC_HAVE_ACCEPT_SIZE_T

#define PETSC_HAVE_SLEEP_RETURNS_EARLY
#define PETSC_USE_KBYTES_FOR_SIZE
#define PETSC_USE_A_FOR_DEBUGGER
#endif
