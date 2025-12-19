#ifndef PLATFORM_NOTIFICATION_H
#define PLATFORM_NOTIFICATION_H

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
typedef int BOOL;
typedef unsigned long DWORD;

#define NIM_ADD 0x00000000
#define NIM_MODIFY 0x00000001
#define NIM_DELETE 0x00000002

#define NIF_MESSAGE 0x00000001
#define NIF_ICON 0x00000002
#define NIF_TIP 0x00000004
#define NIF_INFO 0x00000010

#define NIIF_NONE 0x00000000
#define NIIF_INFO 0x00000001
#define NIIF_WARNING 0x00000002
#define NIIF_ERROR 0x00000003

typedef struct _NOTIFYICONDATAA {
  DWORD cbSize;
  void *hWnd;
  DWORD uID;
  DWORD uFlags;
  DWORD uCallbackMessage;
  void *hIcon;
  char szTip[128];
  DWORD dwState;
  DWORD dwStateMask;
  char szInfo[256];
  union {
    DWORD uTimeout;
    DWORD uVersion;
  } DUMMYUNIONNAME;
  char szInfoTitle[64];
  DWORD dwInfoFlags;
  void *guidItem;
  void *hBalloonIcon;
} NOTIFYICONDATAA;

__declspec(dllimport) BOOL __stdcall Shell_NotifyIconA(DWORD dwMessage,
                                                       NOTIFYICONDATAA *lpData);
#endif

static inline void platform_raise_notification(const char *title, const char *message) {
#ifdef _WIN32
  NOTIFYICONDATAA nid;
  memset(&nid, 0, sizeof(nid));
  nid.cbSize = sizeof(nid);
  nid.uFlags = NIF_INFO;
  strncpy(nid.szInfoTitle, title, sizeof(nid.szInfoTitle) - 1);
  strncpy(nid.szInfo, message, sizeof(nid.szInfo) - 1);
  nid.dwInfoFlags = NIIF_INFO;

  Shell_NotifyIconA(NIM_ADD, &nid);
  Shell_NotifyIconA(NIM_MODIFY, &nid);
// TODO: Ignoring MAC os
#else
  char command[512];
  snprintf(command, sizeof(command), "notify-send -a Pomidoras \"%s\" \"%s\"",
           title, message);
  system(command);
#endif
}
#endif // PLATFORM_NOTIFICATION_H
