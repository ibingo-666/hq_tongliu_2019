// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//- hq_tongliu.cpp : Initialization functions
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include <afxdllx.h>
#include<stdio.h>
#include<math.h>
#include "Main.h"

#include "hq_tongliu.h"
#include "Func.h"
#include <iostream>
#include <string>
#define PI 3.14159265358979323846264338327950288419716939937510


//-----------------------------------------------------------------------------
//- Define the sole extension module object.
AC_IMPLEMENT_EXTENSION_MODULE(hq_tongliuDLL)
//- Please do not remove the 3 following lines. These are here to make .NET MFC Wizards
//- running properly. The object will not compile but is require by .NET to recognize
//- this project as being an MFC project
#ifdef NEVER
AFX_EXTENSION_MODULE hq_tongliuExtDLL ={ NULL, NULL } ;
#endif

//- Now you can use the CAcModuleResourceOverride class in
//- your application to switch to the correct resource instance.
//- Please see the ObjectARX Documentation for more details

//-----------------------------------------------------------------------------
//- DLL Entry Point
extern "C"
BOOL WINAPI DllMain (HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	//- Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved) ;

	if ( dwReason == DLL_PROCESS_ATTACH ) {
        _hdllInstance =hInstance ;
		hq_tongliuDLL.AttachInstance (hInstance) ;
		InitAcUiDLL () ;
	} else if ( dwReason == DLL_PROCESS_DETACH ) {
		hq_tongliuDLL.DetachInstance () ;
	}
	return (TRUE) ;
}
ClassTongliu::ClassTongliu()
{
	iCurrentRow = 0;
	iSheetIndex=0;
	rowIndex = 4;
	aircylinderDataVec.clear();
}

void ClassTongliu::init_cad()
{
	Data->MyFunction->Arx->CreateLayer(L"1粗实线", 7, L"CONTINUOUS");
	Data->MyFunction->Arx->CreateLayer(L"2细实线", 1, L"CONTINUOUS");	//红色细实线
	Data->MyFunction->Arx->CreateLayer(L"3中心线", 2, L"CENTER");		//黄色点划线
	Data->MyFunction->Arx->CreateLayer(L"4虚线", 2, L"DASHED");
	Data->MyFunction->Arx->CreateLayer(L"断面线", 2, L"CONTINUOUS");	//黄色断面线
	Data->MyFunction->Arx->CreateLayer(L"5尺寸线", 1, L"CONTINUOUS");
	Data->MyFunction->Arx->CreateLayer(L"文字", 3, L"CONTINUOUS");
	Data->MyFunction->Arx->CreateLayer(L"6剖面线", 1, L"CONTINUOUS");	//红色抛面线
	Data->MyFunction->Arx->CreateLayer(L"青玉案",5, L"CONTINUOUS");		//蓝色细实线
	Data->MyFunction->Arx->CreateLayer(L"双点划线",6, L"DIVIDE2");		//粉色电话线
	Data->MyFunction->Arx->CreateLayer(L"细点划线", 4, L"CENTER");		//青色点划线
	Data->MyFunction->Arx->CreateLayer(L"图框", 2, L"CONTINUOUS");		//黄色
	Data->MyFunction->Arx->CreateLayer(L"断面线", 2, L"CONTINUOUS");	//黄色断面线

	//导入图块
	CString BlockPath =setting.ResPath+ "\\QFC_T.dwg";
	Data->MyFunction->Arx->ImportBlock(BlockPath);
	BlockPath =setting.ResPath+ "\\LastThreeStages.dwg";
	Data->MyFunction->Arx->ImportBlock(BlockPath);
	//acedAlert(BlockPath);
}


//************数据处理

//读取叶片数据 
int ClassTongliu::getBeBladeData()
{
	setlocale(LC_ALL,"Chinese-simplified");//设置中文环境 

	//string expath = "C:\\Users\\ibingo\\Desktop\\test.xls";
	//"C:\\Users\\ibingo\\Desktop\\AirPassageData.xls";
	string expath = Data->MyFunction->Basic->CString2Char(Data->EXCELPath);
	//BasicExcel beObject;
	beObject.Load(expath.c_str());

	bewCurrentSheet = beObject.GetWorksheet("Sheet1");
	
	if(bewCurrentSheet)
	{
		this->iCurrentRow = bewCurrentSheet->GetTotalRows();

		if(iCurrentRow < rowIndex)
		{
			return -2;//sheet页结构出错
		}
		
		BasicExcelCell* becCurrentCell;
		double p0Jz_1_num =0.0;
		string AirPassage_str ,OriginalPoint_str,LastThreeStages_str;
		double OutNum = 0.0;

		//AirPassage 气道类型
		becCurrentCell = bewCurrentSheet->Cell(0,3);
		if(isCellString(becCurrentCell,AirPassage_str))
			aircylinderTmp.AirPassage = AirPassage_str;

		//isHorizonBlade 是否含有横置叶片 1是0否
		becCurrentCell = bewCurrentSheet->Cell(0,5);
		if(isCellDouble(becCurrentCell,OutNum))
			aircylinderTmp.isHorizonBlade = OutNum;

		//通流图原点
		becCurrentCell = bewCurrentSheet->Cell(0,7);
		if(isCellString( becCurrentCell,OriginalPoint_str))
		{
			string first, second;
			double firstValue, secondValue;
			splitString(OriginalPoint_str, first, second, firstValue, secondValue);
			setting.OriginalPoint = AcGePoint3d(firstValue,secondValue,0);
		}

		//气道中心线轴向距离
		becCurrentCell = bewCurrentSheet->Cell(0,9);
		if(isCellDouble(becCurrentCell,OutNum))
			aircylinderTmp.AirAxialDis = OutNum;

		//汽道中心宽度
		becCurrentCell = bewCurrentSheet->Cell(0,11);
		if(isCellDouble(becCurrentCell,OutNum))
			aircylinderTmp.AirWeight = OutNum;

		//悬臂宽度
		becCurrentCell = bewCurrentSheet->Cell(0,13);
		if(isCellDouble(becCurrentCell,OutNum))
			aircylinderTmp.CantileverWidth = OutNum;

		//末三级选型
		becCurrentCell = bewCurrentSheet->Cell(0,15);
		if(isCellString(becCurrentCell,LastThreeStages_str))
			aircylinderTmp.LastThreeStages = LastThreeStages_str;

		//p0Jz_1
		becCurrentCell = bewCurrentSheet->Cell(4,2);
		if(isCellDouble( becCurrentCell,p0Jz_1_num))
		{
			//当有横置叶片时
			if(aircylinderTmp.isHorizonBlade==1.0)
			{
				//p30
				becCurrentCell = bewCurrentSheet->Cell(4,33);
				if(isCellDouble( becCurrentCell,p0Jz_1_num))
					aircylinderTmp.p0Jz_1 = p0Jz_1_num;
			}
			else
			{
				aircylinderTmp.p0Jz_1 = 0.0;
			}
		}
			
		vector<bladeData>bladeDataVec;

#pragma region
		for(int i=rowIndex;i<iCurrentRow;i++)
		{	
			bladeData blade_tmp;
			double thNum =0.0;

			becCurrentCell = bewCurrentSheet->Cell(0,1);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.Margin = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,35);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.ForwardSpace = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,36);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.NegativeSpace = thNum;
			else
				continue;

			//正胀差
			becCurrentCell = bewCurrentSheet->Cell(i,37);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.P = thNum;
			else
				continue;

			//负胀差
			becCurrentCell = bewCurrentSheet->Cell(i,38);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.Q = thNum;
			else
				continue;

			//N
			becCurrentCell = bewCurrentSheet->Cell(i,39);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.N = thNum;
			else
				continue;

			//稳态胀差
			becCurrentCell = bewCurrentSheet->Cell(i,40);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.W = thNum;
			else
				continue;

			//Hmin
			becCurrentCell = bewCurrentSheet->Cell(i,41);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.Hmin = thNum;
			else
				continue;

			//Jmin
			becCurrentCell = bewCurrentSheet->Cell(i,42);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.Jmin = thNum;
			else
				continue;

			//β进气侧倾角
			becCurrentCell = bewCurrentSheet->Cell(i,43);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.β = thNum;
			else
				continue;

			//p32d /每级动叶为了修正离心力及重心对围带挖槽的宽度
			becCurrentCell = bewCurrentSheet->Cell(i,44);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p32d = thNum;
			else
				continue;

			//c_deep 槽深度常数尺寸
			becCurrentCell = bewCurrentSheet->Cell(i,45);
			if(isCellDouble( becCurrentCell,thNum))
			{
				blade_tmp.c_deep = thNum;
				blade_tmp.Sc_deep = thNum;
			}
			else
				continue;


			//静叶
			//becCurrentCell = bewCurrentSheet->Cell(i,2);
			//if(isCellDouble( becCurrentCell,thNum))
			//	blade_tmp.p0Jz = thNum;
			//else
			//	continue;

			becCurrentCell = bewCurrentSheet->Cell(i,3);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p1BG = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,4);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p2BD = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,5);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p3bg = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,6);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p4bd = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,7);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p5Bw = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,8);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p6bw = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,9);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p7hw = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,10);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p8Bz = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,11);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p9bz = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,12);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p10hz = thNum;
			else
				continue;


			becCurrentCell = bewCurrentSheet->Cell(i,13);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p11ad = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,14);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p12ag = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,15);
			//blade_tmp.SHubType =becCurrentCell->GetString();
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.SHubType = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,29);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p26Dpp = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,30);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p27Lp = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,34);
			if(isCellDouble( becCurrentCell,thNum))
			{
				blade_tmp.p31zf = thNum;
				blade_tmp.BrokenTag = 0.0;
				blade_tmp.BrokenDis = 0.0;
				if(bladeDataVec.size()!=0)
				{
					bladeData last = bladeDataVec[bladeDataVec.size()-1];
					if(last.p31zf != thNum)
					{
						blade_tmp.BrokenTag = 1.0;
						becCurrentCell = bewCurrentSheet->Cell(i,2);
						if(isCellDouble( becCurrentCell,thNum))
							blade_tmp.BrokenDis = thNum;
						else
							blade_tmp.BrokenDis = 0.0;
					}
				}
			}
			else
				continue;

			
			//动叶
			becCurrentCell = bewCurrentSheet->Cell(i,16);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p13BG = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,17);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p14BD = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,18);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p15bg = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,19);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p16bd = thNum;
			else
				continue;
			becCurrentCell = bewCurrentSheet->Cell(i,20);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p17Bw = thNum;
			else
				continue;
			
			becCurrentCell = bewCurrentSheet->Cell(i,21);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p18bw = thNum;
			else
				continue;
			
			becCurrentCell = bewCurrentSheet->Cell(i,22);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p19hw = thNum;
			else
				continue;
			
			becCurrentCell = bewCurrentSheet->Cell(i,23);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p21Bz = thNum;
			else
				continue;
			
			becCurrentCell = bewCurrentSheet->Cell(i,24);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p22bz = thNum;
			else
				continue;
			
			becCurrentCell = bewCurrentSheet->Cell(i,25);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p23hz = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,26);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p24ad = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,27);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p25ag = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,28);
			if(isCellDouble(becCurrentCell,thNum))
			{
				blade_tmp.HubType = thNum;
			}	
			else
				continue;
			

			becCurrentCell = bewCurrentSheet->Cell(i,31);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p28Dpd = thNum;
			else
				continue;

			becCurrentCell = bewCurrentSheet->Cell(i,32);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p29Ld = thNum;
			else
				continue;

			//becCurrentCell = bewCurrentSheet->Cell(i,33);
			//if(isCellDouble( becCurrentCell,thNum))
			//	blade_tmp.p30Dz = thNum;
			//else
			//	continue;

			//抽口数据
			//IsSuctionPort 是否含有抽口，1为有，0为无
			becCurrentCell = bewCurrentSheet->Cell(i,46);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.IsSuctionPort = thNum;
			else
				continue;

			//InletWeight 进气侧轴向宽度，非空
			becCurrentCell = bewCurrentSheet->Cell(i,47);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.InletWeight = thNum;
			else
				continue;

			//OutletWeight 出气侧轴向宽度，非空
			becCurrentCell = bewCurrentSheet->Cell(i,48);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.OutletWeight = thNum;
			else
				continue;

			//CoverDegree 动静叶顶盖度，非空
			becCurrentCell = bewCurrentSheet->Cell(i,49);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.CoverDegree = thNum;
			else
				continue;

			//V 蒸汽比容V，非空
			becCurrentCell = bewCurrentSheet->Cell(i,50);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.V = thNum;
			else
				continue;

			//G 抽汽流量G，非空
			becCurrentCell = bewCurrentSheet->Cell(i,51);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.G = thNum;
			else
				continue;

			//最大允许流量MaxAllowFlow，XML 高压低压中压，无
			becCurrentCell = bewCurrentSheet->Cell(i,52);
			string MaxAllowFlow_str ;
			if(isCellString( becCurrentCell,MaxAllowFlow_str))
				blade_tmp.MaxAllowFlow = MaxAllowFlow_str;
			else
				continue;

			//抽口中间轴向宽度Y
			becCurrentCell = bewCurrentSheet->Cell(i,53);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.MidY = thNum;
			else
				continue;



			bladeDataVec.push_back(blade_tmp);
		}
#pragma endregion
		aircylinderTmp.bladePair = bladeDataVec;
	}
	else
		return -1;//没有sheet页


	return 0;
}



bool ClassTongliu::isCellDouble(BasicExcelCell* cell,double &thNum)
{
	switch (cell->Type())
	{
	case BasicExcelCell::UNDEFINED:
		return false;

	case BasicExcelCell::INT:
		{
			int tmp = cell->GetInteger();
			thNum = (double)tmp;
			return true;
		}
	case BasicExcelCell::DOUBLE:
		{
			thNum = cell->GetDouble();
			return true;
		}
	case BasicExcelCell::STRING:
		return false;

	case BasicExcelCell::WSTRING:
		return false;
	}
}


//判断单元格数据 string
bool ClassTongliu::isCellString(BasicExcelCell* cell,string &thString)
{
	switch (cell->Type())
	{
	case BasicExcelCell::UNDEFINED:
		return false;

	case BasicExcelCell::INT:
		return false;

	case BasicExcelCell::DOUBLE:
		return false;

	case BasicExcelCell::STRING:
		{
			thString = cell->GetString();
			return true;
		}
	case BasicExcelCell::WSTRING:
		{
		    wstring wString = cell->GetWString();
			thString = Data->MyFunction->Basic->wstring2String(wString);
			return true;
		}
	}
}


//,分割string并且转为double
void ClassTongliu::splitString( std::string input, std::string& first, std::string& second, double& firstValue, double& secondValue )
{
	size_t commaPos = input.find("，"); // 查找中文逗号位置
	if (commaPos == std::string::npos) {
		commaPos = input.find(","); // 查找英文逗号位置
	}

	if (commaPos != std::string::npos) {
		first = input.substr(0, commaPos); // 提取第一个数据
		second = input.substr(commaPos + 1); // 提取第二个数据
	} else {
		first = input; // 没有逗号，则认为整个字符串为第一个数据
		second = ""; // 第二个数据为空字符串
	}

	// 转换为 double 类型
	std::stringstream ss(first);
	ss >> firstValue;
	std::stringstream ss1(second);
	ss1 >> secondValue;
}


//将两个double组成string
void ClassTongliu::combineString(double firstValue, double secondValue, std::string& combinedString) 
{
	std::ostringstream oss;
	oss << firstValue << "," << secondValue;
	combinedString = oss.str();
}
//导出通流xml
bool ClassTongliu::ExportXMLData()
{
	CString str;

	TiXmlDocument *myDocument = new TiXmlDocument();
	TiXmlDeclaration *dec = new TiXmlDeclaration("1.0","UTF-8","");
	myDocument->LinkEndChild(dec);

	TiXmlElement *RootElement = new TiXmlElement("Data");

	//Setting <!--  程序参数   -->
	TiXmlElement *SettingElement = new TiXmlElement("Setting");
	RootElement->LinkEndChild(SettingElement);

	//ResPath 资源路径 -- 块
	TiXmlElement *ResPathElement = new TiXmlElement("ResPath");
	SettingElement->LinkEndChild(ResPathElement);
	//acedAlert(Data->ResPath);
	TiXmlText *ResPathContent = new TiXmlText(Data->MyFunction->Basic->CString2Char(Data->ResPath));
	ResPathElement->LinkEndChild(ResPathContent);

	//通流图原点
	TiXmlElement *OriginalPointElement = new TiXmlElement("OriginalPoint");
	SettingElement->LinkEndChild(OriginalPointElement);

	double firstValue = setting.OriginalPoint.x;
	double secondValue = setting.OriginalPoint.y;
	string combinedString;
	combineString(firstValue, secondValue, combinedString);

	TiXmlText *OriginalPointContent = new TiXmlText(combinedString.c_str());
	OriginalPointElement->LinkEndChild(OriginalPointContent);

	//////////////////////////////////////////////////////////////////////////////////

	//AirCylinder <!--  气缸   -->
	TiXmlElement *AirCylinderElement = new TiXmlElement("AirCylinder");
	RootElement->LinkEndChild(AirCylinderElement);
	

	//AirPassage 气道类型
	TiXmlElement *AirPassageElement = new TiXmlElement("AirPassage");
	AirCylinderElement->LinkEndChild(AirPassageElement);
	string str_tmp = aircylinderTmp.AirPassage;
	TiXmlText *AirPassageContent = new TiXmlText(str_tmp.c_str());
	AirPassageElement->LinkEndChild(AirPassageContent);


	//LastThreeStages 末三级选型及定位数据
	TiXmlElement *LastThreeStageseElement = new TiXmlElement("LastThreeStages");
	AirCylinderElement->LinkEndChild(LastThreeStageseElement);
	TiXmlText *LastThreeStageseContent = new TiXmlText(aircylinderTmp.LastThreeStages.c_str());
	LastThreeStageseElement->LinkEndChild(LastThreeStageseContent);

	//气道中心
	TiXmlElement *AirwayCenterElement = new TiXmlElement("AirwayCenter");
	AirCylinderElement->LinkEndChild(AirwayCenterElement);

	//气道中心线轴向距离
	TiXmlElement *AirAxialDisElement = new TiXmlElement("AirAxialDis");
	AirwayCenterElement->LinkEndChild(AirAxialDisElement);
	TiXmlText *AirAxialDisContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(aircylinderTmp.AirAxialDis));
	AirAxialDisElement->LinkEndChild(AirAxialDisContent);

	//汽道中心宽度
	TiXmlElement *AirWeightElement = new TiXmlElement("AirWeight");
	AirwayCenterElement->LinkEndChild(AirWeightElement);
	TiXmlText *AirWeightContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(aircylinderTmp.AirWeight));
	AirWeightElement->LinkEndChild(AirWeightContent);

	//悬臂宽度
	TiXmlElement *CantileverWidthElement = new TiXmlElement("CantileverWidth");
	AirwayCenterElement->LinkEndChild(CantileverWidthElement);
	TiXmlText *CantileverWidthContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(aircylinderTmp.CantileverWidth));
	CantileverWidthElement->LinkEndChild(CantileverWidthContent);

	//isHorizonBlade 是否含有横置叶片 1是0否
	TiXmlElement *IsHorizonBladeElement = new TiXmlElement("IsHorizonBlade");
	AirCylinderElement->LinkEndChild(IsHorizonBladeElement);
	TiXmlText *IsHorizonBladeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(aircylinderTmp.isHorizonBlade));
	IsHorizonBladeElement->LinkEndChild(IsHorizonBladeContent);

	//p0Jz_1<!--  原点到第1级静叶中心线的距离  -->
	TiXmlElement *p0Jz_1Element = new TiXmlElement("p0Jz_1");
	AirCylinderElement->LinkEndChild(p0Jz_1Element);
	TiXmlText *p0Jz_1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(aircylinderTmp.p0Jz_1));
	p0Jz_1Element->LinkEndChild(p0Jz_1Content);

	vector<bladeData>bladeDataVec = aircylinderTmp.bladePair;
	//BladePair<!--  叶片对   -->
	for (int i = 0; i <bladeDataVec.size(); ++i)
	{
		ExportXMLData(AirCylinderElement,bladeDataVec[i]);
	}

	
	//SAVE to %TMP%
	//TCHAR lpTempPathBuffer[MAX_PATH];
	//GetTempPath(MAX_PATH, lpTempPathBuffer);
	//str.Format(L"%s%s", lpTempPathBuffer, L"DQXML\\TLData-update.xml");
	myDocument->LinkEndChild(RootElement);
	str = Data->MyFunction->Basic->CString2Char(Data->XMLPath);//L"C:\\Users\\ibingo\\Desktop\\AirPassageData.xml";
	myDocument->SaveFile(Data->MyFunction->Basic->CString2Char(str));

	return true;

}
//叶片对节点数据导出
bool ClassTongliu::ExportXMLData( TiXmlElement* pElem,bladeData tmp)
{
	CString str;

	//BladePair 叶片对
	TiXmlElement *BladePairElement = new TiXmlElement("BladePair");
	pElem->LinkEndChild(BladePairElement);

	//SuctionPort 抽口位置及数据
	TiXmlElement *SuctionPortElement = new TiXmlElement("SuctionPort");
	BladePairElement->LinkEndChild(SuctionPortElement);
	
	//IsSuctionPort 是否含有抽口，1为有，0为无
	TiXmlElement *IsSuctionPortElement = new TiXmlElement("IsSuctionPort");
	SuctionPortElement->LinkEndChild(IsSuctionPortElement);
	TiXmlText *IsSuctionPortContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.IsSuctionPort));
	IsSuctionPortElement->LinkEndChild(IsSuctionPortContent);

	//InletWeight 进气侧轴向宽度，非空
	TiXmlElement *InletWeightElement = new TiXmlElement("InletWeight");
	SuctionPortElement->LinkEndChild(InletWeightElement);
	TiXmlText *InletWeightContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.InletWeight));
	InletWeightElement->LinkEndChild(InletWeightContent);

	//OutletWeight 出气侧轴向宽度，非空
	TiXmlElement *OutletWeightElement = new TiXmlElement("OutletWeight");
	SuctionPortElement->LinkEndChild(OutletWeightElement);
	TiXmlText *OutletWeightContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.OutletWeight));
	OutletWeightElement->LinkEndChild(OutletWeightContent);

	//CoverDegree 动静叶顶盖度，非空
	TiXmlElement *CoverDegreeElement = new TiXmlElement("CoverDegree");
	SuctionPortElement->LinkEndChild(CoverDegreeElement);
	TiXmlText *CoverDegreeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.CoverDegree));
	CoverDegreeElement->LinkEndChild(CoverDegreeContent);

	//蒸汽比容V，非空
	TiXmlElement *VElement = new TiXmlElement("V");
	SuctionPortElement->LinkEndChild(VElement);
	TiXmlText *VContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.V));
	VElement->LinkEndChild(VContent);

	//抽汽流量G，非空
	TiXmlElement *GElement = new TiXmlElement("G");
	SuctionPortElement->LinkEndChild(GElement);
	TiXmlText *GContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.G));
	GElement->LinkEndChild(GContent);

	//最大允许流量MaxAllowFlow，XML 高压低压中压，无
	TiXmlElement *MaxAllowFlowElement = new TiXmlElement("MaxAllowFlow");
	SuctionPortElement->LinkEndChild(MaxAllowFlowElement);
	TiXmlText *MaxAllowFlowContent = new TiXmlText(tmp.MaxAllowFlow.c_str());
	MaxAllowFlowElement->LinkEndChild(MaxAllowFlowContent);

	//抽口中间轴向宽度Y
	TiXmlElement *MidYElement = new TiXmlElement("MidY");
	SuctionPortElement->LinkEndChild(MidYElement);
	TiXmlText *MidYContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.MidY));
	MidYElement->LinkEndChild(MidYContent);

	//p31zf 汽流方向，定义数值=1，汽流从左向右，否则标其他数字，非空
	TiXmlElement *p31zfElement = new TiXmlElement("p31zf");
	BladePairElement->LinkEndChild(p31zfElement);
	const char * thetmp = Data->MyFunction->Basic->Double2Char(tmp.p31zf);
	TiXmlText *p31zfContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p31zf));
	p31zfElement->LinkEndChild(p31zfContent);

	//折流处标记。1为是0为否
	TiXmlElement *BrokenElement = new TiXmlElement("Broken");
	BladePairElement->LinkEndChild(BrokenElement);

	//BrokenTag 折流处标记。1为是0为否
	TiXmlElement *BrokenTagElement = new TiXmlElement("BrokenTag");
	BrokenElement->LinkEndChild(BrokenTagElement);
	TiXmlText *BrokenTagContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.BrokenTag));
	BrokenTagElement->LinkEndChild(BrokenTagContent);

	//BrokenDis 折流处定义距离，静叶中心线距气道中心线
	TiXmlElement *BrokenDisElement = new TiXmlElement("BrokenDis");
	BrokenElement->LinkEndChild(BrokenDisElement);
	TiXmlText *BrokenDisContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.BrokenDis));
	BrokenDisElement->LinkEndChild(BrokenDisContent);

	//Margin 余量
	TiXmlElement *MarginElement = new TiXmlElement("Margin");
	BladePairElement->LinkEndChild(MarginElement);
	TiXmlText *MarginContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.Margin));
	MarginElement->LinkEndChild(MarginContent);
	
	//Jmin 进气侧汽封槽到上级叶片的距离 J最小距离
	TiXmlElement *JminElement = new TiXmlElement("Jmin");
	BladePairElement->LinkEndChild(JminElement);
	TiXmlText *JminContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.Jmin));
	JminElement->LinkEndChild(JminContent);

	//Hmin 出气侧汽封槽到下级叶片的距离 H最小距离
	TiXmlElement *HminElement = new TiXmlElement("Hmin");
	BladePairElement->LinkEndChild(HminElement);
	TiXmlText *HminContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.Hmin));
	HminElement->LinkEndChild(HminContent);

	//P<!--  正胀差  -->
	TiXmlElement *PElement = new TiXmlElement("P");
	BladePairElement->LinkEndChild(PElement);
	TiXmlText *PContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.P));
	PElement->LinkEndChild(PContent);

	//Q<!--  负胀差  -->
	TiXmlElement *QElement = new TiXmlElement("Q");
	BladePairElement->LinkEndChild(QElement);
	TiXmlText *QContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.Q));
	QElement->LinkEndChild(QContent);

	//W<!--  稳态胀差  先取正负涨差的均值  -->
	TiXmlElement *WElement = new TiXmlElement("W");
	BladePairElement->LinkEndChild(WElement);
	TiXmlText *WContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.W));
	WElement->LinkEndChild(WContent);

	/*N<!--  每个气道的布置方向与推力轴承的关系Nn值，暂时与P31相同
	当气道1在推力轴承右侧且为反向布置，或气道1在推力轴承左侧且正向布置，定义：N1=0
	当气道1在推力轴承右侧且为正向布置，或气道1在推力轴承左侧且反向布置，定义：N1=1
	-->*/
	TiXmlElement *NElement = new TiXmlElement("N");
	BladePairElement->LinkEndChild(NElement);
	TiXmlText *NContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.N));
	NElement->LinkEndChild(NContent);


	//Blade//静叶
	TiXmlElement *BladeElement = new TiXmlElement("Blade");
	BladePairElement->LinkEndChild(BladeElement);
	
	//BladeType 叶片类型 //静叶
	TiXmlElement *BladeTypeElement = new TiXmlElement("BladeType");
	BladeElement->LinkEndChild(BladeTypeElement);
	TiXmlText *BladeTypeContent = new TiXmlText("stator");
	BladeTypeElement->LinkEndChild(BladeTypeContent);

	//Profile
	TiXmlElement *ProfileElement = new TiXmlElement("Profile");
	BladeElement->LinkEndChild(ProfileElement);

	////p0Jz 原点到第n级静叶中心线的距离
	//TiXmlElement *p0JzElement = new TiXmlElement("p0Jz");
	//ProfileElement->LinkEndChild(p0JzElement);
	//TiXmlText *p0JzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p0Jz));
	//p0JzElement->LinkEndChild(p0JzContent);

	//p1BG 原点到第n轴向宽度根截面BG
	TiXmlElement *p1BGElement = new TiXmlElement("p1BG");
	ProfileElement->LinkEndChild(p1BGElement);
	TiXmlText *p1BGContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p1BG));
	p1BGElement->LinkEndChild(p1BGContent);

	//p2BD 顶截面BD
	TiXmlElement *p2BDElement = new TiXmlElement("p2BD");
	ProfileElement->LinkEndChild(p2BDElement);
	TiXmlText *p2BDContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p2BD));
	p2BDElement->LinkEndChild(p2BDContent);

	//p3bg 出汽边bg
	TiXmlElement *p3bgElement = new TiXmlElement("p3bg");
	ProfileElement->LinkEndChild(p3bgElement);
	TiXmlText *p3bgContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p3bg));
	p3bgElement->LinkEndChild(p3bgContent);

	//p4bd 出汽边bd
	TiXmlElement *p4bdElement = new TiXmlElement("p4bd");
	ProfileElement->LinkEndChild(p4bdElement);
	TiXmlText *p4bdContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p4bd));
	p4bdElement->LinkEndChild(p4bdContent);

	//p5Bw 围带宽度Bw
	TiXmlElement *p5BwElement = new TiXmlElement("p5Bw");
	ProfileElement->LinkEndChild(p5BwElement);
	TiXmlText *p5BwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p5Bw));
	p5BwElement->LinkEndChild(p5BwContent);

	//p6bw 出汽边到中心线距离bw
	TiXmlElement *p6bwElement = new TiXmlElement("p6bw");
	ProfileElement->LinkEndChild(p6bwElement);
	TiXmlText *p6bwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p6bw));
	p6bwElement->LinkEndChild(p6bwContent);

	//p7hw 围带高度hw
	TiXmlElement *p7hwElement = new TiXmlElement("p7hw");
	ProfileElement->LinkEndChild(p7hwElement);
	TiXmlText *p7hwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p7hw));
	p7hwElement->LinkEndChild(p7hwContent);

	//p8Bz 中间体宽度Bz
	TiXmlElement *p8BzElement = new TiXmlElement("p8Bz");
	ProfileElement->LinkEndChild(p8BzElement);
	TiXmlText *p8BzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz));
	p8BzElement->LinkEndChild(p8BzContent);

	//p9bz 出汽边到中心线距离bz
	TiXmlElement *p9bzElement = new TiXmlElement("p9bz");
	ProfileElement->LinkEndChild(p9bzElement);
	TiXmlText *p9bzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p9bz));
	p9bzElement->LinkEndChild(p9bzContent);

	//p10hz 中间体高度hz
	TiXmlElement *p10hzElement = new TiXmlElement("p10hz");
	ProfileElement->LinkEndChild(p10hzElement);
	TiXmlText *p10hzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p10hz));
	p10hzElement->LinkEndChild(p10hzContent);

	//p26Dpp 出汽边平均直径Dpp
	TiXmlElement *p26DppElement = new TiXmlElement("p26Dpp");
	ProfileElement->LinkEndChild(p26DppElement);
	TiXmlText *p26DppContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p26Dpp));
	p26DppElement->LinkEndChild(p26DppContent);

	//p27Lp 出汽边高度Lp
	TiXmlElement *p27LpElement = new TiXmlElement("p27Lp");
	ProfileElement->LinkEndChild(p27LpElement);
	TiXmlText *p27LpContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p27Lp));
	p27LpElement->LinkEndChild(p27LpContent);

	//p11ad 顶部扩张角αd
	TiXmlElement *p11adElement = new TiXmlElement("p11ad");
	ProfileElement->LinkEndChild(p11adElement);
	TiXmlText *p11adContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p11ad));
	p11adElement->LinkEndChild(p11adContent);

	//p12ag 根部扩张角αg
	TiXmlElement *p12agElement = new TiXmlElement("p12ag");
	ProfileElement->LinkEndChild(p12agElement);
	TiXmlText *p12agContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p12ag));
	p12agElement->LinkEndChild(p12agContent);
	
	//<Hub><!--  静叶叶根  -->
	ExportXMLData( BladeElement, 0 ,tmp);
	//<Shroud><!--  静叶围带  -->
	ExportShroudXMLData( BladeElement, 0 ,tmp);


	//Blade//动叶******************************************************************************
	BladeElement = new TiXmlElement("Blade");
	BladePairElement->LinkEndChild(BladeElement);

	//BladeType 叶片类型 //动叶
	BladeTypeElement = new TiXmlElement("BladeType");
	BladeElement->LinkEndChild(BladeTypeElement);
	BladeTypeContent = new TiXmlText("rotor");
	BladeTypeElement->LinkEndChild(BladeTypeContent);

	//Profile
	ProfileElement = new TiXmlElement("Profile");
	BladeElement->LinkEndChild(ProfileElement);

	////p30Dz 原点到第n级静叶中心线的距离
	//TiXmlElement *p30DzElement = new TiXmlElement("p30Dz");
	//ProfileElement->LinkEndChild(p30DzElement);
	//TiXmlText *p30DzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p30Dz));
	//p30DzElement->LinkEndChild(p30DzContent);

	//p13BG 原点到第n轴向宽度根截面BG
	TiXmlElement *p13BGElement = new TiXmlElement("p13BG");
	ProfileElement->LinkEndChild(p13BGElement);
	TiXmlText *p13BGContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p13BG));
	p13BGElement->LinkEndChild(p13BGContent);

	//p14BD 顶截面BD
	TiXmlElement *p14BDElement = new TiXmlElement("p14BD");
	ProfileElement->LinkEndChild(p14BDElement);
	TiXmlText *p14BDContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p14BD));
	p14BDElement->LinkEndChild(p14BDContent);

	//p15bg 出汽边bg
	TiXmlElement *p15bgElement = new TiXmlElement("p15bg");
	ProfileElement->LinkEndChild(p15bgElement);
	TiXmlText *p15bgContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p15bg));
	p15bgElement->LinkEndChild(p15bgContent);

	//p16bd 出汽边bd
	TiXmlElement *p16bdElement = new TiXmlElement("p16bd");
	ProfileElement->LinkEndChild(p16bdElement);
	TiXmlText *p16bdContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p16bd));
	p16bdElement->LinkEndChild(p16bdContent);

	//p17Bw 围带宽度Bw
	TiXmlElement *p17BwElement = new TiXmlElement("p17Bw");
	ProfileElement->LinkEndChild(p17BwElement);
	TiXmlText *p17BwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p17Bw));
	p17BwElement->LinkEndChild(p17BwContent);

	//p18bw 出汽边到中心线距离bw
	TiXmlElement *p18bwElement = new TiXmlElement("p18bw");
	ProfileElement->LinkEndChild(p18bwElement);
	TiXmlText *p18bwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p18bw));
	p18bwElement->LinkEndChild(p18bwContent);

	//p19hw 围带高度hw
	TiXmlElement *p19hwElement = new TiXmlElement("p19hw");
	ProfileElement->LinkEndChild(p19hwElement);
	TiXmlText *p19hwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p19hw));
	p19hwElement->LinkEndChild(p19hwContent);

	//p21Bz 中间体宽度Bz
	TiXmlElement *p21BzElement = new TiXmlElement("p21Bz");
	ProfileElement->LinkEndChild(p21BzElement);
	TiXmlText *p21BzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz));
	p21BzElement->LinkEndChild(p21BzContent);

	//p22bz 出汽边到中心线距离bz
	TiXmlElement *p22bzElement = new TiXmlElement("p22bz");
	ProfileElement->LinkEndChild(p22bzElement);
	TiXmlText *p22bzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p22bz));
	p22bzElement->LinkEndChild(p22bzContent);

	//p23hz 中间体高度hz
	TiXmlElement *p23hzElement = new TiXmlElement("p23hz");
	ProfileElement->LinkEndChild(p23hzElement);
	TiXmlText *p23hzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p23hz));
	p23hzElement->LinkEndChild(p23hzContent);

	//p28Dpd 出汽边平均直径Dpd
	TiXmlElement *p28DpdElement = new TiXmlElement("p28Dpd");
	ProfileElement->LinkEndChild(p28DpdElement);
	TiXmlText *p28DpdContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p28Dpd));
	p28DpdElement->LinkEndChild(p28DpdContent);

	//p29Ld 出汽边高度Ld
	TiXmlElement *p29LdElement = new TiXmlElement("p29Ld");
	ProfileElement->LinkEndChild(p29LdElement);
	TiXmlText *p29LdContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p29Ld));
	p29LdElement->LinkEndChild(p29LdContent);

	//p24ad 顶部扩张角αd
	TiXmlElement *p24adElement = new TiXmlElement("p24ad");
	ProfileElement->LinkEndChild(p24adElement);
	TiXmlText *p24adContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p24ad));
	p24adElement->LinkEndChild(p24adContent);

	//p25ag 根部扩张角αg
	TiXmlElement *p25agElement = new TiXmlElement("p25ag");
	ProfileElement->LinkEndChild(p25agElement);
	TiXmlText *p25agContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p25ag));
	p25agElement->LinkEndChild(p25agContent);

	//<Hub><!--  动叶叶根  -->
	ExportXMLData( BladeElement, 1 ,tmp);
	//<Shroud><!--  动叶围带  -->
	ExportShroudXMLData( BladeElement, 1 ,tmp);
	return true;

}


//叶根节点数据导出
bool ClassTongliu::ExportXMLData( TiXmlElement* pElem, int HubType ,bladeData tmp)
{
	CString str;
	TiXmlElement *HubElement = NULL;

#pragma region
	//静叶
	if (HubType == 0)
	{
		//<!--  静叶叶根  -->
		HubElement = new TiXmlElement("StatorHub");
		pElem->LinkEndChild(HubElement);

		//HubMType<!--  叶根类型  -->
		TiXmlElement *HubMTypeElement = new TiXmlElement("HubMType");
		HubElement->LinkEndChild(HubMTypeElement);
		TiXmlText *HubMTypContent = new TiXmlText("反动式预扭T叶根");
		HubMTypeElement->LinkEndChild(HubMTypContent);

		//HubType<!--  叶根型线号  -->
		TiXmlElement *HubTypeElement = new TiXmlElement("HubType");
		HubElement->LinkEndChild(HubTypeElement);
		TiXmlText *HubTypeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.SHubType));
		HubTypeElement->LinkEndChild(HubTypeContent);

		//HubPart<!--  叶根部分  -->//////////////////////////////////
		TiXmlElement *HubPartElement = new TiXmlElement("HubPart");
		HubElement->LinkEndChild(HubPartElement);

		//bn<!--  叶根颈部宽度bn  -->
		TiXmlElement *bnElement = new TiXmlElement("bn");
		HubPartElement->LinkEndChild(bnElement);
		TiXmlText *bnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*32.0/56.0));
		bnElement->LinkEndChild(bnContent);

		//hn<!--  叶根颈部高度hn  -->
		TiXmlElement *hnElement = new TiXmlElement("hn");
		HubPartElement->LinkEndChild(hnElement);
		TiXmlText *hnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*12.5/56.0));
		hnElement->LinkEndChild(hnContent);

		//hx><!--  叶根下部高度hx  -->
		TiXmlElement *hxElement = new TiXmlElement("hx");
		HubPartElement->LinkEndChild(hxElement);
		TiXmlText *hxContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*12.5/56.0));
		hxElement->LinkEndChild(hxContent);

		//bz<!--  中间体轴向宽度的一半bz  -->
		TiXmlElement *bzElement = new TiXmlElement("bz");
		HubPartElement->LinkEndChild(bzElement);
		TiXmlText *bzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*28/56.0));
		bzElement->LinkEndChild(bzContent);

		//daojiao_c><!--  叶根端部出汽边倒角[mm]daojiaoc=  -->
		TiXmlElement *daojiao_cElement = new TiXmlElement("daojiao_c");
		HubPartElement->LinkEndChild(daojiao_cElement);
		TiXmlText *daojiao_cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*4.0/56.0));
		daojiao_cElement->LinkEndChild(daojiao_cContent);

		//daojiao_cb><!--  叶根下部出汽边上倒角[mm]daojiaocb=（静叶为叶根上部出汽边下倒角）  -->
		TiXmlElement *daojiao_cbElement = new TiXmlElement("daojiao_cb");
		HubPartElement->LinkEndChild(daojiao_cbElement);
		TiXmlText *daojiao_cbContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*1.0/56.0));
		daojiao_cbElement->LinkEndChild(daojiao_cbContent);

		//daojiao_r><!--  叶根颈部四周倒圆角[mm]daojiaor=  -->
		TiXmlElement *daojiao_rElement = new TiXmlElement("daojiao_r");
		HubPartElement->LinkEndChild(daojiao_rElement);
		TiXmlText *daojiao_rContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*3.0/56.0));
		daojiao_rElement->LinkEndChild(daojiao_rContent);

		//rcj><!--  叶根进出汽侧中间体挖槽半径rcj=  -->
		TiXmlElement *rcjElement = new TiXmlElement("rcj");
		HubPartElement->LinkEndChild(rcjElement);
		TiXmlText *rcjContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*2.0/56.0));
		rcjElement->LinkEndChild(rcjContent);

		//HubHorizontalAngle><!--  叶根工作面与水平夹角  -->
		TiXmlElement *HubHorizontalAngleElement = new TiXmlElement("HubHorizontalAngle");
		HubPartElement->LinkEndChild(HubHorizontalAngleElement);
		TiXmlText *HubHorizontalAngleContent = new TiXmlText("0");
		HubHorizontalAngleElement->LinkEndChild(HubHorizontalAngleContent);


		//HubGroove><!--  叶根槽部分  -->//////////////////////////////////
		TiXmlElement *HubGrooveElement = new TiXmlElement("HubGroove");
		HubElement->LinkEndChild(HubGrooveElement);

		//hy><!--  叶根进汽边沉入转子深度hy  -->
		TiXmlElement *hyElement = new TiXmlElement("hy");
		HubGrooveElement->LinkEndChild(hyElement);
		TiXmlText *hyContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*5.0/56.0));
		hyElement->LinkEndChild(hyContent);

		//InterstitialsR><!--  填隙条半径  -->
		TiXmlElement *InterstitialsRElement = new TiXmlElement("InterstitialsR");
		HubGrooveElement->LinkEndChild(InterstitialsRElement);
		TiXmlText *InterstitialsRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*5.0/56.0));
		InterstitialsRElement->LinkEndChild(InterstitialsRContent);

		//GrooveSurfaceH><!--  中间体下表面到叶根槽下表面高度  -->
		TiXmlElement *GrooveSurfaceHElement = new TiXmlElement("GrooveSurfaceH");
		HubGrooveElement->LinkEndChild(GrooveSurfaceHElement);
		TiXmlText *GrooveSurfaceHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*26.0/56.0));
		GrooveSurfaceHElement->LinkEndChild(GrooveSurfaceHContent);

		//GrooveNeckH><!--  叶根槽颈部高度  -->
		TiXmlElement *GrooveNeckHElement = new TiXmlElement("GrooveNeckH");
		HubGrooveElement->LinkEndChild(GrooveNeckHElement);
		TiXmlText *GrooveNeckHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*12.5/56.0));
		GrooveNeckHElement->LinkEndChild(GrooveNeckHContent);

		//GrooveBottomH><!--  叶根槽下部高度（包含填隙条槽的高度）  -->
		TiXmlElement *GrooveBottomHElement = new TiXmlElement("GrooveBottomH");
		HubGrooveElement->LinkEndChild(GrooveBottomHElement);
		TiXmlText *GrooveBottomHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*18.0/56.0));
		GrooveBottomHElement->LinkEndChild(GrooveBottomHContent);

		//MidBodyH><!--  叶根槽中间体宽度  -->
		TiXmlElement *MidBodyHElement = new TiXmlElement("MidBodyH");
		HubGrooveElement->LinkEndChild(MidBodyHElement);
		TiXmlText *MidBodyHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*56.0/56.0));
		MidBodyHElement->LinkEndChild(MidBodyHContent);

		//GrooveNeckW><!--  叶根槽颈部宽度  -->
		TiXmlElement *GrooveNeckWElement = new TiXmlElement("GrooveNeckW");
		HubGrooveElement->LinkEndChild(GrooveNeckWElement);
		TiXmlText *GrooveNeckWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*36.4/56.0));
		GrooveNeckWElement->LinkEndChild(GrooveNeckWContent);

		//GrooveNeckChamfer><!--  叶根槽颈部倒角  -->
		TiXmlElement *GrooveNeckChamferElement = new TiXmlElement("GrooveNeckChamfer");
		HubGrooveElement->LinkEndChild(GrooveNeckChamferElement);
		TiXmlText *GrooveNeckChamferContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*1.0/56.0));
		GrooveNeckChamferElement->LinkEndChild(GrooveNeckChamferContent);

		//GrooveBottomW><!--  叶根槽下部宽度（若两侧不为竖直则填最大处宽度）  -->
		TiXmlElement *GrooveBottomWElement = new TiXmlElement("GrooveBottomW");
		HubGrooveElement->LinkEndChild(GrooveBottomWElement);
		TiXmlText *GrooveBottomWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*57.4/56.0));
		GrooveBottomWElement->LinkEndChild(GrooveBottomWContent);

		//GrooveVerticalAngle><!--  叶根槽下部与竖直夹角  -->
		TiXmlElement *GrooveVerticalAngleElement = new TiXmlElement("GrooveVerticalAngle");
		HubGrooveElement->LinkEndChild(GrooveVerticalAngleElement);
		TiXmlText *GrooveVerticalAngleContent = new TiXmlText("0");
		GrooveVerticalAngleElement->LinkEndChild(GrooveVerticalAngleContent);

		//UCornerR><!--  叶根槽下部上倒圆（静叶为轮槽上部下倒圆）  -->
		TiXmlElement *UCornerRElement = new TiXmlElement("UCornerR");
		HubGrooveElement->LinkEndChild(UCornerRElement);
		TiXmlText *UCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*1.0/56.0));
		UCornerRElement->LinkEndChild(UCornerRContent);

		//DCornerR><!--  叶根槽下部下倒圆（静叶为轮槽上部上倒圆）  -->
		TiXmlElement *DCornerRElement = new TiXmlElement("DCornerR");
		HubGrooveElement->LinkEndChild(DCornerRElement);
		TiXmlText *DCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*4.0/56.0));
		DCornerRElement->LinkEndChild(DCornerRContent);

		
	}
#pragma endregion

	//动叶
	if (HubType == 1)
	{
		HubElement = new TiXmlElement("RotorHub");
		pElem->LinkEndChild(HubElement);
		
		//判断型线号//反动式预扭T叶根
		if(tmp.HubType == 1)
#pragma region
		{
			//HubMType<!--  叶根类型  -->
			TiXmlElement *HubMTypeElement = new TiXmlElement("HubMType");
			HubElement->LinkEndChild(HubMTypeElement);
			TiXmlText *HubMTypContent = new TiXmlText("反动式预扭T叶根");
			HubMTypeElement->LinkEndChild(HubMTypContent);

			//HubType<!--  叶根型线号  -->
			TiXmlElement *HubTypeElement = new TiXmlElement("HubType");
			HubElement->LinkEndChild(HubTypeElement);
			TiXmlText *HubTypeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.HubType));
			HubTypeElement->LinkEndChild(HubTypeContent);

			//HubPart<!--  叶根部分  -->//////////////////////////////////
			TiXmlElement *HubPartElement = new TiXmlElement("HubPart");
			HubElement->LinkEndChild(HubPartElement);

			//bn<!--  叶根颈部宽度bn  -->
			TiXmlElement *bnElement = new TiXmlElement("bn");
			HubPartElement->LinkEndChild(bnElement);
			TiXmlText *bnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*23.5/45.0));
			bnElement->LinkEndChild(bnContent);

			//hn<!--  叶根颈部高度hn  -->
			TiXmlElement *hnElement = new TiXmlElement("hn");
			HubPartElement->LinkEndChild(hnElement);
			TiXmlText *hnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*25.0/45.0));
			hnElement->LinkEndChild(hnContent);

			//hx><!--  叶根下部高度hx  -->
			TiXmlElement *hxElement = new TiXmlElement("hx");
			HubPartElement->LinkEndChild(hxElement);
			TiXmlText *hxContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.0/45.0));
			hxElement->LinkEndChild(hxContent);

			//bz<!--  中间体轴向宽度的一半bz  -->
			TiXmlElement *bzElement = new TiXmlElement("bz");
			HubPartElement->LinkEndChild(bzElement);
			TiXmlText *bzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.5/45.0));
			bzElement->LinkEndChild(bzContent);

			//daojiao_c><!--  叶根端部出汽边倒角[mm]daojiaoc=  -->
			TiXmlElement *daojiao_cElement = new TiXmlElement("daojiao_c");
			HubPartElement->LinkEndChild(daojiao_cElement);
			TiXmlText *daojiao_cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*6.0/45.0));
			daojiao_cElement->LinkEndChild(daojiao_cContent);

			//daojiao_cb><!--  叶根下部出汽边上倒角[mm]daojiaocb=（静叶为叶根上部出汽边下倒角）  -->
			TiXmlElement *daojiao_cbElement = new TiXmlElement("daojiao_cb");
			HubPartElement->LinkEndChild(daojiao_cbElement);
			TiXmlText *daojiao_cbContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.1/45.0));
			daojiao_cbElement->LinkEndChild(daojiao_cbContent);

			//daojiao_r><!--  叶根颈部四周倒圆角[mm]daojiaor=  -->
			TiXmlElement *daojiao_rElement = new TiXmlElement("daojiao_r");
			HubPartElement->LinkEndChild(daojiao_rElement);
			TiXmlText *daojiao_rContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.5/45.0));
			daojiao_rElement->LinkEndChild(daojiao_rContent);

			//rcj><!--  叶根进出汽侧中间体挖槽半径rcj=  -->
			TiXmlElement *rcjElement = new TiXmlElement("rcj");
			HubPartElement->LinkEndChild(rcjElement);
			TiXmlText *rcjContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.0/45.0));
			rcjElement->LinkEndChild(rcjContent);

			//HubHorizontalAngle><!--  叶根工作面与水平夹角  -->
			TiXmlElement *HubHorizontalAngleElement = new TiXmlElement("HubHorizontalAngle");
			HubPartElement->LinkEndChild(HubHorizontalAngleElement);
			TiXmlText *HubHorizontalAngleContent = new TiXmlText("0");
			HubHorizontalAngleElement->LinkEndChild(HubHorizontalAngleContent);


			//HubGroove><!--  叶根槽部分  -->//////////////////////////////////
			TiXmlElement *HubGrooveElement = new TiXmlElement("HubGroove");
			HubElement->LinkEndChild(HubGrooveElement);

			//hy><!--  叶根进汽边沉入转子深度hy  -->
			TiXmlElement *hyElement = new TiXmlElement("hy");
			HubGrooveElement->LinkEndChild(hyElement);
			TiXmlText *hyContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*8.0/45.0));
			hyElement->LinkEndChild(hyContent);

			//InterstitialsR><!--  填隙条半径  -->
			TiXmlElement *InterstitialsRElement = new TiXmlElement("InterstitialsR");
			HubGrooveElement->LinkEndChild(InterstitialsRElement);
			TiXmlText *InterstitialsRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*5.0/45.0));
			InterstitialsRElement->LinkEndChild(InterstitialsRContent);

			//GrooveSurfaceH><!--  中间体下表面到叶根槽下表面高度  -->
			TiXmlElement *GrooveSurfaceHElement = new TiXmlElement("GrooveSurfaceH");
			HubGrooveElement->LinkEndChild(GrooveSurfaceHElement);
			TiXmlText *GrooveSurfaceHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*48.0/45.0));
			GrooveSurfaceHElement->LinkEndChild(GrooveSurfaceHContent);

			//GrooveNeckH><!--  叶根槽颈部高度  -->
			TiXmlElement *GrooveNeckHElement = new TiXmlElement("GrooveNeckH");
			HubGrooveElement->LinkEndChild(GrooveNeckHElement);
			TiXmlText *GrooveNeckHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*25.0/45.0));
			GrooveNeckHElement->LinkEndChild(GrooveNeckHContent);

			//GrooveBottomH><!--  叶根槽下部高度（包含填隙条槽的高度）  -->
			TiXmlElement *GrooveBottomHElement = new TiXmlElement("GrooveBottomH");
			HubGrooveElement->LinkEndChild(GrooveBottomHElement);
			TiXmlText *GrooveBottomHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*27.5/45.0));
			GrooveBottomHElement->LinkEndChild(GrooveBottomHContent);

			//MidBodyH><!--  叶根槽中间体宽度  -->
			TiXmlElement *MidBodyHElement = new TiXmlElement("MidBodyH");
			HubGrooveElement->LinkEndChild(MidBodyHElement);
			TiXmlText *MidBodyHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*45.0/45.0));
			MidBodyHElement->LinkEndChild(MidBodyHContent);

			//GrooveNeckW><!--  叶根槽颈部宽度  -->
			TiXmlElement *GrooveNeckWElement = new TiXmlElement("GrooveNeckW");
			HubGrooveElement->LinkEndChild(GrooveNeckWElement);
			TiXmlText *GrooveNeckWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*29.0/45.0));
			GrooveNeckWElement->LinkEndChild(GrooveNeckWContent);

			//GrooveNeckChamfer><!--  叶根槽颈部倒角  -->
			TiXmlElement *GrooveNeckChamferElement = new TiXmlElement("GrooveNeckChamfer");
			HubGrooveElement->LinkEndChild(GrooveNeckChamferElement);
			TiXmlText *GrooveNeckChamferContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/45.0));
			GrooveNeckChamferElement->LinkEndChild(GrooveNeckChamferContent);

			//GrooveBottomW><!--  叶根槽下部宽度（若两侧不为竖直则填最大处宽度）  -->
			TiXmlElement *GrooveBottomWElement = new TiXmlElement("GrooveBottomW");
			HubGrooveElement->LinkEndChild(GrooveBottomWElement);
			TiXmlText *GrooveBottomWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*47.0/45.0));
			GrooveBottomWElement->LinkEndChild(GrooveBottomWContent);

			//GrooveVerticalAngle><!--  叶根槽下部与竖直夹角  -->
			TiXmlElement *GrooveVerticalAngleElement = new TiXmlElement("GrooveVerticalAngle");
			HubGrooveElement->LinkEndChild(GrooveVerticalAngleElement);
			TiXmlText *GrooveVerticalAngleContent = new TiXmlText("0");
			GrooveVerticalAngleElement->LinkEndChild(GrooveVerticalAngleContent);

			//UCornerR><!--  叶根槽下部上倒圆（静叶为轮槽上部下倒圆）  -->
			TiXmlElement *UCornerRElement = new TiXmlElement("UCornerR");
			HubGrooveElement->LinkEndChild(UCornerRElement);
			TiXmlText *UCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.8/45.0));
			UCornerRElement->LinkEndChild(UCornerRContent);

			//DCornerR><!--  叶根槽下部下倒圆（静叶为轮槽上部上倒圆）  -->
			TiXmlElement *DCornerRElement = new TiXmlElement("DCornerR");
			HubGrooveElement->LinkEndChild(DCornerRElement);
			TiXmlText *DCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*9.0/45.0));
			DCornerRElement->LinkEndChild(DCornerRContent);

		}
#pragma endregion
		//冲动式非预扭T叶根
		else if(tmp.HubType == 2)
#pragma region
		{
			//HubMType<!--  叶根类型  -->
			TiXmlElement *HubMTypeElement = new TiXmlElement("HubMType");
			HubElement->LinkEndChild(HubMTypeElement);
			TiXmlText *HubMTypContent = new TiXmlText("冲动式非预扭T叶根");
			HubMTypeElement->LinkEndChild(HubMTypContent);

			//HubType<!--  叶根型线号  -->
			TiXmlElement *HubTypeElement = new TiXmlElement("HubType");
			HubElement->LinkEndChild(HubTypeElement);
			TiXmlText *HubTypeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.HubType));
			HubTypeElement->LinkEndChild(HubTypeContent);

			//HubPart<!--  叶根部分  -->//////////////////////////////////
			TiXmlElement *HubPartElement = new TiXmlElement("HubPart");
			HubElement->LinkEndChild(HubPartElement);

			//MidBodySurW<!--  中间体下表面去掉小脚宽度  -->
			TiXmlElement *MidBodySurWElement = new TiXmlElement("MidBodySurW");
			HubPartElement->LinkEndChild(MidBodySurWElement);
			TiXmlText *MidBodySurWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*24.0/50.0));
			MidBodySurWElement->LinkEndChild(MidBodySurWContent);

			//bn<!--  叶根颈部宽度bn  -->
			TiXmlElement *bnElement = new TiXmlElement("bn");
			HubPartElement->LinkEndChild(bnElement);
			TiXmlText *bnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*11.0/50.0));
			bnElement->LinkEndChild(bnContent);

			//hn<!--  叶根颈部高度hn  -->
			TiXmlElement *hnElement = new TiXmlElement("hn");
			HubPartElement->LinkEndChild(hnElement);
			TiXmlText *hnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.0/50.0));
			hnElement->LinkEndChild(hnContent);

			//HubDW><!--  叶根下部宽度  -->
			TiXmlElement *HubDWElement = new TiXmlElement("HubDW"); 
			HubPartElement->LinkEndChild(HubDWElement);
			TiXmlText *HubDWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*21.0/50.0));
			HubDWElement->LinkEndChild(HubDWContent);

			//hx><!--  叶根下部高度hx  -->
			TiXmlElement *hxElement = new TiXmlElement("hx");
			HubPartElement->LinkEndChild(hxElement);
			TiXmlText *hxContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*14.0/50.0));
			hxElement->LinkEndChild(hxContent);

			//TSFH><!--  T叶根小脚高度  如无小脚，该项为0-->
			TiXmlElement *TSFHElement = new TiXmlElement("TSFH");
			HubPartElement->LinkEndChild(TSFHElement);
			TiXmlText *TSFHContent = new TiXmlText("5");
			TSFHElement->LinkEndChild(TSFHContent);

			//TUestW><!--  T叶根颈部上端最大宽度  -->
			TiXmlElement *TUestWElement = new TiXmlElement("TUestW");
			HubPartElement->LinkEndChild(TUestWElement);
			TiXmlText *TUestWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*11.0/50.0));
			TUestWElement->LinkEndChild(TUestWContent);

			//TZAngle><!--  T叶根颈部倾斜边与Z轴夹角  -->
			TiXmlElement *TZAngleElement = new TiXmlElement("TZAngle");
			HubPartElement->LinkEndChild(TZAngleElement);
			TiXmlText *TZAngleContent = new TiXmlText("0");
			TZAngleElement->LinkEndChild(TZAngleContent);

			//daojiao_c><!--  叶根端部出汽边倒角[mm]daojiaoc=  -->
			TiXmlElement *daojiao_cElement = new TiXmlElement("daojiao_c");
			HubPartElement->LinkEndChild(daojiao_cElement);
			TiXmlText *daojiao_cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*3.0/50.0));
			daojiao_cElement->LinkEndChild(daojiao_cContent);

			//daojiao_cb><!--  叶根下部出汽边上倒角[mm]daojiaocb=（静叶为叶根上部出汽边下倒角）  -->
			TiXmlElement *daojiao_cbElement = new TiXmlElement("daojiao_cb");
			HubPartElement->LinkEndChild(daojiao_cbElement);
			TiXmlText *daojiao_cbContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/50.0));
			daojiao_cbElement->LinkEndChild(daojiao_cbContent);

			//NeckDCornerR><!--  叶根颈部下倒圆  -->
			TiXmlElement *NeckDCornerRElement = new TiXmlElement("NeckDCornerR");
			HubPartElement->LinkEndChild(NeckDCornerRElement);
			TiXmlText *NeckDCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			NeckDCornerRElement->LinkEndChild(NeckDCornerRContent);

			//NeckUCornerR><!--  叶根颈部上倒圆  -->
			TiXmlElement *NeckUCornerRElement = new TiXmlElement("NeckUCornerR");
			HubPartElement->LinkEndChild(NeckUCornerRElement);
			TiXmlText *NeckUCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.0/50.0));
			NeckUCornerRElement->LinkEndChild(NeckUCornerRContent);

			//NeckMCornerR><!--  叶根颈部中倒圆  -->
			TiXmlElement *NeckMCornerRElement = new TiXmlElement("NeckMCornerR");
			HubPartElement->LinkEndChild(NeckMCornerRElement);
			TiXmlText *NeckMCornerRContent = new TiXmlText("0.5");
			NeckMCornerRElement->LinkEndChild(NeckMCornerRContent);

			//TAngle><!--  T叶根工作面与水平夹角  -->
			TiXmlElement *TAngleElement = new TiXmlElement("TAngle");
			HubPartElement->LinkEndChild(TAngleElement);
			TiXmlText *TAngleContent = new TiXmlText("0");
			TAngleElement->LinkEndChild(TAngleContent);

			//TSFMidCornerR><!--  T叶根小脚与中间体相接处倒圆   -->
			TiXmlElement *TSFMidCornerRElement = new TiXmlElement("TSFMidCornerR");
			HubPartElement->LinkEndChild(TSFMidCornerRElement);
			TiXmlText *TSFMidCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			TSFMidCornerRElement->LinkEndChild(TSFMidCornerRContent);

			//SFInAngle><!--  T叶根小脚内侧倒角  -->
			TiXmlElement *SFInAngleElement = new TiXmlElement("SFInAngle");
			HubPartElement->LinkEndChild(SFInAngleElement);
			TiXmlText *SFInAngleContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/50.0));
			SFInAngleElement->LinkEndChild(SFInAngleContent);

			//轮槽部分
			//HubGroove><!--  轮槽部分  -->//////////////////////////////////
			TiXmlElement *HubGrooveElement = new TiXmlElement("HubGroove");
			HubElement->LinkEndChild(HubGrooveElement);

			//NeckH><!--  轮槽颈部高度  -->
			TiXmlElement *NeckHElement = new TiXmlElement("NeckH");
			HubGrooveElement->LinkEndChild(NeckHElement);
			TiXmlText *NeckHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.0/50.0));
			NeckHElement->LinkEndChild(NeckHContent);

			//SFH><!--  轮槽小脚高度  -->
			TiXmlElement *SFHElement = new TiXmlElement("SFH");
			HubGrooveElement->LinkEndChild(SFHElement);
			TiXmlText *SFHContent = new TiXmlText("5");
			SFHElement->LinkEndChild(SFHContent);

			//NeckDH><!--  轮槽下部高度  -->
			TiXmlElement *NeckDHElement = new TiXmlElement("NeckDH");
			HubGrooveElement->LinkEndChild(NeckDHElement);
			TiXmlText *NeckDHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*15.5/50.0));
			NeckDHElement->LinkEndChild(NeckDHContent);

			//NeckW><!--  轮槽颈部宽度  -->
			TiXmlElement *NeckWElement = new TiXmlElement("NeckW");
			HubGrooveElement->LinkEndChild(NeckWElement);
			TiXmlText *NeckWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*11.0/50.0));
			NeckWElement->LinkEndChild(NeckWContent);

			//NeckDW><!--  轮槽下部宽度  -->
			TiXmlElement *NeckDWElement = new TiXmlElement("NeckDW");
			HubGrooveElement->LinkEndChild(NeckDWElement);
			TiXmlText *NeckDWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*21.4/50.0));
			NeckDWElement->LinkEndChild(NeckDWContent);

			//NeckUestW><!--  轮槽颈部上端最大宽度  -->
			TiXmlElement *NeckUestWElement = new TiXmlElement("NeckUestW");
			HubGrooveElement->LinkEndChild(NeckUestWElement);
			TiXmlText *NeckUestWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*11.0/50.0));
			NeckUestWElement->LinkEndChild(NeckUestWContent);

			//NeckZAngle><!--  轮槽颈部倾斜边与Z轴夹角  -->
			TiXmlElement *NeckZAngleElement = new TiXmlElement("NeckZAngle");
			HubGrooveElement->LinkEndChild(NeckZAngleElement);
			TiXmlText *NeckZAngleContent = new TiXmlText("0");
			NeckZAngleElement->LinkEndChild(NeckZAngleContent);

			//NeckUAngle><!--  轮槽颈部上倒角（颈部有斜边时为倒圆）  -->
			TiXmlElement *NeckUAngleElement = new TiXmlElement("NeckUAngle");
			HubGrooveElement->LinkEndChild(NeckUAngleElement);
			TiXmlText *NeckUAngleContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.5/50.0));
			NeckUAngleElement->LinkEndChild(NeckUAngleContent);

			//NeckDAngle><!--  轮槽颈部下倒角  -->
			TiXmlElement *NeckDAngleElement = new TiXmlElement("NeckDAngle");
			HubGrooveElement->LinkEndChild(NeckDAngleElement);
			TiXmlText *NeckDAngleContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/50.0));
			NeckDAngleElement->LinkEndChild(NeckDAngleContent);

			//RaceNeckMCornerR><!--  轮槽颈部中倒圆  -->
			TiXmlElement *RaceNeckMCornerRElement = new TiXmlElement("RaceNeckMCornerR");
			HubGrooveElement->LinkEndChild(RaceNeckMCornerRElement);
			TiXmlText *RaceNeckMCornerRContent = new TiXmlText("0.5");
			RaceNeckMCornerRElement->LinkEndChild(RaceNeckMCornerRContent);

			//RaceUCornerR><!--  轮槽下部上倒圆  -->
			TiXmlElement *RaceUCornerRElement = new TiXmlElement("RaceUCornerR");
			HubGrooveElement->LinkEndChild(RaceUCornerRElement);
			TiXmlText *RaceUCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			RaceUCornerRElement->LinkEndChild(RaceUCornerRContent);

			//RaceDCornerR><!--  轮槽下部下倒圆  -->
			TiXmlElement *RaceDCornerRElement = new TiXmlElement("RaceDCornerR");
			HubGrooveElement->LinkEndChild(RaceDCornerRElement);
			TiXmlText *RaceDCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*3.0/50.0));
			RaceDCornerRElement->LinkEndChild(RaceDCornerRContent);

			//RaceSFAngle><!--  轮槽小脚外侧倒角  -->
			TiXmlElement *RaceSFAngleElement = new TiXmlElement("RaceSFAngle");
			HubGrooveElement->LinkEndChild(RaceSFAngleElement);
			TiXmlText *RaceSFAngleContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/50.0));
			RaceSFAngleElement->LinkEndChild(RaceSFAngleContent);

			//RaceSFCornerR><!--  轮槽小脚外侧倒圆  -->
			TiXmlElement *RaceSFCornerRElement = new TiXmlElement("RaceSFCornerR");
			HubGrooveElement->LinkEndChild(RaceSFCornerRElement);
			TiXmlText *RaceSFCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			RaceSFCornerRElement->LinkEndChild(RaceSFCornerRContent);
		}
#pragma endregion
		//非预扭双倒T叶根
		else if(tmp.HubType == 3)
#pragma region
		{
			//HubMType<!--  叶根类型  -->
			TiXmlElement *HubMTypeElement = new TiXmlElement("HubMType");
			HubElement->LinkEndChild(HubMTypeElement);
			TiXmlText *HubMTypContent = new TiXmlText("非预扭双倒T叶根");
			HubMTypeElement->LinkEndChild(HubMTypContent);

			//HubType<!--  叶根型线号  -->
			TiXmlElement *HubTypeElement = new TiXmlElement("HubType");
			HubElement->LinkEndChild(HubTypeElement);
			TiXmlText *HubTypeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.HubType));
			HubTypeElement->LinkEndChild(HubTypeContent);

			//HubPart<!--  叶根部分  -->//////////////////////////////////
			TiXmlElement *HubPartElement = new TiXmlElement("HubPart");
			HubElement->LinkEndChild(HubPartElement);

			//b1<!--  b1  -->
			TiXmlElement *b1Element = new TiXmlElement("b1");
			HubPartElement->LinkEndChild(b1Element);
			TiXmlText *b1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*56.0/72.0));
			b1Element->LinkEndChild(b1Content);

			//b2<!--  b2  -->
			TiXmlElement *b2Element = new TiXmlElement("b2");
			HubPartElement->LinkEndChild(b2Element);
			TiXmlText *b2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*36.0/72.0));
			b2Element->LinkEndChild(b2Content);

			//b3<!--  b3  -->
			TiXmlElement *b3Element = new TiXmlElement("b3");
			HubPartElement->LinkEndChild(b3Element);
			TiXmlText *b3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*54.0/72.0));
			b3Element->LinkEndChild(b3Content);

			//b4<!--  b4  -->
			TiXmlElement *b4Element = new TiXmlElement("b4");
			HubPartElement->LinkEndChild(b4Element);
			TiXmlText *b4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.0/72.0));
			b4Element->LinkEndChild(b4Content);

			//b5<!--  b5  -->
			TiXmlElement *b5Element = new TiXmlElement("b5");
			HubPartElement->LinkEndChild(b5Element);
			TiXmlText *b5Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*40.0/72.0));
			b5Element->LinkEndChild(b5Content);

			//b6<!--  b6  -->
			TiXmlElement *b6Element = new TiXmlElement("b6");
			HubPartElement->LinkEndChild(b6Element);
			TiXmlText *b6Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.0/72.0));
			b6Element->LinkEndChild(b6Content);

			//h1<!--  h1  -->
			TiXmlElement *h1Element = new TiXmlElement("h1");
			HubPartElement->LinkEndChild(h1Element);
			TiXmlText *h1Content = new TiXmlText("5");
			h1Element->LinkEndChild(h1Content);

			//h2<!--  h2  -->
			TiXmlElement *h2Element = new TiXmlElement("h2");
			HubPartElement->LinkEndChild(h2Element);
			TiXmlText *h2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*28.0/72.0));
			h2Element->LinkEndChild(h2Content);

			//h3<!--  h3  -->
			TiXmlElement *h3Element = new TiXmlElement("h3");
			HubPartElement->LinkEndChild(h3Element);
			TiXmlText *h3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*8.0/72.0));
			h3Element->LinkEndChild(h3Content);

			//h4<!--  h4  -->
			TiXmlElement *h4Element = new TiXmlElement("h4");
			HubPartElement->LinkEndChild(h4Element);
			TiXmlText *h4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*16.0/72.0));
			h4Element->LinkEndChild(h4Content);

			//h5<!--  h5  -->
			TiXmlElement *h5Element = new TiXmlElement("h5");
			HubPartElement->LinkEndChild(h5Element);
			TiXmlText *h5Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*10.0/72.0));
			h5Element->LinkEndChild(h5Content);

			//h6<!--  h6  -->
			TiXmlElement *h6Element = new TiXmlElement("h6");
			HubPartElement->LinkEndChild(h6Element);
			TiXmlText *h6Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*9.0/72.0));
			h6Element->LinkEndChild(h6Content);

			//h7<!--  h7  -->
			TiXmlElement *h7Element = new TiXmlElement("h7");
			HubPartElement->LinkEndChild(h7Element);
			TiXmlText *h7Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*9.0/72.0));
			h7Element->LinkEndChild(h7Content);

			//r1<!--  r1（叶根）  -->
			TiXmlElement *r1Element = new TiXmlElement("r1");
			HubPartElement->LinkEndChild(r1Element);
			TiXmlText *r1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/72.0));
			r1Element->LinkEndChild(r1Content);

			//r3<!--  r3（叶根）  -->
			TiXmlElement *r3Element = new TiXmlElement("r3");
			HubPartElement->LinkEndChild(r3Element);
			TiXmlText *r3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/72.0));
			r3Element->LinkEndChild(r3Content);

			//d1<!--  d1（叶根）  -->
			TiXmlElement *d1Element = new TiXmlElement("d1");
			HubPartElement->LinkEndChild(d1Element);
			TiXmlText *d1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.4/72.0));
			d1Element->LinkEndChild(d1Content);

			//d3<!--  d3（叶根）  -->
			TiXmlElement *d3Element = new TiXmlElement("d3");
			HubPartElement->LinkEndChild(d3Element);
			TiXmlText *d3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.4/72.0));
			d3Element->LinkEndChild(d3Content);

			//h<!--  小脚高度h  -->
			TiXmlElement *hElement = new TiXmlElement("h");
			HubPartElement->LinkEndChild(hElement);
			TiXmlText *hContent = new TiXmlText("5");
			hElement->LinkEndChild(hContent);

			//HubSFTol<!--  叶根出汽侧小脚宽度公差  -->
			TiXmlElement *HubSFTolElement = new TiXmlElement("HubSFTol");
			HubPartElement->LinkEndChild(HubSFTolElement);
			TiXmlText *HubSFTolContent = new TiXmlText("0/-0.04");
			HubSFTolElement->LinkEndChild(HubSFTolContent);

			//NeckDisTol<!--  叶根颈部到出汽侧距离公差  -->
			TiXmlElement *NeckDisTolElement = new TiXmlElement("NeckDisTol");
			HubPartElement->LinkEndChild(NeckDisTolElement);
			TiXmlText *NeckDisTolContent = new TiXmlText("+0.09/+0.04");
			NeckDisTolElement->LinkEndChild(NeckDisTolContent);

			//轮槽部分
			//HubGroove><!--  轮槽部分  -->//////////////////////////////////
			TiXmlElement *HubGrooveElement = new TiXmlElement("HubGroove");
			HubElement->LinkEndChild(HubGrooveElement);

			//r2><!--  r2（轮槽）  -->
			TiXmlElement *r2Element = new TiXmlElement("r2");
			HubGrooveElement->LinkEndChild(r2Element);
			TiXmlText *r2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/72.0));
			r2Element->LinkEndChild(r2Content);

			//r4><!--  r4（轮槽）  -->
			TiXmlElement *r4Element = new TiXmlElement("r4");
			HubGrooveElement->LinkEndChild(r4Element);
			TiXmlText *r4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/72.0));
			r4Element->LinkEndChild(r4Content);

			//d2><!--  d2（轮槽）  -->
			TiXmlElement *d2Element = new TiXmlElement("d2");
			HubGrooveElement->LinkEndChild(d2Element);
			TiXmlText *d2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.4/72.0));
			d2Element->LinkEndChild(d2Content);

			//d4><!--  d4（轮槽）  -->
			TiXmlElement *d4Element = new TiXmlElement("d4");
			HubGrooveElement->LinkEndChild(d4Element);
			TiXmlText *d4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.4/72.0));
			d4Element->LinkEndChild(d4Content);

			//lb2><!--  lb2  -->
			TiXmlElement *lb2Element = new TiXmlElement("lb2");
			HubGrooveElement->LinkEndChild(lb2Element);
			TiXmlText *lb2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*36.0/72.0));
			lb2Element->LinkEndChild(lb2Content);

			//lb3><!--  lb3  -->
			TiXmlElement *lb3Element = new TiXmlElement("lb3");
			HubGrooveElement->LinkEndChild(lb3Element);
			TiXmlText *lb3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*54.6/72.0));
			lb3Element->LinkEndChild(lb3Content);

			//lb4><!--  lb4Z轴夹角  -->
			TiXmlElement *lb4Element = new TiXmlElement("lb4");
			HubGrooveElement->LinkEndChild(lb4Element);
			TiXmlText *lb4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.0/72.0));
			lb4Element->LinkEndChild(lb4Content);

			//lb5><!--  lb5  -->
			TiXmlElement *lb5Element = new TiXmlElement("lb5");
			HubGrooveElement->LinkEndChild(lb5Element);
			TiXmlText *lb5Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*40.6/72.0));
			lb5Element->LinkEndChild(lb5Content);
		}
#pragma endregion
		//预扭双倒T叶根
		else if(tmp.HubType == 4)
#pragma region
		{
			//HubMType<!--  叶根类型  -->
			TiXmlElement *HubMTypeElement = new TiXmlElement("HubMType");
			HubElement->LinkEndChild(HubMTypeElement);
			TiXmlText *HubMTypContent = new TiXmlText("预扭双倒T叶根");
			HubMTypeElement->LinkEndChild(HubMTypContent);

			//HubType<!--  叶根型线号  -->
			TiXmlElement *HubTypeElement = new TiXmlElement("HubType");
			HubElement->LinkEndChild(HubTypeElement);
			TiXmlText *HubTypeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.HubType));
			HubTypeElement->LinkEndChild(HubTypeContent);

			//HubPart<!--  叶根部分  -->//////////////////////////////////
			TiXmlElement *HubPartElement = new TiXmlElement("HubPart");
			HubElement->LinkEndChild(HubPartElement);

			//b1<!--  b1  -->
			TiXmlElement *b1Element = new TiXmlElement("b1");
			HubPartElement->LinkEndChild(b1Element);
			TiXmlText *b1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*50.0/50.0));
			b1Element->LinkEndChild(b1Content);

			//b2<!--  b2  -->
			TiXmlElement *b2Element = new TiXmlElement("b2");
			HubPartElement->LinkEndChild(b2Element);
			TiXmlText *b2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*25.6/50.0));
			b2Element->LinkEndChild(b2Content);

			//b3<!--  b3  -->
			TiXmlElement *b3Element = new TiXmlElement("b3");
			HubPartElement->LinkEndChild(b3Element);
			TiXmlText *b3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*49.6/50.0));
			b3Element->LinkEndChild(b3Content);

			//b4<!--  b4  -->
			TiXmlElement *b4Element = new TiXmlElement("b4");
			HubPartElement->LinkEndChild(b4Element);
			TiXmlText *b4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*17.6/50.0));
			b4Element->LinkEndChild(b4Content);

			//b5<!--  b5  -->
			TiXmlElement *b5Element = new TiXmlElement("b5");
			HubPartElement->LinkEndChild(b5Element);
			TiXmlText *b5Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*36.0/50.0));
			b5Element->LinkEndChild(b5Content);

			//b6<!--  b6  -->
			TiXmlElement *b6Element = new TiXmlElement("b6");
			HubPartElement->LinkEndChild(b6Element);
			TiXmlText *b6Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*20.0/50.0));
			b6Element->LinkEndChild(b6Content);

			//h1<!--  h1  -->
			TiXmlElement *h1Element = new TiXmlElement("h1");
			HubPartElement->LinkEndChild(h1Element);
			TiXmlText *h1Content = new TiXmlText("0");
			h1Element->LinkEndChild(h1Content);

			//h2<!--  h2  -->
			TiXmlElement *h2Element = new TiXmlElement("h2");
			HubPartElement->LinkEndChild(h2Element);
			TiXmlText *h2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*24.0/50.0));
			h2Element->LinkEndChild(h2Content);

			//h3<!--  h3  -->
			TiXmlElement *h3Element = new TiXmlElement("h3");
			HubPartElement->LinkEndChild(h3Element);
			TiXmlText *h3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*10.0/50.0));
			h3Element->LinkEndChild(h3Content);

			//h4<!--  h4  -->
			TiXmlElement *h4Element = new TiXmlElement("h4");
			HubPartElement->LinkEndChild(h4Element);
			TiXmlText *h4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*6.0/50.0));
			h4Element->LinkEndChild(h4Content);

			//h5<!--  h5  -->
			TiXmlElement *h5Element = new TiXmlElement("h5");
			HubPartElement->LinkEndChild(h5Element);
			TiXmlText *h5Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.0/50.0));
			h5Element->LinkEndChild(h5Content);

			//h6<!--  h6  -->
			TiXmlElement *h6Element = new TiXmlElement("h6");
			HubPartElement->LinkEndChild(h6Element);
			TiXmlText *h6Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*13.0/50.0));
			h6Element->LinkEndChild(h6Content);

			//h7<!--  h7  -->
			TiXmlElement *h7Element = new TiXmlElement("h7");
			HubPartElement->LinkEndChild(h7Element);
			TiXmlText *h7Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*8.0/50.0));
			h7Element->LinkEndChild(h7Content);

			//r1<!--  r1（叶根）  -->
			TiXmlElement *r1Element = new TiXmlElement("r1");
			HubPartElement->LinkEndChild(r1Element);
			TiXmlText *r1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.5/50.0));
			r1Element->LinkEndChild(r1Content);

			//HubLength<!--  叶根端部长度（去除倒角）  -->
			TiXmlElement *HubLengthElement = new TiXmlElement("HubLength");
			HubPartElement->LinkEndChild(HubLengthElement);
			TiXmlText *HubLengthContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*20.0/50.0));
			HubLengthElement->LinkEndChild(HubLengthContent);

			//r3<!--  r3（叶根）  -->
			TiXmlElement *r3Element = new TiXmlElement("r3");
			HubPartElement->LinkEndChild(r3Element);
			TiXmlText *r3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.5/50.0));
			r3Element->LinkEndChild(r3Content);

			//d1<!--  d1（叶根）  -->
			TiXmlElement *d1Element = new TiXmlElement("d1");
			HubPartElement->LinkEndChild(d1Element);
			TiXmlText *d1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			d1Element->LinkEndChild(d1Content);

			//d3<!--  d3（叶根）  -->
			TiXmlElement *d3Element = new TiXmlElement("d3");
			HubPartElement->LinkEndChild(d3Element);
			TiXmlText *d3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*0.6/50.0));
			d3Element->LinkEndChild(d3Content);

			//bz<!--  中间体轴向宽度的一半bz  -->
			TiXmlElement *bzElement = new TiXmlElement("bz");
			HubPartElement->LinkEndChild(bzElement);
			TiXmlText *bzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*50.0/50.0));
			bzElement->LinkEndChild(bzContent);

			//hr<!--  叶根高度hr  -->
			TiXmlElement *hrElement = new TiXmlElement("hr");
			HubPartElement->LinkEndChild(hrElement);
			TiXmlText *hrContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*83.0/50.0));
			hrElement->LinkEndChild(hrContent);

			//daojiao_c><!--  叶根端部出汽边倒角[mm]daojiaoc=  -->
			TiXmlElement *daojiao_cElement = new TiXmlElement("daojiao_c");
			HubPartElement->LinkEndChild(daojiao_cElement);
			TiXmlText *daojiao_cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*6.0/50.0));
			daojiao_cElement->LinkEndChild(daojiao_cContent);

			//daojiao_cb><!--  叶根下部出汽边上倒角[mm]daojiaocb=（静叶为叶根上部出汽边下倒角）  -->
			TiXmlElement *daojiao_cbElement = new TiXmlElement("daojiao_cb");
			HubPartElement->LinkEndChild(daojiao_cbElement);
			TiXmlText *daojiao_cbContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			daojiao_cbElement->LinkEndChild(daojiao_cbContent);

			//daojiao_r><!--  叶根颈部四周倒圆角[mm]daojiaor=  -->
			TiXmlElement *daojiao_rElement = new TiXmlElement("daojiao_r");
			HubPartElement->LinkEndChild(daojiao_rElement);
			TiXmlText *daojiao_rContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.5/50.0));
			daojiao_rElement->LinkEndChild(daojiao_rContent);

			//rcj><!--  叶根进出汽侧中间体挖槽半径rcj=  -->
			TiXmlElement *rcjElement = new TiXmlElement("rcj");
			HubPartElement->LinkEndChild(rcjElement);
			TiXmlText *rcjContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*3.0/50.0));
			rcjElement->LinkEndChild(rcjContent);


			//轮槽部分
			//HubGroove><!--  轮槽部分  -->//////////////////////////////////
			TiXmlElement *HubGrooveElement = new TiXmlElement("HubGroove");
			HubElement->LinkEndChild(HubGrooveElement);

			//r2><!--  r2（轮槽）  -->
			TiXmlElement *r2Element = new TiXmlElement("r2");
			HubGrooveElement->LinkEndChild(r2Element);
			TiXmlText *r2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*5.0/50.0));
			r2Element->LinkEndChild(r2Content);

			//r4><!--  r4（轮槽）  -->
			TiXmlElement *r4Element = new TiXmlElement("r4");
			HubGrooveElement->LinkEndChild(r4Element);
			TiXmlText *r4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*5.0/50.0));
			r4Element->LinkEndChild(r4Content);

			//d2><!--  d2（轮槽）  -->
			TiXmlElement *d2Element = new TiXmlElement("d2");
			HubGrooveElement->LinkEndChild(d2Element);
			TiXmlText *d2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			d2Element->LinkEndChild(d2Content);

			//d4><!--  d4（轮槽）  -->
			TiXmlElement *d4Element = new TiXmlElement("d4");
			HubGrooveElement->LinkEndChild(d4Element);
			TiXmlText *d4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			d4Element->LinkEndChild(d4Content);

			//lb2><!--  lb2  -->
			TiXmlElement *lb2Element = new TiXmlElement("lb2");
			HubGrooveElement->LinkEndChild(lb2Element);
			TiXmlText *lb2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*30.0/50.0));
			lb2Element->LinkEndChild(lb2Content);

			//lb3><!--  lb3  -->
			TiXmlElement *lb3Element = new TiXmlElement("lb3");
			HubGrooveElement->LinkEndChild(lb3Element);
			TiXmlText *lb3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*58.4/50.0));
			lb3Element->LinkEndChild(lb3Content);

			//lb4><!--  lb4Z轴夹角  -->
			TiXmlElement *lb4Element = new TiXmlElement("lb4");
			HubGrooveElement->LinkEndChild(lb4Element);
			TiXmlText *lb4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*20.6/50.0));
			lb4Element->LinkEndChild(lb4Content);

			//lb5><!--  lb5  -->
			TiXmlElement *lb5Element = new TiXmlElement("lb5");
			HubGrooveElement->LinkEndChild(lb5Element);
			TiXmlText *lb5Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*45.0/50.0));
			lb5Element->LinkEndChild(lb5Content);

			//hy><!--  叶根进汽边沉入转子深度hy  -->
			TiXmlElement *hyElement = new TiXmlElement("hy");
			HubGrooveElement->LinkEndChild(hyElement);
			TiXmlText *hyContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*8.0/50.0));
			hyElement->LinkEndChild(hyContent);

		}
#pragma endregion
	}
	return true;
}


//围带节点数据导出
bool ClassTongliu::ExportShroudXMLData( TiXmlElement* pElem, int HubType ,bladeData tmp)
{
	CString str;
	TiXmlElement *ShroudElement = NULL;
	TiXmlElement *RotorElement = NULL;

	//静叶
	if (HubType == 0)
	{
		//<!--  静叶围带及汽封  -->
		ShroudElement = new TiXmlElement("StatorShroud");
		pElem->LinkEndChild(ShroudElement);

		//c<!--  槽深度常数尺寸  -->
		TiXmlElement *cElement = new TiXmlElement("c");
		ShroudElement->LinkEndChild(cElement);
		TiXmlText *cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.Sc_deep));
		cElement->LinkEndChild(cContent);

	}
	//动叶
	if (HubType == 1)
	{
		//<!--  动叶围带及汽封  -->
		RotorElement = new TiXmlElement("RotorShroud");
		pElem->LinkEndChild(RotorElement);

		//Hmin<!--  进气侧倾角  -->
		TiXmlElement *βElement = new TiXmlElement("β");
		RotorElement->LinkEndChild(βElement);
		TiXmlText *βContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.β));
		βElement->LinkEndChild(βContent);

		//Hmin<!--  每级动叶为了修正离心力及重心对围带挖槽的宽度  -->
		TiXmlElement *p32dElement = new TiXmlElement("p32d");
		RotorElement->LinkEndChild(p32dElement);
		TiXmlText *p32dContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p32d));
		p32dElement->LinkEndChild(p32dContent);

		//c<!--  槽深度常数尺寸  -->
		TiXmlElement *cElement = new TiXmlElement("c");
		RotorElement->LinkEndChild(cElement);
		TiXmlText *cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.c_deep));
		cElement->LinkEndChild(cContent);
	}
	return true;
}




//预读数据
bool ClassTongliu::LoadXMLData()
{
	//TCHAR lpTempPathBuffer[MAX_PATH];
	//GetTempPath(MAX_PATH, lpTempPathBuffer);
	//XMLPath.Format(L"%s%s", lpTempPathBuffer, L"DQXML\\TLData.xml");
	CString XMLPath = Data->MyFunction->Basic->CString2Char(Data->XMLPath);
	aircylinderDataVec.clear();

	TiXmlDocument *myDocument = new TiXmlDocument;
	if (!myDocument->LoadFile(Data->MyFunction->Basic->CString2Char(XMLPath)))
	{
		AfxMessageBox(L"XMLERROR-loadfailed");
		return false;
	}
	
	//Data
	TiXmlElement *DataElement = myDocument->RootElement();
	if (!DataElement)
		return false;


	for (TiXmlElement *pElem1st = DataElement->FirstChildElement(); pElem1st != NULL; pElem1st = pElem1st->NextSiblingElement())
	{
		//Setting程序参数
		if (Data->MyFunction->Basic->Char2CString(pElem1st->Value()).MakeLower() == L"setting")
		{
			for (TiXmlElement *pElem2nd = pElem1st->FirstChildElement(); pElem2nd != NULL; pElem2nd = pElem2nd->NextSiblingElement())
			{
				//ResPath资源路径--块
				if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()).MakeLower() == L"respath")
				{
					setting.ResPath = pElem2nd->FirstChild()->Value();
				}
				//通流图原点
				if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()).MakeLower() == L"originalpoint")
				{
					string str_tmp = pElem2nd->FirstChild()->Value();
					string first, second;
					double firstValue, secondValue;
					splitString(str_tmp, first, second, firstValue, secondValue);
					setting.OriginalPoint = AcGePoint3d(firstValue,secondValue,0);
				}
			}
		}
		//AirCylinder气缸
		if (Data->MyFunction->Basic->Char2CString(pElem1st->Value()).MakeLower() == L"aircylinder")
		{
			//是否是第一个 BladePair 节点
			bool isFirstBladePair_stator = true;

			//预存数据
			aircylinderData theAir;
			vector<bladeData>bladeTmpVec;
			
			//先算p0再算p30   第一個p0 = p0Jz_1 ，不用算
			//p0 =MAX(AI2+AF2+U2+K3-L3,AI2+AF2+X2+H3-I3)
			//p30=MAX(AJ2+C2+L2+T2-U2,AJ2+C2+I2+W2-X2)
			double ForwardSpace_U;		//上一行 正向间隙 AI
			double NegativeSpace_U;		//上一行 负向间隙 AJ
			double p30Dz_U;				//上一行 p30Dz AF
			double p18bw_U;				//上一行 p18bw U
			double p22bz_U;				//上一行 p22bz X

			for (TiXmlElement *pElem2nd = pElem1st->FirstChildElement(); pElem2nd != NULL; pElem2nd = pElem2nd->NextSiblingElement())
			{
				//AirPassage气道类型
				if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()).MakeLower() == L"airpassage")
				{
					theAir.AirPassage = pElem2nd->FirstChild()->Value();
				}
				//LastThreeStages末三级选型及定位数据
				else if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()).MakeLower() == L"lastthreestages")
				{
					theAir.LastThreeStages = pElem2nd->FirstChild()->Value();
				}
				//AirwayCenter  气道中心
				else if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()) == L"AirwayCenter")
				{
					//AirAxialDis  气道中心线轴向距离
					theAir.AirAxialDis = _wtof(Data->MyFunction->Basic->Char2CString(pElem2nd->FirstChildElement("AirAxialDis")->GetText()));
					//AirWeight  汽道中心宽度
					theAir.AirWeight = _wtof(Data->MyFunction->Basic->Char2CString(pElem2nd->FirstChildElement("AirWeight")->GetText()));
					//CantileverWidth  悬臂宽度
					theAir.CantileverWidth = _wtof(Data->MyFunction->Basic->Char2CString(pElem2nd->FirstChildElement("CantileverWidth")->GetText()));

				}
				//isHorizonBlade 是否含有横置叶片 1是0否
				else if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()) == L"IsHorizonBlade")
				{
					theAir.isHorizonBlade = _wtof(Data->MyFunction->Basic->Char2CString(pElem2nd->FirstChild()->Value()));
				}
				//p0Jz_1 原点到第1级静叶中心线的距离 
				else if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()) == L"p0Jz_1")
				{
					if(theAir.isHorizonBlade == 1.0)
						theAir.p0Jz_1 = _wtof(Data->MyFunction->Basic->Char2CString(pElem2nd->FirstChild()->Value()));
				}
				//BladePair叶片对
				else if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()).MakeLower() == L"bladepair")
				{
					bladeData bladeTmp;
#pragma region
					for (TiXmlElement *pElem3rd = pElem2nd->FirstChildElement(); pElem3rd != NULL; pElem3rd = pElem3rd->NextSiblingElement())
					{
						//SuctionPort抽口位置及数据
						if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()).MakeLower() == L"suctionport")
						{
							bladeTmp.IsSuctionPort = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChildElement("IsSuctionPort")->GetText()));
							bladeTmp.InletWeight = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChildElement("InletWeight")->GetText()));
							bladeTmp.OutletWeight = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChildElement("OutletWeight")->GetText()));
							bladeTmp.CoverDegree = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChildElement("CoverDegree")->GetText()));
							bladeTmp.V = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChildElement("V")->GetText()));
							bladeTmp.G = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChildElement("G")->GetText()));
							bladeTmp.MidY = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChildElement("MidY")->GetText()));

							string MaxAllowFlow_str = pElem3rd->FirstChildElement("MaxAllowFlow")->GetText();
							if(MaxAllowFlow_str == "高压"  || MaxAllowFlow_str == "中压")
								bladeTmp.μ = 50.0;
							else if(MaxAllowFlow_str == "低压")
								bladeTmp.μ = 70.0;
							else
								bladeTmp.μ = 0.0;
							
						}
						//p31zf 汽流方向，定义数值=1，汽流从左向右，否则标其他数字，非空
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"p31zf")
						{
							bladeTmp.p31zf = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
							//双分流默认为0   否则标注不对称
							if(theAir.AirPassage=="双分流")
								bladeTmp.p31zf =0.0;
						}
						//Broken 折流
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"Broken")
						{
							bladeTmp.BrokenTag = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChildElement("BrokenTag")->GetText()));
							bladeTmp.BrokenDis = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChildElement("BrokenDis")->GetText()));
						}
						//Margin 余量
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"Margin")
						{
							bladeTmp.Margin = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
						}
						//Jmin><!--  进气侧汽封槽到上级叶片的距离 J最小距离 -->
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"Jmin")
						{
							bladeTmp.Jmin = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
						}
						//Hmin><!--  出气侧汽封槽到下级叶片的距离 H最小距离 -->
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"Hmin")
						{
							bladeTmp.Hmin = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
						}
						//P><!--  正胀差  -->
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"P")
						{
							bladeTmp.P = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
							bladeTmp.P = abs(bladeTmp.P);
							//正向间隙 AI
							bladeTmp.ForwardSpace = Data->MyFunction->Basic->ceilToSignificance(bladeTmp.P+bladeTmp.Margin,0.5);
						}
						//Q><!--  负胀差  -->
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"Q")
						{
							bladeTmp.Q = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
							bladeTmp.Q = abs(bladeTmp.Q);
							//负向间隙 AJ
							bladeTmp.NegativeSpace =Data->MyFunction->Basic->ceilToSignificance(bladeTmp.Q+bladeTmp.Margin,0.5);
						}
						//W><!--  稳态胀差 -->
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"W")
						{
							bladeTmp.W = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
						}
						//N><!--  每个气道的布置方向与推力轴承的关系Nn值
						//当气道1在推力轴承右侧且为反向布置，或气道1在推力轴承左侧且正向布置，定义：N1=0
						//当气道1在推力轴承右侧且为正向布置，或气道1在推力轴承左侧且反向布置，定义：N1=1
						//-->
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"N")
						{
							bladeTmp.N = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
							if(bladeTmp.N == 0)
							{
								bladeTmp.X = bladeTmp.P;
								bladeTmp.Y = bladeTmp.Q;
							}
							else if(bladeTmp.N == 1)
							{
								bladeTmp.X = bladeTmp.Q;
								bladeTmp.Y = bladeTmp.P;
							}
						}
						//Blade叶片数据
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()).MakeLower() == L"blade")
						{
							string str_type = pElem3rd->FirstChildElement("BladeType")->GetText();
							if(str_type == "stator")
							{
								//Profile静叶片数据
#pragma region	
								//如果是第一级叶片并且有横置叶片
								if(isFirstBladePair_stator && theAir.isHorizonBlade == 1.0)
								{
									ForwardSpace_U = bladeTmp.ForwardSpace;		//上一行 正向间隙 AI
									NegativeSpace_U = bladeTmp.NegativeSpace;	//上一行 负向间隙 AJ
									continue;
								}
								TiXmlElement *profileElement =pElem3rd->FirstChildElement("Profile");
								if (profileElement)
								{
									bladeTmp.p1BG = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p1BG")->GetText()));
									bladeTmp.p2BD = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p2BD")->GetText()));
									bladeTmp.p3bg = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p3bg")->GetText()));
									bladeTmp.p4bd = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p4bd")->GetText()));
									bladeTmp.p5Bw = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p5Bw")->GetText()));
									bladeTmp.p6bw = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p6bw")->GetText()));
									bladeTmp.p7hw = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p7hw")->GetText()));
									bladeTmp.p8Bz = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p8Bz")->GetText()));
									bladeTmp.p9bz = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p9bz")->GetText()));
									bladeTmp.p10hz = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p10hz")->GetText()));
									bladeTmp.p26Dpp = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p26Dpp")->GetText()))+ setting.OriginalPoint.y;
									bladeTmp.p27Lp = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p27Lp")->GetText()));
									bladeTmp.p11ad = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p11ad")->GetText()));
									bladeTmp.p12ag = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p12ag")->GetText()));
									
									//如果是第一级叶片并且同时没有横置叶片
									if (isFirstBladePair_stator)
									{
										// 处理第一个 BladePair 节点的情况  第一个p0不需要计算
										isFirstBladePair_stator = false;
										//当没有横置静叶时，第一级P0=0.5*汽道宽度+悬臂宽度+0.5*第一级静叶中间体宽（p8Bz-p9bz）
										theAir.p0Jz_1 = 0.5 * theAir.AirWeight + theAir.CantileverWidth + 0.5*(bladeTmp.p8Bz - bladeTmp.p9bz) ;
										// + 气道轴向距离
										bladeTmp.p0Jz = theAir.p0Jz_1 + theAir.AirAxialDis + setting.OriginalPoint.x;
										
										//存此行  下一行p0要用
										ForwardSpace_U = bladeTmp.ForwardSpace;		//上一行 正向间隙 AI
										NegativeSpace_U = bladeTmp.NegativeSpace;	//上一行 负向间隙 AJ
									}
									else
									{
										if(bladeTmp.BrokenTag==0.0)
										{
											// 处理非第一个 BladePair 节点的情况
											////p0 =MAX(AI2+AF2+U2+K3-L3,AI2+AF2+X2+H3-I3)
											double u_num = 0.0;
											if(bladeTmp.N == 0)
												u_num = ForwardSpace_U ;
											else
												u_num = NegativeSpace_U ;

											double tmp1 = u_num + p30Dz_U + p18bw_U + bladeTmp.p8Bz - bladeTmp.p9bz;
											double tmp2 = u_num + p30Dz_U + p22bz_U + bladeTmp.p5Bw - bladeTmp.p6bw;
											bladeTmp.p0Jz = max(tmp1,tmp2);
										
										}
										else
										{
											bladeTmp.p0Jz = bladeTmp.BrokenDis+theAir.AirAxialDis;
										}
										
										//存此行  下一行p0要用
										ForwardSpace_U = bladeTmp.ForwardSpace;		//上一行 正向间隙 AI
										NegativeSpace_U = bladeTmp.NegativeSpace;	//上一行 负向间隙 AJ
									}
								}

								//StatorHub静叶根数据
								TiXmlElement *statorhubElement =pElem3rd->FirstChildElement("StatorHub");
								if (statorhubElement)
								{
									bladeTmp.SHubMType = statorhubElement->FirstChildElement("HubMType")->GetText();
									bladeTmp.SHubType = _wtof(Data->MyFunction->Basic->Char2CString(statorhubElement->FirstChildElement("HubType")->GetText()));

									TiXmlElement *hubpartElement =statorhubElement->FirstChildElement("HubPart");
									if (hubpartElement)
									{
										bladeTmp.Sbn = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("bn")->GetText()));
										bladeTmp.Shn = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("hn")->GetText()));
										bladeTmp.Shx = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("hx")->GetText()));
										bladeTmp.Sbz = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("bz")->GetText()));
										bladeTmp.Sdaojiao_c = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("daojiao_c")->GetText()));
										bladeTmp.Sdaojiao_cb = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("daojiao_cb")->GetText()));
										bladeTmp.Sdaojiao_r = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("daojiao_r")->GetText()));
										bladeTmp.Srcj = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("rcj")->GetText()));
										bladeTmp.SHubHorizontalAngle = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("HubHorizontalAngle")->GetText()));
									}

									TiXmlElement *hubgrooveElement =statorhubElement->FirstChildElement("HubGroove");
									if (hubgrooveElement)
									{
										bladeTmp.Shy = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("hy")->GetText()));
										bladeTmp.SInterstitialsR = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("InterstitialsR")->GetText()));
										bladeTmp.SGrooveSurfaceH = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveSurfaceH")->GetText()));
										bladeTmp.SGrooveNeckH = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveNeckH")->GetText()));
										bladeTmp.SGrooveBottomH = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveBottomH")->GetText()));
										bladeTmp.SMidBodyH = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("MidBodyH")->GetText()));
										bladeTmp.SGrooveNeckW = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveNeckW")->GetText()));
										bladeTmp.SGrooveNeckChamfer = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveNeckChamfer")->GetText()));
										bladeTmp.SGrooveBottomW = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveBottomW")->GetText()));
										bladeTmp.SGrooveVerticalAngle = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveVerticalAngle")->GetText()));
										bladeTmp.SUCornerR = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("UCornerR")->GetText()));
										bladeTmp.SDCornerR = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("DCornerR")->GetText()));
										
										//if(bladeTmp.p31zf == 1.0)//气流方向从左向右静叶进气侧
										bladeTmp.S_deep_point = AcGePoint3d(bladeTmp.p0Jz -bladeTmp.p8Bz +bladeTmp.p9bz ,bladeTmp.p26Dpp +bladeTmp.p27Lp/2.0 +bladeTmp.p10hz -bladeTmp.Shy,0);
										/*else
											bladeTmp.S_deep_point = AcGePoint3d(bladeTmp.p0Jz +bladeTmp.p9bz ,bladeTmp.p26Dpp +bladeTmp.p27Lp/2.0 +bladeTmp.p10hz -bladeTmp.Shy,0);*/
									}
								}

								//StatorShroud静叶围带及汽封
								TiXmlElement *statorshroudElement =pElem3rd->FirstChildElement("StatorShroud");
								if (statorshroudElement)
								{
									bladeTmp.Sc_deep = _wtof(Data->MyFunction->Basic->Char2CString(statorshroudElement->FirstChildElement("c")->GetText()));
								}
							}
#pragma endregion
							else if(str_type == "rotor")
							{
								//Profile动叶片数据
#pragma region
								TiXmlElement *profileElement =pElem3rd->FirstChildElement("Profile");
								if (profileElement)
								{
									bladeTmp.p13BG = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p13BG")->GetText()));
									bladeTmp.p14BD = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p14BD")->GetText()));
									bladeTmp.p15bg = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p15bg")->GetText()));
									bladeTmp.p16bd = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p16bd")->GetText()));
									bladeTmp.p17Bw = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p17Bw")->GetText()));
									bladeTmp.p18bw = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p18bw")->GetText()));
									bladeTmp.p19hw = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p19hw")->GetText()));
									bladeTmp.p21Bz = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p21Bz")->GetText()));
									bladeTmp.p22bz = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p22bz")->GetText()));
									bladeTmp.p23hz = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p23hz")->GetText()));
									bladeTmp.p28Dpd = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p28Dpd")->GetText()))+ setting.OriginalPoint.y;
									bladeTmp.p29Ld = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p29Ld")->GetText()));
									bladeTmp.p24ad = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p24ad")->GetText()));
									bladeTmp.p25ag = _wtof(Data->MyFunction->Basic->Char2CString(profileElement->FirstChildElement("p25ag")->GetText()));
									
									//如果是第一级叶片并且有横置叶片
									if(isFirstBladePair_stator && theAir.isHorizonBlade == 1.0)
									{
										// 处理第一个 BladePair 节点的情况  第一个p30不需要计算
										isFirstBladePair_stator = false;
										bladeTmp.p30Dz = theAir.p0Jz_1+ + theAir.AirAxialDis + setting.OriginalPoint.x;
									}
									else
									{
										//=MAX(AJ2+C2+L2+T2-U2,AJ2+C2+I2+W2-X2)
										double u_num = 0.0;
										if(bladeTmp.N == 0)
											u_num = bladeTmp.NegativeSpace ;
										else
											u_num = bladeTmp.ForwardSpace ;

										double tmp1 = u_num + bladeTmp.p0Jz + bladeTmp.p9bz + bladeTmp.p17Bw - bladeTmp.p18bw;
										double tmp2 = u_num + bladeTmp.p0Jz + bladeTmp.p6bw + bladeTmp.p21Bz - bladeTmp.p22bz;
										//计算
										bladeTmp.p30Dz = max(tmp1,tmp2);
									}
									

									//存此行  下一行p0要用
									p30Dz_U = bladeTmp.p30Dz;				//上一行 p30Dz AF
									p18bw_U = bladeTmp.p18bw;				//上一行 p18bw U
									p22bz_U = bladeTmp.p22bz;				//上一行 p22bz X
								
								}

								//RotorHub动叶根数据
								TiXmlElement *rotorhubElement =pElem3rd->FirstChildElement("RotorHub");
								if (rotorhubElement)
								{
									bladeTmp.HubMType = rotorhubElement->FirstChildElement("HubMType")->GetText();
									bladeTmp.HubType = _wtof(Data->MyFunction->Basic->Char2CString(rotorhubElement->FirstChildElement("HubType")->GetText()));

									TiXmlElement *hubpartElement =rotorhubElement->FirstChildElement("HubPart");
									if (hubpartElement)
									{
										for (TiXmlElement *pElem4th = hubpartElement->FirstChildElement(); pElem4th != NULL; pElem4th = pElem4th->NextSiblingElement())
										{
											if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()).MakeLower() == L"bn")
											{
												bladeTmp.bn = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"hn")
											{
												bladeTmp.hn = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"hx")
											{
												bladeTmp.hx = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"bz")
											{
												bladeTmp.bz = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"daojiao_c")
											{
												bladeTmp.daojiao_c = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"daojiao_cb")
											{
												bladeTmp.daojiao_cb = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"daojiao_r")
											{
												bladeTmp.daojiao_r = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"rcj")
											{
												bladeTmp.rcj = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"HubHorizontalAngle")
											{
												bladeTmp.HubHorizontalAngle = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"MidBodySurW")
											{
												bladeTmp.MidBodySurW = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"HubDW")
											{
												bladeTmp.HubDW = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"TUestW")
											{
												bladeTmp.TUestW = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"TZAngle")
											{
												bladeTmp.TZAngle = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckDCornerR")
											{
												bladeTmp.NeckDCornerR = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckUCornerR")
											{
												bladeTmp.NeckUCornerR = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckMCornerR")
											{
												bladeTmp.NeckMCornerR = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"TAngle")
											{
												bladeTmp.TAngle = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"TSFMidCornerR")
											{
												bladeTmp.TSFMidCornerR = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"SFInAngle")
											{
												bladeTmp.SFInAngle = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"b1")
											{
												bladeTmp.b1 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"b2")
											{
												bladeTmp.b2 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"b3")
											{
												bladeTmp.b3 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"b4")
											{
												bladeTmp.b4 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"b5")
											{
												bladeTmp.b5 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"b6")
											{
												bladeTmp.b6 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"h1")
											{
												bladeTmp.h1 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"h2")
											{
												bladeTmp.h2 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"h3")
											{
												bladeTmp.h3 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"h4")
											{
												bladeTmp.h4 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"h5")
											{
												bladeTmp.h5 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"h6")
											{
												bladeTmp.h6 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"h7")
											{
												bladeTmp.h7 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"r1")
											{
												bladeTmp.r1 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"r3")
											{
												bladeTmp.r3 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"d1")
											{
												bladeTmp.d1 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"d3")
											{
												bladeTmp.d3 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"h")
											{
												bladeTmp.h = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"HubSFTol")
											{
												bladeTmp.HubSFTol = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckDisTol")
											{
												bladeTmp.NeckDisTol = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"HubLength")
											{
												bladeTmp.HubLength = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"hr")
											{
												bladeTmp.hr = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"TSFH")
											{
												bladeTmp.TSFH = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
										}
										//bladeTmp.bn = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("bn")->GetText()));
										//bladeTmp.hn = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("hn")->GetText()));
										//bladeTmp.hx = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("hx")->GetText()));
										//bladeTmp.bz = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("bz")->GetText()));
										//bladeTmp.daojiao_c = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("daojiao_c")->GetText()));
										//bladeTmp.daojiao_cb = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("daojiao_cb")->GetText()));
										//bladeTmp.daojiao_r = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("daojiao_r")->GetText()));
										//bladeTmp.rcj = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("rcj")->GetText()));
										//bladeTmp.HubHorizontalAngle = _wtof(Data->MyFunction->Basic->Char2CString(hubpartElement->FirstChildElement("HubHorizontalAngle")->GetText()));
									}

									TiXmlElement *hubgrooveElement =rotorhubElement->FirstChildElement("HubGroove");
									if (hubgrooveElement)
									{
										for (TiXmlElement *pElem4th = hubgrooveElement->FirstChildElement(); pElem4th != NULL; pElem4th = pElem4th->NextSiblingElement())
										{
											if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()).MakeLower() == L"hy")
											{
												bladeTmp.hy = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"InterstitialsR")
											{
												bladeTmp.InterstitialsR = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"GrooveSurfaceH")
											{
												bladeTmp.GrooveSurfaceH = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"GrooveNeckH")
											{
												bladeTmp.GrooveNeckH = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"GrooveBottomH")
											{
												bladeTmp.GrooveBottomH = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"MidBodyH")
											{
												bladeTmp.MidBodyH = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"GrooveNeckW")
											{
												bladeTmp.GrooveNeckW = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"GrooveNeckChamfer")
											{
												bladeTmp.GrooveNeckChamfer = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"GrooveBottomW")
											{
												bladeTmp.GrooveBottomW = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"GrooveVerticalAngle")
											{
												bladeTmp.GrooveVerticalAngle = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"UCornerR")
											{
												bladeTmp.UCornerR = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"DCornerR")
											{
												bladeTmp.DCornerR = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckH")
											{
												bladeTmp.NeckH = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"SFH")
											{
												bladeTmp.SFH = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckDH")
											{
												bladeTmp.NeckDH = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckW")
											{
												bladeTmp.NeckW = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckDW")
											{
												bladeTmp.NeckDW = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckUestW")
											{
												bladeTmp.NeckUestW = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckZAngle")
											{
												bladeTmp.NeckZAngle = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckDAngle")
											{
												bladeTmp.NeckDAngle = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"RaceNeckMCornerR")
											{
												bladeTmp.RaceNeckMCornerR = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"RaceUCornerR")
											{
												bladeTmp.RaceUCornerR = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"RaceDCornerR")
											{
												bladeTmp.RaceDCornerR = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"RaceSFAngle")
											{
												bladeTmp.RaceSFAngle = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"RaceSFCornerR")
											{
												bladeTmp.RaceSFCornerR = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"r2")
											{
												bladeTmp.r2 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"r4")
											{
												bladeTmp.r4 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"d2")
											{
												bladeTmp.d2 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"d4")
											{
												bladeTmp.d4 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"lb2")
											{
												bladeTmp.lb2 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"lb3")
											{
												bladeTmp.lb3 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"lb4")
											{
												bladeTmp.lb4 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"lb5")
											{
												bladeTmp.lb5 = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
											else if (Data->MyFunction->Basic->Char2CString(pElem4th->Value()) == L"NeckUAngle")
											{
												bladeTmp.NeckUAngle = _wtof(Data->MyFunction->Basic->Char2CString(pElem4th->FirstChild()->Value()));
											}
										}

										/*bladeTmp.hy = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("hy")->GetText()));
										bladeTmp.InterstitialsR = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("InterstitialsR")->GetText()));
										bladeTmp.GrooveSurfaceH = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveSurfaceH")->GetText()));
										bladeTmp.GrooveNeckH = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveNeckH")->GetText()));
										bladeTmp.GrooveBottomH = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveBottomH")->GetText()));
										bladeTmp.MidBodyH = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("MidBodyH")->GetText()));
										bladeTmp.GrooveNeckW = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveNeckW")->GetText()));
										bladeTmp.GrooveNeckChamfer = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveNeckChamfer")->GetText()));
										bladeTmp.GrooveBottomW = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveBottomW")->GetText()));
										bladeTmp.GrooveVerticalAngle = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("GrooveVerticalAngle")->GetText()));
										bladeTmp.UCornerR = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("UCornerR")->GetText()));
										bladeTmp.DCornerR = _wtof(Data->MyFunction->Basic->Char2CString(hubgrooveElement->FirstChildElement("DCornerR")->GetText()));*/

										//if(bladeTmp.p31zf == 1.0)//气流方向从左向右
										bladeTmp.deep_point = AcGePoint3d(bladeTmp.p30Dz -bladeTmp.p21Bz +bladeTmp.p22bz,bladeTmp.p28Dpd-bladeTmp.p29Ld/2.0-bladeTmp.p23hz+bladeTmp.hy,0);
										bladeTmp.rator_deeppoint = AcGePoint3d(bladeTmp.deep_point.x,bladeTmp.deep_point.y,bladeTmp.deep_point.z);
										/*else
											bladeTmp.deep_point = AcGePoint3d(bladeTmp.p30Dz +bladeTmp.p22bz,bladeTmp.p28Dpd-bladeTmp.p29Ld/2.0-bladeTmp.p23hz+bladeTmp.hy,0);*/

									}
								}

								//RotorShroud动叶围带及汽封
								TiXmlElement *rotorshroudElement =pElem3rd->FirstChildElement("RotorShroud");
								if (rotorshroudElement)
								{
									bladeTmp.β = _wtof(Data->MyFunction->Basic->Char2CString(rotorshroudElement->FirstChildElement("β")->GetText()));
									bladeTmp.p32d = _wtof(Data->MyFunction->Basic->Char2CString(rotorshroudElement->FirstChildElement("p32d")->GetText()));
									bladeTmp.c_deep = _wtof(Data->MyFunction->Basic->Char2CString(rotorshroudElement->FirstChildElement("c")->GetText()));
								}
							}
#pragma endregion
						}
					}//////////////////////////////////

					bladeTmpVec.push_back(bladeTmp);
				}
#pragma endregion

			}
			//循环bladeTmpVec，计算部分参数
			for(int i=0;i<bladeTmpVec.size();i++)
			{
				bladeData theTmp = bladeTmpVec[i];

				//A =IF(p5Bw=0,0,IF(p9bz+p17Bw-p18bw>=p6bw+p21Bz-p22bz,ROUNDUP((H5+$A$2+L5+M5-N5)*2,0)/2-L5-M5+N5,ROUNDUP((H5+$A$2+J5+O5-P5)*2,0)/2-L5-M5+N5))
#pragma region
				if(theTmp.p5Bw == 0)
					theTmp.B = 0;
				else
				{
					double tmp1 = theTmp.p9bz+theTmp.p17Bw-theTmp.p18bw;
					//判断(L5+M5-N5>=J5+O5-P32
					if(tmp1>=theTmp.p6bw+theTmp.p21Bz-theTmp.p22bz)
					{
						//ROUNDUP((H5+$A$2+L5+M5-N5)*2,0)/2-L5-M5+N5
						double mid1 = (theTmp.Y+theTmp.Margin+tmp1)*2;
						theTmp.A = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-tmp1;
					}
					else
					{
						//ROUNDUP((H5+$A$2+J5+O5-P5)*2,0)/2-L5-M5+N5))
						double mid2 = theTmp.p6bw+theTmp.p21Bz-theTmp.p22bz;
						double mid1 = (theTmp.Y+theTmp.Margin+mid2)*2;
						theTmp.A = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-tmp1;
					}
				}
#pragma endregion

				//B =IF(I4=0,0,IF(L4+M4-N4<J4+O4-P4,ROUNDUP((H4+$A$2+J4+O4-P4)*2,0)/2-J4-O4+P4,ROUNDUP((H4+$A$2+L4+M4-N4)*2,0)/2-J4-O4+P4))
#pragma region
				if(theTmp.p5Bw == 0)
					theTmp.B = 0;
				else
				{
					//判断L4+M4-N4是否小于J4+O4-P4
					if(theTmp.p9bz+theTmp.p17Bw-theTmp.p18bw<theTmp.p6bw+theTmp.p21Bz-theTmp.p22bz)
					{
						//ROUNDUP((H4+$A$2+J4+O4-P4)*2,0)/2-J4-O4+P4
						double mid2 = theTmp.p6bw+theTmp.p21Bz-theTmp.p22bz;
						double mid1 = (theTmp.Y+theTmp.Margin+mid2)*2;
						theTmp.B = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-mid2;
					}
					else
					{
						//ROUNDUP((H4+$A$2+L4+M4-N4)*2,0)/2-J4-O4+P4
						double mid2 = theTmp.p6bw+theTmp.p21Bz-theTmp.p22bz;
						double mid3 = theTmp.p9bz+theTmp.p17Bw-theTmp.p18bw;
						double mid1 = (theTmp.Y+theTmp.Margin+mid3)*2;
						theTmp.B = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-mid2;
					}
				}
#pragma endregion

				//E =IF(N4+K5-L5>=P4+I5-J5,ROUNDUP((G4+$A$2+N4+K5-L5)*2,0)/2-N4-K5+L5,ROUNDUP((G4+$A$2+P4+I5-J5)*2,0)/2-N4-K5+L5)
#pragma region

				//最后一个叶片
				if(i==bladeTmpVec.size()-1)
				{
					double tmp1 = theTmp.p18bw;
					//+P4+I5-J5
					double tmp2 = theTmp.p22bz;

					if(tmp1>=tmp1)
					{
						//ROUNDUP((G4+$A$2+N4+K5-L5)*2,0)/2-N4-K5+L5
						double mid1 = (theTmp.X+theTmp.Margin+tmp1)*2;
						theTmp.E = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-tmp1;
					}
					else
					{
						//ROUNDUP((G4+$A$2+P4+I5-J5)*2,0)/2-N4-K5+L5)
						double mid1 = (theTmp.X+theTmp.Margin+tmp2)*2;
						theTmp.E = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-tmp1;
					}
				}
				else
				{
					//=IF(N4+K5-L5>=P4+I5-J5,ROUNDUP((G4+$A$2+N4+K5-L5)*2,0)/2-N4-K5+L5,ROUNDUP((G4+$A$2+P4+I5-J5)*2,0)/2-N4-K5+L5)
					bladeData NextTmp = bladeTmpVec[i+1];
					//N4+K5-L5
					double tmp1 = theTmp.p18bw+NextTmp.p8Bz-NextTmp.p9bz;
					//+P4+I5-J5
					double tmp2 = theTmp.p22bz+NextTmp.p5Bw-NextTmp.p6bw;

					if(tmp1>=tmp1)
					{
						//ROUNDUP((G4+$A$2+N4+K5-L5)*2,0)/2-N4-K5+L5
						double mid1 = (theTmp.X+theTmp.Margin+tmp1)*2;
						theTmp.E = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-tmp1;
					}
					else
					{
						//ROUNDUP((G4+$A$2+P4+I5-J5)*2,0)/2-N4-K5+L5)
						double mid1 = (theTmp.X+theTmp.Margin+tmp2)*2;
						theTmp.E = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-tmp1;
					}
				}
#pragma endregion

				//F =IF(N3+K4-L4<P3+I4-J4,ROUNDUP((G3+$A$2+P3+I4-J4)*2,0)/2-P3-I4+J4,ROUNDUP((G3+$A$2+N3+K4-L4)*2,0)/2-P3-I4+J4)
#pragma region
				//如果是第一个叶片 
				if(i==0)
				{
					theTmp.F = theTmp.X+theTmp.Margin;
				}
				//最后一个叶片
				else if(i==bladeTmpVec.size()-1)
				{
					double tmp1 = theTmp.p22bz;

					if((theTmp.p18bw)<tmp1)
					{
						//ROUNDUP((G4+$A$2+P4+I5-J5)*2,0)/2-P4-I5+J5
						double mid1 = (theTmp.X+theTmp.Margin+tmp1)*2;
						theTmp.F = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-tmp1;
					}
					else
					{
						//ROUNDUP((G4+$A$2+N4+K5-L5)*2,0)/2-P4-I5+J5)
						double mid1 = (theTmp.X+theTmp.Margin+(theTmp.p18bw))*2;
						theTmp.F = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-tmp1;
					}
				}
				else
				{
					//=IF(N4+K5-L5<P4+I5-J5,ROUNDUP((G4+$A$2+P4+I5-J5)*2,0)/2-P4-I5+J5,ROUNDUP((G4+$A$2+N4+K5-L5)*2,0)/2-P4-I5+J5)
					bladeData NextTmp = bladeTmpVec[i+1];
					//+P4+I5-J5
					double tmp1 = theTmp.p22bz+NextTmp.p5Bw-NextTmp.p6bw;

					if((theTmp.p18bw+NextTmp.p8Bz-NextTmp.p9bz)<tmp1)
					{
						//ROUNDUP((G4+$A$2+P4+I5-J5)*2,0)/2-P4-I5+J5
						double mid1 = (theTmp.X+theTmp.Margin+tmp1)*2;
						theTmp.F = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-tmp1;
					}
					else
					{
						//ROUNDUP((G4+$A$2+N4+K5-L5)*2,0)/2-P4-I5+J5)
						double mid1 = (theTmp.X+theTmp.Margin+(theTmp.p18bw+NextTmp.p8Bz-NextTmp.p9bz))*2;
						theTmp.F = Data->MyFunction->Basic->roundNumber(mid1,0)/2.0-tmp1;
					}
				}
#pragma endregion

				//Z_R =IF(M4-Q4<F4+1.27*2+MAX(H4+$A$2+F4,3),0,IF(AND(M4-Q4<F4+1.27*3+MAX(G4+$A$2,3)+MAX(H4+$A$2+F4,3),M4-Q4>=F4+1.27*2+MAX(H4+$A$2+F4,3)),ROUNDUP(F4+1.27+H4+$A$2,1),ROUNDUP(G4+$A$2+1.27+H4+$A$2,1)))
#pragma region
				double temp1 = theTmp.W + 1.27 * 2 + max(theTmp.Y + theTmp.Margin + theTmp.W, 3.0);
				double temp2 = theTmp.W + 1.27 * 3 + max(theTmp.X + theTmp.Margin, 3.0) + max(theTmp.Y + theTmp.Margin + theTmp.W, 3.0);

				if (theTmp.p17Bw - theTmp.p32d < temp1) 
				{
					theTmp.Z_R = 0;
				} 
				else if (theTmp.p17Bw - theTmp.p32d < temp2 && theTmp.p17Bw - theTmp.p32d >= temp1) 
				{
					theTmp.Z_R = Data->MyFunction->Basic->roundNumber(theTmp.W + 1.27 + theTmp.Y + theTmp.Margin,1);
				} 
				else
				{
					theTmp.Z_R = Data->MyFunction->Basic->roundNumber(theTmp.X + theTmp.Margin + 1.27 + theTmp.Y + theTmp.Margin,1);
				}

#pragma endregion

				//Z_S =IF(I4=0,"",ROUNDUP(G4+$A$2+1.27+H4+$A$2,1))
#pragma region
				if(theTmp.p5Bw == 0)
					theTmp.Z_S = 0;
				else
				{
					//ROUNDUP(G4+$A$2+1.27+H4+$A$2,1)
					double mid1 = theTmp.X+theTmp.Margin+1.27+theTmp.Y+theTmp.Margin;
					theTmp.Z_S = Data->MyFunction->Basic->roundNumber(mid1,1);

				}
#pragma endregion

				//a =IF(M4-Q4>=F4+1.27*2+MAX(H4+$A$2+F4,3),ROUNDUP(V4+F4+1.27,1),"")
#pragma region
				temp1 = theTmp.W + 1.27 * 2 + max(theTmp.Y + theTmp.Margin + theTmp.W, 3);

				if (theTmp.p17Bw - theTmp.p32d >= temp1) 
				{
					//ROUNDUP(V4+F4+1.27,1)
					double mid1 = theTmp.A+theTmp.W +1.27;
					theTmp.a = Data->MyFunction->Basic->roundNumber(mid1,1);
				}
				else
				{
					theTmp.a = 0;
				}
#pragma endregion

				//t=rounddown[(p17Bw-p32d-W)/4.3,0]
#pragma region
				temp1 = theTmp.W + 1.27 * 3 + max(theTmp.X + theTmp.Margin , 3.0)+ max(theTmp.Y + theTmp.Margin + theTmp.W, 3.0);
				temp2 = theTmp.W + 1.27 * 2 + max(theTmp.Y + theTmp.Margin + theTmp.W, 3);
				//p17Bw-p32d≥W+1.27*3+max[(X+1.1),3]+max[(Y+1.1+W),3],t=3
				if (theTmp.p17Bw - theTmp.p32d >= temp1) 
				{
					theTmp.t = 3;
					theTmp.t_type = 1;
				}
				//W+1.27*3+max[(X+1.1),3]+max[(Y+1.1+W),3]>p17Bw-p32d≥W+1.27*2+max[(Y+1.1+W),3],t=2
				else if(temp1>theTmp.p17Bw - theTmp.p32d && theTmp.p17Bw - theTmp.p32d>=temp2)
				{
					theTmp.t = 2;
					theTmp.t_type = 2;
				}
				else
				{
					//rounddown[(p17Bw-p32d-W)/4.3,0]
					theTmp.t = Data->MyFunction->Basic->floorNumber((theTmp.p17Bw - theTmp.p32d- theTmp.W)/4.3,0);
					theTmp.t_type = 3;
				}
#pragma endregion

				//b =IF(M4-Q4<F4+1.27*2+MAX(H4+$A$2+F4,3),ROUNDUP(V4+F4+1.27,1)+4.3*(AG4-1),IF(AND(M4-Q4<F4+1.27*3+MAX(G4+$A$2,3)+MAX(H4+$A$2+F4,3),(M4-Q4>=F4+1.27*2+MAX(H4+$A$2+F4,3))),ROUNDUP(V4+F4+1.27,1),ROUNDUP(MAX((M4-Q4-AD4)/2+V4-AF4+G4+$A$2,3)+AF4+1.27,1)))
#pragma region
				temp1 = theTmp.W + 1.27 * 2 + max(theTmp.Y + theTmp.Margin + theTmp.W, 3);
				temp2 = theTmp.W + 1.27 * 3 + max(theTmp.X + theTmp.Margin, 3) + max(theTmp.Y + theTmp.Margin + theTmp.W, 3);

				if (theTmp.p17Bw - theTmp.p32d < temp1) 
				{
					//最后一个汽封齿的位置  第t个汽封片轴向定位尺寸b=roundup[(A+W+1.27),1]+4.3(t-1)
					theTmp.b = Data->MyFunction->Basic->roundNumber(theTmp.A+theTmp.W+1.27,1)+4.3*(theTmp.t-1);
				} 
				else if (theTmp.p17Bw - theTmp.p32d < temp2 && theTmp.p17Bw - theTmp.p32d >= temp1) 
				{
					//ROUNDUP(V4+F4+1.27,1)
					theTmp.b = Data->MyFunction->Basic->roundNumber(theTmp.A+theTmp.W+1.27,1);
				} 
				else
				{
					//ROUNDUP(	MAX((M7-Q7-AD7)/2+V7-AF7+G7+$A$2,3)	+AF7+1.27,1)
					double mid1 = max((theTmp.p17Bw - theTmp.p32d -theTmp.Z_R)/2.0+theTmp.A- theTmp.a+theTmp.X+ theTmp.Margin,3)+theTmp.a+1.27;
					theTmp.b = Data->MyFunction->Basic->roundNumber(mid1,1);
				}
#pragma endregion

				//c =IF(M5-Q5<F5+1.27*2+MAX(H5+$A$2+F5,3),"",IF(AND(M5-Q5<F5+1.27*3+MAX(G5+$A$2,3)+MAX(H5+$A$2+F5,3),(M5-Q5>=F5+1.27*2+MAX(H5+$A$2+F5,3))),ROUNDUP(MAX(H5+$A$2+F5,3)+AF5+1.27,1),ROUNDUP(MAX(H5+$A$2+F5,3)+AH5+1.27,1)))
#pragma region
				temp1 = theTmp.W + 1.27 * 2 + max(theTmp.Y + theTmp.Margin + theTmp.W, 3);
				temp2 = theTmp.W + 1.27 * 3 + max(theTmp.X + theTmp.Margin, 3) + max(theTmp.Y + theTmp.Margin + theTmp.W, 3);

				if (theTmp.p17Bw - theTmp.p32d < temp1) 
				{
					theTmp.c = 0;
				} 
				else if (theTmp.p17Bw - theTmp.p32d < temp2 && theTmp.p17Bw - theTmp.p32d >= temp1) 
				{
					//ROUNDUP(MAX(H5+$A$2+F5,3)+AF5+1.27,1)
					double mid1 = max((theTmp.Y + theTmp.Margin +theTmp.W),3)+theTmp.a+1.27;
					theTmp.c = Data->MyFunction->Basic->roundNumber(mid1,1);
				} 
				else
				{
					//ROUNDUP(MAX(H5+$A$2+F5,3)+AH5+1.27,1)
					double mid1 = max((theTmp.Y + theTmp.Margin +theTmp.W),3) +theTmp.b+1.27;
					theTmp.c = Data->MyFunction->Basic->roundNumber(mid1,1);
				}
#pragma endregion

				//d =IF(I4=0,"",IF(I4>=2.9*2+1.27*3+MAX(G4+$A$2,3)+MAX(H4+$A$2+F4,3),ROUNDUP(W4+2.9,1),""))
#pragma region
				if(theTmp.p5Bw == 0)
					theTmp.d = 0;
				else if(theTmp.p5Bw > 2.9*2+1.27*3+max(theTmp.X + theTmp.Margin, 3.0)+max(theTmp.Y + theTmp.Margin + theTmp.W, 3.0))
				{
					//ROUNDUP(W4+2.9,1)
					theTmp.d = Data->MyFunction->Basic->roundNumber(theTmp.B+2.9,1);
				}
				else
					theTmp.d = 0;
#pragma endregion

				//f ==IF(I5=0,"",ROUNDDOWN(W5+I5-2.9-1.3,1))
#pragma region
				if(theTmp.p5Bw == 0)
					theTmp.f = 0;
				else
					//ROUNDDOWN(W5+I5-2.9-1.3,1)
					theTmp.f = Data->MyFunction->Basic->floorNumber(theTmp.B+theTmp.p5Bw-2.9-1.3,1);
#pragma endregion

				//e_min =IF(I4=0,"",IF(I4>=2.9*2+1.27*3+MAX(G4+$A$2,3)+MAX(H4+$A$2+F4,3),ROUNDUP(AJ4+1.27+3,1),MAX(ROUNDUP(W4+2.9,1),ROUNDUP(R4+H4+$A$2,1))))
#pragma region
				if(theTmp.p5Bw == 0)
					theTmp.e_min = 0;
				else if(theTmp.p5Bw > 2.9*2+1.27*3+max(theTmp.X + theTmp.Margin, 3)+max(theTmp.Y + theTmp.Margin + theTmp.W, 3))
				{
					//ROUNDUP(W4+2.9,1)
					theTmp.e_min = Data->MyFunction->Basic->roundNumber(theTmp.B+2.9,1);
				}
				else
					//ROUNDUP(R4+H4+$A$2,1)
					theTmp.e_min = Data->MyFunction->Basic->roundNumber(theTmp.Hmin+theTmp.Y+ theTmp.Margin,1);
#pragma endregion

				//e =IF(I4=0,"",IF(I4>=2.9*2+1.27*3+MAX(G4+$A$2,3)+MAX(H4+$A$2+F4,3),ROUND((Y3+I4+W4-AE4)/2+H4+$A$2,1),MAX(ROUNDUP(W4+2.9,1),ROUNDUP(R4+H4+$A$2,1))))
#pragma region
				if(theTmp.p5Bw == 0)
					theTmp.e = 0;
				else if(theTmp.p5Bw > 2.9*2+1.27*3+max(theTmp.X + theTmp.Margin, 3)+max(theTmp.Y + theTmp.Margin + theTmp.W, 3))
				{
					if(i==0)
					{
						//ROUND((0+I4+W4-AE4)/2+H4+$A$2,1)
						theTmp.e = Data->MyFunction->Basic->round((theTmp.p5Bw+theTmp.B-theTmp.Z_S)/2.0+theTmp.Y+theTmp.Margin,1);
					}
					else
					{
						bladeData theUpTmp = bladeTmpVec[i-1];
						//ROUND((Y3+I4+W4-AE4)/2+H4+$A$2,1)
						theTmp.e = Data->MyFunction->Basic->round((theUpTmp.F+theTmp.p5Bw+theTmp.B-theTmp.Z_S)/2.0+theTmp.Y+theTmp.Margin,1);
					}
				}
				else
				{
					//MAX(ROUNDUP(W4+2.9,1),ROUNDUP(R4+H4+$A$2,1))))
					double mid1 = Data->MyFunction->Basic->roundNumber(theTmp.B+2.9,1);
					double mid2 = Data->MyFunction->Basic->roundNumber(theTmp.Hmin+theTmp.Y+theTmp.Margin,1);
					theTmp.e = max(mid1,mid2);
				}

#pragma endregion

				//e_max =IF(I4=0,"",IF(I4>=2.9*2+1.27*3+MAX(G4+$A$2,3)+MAX(H4+$A$2+F4,3),ROUNDDOWN(AN4-3-1.27,1),MAX(ROUNDUP(W4+2.9,1),ROUNDUP(R4+H4+$A$2,1))))
#pragma region
				if(theTmp.p5Bw == 0)
					theTmp.e_max = 0;
				else if(theTmp.p5Bw >= 2.9*2+1.27*3+max(theTmp.X + theTmp.Margin, 3)+max(theTmp.Y + theTmp.Margin + theTmp.W, 3))
				{
					//ROUNDDOWN(AN4-3-1.27,1)
					theTmp.e_max = Data->MyFunction->Basic->floorNumber(theTmp.f-1.27,1);
				}
				else
				{
					//MAX(ROUNDUP(W4+2.9,1),ROUNDUP(R4+H4+$A$2,1))))
					double mid1 = Data->MyFunction->Basic->roundNumber(theTmp.B+2.9,1);
					double mid2 = Data->MyFunction->Basic->roundNumber(theTmp.Hmin+theTmp.Y+theTmp.Margin,1);
					theTmp.e_max = max(mid1,mid2);
				}
#pragma endregion

				//H =IF(I4=0,"",IF(AK4=AM4,ROUNDDOWN(AK4-H4-$A$2,1),ROUNDDOWN(AL4-H4-$A$2,1)))
#pragma region
				if(theTmp.p5Bw == 0)
					theTmp.H = 0;
				else if(theTmp.e_min==theTmp.e_max)
				{
					//ROUNDDOWN(AK4-H4-$A$2,1)
					theTmp.H = Data->MyFunction->Basic->floorNumber(theTmp.e_min-theTmp.Y-theTmp.Margin,1);
				}
				else
				{
					//ROUNDDOWN(AL4-H4-$A$2,1)
					theTmp.H = Data->MyFunction->Basic->floorNumber(theTmp.e-theTmp.Y-theTmp.Margin,1);
				}
#pragma endregion

				//J =IF(I4=0,"",IF(AK4=AM4,ROUNDUP(Y3+I4+W4-AK4+H4+$A$2-AE4,1),ROUNDUP(Y3+I4+W4-AL4+H4+$A$2-AE4,1)))
#pragma region
				if(theTmp.p5Bw == 0)
					theTmp.J = 0;
				else if(theTmp.e_min==theTmp.e_max)
				{
					if(i==0)
					{
						//ROUNDUP(0+I4+W4-AK4+H4+$A$2-AE4,1)
						theTmp.J = Data->MyFunction->Basic->roundNumber(theTmp.p5Bw+theTmp.B-theTmp.e_min+theTmp.Y+theTmp.Margin-theTmp.Z_S,1);
					}
					else
					{
						bladeData theUpTmp = bladeTmpVec[i-1];
						//ROUNDUP(Y3+I4+W4-AK4+H4+$A$2-AE4,1)
						theTmp.J = Data->MyFunction->Basic->roundNumber(theUpTmp.F+theTmp.p5Bw+theTmp.B-theTmp.e_min+theTmp.Y+theTmp.Margin-theTmp.Z_S,1);
					}
				}
				else
				{
					if(i==0)
					{
						//ROUNDUP(0+I4+W4-AL4+H4+$A$2-AE4,1)
						theTmp.J = Data->MyFunction->Basic->roundNumber(theTmp.p5Bw+theTmp.B-theTmp.e+theTmp.Y+theTmp.Margin-theTmp.Z_S,1);
					}
					else
					{
						bladeData theUpTmp = bladeTmpVec[i-1];
						//ROUNDUP(Y3+I4+W4-AL4+H4+$A$2-AE4,1)
						theTmp.J = Data->MyFunction->Basic->roundNumber(theUpTmp.F+theTmp.p5Bw+theTmp.B-theTmp.e+theTmp.Y+theTmp.Margin-theTmp.Z_S,1);
					}
				}
#pragma endregion

				//t_type_s静叶 // p5Bw≥2.9*2+1.27*3+max[(X+1.1),3]+max[(Y+1.1+W),3]且H≥Hmin且J≥Jmin（当本级为本气道的第一级时，无J≥Jmin要求）
#pragma region
				if(theTmp.p5Bw >= 2.9*2+1.27*3+max(theTmp.X + theTmp.Margin, 3)+max(theTmp.Y + theTmp.Margin + theTmp.W, 3) && theTmp.H >= theTmp.Hmin)
				{
					//如果是第一个叶片 
					if(i==0)
					{
						//一短一长一短
						theTmp.t_type_s = 1;
					}
					else if(theTmp.J >= theTmp.Jmin)
					{
						//一短一长一短
						theTmp.t_type_s = 1;
					}
					else
					{
						//一短一长
						theTmp.t_type_s = 2;
					}
					
				}
				else
				{
					//一短一长
					theTmp.t_type_s = 2;
				}
#pragma endregion


				//γ H≥Hmin->0 -->γ=Hmin-H
#pragma region
				if(theTmp.H >= theTmp.Hmin)
					theTmp.γ = 0;
				else
					theTmp.γ = theTmp.Hmin-theTmp.H;
#pragma endregion

				//λ J≥Jmin->0 -->λ=Jmin-J
#pragma region
				if(theTmp.J >= theTmp.Jmin)
					theTmp.λ = 0;
				else
					theTmp.λ = theTmp.Jmin-theTmp.J;
#pragma endregion

				//p0Jz λ
#pragma region
				if(i!=0)
					theTmp.p0Jz = theTmp.p0Jz + theTmp.λ;
#pragma endregion

				//p30Dz γ
#pragma region
				theTmp.p30Dz = theTmp.p30Dz + theTmp.γ;
#pragma endregion
				bladeTmpVec[i] = theTmp;
			}

			theAir.bladePair = bladeTmpVec;
			aircylinderDataVec.push_back(theAir);
		}
	}


	return true;

}


//预读数据处理抽空数据
bool ClassTongliu::LoadXMLDataDispose()
{
	//循环气缸
	for(int i = 0;i<aircylinderDataVec.size();i++)
	{
		//增量
		double increment = 0.0;
		//获取叶片数据
		vector<bladeData> bladePair = aircylinderDataVec[i].bladePair;
		for(int ii=0;ii<bladePair.size();ii++)
		{
			//先判断是否有增量 //第一个叶片不会有增量 所以不用跳过
			if(increment != 0.0)
			{
				//如果折流归零
				if(bladePair[ii].BrokenTag==1.0)
				{
					increment = 0.0;
				}
				else
				{
					bladePair[ii].p0Jz = bladePair[ii].p0Jz+increment;
					bladePair[ii].p30Dz = bladePair[ii].p30Dz+increment;
					bladePair[ii].S_deep_point.x = bladePair[ii].S_deep_point.x+increment;
					bladePair[ii].deep_point.x = bladePair[ii].deep_point.x+increment;
					bladePair[ii].rator_deeppoint.x = bladePair[ii].rator_deeppoint.x+increment;
				}

			}

			//判断叶片对后是否有抽口
			if(bladePair[ii].IsSuctionPort == 1.0)
			{
				double left_dis1 = bladePair[ii].OutletWeight;
				double left_dis2 = 0.0;
				if(aircylinderDataVec[i].isHorizonBlade != 1.0 || ii != 0)
				{
					left_dis2 = bladePair[ii].p30Dz - bladePair[ii].p0Jz + bladePair[ii].p8Bz - bladePair[ii].p9bz + bladePair[ii].W + 4;
				}
				
				left_dis2 =  Data->MyFunction->Basic->ceilToSignificance(left_dis2, 0.5);
				double left_re_dis = max(left_dis1, left_dis2);
				double right_re_dis = bladePair[ii].InletWeight;

				//a. 首先情况3符合->使用给定Y值（需要修改XML格式，接收Y值）
				//b. 否则从1或2判断
				double mid_re_dis = 0.0;
				if(isMidWeightY(bladePair,ii))
				{
					mid_re_dis = bladePair[ii].MidY;
				}
				else
				{
					double mid_dis1 = 0.0;
					double mid_dis_x = 0.0;
					if(aircylinderDataVec[i].isHorizonBlade != 1.0 || ii != 0)
					{
						mid_dis1 = bladePair[ii+1].p0Jz - bladePair[ii].p0Jz - bladePair[ii].p9bz - bladePair[ii+1].p8Bz+ bladePair[ii+1].p9bz - left_re_dis - right_re_dis+increment;
						AcGePoint3d theP = bladePair[ii+1].S_deep_point;
						mid_dis_x = bladePair[ii].V * bladePair[ii].G *pow(10.0, 9.0)*1000/3600 / (bladePair[ii].μ*1000*PI *theP.y);
					}
					mid_re_dis = max(mid_dis1,mid_dis_x);

				}

				bladePair[ii].InletWeight = right_re_dis;
				bladePair[ii].OutletWeight = left_re_dis;
				bladePair[ii].MidWeight = mid_re_dis;
				double tmp1 = left_re_dis + right_re_dis + mid_re_dis - (bladePair[ii+1].p0Jz - bladePair[ii].p0Jz+increment);
				increment =  increment + tmp1;
				//保护
				if(increment<0)
					increment = 0.0;
				bladePair[ii].rator_deeppoint.x = bladePair[ii].rator_deeppoint.x+tmp1;
			}
		}

		aircylinderDataVec[i].bladePair = bladePair;
	}
	return true;
}



//判断中间轴向宽度Y
bool ClassTongliu::isMidWeightY(vector<bladeData> bladePair,int theNum)
{
	bool hasOneBefore = false;
	bool hasOneAfter = false;

	// 首先确保i的范围在合理的范围内
	if(theNum < 0 || theNum >= bladePair.size()) {
		return false;
	}

	// 判断第i之前的元素是否有抽口
	for (int j = 0; j < theNum; ++j)
	{
		if (bladePair[j].IsSuctionPort == 1.0) 
		{
			//判断这三个抽汽口间的两段内级数是否均小于等于2级
			if(theNum - j <=2)
			{
				hasOneAfter = true;
				break;
			}
			else
				return false;
		}
	}
	// 判断第i之后的元素是否有1
	for (int j = theNum + 1; j < bladePair.size(); ++j) 
	{
		if(bladePair[j].IsSuctionPort == 1.0) 
		{
			//判断这三个抽汽口间的两段内级数是否均小于等于2级
			if(theNum - j <=2)
			{
				hasOneBefore = true;
				break;
			}
			else
				return false;
		}
	}

    return hasOneBefore && hasOneAfter;
}

//************图纸创建

//创建通流图
int ClassTongliu::createTL()
{	
	//循环气缸
	for(int ii = 0;ii<aircylinderDataVec.size();ii++)
	{
		//获取末三级数据
		string last_three_stages = aircylinderDataVec[ii].LastThreeStages;
		//获取叶片数据
		vector<bladeData> bladePair = aircylinderDataVec[ii].bladePair;

		//气道中心线轴向距离
		double AirAxialDis = aircylinderDataVec[ii].AirAxialDis;
		//汽道中心宽度
		double AirWeight = aircylinderDataVec[ii].AirWeight;
		//悬臂宽度
		double CantileverWidth = aircylinderDataVec[ii].CantileverWidth;
		
		//标注isdelete 单流删除 ， 双流不删
		//判断气道类型 
		int isdelete = 0;
		if(aircylinderDataVec[ii].AirPassage=="单流" || aircylinderDataVec[ii].AirPassage=="折流")
			isdelete = 1;

		//循环叶片数据
#pragma region
		for(int i=0;i<bladePair.size();i++)
		{
			//镜像
			AcDbObjectIdArray AircylinderObjects;
			AcDbObjectId GeneralObjects;
			vector<AcGePoint3d>s_vertex_out_vec,vertex_out_vec;

			bladeData tmp  = bladePair[i];
			tmp.p19 = tmp.p27Lp/2.0;
			tmp.p32 = tmp.p29Ld/2.0;
			tmp.p3 = 0.3;
			tmp.p4 = tmp.p3;
			tmp.p5 = tmp.p3;
			tmp.p6 = tmp.p3;
			tmp.p18 = tmp.p9bz;
			tmp.p17 = tmp.p9bz;
			tmp.p21 = 35.0;

			tmp.p23 = tmp.p3;
			tmp.p24 = tmp.p3;
			tmp.p25 = tmp.p3;
			tmp.p26 = tmp.p3;
			tmp.p34 = tmp.p22bz;
			tmp.p35 = tmp.p22bz;
			tmp.p27 = tmp.p21;
			
			//如果是第一个叶片 且是横置叶片则画横置叶片
			double angle_up,angle_down;
			AcGePoint3d GroovePoint(0,0,0);
			bladeData after_data;
			AcGePoint3d up_point1,up_point2,left_point,right_point, point1, point2, point3,vertex1,vertex2,vertex3,vertex4,zhangchai_point,l_p,r_p;
			AcGePoint3d ids_up1=AcGePoint3d(0,0,0);
			AcGePoint3d ids_up2 =AcGePoint3d(0,0,0);
			AcGePoint3d ids_down1=AcGePoint3d(0,0,0);
			AcGePoint3d ids_down2=AcGePoint3d(0,0,0); 
			if(i == 0 && aircylinderDataVec[ii].isHorizonBlade == 1.0)
			{
				//先画气道
				l_p = AcGePoint3d(setting.OriginalPoint.x +AirAxialDis - AirWeight/2.0 ,setting.OriginalPoint.y + tmp.p28Dpd + tmp.p29Ld +50.0,0);
				r_p = AcGePoint3d(setting.OriginalPoint.x +AirAxialDis + AirWeight/2.0 ,setting.OriginalPoint.y + tmp.p28Dpd + tmp.p29Ld +50.0,0);
			}
			else
			{
				//静叶
				//创建顶点 叶片四边形轮廓 
				vertex1   = AcGePoint3d(tmp.p0Jz + tmp.p4bd -tmp.p2BD, tmp.p26Dpp + tmp.p19, 0);
				vertex2   = AcGePoint3d(tmp.p0Jz + tmp.p3bg -tmp.p1BG, tmp.p26Dpp - tmp.p19, 0);
				vertex3   = AcGePoint3d(tmp.p0Jz + tmp.p3bg , tmp.p26Dpp - tmp.p19, 0);
				vertex4   = AcGePoint3d(tmp.p0Jz + tmp.p4bd , tmp.p26Dpp + tmp.p19, 0);
				//外围轮廓四点 顺序逆时针
				AcGePoint3d vertex_01 = AcGePoint3d(tmp.p0Jz + tmp.p9bz -tmp.p8Bz, vertex1.y+tmp.p10hz, 0);
				AcGePoint3d vertex_02 = AcGePoint3d(tmp.p0Jz + tmp.p6bw -tmp.p5Bw, vertex3.y-tmp.p7hw, 0);
				AcGePoint3d vertex_03 = AcGePoint3d(tmp.p0Jz + tmp.p6bw , vertex3.y-tmp.p7hw , 0);
				AcGePoint3d vertex_04 = AcGePoint3d(tmp.p0Jz + tmp.p9bz , vertex1.y+tmp.p10hz, 0);
				s_vertex_out_vec.push_back(vertex_01);
				s_vertex_out_vec.push_back(vertex_02);
				s_vertex_out_vec.push_back(vertex_03);
				s_vertex_out_vec.push_back(vertex_04);
				//角度
				angle_up = tmp.p11ad * PI /180.0;
				angle_down = tmp.p12ag * PI /180.0;

				//求叶片交点
				AcDbObjectId tag_ray_Id, tag_upl_ray_Id,tag_upr_ray_Id,tag_downr_ray_Id,tag_downl_ray_Id,ray_upl_ID,ray_upr_ID,ray_downl_ID,ray_downr_ID;
				Data->MyFunction->Arx->CreateRay(vertex_01,AcGeVector3d(0,-1,0), &tag_upl_ray_Id);
				Data->MyFunction->Arx->CreateRay(vertex_04,AcGeVector3d(0,-1,0), &tag_upr_ray_Id);
				Data->MyFunction->Arx->CreateRay(vertex_02,AcGeVector3d(0,1,0) , &tag_downl_ray_Id);
				Data->MyFunction->Arx->CreateRay(vertex_03,AcGeVector3d(0,1,0) , &tag_downr_ray_Id);

				AcGePoint3d ids_up,ids_down;
				AcGeVector3d tag_vec =  AcGeVector3d(vertex2.x - vertex1.x, vertex2.y - vertex1.y, 0);//vertex2 - vertex1
				Data->MyFunction->Arx->CreateRay(vertex1,tag_vec, &tag_ray_Id);

				Data->MyFunction->Arx->CreateLine(vertex4,angle_up,  1000.0,0, &ray_upl_ID);
				Data->MyFunction->Arx->CreateLine(vertex4,angle_up,  1000.0,1, &ray_upr_ID);
				Data->MyFunction->Arx->CreateLine(vertex3,angle_down,1000.0,0, &ray_downl_ID);
				Data->MyFunction->Arx->CreateLine(vertex3,angle_down,1000.0,1, &ray_downr_ID);

				//叶片两点
				ids_up=AcGePoint3d(0,0,0);
				ids_down=AcGePoint3d(0,0,0) ;
				Data->MyFunction->Arx->GetObjectIntersect(tag_ray_Id,ray_upl_ID,&ids_up);
				Data->MyFunction->Arx->GetObjectIntersect(tag_ray_Id,ray_downl_ID,&ids_down);

				//叶根两点
				Data->MyFunction->Arx->GetObjectIntersect(tag_upl_ray_Id,ray_upl_ID,&ids_up1);
				Data->MyFunction->Arx->GetObjectIntersect(tag_upr_ray_Id,ray_upr_ID,&ids_up2);
				s_vertex_out_vec.push_back(ids_up1);
				s_vertex_out_vec.push_back(ids_up2);
				//围带两点
				Data->MyFunction->Arx->GetObjectIntersect(tag_downl_ray_Id,ray_downl_ID,&ids_down1);
				Data->MyFunction->Arx->GetObjectIntersect(tag_downr_ray_Id,ray_downr_ID,&ids_down2);

				//删除辅助线
				Data->MyFunction->Arx->DeleteObject(tag_ray_Id);
				Data->MyFunction->Arx->DeleteObject(tag_upl_ray_Id);
				Data->MyFunction->Arx->DeleteObject(tag_upr_ray_Id);
				Data->MyFunction->Arx->DeleteObject(tag_downl_ray_Id);
				Data->MyFunction->Arx->DeleteObject(tag_downr_ray_Id);
				Data->MyFunction->Arx->DeleteObject(ray_upl_ID);
				Data->MyFunction->Arx->DeleteObject(ray_upr_ID);
				Data->MyFunction->Arx->DeleteObject(ray_downl_ID);
				Data->MyFunction->Arx->DeleteObject(ray_downr_ID);


				//勾画叶片 逆时针
				Data->MyFunction->Arx->SetNowLayerName(L"2细实线");
				Data->MyFunction->Arx->CreateLine(vertex4,vertex1,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(vertex2,vertex3,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->SetNowLayerName(L"1粗实线");

				Data->MyFunction->Arx->CreateLine(vertex1,ids_down,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(vertex3,vertex4,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);


				//围带
				AcDbObjectId yegen_down_line_id,weidai_up_line_id;
				Data->MyFunction->Arx->CreateLine(ids_down1,vertex_02,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(vertex_02,vertex_03,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(vertex_03,ids_down2,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(ids_down2,ids_down1,&weidai_up_line_id);
				AircylinderObjects.append(weidai_up_line_id);

				//若无横置叶片并且是第一级叶片则勾画气道
				if(i == 0 && aircylinderDataVec[ii].isHorizonBlade == 0.0)
				{
					l_p = AcGePoint3d(setting.OriginalPoint.x +AirAxialDis - AirWeight/2.0 ,setting.OriginalPoint.y + tmp.p26Dpp + tmp.p27Lp+50.0 ,0);
					r_p = AcGePoint3d(setting.OriginalPoint.x +AirAxialDis + AirWeight/2.0 ,setting.OriginalPoint.y + tmp.p26Dpp + tmp.p27Lp+50.0 ,0);
				
					//上部链接线
					AcDbObjectId tmp1,tmp2,tmp3;
					AcGePoint3d StartPoint,EndPoint;
					Data->MyFunction->Arx->CreateLine(r_p,AcGePoint3d(r_p.x,ids_up1.y,0) ,&tmp1);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_p.x,ids_up1.y,0) ,AcGePoint3d(ids_up1.x-0.2,ids_up1.y,0) ,&tmp2);
					Data->MyFunction->Arx->CreateFillet(tmp1, r_p , tmp2, AcGePoint3d(ids_up1.x-0.2,ids_up1.y,0), 5 , &tmp3);
					AircylinderObjects.append(tmp1);
					AircylinderObjects.append(tmp2);
					AircylinderObjects.append(tmp3);

					Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_up1.x-0.2,ids_up1.y,0) ,  AcGePoint3d(ids_up1.x-0.2,tmp.S_deep_point.y,0)  ,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine( AcGePoint3d(ids_up1.x-0.2,tmp.S_deep_point.y,0),tmp.S_deep_point,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);

					double u_num = 0.0;
					//静叶用不用的间隙计算余量与读取xml相反
					if(tmp.N == 0)
						u_num = tmp.NegativeSpace ;
					else
						u_num = tmp.ForwardSpace ;
					
					//判断气道类型 
					if(aircylinderDataVec[ii].AirPassage=="单流"||aircylinderDataVec[ii].AirPassage=="折流")
					{
						//单流正常画线最后判断镜像
						if(tmp.HubType==1.0||tmp.HubType==4.0)
						{
							//下部链接线
							AcGePoint3d d_p = tmp.deep_point;
							Data->MyFunction->Arx->CreateLine(l_p,AcGePoint3d(l_p.x,ids_down1.y,0) ,&tmp1);
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(l_p.x,ids_down1.y,0) ,AcGePoint3d(ids_down1.x-u_num,ids_down1.y,0) ,&tmp2);
							Data->MyFunction->Arx->CreateFillet(tmp1, l_p , tmp2,AcGePoint3d(ids_down1.x-u_num,ids_down1.y,0), AirWeight , &tmp3);
							AircylinderObjects.append(tmp1);
							AircylinderObjects.append(tmp3);

							Data->MyFunction->Arx->GetObjectStartPoint(tmp2, StartPoint);
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_down1.x-u_num,ids_down1.y,0), AcGePoint3d(ids_down1.x-u_num,d_p.y,0) ,&tmp1);
							Data->MyFunction->Arx->CreateFillet(tmp2, StartPoint , tmp1,AcGePoint3d(ids_down1.x-u_num,d_p.y,0), 3 , &tmp3);
							AircylinderObjects.append(tmp2);
							AircylinderObjects.append(tmp3);

							Data->MyFunction->Arx->GetObjectStartPoint(tmp1, StartPoint);
							Data->MyFunction->Arx->CreateLine( AcGePoint3d(ids_down1.x-u_num,d_p.y,0) ,d_p ,&tmp2);
							Data->MyFunction->Arx->CreateFillet(tmp1, StartPoint , tmp2,d_p, 4 , &tmp3);
							AircylinderObjects.append(tmp1);
							AircylinderObjects.append(tmp2);
							AircylinderObjects.append(tmp3);
						}
						else if(tmp.HubType==2.0||tmp.HubType==3.0)
						{
							AcGePoint3d d_p = AcGePoint3d(tmp.p30Dz -tmp.p21Bz + tmp.p22bz,tmp.p28Dpd - tmp.p29Ld/2.0 - tmp.p23hz,0);
							Data->MyFunction->Arx->CreateLine(l_p,AcGePoint3d(l_p.x,ids_down1.y,0) ,&tmp1);
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(l_p.x,ids_down1.y,0) ,AcGePoint3d(ids_down1.x-u_num,ids_down1.y,0) ,&tmp2);
							Data->MyFunction->Arx->CreateFillet(tmp1, l_p , tmp2,AcGePoint3d(ids_down1.x-u_num,ids_down1.y,0), AirWeight , &tmp3);
							AircylinderObjects.append(tmp1);
							AircylinderObjects.append(tmp3);

							Data->MyFunction->Arx->GetObjectStartPoint(tmp2, StartPoint);
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_down1.x-u_num,ids_down1.y,0), AcGePoint3d(ids_down1.x-u_num,d_p.y,0) ,&tmp1);
							Data->MyFunction->Arx->CreateFillet(tmp2, StartPoint , tmp1,AcGePoint3d(ids_down1.x-u_num,d_p.y,0), 3 , &tmp3);
							AircylinderObjects.append(tmp2);
							AircylinderObjects.append(tmp3);

							Data->MyFunction->Arx->GetObjectStartPoint(tmp1, StartPoint);
							Data->MyFunction->Arx->CreateLine( AcGePoint3d(ids_down1.x-u_num,d_p.y,0) ,d_p ,&tmp2);
							Data->MyFunction->Arx->CreateFillet(tmp1, StartPoint , tmp2,d_p, 4 , &tmp3);
							AircylinderObjects.append(tmp1);
							AircylinderObjects.append(tmp2);
							AircylinderObjects.append(tmp3);
						}
					}
					else if(aircylinderDataVec[ii].AirPassage=="双分流")
					{
						//双分流下部链接线起始点在中心线上，最后要保留内容镜像
						if(tmp.HubType==1.0||tmp.HubType==4.0)
						{
							//下部链接线
							AcGePoint3d d_p = tmp.deep_point;
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(setting.OriginalPoint.x +AirAxialDis,ids_down1.y,0) ,AcGePoint3d(ids_down1.x-u_num,ids_down1.y,0) ,&tmp2);

							Data->MyFunction->Arx->GetObjectStartPoint(tmp2, StartPoint);
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_down1.x-u_num,ids_down1.y,0), AcGePoint3d(ids_down1.x-u_num,d_p.y,0) ,&tmp1);
							Data->MyFunction->Arx->CreateFillet(tmp2, StartPoint , tmp1,AcGePoint3d(ids_down1.x-u_num,d_p.y,0), 3 , &tmp3);
							AircylinderObjects.append(tmp2);
							AircylinderObjects.append(tmp3);

							Data->MyFunction->Arx->GetObjectStartPoint(tmp1, StartPoint);
							Data->MyFunction->Arx->CreateLine( AcGePoint3d(ids_down1.x-u_num,d_p.y,0) ,d_p ,&tmp2);
							Data->MyFunction->Arx->CreateFillet(tmp1, StartPoint , tmp2,d_p, 4 , &tmp3);
							AircylinderObjects.append(tmp1);
							AircylinderObjects.append(tmp2);
							AircylinderObjects.append(tmp3);
						}
						else if(tmp.HubType==2.0||tmp.HubType==3.0)
						{
							//下部链接线
							AcGePoint3d d_p = AcGePoint3d(tmp.p30Dz -tmp.p21Bz + tmp.p22bz,tmp.p28Dpd - tmp.p29Ld/2.0 - tmp.p23hz,0);
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(setting.OriginalPoint.x +AirAxialDis,ids_down1.y,0) ,AcGePoint3d(ids_down1.x-u_num,ids_down1.y,0) ,&tmp2);

							Data->MyFunction->Arx->GetObjectStartPoint(tmp2, StartPoint);
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_down1.x-u_num,ids_down1.y,0), AcGePoint3d(ids_down1.x-u_num,d_p.y,0) ,&tmp1);
							Data->MyFunction->Arx->CreateFillet(tmp2, StartPoint , tmp1,AcGePoint3d(ids_down1.x-u_num,d_p.y,0), 3 , &tmp3);
							AircylinderObjects.append(tmp2);
							AircylinderObjects.append(tmp3);

							Data->MyFunction->Arx->GetObjectStartPoint(tmp1, StartPoint);
							Data->MyFunction->Arx->CreateLine( AcGePoint3d(ids_down1.x-u_num,d_p.y,0) ,d_p ,&tmp2);
							Data->MyFunction->Arx->CreateFillet(tmp1, StartPoint , tmp2,d_p, 4 , &tmp3);
							AircylinderObjects.append(tmp1);
							AircylinderObjects.append(tmp2);
							AircylinderObjects.append(tmp3);
						}

					}
					
					//气道中心线
					Data->MyFunction->Arx->SetNowLayerName(L"细点划线");
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_p.x - AirWeight/2.0 ,ids_up1.y +80,0) ,AcGePoint3d(r_p.x - AirWeight/2.0 ,ids_down1.y -80,0) ,&tmp2);
					AircylinderObjects.append(tmp3);
					Data->MyFunction->Arx->SetNowLayerName(L"1粗实线");
				}

				//创建叶根
				createHub("静叶", 1, tmp, s_vertex_out_vec, AircylinderObjects);

				//创建叶根槽
				if(i!=bladePair.size()-1)
				{
					after_data = bladePair[i+1];
					GroovePoint = after_data.S_deep_point;
				}
				/*叶片类型，结构体，中间体左基准点，中间体有基准点,后一叶根槽基准深度点（注：最后一对没有深度点）*/
				createBladeHubGroove("静叶", 1, tmp,after_data,vertex_01,vertex_04,AircylinderObjects,GroovePoint);

				//汽封齿排布 判断汽封齿类型 静叶 因为需要下一个动叶的deeppoint
				AcDbObjectId ObjectId;
				if(tmp.t_type_s == 1)//一短一长一短
				{
					Data->MyFunction->Arx->InsertBlock("S_L_short",AcGePoint3d(tmp.deep_point.x-tmp.f-0.65,vertex_02.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("S_L_long",AcGePoint3d(tmp.deep_point.x-tmp.e-0.65,vertex_02.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("S_L_short",AcGePoint3d(tmp.deep_point.x-tmp.d-0.65,vertex_02.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
				}
				else if(tmp.t_type_s == 2)// 一长一短
				{
					Data->MyFunction->Arx->InsertBlock("S_L_long",AcGePoint3d(tmp.deep_point.x-tmp.e-0.65,vertex_02.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("S_L_short",AcGePoint3d(tmp.deep_point.x-tmp.f-0.65,vertex_02.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
				}

				//静叶中心线 下-》上 左-》右
				Data->MyFunction->Arx->SetNowLayerName(L"细点划线");
				up_point1 = AcGePoint3d(tmp.p0Jz,tmp.p26Dpp-tmp.p19-tmp.p7hw,0);
				up_point2 = AcGePoint3d(tmp.p0Jz,tmp.p26Dpp+tmp.p19+tmp.p10hz+tmp.p21,0);
				left_point = AcGePoint3d(tmp.p0Jz-tmp.p17,tmp.p26Dpp,0);
				right_point = AcGePoint3d(tmp.p0Jz+tmp.p18,tmp.p26Dpp,0);
				AcDbObjectId up_line_id;
				Data->MyFunction->Arx->CreateLine(up_point1,AcGePoint3d(up_point2.x,vertex_01.y+tmp.SGrooveNeckH+tmp.SGrooveBottomH+10.0,0),&up_line_id);
				AircylinderObjects.append(up_line_id);
				Data->MyFunction->Arx->CreateLine(left_point,right_point,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);


				//标注////////左右中  下上中
				//静叶根高度
				double p96 = 0.0;
				if(tmp.p31zf ==1)
					p96 =3.4;
				else
					p96 = 7.8;

				point1 = AcGePoint3d(tmp.p0Jz,vertex1.y,0);
				point2 = AcGePoint3d(tmp.p0Jz,vertex_01.y,0);
				point3 = AcGePoint3d(point1.x - p96,vertex1.y+tmp.p10hz/2,0);
				Data->MyFunction->Arx->SetNowLayerName(L"5尺寸线");
				Data->MyFunction->Arx->CreateLinearDim(point1,point2,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p10hz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

				//静叶上半宽度
				Data->MyFunction->Arx->CreateLine(ids_up1,ids_up2,&yegen_down_line_id);
				Data->MyFunction->Arx->GetObjectIntersect(yegen_down_line_id,up_line_id,&point1);
				Data->MyFunction->Arx->DeleteObject(yegen_down_line_id);
				point3 = AcGePoint3d(point1.x +tmp.p4bd/2,point1.y-2.6,0);
				Data->MyFunction->Arx->CreateLinearDim(point1,vertex4,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p4bd),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//静叶上宽度
				point3 = AcGePoint3d(vertex4.x-tmp.p2BD/2,point3.y-5.0,0);
				Data->MyFunction->Arx->CreateLinearDim(vertex1,vertex4,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p2BD),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//静叶高
				point3 = AcGePoint3d(ids_down1.x - p96,left_point.y+tmp.p27Lp/4,0);
				Data->MyFunction->Arx->CreateLinearDim(vertex1,vertex2,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p27Lp),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//静叶下宽度
				point3 = AcGePoint3d(vertex3.x-tmp.p1BG/2,vertex3.y+8.4,0);
				Data->MyFunction->Arx->CreateLinearDim(vertex2,vertex3,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p1BG),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//静叶下半宽度
				point1 = AcGePoint3d(tmp.p0Jz,vertex3.y,0);
				point3 = AcGePoint3d(tmp.p0Jz +tmp.p3bg/2,vertex3.y+3.4,0);
				Data->MyFunction->Arx->CreateLinearDim(point1,vertex3,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p4bd),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

				//静叶根半宽度
				point1 = AcGePoint3d(tmp.p0Jz,ids_up2.y,0);
				point3 = AcGePoint3d(tmp.p0Jz +tmp.p9bz/2,vertex4.y+3.4,0);
				Data->MyFunction->Arx->CreateLinearDim(point1,ids_up2,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p9bz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//静叶根宽度
				point1 = AcGePoint3d(ids_up1.x ,ids_up2.y,0);
				point3 = AcGePoint3d(tmp.p0Jz +tmp.p9bz/2,ids_up2.y+8.4,0);
				Data->MyFunction->Arx->CreateLinearDim(point1,ids_up2,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p8Bz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//静叶围带半宽度
				Data->MyFunction->Arx->GetObjectIntersect(weidai_up_line_id,up_line_id,&point1);
				zhangchai_point = point1;
				point3 = AcGePoint3d(tmp.p0Jz +tmp.p6bw/2,point1.y-2.6,0);
				Data->MyFunction->Arx->CreateLinearDim(point1,ids_down2,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p6bw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//静叶围带宽度
				double p100 = 0.0;
				if(tmp.p27Lp >30.4)
					p100 =13.4;
				else
					p100 = -32-tmp.p7hw;
				point3 = AcGePoint3d(ids_down2.x-tmp.p5Bw/2,ids_down2.y+p100,0);
				Data->MyFunction->Arx->CreateLinearDim(ids_down1,ids_down2,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p5Bw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//静叶围带高度
				point3 = AcGePoint3d(ids_down1.x-p96,ids_down2.y-tmp.p7hw/2,0);
				Data->MyFunction->Arx->CreateLinearDim(vertex_02,vertex3,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p7hw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);


			}
			
			//动叶
			//创建顶点 叶片四边形轮廓 顺序逆时针
			AcGePoint3d vertex5 = AcGePoint3d(tmp.p30Dz + tmp.p16bd -tmp.p14BD, tmp.p28Dpd + tmp.p32, 0);
			AcGePoint3d vertex6 = AcGePoint3d(tmp.p30Dz + tmp.p15bg -tmp.p13BG, tmp.p28Dpd - tmp.p32, 0);
			AcGePoint3d vertex7 = AcGePoint3d(tmp.p30Dz + tmp.p15bg ,tmp.p28Dpd - tmp.p32, 0);
			AcGePoint3d vertex8 = AcGePoint3d(tmp.p30Dz + tmp.p16bd, tmp.p28Dpd + tmp.p32, 0);
			//外围轮廓四点 顺序逆时针
			AcGePoint3d vertex_05 = AcGePoint3d(tmp.p30Dz + tmp.p18bw -tmp.p17Bw, vertex5.y+tmp.p19hw, 0);
			AcGePoint3d vertex_06 = AcGePoint3d(tmp.p30Dz + tmp.p22bz -tmp.p21Bz, vertex7.y-tmp.p23hz, 0);
			AcGePoint3d vertex_07 = AcGePoint3d(tmp.p30Dz + tmp.p22bz , vertex7.y-tmp.p23hz , 0);
			AcGePoint3d vertex_08 = AcGePoint3d(tmp.p30Dz + tmp.p18bw , vertex5.y+tmp.p19hw, 0);
			vertex_out_vec.push_back(vertex_05);
			vertex_out_vec.push_back(vertex_06);
			vertex_out_vec.push_back(vertex_07);
			vertex_out_vec.push_back(vertex_08);
			
			//求叶片交点
			AcDbObjectId tag_ray_Id1, tag_upl_ray_Id1,tag_upr_ray_Id1,tag_downr_ray_Id1,tag_downl_ray_Id1,ray_upl_ID1,ray_upr_ID1,ray_downl_ID1,ray_downr_ID1;
			Data->MyFunction->Arx->CreateRay(vertex_05,AcGeVector3d(0,-1,0), &tag_upl_ray_Id1);
			Data->MyFunction->Arx->CreateRay(vertex_08,AcGeVector3d(0,-1,0), &tag_upr_ray_Id1);
			Data->MyFunction->Arx->CreateRay(vertex_06,AcGeVector3d(0,1,0) , &tag_downl_ray_Id1);
			Data->MyFunction->Arx->CreateRay(vertex_07,AcGeVector3d(0,1,0) , &tag_downr_ray_Id1);

			AcGePoint3d ids_up01,ids_down01;
			AcGeVector3d tag_vec1 =  AcGeVector3d(vertex6.x - vertex5.x, vertex6.y - vertex5.y, 0);//vertex6 - vertex5
			Data->MyFunction->Arx->CreateRay(vertex5,tag_vec1, &tag_ray_Id1);
			
			//角度
			angle_up = tmp.p24ad * PI /180.0;
			angle_down = tmp.p25ag * PI /180.0;
			Data->MyFunction->Arx->CreateLine(vertex8,angle_up,  1000.0,0, &ray_upl_ID1);
			Data->MyFunction->Arx->CreateLine(vertex8,angle_up,  1000.0,1, &ray_upr_ID1);
			Data->MyFunction->Arx->CreateLine(vertex7,angle_down,1000.0,0, &ray_downl_ID1);
			Data->MyFunction->Arx->CreateLine(vertex7,angle_down,1000.0,1, &ray_downr_ID1);

			//叶片两点
			ids_up01=AcGePoint3d(0,0,0);
			ids_down01=AcGePoint3d(0,0,0) ;
			Data->MyFunction->Arx->GetObjectIntersect(tag_ray_Id1,ray_upl_ID1,&ids_up01);
			Data->MyFunction->Arx->GetObjectIntersect(tag_ray_Id1,ray_downl_ID1,&ids_down01);
			
			//围带两点
			AcGePoint3d ids_up11=AcGePoint3d(0,0,0);
			AcGePoint3d ids_up21 =AcGePoint3d(0,0,0);
			Data->MyFunction->Arx->GetObjectIntersect(tag_upl_ray_Id1,ray_upl_ID1,&ids_up11);
			Data->MyFunction->Arx->GetObjectIntersect(tag_upr_ray_Id1,ray_upr_ID1,&ids_up21);
			//叶根两点
			AcGePoint3d ids_down11=AcGePoint3d(0,0,0);
			AcGePoint3d ids_down21=AcGePoint3d(0,0,0); 
			Data->MyFunction->Arx->GetObjectIntersect(tag_downl_ray_Id1,ray_downl_ID1,&ids_down11);
			Data->MyFunction->Arx->GetObjectIntersect(tag_downr_ray_Id1,ray_downr_ID1,&ids_down21);
			vertex_out_vec.push_back(ids_down11);
			vertex_out_vec.push_back(ids_down21);


			//删除辅助线
			Data->MyFunction->Arx->DeleteObject(tag_ray_Id1);
			Data->MyFunction->Arx->DeleteObject(tag_upl_ray_Id1);
			Data->MyFunction->Arx->DeleteObject(tag_upr_ray_Id1);
			Data->MyFunction->Arx->DeleteObject(tag_downl_ray_Id1);
			Data->MyFunction->Arx->DeleteObject(tag_downr_ray_Id1);
			Data->MyFunction->Arx->DeleteObject(ray_upl_ID1);
			Data->MyFunction->Arx->DeleteObject(ray_upr_ID1);
			Data->MyFunction->Arx->DeleteObject(ray_downl_ID1);
			Data->MyFunction->Arx->DeleteObject(ray_downr_ID1);

			//勾画叶片 逆时针
			Data->MyFunction->Arx->SetNowLayerName(L"2细实线");
			Data->MyFunction->Arx->CreateLine(vertex8,vertex5 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(vertex6,vertex7 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			Data->MyFunction->Arx->SetNowLayerName(L"1粗实线");
			Data->MyFunction->Arx->CreateLine(vertex5,ids_down01 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(vertex7,vertex8 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			
			//围带
			AcDbObjectId yegen_up_line_id,weidai_down_line_id;
			//修改轮廓点从上到下 从左到右
			AcGePoint3d outlineP1,outlineP2,outlineP3;
			double angle_β = tmp.β * PI /180.0;
			outlineP1 = AcGePoint3d(vertex_05.x +tmp.p32d,vertex_05.y,0);
			outlineP2 = AcGePoint3d(outlineP1.x , outlineP1.y - tmp.c_deep,0);
			outlineP3 = AcGePoint3d(vertex_05.x, outlineP2.y - tmp.p32d*tan(angle_β),0);
		
			
			//判断汽封齿类型
			AcGePoint3d midP1,midP2,midP3,midP4;
			if(tmp.t_type ==1 || tmp.t_type ==2)
			{
				//Z
				midP1 = AcGePoint3d(outlineP1.x+(tmp.p17Bw-tmp.p32d)/2.0-tmp.Z_R/2.0,outlineP1.y,0);
				midP2 = AcGePoint3d(midP1.x,midP1.y-2.0,0);
				midP3 = AcGePoint3d(midP2.x+tmp.Z_R,midP2.y,0);
				midP4 = AcGePoint3d(midP3.x,midP3.y+2.0,0);
			}
			else
			{
				//无Z
				midP1 = outlineP1;
				midP2 = midP1;
				midP3 = midP1;
				midP4 = midP1;
			}
			Data->MyFunction->Arx->CreateLine(outlineP1,outlineP2 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(outlineP2,outlineP3 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			Data->MyFunction->Arx->CreateLine(outlineP3,ids_up11 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(ids_up11,ids_up21,&weidai_down_line_id);
			AircylinderObjects.append(weidai_down_line_id);
			Data->MyFunction->Arx->CreateLine(ids_up21,vertex_08 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(vertex_08,midP4 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			
			Data->MyFunction->Arx->CreateLine(midP4,midP3 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(midP3,midP2 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(midP2,midP1 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(midP1,outlineP1 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			

			//创建叶根槽获取点
			GroovePoint = AcGePoint3d(0,0,0);
			if(i!=bladePair.size()-1)
			{
				after_data = bladePair[i+1];
				GroovePoint = after_data.deep_point;
			}
			else
			{
				AcGePoint3d r_point;
				if(tmp.HubType == 1.0 || tmp.HubType == 4.0)
				{
					r_point = tmp.deep_point;
					r_point.x = r_point.x + tmp.p21Bz;
					
				}
				else if(tmp.HubType == 2.0 || tmp.HubType == 3.0)
				{
					//上浮8.0固定值
					r_point = AcGePoint3d(tmp.p30Dz +tmp.p21Bz - tmp.p22bz,tmp.p28Dpd - tmp.p29Ld/2.0 - tmp.p23hz+8.0,0);
				}

				//最后一级叶片调用末三级
				createLastThreeStages(aircylinderDataVec[ii].LastThreeStages,r_point,tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete,AircylinderObjects);
			}

			//若是横置叶片并且是第一级叶片则链接横置静叶片
			if(i == 0 && aircylinderDataVec[ii].isHorizonBlade == 1.0)
			{
				//下部链接线
				AcDbObjectId tmp1,tmp2,tmp3;
				AcGePoint3d StartPoint,EndPoint;
				//判断气道类型 
				if(aircylinderDataVec[ii].AirPassage=="单流"||aircylinderDataVec[ii].AirPassage=="折流")
				{
					if(tmp.HubType==1.0||tmp.HubType==4.0)
					{
						Data->MyFunction->Arx->CreateLine(l_p,AcGePoint3d(l_p.x,ids_down11.y,0) ,&tmp1);
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(l_p.x,ids_down11.y,0) ,AcGePoint3d(ids_down11.x-0.2,ids_down11.y,0) ,&tmp2);
						Data->MyFunction->Arx->CreateFillet(tmp1, l_p , tmp2, AcGePoint3d(ids_down11.x-0.2,ids_down11.y,0), AirWeight , &tmp3);
						AircylinderObjects.append(tmp1);
						AircylinderObjects.append(tmp2);
						AircylinderObjects.append(tmp3);

						Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_down11.x-0.2,ids_down11.y,0) ,  AcGePoint3d(ids_down11.x-0.2,tmp.deep_point.y,0)  ,&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_down11.x-0.2,tmp.deep_point.y,0),tmp.deep_point,&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
					}
					else if(tmp.HubType==2.0||tmp.HubType==3.0)
					{
						Data->MyFunction->Arx->CreateLine(l_p,AcGePoint3d(l_p.x,ids_down11.y,0) ,&tmp1);
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(l_p.x,ids_down11.y,0) ,AcGePoint3d(ids_down11.x,ids_down11.y,0) ,&tmp2);
						Data->MyFunction->Arx->CreateFillet(tmp1, l_p , tmp2, AcGePoint3d(ids_down11.x,ids_down11.y,0), AirWeight , &tmp3);
						AircylinderObjects.append(tmp1);
						AircylinderObjects.append(tmp2);
						AircylinderObjects.append(tmp3);
					}
				}
				else if(aircylinderDataVec[ii].AirPassage=="双分流")
				{
					////下部链接线
					if(tmp.HubType==1.0||tmp.HubType==4.0)
					{

						Data->MyFunction->Arx->CreateLine(AcGePoint3d(setting.OriginalPoint.x +AirAxialDis,ids_down11.y,0) ,AcGePoint3d(ids_down11.x-0.2,ids_down11.y,0) ,&tmp2);
						AircylinderObjects.append(tmp2);

						Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_down11.x-0.2,ids_down11.y,0) ,  AcGePoint3d(ids_down11.x-0.2,tmp.deep_point.y,0)  ,&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_down11.x-0.2,tmp.deep_point.y,0),tmp.deep_point,&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
					}
					else if(tmp.HubType==2.0||tmp.HubType==3.0)
					{
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(setting.OriginalPoint.x +AirAxialDis,ids_down11.y,0) ,ids_down11 ,&tmp2);
						AircylinderObjects.append(tmp2);
					}

				}
				
				double u_num = 0.0;
				if(tmp.N == 0)
					u_num = tmp.NegativeSpace ;
				else
					u_num = tmp.ForwardSpace ;

				//上部链接线
				AcGePoint3d next_d_p = after_data.S_deep_point;
				Data->MyFunction->Arx->CreateLine(r_p,AcGePoint3d(r_p.x,ids_up11.y,0) ,&tmp1);
				Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_p.x,ids_up11.y,0) ,AcGePoint3d(ids_up11.x-u_num,ids_up11.y,0) ,&tmp2);
				Data->MyFunction->Arx->CreateFillet(tmp1, r_p , tmp2,AcGePoint3d(ids_up11.x-u_num,ids_up11.y,0), 5 , &tmp3);
				AircylinderObjects.append(tmp1);
				AircylinderObjects.append(tmp3);
				
				Data->MyFunction->Arx->GetObjectStartPoint(tmp2, StartPoint);
				Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_up11.x-u_num,ids_up11.y,0), AcGePoint3d(ids_up11.x-u_num,next_d_p.y,0) ,&tmp1);
				Data->MyFunction->Arx->CreateFillet(tmp2, StartPoint , tmp1,AcGePoint3d(ids_up11.x-u_num,next_d_p.y,0), 3 , &tmp3);
				AircylinderObjects.append(tmp2);
				AircylinderObjects.append(tmp3);

				Data->MyFunction->Arx->GetObjectStartPoint(tmp1, StartPoint);
				Data->MyFunction->Arx->CreateLine( AcGePoint3d(ids_up11.x-u_num,next_d_p.y,0) ,next_d_p ,&tmp2);
				Data->MyFunction->Arx->CreateFillet(tmp1, StartPoint , tmp2,next_d_p, 4 , &tmp3);
				AircylinderObjects.append(tmp1);
				AircylinderObjects.append(tmp2);
				AircylinderObjects.append(tmp3);

				//气道中心线
				Data->MyFunction->Arx->SetNowLayerName(L"细点划线");
				Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_p.x - AirWeight/2.0 ,ids_up11.y +80,0) ,AcGePoint3d(r_p.x - AirWeight/2.0 ,ids_down11.y -80,0) ,&tmp2);
				AircylinderObjects.append(tmp3);
				Data->MyFunction->Arx->SetNowLayerName(L"1粗实线");
			}

			//创建叶根
			createHub("动叶", tmp.HubType, tmp, vertex_out_vec, AircylinderObjects);
		
			//创建叶根槽
			createBladeHubGroove("动叶", tmp.HubType, tmp,after_data,vertex_06,vertex_07,AircylinderObjects,GroovePoint);

			//动叶中心线 下-》上 左-》右
			Data->MyFunction->Arx->SetNowLayerName(L"细点划线");
			AcDbObjectId up_line_id_dong;
			up_point1 = AcGePoint3d(tmp.p30Dz,tmp.p28Dpd-tmp.p32-tmp.p23hz-tmp.p27,0);
			up_point2 = AcGePoint3d(tmp.p30Dz,tmp.p28Dpd+tmp.p32+tmp.p19hw,0);
			left_point = AcGePoint3d(tmp.p30Dz-tmp.p35,tmp.p28Dpd,0);
			right_point = AcGePoint3d(tmp.p30Dz+tmp.p35,tmp.p28Dpd,0);
			
			if(tmp.HubType ==1)
				Data->MyFunction->Arx->CreateLine(AcGePoint3d(up_point1.x,vertex_06.y-tmp.GrooveNeckH-tmp.GrooveBottomH-10,0),up_point2,&up_line_id_dong);
			else if(tmp.HubType ==2)
				Data->MyFunction->Arx->CreateLine(AcGePoint3d(up_point1.x,vertex_06.y-tmp.NeckH-tmp.NeckDH+tmp.TSFH-10,0),up_point2,&up_line_id_dong);
			else if(tmp.HubType ==3)
				Data->MyFunction->Arx->CreateLine(AcGePoint3d(up_point1.x,vertex_06.y-tmp.h7-tmp.h6-tmp.h5-tmp.h4-tmp.h3-tmp.h2-tmp.h1-10,0),up_point2,&up_line_id_dong);
			else if(tmp.HubType ==4)
				Data->MyFunction->Arx->CreateLine(AcGePoint3d(up_point1.x,vertex_06.y-tmp.hr-10,0),up_point2,&up_line_id_dong);
			AircylinderObjects.append(up_line_id_dong);
			Data->MyFunction->Arx->CreateLine(left_point,right_point,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			
			//标注////////左右中  下上中
			//动叶根高度
			double p99 = 0.0;
			if(tmp.p31zf ==1)
				p99 =5.6;
			else
				p99 = tmp.p21Bz-tmp.p22bz-7.4;

			point1 = AcGePoint3d(tmp.p30Dz,vertex_06.y,0);
			point2 = AcGePoint3d(tmp.p30Dz,vertex6.y,0);
			point3 = AcGePoint3d(ids_down11.x + p99,vertex6.y-tmp.p23hz/2,0);
			Data->MyFunction->Arx->SetNowLayerName(L"5尺寸线");
			Data->MyFunction->Arx->CreateLinearDim(point1,point2,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p23hz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			//动叶下半宽度
			point1 = AcGePoint3d(tmp.p30Dz,vertex6.y,0);
			point3 = AcGePoint3d(tmp.p30Dz +tmp.p15bg/2,vertex6.y+3.4,0);
			Data->MyFunction->Arx->CreateLinearDim(point1,vertex7,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p15bg),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
			//动叶下宽度
			point3 = AcGePoint3d(vertex7.x-tmp.p13BG/2,point3.y+5.0,0);
			Data->MyFunction->Arx->CreateLinearDim(vertex6,vertex7,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p13BG),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
			//动叶高
			double p98 = 0.0;
			if(tmp.p31zf ==1)
				p98 =1.4;
			else
				p98 = 4.3;
			point3 = AcGePoint3d(ids_down11.x - p98,left_point.y+tmp.p29Ld/4,0);
			Data->MyFunction->Arx->CreateLinearDim(vertex5,vertex6,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p29Ld),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			//动叶上半宽度
			Data->MyFunction->Arx->GetObjectIntersect(weidai_down_line_id,up_line_id_dong,&point1);
			point3 = AcGePoint3d(tmp.p30Dz +tmp.p16bd/2,point1.y-2.6,0);
			Data->MyFunction->Arx->CreateLinearDim(point1,vertex8,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p16bd),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
			//动叶上宽度
			point3 = AcGePoint3d(vertex8.x-tmp.p14BD/2,point1.y-7.6,0);
			Data->MyFunction->Arx->CreateLinearDim(vertex5,vertex8,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p14BD),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			//动叶叶根半宽度
			Data->MyFunction->Arx->GetObjectIntersect(yegen_up_line_id,up_line_id_dong,&point1);
			point3 = AcGePoint3d(tmp.p30Dz +tmp.p22bz/2,point1.y-2.6,0);
			Data->MyFunction->Arx->CreateLinearDim(point1,ids_down21,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p22bz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
			//动叶叶根宽度
			point3 = AcGePoint3d(point3.x,point1.y-7.6,0);
			Data->MyFunction->Arx->CreateLinearDim(ids_down11,ids_down21,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p21Bz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			//动叶围带半宽度
			point1 = AcGePoint3d(tmp.p30Dz,ids_up21.y,0);
			point3 = AcGePoint3d(tmp.p30Dz +tmp.p18bw/2,vertex5.y+3.4,0);
			Data->MyFunction->Arx->CreateLinearDim(point1,ids_up21,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p18bw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
			//动叶围带宽度
			double p101 = 0.0;
			if(tmp.p29Ld >31.9)
				p101 =tmp.p16bd*tan(angle_up)+12.6;
			else
				p101 = -38.8-tmp.p19hw;

			point3 = AcGePoint3d(ids_up21.x - tmp.p17Bw/2,vertex5.y-p101,0);
			Data->MyFunction->Arx->CreateLinearDim(ids_up11,ids_up21,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p17Bw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			//动叶围带高度
			point3 = AcGePoint3d(ids_up21.x+3.6,vertex5.y+tmp.p19hw/2,0);
			Data->MyFunction->Arx->CreateLinearDim(vertex8,vertex_08,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p19hw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			if(i != 0 || aircylinderDataVec[ii].isHorizonBlade != 1.0)
			{
				//动静叶涨差标注up
				double p79 = (tmp.p30Dz-tmp.p0Jz-tmp.p9bz-tmp.p17Bw+tmp.p18bw)/2;
				point3 = AcGePoint3d(ids_up2.x+p79,vertex4.y+8.4,0);
				Data->MyFunction->Arx->CreateLinearDim(ids_up2,ids_up11,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p30Dz-tmp.p0Jz-tmp.p9bz-tmp.p17Bw+tmp.p18bw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

				//动静叶涨差标注down
				double p81 = (tmp.p30Dz-tmp.p0Jz-tmp.p6bw-tmp.p21Bz+tmp.p22bz)/2;
				point3 = AcGePoint3d(ids_down2.x+p81,zhangchai_point.y-2.6,0);
				Data->MyFunction->Arx->CreateLinearDim(ids_down2,ids_down11,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p30Dz-tmp.p0Jz-tmp.p6bw-tmp.p21Bz+tmp.p22bz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			}
			
			//判断气道类型 
			if(aircylinderDataVec[ii].AirPassage=="单流"||aircylinderDataVec[ii].AirPassage=="折流")
			{
				//镜像
				if(tmp.p31zf==0)
					Data->MyFunction->Arx->CreateImage(AircylinderObjects,AcGePoint3d(AirAxialDis+setting.OriginalPoint.x,0.0,0),AcGePoint3d(AirAxialDis+setting.OriginalPoint.x,1.0,0),1);
			}
			else if(aircylinderDataVec[ii].AirPassage=="双分流")
			{
				//镜像
				if(tmp.p31zf==0)
					Data->MyFunction->Arx->CreateImage(AircylinderObjects,AcGePoint3d(AirAxialDis+setting.OriginalPoint.x,0.0,0),AcGePoint3d(AirAxialDis+setting.OriginalPoint.x,1.0,0),0);
			}
			
		}
#pragma endregion
		
	}
	return 0;
}


//创建叶根				          /*叶片类型，      叶根类型,        结构体,                     中间体基准点，                             镜像vec*/
int ClassTongliu::createHub(string bladeType, double hubtype, bladeData tmp,vector<AcGePoint3d>vertex_out_vec, AcDbObjectIdArray &AircylinderObjects)
{
	AcDbObjectId GeneralObjects ,yegen_down_line_id,yegen_up_line_id;
	AcGePoint3d StartPoint,EndPoint;
	AcDbObjectIdArray ListObjects;
	AcDbObjectIdArray ReListObjects;
	if(bladeType == "静叶") 
#pragma region
	{
		AcGePoint3d vertex_01 = vertex_out_vec[0];
		AcGePoint3d vertex_04 = vertex_out_vec[3];
		AcGePoint3d ids_up1 = vertex_out_vec[4];
		AcGePoint3d ids_up2 = vertex_out_vec[5];
		//叶根
		AcGePoint3d stator_daojiao1,stator_daojiao2;
		AcGePoint3d stator_daojiao3,stator_daojiao4;
		//从下上，从左到右
		stator_daojiao1 = AcGePoint3d(vertex_01.x,vertex_01.y-tmp.p6,vertex_01.z);
		stator_daojiao2 = AcGePoint3d(vertex_04.x,vertex_04.y-tmp.p4,vertex_04.z);
		stator_daojiao3 = AcGePoint3d(vertex_01.x+tmp.p5,vertex_01.y,vertex_01.z);
		stator_daojiao4 = AcGePoint3d(vertex_04.x-tmp.p3,vertex_04.y,vertex_04.z);

		Data->MyFunction->Arx->CreateLine(stator_daojiao1,ids_up1,&GeneralObjects);//做倒角
		AircylinderObjects.append(GeneralObjects);
		Data->MyFunction->Arx->CreateLine(ids_up1,ids_up2,&yegen_down_line_id);
		AircylinderObjects.append(yegen_down_line_id);
		Data->MyFunction->Arx->CreateLine(ids_up2,stator_daojiao2,&GeneralObjects);//做倒角
		AircylinderObjects.append(GeneralObjects);
		//Data->MyFunction->Arx->CreateLine(vertex_04,vertex_01);

		//叶根部分倒角
		Data->MyFunction->Arx->CreateLine(stator_daojiao1,stator_daojiao3,&GeneralObjects);//做倒角
		AircylinderObjects.append(GeneralObjects);
		Data->MyFunction->Arx->CreateLine(stator_daojiao2,stator_daojiao4,&GeneralObjects);//做倒角
		AircylinderObjects.append(GeneralObjects);

		//向下勾画动叶片叶根   从上下，从左到右
		AcGePoint3d stator_p1,stator_p2;
		AcGePoint3d stator_p3,stator_p4;
		AcGePoint3d stator_p5,stator_p6;
		AcGePoint3d stator_p7,stator_p8;
		AcGePoint3d stator_mid1,stator_mid2;
		AcGePoint3d stator_mid3,stator_mid4;
		AcGePoint3d stator_mid5,stator_mid6;
		AcGePoint3d stator_mid7,stator_mid8;
		stator_p1 = AcGePoint3d(tmp.p0Jz-tmp.Sbn/2.0,vertex_01.y,0);
		stator_p2 = AcGePoint3d(tmp.p0Jz+tmp.Sbn/2.0,stator_p1.y ,0);
		stator_p3 = AcGePoint3d(stator_p1.x,stator_p1.y+tmp.Shn,0);
		stator_p4 = AcGePoint3d(stator_p2.x,stator_p2.y+tmp.Shn,0);
		stator_p5 = AcGePoint3d(tmp.p0Jz-tmp.Sbz,stator_p3.y+tan(tmp.SHubHorizontalAngle * PI /180.0)*(tmp.Sbz-tmp.Sbn/2.0),0);//叶根夹角
		stator_p6 = AcGePoint3d(tmp.p0Jz+tmp.Sbz,stator_p4.y+tan(tmp.SHubHorizontalAngle * PI /180.0)*(tmp.Sbz-tmp.Sbn/2.0),0);
		stator_p7 = AcGePoint3d(stator_p5.x,stator_p5.y+tmp.Shx,0);
		stator_p8 = AcGePoint3d(stator_p6.x,stator_p6.y+tmp.Shx,0);

		stator_mid1 = AcGePoint3d(stator_p5.x+tmp.Sdaojiao_cb,stator_p5.y-tan(tmp.SHubHorizontalAngle * PI /180.0)*(tmp.Sdaojiao_cb),0);
		stator_mid3 = AcGePoint3d(stator_p5.x,stator_p5.y+tmp.Sdaojiao_cb,0);
		stator_mid5 = AcGePoint3d(stator_mid3.x,stator_p7.y-tmp.Sdaojiao_c,0);
		stator_mid7 = AcGePoint3d(stator_p7.x+tmp.Sdaojiao_c,stator_p7.y,0);

		stator_mid2 = AcGePoint3d(stator_p6.x-tmp.Sdaojiao_cb,stator_p6.y-tan(tmp.SHubHorizontalAngle * PI /180.0)*(tmp.Sdaojiao_cb),0);
		stator_mid4 = AcGePoint3d(stator_p6.x,stator_p6.y+tmp.Sdaojiao_cb,0);
		stator_mid6 = AcGePoint3d(stator_mid4.x,stator_p8.y-tmp.Sdaojiao_c,0);
		stator_mid8 = AcGePoint3d(stator_p8.x-tmp.Sdaojiao_c,stator_p8.y,0);

		AcDbObjectId stator_id_tmp,stator_id1,stator_id2,stator_id3,stator_id4,stator_id5,stator_id6;
		//连接 左
		Data->MyFunction->Arx->CreateLine(stator_daojiao3,stator_p1,  &stator_id1);
		Data->MyFunction->Arx->CreateLine(stator_p1 ,stator_p3,  &stator_id2);
		Data->MyFunction->Arx->CreateFillet(stator_id1, stator_daojiao3 , stator_id2, stator_p3, tmp.daojiao_r, &stator_id_tmp);
		AircylinderObjects.append(stator_id_tmp);
		AircylinderObjects.append(stator_id1);

		Data->MyFunction->Arx->CreateLine(stator_p3,stator_mid1 ,&stator_id3);

		Data->MyFunction->Arx->GetObjectStartPoint(stator_id2, StartPoint);
		Data->MyFunction->Arx->CreateFillet(stator_id2, StartPoint,stator_id3, stator_mid1, tmp.daojiao_r, &stator_id_tmp);
		AircylinderObjects.append(stator_id_tmp);
		AircylinderObjects.append(stator_id2);
		AircylinderObjects.append(stator_id3);

		Data->MyFunction->Arx->CreateLine(stator_mid1,stator_mid3,&GeneralObjects);
		AircylinderObjects.append(GeneralObjects);
		Data->MyFunction->Arx->CreateLine(stator_mid3 ,stator_mid5,&GeneralObjects);
		AircylinderObjects.append(GeneralObjects);
		Data->MyFunction->Arx->CreateLine(stator_mid5,stator_mid7,&GeneralObjects);
		AircylinderObjects.append(GeneralObjects);

		//连接 右
		Data->MyFunction->Arx->CreateLine(stator_daojiao4,stator_p2,  &stator_id4);
		//AircylinderObjects.append(stator_id4);
		Data->MyFunction->Arx->CreateLine(stator_p2 ,stator_p4,  &stator_id5);
		//AircylinderObjects.append(stator_id5);
		Data->MyFunction->Arx->CreateFillet(stator_id4, stator_daojiao4 , stator_id5, stator_p4, tmp.daojiao_r, &stator_id_tmp);
		AircylinderObjects.append(stator_id_tmp);
		AircylinderObjects.append(stator_id4);
		Data->MyFunction->Arx->CreateLine(stator_p4,stator_mid2 ,&stator_id6);
		AircylinderObjects.append(stator_id6);

		Data->MyFunction->Arx->GetObjectStartPoint(stator_id5, StartPoint);
		Data->MyFunction->Arx->CreateFillet(stator_id5, StartPoint, stator_id6, stator_mid2, tmp.daojiao_r, &stator_id_tmp);
		AircylinderObjects.append(stator_id_tmp);
		AircylinderObjects.append(stator_id6);
		AircylinderObjects.append(stator_id5);

		Data->MyFunction->Arx->CreateLine(stator_mid2,stator_mid4,&GeneralObjects);
		AircylinderObjects.append(GeneralObjects);
		Data->MyFunction->Arx->CreateLine(stator_mid4 ,stator_mid6,&GeneralObjects);
		AircylinderObjects.append(GeneralObjects);
		Data->MyFunction->Arx->CreateLine(stator_mid6,stator_mid8,&GeneralObjects);
		AircylinderObjects.append(GeneralObjects);
		//封闭静叶根
		Data->MyFunction->Arx->CreateLine(stator_mid7,stator_mid8,&GeneralObjects);
		AircylinderObjects.append(GeneralObjects);
	}
#pragma endregion

	else if(bladeType == "动叶")
	{
		AcGePoint3d vertex_06 = vertex_out_vec[1];
		AcGePoint3d vertex_07 = vertex_out_vec[2];
		AcGePoint3d ids_down11 = vertex_out_vec[4];
		AcGePoint3d ids_down21 = vertex_out_vec[5];

		if(hubtype == 1)
		//反动式预扭T叶根
#pragma region
		{
			//叶根
			AcGePoint3d rotor_daojiao1,rotor_daojiao2;
			AcGePoint3d rotor_daojiao3,rotor_daojiao4;
			//从上下，从左到右
			rotor_daojiao1 = AcGePoint3d(vertex_06.x,vertex_06.y+tmp.p24,vertex_06.z);
			rotor_daojiao2 = AcGePoint3d(vertex_07.x,vertex_07.y+tmp.p24,vertex_07.z);
			rotor_daojiao3 = AcGePoint3d(vertex_06.x+tmp.p24,vertex_06.y,vertex_06.z);
			rotor_daojiao4 = AcGePoint3d(vertex_07.x-tmp.p24,vertex_07.y,vertex_07.z);
			Data->MyFunction->Arx->CreateLine(ids_down11,rotor_daojiao1,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);//做倒角
			//Data->MyFunction->Arx->CreateLine(vertex_06,vertex_07);
			Data->MyFunction->Arx->CreateLine(rotor_daojiao2,ids_down21,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);//做倒角
			Data->MyFunction->Arx->CreateLine(ids_down21,ids_down11,&yegen_up_line_id);
			AircylinderObjects.append(yegen_up_line_id);

			//叶根部分倒角
			Data->MyFunction->Arx->CreateLine(rotor_daojiao1,rotor_daojiao3,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);//做倒角
			Data->MyFunction->Arx->CreateLine(rotor_daojiao2,rotor_daojiao4,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);//做倒角

			//向下勾画动叶片叶根   从上下，从左到右
			AcGePoint3d rotor_p1,rotor_p2;
			AcGePoint3d rotor_p3,rotor_p4;
			AcGePoint3d rotor_p5,rotor_p6;
			AcGePoint3d rotor_p7,rotor_p8;
			AcGePoint3d rotor_mid1,rotor_mid2;
			AcGePoint3d rotor_mid3,rotor_mid4;
			AcGePoint3d rotor_mid5,rotor_mid6;
			AcGePoint3d rotor_mid7,rotor_mid8;
			rotor_p1 = AcGePoint3d(tmp.p30Dz-tmp.bn/2.0,vertex_06.y,0);
			rotor_p2 = AcGePoint3d(tmp.p30Dz+tmp.bn/2.0,rotor_p1.y ,0);
			rotor_p3 = AcGePoint3d(rotor_p1.x,rotor_p1.y-tmp.hn,0);
			rotor_p4 = AcGePoint3d(rotor_p2.x,rotor_p2.y-tmp.hn,0);
			rotor_p5 = AcGePoint3d(tmp.p30Dz-tmp.bz,rotor_p3.y-tan(tmp.HubHorizontalAngle * PI /180.0)*(tmp.bz-tmp.bn/2.0),0);//叶根夹角
			rotor_p6 = AcGePoint3d(tmp.p30Dz+tmp.bz,rotor_p4.y-tan(tmp.HubHorizontalAngle * PI /180.0)*(tmp.bz-tmp.bn/2.0),0);
			rotor_p7 = AcGePoint3d(rotor_p5.x,rotor_p5.y-tmp.hx,0);
			rotor_p8 = AcGePoint3d(rotor_p6.x,rotor_p6.y-tmp.hx,0);

			//气流从左到右 左侧向内截取0.4 否则反之  此时为左到右
			rotor_mid1 = AcGePoint3d(rotor_p5.x+tmp.daojiao_cb,rotor_p5.y+tan(tmp.HubHorizontalAngle * PI /180.0)*(tmp.daojiao_cb),0);
			rotor_mid3 = AcGePoint3d(rotor_p5.x+0.4,rotor_p5.y-tmp.daojiao_cb+0.4,0);
			rotor_mid5 = AcGePoint3d(rotor_mid3.x,rotor_p7.y+tmp.daojiao_c-0.4,0);
			rotor_mid7 = AcGePoint3d(rotor_p7.x+tmp.daojiao_c,rotor_p7.y,0);

			rotor_mid2 = AcGePoint3d(rotor_p6.x-tmp.daojiao_cb,rotor_p6.y+tan(tmp.HubHorizontalAngle * PI /180.0)*(tmp.daojiao_cb),0);
			rotor_mid4 = AcGePoint3d(rotor_p6.x,rotor_p6.y-tmp.daojiao_cb,0);
			rotor_mid6 = AcGePoint3d(rotor_mid4.x,rotor_p8.y+tmp.daojiao_c,0);
			rotor_mid8 = AcGePoint3d(rotor_p8.x-tmp.daojiao_c,rotor_p8.y,0);

			AcDbObjectId rotor_id_tmp,rotor_id1,rotor_id2,rotor_id3,rotor_id4,rotor_id5,rotor_id6,rotor_id7,rotor_id8;
			//连接 左
			Data->MyFunction->Arx->CreateLine(rotor_daojiao3,rotor_p1,  &rotor_id1);
			Data->MyFunction->Arx->CreateLine(rotor_p1 ,rotor_p3,  &rotor_id2);
			Data->MyFunction->Arx->CreateFillet(rotor_id1, rotor_daojiao3 , rotor_id2, rotor_p3, tmp.daojiao_r, &rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id1);
			Data->MyFunction->Arx->CreateLine(rotor_p3,rotor_mid1 ,&rotor_id3);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id2, StartPoint);
			Data->MyFunction->Arx->CreateFillet(rotor_id2, StartPoint,rotor_id3, rotor_mid1, tmp.daojiao_r, &rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id2);
			AircylinderObjects.append(rotor_id3);

			//向内截取0.4
			Data->MyFunction->Arx->CreateLine(rotor_mid1,rotor_mid3,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_mid3 ,rotor_mid5,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_mid5,rotor_mid7,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			
			//连接 右
			Data->MyFunction->Arx->CreateLine(rotor_daojiao4,rotor_p2,  &rotor_id4);
			Data->MyFunction->Arx->CreateLine(rotor_p2 ,rotor_p4,  &rotor_id5);
			Data->MyFunction->Arx->CreateFillet(rotor_id4, rotor_daojiao4 , rotor_id5, rotor_p4, tmp.daojiao_r, &rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id4);
			Data->MyFunction->Arx->CreateLine(rotor_p4,rotor_mid2 ,&rotor_id6);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id5, StartPoint);
			Data->MyFunction->Arx->CreateFillet(rotor_id5, StartPoint, rotor_id6, rotor_mid2, tmp.daojiao_r, &rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id5);
			AircylinderObjects.append(rotor_id6);
			//右侧不用截取0.4
			Data->MyFunction->Arx->CreateLine(rotor_mid2,rotor_mid4,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_mid4 ,rotor_mid6,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_mid6,rotor_mid8,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			//封闭动叶根
			Data->MyFunction->Arx->CreateLine(rotor_mid7,rotor_mid8,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
		}
#pragma endregion
		else if(hubtype == 2)
		//冲动式非预扭T叶根
#pragma region
		{
			Data->MyFunction->Arx->CreateLine(ids_down11,ids_down21,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			//有角度
			if(tmp.TZAngle == 0.0)
			{
				//叶根左齿
				AcGePoint3d chi1, chi2, chi3, chi4, chi5, chi6, chi7, chi8;
				//从下到上
				chi8 = AcGePoint3d(vertex_06.x,vertex_06.y+tmp.TSFH-1.0,vertex_06.z);
				chi7 = AcGePoint3d(chi8.x-3.0,chi8.y+1.0,chi7.z);
				chi6 = AcGePoint3d(chi7.x,chi7.y+1.0,chi7.z);
				chi5 = AcGePoint3d(chi6.x+3.0 ,chi6.y,chi6.z);
				chi4 = AcGePoint3d(chi5.x,chi5.y+2.0,chi5.z);
				chi3 = AcGePoint3d(chi4.x-3.0,chi4.y+1.0,chi4.z);
				chi2 = AcGePoint3d(chi3.x,chi3.y+1.0,chi3.z);
				chi1 = AcGePoint3d(chi2.x+3.0,chi2.y,chi2.z);
				//连接直线 上到下 左
				Data->MyFunction->Arx->CreateLine(ids_down11,chi1,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(chi1,chi2,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(chi2,chi3,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(chi3,chi4,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(chi4,chi5,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(chi5,chi6,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(chi6,chi7,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(chi7,chi8,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(chi8,vertex_06,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				//右边
				Data->MyFunction->Arx->CreateLine(ids_down21,vertex_07,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);

				//向下勾画动叶片叶根   从上下，从左到右
				AcGePoint3d rotor_p1,rotor_p2,rotor_p3,rotor_p4,rotor_p5,rotor_p6,rotor_p7,rotor_p8,rotor_p9,rotor_p10;
				rotor_p1 = AcGePoint3d(vertex_06.x + (tmp.p21Bz-tmp.MidBodySurW)/2.0-tmp.SFInAngle,vertex_06.y,vertex_06.z);
				rotor_p2 = AcGePoint3d(rotor_p1.x+tmp.SFInAngle,rotor_p1.y+tmp.SFInAngle ,rotor_p1.z);
				rotor_p3 = AcGePoint3d(rotor_p2.x,rotor_p1.y+tmp.TSFH,rotor_p2.z);
				rotor_p4 = AcGePoint3d(rotor_p3.x+(tmp.MidBodySurW -tmp.bn)/2.0,rotor_p3.y,rotor_p3.z);
				
				double y5 = rotor_p4.y-tmp.hn +(tmp.HubDW-tmp.bn)/2.0*tan(tmp.TAngle * PI /180.0);
				double y6 = rotor_p4.y-tmp.hn +tmp.daojiao_cb*tan(tmp.TAngle * PI /180.0);
				rotor_p5 = AcGePoint3d(rotor_p4.x,y5,rotor_p4.z);
				rotor_p6 = AcGePoint3d(tmp.p30Dz-tmp.HubDW/2.0+tmp.daojiao_cb,y6,rotor_p5.z);
				rotor_p7 = AcGePoint3d(rotor_p6.x-tmp.daojiao_cb,rotor_p6.y-tmp.daojiao_cb,rotor_p6.z);
				rotor_p8 = AcGePoint3d(rotor_p7.x,rotor_p6.y-tmp.hx+tmp.daojiao_c,rotor_p7.z);
				rotor_p9 = AcGePoint3d(rotor_p8.x+tmp.daojiao_c,rotor_p8.y-tmp.daojiao_c,rotor_p8.z);
				rotor_p10 = AcGePoint3d(tmp.p30Dz,rotor_p9.y,rotor_p9.z);

				AcDbObjectId rotor_id_tmp,rotor_id1,rotor_id2,rotor_id3,rotor_id4;
				//连接 陆续从左到右从上到下
				Data->MyFunction->Arx->CreateLine(vertex_06,rotor_p1,  &GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(rotor_p1 ,rotor_p2,  &GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);

				Data->MyFunction->Arx->CreateLine(rotor_p2 ,rotor_p3,  &rotor_id1);
				Data->MyFunction->Arx->CreateLine(rotor_p3 ,rotor_p4,  &rotor_id2);
				Data->MyFunction->Arx->CreateFillet(rotor_id1, rotor_p2 , rotor_id2, rotor_p4, tmp.TSFMidCornerR, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id1);
				AircylinderObjects.append(rotor_id1);

				Data->MyFunction->Arx->GetObjectStartPoint(rotor_id2, StartPoint);
				Data->MyFunction->Arx->CreateLine(rotor_p4 ,rotor_p5,  &rotor_id3);
				Data->MyFunction->Arx->CreateFillet(rotor_id2, StartPoint , rotor_id3, rotor_p5, tmp.NeckUCornerR, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id2);
				AircylinderObjects.append(rotor_id2);

				Data->MyFunction->Arx->GetObjectStartPoint(rotor_id3, StartPoint);
				Data->MyFunction->Arx->CreateLine(rotor_p5 ,rotor_p6,  &rotor_id4);
				Data->MyFunction->Arx->CreateFillet(rotor_id3, StartPoint , rotor_id4, rotor_p6, tmp.NeckDCornerR, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id4);
				AircylinderObjects.append(rotor_id4);
				ListObjects.append(rotor_id3);
				AircylinderObjects.append(rotor_id3);

				Data->MyFunction->Arx->CreateLine(rotor_p6,rotor_p7 ,&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(rotor_p7,rotor_p8 ,&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(rotor_p8,rotor_p9 ,&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(rotor_p9,rotor_p10 ,&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
			}
			//无角度
			else
			{
				//中间体
				Data->MyFunction->Arx->CreateLine(ids_down11,vertex_06 ,&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(vertex_06,AcGePoint3d(tmp.p30Dz,vertex_06.y,vertex_06.z ),&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);

				//向下勾画动叶片叶根   从上下，从左到右
				AcGePoint3d rotor_p1,rotor_p2,rotor_p3,rotor_p4,rotor_p5,rotor_p6,rotor_p7,rotor_p8,rotor_p9,rotor_p10;
				rotor_p1 = AcGePoint3d(tmp.p30Dz - tmp.TUestW/2.0,vertex_06.y,vertex_06.z);
				rotor_p2 = AcGePoint3d(tmp.p30Dz - tmp.bn/2.0,rotor_p1.y-(tmp.TUestW-tmp.bn)/2.0/tan(tmp.TZAngle * PI /180.0),rotor_p1.z);
				
				double y5 = rotor_p1.y-tmp.hn +tmp.TSFH +(tmp.HubDW-tmp.bn)/2.0*tan(tmp.TAngle * PI /180.0);
				double y6 = rotor_p1.y-tmp.hn +tmp.TSFH +tmp.daojiao_cb*tan(tmp.TAngle * PI /180.0);
				rotor_p5 = AcGePoint3d(rotor_p2.x,y5,rotor_p2.z);
				rotor_p6 = AcGePoint3d(rotor_p5.x-(tmp.MidBodySurW -tmp.bn)/2.0+tmp.daojiao_cb,y6,rotor_p5.z);
				rotor_p7 = AcGePoint3d(rotor_p6.x-tmp.daojiao_cb,rotor_p6.y-tmp.daojiao_cb,rotor_p6.z);
				rotor_p8 = AcGePoint3d(rotor_p7.x,rotor_p6.y-tmp.hx+tmp.daojiao_c,rotor_p7.z);
				rotor_p9 = AcGePoint3d(rotor_p8.x+tmp.daojiao_c,rotor_p8.y-tmp.daojiao_c,rotor_p8.z);
				rotor_p10 = AcGePoint3d(tmp.p30Dz,rotor_p9.y,rotor_p9.z);

				AcDbObjectId rotor_id_tmp,rotor_id1,rotor_id2,rotor_id3,rotor_id4;
				//连接 陆续从左到右从上到下
				Data->MyFunction->Arx->CreateLine(vertex_06,rotor_p1,  &rotor_id1);
				Data->MyFunction->Arx->CreateLine(rotor_p1 ,rotor_p2,  &rotor_id2);
				Data->MyFunction->Arx->CreateFillet(rotor_id1, vertex_06 , rotor_id2, rotor_p2, 3.0, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id1);
				AircylinderObjects.append(rotor_id1);

				Data->MyFunction->Arx->GetObjectStartPoint(rotor_id2, StartPoint);
				Data->MyFunction->Arx->CreateLine(rotor_p2 ,rotor_p5,  &rotor_id3);
				Data->MyFunction->Arx->CreateFillet(rotor_id2, StartPoint , rotor_id3, rotor_p5, tmp.NeckMCornerR, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id2);
				AircylinderObjects.append(rotor_id2);
				Data->MyFunction->Arx->CreateLine(rotor_p1,StartPoint,  &GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				
				Data->MyFunction->Arx->GetObjectStartPoint(rotor_id3, StartPoint);
				Data->MyFunction->Arx->CreateLine(rotor_p5 ,rotor_p6,  &rotor_id4);
				Data->MyFunction->Arx->CreateFillet(rotor_id3, StartPoint , rotor_id4, rotor_p6, tmp.NeckDCornerR, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id4);
				AircylinderObjects.append(rotor_id4);
				ListObjects.append(rotor_id3);
				AircylinderObjects.append(rotor_id3);

				Data->MyFunction->Arx->CreateLine(rotor_p6,rotor_p7 ,&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(rotor_p7,rotor_p8 ,&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(rotor_p8,rotor_p9 ,&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(rotor_p9,rotor_p10 ,&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
			}
			ReListObjects = Data->MyFunction->Arx->CreateImage(ListObjects,AcGePoint3d(tmp.p30Dz,1,0),AcGePoint3d(tmp.p30Dz,0,0),0);
			AircylinderObjects.append(ListObjects);
			AircylinderObjects.append(ReListObjects);
			
		}
#pragma endregion
		else if(hubtype == 3)
		//非预扭双倒T叶根
#pragma region
		{
			Data->MyFunction->Arx->CreateLine(ids_down11,ids_down21,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			//叶根左齿
			AcGePoint3d chi1, chi2, chi3, chi4, chi5, chi6, chi7, chi8;
			//从下到上
			chi8 = AcGePoint3d(vertex_06.x,vertex_06.y+tmp.h-1.0,vertex_06.z);
			chi7 = AcGePoint3d(chi8.x-3.0,chi8.y+1.0,chi7.z);
			chi6 = AcGePoint3d(chi7.x,chi7.y+1.0,chi7.z);
			chi5 = AcGePoint3d(chi6.x+3.0 ,chi6.y,chi6.z);
			chi4 = AcGePoint3d(chi5.x,chi5.y+2.0,chi5.z);
			chi3 = AcGePoint3d(chi4.x-3.0,chi4.y+1.0,chi4.z);
			chi2 = AcGePoint3d(chi3.x,chi3.y+1.0,chi3.z);
			chi1 = AcGePoint3d(chi2.x+3.0,chi2.y,chi2.z);
			//连接直线 上到下 左
			Data->MyFunction->Arx->CreateLine(ids_down11,chi1,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(chi1,chi2,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(chi2,chi3,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(chi3,chi4,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(chi4,chi5,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(chi5,chi6,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(chi6,chi7,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(chi7,chi8,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(chi8,vertex_06,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			//右边
			Data->MyFunction->Arx->CreateLine(ids_down21,vertex_07,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			//向下勾画动叶片叶根   从上下，从左到右
			AcGePoint3d rotor_p1,rotor_p2,rotor_p3,rotor_p4,rotor_p5,rotor_p6,rotor_p7,rotor_p8,rotor_p9,rotor_p10,rotor_p11,rotor_p12,rotor_p13,rotor_p14,rotor_p15;
			rotor_p1 = AcGePoint3d(tmp.p30Dz - tmp.b1/2.0 - 0.5,vertex_06.y,0);
			rotor_p2 = AcGePoint3d(rotor_p1.x+0.5 ,rotor_p1.y+0.5 ,0);
			rotor_p3 = AcGePoint3d(rotor_p2.x ,rotor_p1.y+tmp.h ,0);
			rotor_p4 = AcGePoint3d(rotor_p3.x+(tmp.b1-tmp.b2)/2.0 ,rotor_p3.y ,0);
			rotor_p5 = AcGePoint3d(rotor_p4.x ,rotor_p4.y-tmp.h2 ,0);
			rotor_p6 = AcGePoint3d(tmp.p30Dz-tmp.b3/2.0+tmp.d1 ,rotor_p5.y ,0);
			rotor_p7 = AcGePoint3d(rotor_p6.x-tmp.d1 ,rotor_p6.y-tmp.d1 ,0);
			rotor_p8 = AcGePoint3d(rotor_p7.x ,rotor_p6.y-tmp.h3 ,0);
			rotor_p9 = AcGePoint3d(tmp.p30Dz-tmp.b4/2.0 ,rotor_p8.y-tmp.h4 ,0);

			rotor_p10 = AcGePoint3d(rotor_p9.x ,rotor_p9.y-tmp.h5 ,0);
			rotor_p11 = AcGePoint3d(tmp.p30Dz-tmp.b5/2.0+tmp.d3 ,rotor_p10.y ,0);
			rotor_p12 = AcGePoint3d(rotor_p11.x-tmp.d3 ,rotor_p11.y-tmp.d3 ,0);
			rotor_p13 = AcGePoint3d(rotor_p12.x ,rotor_p11.y - tmp.h6 ,0);
			rotor_p14 = AcGePoint3d(rotor_p13.x+tmp.h7 ,rotor_p13.y-tmp.h7 ,0);
			rotor_p15 = AcGePoint3d(tmp.p30Dz ,rotor_p14.y ,0);
			
			AcDbObjectId rotor_id_tmp,rotor_id1,rotor_id2,rotor_id3,rotor_id4,rotor_id5,rotor_id6,rotor_id7,rotor_id8,rotor_id9,rotor_id10;
			Data->MyFunction->Arx->CreateLine(vertex_06,rotor_p1,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p1 ,rotor_p2,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			Data->MyFunction->Arx->CreateLine(rotor_p2 ,rotor_p3,  &rotor_id1);
			Data->MyFunction->Arx->CreateLine(rotor_p3 ,rotor_p4,  &rotor_id2);
			Data->MyFunction->Arx->CreateFillet(rotor_id1, rotor_p2 , rotor_id2, rotor_p4, 0.5, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id1);
			AircylinderObjects.append(rotor_id1);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id2, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p4 ,rotor_p5,  &rotor_id3);
			Data->MyFunction->Arx->CreateFillet(rotor_id2, StartPoint , rotor_id3, rotor_p5, 0.5, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id2);
			AircylinderObjects.append(rotor_id2);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id3, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p5 ,rotor_p6,  &rotor_id4);
			Data->MyFunction->Arx->CreateFillet(rotor_id3, StartPoint , rotor_id4, rotor_p6, tmp.r1, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id3);
			AircylinderObjects.append(rotor_id3);
			ListObjects.append(rotor_id4);
			AircylinderObjects.append(rotor_id4);

			Data->MyFunction->Arx->CreateLine(rotor_p6,rotor_p7,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			Data->MyFunction->Arx->CreateLine(rotor_p7 ,rotor_p8,  &rotor_id5);
			Data->MyFunction->Arx->CreateLine(rotor_p8 ,rotor_p9,  &rotor_id6);
			Data->MyFunction->Arx->CreateFillet(rotor_id5, rotor_p7 , rotor_id6, rotor_p9, tmp.r1, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id5);
			AircylinderObjects.append(rotor_id5);
			
			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id6, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p9 ,rotor_p10,  &rotor_id7);
			Data->MyFunction->Arx->CreateFillet(rotor_id6, StartPoint , rotor_id7, rotor_p10, tmp.r3, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id6);
			AircylinderObjects.append(rotor_id6);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id7, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p10 ,rotor_p11,  &rotor_id8);
			Data->MyFunction->Arx->CreateFillet(rotor_id7, StartPoint , rotor_id8, rotor_p11, tmp.r3, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id7);
			AircylinderObjects.append(rotor_id7);
			ListObjects.append(rotor_id8);
			AircylinderObjects.append(rotor_id8);

			Data->MyFunction->Arx->CreateLine(rotor_p11,rotor_p12,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			Data->MyFunction->Arx->CreateLine(rotor_p12,rotor_p13,  &rotor_id9);
			Data->MyFunction->Arx->CreateLine(rotor_p13,rotor_p14,  &rotor_id10);
			Data->MyFunction->Arx->CreateFillet(rotor_id9, rotor_p12 , rotor_id10, rotor_p14, 1.2, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id9);
			AircylinderObjects.append(rotor_id9);
			ListObjects.append(rotor_id10);
			AircylinderObjects.append(rotor_id10);

			Data->MyFunction->Arx->CreateLine(rotor_p14,rotor_p15,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			ReListObjects = Data->MyFunction->Arx->CreateImage(ListObjects,AcGePoint3d(tmp.p30Dz,1,0),AcGePoint3d(tmp.p30Dz,0,0),0);
			AircylinderObjects.append(ListObjects);
			AircylinderObjects.append(ReListObjects);
		}
#pragma endregion
		else if(hubtype == 4)
		//预扭双倒T叶根
#pragma region
		{
			AcGePoint3d rotor_p1,rotor_p2,rotor_p3,rotor_p4,rotor_p5,rotor_p6,rotor_p7,rotor_p8,rotor_p9,rotor_p10,rotor_p11,rotor_p12,rotor_p13,rotor_p14,rotor_p15;
			rotor_p1 = AcGePoint3d(vertex_06.x,vertex_06.y+0.3,0);
			rotor_p2 = AcGePoint3d(vertex_06.x+0.3 ,vertex_06.y ,0);

			Data->MyFunction->Arx->CreateLine(ids_down11,ids_down21,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(ids_down11,rotor_p1,  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p1,rotor_p2,  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(ids_down21,AcGePoint3d(vertex_07.x,vertex_07.y +0.3,0),  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(AcGePoint3d(vertex_07.x,vertex_07.y +0.3,0),AcGePoint3d(vertex_07.x-0.3,vertex_07.y,0),  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			//向下勾画动叶片叶根   从上下，从左到右
			rotor_p3 = AcGePoint3d(vertex_06.x + (tmp.b1-tmp.b2)/2.0,rotor_p2.y ,0);
			rotor_p4 = AcGePoint3d(rotor_p3.x ,rotor_p3.y -tmp.h2,0);
			rotor_p5 = AcGePoint3d(tmp.p30Dz-(tmp.b3+0.4)/2.0 + 1 ,rotor_p4.y,0);
			rotor_p6 = AcGePoint3d(tmp.p30Dz-(tmp.b3+0.4)/2.0 ,rotor_p5.y-1 ,0);
			rotor_p7 = AcGePoint3d(rotor_p6.x ,rotor_p5.y-tmp.h3 ,0);
			rotor_p8 = AcGePoint3d(rotor_p7.x+tmp.daojiao_c ,rotor_p7.y-tmp.daojiao_c ,0);
			rotor_p9 = AcGePoint3d(tmp.p30Dz-tmp.b4/2.0 ,rotor_p8.y ,0);
			rotor_p10 = AcGePoint3d(rotor_p9.x ,rotor_p9.y-tmp.h5 ,0);
			rotor_p11 = AcGePoint3d(tmp.p30Dz-tmp.b5/2.0+tmp.d3 ,rotor_p10.y ,0);
			rotor_p12 = AcGePoint3d(rotor_p11.x-tmp.d3 ,rotor_p11.y-tmp.d3 ,0);
			rotor_p13 = AcGePoint3d(rotor_p12.x ,rotor_p11.y - tmp.h6 ,0);
			rotor_p14 = AcGePoint3d(rotor_p13.x+tmp.h7 ,rotor_p13.y-tmp.h7 ,0);
			rotor_p15 = AcGePoint3d(tmp.p30Dz ,rotor_p14.y ,0);

			AcDbObjectId rotor_id_tmp,rotor_id1,rotor_id2,rotor_id3,rotor_id4,rotor_id5,rotor_id6,rotor_id7,rotor_id8,rotor_id9,rotor_id10;
			Data->MyFunction->Arx->CreateLine(rotor_p2,rotor_p3,  &rotor_id1);
			Data->MyFunction->Arx->CreateLine(rotor_p3,rotor_p4,  &rotor_id2);
			Data->MyFunction->Arx->CreateFillet(rotor_id1, rotor_p2 , rotor_id2, rotor_p4, tmp.r1, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id1);
			AircylinderObjects.append(rotor_id1);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id2, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p4,rotor_p5,  &rotor_id3);
			Data->MyFunction->Arx->CreateFillet(rotor_id2, StartPoint , rotor_id3, rotor_p5, tmp.r1, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id2);
			AircylinderObjects.append(rotor_id2);
			ListObjects.append(rotor_id3);
			AircylinderObjects.append(rotor_id3);

			//进气侧扣除0.4，加入进气镜像，不进入叶根镜像
			Data->MyFunction->Arx->CreateLine(rotor_p5,AcGePoint3d(rotor_p6.x+0.4,rotor_p6.y+0.4,0),  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(AcGePoint3d(rotor_p6.x+0.4,rotor_p6.y+0.4,0),AcGePoint3d(rotor_p6.x+0.4,rotor_p7.y-0.4,0),  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(AcGePoint3d(rotor_p6.x+0.4,rotor_p7.y-0.4,0),rotor_p8 , &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			//出气测
			Data->MyFunction->Arx->CreateLine(AcGePoint3d((tmp.p30Dz -rotor_p5.x)*2+rotor_p5.x , rotor_p5.y , 0),AcGePoint3d((tmp.p30Dz -rotor_p6.x)*2+rotor_p6.x , rotor_p6.y , 0),  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(AcGePoint3d((tmp.p30Dz -rotor_p6.x)*2+rotor_p6.x , rotor_p6.y , 0),AcGePoint3d((tmp.p30Dz -rotor_p7.x)*2+rotor_p7.x , rotor_p7.y , 0),  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(AcGePoint3d((tmp.p30Dz -rotor_p7.x)*2+rotor_p7.x , rotor_p7.y , 0),AcGePoint3d((tmp.p30Dz -rotor_p8.x)*2+rotor_p8.x , rotor_p8.y , 0),  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			
			Data->MyFunction->Arx->CreateLine(rotor_p8,rotor_p9,  &rotor_id4);
			Data->MyFunction->Arx->CreateLine(rotor_p9,rotor_p10,  &rotor_id5);
			Data->MyFunction->Arx->CreateFillet(rotor_id4, rotor_p8 , rotor_id5, rotor_p10, tmp.r3, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id4);
			AircylinderObjects.append(rotor_id4);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id5, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p10,rotor_p11,  &rotor_id6);
			Data->MyFunction->Arx->CreateFillet(rotor_id5, StartPoint , rotor_id6, rotor_p11, tmp.r1, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id5);
			AircylinderObjects.append(rotor_id5);
			ListObjects.append(rotor_id6);
			AircylinderObjects.append(rotor_id6);

			Data->MyFunction->Arx->CreateLine(rotor_p11,rotor_p12,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p12,rotor_p13,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p13,rotor_p14,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p14,rotor_p15,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			ReListObjects = Data->MyFunction->Arx->CreateImage(ListObjects,AcGePoint3d(tmp.p30Dz,1,0),AcGePoint3d(tmp.p30Dz,0,0),0);
			AircylinderObjects.append(ListObjects);
			AircylinderObjects.append(ReListObjects);
		}
#pragma endregion
	}
	return 0;
}

//创建叶根槽									/*叶片类型，    叶根类型,       结构体，	下一级结构体，      中间体左基准点，         中间体右基准点,				镜像vec,				后一叶根槽基准深度点（注：最后一对没有深度点）*/
int ClassTongliu::createBladeHubGroove(string bladeType, double hubtype, bladeData tmp,  bladeData next_tmp,  AcGePoint3d basePoint_l, AcGePoint3d basePoint_r,   AcDbObjectIdArray &AircylinderObjects,  AcGePoint3d deepPoint/* = AcGePoint3d::kOrigin*/)
{
	AcGePoint3d StartPoint,EndPoint;
	//从下上，从右到左 预先从右到左
	AcGePoint3d groove_mid1,groove_mid3,groove_mid5,groove_mid7,groove_mid9,groove_mid11,groove_mid13,groove_mid15,groove_mid14;
	AcDbObjectIdArray ListObjects;
	AcDbObjectIdArray ReListObjects;
	AcDbObjectId tmp1,tmp2,tmp3;
	AcDbObjectId GeneralObjects;

	if(bladeType == "静叶") 
#pragma region
	{
		groove_mid1  = AcGePoint3d(basePoint_r.x-(tmp.SMidBodyH-tmp.SGrooveNeckW)/2.0+tmp.SGrooveNeckChamfer, basePoint_r.y,0);
		groove_mid3  = AcGePoint3d(groove_mid1.x-tmp.SGrooveNeckChamfer,groove_mid1.y+tmp.SGrooveNeckChamfer,0);
		groove_mid5  = AcGePoint3d(groove_mid3.x,groove_mid1.y+tmp.SGrooveNeckH -tmp.SGrooveNeckChamfer,0);
		groove_mid7  = AcGePoint3d(groove_mid5.x+tmp.SGrooveNeckChamfer,groove_mid5.y+tmp.SGrooveNeckChamfer+tan(tmp.SHubHorizontalAngle * PI /180.0)*tmp.SGrooveNeckChamfer,0);
		groove_mid9  = AcGePoint3d(groove_mid5.x+(tmp.SGrooveBottomW-tmp.SGrooveNeckW)/2.0,groove_mid5.y+tmp.SGrooveNeckChamfer + tan(tmp.SHubHorizontalAngle * PI /180.0)*(tmp.SGrooveBottomW-tmp.SGrooveNeckW)/2.0,0);
		groove_mid11 = AcGePoint3d(0,0,0);
		AcGePoint3d groove_mids[] ={groove_mid1 , groove_mid3, groove_mid5, groove_mid7, groove_mid9};

		//叶根槽
		AcGePoint3d S_deep_point = tmp.S_deep_point;
		double x1 = tmp.p0Jz;
		double dx = S_deep_point.x - x1;
		S_deep_point.x = x1 - dx;

		//从上下，从左到右
		Data->MyFunction->Arx->CreateLine(S_deep_point,basePoint_r, &tmp1);
		Data->MyFunction->Arx->CreateLine(basePoint_r,groove_mid1, &tmp2);
		Data->MyFunction->Arx->CreateFillet(tmp1, S_deep_point , tmp2, groove_mid1, 0.3 , &tmp3);
		ListObjects.append(tmp1);
		ListObjects.append(tmp2);
		ListObjects.append(tmp3);

		Data->MyFunction->Arx->CreateLine(groove_mid1,groove_mid3,&tmp1);
		ListObjects.append(tmp1);
		Data->MyFunction->Arx->CreateLine(groove_mid3,groove_mid5,&tmp1);
		ListObjects.append(tmp1);
		Data->MyFunction->Arx->CreateLine(groove_mid5,groove_mid7,&tmp1);
		ListObjects.append(tmp1);
		Data->MyFunction->Arx->CreateLine(groove_mid7,groove_mid9,&tmp1);

		if(tmp.SGrooveVerticalAngle!=0.0)
		{
			double GrooveVerticalAngle = (270 +tmp.SGrooveVerticalAngle) * PI /180.0;
			Data->MyFunction->Arx->CreateLine(groove_mid9,GrooveVerticalAngle, 1000.0,0, &tmp2);
			AcGePoint3d point1 = AcGePoint3d(tmp.p0Jz-1000,groove_mid9.y+tmp.SGrooveBottomH,0);
			AcGePoint3d point2 = AcGePoint3d(tmp.p0Jz+1000,groove_mid9.y+tmp.SGrooveBottomH,0);
			Data->MyFunction->Arx->CreateLine(point1,point2, &tmp3);
			Data->MyFunction->Arx->GetObjectIntersect(tmp2,tmp3,&groove_mid11);
			Data->MyFunction->Arx->DeleteObject(tmp2);
			Data->MyFunction->Arx->DeleteObject(tmp3);
		}
		else
		{
			groove_mid11 = AcGePoint3d(groove_mid9.x,groove_mid9.y+tmp.SGrooveBottomH,0);
		}

		groove_mid13 = AcGePoint3d(tmp.p0Jz+tmp.SInterstitialsR,groove_mid11.y,0);
		groove_mid15 = AcGePoint3d(tmp.p0Jz,groove_mid11.y,0);
		groove_mid14 = AcGePoint3d(tmp.p0Jz-tmp.SInterstitialsR,groove_mid11.y,0);

		Data->MyFunction->Arx->CreateLine(groove_mid9,groove_mid11 , &tmp2);
		//ListObjects.append(tmp2);
		Data->MyFunction->Arx->CreateFillet(tmp1, groove_mid7 , tmp2, groove_mid11, tmp.SUCornerR , &tmp3);
		ListObjects.append(tmp1);
		ListObjects.append(tmp3);

		Data->MyFunction->Arx->GetObjectStartPoint(tmp2, StartPoint);

		Data->MyFunction->Arx->CreateLine(groove_mid11,groove_mid13 , &tmp1);
		Data->MyFunction->Arx->CreateFillet(tmp2, StartPoint , tmp1, groove_mid13, tmp.SDCornerR , &tmp3);
		ListObjects.append(tmp3);
		ListObjects.append(tmp2);
		ListObjects.append(tmp1);

		Data->MyFunction->Arx->CreateArc(groove_mid15, tmp.SInterstitialsR, 0, 180, &tmp1);
		AircylinderObjects.append(tmp1);

		ReListObjects = Data->MyFunction->Arx->CreateImage(ListObjects,AcGePoint3d(tmp.p0Jz,1,0),AcGePoint3d(tmp.p0Jz,0,0),0);
		AircylinderObjects.append(ListObjects);
		AircylinderObjects.append(ReListObjects);

		//叶根槽连接线
		if(!Data->MyFunction->Arx->IsPointSame(deepPoint, AcGePoint3d(0,0,0)))
		{
			AcGePoint3d r_point = S_deep_point;

			//若有抽口则不连线单独勾画
			if(tmp.IsSuctionPort==1.0)
			{

				//右侧先计算下一级中间体左基准点
				AcGePoint3d vertex1   = AcGePoint3d(next_tmp.p0Jz + next_tmp.p4bd -next_tmp.p2BD, next_tmp.p26Dpp + next_tmp.p27Lp/2.0, 0);
				AcGePoint3d vertex4   = AcGePoint3d(next_tmp.p0Jz + next_tmp.p4bd , next_tmp.p26Dpp + next_tmp.p27Lp/2.0, 0);
				//外围轮廓点 顺序逆时针
				AcGePoint3d vertex_01 = AcGePoint3d(next_tmp.p0Jz + next_tmp.p9bz -next_tmp.p8Bz, vertex1.y+next_tmp.p10hz, 0);
				//角度
				double angle_up = next_tmp.p11ad * PI /180.0;

				//求叶片交点
				AcDbObjectId  tag_upl_ray_Id,tag_upr_ray_Id,ray_upl_ID,ray_upr_ID;
				Data->MyFunction->Arx->CreateRay(vertex_01,AcGeVector3d(0,-1,0), &tag_upl_ray_Id);
				Data->MyFunction->Arx->CreateLine(vertex4,angle_up,  1000.0,0, &ray_upl_ID);

				//左基准点
				AcGePoint3d ids_up1=AcGePoint3d(0,0,0);
				Data->MyFunction->Arx->GetObjectIntersect(tag_upl_ray_Id,ray_upl_ID,&ids_up1);
				Data->MyFunction->Arx->DeleteObject(tag_upl_ray_Id);
				Data->MyFunction->Arx->DeleteObject(ray_upl_ID);

				if(tmp.CoverDegree<=0.5)
				{
					//左侧为直角
					Data->MyFunction->Arx->CreateLine(r_point, AcGePoint3d(r_point.x+tmp.OutletWeight,r_point.y,0) ,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point.x+tmp.OutletWeight,r_point.y,0), AcGePoint3d(r_point.x+tmp.OutletWeight,r_point.y+40.0,0) ,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);

					//右侧为圆角
					AcDbObjectId tmp1,tmp2;
					Data->MyFunction->Arx->CreateLine(ids_up1,AcGePoint3d(ids_up1.x-tmp.InletWeight,ids_up1.y,ids_up1.z) , &tmp1);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_up1.x-tmp.InletWeight,ids_up1.y,ids_up1.z),AcGePoint3d(ids_up1.x-tmp.InletWeight,ids_up1.y+40.0,ids_up1.z) , &tmp2);
					Data->MyFunction->Arx->CreateFillet(tmp1, ids_up1 , tmp2, AcGePoint3d(ids_up1.x-tmp.InletWeight,ids_up1.y+40.0,ids_up1.z), 20.0 , &GeneralObjects);
					AircylinderObjects.append(tmp1);
					AircylinderObjects.append(tmp2);
					AircylinderObjects.append(GeneralObjects);
				}
				else
				{
					//否则都为2*45倒角
					Data->MyFunction->Arx->CreateLine(r_point, AcGePoint3d(r_point.x+tmp.OutletWeight-2,r_point.y,0) ,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point.x+tmp.OutletWeight-2,r_point.y,0), AcGePoint3d(r_point.x+tmp.OutletWeight,r_point.y+2,0) ,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point.x+tmp.OutletWeight,r_point.y+2,0), AcGePoint3d(r_point.x+tmp.OutletWeight,r_point.y+40.0,0) ,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);

					Data->MyFunction->Arx->CreateLine(ids_up1,AcGePoint3d(ids_up1.x-tmp.InletWeight+2.0,ids_up1.y,ids_up1.z) , &GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_up1.x-tmp.InletWeight+2.0,ids_up1.y,ids_up1.z),AcGePoint3d(ids_up1.x-tmp.InletWeight,ids_up1.y+2.0,ids_up1.z) , &GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(ids_up1.x-tmp.InletWeight,ids_up1.y+2.0,ids_up1.z),AcGePoint3d(ids_up1.x-tmp.InletWeight,ids_up1.y+40.0,ids_up1.z) , &GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
				}
			}
			//若下级折流则不连线
			else if(next_tmp.BrokenTag!=1.0)
			{
				//对比此槽深度与下一槽深度
				if(r_point.y == deepPoint.y)
				{
					Data->MyFunction->Arx->CreateLine(r_point, deepPoint,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
				}
				else if(r_point.y < deepPoint.y)
				{
					Data->MyFunction->Arx->CreateLine(r_point, AcGePoint3d(deepPoint.x-0.2,r_point.y,0),&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(deepPoint.x-0.2,r_point.y,0), AcGePoint3d(deepPoint.x-0.2,deepPoint.y,0),&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(deepPoint.x-0.2,deepPoint.y,0), deepPoint,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
				}
				else if(r_point.y > deepPoint.y)
				{
					Data->MyFunction->Arx->CreateLine(r_point, AcGePoint3d(r_point.x+0.2,r_point.y,0),&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point.x+0.2,r_point.y,0), AcGePoint3d(r_point.x+0.2,deepPoint.y,0),&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point.x+0.2,deepPoint.y,0), deepPoint,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
				}

			}

			//若下级折流则不画汽封齿
			if(next_tmp.BrokenTag!=1.0)
			{
				//汽封齿排布 判断汽封齿类型
				AcDbObjectId ObjectId;
				if(tmp.t_type == 1)//一短一长一短
				{
					Data->MyFunction->Arx->InsertBlock("R_L_short",AcGePoint3d(r_point.x+tmp.a-0.65,r_point.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("R_L_long",AcGePoint3d(r_point.x+tmp.b-0.65,r_point.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("R_L_short",AcGePoint3d(r_point.x+tmp.c-0.65,r_point.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
				}
				else if(tmp.t_type == 2)// 一长一短
				{
					Data->MyFunction->Arx->InsertBlock("R_L_long",AcGePoint3d(r_point.x+tmp.b-0.65,r_point.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("R_L_short",AcGePoint3d(r_point.x+tmp.c-0.65,r_point.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
				}
				else if(tmp.t_type == 3)// 短齿
				{
					for(int i = 1;i<=tmp.t;i++)
					{
						double dis = tmp.b + 4.3*(i - tmp.t);
						Data->MyFunction->Arx->InsertBlock("R_L_short",AcGePoint3d(r_point.x+dis,r_point.y,0), ObjectId);
						AircylinderObjects.append(ObjectId);
					}
				}
			}
		}
	}
#pragma endregion
	else if(bladeType == "动叶")
	{
		if(hubtype == 1)
		//反动式预扭T叶根
#pragma region
		{
			//从上下，从左到右 预先从左到右
			groove_mid1  = AcGePoint3d(basePoint_l.x+(tmp.MidBodyH-tmp.GrooveNeckW)/2.0-tmp.GrooveNeckChamfer, basePoint_l.y,0);
			groove_mid3  = AcGePoint3d(groove_mid1.x+tmp.GrooveNeckChamfer,groove_mid1.y-tmp.GrooveNeckChamfer,0);
			groove_mid5  = AcGePoint3d(groove_mid3.x,groove_mid1.y-tmp.GrooveNeckH +tmp.GrooveNeckChamfer,0);
			groove_mid7  = AcGePoint3d(groove_mid5.x-tmp.GrooveNeckChamfer,groove_mid5.y-tmp.GrooveNeckChamfer-tan(tmp.HubHorizontalAngle * PI /180.0)*tmp.GrooveNeckChamfer,0);
			groove_mid9  = AcGePoint3d(groove_mid5.x-(tmp.GrooveBottomW-tmp.GrooveNeckW)/2.0,groove_mid5.y-tmp.GrooveNeckChamfer - tan(tmp.HubHorizontalAngle * PI /180.0)*(tmp.GrooveBottomW-tmp.GrooveNeckW)/2.0,0);
			groove_mid11 = AcGePoint3d(0,0,0);
			AcGePoint3d groove_mids_r[] ={groove_mid1 , groove_mid3, groove_mid5, groove_mid7, groove_mid9};

			//叶根槽
			//if(tmp.p31zf==1.0)
			//{
				//从上下，从左到右
				Data->MyFunction->Arx->CreateLine(tmp.deep_point,basePoint_l, &tmp1);
				Data->MyFunction->Arx->CreateLine(basePoint_l,groove_mid1, &tmp2);
				Data->MyFunction->Arx->CreateFillet(tmp1, tmp.deep_point , tmp2, groove_mid1, 0.3 , &tmp3);
				ListObjects.append(tmp1);
				ListObjects.append(tmp2);
				ListObjects.append(tmp3);

				Data->MyFunction->Arx->CreateLine(groove_mid1,groove_mid3,&tmp1);
				ListObjects.append(tmp1);
				Data->MyFunction->Arx->CreateLine(groove_mid3,groove_mid5,&tmp1);
				ListObjects.append(tmp1);
				Data->MyFunction->Arx->CreateLine(groove_mid5,groove_mid7,&tmp1);
				ListObjects.append(tmp1);
				Data->MyFunction->Arx->CreateLine(groove_mid7,groove_mid9,&tmp1);

				if(tmp.GrooveVerticalAngle!=0.0)
				{
					double GrooveVerticalAngle = (tmp.GrooveVerticalAngle+270) * PI /180.0;
					Data->MyFunction->Arx->CreateLine(groove_mid9,GrooveVerticalAngle, 1000.0,1, &tmp2);
					AcGePoint3d point1 = AcGePoint3d(tmp.p30Dz-1000,groove_mid9.y-tmp.GrooveBottomH,0);
					AcGePoint3d point2 = AcGePoint3d(tmp.p30Dz+1000,groove_mid9.y-tmp.GrooveBottomH,0);
					Data->MyFunction->Arx->CreateLine(point1,point2, &tmp3);
					Data->MyFunction->Arx->GetObjectIntersect(tmp2,tmp3,&groove_mid11);
					Data->MyFunction->Arx->DeleteObject(tmp2);
					Data->MyFunction->Arx->DeleteObject(tmp3);
				}
				else
				{
					groove_mid11 = AcGePoint3d(groove_mid9.x,groove_mid9.y-tmp.GrooveBottomH,0);
				}

				groove_mid13 = AcGePoint3d(tmp.p30Dz-tmp.InterstitialsR,groove_mid11.y,0);
				groove_mid15 = AcGePoint3d(tmp.p30Dz,groove_mid11.y,0);
				groove_mid14 = AcGePoint3d(tmp.p30Dz+tmp.InterstitialsR,groove_mid11.y,0);

				Data->MyFunction->Arx->CreateLine(groove_mid9,groove_mid11 , &tmp2);
				//ListObjects.append(tmp2);
				Data->MyFunction->Arx->CreateFillet(tmp1, groove_mid7 , tmp2, groove_mid11, tmp.UCornerR , &tmp3);
				ListObjects.append(tmp1);
				ListObjects.append(tmp3);

				//Data->MyFunction->Arx->GetObjectStartPoint(tmp1, StartPoint);
				//Data->MyFunction->Arx->GetObjectEndPoint(tmp1, EndPoint);
				//Data->MyFunction->Arx->CreateLine(StartPoint,EndPoint,&tmp1);
				//ListObjects.append(tmp1);
				Data->MyFunction->Arx->GetObjectStartPoint(tmp2, StartPoint);

				Data->MyFunction->Arx->CreateLine(groove_mid11,groove_mid13 , &tmp1);
				Data->MyFunction->Arx->CreateFillet(tmp2, StartPoint , tmp1, groove_mid13, tmp.DCornerR , &tmp3);
				ListObjects.append(tmp3);
				ListObjects.append(tmp2);
				ListObjects.append(tmp1);

				Data->MyFunction->Arx->CreateArc(groove_mid15, tmp.InterstitialsR, 180.0, 360.0, &tmp1);
				AircylinderObjects.append(tmp1);

				ReListObjects = Data->MyFunction->Arx->CreateImage(ListObjects,AcGePoint3d(tmp.p30Dz,1,0),AcGePoint3d(tmp.p30Dz,0,0),0);
				AircylinderObjects.append(ListObjects);
				AircylinderObjects.append(ReListObjects);

				//叶根槽连接线
				if((!Data->MyFunction->Arx->IsPointSame(deepPoint, AcGePoint3d(0,0,0)))&&(next_tmp.HubType==1.0||next_tmp.HubType==4.0))
				{
					AcGePoint3d r_point = tmp.deep_point;
					r_point.x = r_point.x + tmp.p21Bz;

					AcGePoint3d r_point_qufeng = tmp.rator_deeppoint;
					r_point_qufeng.x = r_point_qufeng.x + tmp.p21Bz;
					
					//若有抽口则不连线单独勾画
					if(tmp.IsSuctionPort==1.0)
					{
						//动叶
						//创建顶点 叶片四边形轮廓 顺序逆时针
						AcGePoint3d vertex7 = AcGePoint3d(tmp.p30Dz + tmp.p15bg ,tmp.p28Dpd - tmp.p32, 0);
						//外围轮廓四点 顺序逆时针
						AcGePoint3d vertex_07 = AcGePoint3d(tmp.p30Dz + tmp.p22bz , vertex7.y-tmp.p23hz , 0);

						//求叶片交点
						AcDbObjectId tag_downr_ray_Id1,ray_downr_ID1;
						Data->MyFunction->Arx->CreateRay(vertex_07,AcGeVector3d(0,1,0) , &tag_downr_ray_Id1);

						//角度
						double angle_down = tmp.p25ag * PI /180.0;
						Data->MyFunction->Arx->CreateLine(vertex7,angle_down,1000.0,1, &ray_downr_ID1);

						//叶根点
						AcGePoint3d ids_down21=AcGePoint3d(0,0,0); 
						Data->MyFunction->Arx->GetObjectIntersect(tag_downr_ray_Id1,ray_downr_ID1,&ids_down21);

						//删除辅助线
						Data->MyFunction->Arx->DeleteObject(tag_downr_ray_Id1);
						Data->MyFunction->Arx->DeleteObject(ray_downr_ID1);


						//求叶片交点
						AcGePoint3d theP=AcGePoint3d(0,0,0);
						AcDbObjectId  ray_upl_ID;
						Data->MyFunction->Arx->CreateLine(ids_down21,angle_down,  100000.0,1, &ray_upl_ID);
						Data->MyFunction->Arx->CreateLine(deepPoint,AcGePoint3d(deepPoint.x-100000.0,deepPoint.y,deepPoint.z),&GeneralObjects);
						Data->MyFunction->Arx->GetObjectIntersect(ray_upl_ID,GeneralObjects,&theP);
						Data->MyFunction->Arx->DeleteObject(ray_upl_ID);
						Data->MyFunction->Arx->DeleteObject(GeneralObjects);

						//左基准点
						Data->MyFunction->Arx->CreateLine(ids_down21,theP, &GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
						Data->MyFunction->Arx->CreateLine(theP,deepPoint,  &GeneralObjects);
						AircylinderObjects.append(GeneralObjects);

					}
					//若下级折流则不连线
					else if(next_tmp.BrokenTag!=1.0)
					{
						//对比此槽深度与下一槽深度
						if(r_point.y == deepPoint.y)
						{
							Data->MyFunction->Arx->CreateLine(r_point, deepPoint,&GeneralObjects);
							AircylinderObjects.append(GeneralObjects);
						}
						else if(r_point.y < deepPoint.y)
						{
							Data->MyFunction->Arx->CreateLine(r_point, AcGePoint3d(r_point.x+0.2,r_point.y,0),&GeneralObjects);
							AircylinderObjects.append(GeneralObjects);
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point.x+0.2,r_point.y,0), AcGePoint3d(r_point.x+0.2,deepPoint.y,0),&GeneralObjects);
							AircylinderObjects.append(GeneralObjects);
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point.x+0.2,deepPoint.y,0), deepPoint,&GeneralObjects);
							AircylinderObjects.append(GeneralObjects);
						}
						else if(r_point.y > deepPoint.y)
						{
							Data->MyFunction->Arx->CreateLine(r_point, AcGePoint3d(deepPoint.x-0.2,r_point.y,0),&GeneralObjects);
							AircylinderObjects.append(GeneralObjects);
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(deepPoint.x-0.2,r_point.y,0), AcGePoint3d(deepPoint.x-0.2,deepPoint.y,0),&GeneralObjects);
							AircylinderObjects.append(GeneralObjects);
							Data->MyFunction->Arx->CreateLine(AcGePoint3d(deepPoint.x-0.2,deepPoint.y,0), deepPoint,&GeneralObjects);
							AircylinderObjects.append(GeneralObjects);
						}

					}
					//若下级折流则不画汽封齿槽
					if(next_tmp.BrokenTag!=1.0)
					{
						//勾画汽封齿槽
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point_qufeng.x+tmp.J,deepPoint.y,0), AcGePoint3d(r_point_qufeng.x+tmp.J,deepPoint.y-2,0),&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point_qufeng.x+tmp.J,deepPoint.y-2,0), AcGePoint3d(r_point_qufeng.x+tmp.J+tmp.Z_S,deepPoint.y-2,0),&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point_qufeng.x+tmp.J+tmp.Z_S,deepPoint.y-2,0), AcGePoint3d(r_point_qufeng.x+tmp.J+tmp.Z_S,deepPoint.y,0),&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
					}
				}
		}
#pragma endregion
		else if(hubtype == 2)
		//冲动式非预扭T叶根
#pragma region
		{
			//无角度
			if(tmp.NeckZAngle == 0.0)
			{
				//向下勾画动叶片叶根   从上下，从左到右
				AcGePoint3d rotor_p1,rotor_p2,rotor_p3,rotor_p4,rotor_p5,rotor_p6,rotor_p7,rotor_p8,rotor_p9,rotor_p10;
				rotor_p1 = AcGePoint3d(basePoint_l.x + (tmp.p21Bz-tmp.MidBodySurW)/2.0,basePoint_l.y,basePoint_l.z);
				rotor_p2 = AcGePoint3d(rotor_p1.x,rotor_p1.y+tmp.TSFH - tmp.RaceSFAngle ,rotor_p1.z);
				rotor_p3 = AcGePoint3d(rotor_p2.x+tmp.RaceSFAngle,rotor_p2.y+tmp.RaceSFAngle,rotor_p2.z);
				rotor_p4 = AcGePoint3d(rotor_p2.x+(tmp.MidBodySurW -tmp.NeckW)/2.0-tmp.NeckUAngle,rotor_p3.y,rotor_p3.z);
				rotor_p5 = AcGePoint3d(rotor_p4.x+tmp.NeckUAngle,rotor_p4.y-tmp.NeckUAngle,rotor_p4.z);
				
				double y6 = rotor_p1.y-tmp.NeckH +tmp.TSFH +(tmp.NeckDW-tmp.NeckW)/2.0*tan(tmp.TAngle * PI /180.0)+ tmp.NeckDAngle;
				rotor_p6 = AcGePoint3d(rotor_p5.x,y6 ,rotor_p5.z);

				double y7 = rotor_p1.y-tmp.NeckH +tmp.TSFH +(tmp.NeckDW-tmp.NeckW)/2.0*tan(tmp.TAngle * PI /180.0)- tmp.NeckDAngle*tan(tmp.TAngle * PI /180.0);
				rotor_p7 = AcGePoint3d(rotor_p6.x-tmp.NeckDAngle,y7,rotor_p6.z);

				rotor_p8 = AcGePoint3d(rotor_p6.x - (tmp.NeckDW-tmp.NeckW)/2.0,rotor_p1.y-tmp.NeckH +tmp.TSFH,rotor_p7.z);
				rotor_p9 = AcGePoint3d(rotor_p8.x,rotor_p8.y-tmp.NeckDH,rotor_p8.z);
				rotor_p10 = AcGePoint3d(tmp.p30Dz,rotor_p9.y,rotor_p9.z);

				AcDbObjectId rotor_id_tmp,rotor_id1,rotor_id2,rotor_id3,rotor_id4,rotor_id5;
				//连接 陆续从左到右从上到下
				Data->MyFunction->Arx->CreateLine(basePoint_l,rotor_p1,  &rotor_id1);
				Data->MyFunction->Arx->CreateLine(rotor_p1 ,rotor_p2,  &rotor_id2);
				Data->MyFunction->Arx->CreateFillet(rotor_id1, basePoint_l , rotor_id2, rotor_p2, tmp.RaceSFCornerR, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id1);
				AircylinderObjects.append(rotor_id1);
				ListObjects.append(rotor_id2);
				AircylinderObjects.append(rotor_id2);

				Data->MyFunction->Arx->CreateLine(rotor_p2 ,rotor_p3,  &GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(rotor_p3 ,rotor_p4,  &GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(rotor_p4 ,rotor_p5,  &GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(rotor_p5 ,rotor_p6,  &GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(rotor_p6 ,rotor_p7,  &GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);

				Data->MyFunction->Arx->CreateLine(rotor_p7 ,rotor_p8,  &rotor_id3);
				Data->MyFunction->Arx->CreateLine(rotor_p8 ,rotor_p9,  &rotor_id4);
				Data->MyFunction->Arx->CreateFillet(rotor_id3, rotor_p7 , rotor_id4, rotor_p9, tmp.RaceUCornerR, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id3);
				AircylinderObjects.append(rotor_id3);

				Data->MyFunction->Arx->GetObjectStartPoint(rotor_id4, StartPoint);
				Data->MyFunction->Arx->CreateLine(rotor_p9 ,rotor_p10,  &rotor_id5);
				Data->MyFunction->Arx->CreateFillet(rotor_id4, StartPoint , rotor_id5, rotor_p10, tmp.RaceDCornerR, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id4);
				AircylinderObjects.append(rotor_id4);
				ListObjects.append(rotor_id5);
				AircylinderObjects.append(rotor_id5);
			}
			//有角度
			else
			{
				//向下勾画动叶片叶根   从上下，从左到右
				AcGePoint3d rotor_p1,rotor_p2,rotor_p3,rotor_p4,rotor_p5,rotor_p6,rotor_p7,rotor_p8,rotor_p9,rotor_p10;
				rotor_p1 = AcGePoint3d(tmp.p30Dz - tmp.NeckUestW/2.0,basePoint_l.y,basePoint_l.z);
				rotor_p2 = AcGePoint3d(tmp.p30Dz - tmp.NeckW/2.0,rotor_p1.y-(tmp.NeckUestW-tmp.NeckW)/2.0/tan(tmp.NeckZAngle * PI /180.0),rotor_p1.z);

				double y5 = rotor_p1.y-tmp.NeckH +tmp.TSFH +(tmp.NeckDW-tmp.NeckW)/2.0*tan(tmp.TAngle * PI /180.0);
				rotor_p3 = AcGePoint3d(rotor_p2.x,y5+tmp.NeckDAngle,rotor_p2.z);
				rotor_p4 = AcGePoint3d(rotor_p2.x-tmp.NeckDAngle,y5-tmp.NeckDAngle*tan(tmp.TAngle * PI /180.0),rotor_p2.z);

				double y6 = rotor_p1.y-tmp.NeckH +tmp.TSFH ;
				rotor_p6 = AcGePoint3d(rotor_p3.x-(tmp.NeckDW -tmp.NeckW)/2.0,y6,rotor_p3.z);
				rotor_p7 = AcGePoint3d(rotor_p6.x,rotor_p6.y-tmp.NeckDH,rotor_p6.z);
				rotor_p8 = AcGePoint3d(tmp.p30Dz,rotor_p7.y,rotor_p7.z);
				
				AcDbObjectId rotor_id_tmp,rotor_id1,rotor_id2,rotor_id3,rotor_id4,rotor_id5,rotor_id6;
				//连接 陆续从左到右从上到下
				Data->MyFunction->Arx->CreateLine(basePoint_l,rotor_p1,  &rotor_id1);
				Data->MyFunction->Arx->CreateLine(rotor_p1 ,rotor_p2,  &rotor_id2);
				Data->MyFunction->Arx->CreateFillet(rotor_id1, basePoint_l , rotor_id2, rotor_p2, tmp.NeckUAngle, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id1);
				AircylinderObjects.append(rotor_id1);

				Data->MyFunction->Arx->GetObjectStartPoint(rotor_id2, StartPoint);
				Data->MyFunction->Arx->CreateLine(rotor_p2 ,rotor_p3,  &rotor_id3);
				Data->MyFunction->Arx->CreateFillet(rotor_id2, StartPoint , rotor_id3, rotor_p3, tmp.RaceNeckMCornerR, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id2);
				AircylinderObjects.append(rotor_id2);
				Data->MyFunction->Arx->CreateLine(rotor_p1,StartPoint,  &GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);

				Data->MyFunction->Arx->CreateLine(rotor_p3,rotor_p4 ,&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);

				Data->MyFunction->Arx->CreateLine(rotor_p4,rotor_p6 ,&rotor_id4);
				Data->MyFunction->Arx->CreateLine(rotor_p6,rotor_p7 ,&rotor_id5);
				Data->MyFunction->Arx->CreateFillet(rotor_id4, rotor_p4 , rotor_id5, rotor_p7, tmp.RaceUCornerR, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id4);
				AircylinderObjects.append(rotor_id4);

				Data->MyFunction->Arx->GetObjectStartPoint(rotor_id5, StartPoint);
				Data->MyFunction->Arx->CreateLine(rotor_p7 ,rotor_p8,  &rotor_id6);
				Data->MyFunction->Arx->CreateFillet(rotor_id5, StartPoint , rotor_id6, rotor_p8, tmp.RaceDCornerR, &rotor_id_tmp);
				ListObjects.append(rotor_id_tmp);
				AircylinderObjects.append(rotor_id_tmp);
				ListObjects.append(rotor_id5);
				AircylinderObjects.append(rotor_id5);
				ListObjects.append(rotor_id6);
				AircylinderObjects.append(rotor_id6);
			}

			ReListObjects = Data->MyFunction->Arx->CreateImage(ListObjects,AcGePoint3d(tmp.p30Dz,1,0),AcGePoint3d(tmp.p30Dz,0,0),0);
			AircylinderObjects.append(ListObjects);
			AircylinderObjects.append(ReListObjects);

		}
#pragma endregion
		else if(hubtype == 3)
		//非预扭双倒T叶根
#pragma region
		{
			//向下勾画动叶片叶根   从上下，从左到右
			AcGePoint3d rotor_p1,rotor_p2,rotor_p3,rotor_p4,rotor_p5,rotor_p6,rotor_p7,rotor_p8,rotor_p9,rotor_p10,rotor_p11,rotor_p12,rotor_p13,rotor_p14,rotor_p15,rotor_p16;
			rotor_p1 = AcGePoint3d(tmp.p30Dz - tmp.b1/2.0,basePoint_l.y,0);
			rotor_p2 = AcGePoint3d(rotor_p1.x ,rotor_p1.y+tmp.h-1.0 ,0);
			rotor_p3 = AcGePoint3d(rotor_p2.x+1.0 ,rotor_p2.y+1.0 ,0);
			rotor_p4 = AcGePoint3d(rotor_p2.x+(tmp.b1-tmp.lb2)/2.0-0.5 ,rotor_p3.y ,0);
			rotor_p5 = AcGePoint3d(rotor_p4.x+0.5 ,rotor_p4.y-0.5 ,0);

			rotor_p6 = AcGePoint3d(rotor_p5.x ,rotor_p4.y-tmp.h2+tmp.d2 ,0);
			rotor_p7 = AcGePoint3d(rotor_p6.x-tmp.d2 ,rotor_p6.y-tmp.d2 ,0);

			rotor_p8 = AcGePoint3d(tmp.p30Dz-tmp.lb3/2.0 ,rotor_p7.y ,0);
			rotor_p9 = AcGePoint3d(rotor_p8.x ,rotor_p8.y-tmp.h3 ,0);

			rotor_p10 = AcGePoint3d(tmp.p30Dz-tmp.lb4/2.0 ,rotor_p9.y-tmp.h4 ,0);

			rotor_p11 = AcGePoint3d(rotor_p10.x ,rotor_p10.y-tmp.h5+tmp.d4 ,0);
			rotor_p12 = AcGePoint3d(rotor_p11.x-tmp.d4 ,rotor_p11.y-tmp.d4 ,0);
			rotor_p13 = AcGePoint3d(tmp.p30Dz-tmp.lb5/2.0 ,rotor_p12.y  ,0);
			rotor_p14 = AcGePoint3d(rotor_p13.x,rotor_p13.y-tmp.h6 ,0);
			rotor_p15 = AcGePoint3d(rotor_p14.x+tmp.h7 ,rotor_p14.y-tmp.h7-1.5 ,0);
			rotor_p16 = AcGePoint3d(tmp.p30Dz ,rotor_p15.y,0);

			AcDbObjectId rotor_id_tmp,rotor_id1,rotor_id2,rotor_id3,rotor_id4,rotor_id5,rotor_id6,rotor_id7,rotor_id8,rotor_id9,rotor_id10;
			Data->MyFunction->Arx->CreateLine(basePoint_l,rotor_p1,  &rotor_id1);
			Data->MyFunction->Arx->CreateLine(rotor_p1 ,rotor_p2,  &rotor_id2);
			Data->MyFunction->Arx->CreateFillet(rotor_id1, basePoint_l , rotor_id2, rotor_p2, 0.5, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id1);
			AircylinderObjects.append(rotor_id1);
			ListObjects.append(rotor_id2);
			AircylinderObjects.append(rotor_id2);

			Data->MyFunction->Arx->CreateLine(rotor_p2,rotor_p3,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p3,rotor_p4,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p4,rotor_p5,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p5,rotor_p6,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p6,rotor_p7,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			Data->MyFunction->Arx->CreateLine(rotor_p7,rotor_p8,  &rotor_id3);
			Data->MyFunction->Arx->CreateLine(rotor_p8 ,rotor_p9,  &rotor_id4);
			Data->MyFunction->Arx->CreateFillet(rotor_id3, rotor_p7 , rotor_id4, rotor_p9, tmp.r2, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id3);
			AircylinderObjects.append(rotor_id3);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id4, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p9 ,rotor_p10,  &rotor_id5);
			Data->MyFunction->Arx->CreateFillet(rotor_id4, StartPoint , rotor_id5, rotor_p10, tmp.r2, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id4);
			AircylinderObjects.append(rotor_id4);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id5, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p10 ,rotor_p11,  &rotor_id6);
			Data->MyFunction->Arx->CreateFillet(rotor_id5, StartPoint , rotor_id6, rotor_p11, 2.0, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id5);
			AircylinderObjects.append(rotor_id5);
			ListObjects.append(rotor_id6);
			AircylinderObjects.append(rotor_id6);

			Data->MyFunction->Arx->CreateLine(rotor_p11,rotor_p12,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			Data->MyFunction->Arx->CreateLine(rotor_p12,rotor_p13,  &rotor_id7);
			Data->MyFunction->Arx->CreateLine(rotor_p13,rotor_p14,  &rotor_id8);
			Data->MyFunction->Arx->CreateFillet(rotor_id7, rotor_p12 , rotor_id8, rotor_p14, tmp.r4, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id7);
			AircylinderObjects.append(rotor_id7);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id8, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p14 ,rotor_p15,  &rotor_id9);
			Data->MyFunction->Arx->CreateFillet(rotor_id8, StartPoint , rotor_id9, rotor_p15, 2.0, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id8);
			AircylinderObjects.append(rotor_id8);
			
			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id9, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p15 ,rotor_p16,  &rotor_id10);
			Data->MyFunction->Arx->CreateFillet(rotor_id9, StartPoint , rotor_id10, rotor_p16, 1.2, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id9);
			AircylinderObjects.append(rotor_id9);
			ListObjects.append(rotor_id10);
			AircylinderObjects.append(rotor_id10);

			ReListObjects = Data->MyFunction->Arx->CreateImage(ListObjects,AcGePoint3d(tmp.p30Dz,1,0),AcGePoint3d(tmp.p30Dz,0,0),0);
			AircylinderObjects.append(ListObjects);
			AircylinderObjects.append(ReListObjects);
		}
#pragma endregion
		else if(hubtype == 4)
		//预扭双倒T叶根
#pragma region
		{
			//向下勾画动叶片叶根   从上下，从左到右
			AcGePoint3d rotor_p1,rotor_p2,rotor_p3,rotor_p4,rotor_p5,rotor_p6,rotor_p7,rotor_p8,rotor_p9,rotor_p10,rotor_p11,rotor_p12,rotor_p13,rotor_p14,rotor_p15,rotor_p16;
			rotor_p1 = AcGePoint3d(basePoint_l.x,basePoint_l.y+0.3,0);
			rotor_p2 = AcGePoint3d(basePoint_l.x+0.3 ,basePoint_l.y ,0);

			Data->MyFunction->Arx->CreateLine(rotor_p1,rotor_p2,  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(AcGePoint3d(basePoint_r.x,basePoint_r.y +0.3,0),AcGePoint3d(basePoint_r.x-0.3,basePoint_r.y,0),  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			rotor_p3 = AcGePoint3d(basePoint_l.x + (tmp.b1-tmp.lb2)/2.0-tmp.d2,rotor_p2.y ,0);
			rotor_p4 = AcGePoint3d(rotor_p3.x+tmp.d2 ,rotor_p3.y -tmp.d2,0);
			rotor_p5 = AcGePoint3d(rotor_p4.x , rotor_p3.y-tmp.h2+tmp.d2 ,0);
			rotor_p6 = AcGePoint3d(rotor_p5.x-tmp.d2 ,rotor_p5.y-tmp.d2 ,0);
			rotor_p7 = AcGePoint3d(tmp.p30Dz-tmp.lb3/2.0 ,rotor_p6.y ,0);
			rotor_p8 = AcGePoint3d(rotor_p7.x,rotor_p7.y-tmp.h3-tmp.h4 ,0);
			rotor_p9 = AcGePoint3d(tmp.p30Dz-tmp.lb4/2.0-tmp.d4 ,rotor_p8.y ,0);
			rotor_p10 = AcGePoint3d(rotor_p9.x +tmp.d4,rotor_p9.y-tmp.d4 ,0);
			rotor_p11 = AcGePoint3d(rotor_p10.x ,rotor_p9.y-tmp.h5+tmp.d4 ,0);
			rotor_p12 = AcGePoint3d(rotor_p11.x-tmp.d4 ,rotor_p11.y-tmp.d4 ,0);
			rotor_p13 = AcGePoint3d(tmp.p30Dz-tmp.lb5/2.0 ,rotor_p12.y ,0);
			rotor_p14 = AcGePoint3d(rotor_p13.x ,rotor_p13.y-tmp.h6-tmp.h7 ,0);
			rotor_p15 = AcGePoint3d(tmp.p30Dz-tmp.p21Bz*5.0/50.0 ,rotor_p14.y-0.5 ,0);//暂时半径
			rotor_p16 = AcGePoint3d(tmp.p30Dz ,rotor_p15.y ,0);

			AcDbObjectId rotor_id_tmp,rotor_id1,rotor_id2,rotor_id3,rotor_id4,rotor_id5,rotor_id6,rotor_id7,rotor_id8,rotor_id9,rotor_id10;
			Data->MyFunction->Arx->CreateLine(rotor_p2,rotor_p3,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p3,rotor_p4,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p4,rotor_p5,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p5,rotor_p6,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			Data->MyFunction->Arx->CreateLine(rotor_p6,rotor_p7,  &rotor_id1);
			Data->MyFunction->Arx->CreateLine(rotor_p7,rotor_p8,  &rotor_id2);
			Data->MyFunction->Arx->CreateFillet(rotor_id1, rotor_p6 , rotor_id2, rotor_p8, tmp.r2, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id1);
			AircylinderObjects.append(rotor_id1);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id2, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p8 ,rotor_p9,  &rotor_id3);
			Data->MyFunction->Arx->CreateFillet(rotor_id2, StartPoint , rotor_id3, rotor_p9, tmp.p21Bz*8.0/50.0, &rotor_id_tmp);//半径暂时
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id2);
			AircylinderObjects.append(rotor_id2);
			ListObjects.append(rotor_id3);
			AircylinderObjects.append(rotor_id3);

			Data->MyFunction->Arx->CreateLine(rotor_p9,rotor_p10,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p10,rotor_p11,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_p11,rotor_p12,  &GeneralObjects);
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			Data->MyFunction->Arx->CreateLine(rotor_p12,rotor_p13,  &rotor_id4);
			Data->MyFunction->Arx->CreateLine(rotor_p13,rotor_p14,  &rotor_id5);
			Data->MyFunction->Arx->CreateFillet(rotor_id4, rotor_p12 , rotor_id5, rotor_p14, tmp.r4, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id4);
			AircylinderObjects.append(rotor_id4);

			Data->MyFunction->Arx->GetObjectStartPoint(rotor_id5, StartPoint);
			Data->MyFunction->Arx->CreateLine(rotor_p14 ,rotor_p15,  &rotor_id6);
			Data->MyFunction->Arx->CreateFillet(rotor_id5, StartPoint , rotor_id6, rotor_p15,tmp.p21Bz*14.0/50.0, &rotor_id_tmp);
			ListObjects.append(rotor_id_tmp);
			AircylinderObjects.append(rotor_id_tmp);
			ListObjects.append(rotor_id5);
			AircylinderObjects.append(rotor_id5);
			ListObjects.append(rotor_id6);
			AircylinderObjects.append(rotor_id6);

			Data->MyFunction->Arx->CreateArc(rotor_p16, tmp.p21Bz*5.0/50.0, 180.0, 270.0, &GeneralObjects);//暂时半径
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			ReListObjects = Data->MyFunction->Arx->CreateImage(ListObjects,AcGePoint3d(tmp.p30Dz,1,0),AcGePoint3d(tmp.p30Dz,0,0),0);
			AircylinderObjects.append(ListObjects);
			AircylinderObjects.append(ReListObjects);

			//叶根槽连接线
			if((!Data->MyFunction->Arx->IsPointSame(deepPoint, AcGePoint3d(0,0,0)))&&(next_tmp.HubType==1.0||next_tmp.HubType==4.0))
			{
				AcGePoint3d r_point = tmp.deep_point;
				r_point.x = r_point.x + tmp.p21Bz;

				AcGePoint3d r_point_qufeng = tmp.rator_deeppoint;
				r_point_qufeng.x = r_point_qufeng.x + tmp.p21Bz;
				
				//若有抽口则不连线单独勾画
				if(tmp.IsSuctionPort==1.0)
				{
					//动叶
					//创建顶点 叶片四边形轮廓 顺序逆时针
					AcGePoint3d vertex7 = AcGePoint3d(tmp.p30Dz + tmp.p15bg ,tmp.p28Dpd - tmp.p32, 0);
					//外围轮廓四点 顺序逆时针
					AcGePoint3d vertex_07 = AcGePoint3d(tmp.p30Dz + tmp.p22bz , vertex7.y-tmp.p23hz , 0);

					//求叶片交点
					AcDbObjectId tag_downr_ray_Id1,ray_downr_ID1;
					Data->MyFunction->Arx->CreateRay(vertex_07,AcGeVector3d(0,1,0) , &tag_downr_ray_Id1);

					//角度
					double angle_down = tmp.p25ag * PI /180.0;
					Data->MyFunction->Arx->CreateLine(vertex7,angle_down,1000.0,1, &ray_downr_ID1);

					//叶根点
					AcGePoint3d ids_down21=AcGePoint3d(0,0,0); 
					Data->MyFunction->Arx->GetObjectIntersect(tag_downr_ray_Id1,ray_downr_ID1,&ids_down21);

					//删除辅助线
					Data->MyFunction->Arx->DeleteObject(tag_downr_ray_Id1);
					Data->MyFunction->Arx->DeleteObject(ray_downr_ID1);


					//求叶片交点
					AcGePoint3d theP=AcGePoint3d(0,0,0);
					AcDbObjectId  ray_upl_ID;
					Data->MyFunction->Arx->CreateLine(ids_down21,angle_down,  100000.0,1, &ray_upl_ID);
					Data->MyFunction->Arx->CreateLine(deepPoint,AcGePoint3d(deepPoint.x-100000.0,deepPoint.y,deepPoint.z),&GeneralObjects);
					Data->MyFunction->Arx->GetObjectIntersect(ray_upl_ID,GeneralObjects,&theP);
					Data->MyFunction->Arx->DeleteObject(ray_upl_ID);
					Data->MyFunction->Arx->DeleteObject(GeneralObjects);

					//左基准点
					Data->MyFunction->Arx->CreateLine(ids_down21,theP, &GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(theP,deepPoint,  &GeneralObjects);
					AircylinderObjects.append(GeneralObjects);

				}
				//若下级折流则不连线且把所有叶片根据条件镜像之后清空
				else if(next_tmp.BrokenTag!=1.0)
				{
					//对比此槽深度与下一槽深度
					if(r_point.y == deepPoint.y)
					{
						Data->MyFunction->Arx->CreateLine(r_point, deepPoint,&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
					}
					else if(r_point.y < deepPoint.y)
					{
						Data->MyFunction->Arx->CreateLine(r_point, AcGePoint3d(r_point.x+0.2,r_point.y,0),&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point.x+0.2,r_point.y,0), AcGePoint3d(r_point.x+0.2,deepPoint.y,0),&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point.x+0.2,deepPoint.y,0), deepPoint,&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
					}
					else if(r_point.y > deepPoint.y)
					{
						Data->MyFunction->Arx->CreateLine(r_point, AcGePoint3d(deepPoint.x-0.2,r_point.y,0),&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(deepPoint.x-0.2,r_point.y,0), AcGePoint3d(deepPoint.x-0.2,deepPoint.y,0),&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
						Data->MyFunction->Arx->CreateLine(AcGePoint3d(deepPoint.x-0.2,deepPoint.y,0), deepPoint,&GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
					}

				}
				//若下级折流则不画汽封齿槽
				if(next_tmp.BrokenTag!=1.0)
				{
					//勾画汽封齿槽
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point_qufeng.x+tmp.J,deepPoint.y,0), AcGePoint3d(r_point_qufeng.x+tmp.J,deepPoint.y-2,0),&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point_qufeng.x+tmp.J,deepPoint.y-2,0), AcGePoint3d(r_point_qufeng.x+tmp.J+tmp.Z_S,deepPoint.y-2,0),&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point_qufeng.x+tmp.J+tmp.Z_S,deepPoint.y-2,0), AcGePoint3d(r_point_qufeng.x+tmp.J+tmp.Z_S,deepPoint.y,0),&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
				}
			}
		}
#pragma endregion
	}
	return 0;
}


//创建末三级                                        /*末三级类型名称，    原点,    是否镜像       镜像轴       是否删除原始          镜像vec*/
int ClassTongliu::createLastThreeStages( string TypeName, AcGePoint3d originP,double Image,double AxisNum,int isDelete, AcDbObjectIdArray &AircylinderObjects )
{
	AcDbObjectId ObjectId;
	Data->MyFunction->Arx->InsertBlock(TypeName.c_str(),originP, ObjectId);
	AircylinderObjects.append(ObjectId);

	Data->MyFunction->Arx->SetNowLayerName(L"5尺寸线");
	AcGePoint3d p1,p2,p3,p4;
	CString Ctext;
	double c_num;
	//末三级做标注  从下到上 从左到右
	if(TypeName == "MSJ-179-9")
	{
#pragma region
		p1 = AcGePoint3d(originP.x+451.4,originP.y-325.75,0);
		p2 = AcGePoint3d(originP.x+905.4,originP.y+446.47,0);
		p3 = AcGePoint3d(originP.x+679.03,originP.y-500.12,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"454",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+364.93,originP.y+349.11,0);
		p2 = AcGePoint3d(originP.x+315.06,originP.y+342.25,0);
		Data->MyFunction->Arx->CreateRadiusDim( p1,p2,  20, "R50",Image,AxisNum,isDelete);
		
		p1 = AcGePoint3d(originP.x+547.21,originP.y+363.41,0);
		p2 = AcGePoint3d(originP.x+588.83,originP.y+348.83,0);
		Data->MyFunction->Arx->CreateRadiusDim( p1,p2,  20, "R100",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+286.28,originP.y+402.96,0);
		p2 = AcGePoint3d(originP.x+451.4,originP.y+402.96,0);
		p3 = AcGePoint3d(originP.x+368.53,originP.y+402.96+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"164.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+451.4,originP.y+402.96,0);
		p2 = AcGePoint3d(originP.x+584.4,originP.y+318.7,0);
		p3 = AcGePoint3d(originP.x+517.9,originP.y+402.96+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"133",Image,AxisNum,isDelete);

		p1 =AcGePoint3d(originP.x+584.4,originP.y+318.7,0);
		p2 = AcGePoint3d(originP.x+615.9,originP.y+402.96,0);
		p3 = AcGePoint3d(originP.x+600.15,originP.y+402.96+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"31.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+295.4,originP.y+449.42,0);
		p2 = AcGePoint3d(originP.x+607.4,originP.y+449.42,0);
		p3 = AcGePoint3d(originP.x+451.4,originP.y+449.42+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"312",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+607.4,originP.y+458.8,0);
		p2 = AcGePoint3d(originP.x+630.59,originP.y+482.78,0);
		p3 = AcGePoint3d(originP.x+619,originP.y+458.8+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"23.19",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+451.4,originP.y+477.05,0);
		p2 = AcGePoint3d(originP.x+607.4,originP.y+477.05,0);
		p3 = AcGePoint3d(originP.x+529.4,originP.y+477.05+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"156",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+642.98,originP.y+517.78,0);
		p2 = AcGePoint3d(originP.x+732.6,originP.y+517.78,0);
		p3 = AcGePoint3d(originP.x+687.48,originP.y+493.72+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"89.01",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+779.59,originP.y+493.72,0);
		p2 = AcGePoint3d(originP.x+846.9,originP.y+493.72,0);
		p3 = AcGePoint3d(originP.x+813.25,originP.y+493.72+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"67.31",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+335.56,originP.y+508,0);
		p2 = AcGePoint3d(originP.x+451.4,originP.y+508,0);
		p3 = AcGePoint3d(originP.x+378.95,originP.y+508+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"115.84",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+335.56,originP.y+508,0);
		p2 = AcGePoint3d(originP.x+570.72,originP.y+508,0);
		p3 = AcGePoint3d(originP.x+421.48,originP.y+562.22+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"235.16",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+630.59,originP.y+515.49,0);
		p2 = AcGePoint3d(originP.x+643.59,originP.y+517.78,0);
		p3 = AcGePoint3d(originP.x+673.8,originP.y+550.73+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"13",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+643.59,originP.y+517.78,0);
		p2 = AcGePoint3d(originP.x+759.87,originP.y+517.78,0);
		p3 = AcGePoint3d(originP.x+758.1,originP.y+537.87,0);
		p4 = AcGePoint3d(originP.x+759.43,originP.y+527.91,0);
		Ctext.Format(L"10%%%%d");
		Data->MyFunction->Arx->CreateAngleDim(p1,p2,p3,p4,Ctext,Image,AxisNum,isDelete);


		p1 = AcGePoint3d(originP.x+960.4,originP.y+538.71,0);
		p2 = AcGePoint3d(originP.x+979.4,originP.y+538.71,0);
		p3 = AcGePoint3d(originP.x+969.9,originP.y+538.71+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"19",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+312.21,originP.y+508,0);
		p2 = AcGePoint3d(originP.x+432.9,originP.y+1288,0);
		p3 = AcGePoint3d(originP.x+312.21-3,originP.y+898,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"780",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+538.84,originP.y+898,0);
		p2 = AcGePoint3d(originP.x+579.95,originP.y+898,0);
		Data->MyFunction->Arx->CreateLine(p1, p2, &ObjectId);
		AircylinderObjects.append(ObjectId);
		c_num = originP.y - setting.OriginalPoint.y + 898.0;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+566.7,originP.y+898,0);
		p2 = AcGePoint3d(originP.x+566.7,originP.y+759.2,0);
		p3 = AcGePoint3d(originP.x+566.7,originP.y+822.6,0);
		Data->MyFunction->Arx->CreateLeader(p1, p3,p2,  Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+643.59,originP.y+517.78,0);
		p2 = AcGePoint3d(originP.x+633.28,originP.y+1112.78,0);
		p3 = AcGePoint3d(originP.x+593.5-3,originP.y+815.28,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"595",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+633.29,originP.y+1112.78,0);
		p2 = AcGePoint3d(originP.x+643.59,originP.y+517.78,0);
		p3 = AcGePoint3d(originP.x+688.34,originP.y+659.81+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"10.31",Image,AxisNum,isDelete);

		c_num = originP.y - setting.OriginalPoint.y + 815.25;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+716.64,originP.y+815.25,0);
		p2 = AcGePoint3d(originP.x+716.64,originP.y+689.63,0);
		p3 = AcGePoint3d(originP.x+716.64,originP.y+746.44,0);
		Data->MyFunction->Arx->CreateLeader(p1, p3,p2, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+850.4,originP.y+548,0);
		p2 = AcGePoint3d(originP.x+863.64,originP.y+898,0);
		p3 = AcGePoint3d(originP.x+837.44-3,originP.y+720.5,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"350",Image,AxisNum,isDelete);

		c_num = originP.y - setting.OriginalPoint.y + 723;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+938.57,originP.y+723,0);
		p2 = AcGePoint3d(originP.x+938.57,originP.y+646.54,0);
		p3 = AcGePoint3d(originP.x+938.57,originP.y+709.25,0);
		Data->MyFunction->Arx->CreateLeader(p1,p3, p2,  Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+979.4,originP.y+549.25,0);
		p2 = AcGePoint3d(originP.x+983.43,originP.y+869.25,0);
		p3 = AcGePoint3d(originP.x+971.58-3,originP.y+709.25,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"320",Image,AxisNum,isDelete);

		c_num = originP.y - setting.OriginalPoint.y + 709.25;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+1045.57,originP.y+709.25,0);
		p2 = AcGePoint3d(originP.x+1045.57,originP.y+668.13,0);
		p3 = AcGePoint3d(originP.x+1045.57,originP.y+639.01,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2,p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+940.42,originP.y+908.55,0);
		p2 = AcGePoint3d(originP.x+967.4,originP.y+908.55,0);
		p3 = AcGePoint3d(originP.x+953.91,originP.y+908.55+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"26.98",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+983.42,originP.y+869.25,0);
		p2 = AcGePoint3d(originP.x+1063.14,originP.y+869.25,0);
		p3 = AcGePoint3d(originP.x+1023.28,originP.y+880,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"79.72",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+263.82,originP.y+1304,0);
		p2 = AcGePoint3d(originP.x+432.9,originP.y+1304,0);
		Data->MyFunction->Arx->CreateLine(p1, p2, &ObjectId);
		AircylinderObjects.append(ObjectId);
		c_num = originP.y - setting.OriginalPoint.y + 1304.0;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+276,originP.y+1304.0,0);
		p2 = AcGePoint3d(originP.x+276,originP.y+1304.0-40,0);
		p3 = AcGePoint3d(originP.x+276,originP.y+1304.0-80,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+434.98,originP.y+1288,0);
		p2 = AcGePoint3d(originP.x+451.4,originP.y+1288,0);
		p3 = AcGePoint3d(originP.x+443.73,originP.y+1288-10,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"16.42",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+434.98,originP.y+1288,0);
		p2 = AcGePoint3d(originP.x+467.95,originP.y+1288,0);
		p3 = AcGePoint3d(originP.x+450.9,originP.y+1288-30,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"32.97",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+432.9,originP.y+1288,0);
		p2 = AcGePoint3d(originP.x+468.9,originP.y+1288,0);
		p3 = AcGePoint3d(originP.x+450.9,originP.y+1288+5,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"36",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+468.9,originP.y+1288,0);
		p2 = AcGePoint3d(originP.x+526.39,originP.y+1288,0);
		p3 = AcGePoint3d(originP.x+497.65,originP.y+1288+5,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"57.49",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+633.29,originP.y+1112.16,0);
		p2 = AcGePoint3d(originP.x+768.45,originP.y+1112.16,0);
		p3 = AcGePoint3d(originP.x+700.87,originP.y+1137.63,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"135.17",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+633.29,originP.y+1112.78,0);
		p2 = AcGePoint3d(originP.x+707.2,originP.y+1036.25,0);
		p3 = AcGePoint3d(originP.x+739.68,originP.y+1112.78,0);
		p4 = AcGePoint3d(originP.x+731.22,originP.y+1071.21,0);
		Ctext.Format(L"46%%%%d");
		Data->MyFunction->Arx->CreateAngleDim(p1,p2,p3,p4,Ctext,Image,AxisNum,isDelete);
#pragma endregion
	} 
	else if(TypeName == "MSJ-179-15")
	{
#pragma region
		p1 = AcGePoint3d(originP.x+236.08,originP.y+430.5,0);
		p2 = AcGePoint3d(originP.x+331.68,originP.y+112.38,0);
		p3 = AcGePoint3d(originP.x+283.88,originP.y+112.38+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"95.6",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+229.18,originP.y+400.9,0);
		p2 = AcGePoint3d(originP.x+434.18,originP.y+400.9,0);
		p3 = AcGePoint3d(originP.x+331.68,originP.y+400.9+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"205",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+667.68,originP.y+400.15,0);
		p2 = AcGePoint3d(originP.x+731.68,originP.y+378.02,0);
		p3 = AcGePoint3d(originP.x+699.68,originP.y+378.02+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"64",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+731.68,originP.y+378.02,0);
		p2 = AcGePoint3d(originP.x+795.68,originP.y+400.15,0);
		p3 = AcGePoint3d(originP.x+763.68,originP.y+378.02+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"64",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+667.68,originP.y+400.15,0);
		p2 = AcGePoint3d(originP.x+795.68,originP.y+400.15,0);
		p3 = AcGePoint3d(originP.x+731.68,originP.y+395.45+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"128",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+952.18,originP.y+400.15,0);
		p2 = AcGePoint3d(originP.x+1007.18,originP.y+378.27,0);
		p3 = AcGePoint3d(originP.x+979.68,originP.y+378.27+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"55",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+1007.18,originP.y+378.27,0);
		p2 = AcGePoint3d(originP.x+1062.18,originP.y+400.15,0);
		p3 = AcGePoint3d(originP.x+1034.68,originP.y+378.27+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"55",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+952.18,originP.y+400.15,0);
		p2 = AcGePoint3d(originP.x+1062.18,originP.y+400.15,0);
		p3 = AcGePoint3d(originP.x+1007.18,originP.y+400.15+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"110",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+474.18,originP.y+470.71,0);
		p2 = AcGePoint3d(originP.x+569.37,originP.y+430.5,0);
		p3 = AcGePoint3d(originP.x+521.78,originP.y+470.71+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"95.194",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+669.18,originP.y+430.5,0);
		p2 = AcGePoint3d(originP.x+731.68,originP.y+455.99,0);
		p3 = AcGePoint3d(originP.x+700.43,originP.y+456.99+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"62.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+955.38,originP.y+430.5,0);
		p2 = AcGePoint3d(originP.x+1007.18,originP.y+481.2,0);
		p3 = AcGePoint3d(originP.x+981.28,originP.y+481.2+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"51.8",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+1078.68,originP.y+737.5,0);
		p2 = AcGePoint3d(originP.x+1089.18,originP.y+430.5,0);
		p3 = AcGePoint3d(originP.x+1119.3,originP.y+476.43+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"12.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+1089.18,originP.y+430.5,0);
		p2 = AcGePoint3d(originP.x+1143.19,originP.y+430.5,0);
		p3 = AcGePoint3d(originP.x+1116.19,originP.y+448.09+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"54.1",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+188.77,originP.y+880.5,0);
		p2 = AcGePoint3d(originP.x+250.17,originP.y+880.5,0);
		Data->MyFunction->Arx->CreateLine(p1, p2, &ObjectId);
		AircylinderObjects.append(ObjectId);
		c_num = originP.y - setting.OriginalPoint.y + 191.77;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+191.77,originP.y+880.5,0);
		p2 = AcGePoint3d(originP.x+191.77,originP.y+880.5-40,0);
		p3 = AcGePoint3d(originP.x+191.77,originP.y+880.5-80,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+310.28,originP.y+1330.5,0);
		p2 = AcGePoint3d(originP.x+331.68,originP.y+1309.61,0);
		p3 = AcGePoint3d(originP.x+366,originP.y+1309.61+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"21.45",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+310.28,originP.y+1330.5,0);
		p2 = AcGePoint3d(originP.x+352.16,originP.y+1330.5,0);
		p3 = AcGePoint3d(originP.x+376.67,originP.y+1301.35+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"40.97",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+406.47,originP.y+430.5,0);
		p2 = AcGePoint3d(originP.x+351.2,originP.y+1330.5,0);
		p3 = AcGePoint3d(originP.x+406.47-3,originP.y+880.5,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"900",Image,AxisNum,isDelete);

		c_num = originP.y - setting.OriginalPoint.y + 825.5;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+450.38,originP.y+825.5,0);
		p2 = AcGePoint3d(originP.x+450.38,originP.y+825.5-40,0);
		p3 = AcGePoint3d(originP.x+450.38,originP.y+825.5-80,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+622.26,originP.y+430.5,0);
		p2 = AcGePoint3d(originP.x+639.96,originP.y+1220.5,0);
		p3 = AcGePoint3d(originP.x+639.96-3,originP.y+825.5,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"790.003",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+473.63,originP.y+1220.5,0);
		p2 = AcGePoint3d(originP.x+639.96,originP.y+1220.5,0);
		p3 = AcGePoint3d(originP.x+556.79-3,originP.y+1310.23,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"166.33",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+660.31,originP.y+699.25,0);
		p2 = AcGePoint3d(originP.x+673.68,originP.y+699.25,0);
		Data->MyFunction->Arx->CreateLine(p1, p2, &ObjectId);
		AircylinderObjects.append(ObjectId);
		c_num = originP.y - setting.OriginalPoint.y + 699.25;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+663.31,originP.y+699.25,0);
		p2 = AcGePoint3d(originP.x+663.31,originP.y+699.25-40,0);
		p3 = AcGePoint3d(originP.x+663.31,originP.y+699.25-80,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+706.23,originP.y+968,0);
		p2 = AcGePoint3d(originP.x+731.68,originP.y+968,0);
		p3 = AcGePoint3d(originP.x+718.96,originP.y+953.96,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"25.45",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+706.23,originP.y+968,0);
		p2 = AcGePoint3d(originP.x+756.56,originP.y+968,0);
		p3 = AcGePoint3d(originP.x+731.68,originP.y+938.02,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"50.33",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+795.68,originP.y+430.5,0);
		p2 = AcGePoint3d(originP.x+756.56,originP.y+968,0);
		p3 = AcGePoint3d(originP.x+795.68-3,originP.y+699.25,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"537.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+867.91,originP.y+665.5,0);
		p2 = AcGePoint3d(originP.x+804.6,originP.y+665.5,0);
		Data->MyFunction->Arx->CreateLine(p1, p2, &ObjectId);
		AircylinderObjects.append(ObjectId);
		c_num = originP.y - setting.OriginalPoint.y + 665.5;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+864.91,originP.y+665.5,0);
		p2 = AcGePoint3d(originP.x+864.91,originP.y+665.5-40,0);
		p3 = AcGePoint3d(originP.x+864.91,originP.y+665.5-80,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+890.87,originP.y+430.5,0);
		p2 = AcGePoint3d(originP.x+910.42,originP.y+900.5,0);
		p3 = AcGePoint3d(originP.x+930.41,originP.y+664.87,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"470",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+932.53,originP.y+605.5,0);
		p2 = AcGePoint3d(originP.x+954.24,originP.y+605.5,0);
		Data->MyFunction->Arx->CreateLine(p1, p2, &ObjectId);
		AircylinderObjects.append(ObjectId);
		c_num = originP.y - setting.OriginalPoint.y + 605.5;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+935.53,originP.y+605.5,0);
		p2 = AcGePoint3d(originP.x+935.53,originP.y+605.5-40,0);
		p3 = AcGePoint3d(originP.x+935.53,originP.y+605.5-80,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+1160.35,originP.y+584,0);
		p2 = AcGePoint3d(originP.x+1212.29,originP.y+584,0);
		Data->MyFunction->Arx->CreateLine(p1, p2, &ObjectId);
		AircylinderObjects.append(ObjectId);
		c_num = originP.y - setting.OriginalPoint.y + 584;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+1209.29,originP.y+584,0);
		p2 = AcGePoint3d(originP.x+1209.29,originP.y+584-40,0);
		p3 = AcGePoint3d(originP.x+1209.29,originP.y+584-80,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+968.99,originP.y+780.5,0);
		p2 = AcGePoint3d(originP.x+1007.18,originP.y+780.5,0);
		p3 = AcGePoint3d(originP.x+1032.5,originP.y+760.56,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"38.19",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+968.99,originP.y+780.5,0);
		p2 = AcGePoint3d(originP.x+1041.68,originP.y+780.5,0);
		p3 = AcGePoint3d(originP.x+1032.5-3,originP.y+737.75,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"72.69",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+1021.55,originP.y+781.12,0);
		p2 = AcGePoint3d(originP.x+1021.55,originP.y+431.12,0);
		p3 = AcGePoint3d(originP.x+1021.55-3,originP.y+606.12,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"350",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+1076.68,originP.y+737.5,0);
		p2 = AcGePoint3d(originP.x+1154.69,originP.y+737.5,0);
		p3 = AcGePoint3d(originP.x+1115.69,originP.y+774.2+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"78.01",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+1089.18,originP.y+430.5,0);
		p2 = AcGePoint3d(originP.x+1076.68,originP.y+737.5,0);
		p3 = AcGePoint3d(originP.x+1186.19-3,originP.y+584,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"307",Image,AxisNum,isDelete);
#pragma endregion
	}
	else if(TypeName == "MSJ-182")
	{
#pragma region
		p1 = AcGePoint3d(originP.x+24.5-3.05,originP.y-3.75-3.96,0);
		p2 = AcGePoint3d(originP.x+15.31,originP.y-15.66,0);
		Data->MyFunction->Arx->CreateRadiusDim( AcGePoint3d(p1.x+2,p1.y+2,0),p1,  1, "R5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+73.9,originP.y+51.25,0);
		p2 = AcGePoint3d(originP.x+21.91+25.49,originP.y+51.25,0);
		p3 = AcGePoint3d(originP.x+23.05+22.38,originP.y+41.07-0.64+4.77,0);
		p4 = AcGePoint3d(originP.x+25.18,originP.y+46.13,0);
		Ctext.Format(L"12%%%%d");
		Data->MyFunction->Arx->CreateAngleDim(p1,p2,p3,p4,Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+32.5,originP.y+31.52,0);
		p2 = AcGePoint3d(originP.x+82.5,originP.y+39.25,0);
		p3 = AcGePoint3d(originP.x+57.5,originP.y+42.29+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"50",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+100.5,originP.y+38.6,0);
		p2 = AcGePoint3d(originP.x+140.5,originP.y+38.6,0);
		p3 = AcGePoint3d(originP.x+120.5,originP.y+32.39+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"40",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+120.5,originP.y+45.85,0);
		p2 = AcGePoint3d(originP.x+140.5,originP.y+45.85,0);
		p3 = AcGePoint3d(originP.x+130.5,originP.y+45.85+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"20",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+101.5,originP.y+51,0);
		p2 = AcGePoint3d(originP.x+101.5,originP.y+43,0);
		p3 = AcGePoint3d(originP.x+97.04-3,originP.y+47,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"8",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+45.78,originP.y+45.33,0);
		p2 = AcGePoint3d(originP.x+73.91,originP.y+51.25,0);
		p3 = AcGePoint3d(originP.x+59.84,originP.y+55.55+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"28.199",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+73.9,originP.y+51.25,0);
		p2 = AcGePoint3d(originP.x+82.5,originP.y+67.16-28.54,0);
		p3 = AcGePoint3d(originP.x+78.2,originP.y+67.16+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"8.6",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+77.5,originP.y+46.9,0);
		p2 = AcGePoint3d(originP.x+89.54-7.61,originP.y+53.22-4,0);
		Data->MyFunction->Arx->CreateRadiusDim( p1,p2,  1, "R5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+34.28,originP.y+106.25,0);
		p2 = AcGePoint3d(originP.x+45.5,originP.y+106.25,0);
		Data->MyFunction->Arx->CreateLine(p1, p2, &ObjectId);
		AircylinderObjects.append(ObjectId);
		c_num = originP.y - setting.OriginalPoint.y + 106.25;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+37.28,originP.y+106.25,0);
		p2 = AcGePoint3d(originP.x+37.28,originP.y+106.25-10,0);
		p3 = AcGePoint3d(originP.x+37.28,originP.y+106.25-20,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+101.23,originP.y+124.5,0);
		p2 = AcGePoint3d(originP.x+104.24,originP.y+124.5,0);
		Data->MyFunction->Arx->CreateLine(p1, p2, &ObjectId);
		AircylinderObjects.append(ObjectId);
		c_num = originP.y - setting.OriginalPoint.y + 124.5;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+102.74,originP.y+124.5,0);
		p2 = AcGePoint3d(originP.x+102.74,originP.y+124.5-10,0);
		p3 = AcGePoint3d(originP.x+102.74,originP.y+124.5-20,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+74.8,originP.y+51.25,0);
		p2 = AcGePoint3d(originP.x+79.87,originP.y+161.25,0);
		p3 = AcGePoint3d(originP.x+86.64-3,originP.y+106.25,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"110",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+73.9,originP.y+51.25,0);
		p2 = AcGePoint3d(originP.x+79.87,originP.y+152.26+8.99,0);
		p3 = AcGePoint3d(originP.x+91.3,originP.y+152.26+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"5.975",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+136.99,originP.y+51,0);
		p2 = AcGePoint3d(originP.x+128.8,originP.y+198,0);
		p3 = AcGePoint3d(originP.x+150.42-3,originP.y+124.5,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"147",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+43.06,originP.y+161.25,0);
		p2 = AcGePoint3d(originP.x+79.53+0.34,originP.y+161.25,0);
		p3 = AcGePoint3d(originP.x+61.73,originP.y+163.45+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"36.82",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+29.5,originP.y+212.54,0);
		p2 = AcGePoint3d(originP.x+102.5,originP.y+212.54,0);
		p3 = AcGePoint3d(originP.x+66,originP.y+212.54+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"73",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+79.87,originP.y+161.25,0);
		p2 = AcGePoint3d(originP.x+56.8,originP.y+161.25,0);
		p3 = AcGePoint3d(originP.x+42.21,originP.y+139.5,0);
		p4 = AcGePoint3d(originP.x+38.2,originP.y+150.08,0);
		Ctext.Format(L"30%%%%d");
		Data->MyFunction->Arx->CreateAngleDim(p1,p2,p3,p4,Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+120.49,originP.y+212.75,0);
		p2 = AcGePoint3d(originP.x+120.49,originP.y+216.65,0);
		p3 = AcGePoint3d(originP.x+109.48-3,originP.y+214.75,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"4",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+111.12,originP.y+202.29,0);
		p2 = AcGePoint3d(originP.x+129.68,originP.y+201.32,0);
		p3 = AcGePoint3d(originP.x+120.5,originP.y+223.31+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"18.56",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+120.5,originP.y+205.2,0);
		p2 = AcGePoint3d(originP.x+129.68,originP.y+201.32,0);
		p3 = AcGePoint3d(originP.x+136.91,originP.y+218.42+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"9.18",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+111.12+15.14,originP.y+205.55,0);
		p2 = AcGePoint3d(originP.x+129.68,originP.y+204.12,0);
		p3 = AcGePoint3d(originP.x+120.5,originP.y+206.58+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"3.42",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+120.5+8.62-18,originP.y+201.04,0);
		p2 = AcGePoint3d(originP.x+129.68+3.86-18.56,originP.y+201.04,0);
		p3 = AcGePoint3d(originP.x+136.91-17.34,originP.y+201.04+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"3.86",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+120.5-18,originP.y+201.04,0);
		p2 = AcGePoint3d(originP.x+129.68-18.56,originP.y+201.04,0);
		p3 = AcGePoint3d(originP.x+136.91,originP.y+201.04+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"8.62",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+128.8,originP.y+214.37-16.37,0);
		p2 = AcGePoint3d(originP.x+112.73,originP.y+214.37-16.37,0);
		p3 = AcGePoint3d(originP.x+117.89,originP.y+207.82-16.16,0);
		p4 = AcGePoint3d(originP.x+116.12,originP.y+210.94-18.51,0);
		Ctext.Format(L"31%%%%d");
		Data->MyFunction->Arx->CreateAngleDim(p1,p2,p3,p4,Ctext,Image,AxisNum,isDelete);

		p2 = AcGePoint3d(originP.x+344.33,originP.y-177.21,0);
		Data->MyFunction->Arx->CreateRadiusDim( AcGePoint3d(p2.x+2,p2.y-2,0),p2,  1, "R30",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+363.5,originP.y-78.2,0);
		p2 = AcGePoint3d(originP.x+448.5,originP.y-78.2,0);
		p3 = AcGePoint3d(originP.x+406,originP.y-78.2+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"85",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+607.5,originP.y-70,0);
		p2 = AcGePoint3d(originP.x+784.5,originP.y-70,0);
		p3 = AcGePoint3d(originP.x+696,originP.y-70+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"177",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+784.5,originP.y-71.06,0);
		p2 = AcGePoint3d(originP.x+784.5,originP.y-75.59,0);
		p3 = AcGePoint3d(originP.x+798.83,originP.y-152.93,0);
		p4 = AcGePoint3d(originP.x+793.24,originP.y-171.57,0);
		Ctext.Format(L"10%%%%d");
		Data->MyFunction->Arx->CreateAngleDim(p1,p2,p3,p4,Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+321,originP.y-17.25,0);
		p2 = AcGePoint3d(originP.x+326.12,originP.y-17.25,0);
		p3 = AcGePoint3d(originP.x+337.55,originP.y-22.92+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"5.125",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+352,originP.y-3.88,0);
		p2 = AcGePoint3d(originP.x+360,originP.y-3.88,0);
		p3 = AcGePoint3d(originP.x+356,originP.y-3.88+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"8",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+343,originP.y-2.75,0);
		p2 = AcGePoint3d(originP.x+352,originP.y+2.53,0);
		p3 = AcGePoint3d(originP.x+347.5,originP.y+7.94+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"9",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+338.5,originP.y+7.94,0);
		p2 = AcGePoint3d(originP.x+343,originP.y+2.53,0);
		p3 = AcGePoint3d(originP.x+351.99,originP.y+14.22+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"4.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+330,originP.y+13.51,0);
		p2 = AcGePoint3d(originP.x+338.5,originP.y+13.51,0);
		p3 = AcGePoint3d(originP.x+344.47,originP.y+23.58+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"8.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+338.5,originP.y+13.3,0);
		p2 = AcGePoint3d(originP.x+356,originP.y+22.06,0);
		p3 = AcGePoint3d(originP.x+347.25,originP.y+18.34+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"17.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+356,originP.y+26.9,0);
		p2 = AcGePoint3d(originP.x+365,originP.y+26.9,0);
		p3 = AcGePoint3d(originP.x+360.5,originP.y+26.9+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"9",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+368.5,originP.y+35.01,0);
		p2 = AcGePoint3d(originP.x+443.5,originP.y+34.73,0);
		p3 = AcGePoint3d(originP.x+406,originP.y+27.72+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"75",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+406,originP.y+34.56,0);
		p2 = AcGePoint3d(originP.x+443.5,originP.y+34.56,0);
		p3 = AcGePoint3d(originP.x+424.75,originP.y+34.56+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"37.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+443.5,originP.y+38.53,0);
		p2 = AcGePoint3d(originP.x+464,originP.y+38.53,0);
		p3 = AcGePoint3d(originP.x+453.75,originP.y+38.53+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"20.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+452,originP.y+8.56,0);
		p2 = AcGePoint3d(originP.x+464,originP.y+8.56,0);
		p3 = AcGePoint3d(originP.x+458,originP.y+8.56+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"12",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+312.44,originP.y+52.25,0);
		p2 = AcGePoint3d(originP.x+345.18,originP.y+52.25,0);
		p3 = AcGePoint3d(originP.x+328.81,originP.y+60.56+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"32.74",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+343.98,originP.y+292.25,0);
		p2 = AcGePoint3d(originP.x+345.18,originP.y+52.25,0);
		p3 = AcGePoint3d(originP.x+353.6,originP.y+66.3+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"1.2",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+356,originP.y+49.09,0);
		p2 = AcGePoint3d(originP.x+368.5,originP.y+49.09,0);
		p3 = AcGePoint3d(originP.x+362.25,originP.y+57.25+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"12.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+464,originP.y+25.81,0);
		p2 = AcGePoint3d(originP.x+599,originP.y+25.81,0);
		p3 = AcGePoint3d(originP.x+531.5,originP.y+25.81+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"135",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+521.67,originP.y+48.5,0);
		p2 = AcGePoint3d(originP.x+583,originP.y+48.5,0);
		p3 = AcGePoint3d(originP.x+552.33,originP.y+34.2+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"61.33",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+583,originP.y+48.5,0);
		p2 = AcGePoint3d(originP.x+596.85,originP.y+506.5,0);
		p3 = AcGePoint3d(originP.x+589.93,originP.y+34.2+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"13.85",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+583,originP.y+48.5,0);
		p2 = AcGePoint3d(originP.x+599,originP.y+48.5,0);
		p3 = AcGePoint3d(originP.x+591,originP.y+53.05+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"16",Image,AxisNum,isDelete);


		p1 = AcGePoint3d(originP.x+616,originP.y+45.85,0);
		p2 = AcGePoint3d(originP.x+776,originP.y+45.85,0);
		p3 = AcGePoint3d(originP.x+696,originP.y+73.71+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"160",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+696,originP.y+45.85,0);
		p2 = AcGePoint3d(originP.x+776,originP.y+45.85,0);
		p3 = AcGePoint3d(originP.x+736,originP.y+62.75+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"80",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+293.99,originP.y+172.25,0);
		p2 = AcGePoint3d(originP.x+290.74,originP.y+172.25,0);
		Data->MyFunction->Arx->CreateLine(p1, p2, &ObjectId);
		AircylinderObjects.append(ObjectId);
		c_num = originP.y - setting.OriginalPoint.y + 172.25;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+292.24,originP.y+172.25,0);
		p2 = AcGePoint3d(originP.x+292.24,originP.y+172.25-10,0);
		p3 = AcGePoint3d(originP.x+292.24,originP.y+172.25-20,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+335.63,originP.y+292.25,0);
		p2 = AcGePoint3d(originP.x+335.63,originP.y+52.25,0);
		p3 = AcGePoint3d(originP.x+335.63-3,originP.y+172.25,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"240",Image,AxisNum,isDelete);

		c_num = originP.y - setting.OriginalPoint.y + 189;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+393.78,originP.y+189,0);
		p2 = AcGePoint3d(originP.x+393.78,originP.y+189-10,0);
		p3 = AcGePoint3d(originP.x+393.78,originP.y+189-20,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+416.94,originP.y+326.75,0);
		p2 = AcGePoint3d(originP.x+443.5,originP.y+51.25,0);
		p3 = AcGePoint3d(originP.x+445.28-3,originP.y+189,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"275.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+305.7,originP.y+292.25,0);
		p2 = AcGePoint3d(originP.x+343.98,originP.y+292.25,0);
		p3 = AcGePoint3d(originP.x+324.84,originP.y+305.9+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"38.28",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+343.98,originP.y+292.25,0);
		p2 = AcGePoint3d(originP.x+318.06,originP.y+292.25,0);
		p3 = AcGePoint3d(originP.x+320.12,originP.y+282.12,0);
		p4 = AcGePoint3d(originP.x+318.98,originP.y+285.4,0);
		Ctext.Format(L"23%%%%d");
		Data->MyFunction->Arx->CreateAngleDim(p1,p2,p3,p4,Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+366,originP.y+327.69,0);
		p2 = AcGePoint3d(originP.x+395.26,originP.y+327.69,0);
		p3 = AcGePoint3d(originP.x+380.63,originP.y+327.69+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"29.26",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+395.26,originP.y+331.85,0);
		p2 = AcGePoint3d(originP.x+416.54,originP.y+331.85,0);
		p3 = AcGePoint3d(originP.x+406,originP.y+358.42+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"21.28",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+406,originP.y+331.85,0);
		p2 = AcGePoint3d(originP.x+416.54,originP.y+331.85,0);
		p3 = AcGePoint3d(originP.x+411.27,originP.y+351.7+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"10.54",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+416.94,originP.y+326.75,0);
		p2 = AcGePoint3d(originP.x+416.94,originP.y+331.85,0);
		p3 = AcGePoint3d(originP.x+430.47-3,originP.y+329.3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"5.1",Image,AxisNum,isDelete);

		c_num = originP.y - setting.OriginalPoint.y + 277.5;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+540.89,originP.y+277.5,0);
		p2 = AcGePoint3d(originP.x+540.89,originP.y+277.5-10,0);
		p3 = AcGePoint3d(originP.x+540.89,originP.y+277.5-20,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+580.42,originP.y+48.5,0);
		p2 = AcGePoint3d(originP.x+580.42,originP.y+506.5,0);
		p3 = AcGePoint3d(originP.x+580.42-3,originP.y+277.5,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"458",Image,AxisNum,isDelete);

		c_num = originP.y - setting.OriginalPoint.y + 310.75;
		Ctext.Format(L"%%%%c%g",c_num);
		p1 = AcGePoint3d(originP.x+680.81,originP.y+310.75,0);
		p2 = AcGePoint3d(originP.x+680.81,originP.y+310.75-10,0);
		p3 = AcGePoint3d(originP.x+680.81,originP.y+310.75-20,0);
		Data->MyFunction->Arx->CreateLeader(p1, p2, p3, Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+723.33,originP.y+45.5,0);
		p2 = AcGePoint3d(originP.x+723.33,originP.y+576,0);
		p3 = AcGePoint3d(originP.x+723.33-3,originP.y+310.75,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"530.5",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+503.89,originP.y+506.5,0);
		p2 = AcGePoint3d(originP.x+596.85,originP.y+506.5,0);
		p3 = AcGePoint3d(originP.x+550.37,originP.y+513.55+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"92.96",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+564.85,originP.y+506.5,0);
		p2 = AcGePoint3d(originP.x+596.79,originP.y+481.05,0);
		p3 = AcGePoint3d(originP.x+580.85,originP.y+509.13+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"32",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+596.79,originP.y+506.5,0);
		p2 = AcGePoint3d(originP.x+568.7,originP.y+506.5,0);
		p3 = AcGePoint3d(originP.x+574.82,originP.y+488.98,0);
		p4 = AcGePoint3d(originP.x+569.91,originP.y+495.81,0);
		Ctext.Format(L"38.5%%%%d");
		Data->MyFunction->Arx->CreateAngleDim(p1,p2,p3,p4,Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+596.85,originP.y+506.5,0);
		p2 = AcGePoint3d(originP.x+523.24,originP.y+506.5,0);
		p3 = AcGePoint3d(originP.x+545.55,originP.y+458.66,0);
		p4 = AcGePoint3d(originP.x+528.53,originP.y+479.91,0);
		Ctext.Format(L"50%%%%d");
		Data->MyFunction->Arx->CreateAngleDim(p1,p2,p3,p4,Ctext,Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+686.05,originP.y+588.3,0);
		p2 = AcGePoint3d(originP.x+706.05,originP.y+588.3,0);
		p3 = AcGePoint3d(originP.x+696.05,originP.y+600.56+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"20",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+696.05,originP.y+588.3,0);
		p2 = AcGePoint3d(originP.x+706.05,originP.y+588.3,0);
		p3 = AcGePoint3d(originP.x+701.02,originP.y+591.53+3,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,0,"10.05",Image,AxisNum,isDelete);

		p1 = AcGePoint3d(originP.x+706,originP.y+576,0);
		p2 = AcGePoint3d(originP.x+706,originP.y+588.3,0);
		p3 = AcGePoint3d(originP.x+723.33-3,originP.y+582.15,0);
		Data->MyFunction->Arx->CreateLinearDim(p1,p2,p3,90,"12.3",Image,AxisNum,isDelete);

#pragma endregion
	}

	Data->MyFunction->Arx->SetNowLayerName(L"1粗实线");
	return 0;
}

//出图
bool ClassTongliu::Export(){
	getBeBladeData();
	ExportXMLData();
	if (!LoadXMLData())
		return false;
	LoadXMLDataDispose();
	init_cad();
	createTL();
	return true;
}



