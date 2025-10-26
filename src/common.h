#ifndef COMMON_H
#define COMMON_H

#if !defined(__linux__)
#  error "This program compiles only on a Linux operating system."
#endif

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 202311L)
#  error "gcc >= 15 is required."
#endif

#endif
