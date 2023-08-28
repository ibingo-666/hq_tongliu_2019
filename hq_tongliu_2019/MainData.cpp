#include "stdafx.h"
#include "MainData.h"
#include <math.h>
#include "tinyxml.h"
#include<string.h>
#include <io.h>
#include <tchar.h>
#include "hq_tongliu.h"
#include "Main.h"

ClassMainData::ClassMainData()
{
	tl = new ClassTongliu();
}

//»­Í¼
bool ClassMainData::Export()
{
	Data->MainData->tl->Export();

	return true;
}

