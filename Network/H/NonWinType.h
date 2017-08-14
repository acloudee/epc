#ifndef _NonWinType_H_
#define _NonWinType_H_

#ifdef _WIN32
// include win32 header
#elif defined(_LINUX_)
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../Utility/H/Common.h"
#else
#include <socklib.h>
#include <inetlib.h>
#include <types.h>
#include <resolvLib.h>
#include <hostLib.h>
#include "../../Utility/h/Common.h"
#endif

PROJECT_NS_BEGIN

#ifdef _WIN32
//
#elif defined(_LINUX_)	// LINUX

typedef int			BOOL;
typedef float		FLOAT;
typedef FLOAT		*PFLOAT;
typedef bool		*PBOOL;
typedef bool		*LPBOOL;
typedef BYTE		*LPBYTE;
typedef int			*PINT;
typedef int			*LPINT;
typedef WORD		*LPWORD;
typedef long		*LPLONG;
typedef DWORD		*LPDWORD;
typedef void		*LPVOID;
typedef const char	*LPCTSTR;
typedef const char	*LPCSTR;
typedef long		LONG;
typedef char		TCHAR;
typedef int 		SOCKET;

#define ERROR				-1
#define INVALID_SOCKET		ERROR 
#define SOCKET_ERROR		-1
#define WSAGetLastError()	errno
#define WSAETIMEDOUT		ETIME

#define MAX_PATH		260
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

#else	// VXWORKS

typedef int			BOOL;
typedef float		FLOAT;
typedef FLOAT		*PFLOAT;
typedef bool		*PBOOL;
typedef bool		*LPBOOL;
typedef BYTE		*LPBYTE;
typedef int			*PINT;
typedef int			*LPINT;
typedef WORD		*LPWORD;
typedef long		*LPLONG;
typedef DWORD		*LPDWORD;
typedef void		*LPVOID;
typedef const char	*LPCTSTR;
typedef const char	*LPCSTR;
typedef long		LONG;
typedef char		TCHAR;
typedef int 		SOCKET;

#define INVALID_SOCKET		ERROR
#define SOCKET_ERROR		-1
#define WSAGetLastError()	errno
#define WSAETIMEDOUT		ETIME

#define MAX_PATH		260
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

#endif	// _WIN32

PROJECT_NS_END

#endif//_NonWinType_H_
