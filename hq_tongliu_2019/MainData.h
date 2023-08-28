#pragma once

#include <vector>
#include "hq_tongliu.h"
using namespace std;

class ClassMainData
{
public:
	ClassMainData();
	virtual ~ClassMainData() {}


public:
	//通流类
	ClassTongliu* tl;

public:
//******数据
	
	//通流图勾画
	bool Export();


};

