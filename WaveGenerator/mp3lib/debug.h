//
// デバック用メッセージボックス
//

#ifndef __DEBUG_MSG__
#define __DEBUG_MSG__

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <string>
using namespace std;

void dbgmsg(int Value);
void dbgmsg(long Value);
void dbgmsg(DWORD Value);
void dbgmsg(char *Value);

void dbgmsg(int Value)
{
	char tmp[100];
	wsprintf(tmp,"%i",Value);
	MessageBox(NULL,tmp,"",MB_OK);
}

void dbgmsg(long Value)
{
	char tmp[100];
	wsprintf(tmp,"%ld",Value);
	MessageBox(NULL,tmp,"",MB_OK);
}

void dbgmsg(DWORD Value)
{
	char tmp[100];
	wsprintf(tmp,"%ld",Value);
	MessageBox(NULL,tmp,"",MB_OK);
}

void dbgmsg(char *Value)
{
	MessageBox(NULL,Value,"",MB_OK);
}

#endif