#include "DuelMgr.h"

using namespace VirtualArea;

CDuelMgr& CDuelMgr::GetInstance()
{
	static CDuelMgr ms_this;
	return ms_this;
}
