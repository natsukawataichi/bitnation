#pragma once

#if defined(_WIN32) || defined(_WIN64)
# undef HC_WIN
# define HC_WIN 1
#endif

#if defined(HC_WIN)
// lang/builtin/modules/socket.h
# include <winsock2.h>
# include <ws2tcpip.h>
# include <windows.h>
// lib/term.h
# include <wincon.h>
#else
# undef _BSD_SOURCE
# define _BSD_SOURCE 1 /* netdb.h */
# undef __USE_POSIX
# define __USE_POSIX 1 /* netdb.h */
# undef __USE_XOPEN2K
# define __USE_XOPEN2K 1 /* netdb.h */
// lang/builtin/modules/socket.h
# include <netdb.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
#endif

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <getopt.h>

#include <iostream>
#include <vector>
#include <list>
#include <sstream>
#include <fstream>
