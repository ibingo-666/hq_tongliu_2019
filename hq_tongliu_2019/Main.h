#pragma once

#include "Func.h"
#include "MainData.h"



//项目中的所有数据
class ClassProjectAllData
{
public:
	ClassProjectAllData()
	{
		//我的方法类   通用类自定义方法与cad方法
		MyFunction = new ClassMyFunction();

		//功能数据类
		MainData = new ClassMainData();


	}
	~ClassProjectAllData()
	{

	}


public:
	//我的方法类   通用类自定义方法与cad方法
	ClassMyFunction *MyFunction;	
				
public:
	//功能数据类 画图
	ClassMainData *MainData;

	
public:
	//全局变量		
	//excel数据文件路径
	CString EXCELPath;		
	//xml文件路径
	CString XMLPath;		
	//资源路径
	CString ResPath;		



public:

};
	


//引用全局变量
extern ClassProjectAllData *Data;								//项目中的所有数据
