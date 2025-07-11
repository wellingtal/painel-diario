#include "DungeonMgr.h"

using namespace VirtualArea;

CDungeonMgr& CDungeonMgr::GetInstance()
{
	static CDungeonMgr ms_this;
	return ms_this;
}
