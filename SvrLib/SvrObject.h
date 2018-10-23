#pragma once
#include "SvrUtil.h"

class SvrObject
{
public:
	SvrObject();
	virtual ~SvrObject();
private:
	CRITICAL_SECTION m_cs;
};
