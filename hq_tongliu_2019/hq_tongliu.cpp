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
	Data->MyFunction->Arx->CreateLayer(L"1��ʵ��", 7, L"CONTINUOUS");
	Data->MyFunction->Arx->CreateLayer(L"2ϸʵ��", 1, L"CONTINUOUS");	//��ɫϸʵ��
	Data->MyFunction->Arx->CreateLayer(L"3������", 2, L"CENTER");		//��ɫ�㻮��
	Data->MyFunction->Arx->CreateLayer(L"4����", 2, L"DASHED");
	Data->MyFunction->Arx->CreateLayer(L"������", 2, L"CONTINUOUS");	//��ɫ������
	Data->MyFunction->Arx->CreateLayer(L"5�ߴ���", 1, L"CONTINUOUS");
	Data->MyFunction->Arx->CreateLayer(L"����", 3, L"CONTINUOUS");
	Data->MyFunction->Arx->CreateLayer(L"6������", 1, L"CONTINUOUS");	//��ɫ������
	Data->MyFunction->Arx->CreateLayer(L"����",5, L"CONTINUOUS");		//��ɫϸʵ��
	Data->MyFunction->Arx->CreateLayer(L"˫�㻮��",6, L"DIVIDE2");		//��ɫ�绰��
	Data->MyFunction->Arx->CreateLayer(L"ϸ�㻮��", 4, L"CENTER");		//��ɫ�㻮��
	Data->MyFunction->Arx->CreateLayer(L"ͼ��", 2, L"CONTINUOUS");		//��ɫ
	Data->MyFunction->Arx->CreateLayer(L"������", 2, L"CONTINUOUS");	//��ɫ������

	//����ͼ��
	CString BlockPath =setting.ResPath+ "\\QFC_T.dwg";
	Data->MyFunction->Arx->ImportBlock(BlockPath);
	BlockPath =setting.ResPath+ "\\LastThreeStages.dwg";
	Data->MyFunction->Arx->ImportBlock(BlockPath);
	//acedAlert(BlockPath);
}


//************���ݴ���

//��ȡҶƬ���� 
int ClassTongliu::getBeBladeData()
{
	setlocale(LC_ALL,"Chinese-simplified");//�������Ļ��� 

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
			return -2;//sheetҳ�ṹ����
		}
		
		BasicExcelCell* becCurrentCell;
		double p0Jz_1_num =0.0;
		string AirPassage_str ,OriginalPoint_str,LastThreeStages_str;
		double OutNum = 0.0;

		//AirPassage ��������
		becCurrentCell = bewCurrentSheet->Cell(0,3);
		if(isCellString(becCurrentCell,AirPassage_str))
			aircylinderTmp.AirPassage = AirPassage_str;

		//isHorizonBlade �Ƿ��к���ҶƬ 1��0��
		becCurrentCell = bewCurrentSheet->Cell(0,5);
		if(isCellDouble(becCurrentCell,OutNum))
			aircylinderTmp.isHorizonBlade = OutNum;

		//ͨ��ͼԭ��
		becCurrentCell = bewCurrentSheet->Cell(0,7);
		if(isCellString( becCurrentCell,OriginalPoint_str))
		{
			string first, second;
			double firstValue, secondValue;
			splitString(OriginalPoint_str, first, second, firstValue, secondValue);
			setting.OriginalPoint = AcGePoint3d(firstValue,secondValue,0);
		}

		//�����������������
		becCurrentCell = bewCurrentSheet->Cell(0,9);
		if(isCellDouble(becCurrentCell,OutNum))
			aircylinderTmp.AirAxialDis = OutNum;

		//�������Ŀ��
		becCurrentCell = bewCurrentSheet->Cell(0,11);
		if(isCellDouble(becCurrentCell,OutNum))
			aircylinderTmp.AirWeight = OutNum;

		//���ۿ��
		becCurrentCell = bewCurrentSheet->Cell(0,13);
		if(isCellDouble(becCurrentCell,OutNum))
			aircylinderTmp.CantileverWidth = OutNum;

		//ĩ����ѡ��
		becCurrentCell = bewCurrentSheet->Cell(0,15);
		if(isCellString(becCurrentCell,LastThreeStages_str))
			aircylinderTmp.LastThreeStages = LastThreeStages_str;

		//p0Jz_1
		becCurrentCell = bewCurrentSheet->Cell(4,2);
		if(isCellDouble( becCurrentCell,p0Jz_1_num))
		{
			//���к���ҶƬʱ
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

			//���Ͳ�
			becCurrentCell = bewCurrentSheet->Cell(i,37);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.P = thNum;
			else
				continue;

			//���Ͳ�
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

			//��̬�Ͳ�
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

			//�½��������
			becCurrentCell = bewCurrentSheet->Cell(i,43);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.�� = thNum;
			else
				continue;

			//p32d /ÿ����ҶΪ�����������������Ķ�Χ���ڲ۵Ŀ��
			becCurrentCell = bewCurrentSheet->Cell(i,44);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.p32d = thNum;
			else
				continue;

			//c_deep ����ȳ����ߴ�
			becCurrentCell = bewCurrentSheet->Cell(i,45);
			if(isCellDouble( becCurrentCell,thNum))
			{
				blade_tmp.c_deep = thNum;
				blade_tmp.Sc_deep = thNum;
			}
			else
				continue;


			//��Ҷ
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

			
			//��Ҷ
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

			//�������
			//IsSuctionPort �Ƿ��г�ڣ�1Ϊ�У�0Ϊ��
			becCurrentCell = bewCurrentSheet->Cell(i,46);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.IsSuctionPort = thNum;
			else
				continue;

			//InletWeight �����������ȣ��ǿ�
			becCurrentCell = bewCurrentSheet->Cell(i,47);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.InletWeight = thNum;
			else
				continue;

			//OutletWeight �����������ȣ��ǿ�
			becCurrentCell = bewCurrentSheet->Cell(i,48);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.OutletWeight = thNum;
			else
				continue;

			//CoverDegree ����Ҷ���Ƕȣ��ǿ�
			becCurrentCell = bewCurrentSheet->Cell(i,49);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.CoverDegree = thNum;
			else
				continue;

			//V ��������V���ǿ�
			becCurrentCell = bewCurrentSheet->Cell(i,50);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.V = thNum;
			else
				continue;

			//G ��������G���ǿ�
			becCurrentCell = bewCurrentSheet->Cell(i,51);
			if(isCellDouble( becCurrentCell,thNum))
				blade_tmp.G = thNum;
			else
				continue;

			//�����������MaxAllowFlow��XML ��ѹ��ѹ��ѹ����
			becCurrentCell = bewCurrentSheet->Cell(i,52);
			string MaxAllowFlow_str ;
			if(isCellString( becCurrentCell,MaxAllowFlow_str))
				blade_tmp.MaxAllowFlow = MaxAllowFlow_str;
			else
				continue;

			//����м�������Y
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
		return -1;//û��sheetҳ


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


//�жϵ�Ԫ������ string
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


//,�ָ�string����תΪdouble
void ClassTongliu::splitString( std::string input, std::string& first, std::string& second, double& firstValue, double& secondValue )
{
	size_t commaPos = input.find("��"); // �������Ķ���λ��
	if (commaPos == std::string::npos) {
		commaPos = input.find(","); // ����Ӣ�Ķ���λ��
	}

	if (commaPos != std::string::npos) {
		first = input.substr(0, commaPos); // ��ȡ��һ������
		second = input.substr(commaPos + 1); // ��ȡ�ڶ�������
	} else {
		first = input; // û�ж��ţ�����Ϊ�����ַ���Ϊ��һ������
		second = ""; // �ڶ�������Ϊ���ַ���
	}

	// ת��Ϊ double ����
	std::stringstream ss(first);
	ss >> firstValue;
	std::stringstream ss1(second);
	ss1 >> secondValue;
}


//������double���string
void ClassTongliu::combineString(double firstValue, double secondValue, std::string& combinedString) 
{
	std::ostringstream oss;
	oss << firstValue << "," << secondValue;
	combinedString = oss.str();
}
//����ͨ��xml
bool ClassTongliu::ExportXMLData()
{
	CString str;

	TiXmlDocument *myDocument = new TiXmlDocument();
	TiXmlDeclaration *dec = new TiXmlDeclaration("1.0","UTF-8","");
	myDocument->LinkEndChild(dec);

	TiXmlElement *RootElement = new TiXmlElement("Data");

	//Setting <!--  �������   -->
	TiXmlElement *SettingElement = new TiXmlElement("Setting");
	RootElement->LinkEndChild(SettingElement);

	//ResPath ��Դ·�� -- ��
	TiXmlElement *ResPathElement = new TiXmlElement("ResPath");
	SettingElement->LinkEndChild(ResPathElement);
	//acedAlert(Data->ResPath);
	TiXmlText *ResPathContent = new TiXmlText(Data->MyFunction->Basic->CString2Char(Data->ResPath));
	ResPathElement->LinkEndChild(ResPathContent);

	//ͨ��ͼԭ��
	TiXmlElement *OriginalPointElement = new TiXmlElement("OriginalPoint");
	SettingElement->LinkEndChild(OriginalPointElement);

	double firstValue = setting.OriginalPoint.x;
	double secondValue = setting.OriginalPoint.y;
	string combinedString;
	combineString(firstValue, secondValue, combinedString);

	TiXmlText *OriginalPointContent = new TiXmlText(combinedString.c_str());
	OriginalPointElement->LinkEndChild(OriginalPointContent);

	//////////////////////////////////////////////////////////////////////////////////

	//AirCylinder <!--  ����   -->
	TiXmlElement *AirCylinderElement = new TiXmlElement("AirCylinder");
	RootElement->LinkEndChild(AirCylinderElement);
	

	//AirPassage ��������
	TiXmlElement *AirPassageElement = new TiXmlElement("AirPassage");
	AirCylinderElement->LinkEndChild(AirPassageElement);
	string str_tmp = aircylinderTmp.AirPassage;
	TiXmlText *AirPassageContent = new TiXmlText(str_tmp.c_str());
	AirPassageElement->LinkEndChild(AirPassageContent);


	//LastThreeStages ĩ����ѡ�ͼ���λ����
	TiXmlElement *LastThreeStageseElement = new TiXmlElement("LastThreeStages");
	AirCylinderElement->LinkEndChild(LastThreeStageseElement);
	TiXmlText *LastThreeStageseContent = new TiXmlText(aircylinderTmp.LastThreeStages.c_str());
	LastThreeStageseElement->LinkEndChild(LastThreeStageseContent);

	//��������
	TiXmlElement *AirwayCenterElement = new TiXmlElement("AirwayCenter");
	AirCylinderElement->LinkEndChild(AirwayCenterElement);

	//�����������������
	TiXmlElement *AirAxialDisElement = new TiXmlElement("AirAxialDis");
	AirwayCenterElement->LinkEndChild(AirAxialDisElement);
	TiXmlText *AirAxialDisContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(aircylinderTmp.AirAxialDis));
	AirAxialDisElement->LinkEndChild(AirAxialDisContent);

	//�������Ŀ��
	TiXmlElement *AirWeightElement = new TiXmlElement("AirWeight");
	AirwayCenterElement->LinkEndChild(AirWeightElement);
	TiXmlText *AirWeightContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(aircylinderTmp.AirWeight));
	AirWeightElement->LinkEndChild(AirWeightContent);

	//���ۿ��
	TiXmlElement *CantileverWidthElement = new TiXmlElement("CantileverWidth");
	AirwayCenterElement->LinkEndChild(CantileverWidthElement);
	TiXmlText *CantileverWidthContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(aircylinderTmp.CantileverWidth));
	CantileverWidthElement->LinkEndChild(CantileverWidthContent);

	//isHorizonBlade �Ƿ��к���ҶƬ 1��0��
	TiXmlElement *IsHorizonBladeElement = new TiXmlElement("IsHorizonBlade");
	AirCylinderElement->LinkEndChild(IsHorizonBladeElement);
	TiXmlText *IsHorizonBladeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(aircylinderTmp.isHorizonBlade));
	IsHorizonBladeElement->LinkEndChild(IsHorizonBladeContent);

	//p0Jz_1<!--  ԭ�㵽��1����Ҷ�����ߵľ���  -->
	TiXmlElement *p0Jz_1Element = new TiXmlElement("p0Jz_1");
	AirCylinderElement->LinkEndChild(p0Jz_1Element);
	TiXmlText *p0Jz_1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(aircylinderTmp.p0Jz_1));
	p0Jz_1Element->LinkEndChild(p0Jz_1Content);

	vector<bladeData>bladeDataVec = aircylinderTmp.bladePair;
	//BladePair<!--  ҶƬ��   -->
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
//ҶƬ�Խڵ����ݵ���
bool ClassTongliu::ExportXMLData( TiXmlElement* pElem,bladeData tmp)
{
	CString str;

	//BladePair ҶƬ��
	TiXmlElement *BladePairElement = new TiXmlElement("BladePair");
	pElem->LinkEndChild(BladePairElement);

	//SuctionPort ���λ�ü�����
	TiXmlElement *SuctionPortElement = new TiXmlElement("SuctionPort");
	BladePairElement->LinkEndChild(SuctionPortElement);
	
	//IsSuctionPort �Ƿ��г�ڣ�1Ϊ�У�0Ϊ��
	TiXmlElement *IsSuctionPortElement = new TiXmlElement("IsSuctionPort");
	SuctionPortElement->LinkEndChild(IsSuctionPortElement);
	TiXmlText *IsSuctionPortContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.IsSuctionPort));
	IsSuctionPortElement->LinkEndChild(IsSuctionPortContent);

	//InletWeight �����������ȣ��ǿ�
	TiXmlElement *InletWeightElement = new TiXmlElement("InletWeight");
	SuctionPortElement->LinkEndChild(InletWeightElement);
	TiXmlText *InletWeightContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.InletWeight));
	InletWeightElement->LinkEndChild(InletWeightContent);

	//OutletWeight �����������ȣ��ǿ�
	TiXmlElement *OutletWeightElement = new TiXmlElement("OutletWeight");
	SuctionPortElement->LinkEndChild(OutletWeightElement);
	TiXmlText *OutletWeightContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.OutletWeight));
	OutletWeightElement->LinkEndChild(OutletWeightContent);

	//CoverDegree ����Ҷ���Ƕȣ��ǿ�
	TiXmlElement *CoverDegreeElement = new TiXmlElement("CoverDegree");
	SuctionPortElement->LinkEndChild(CoverDegreeElement);
	TiXmlText *CoverDegreeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.CoverDegree));
	CoverDegreeElement->LinkEndChild(CoverDegreeContent);

	//��������V���ǿ�
	TiXmlElement *VElement = new TiXmlElement("V");
	SuctionPortElement->LinkEndChild(VElement);
	TiXmlText *VContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.V));
	VElement->LinkEndChild(VContent);

	//��������G���ǿ�
	TiXmlElement *GElement = new TiXmlElement("G");
	SuctionPortElement->LinkEndChild(GElement);
	TiXmlText *GContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.G));
	GElement->LinkEndChild(GContent);

	//�����������MaxAllowFlow��XML ��ѹ��ѹ��ѹ����
	TiXmlElement *MaxAllowFlowElement = new TiXmlElement("MaxAllowFlow");
	SuctionPortElement->LinkEndChild(MaxAllowFlowElement);
	TiXmlText *MaxAllowFlowContent = new TiXmlText(tmp.MaxAllowFlow.c_str());
	MaxAllowFlowElement->LinkEndChild(MaxAllowFlowContent);

	//����м�������Y
	TiXmlElement *MidYElement = new TiXmlElement("MidY");
	SuctionPortElement->LinkEndChild(MidYElement);
	TiXmlText *MidYContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.MidY));
	MidYElement->LinkEndChild(MidYContent);

	//p31zf �������򣬶�����ֵ=1�������������ң�������������֣��ǿ�
	TiXmlElement *p31zfElement = new TiXmlElement("p31zf");
	BladePairElement->LinkEndChild(p31zfElement);
	const char * thetmp = Data->MyFunction->Basic->Double2Char(tmp.p31zf);
	TiXmlText *p31zfContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p31zf));
	p31zfElement->LinkEndChild(p31zfContent);

	//��������ǡ�1Ϊ��0Ϊ��
	TiXmlElement *BrokenElement = new TiXmlElement("Broken");
	BladePairElement->LinkEndChild(BrokenElement);

	//BrokenTag ��������ǡ�1Ϊ��0Ϊ��
	TiXmlElement *BrokenTagElement = new TiXmlElement("BrokenTag");
	BrokenElement->LinkEndChild(BrokenTagElement);
	TiXmlText *BrokenTagContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.BrokenTag));
	BrokenTagElement->LinkEndChild(BrokenTagContent);

	//BrokenDis ������������룬��Ҷ�����߾�����������
	TiXmlElement *BrokenDisElement = new TiXmlElement("BrokenDis");
	BrokenElement->LinkEndChild(BrokenDisElement);
	TiXmlText *BrokenDisContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.BrokenDis));
	BrokenDisElement->LinkEndChild(BrokenDisContent);

	//Margin ����
	TiXmlElement *MarginElement = new TiXmlElement("Margin");
	BladePairElement->LinkEndChild(MarginElement);
	TiXmlText *MarginContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.Margin));
	MarginElement->LinkEndChild(MarginContent);
	
	//Jmin ����������۵��ϼ�ҶƬ�ľ��� J��С����
	TiXmlElement *JminElement = new TiXmlElement("Jmin");
	BladePairElement->LinkEndChild(JminElement);
	TiXmlText *JminContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.Jmin));
	JminElement->LinkEndChild(JminContent);

	//Hmin ����������۵��¼�ҶƬ�ľ��� H��С����
	TiXmlElement *HminElement = new TiXmlElement("Hmin");
	BladePairElement->LinkEndChild(HminElement);
	TiXmlText *HminContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.Hmin));
	HminElement->LinkEndChild(HminContent);

	//P<!--  ���Ͳ�  -->
	TiXmlElement *PElement = new TiXmlElement("P");
	BladePairElement->LinkEndChild(PElement);
	TiXmlText *PContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.P));
	PElement->LinkEndChild(PContent);

	//Q<!--  ���Ͳ�  -->
	TiXmlElement *QElement = new TiXmlElement("Q");
	BladePairElement->LinkEndChild(QElement);
	TiXmlText *QContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.Q));
	QElement->LinkEndChild(QContent);

	//W<!--  ��̬�Ͳ�  ��ȡ�����ǲ�ľ�ֵ  -->
	TiXmlElement *WElement = new TiXmlElement("W");
	BladePairElement->LinkEndChild(WElement);
	TiXmlText *WContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.W));
	WElement->LinkEndChild(WContent);

	/*N<!--  ÿ�������Ĳ��÷�����������еĹ�ϵNnֵ����ʱ��P31��ͬ
	������1����������Ҳ���Ϊ�����ã�������1�������������������ã����壺N1=0
	������1����������Ҳ���Ϊ�����ã�������1�������������ҷ����ã����壺N1=1
	-->*/
	TiXmlElement *NElement = new TiXmlElement("N");
	BladePairElement->LinkEndChild(NElement);
	TiXmlText *NContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.N));
	NElement->LinkEndChild(NContent);


	//Blade//��Ҷ
	TiXmlElement *BladeElement = new TiXmlElement("Blade");
	BladePairElement->LinkEndChild(BladeElement);
	
	//BladeType ҶƬ���� //��Ҷ
	TiXmlElement *BladeTypeElement = new TiXmlElement("BladeType");
	BladeElement->LinkEndChild(BladeTypeElement);
	TiXmlText *BladeTypeContent = new TiXmlText("stator");
	BladeTypeElement->LinkEndChild(BladeTypeContent);

	//Profile
	TiXmlElement *ProfileElement = new TiXmlElement("Profile");
	BladeElement->LinkEndChild(ProfileElement);

	////p0Jz ԭ�㵽��n����Ҷ�����ߵľ���
	//TiXmlElement *p0JzElement = new TiXmlElement("p0Jz");
	//ProfileElement->LinkEndChild(p0JzElement);
	//TiXmlText *p0JzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p0Jz));
	//p0JzElement->LinkEndChild(p0JzContent);

	//p1BG ԭ�㵽��n�����ȸ�����BG
	TiXmlElement *p1BGElement = new TiXmlElement("p1BG");
	ProfileElement->LinkEndChild(p1BGElement);
	TiXmlText *p1BGContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p1BG));
	p1BGElement->LinkEndChild(p1BGContent);

	//p2BD ������BD
	TiXmlElement *p2BDElement = new TiXmlElement("p2BD");
	ProfileElement->LinkEndChild(p2BDElement);
	TiXmlText *p2BDContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p2BD));
	p2BDElement->LinkEndChild(p2BDContent);

	//p3bg ������bg
	TiXmlElement *p3bgElement = new TiXmlElement("p3bg");
	ProfileElement->LinkEndChild(p3bgElement);
	TiXmlText *p3bgContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p3bg));
	p3bgElement->LinkEndChild(p3bgContent);

	//p4bd ������bd
	TiXmlElement *p4bdElement = new TiXmlElement("p4bd");
	ProfileElement->LinkEndChild(p4bdElement);
	TiXmlText *p4bdContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p4bd));
	p4bdElement->LinkEndChild(p4bdContent);

	//p5Bw Χ�����Bw
	TiXmlElement *p5BwElement = new TiXmlElement("p5Bw");
	ProfileElement->LinkEndChild(p5BwElement);
	TiXmlText *p5BwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p5Bw));
	p5BwElement->LinkEndChild(p5BwContent);

	//p6bw �����ߵ������߾���bw
	TiXmlElement *p6bwElement = new TiXmlElement("p6bw");
	ProfileElement->LinkEndChild(p6bwElement);
	TiXmlText *p6bwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p6bw));
	p6bwElement->LinkEndChild(p6bwContent);

	//p7hw Χ���߶�hw
	TiXmlElement *p7hwElement = new TiXmlElement("p7hw");
	ProfileElement->LinkEndChild(p7hwElement);
	TiXmlText *p7hwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p7hw));
	p7hwElement->LinkEndChild(p7hwContent);

	//p8Bz �м�����Bz
	TiXmlElement *p8BzElement = new TiXmlElement("p8Bz");
	ProfileElement->LinkEndChild(p8BzElement);
	TiXmlText *p8BzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz));
	p8BzElement->LinkEndChild(p8BzContent);

	//p9bz �����ߵ������߾���bz
	TiXmlElement *p9bzElement = new TiXmlElement("p9bz");
	ProfileElement->LinkEndChild(p9bzElement);
	TiXmlText *p9bzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p9bz));
	p9bzElement->LinkEndChild(p9bzContent);

	//p10hz �м���߶�hz
	TiXmlElement *p10hzElement = new TiXmlElement("p10hz");
	ProfileElement->LinkEndChild(p10hzElement);
	TiXmlText *p10hzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p10hz));
	p10hzElement->LinkEndChild(p10hzContent);

	//p26Dpp ������ƽ��ֱ��Dpp
	TiXmlElement *p26DppElement = new TiXmlElement("p26Dpp");
	ProfileElement->LinkEndChild(p26DppElement);
	TiXmlText *p26DppContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p26Dpp));
	p26DppElement->LinkEndChild(p26DppContent);

	//p27Lp �����߸߶�Lp
	TiXmlElement *p27LpElement = new TiXmlElement("p27Lp");
	ProfileElement->LinkEndChild(p27LpElement);
	TiXmlText *p27LpContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p27Lp));
	p27LpElement->LinkEndChild(p27LpContent);

	//p11ad �������ŽǦ�d
	TiXmlElement *p11adElement = new TiXmlElement("p11ad");
	ProfileElement->LinkEndChild(p11adElement);
	TiXmlText *p11adContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p11ad));
	p11adElement->LinkEndChild(p11adContent);

	//p12ag �������ŽǦ�g
	TiXmlElement *p12agElement = new TiXmlElement("p12ag");
	ProfileElement->LinkEndChild(p12agElement);
	TiXmlText *p12agContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p12ag));
	p12agElement->LinkEndChild(p12agContent);
	
	//<Hub><!--  ��ҶҶ��  -->
	ExportXMLData( BladeElement, 0 ,tmp);
	//<Shroud><!--  ��ҶΧ��  -->
	ExportShroudXMLData( BladeElement, 0 ,tmp);


	//Blade//��Ҷ******************************************************************************
	BladeElement = new TiXmlElement("Blade");
	BladePairElement->LinkEndChild(BladeElement);

	//BladeType ҶƬ���� //��Ҷ
	BladeTypeElement = new TiXmlElement("BladeType");
	BladeElement->LinkEndChild(BladeTypeElement);
	BladeTypeContent = new TiXmlText("rotor");
	BladeTypeElement->LinkEndChild(BladeTypeContent);

	//Profile
	ProfileElement = new TiXmlElement("Profile");
	BladeElement->LinkEndChild(ProfileElement);

	////p30Dz ԭ�㵽��n����Ҷ�����ߵľ���
	//TiXmlElement *p30DzElement = new TiXmlElement("p30Dz");
	//ProfileElement->LinkEndChild(p30DzElement);
	//TiXmlText *p30DzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p30Dz));
	//p30DzElement->LinkEndChild(p30DzContent);

	//p13BG ԭ�㵽��n�����ȸ�����BG
	TiXmlElement *p13BGElement = new TiXmlElement("p13BG");
	ProfileElement->LinkEndChild(p13BGElement);
	TiXmlText *p13BGContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p13BG));
	p13BGElement->LinkEndChild(p13BGContent);

	//p14BD ������BD
	TiXmlElement *p14BDElement = new TiXmlElement("p14BD");
	ProfileElement->LinkEndChild(p14BDElement);
	TiXmlText *p14BDContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p14BD));
	p14BDElement->LinkEndChild(p14BDContent);

	//p15bg ������bg
	TiXmlElement *p15bgElement = new TiXmlElement("p15bg");
	ProfileElement->LinkEndChild(p15bgElement);
	TiXmlText *p15bgContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p15bg));
	p15bgElement->LinkEndChild(p15bgContent);

	//p16bd ������bd
	TiXmlElement *p16bdElement = new TiXmlElement("p16bd");
	ProfileElement->LinkEndChild(p16bdElement);
	TiXmlText *p16bdContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p16bd));
	p16bdElement->LinkEndChild(p16bdContent);

	//p17Bw Χ�����Bw
	TiXmlElement *p17BwElement = new TiXmlElement("p17Bw");
	ProfileElement->LinkEndChild(p17BwElement);
	TiXmlText *p17BwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p17Bw));
	p17BwElement->LinkEndChild(p17BwContent);

	//p18bw �����ߵ������߾���bw
	TiXmlElement *p18bwElement = new TiXmlElement("p18bw");
	ProfileElement->LinkEndChild(p18bwElement);
	TiXmlText *p18bwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p18bw));
	p18bwElement->LinkEndChild(p18bwContent);

	//p19hw Χ���߶�hw
	TiXmlElement *p19hwElement = new TiXmlElement("p19hw");
	ProfileElement->LinkEndChild(p19hwElement);
	TiXmlText *p19hwContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p19hw));
	p19hwElement->LinkEndChild(p19hwContent);

	//p21Bz �м�����Bz
	TiXmlElement *p21BzElement = new TiXmlElement("p21Bz");
	ProfileElement->LinkEndChild(p21BzElement);
	TiXmlText *p21BzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz));
	p21BzElement->LinkEndChild(p21BzContent);

	//p22bz �����ߵ������߾���bz
	TiXmlElement *p22bzElement = new TiXmlElement("p22bz");
	ProfileElement->LinkEndChild(p22bzElement);
	TiXmlText *p22bzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p22bz));
	p22bzElement->LinkEndChild(p22bzContent);

	//p23hz �м���߶�hz
	TiXmlElement *p23hzElement = new TiXmlElement("p23hz");
	ProfileElement->LinkEndChild(p23hzElement);
	TiXmlText *p23hzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p23hz));
	p23hzElement->LinkEndChild(p23hzContent);

	//p28Dpd ������ƽ��ֱ��Dpd
	TiXmlElement *p28DpdElement = new TiXmlElement("p28Dpd");
	ProfileElement->LinkEndChild(p28DpdElement);
	TiXmlText *p28DpdContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p28Dpd));
	p28DpdElement->LinkEndChild(p28DpdContent);

	//p29Ld �����߸߶�Ld
	TiXmlElement *p29LdElement = new TiXmlElement("p29Ld");
	ProfileElement->LinkEndChild(p29LdElement);
	TiXmlText *p29LdContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p29Ld));
	p29LdElement->LinkEndChild(p29LdContent);

	//p24ad �������ŽǦ�d
	TiXmlElement *p24adElement = new TiXmlElement("p24ad");
	ProfileElement->LinkEndChild(p24adElement);
	TiXmlText *p24adContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p24ad));
	p24adElement->LinkEndChild(p24adContent);

	//p25ag �������ŽǦ�g
	TiXmlElement *p25agElement = new TiXmlElement("p25ag");
	ProfileElement->LinkEndChild(p25agElement);
	TiXmlText *p25agContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p25ag));
	p25agElement->LinkEndChild(p25agContent);

	//<Hub><!--  ��ҶҶ��  -->
	ExportXMLData( BladeElement, 1 ,tmp);
	//<Shroud><!--  ��ҶΧ��  -->
	ExportShroudXMLData( BladeElement, 1 ,tmp);
	return true;

}


//Ҷ���ڵ����ݵ���
bool ClassTongliu::ExportXMLData( TiXmlElement* pElem, int HubType ,bladeData tmp)
{
	CString str;
	TiXmlElement *HubElement = NULL;

#pragma region
	//��Ҷ
	if (HubType == 0)
	{
		//<!--  ��ҶҶ��  -->
		HubElement = new TiXmlElement("StatorHub");
		pElem->LinkEndChild(HubElement);

		//HubMType<!--  Ҷ������  -->
		TiXmlElement *HubMTypeElement = new TiXmlElement("HubMType");
		HubElement->LinkEndChild(HubMTypeElement);
		TiXmlText *HubMTypContent = new TiXmlText("����ʽԤŤTҶ��");
		HubMTypeElement->LinkEndChild(HubMTypContent);

		//HubType<!--  Ҷ�����ߺ�  -->
		TiXmlElement *HubTypeElement = new TiXmlElement("HubType");
		HubElement->LinkEndChild(HubTypeElement);
		TiXmlText *HubTypeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.SHubType));
		HubTypeElement->LinkEndChild(HubTypeContent);

		//HubPart<!--  Ҷ������  -->//////////////////////////////////
		TiXmlElement *HubPartElement = new TiXmlElement("HubPart");
		HubElement->LinkEndChild(HubPartElement);

		//bn<!--  Ҷ���������bn  -->
		TiXmlElement *bnElement = new TiXmlElement("bn");
		HubPartElement->LinkEndChild(bnElement);
		TiXmlText *bnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*32.0/56.0));
		bnElement->LinkEndChild(bnContent);

		//hn<!--  Ҷ�������߶�hn  -->
		TiXmlElement *hnElement = new TiXmlElement("hn");
		HubPartElement->LinkEndChild(hnElement);
		TiXmlText *hnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*12.5/56.0));
		hnElement->LinkEndChild(hnContent);

		//hx><!--  Ҷ���²��߶�hx  -->
		TiXmlElement *hxElement = new TiXmlElement("hx");
		HubPartElement->LinkEndChild(hxElement);
		TiXmlText *hxContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*12.5/56.0));
		hxElement->LinkEndChild(hxContent);

		//bz<!--  �м��������ȵ�һ��bz  -->
		TiXmlElement *bzElement = new TiXmlElement("bz");
		HubPartElement->LinkEndChild(bzElement);
		TiXmlText *bzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*28/56.0));
		bzElement->LinkEndChild(bzContent);

		//daojiao_c><!--  Ҷ���˲������ߵ���[mm]daojiaoc=  -->
		TiXmlElement *daojiao_cElement = new TiXmlElement("daojiao_c");
		HubPartElement->LinkEndChild(daojiao_cElement);
		TiXmlText *daojiao_cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*4.0/56.0));
		daojiao_cElement->LinkEndChild(daojiao_cContent);

		//daojiao_cb><!--  Ҷ���²��������ϵ���[mm]daojiaocb=����ҶΪҶ���ϲ��������µ��ǣ�  -->
		TiXmlElement *daojiao_cbElement = new TiXmlElement("daojiao_cb");
		HubPartElement->LinkEndChild(daojiao_cbElement);
		TiXmlText *daojiao_cbContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*1.0/56.0));
		daojiao_cbElement->LinkEndChild(daojiao_cbContent);

		//daojiao_r><!--  Ҷ���������ܵ�Բ��[mm]daojiaor=  -->
		TiXmlElement *daojiao_rElement = new TiXmlElement("daojiao_r");
		HubPartElement->LinkEndChild(daojiao_rElement);
		TiXmlText *daojiao_rContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*3.0/56.0));
		daojiao_rElement->LinkEndChild(daojiao_rContent);

		//rcj><!--  Ҷ�����������м����ڲ۰뾶rcj=  -->
		TiXmlElement *rcjElement = new TiXmlElement("rcj");
		HubPartElement->LinkEndChild(rcjElement);
		TiXmlText *rcjContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*2.0/56.0));
		rcjElement->LinkEndChild(rcjContent);

		//HubHorizontalAngle><!--  Ҷ����������ˮƽ�н�  -->
		TiXmlElement *HubHorizontalAngleElement = new TiXmlElement("HubHorizontalAngle");
		HubPartElement->LinkEndChild(HubHorizontalAngleElement);
		TiXmlText *HubHorizontalAngleContent = new TiXmlText("0");
		HubHorizontalAngleElement->LinkEndChild(HubHorizontalAngleContent);


		//HubGroove><!--  Ҷ���۲���  -->//////////////////////////////////
		TiXmlElement *HubGrooveElement = new TiXmlElement("HubGroove");
		HubElement->LinkEndChild(HubGrooveElement);

		//hy><!--  Ҷ�������߳���ת�����hy  -->
		TiXmlElement *hyElement = new TiXmlElement("hy");
		HubGrooveElement->LinkEndChild(hyElement);
		TiXmlText *hyContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*5.0/56.0));
		hyElement->LinkEndChild(hyContent);

		//InterstitialsR><!--  ��϶���뾶  -->
		TiXmlElement *InterstitialsRElement = new TiXmlElement("InterstitialsR");
		HubGrooveElement->LinkEndChild(InterstitialsRElement);
		TiXmlText *InterstitialsRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*5.0/56.0));
		InterstitialsRElement->LinkEndChild(InterstitialsRContent);

		//GrooveSurfaceH><!--  �м����±��浽Ҷ�����±���߶�  -->
		TiXmlElement *GrooveSurfaceHElement = new TiXmlElement("GrooveSurfaceH");
		HubGrooveElement->LinkEndChild(GrooveSurfaceHElement);
		TiXmlText *GrooveSurfaceHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*26.0/56.0));
		GrooveSurfaceHElement->LinkEndChild(GrooveSurfaceHContent);

		//GrooveNeckH><!--  Ҷ���۾����߶�  -->
		TiXmlElement *GrooveNeckHElement = new TiXmlElement("GrooveNeckH");
		HubGrooveElement->LinkEndChild(GrooveNeckHElement);
		TiXmlText *GrooveNeckHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*12.5/56.0));
		GrooveNeckHElement->LinkEndChild(GrooveNeckHContent);

		//GrooveBottomH><!--  Ҷ�����²��߶ȣ�������϶���۵ĸ߶ȣ�  -->
		TiXmlElement *GrooveBottomHElement = new TiXmlElement("GrooveBottomH");
		HubGrooveElement->LinkEndChild(GrooveBottomHElement);
		TiXmlText *GrooveBottomHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*18.0/56.0));
		GrooveBottomHElement->LinkEndChild(GrooveBottomHContent);

		//MidBodyH><!--  Ҷ�����м�����  -->
		TiXmlElement *MidBodyHElement = new TiXmlElement("MidBodyH");
		HubGrooveElement->LinkEndChild(MidBodyHElement);
		TiXmlText *MidBodyHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*56.0/56.0));
		MidBodyHElement->LinkEndChild(MidBodyHContent);

		//GrooveNeckW><!--  Ҷ���۾������  -->
		TiXmlElement *GrooveNeckWElement = new TiXmlElement("GrooveNeckW");
		HubGrooveElement->LinkEndChild(GrooveNeckWElement);
		TiXmlText *GrooveNeckWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*36.4/56.0));
		GrooveNeckWElement->LinkEndChild(GrooveNeckWContent);

		//GrooveNeckChamfer><!--  Ҷ���۾�������  -->
		TiXmlElement *GrooveNeckChamferElement = new TiXmlElement("GrooveNeckChamfer");
		HubGrooveElement->LinkEndChild(GrooveNeckChamferElement);
		TiXmlText *GrooveNeckChamferContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*1.0/56.0));
		GrooveNeckChamferElement->LinkEndChild(GrooveNeckChamferContent);

		//GrooveBottomW><!--  Ҷ�����²���ȣ������಻Ϊ��ֱ������󴦿�ȣ�  -->
		TiXmlElement *GrooveBottomWElement = new TiXmlElement("GrooveBottomW");
		HubGrooveElement->LinkEndChild(GrooveBottomWElement);
		TiXmlText *GrooveBottomWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*57.4/56.0));
		GrooveBottomWElement->LinkEndChild(GrooveBottomWContent);

		//GrooveVerticalAngle><!--  Ҷ�����²�����ֱ�н�  -->
		TiXmlElement *GrooveVerticalAngleElement = new TiXmlElement("GrooveVerticalAngle");
		HubGrooveElement->LinkEndChild(GrooveVerticalAngleElement);
		TiXmlText *GrooveVerticalAngleContent = new TiXmlText("0");
		GrooveVerticalAngleElement->LinkEndChild(GrooveVerticalAngleContent);

		//UCornerR><!--  Ҷ�����²��ϵ�Բ����ҶΪ�ֲ��ϲ��µ�Բ��  -->
		TiXmlElement *UCornerRElement = new TiXmlElement("UCornerR");
		HubGrooveElement->LinkEndChild(UCornerRElement);
		TiXmlText *UCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*1.0/56.0));
		UCornerRElement->LinkEndChild(UCornerRContent);

		//DCornerR><!--  Ҷ�����²��µ�Բ����ҶΪ�ֲ��ϲ��ϵ�Բ��  -->
		TiXmlElement *DCornerRElement = new TiXmlElement("DCornerR");
		HubGrooveElement->LinkEndChild(DCornerRElement);
		TiXmlText *DCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p8Bz*4.0/56.0));
		DCornerRElement->LinkEndChild(DCornerRContent);

		
	}
#pragma endregion

	//��Ҷ
	if (HubType == 1)
	{
		HubElement = new TiXmlElement("RotorHub");
		pElem->LinkEndChild(HubElement);
		
		//�ж����ߺ�//����ʽԤŤTҶ��
		if(tmp.HubType == 1)
#pragma region
		{
			//HubMType<!--  Ҷ������  -->
			TiXmlElement *HubMTypeElement = new TiXmlElement("HubMType");
			HubElement->LinkEndChild(HubMTypeElement);
			TiXmlText *HubMTypContent = new TiXmlText("����ʽԤŤTҶ��");
			HubMTypeElement->LinkEndChild(HubMTypContent);

			//HubType<!--  Ҷ�����ߺ�  -->
			TiXmlElement *HubTypeElement = new TiXmlElement("HubType");
			HubElement->LinkEndChild(HubTypeElement);
			TiXmlText *HubTypeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.HubType));
			HubTypeElement->LinkEndChild(HubTypeContent);

			//HubPart<!--  Ҷ������  -->//////////////////////////////////
			TiXmlElement *HubPartElement = new TiXmlElement("HubPart");
			HubElement->LinkEndChild(HubPartElement);

			//bn<!--  Ҷ���������bn  -->
			TiXmlElement *bnElement = new TiXmlElement("bn");
			HubPartElement->LinkEndChild(bnElement);
			TiXmlText *bnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*23.5/45.0));
			bnElement->LinkEndChild(bnContent);

			//hn<!--  Ҷ�������߶�hn  -->
			TiXmlElement *hnElement = new TiXmlElement("hn");
			HubPartElement->LinkEndChild(hnElement);
			TiXmlText *hnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*25.0/45.0));
			hnElement->LinkEndChild(hnContent);

			//hx><!--  Ҷ���²��߶�hx  -->
			TiXmlElement *hxElement = new TiXmlElement("hx");
			HubPartElement->LinkEndChild(hxElement);
			TiXmlText *hxContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.0/45.0));
			hxElement->LinkEndChild(hxContent);

			//bz<!--  �м��������ȵ�һ��bz  -->
			TiXmlElement *bzElement = new TiXmlElement("bz");
			HubPartElement->LinkEndChild(bzElement);
			TiXmlText *bzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.5/45.0));
			bzElement->LinkEndChild(bzContent);

			//daojiao_c><!--  Ҷ���˲������ߵ���[mm]daojiaoc=  -->
			TiXmlElement *daojiao_cElement = new TiXmlElement("daojiao_c");
			HubPartElement->LinkEndChild(daojiao_cElement);
			TiXmlText *daojiao_cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*6.0/45.0));
			daojiao_cElement->LinkEndChild(daojiao_cContent);

			//daojiao_cb><!--  Ҷ���²��������ϵ���[mm]daojiaocb=����ҶΪҶ���ϲ��������µ��ǣ�  -->
			TiXmlElement *daojiao_cbElement = new TiXmlElement("daojiao_cb");
			HubPartElement->LinkEndChild(daojiao_cbElement);
			TiXmlText *daojiao_cbContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.1/45.0));
			daojiao_cbElement->LinkEndChild(daojiao_cbContent);

			//daojiao_r><!--  Ҷ���������ܵ�Բ��[mm]daojiaor=  -->
			TiXmlElement *daojiao_rElement = new TiXmlElement("daojiao_r");
			HubPartElement->LinkEndChild(daojiao_rElement);
			TiXmlText *daojiao_rContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.5/45.0));
			daojiao_rElement->LinkEndChild(daojiao_rContent);

			//rcj><!--  Ҷ�����������м����ڲ۰뾶rcj=  -->
			TiXmlElement *rcjElement = new TiXmlElement("rcj");
			HubPartElement->LinkEndChild(rcjElement);
			TiXmlText *rcjContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.0/45.0));
			rcjElement->LinkEndChild(rcjContent);

			//HubHorizontalAngle><!--  Ҷ����������ˮƽ�н�  -->
			TiXmlElement *HubHorizontalAngleElement = new TiXmlElement("HubHorizontalAngle");
			HubPartElement->LinkEndChild(HubHorizontalAngleElement);
			TiXmlText *HubHorizontalAngleContent = new TiXmlText("0");
			HubHorizontalAngleElement->LinkEndChild(HubHorizontalAngleContent);


			//HubGroove><!--  Ҷ���۲���  -->//////////////////////////////////
			TiXmlElement *HubGrooveElement = new TiXmlElement("HubGroove");
			HubElement->LinkEndChild(HubGrooveElement);

			//hy><!--  Ҷ�������߳���ת�����hy  -->
			TiXmlElement *hyElement = new TiXmlElement("hy");
			HubGrooveElement->LinkEndChild(hyElement);
			TiXmlText *hyContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*8.0/45.0));
			hyElement->LinkEndChild(hyContent);

			//InterstitialsR><!--  ��϶���뾶  -->
			TiXmlElement *InterstitialsRElement = new TiXmlElement("InterstitialsR");
			HubGrooveElement->LinkEndChild(InterstitialsRElement);
			TiXmlText *InterstitialsRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*5.0/45.0));
			InterstitialsRElement->LinkEndChild(InterstitialsRContent);

			//GrooveSurfaceH><!--  �м����±��浽Ҷ�����±���߶�  -->
			TiXmlElement *GrooveSurfaceHElement = new TiXmlElement("GrooveSurfaceH");
			HubGrooveElement->LinkEndChild(GrooveSurfaceHElement);
			TiXmlText *GrooveSurfaceHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*48.0/45.0));
			GrooveSurfaceHElement->LinkEndChild(GrooveSurfaceHContent);

			//GrooveNeckH><!--  Ҷ���۾����߶�  -->
			TiXmlElement *GrooveNeckHElement = new TiXmlElement("GrooveNeckH");
			HubGrooveElement->LinkEndChild(GrooveNeckHElement);
			TiXmlText *GrooveNeckHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*25.0/45.0));
			GrooveNeckHElement->LinkEndChild(GrooveNeckHContent);

			//GrooveBottomH><!--  Ҷ�����²��߶ȣ�������϶���۵ĸ߶ȣ�  -->
			TiXmlElement *GrooveBottomHElement = new TiXmlElement("GrooveBottomH");
			HubGrooveElement->LinkEndChild(GrooveBottomHElement);
			TiXmlText *GrooveBottomHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*27.5/45.0));
			GrooveBottomHElement->LinkEndChild(GrooveBottomHContent);

			//MidBodyH><!--  Ҷ�����м�����  -->
			TiXmlElement *MidBodyHElement = new TiXmlElement("MidBodyH");
			HubGrooveElement->LinkEndChild(MidBodyHElement);
			TiXmlText *MidBodyHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*45.0/45.0));
			MidBodyHElement->LinkEndChild(MidBodyHContent);

			//GrooveNeckW><!--  Ҷ���۾������  -->
			TiXmlElement *GrooveNeckWElement = new TiXmlElement("GrooveNeckW");
			HubGrooveElement->LinkEndChild(GrooveNeckWElement);
			TiXmlText *GrooveNeckWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*29.0/45.0));
			GrooveNeckWElement->LinkEndChild(GrooveNeckWContent);

			//GrooveNeckChamfer><!--  Ҷ���۾�������  -->
			TiXmlElement *GrooveNeckChamferElement = new TiXmlElement("GrooveNeckChamfer");
			HubGrooveElement->LinkEndChild(GrooveNeckChamferElement);
			TiXmlText *GrooveNeckChamferContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/45.0));
			GrooveNeckChamferElement->LinkEndChild(GrooveNeckChamferContent);

			//GrooveBottomW><!--  Ҷ�����²���ȣ������಻Ϊ��ֱ������󴦿�ȣ�  -->
			TiXmlElement *GrooveBottomWElement = new TiXmlElement("GrooveBottomW");
			HubGrooveElement->LinkEndChild(GrooveBottomWElement);
			TiXmlText *GrooveBottomWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*47.0/45.0));
			GrooveBottomWElement->LinkEndChild(GrooveBottomWContent);

			//GrooveVerticalAngle><!--  Ҷ�����²�����ֱ�н�  -->
			TiXmlElement *GrooveVerticalAngleElement = new TiXmlElement("GrooveVerticalAngle");
			HubGrooveElement->LinkEndChild(GrooveVerticalAngleElement);
			TiXmlText *GrooveVerticalAngleContent = new TiXmlText("0");
			GrooveVerticalAngleElement->LinkEndChild(GrooveVerticalAngleContent);

			//UCornerR><!--  Ҷ�����²��ϵ�Բ����ҶΪ�ֲ��ϲ��µ�Բ��  -->
			TiXmlElement *UCornerRElement = new TiXmlElement("UCornerR");
			HubGrooveElement->LinkEndChild(UCornerRElement);
			TiXmlText *UCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.8/45.0));
			UCornerRElement->LinkEndChild(UCornerRContent);

			//DCornerR><!--  Ҷ�����²��µ�Բ����ҶΪ�ֲ��ϲ��ϵ�Բ��  -->
			TiXmlElement *DCornerRElement = new TiXmlElement("DCornerR");
			HubGrooveElement->LinkEndChild(DCornerRElement);
			TiXmlText *DCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*9.0/45.0));
			DCornerRElement->LinkEndChild(DCornerRContent);

		}
#pragma endregion
		//�嶯ʽ��ԤŤTҶ��
		else if(tmp.HubType == 2)
#pragma region
		{
			//HubMType<!--  Ҷ������  -->
			TiXmlElement *HubMTypeElement = new TiXmlElement("HubMType");
			HubElement->LinkEndChild(HubMTypeElement);
			TiXmlText *HubMTypContent = new TiXmlText("�嶯ʽ��ԤŤTҶ��");
			HubMTypeElement->LinkEndChild(HubMTypContent);

			//HubType<!--  Ҷ�����ߺ�  -->
			TiXmlElement *HubTypeElement = new TiXmlElement("HubType");
			HubElement->LinkEndChild(HubTypeElement);
			TiXmlText *HubTypeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.HubType));
			HubTypeElement->LinkEndChild(HubTypeContent);

			//HubPart<!--  Ҷ������  -->//////////////////////////////////
			TiXmlElement *HubPartElement = new TiXmlElement("HubPart");
			HubElement->LinkEndChild(HubPartElement);

			//MidBodySurW<!--  �м����±���ȥ��С�ſ��  -->
			TiXmlElement *MidBodySurWElement = new TiXmlElement("MidBodySurW");
			HubPartElement->LinkEndChild(MidBodySurWElement);
			TiXmlText *MidBodySurWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*24.0/50.0));
			MidBodySurWElement->LinkEndChild(MidBodySurWContent);

			//bn<!--  Ҷ���������bn  -->
			TiXmlElement *bnElement = new TiXmlElement("bn");
			HubPartElement->LinkEndChild(bnElement);
			TiXmlText *bnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*11.0/50.0));
			bnElement->LinkEndChild(bnContent);

			//hn<!--  Ҷ�������߶�hn  -->
			TiXmlElement *hnElement = new TiXmlElement("hn");
			HubPartElement->LinkEndChild(hnElement);
			TiXmlText *hnContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.0/50.0));
			hnElement->LinkEndChild(hnContent);

			//HubDW><!--  Ҷ���²����  -->
			TiXmlElement *HubDWElement = new TiXmlElement("HubDW"); 
			HubPartElement->LinkEndChild(HubDWElement);
			TiXmlText *HubDWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*21.0/50.0));
			HubDWElement->LinkEndChild(HubDWContent);

			//hx><!--  Ҷ���²��߶�hx  -->
			TiXmlElement *hxElement = new TiXmlElement("hx");
			HubPartElement->LinkEndChild(hxElement);
			TiXmlText *hxContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*14.0/50.0));
			hxElement->LinkEndChild(hxContent);

			//TSFH><!--  TҶ��С�Ÿ߶�  ����С�ţ�����Ϊ0-->
			TiXmlElement *TSFHElement = new TiXmlElement("TSFH");
			HubPartElement->LinkEndChild(TSFHElement);
			TiXmlText *TSFHContent = new TiXmlText("5");
			TSFHElement->LinkEndChild(TSFHContent);

			//TUestW><!--  TҶ�������϶������  -->
			TiXmlElement *TUestWElement = new TiXmlElement("TUestW");
			HubPartElement->LinkEndChild(TUestWElement);
			TiXmlText *TUestWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*11.0/50.0));
			TUestWElement->LinkEndChild(TUestWContent);

			//TZAngle><!--  TҶ��������б����Z��н�  -->
			TiXmlElement *TZAngleElement = new TiXmlElement("TZAngle");
			HubPartElement->LinkEndChild(TZAngleElement);
			TiXmlText *TZAngleContent = new TiXmlText("0");
			TZAngleElement->LinkEndChild(TZAngleContent);

			//daojiao_c><!--  Ҷ���˲������ߵ���[mm]daojiaoc=  -->
			TiXmlElement *daojiao_cElement = new TiXmlElement("daojiao_c");
			HubPartElement->LinkEndChild(daojiao_cElement);
			TiXmlText *daojiao_cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*3.0/50.0));
			daojiao_cElement->LinkEndChild(daojiao_cContent);

			//daojiao_cb><!--  Ҷ���²��������ϵ���[mm]daojiaocb=����ҶΪҶ���ϲ��������µ��ǣ�  -->
			TiXmlElement *daojiao_cbElement = new TiXmlElement("daojiao_cb");
			HubPartElement->LinkEndChild(daojiao_cbElement);
			TiXmlText *daojiao_cbContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/50.0));
			daojiao_cbElement->LinkEndChild(daojiao_cbContent);

			//NeckDCornerR><!--  Ҷ�������µ�Բ  -->
			TiXmlElement *NeckDCornerRElement = new TiXmlElement("NeckDCornerR");
			HubPartElement->LinkEndChild(NeckDCornerRElement);
			TiXmlText *NeckDCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			NeckDCornerRElement->LinkEndChild(NeckDCornerRContent);

			//NeckUCornerR><!--  Ҷ�������ϵ�Բ  -->
			TiXmlElement *NeckUCornerRElement = new TiXmlElement("NeckUCornerR");
			HubPartElement->LinkEndChild(NeckUCornerRElement);
			TiXmlText *NeckUCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.0/50.0));
			NeckUCornerRElement->LinkEndChild(NeckUCornerRContent);

			//NeckMCornerR><!--  Ҷ�������е�Բ  -->
			TiXmlElement *NeckMCornerRElement = new TiXmlElement("NeckMCornerR");
			HubPartElement->LinkEndChild(NeckMCornerRElement);
			TiXmlText *NeckMCornerRContent = new TiXmlText("0.5");
			NeckMCornerRElement->LinkEndChild(NeckMCornerRContent);

			//TAngle><!--  TҶ����������ˮƽ�н�  -->
			TiXmlElement *TAngleElement = new TiXmlElement("TAngle");
			HubPartElement->LinkEndChild(TAngleElement);
			TiXmlText *TAngleContent = new TiXmlText("0");
			TAngleElement->LinkEndChild(TAngleContent);

			//TSFMidCornerR><!--  TҶ��С�����м�����Ӵ���Բ   -->
			TiXmlElement *TSFMidCornerRElement = new TiXmlElement("TSFMidCornerR");
			HubPartElement->LinkEndChild(TSFMidCornerRElement);
			TiXmlText *TSFMidCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			TSFMidCornerRElement->LinkEndChild(TSFMidCornerRContent);

			//SFInAngle><!--  TҶ��С���ڲ൹��  -->
			TiXmlElement *SFInAngleElement = new TiXmlElement("SFInAngle");
			HubPartElement->LinkEndChild(SFInAngleElement);
			TiXmlText *SFInAngleContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/50.0));
			SFInAngleElement->LinkEndChild(SFInAngleContent);

			//�ֲ۲���
			//HubGroove><!--  �ֲ۲���  -->//////////////////////////////////
			TiXmlElement *HubGrooveElement = new TiXmlElement("HubGroove");
			HubElement->LinkEndChild(HubGrooveElement);

			//NeckH><!--  �ֲ۾����߶�  -->
			TiXmlElement *NeckHElement = new TiXmlElement("NeckH");
			HubGrooveElement->LinkEndChild(NeckHElement);
			TiXmlText *NeckHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*22.0/50.0));
			NeckHElement->LinkEndChild(NeckHContent);

			//SFH><!--  �ֲ�С�Ÿ߶�  -->
			TiXmlElement *SFHElement = new TiXmlElement("SFH");
			HubGrooveElement->LinkEndChild(SFHElement);
			TiXmlText *SFHContent = new TiXmlText("5");
			SFHElement->LinkEndChild(SFHContent);

			//NeckDH><!--  �ֲ��²��߶�  -->
			TiXmlElement *NeckDHElement = new TiXmlElement("NeckDH");
			HubGrooveElement->LinkEndChild(NeckDHElement);
			TiXmlText *NeckDHContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*15.5/50.0));
			NeckDHElement->LinkEndChild(NeckDHContent);

			//NeckW><!--  �ֲ۾������  -->
			TiXmlElement *NeckWElement = new TiXmlElement("NeckW");
			HubGrooveElement->LinkEndChild(NeckWElement);
			TiXmlText *NeckWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*11.0/50.0));
			NeckWElement->LinkEndChild(NeckWContent);

			//NeckDW><!--  �ֲ��²����  -->
			TiXmlElement *NeckDWElement = new TiXmlElement("NeckDW");
			HubGrooveElement->LinkEndChild(NeckDWElement);
			TiXmlText *NeckDWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*21.4/50.0));
			NeckDWElement->LinkEndChild(NeckDWContent);

			//NeckUestW><!--  �ֲ۾����϶������  -->
			TiXmlElement *NeckUestWElement = new TiXmlElement("NeckUestW");
			HubGrooveElement->LinkEndChild(NeckUestWElement);
			TiXmlText *NeckUestWContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*11.0/50.0));
			NeckUestWElement->LinkEndChild(NeckUestWContent);

			//NeckZAngle><!--  �ֲ۾�����б����Z��н�  -->
			TiXmlElement *NeckZAngleElement = new TiXmlElement("NeckZAngle");
			HubGrooveElement->LinkEndChild(NeckZAngleElement);
			TiXmlText *NeckZAngleContent = new TiXmlText("0");
			NeckZAngleElement->LinkEndChild(NeckZAngleContent);

			//NeckUAngle><!--  �ֲ۾����ϵ��ǣ�������б��ʱΪ��Բ��  -->
			TiXmlElement *NeckUAngleElement = new TiXmlElement("NeckUAngle");
			HubGrooveElement->LinkEndChild(NeckUAngleElement);
			TiXmlText *NeckUAngleContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.5/50.0));
			NeckUAngleElement->LinkEndChild(NeckUAngleContent);

			//NeckDAngle><!--  �ֲ۾����µ���  -->
			TiXmlElement *NeckDAngleElement = new TiXmlElement("NeckDAngle");
			HubGrooveElement->LinkEndChild(NeckDAngleElement);
			TiXmlText *NeckDAngleContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/50.0));
			NeckDAngleElement->LinkEndChild(NeckDAngleContent);

			//RaceNeckMCornerR><!--  �ֲ۾����е�Բ  -->
			TiXmlElement *RaceNeckMCornerRElement = new TiXmlElement("RaceNeckMCornerR");
			HubGrooveElement->LinkEndChild(RaceNeckMCornerRElement);
			TiXmlText *RaceNeckMCornerRContent = new TiXmlText("0.5");
			RaceNeckMCornerRElement->LinkEndChild(RaceNeckMCornerRContent);

			//RaceUCornerR><!--  �ֲ��²��ϵ�Բ  -->
			TiXmlElement *RaceUCornerRElement = new TiXmlElement("RaceUCornerR");
			HubGrooveElement->LinkEndChild(RaceUCornerRElement);
			TiXmlText *RaceUCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			RaceUCornerRElement->LinkEndChild(RaceUCornerRContent);

			//RaceDCornerR><!--  �ֲ��²��µ�Բ  -->
			TiXmlElement *RaceDCornerRElement = new TiXmlElement("RaceDCornerR");
			HubGrooveElement->LinkEndChild(RaceDCornerRElement);
			TiXmlText *RaceDCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*3.0/50.0));
			RaceDCornerRElement->LinkEndChild(RaceDCornerRContent);

			//RaceSFAngle><!--  �ֲ�С����൹��  -->
			TiXmlElement *RaceSFAngleElement = new TiXmlElement("RaceSFAngle");
			HubGrooveElement->LinkEndChild(RaceSFAngleElement);
			TiXmlText *RaceSFAngleContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/50.0));
			RaceSFAngleElement->LinkEndChild(RaceSFAngleContent);

			//RaceSFCornerR><!--  �ֲ�С����൹Բ  -->
			TiXmlElement *RaceSFCornerRElement = new TiXmlElement("RaceSFCornerR");
			HubGrooveElement->LinkEndChild(RaceSFCornerRElement);
			TiXmlText *RaceSFCornerRContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			RaceSFCornerRElement->LinkEndChild(RaceSFCornerRContent);
		}
#pragma endregion
		//��ԤŤ˫��TҶ��
		else if(tmp.HubType == 3)
#pragma region
		{
			//HubMType<!--  Ҷ������  -->
			TiXmlElement *HubMTypeElement = new TiXmlElement("HubMType");
			HubElement->LinkEndChild(HubMTypeElement);
			TiXmlText *HubMTypContent = new TiXmlText("��ԤŤ˫��TҶ��");
			HubMTypeElement->LinkEndChild(HubMTypContent);

			//HubType<!--  Ҷ�����ߺ�  -->
			TiXmlElement *HubTypeElement = new TiXmlElement("HubType");
			HubElement->LinkEndChild(HubTypeElement);
			TiXmlText *HubTypeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.HubType));
			HubTypeElement->LinkEndChild(HubTypeContent);

			//HubPart<!--  Ҷ������  -->//////////////////////////////////
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

			//r1<!--  r1��Ҷ����  -->
			TiXmlElement *r1Element = new TiXmlElement("r1");
			HubPartElement->LinkEndChild(r1Element);
			TiXmlText *r1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/72.0));
			r1Element->LinkEndChild(r1Content);

			//r3<!--  r3��Ҷ����  -->
			TiXmlElement *r3Element = new TiXmlElement("r3");
			HubPartElement->LinkEndChild(r3Element);
			TiXmlText *r3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/72.0));
			r3Element->LinkEndChild(r3Content);

			//d1<!--  d1��Ҷ����  -->
			TiXmlElement *d1Element = new TiXmlElement("d1");
			HubPartElement->LinkEndChild(d1Element);
			TiXmlText *d1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.4/72.0));
			d1Element->LinkEndChild(d1Content);

			//d3<!--  d3��Ҷ����  -->
			TiXmlElement *d3Element = new TiXmlElement("d3");
			HubPartElement->LinkEndChild(d3Element);
			TiXmlText *d3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.4/72.0));
			d3Element->LinkEndChild(d3Content);

			//h<!--  С�Ÿ߶�h  -->
			TiXmlElement *hElement = new TiXmlElement("h");
			HubPartElement->LinkEndChild(hElement);
			TiXmlText *hContent = new TiXmlText("5");
			hElement->LinkEndChild(hContent);

			//HubSFTol<!--  Ҷ��������С�ſ�ȹ���  -->
			TiXmlElement *HubSFTolElement = new TiXmlElement("HubSFTol");
			HubPartElement->LinkEndChild(HubSFTolElement);
			TiXmlText *HubSFTolContent = new TiXmlText("0/-0.04");
			HubSFTolElement->LinkEndChild(HubSFTolContent);

			//NeckDisTol<!--  Ҷ����������������빫��  -->
			TiXmlElement *NeckDisTolElement = new TiXmlElement("NeckDisTol");
			HubPartElement->LinkEndChild(NeckDisTolElement);
			TiXmlText *NeckDisTolContent = new TiXmlText("+0.09/+0.04");
			NeckDisTolElement->LinkEndChild(NeckDisTolContent);

			//�ֲ۲���
			//HubGroove><!--  �ֲ۲���  -->//////////////////////////////////
			TiXmlElement *HubGrooveElement = new TiXmlElement("HubGroove");
			HubElement->LinkEndChild(HubGrooveElement);

			//r2><!--  r2���ֲۣ�  -->
			TiXmlElement *r2Element = new TiXmlElement("r2");
			HubGrooveElement->LinkEndChild(r2Element);
			TiXmlText *r2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/72.0));
			r2Element->LinkEndChild(r2Content);

			//r4><!--  r4���ֲۣ�  -->
			TiXmlElement *r4Element = new TiXmlElement("r4");
			HubGrooveElement->LinkEndChild(r4Element);
			TiXmlText *r4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.2/72.0));
			r4Element->LinkEndChild(r4Content);

			//d2><!--  d2���ֲۣ�  -->
			TiXmlElement *d2Element = new TiXmlElement("d2");
			HubGrooveElement->LinkEndChild(d2Element);
			TiXmlText *d2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.4/72.0));
			d2Element->LinkEndChild(d2Content);

			//d4><!--  d4���ֲۣ�  -->
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

			//lb4><!--  lb4Z��н�  -->
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
		//ԤŤ˫��TҶ��
		else if(tmp.HubType == 4)
#pragma region
		{
			//HubMType<!--  Ҷ������  -->
			TiXmlElement *HubMTypeElement = new TiXmlElement("HubMType");
			HubElement->LinkEndChild(HubMTypeElement);
			TiXmlText *HubMTypContent = new TiXmlText("ԤŤ˫��TҶ��");
			HubMTypeElement->LinkEndChild(HubMTypContent);

			//HubType<!--  Ҷ�����ߺ�  -->
			TiXmlElement *HubTypeElement = new TiXmlElement("HubType");
			HubElement->LinkEndChild(HubTypeElement);
			TiXmlText *HubTypeContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.HubType));
			HubTypeElement->LinkEndChild(HubTypeContent);

			//HubPart<!--  Ҷ������  -->//////////////////////////////////
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

			//r1<!--  r1��Ҷ����  -->
			TiXmlElement *r1Element = new TiXmlElement("r1");
			HubPartElement->LinkEndChild(r1Element);
			TiXmlText *r1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.5/50.0));
			r1Element->LinkEndChild(r1Content);

			//HubLength<!--  Ҷ���˲����ȣ�ȥ�����ǣ�  -->
			TiXmlElement *HubLengthElement = new TiXmlElement("HubLength");
			HubPartElement->LinkEndChild(HubLengthElement);
			TiXmlText *HubLengthContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*20.0/50.0));
			HubLengthElement->LinkEndChild(HubLengthContent);

			//r3<!--  r3��Ҷ����  -->
			TiXmlElement *r3Element = new TiXmlElement("r3");
			HubPartElement->LinkEndChild(r3Element);
			TiXmlText *r3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.5/50.0));
			r3Element->LinkEndChild(r3Content);

			//d1<!--  d1��Ҷ����  -->
			TiXmlElement *d1Element = new TiXmlElement("d1");
			HubPartElement->LinkEndChild(d1Element);
			TiXmlText *d1Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			d1Element->LinkEndChild(d1Content);

			//d3<!--  d3��Ҷ����  -->
			TiXmlElement *d3Element = new TiXmlElement("d3");
			HubPartElement->LinkEndChild(d3Element);
			TiXmlText *d3Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*0.6/50.0));
			d3Element->LinkEndChild(d3Content);

			//bz<!--  �м��������ȵ�һ��bz  -->
			TiXmlElement *bzElement = new TiXmlElement("bz");
			HubPartElement->LinkEndChild(bzElement);
			TiXmlText *bzContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*50.0/50.0));
			bzElement->LinkEndChild(bzContent);

			//hr<!--  Ҷ���߶�hr  -->
			TiXmlElement *hrElement = new TiXmlElement("hr");
			HubPartElement->LinkEndChild(hrElement);
			TiXmlText *hrContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*83.0/50.0));
			hrElement->LinkEndChild(hrContent);

			//daojiao_c><!--  Ҷ���˲������ߵ���[mm]daojiaoc=  -->
			TiXmlElement *daojiao_cElement = new TiXmlElement("daojiao_c");
			HubPartElement->LinkEndChild(daojiao_cElement);
			TiXmlText *daojiao_cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*6.0/50.0));
			daojiao_cElement->LinkEndChild(daojiao_cContent);

			//daojiao_cb><!--  Ҷ���²��������ϵ���[mm]daojiaocb=����ҶΪҶ���ϲ��������µ��ǣ�  -->
			TiXmlElement *daojiao_cbElement = new TiXmlElement("daojiao_cb");
			HubPartElement->LinkEndChild(daojiao_cbElement);
			TiXmlText *daojiao_cbContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			daojiao_cbElement->LinkEndChild(daojiao_cbContent);

			//daojiao_r><!--  Ҷ���������ܵ�Բ��[mm]daojiaor=  -->
			TiXmlElement *daojiao_rElement = new TiXmlElement("daojiao_r");
			HubPartElement->LinkEndChild(daojiao_rElement);
			TiXmlText *daojiao_rContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*2.5/50.0));
			daojiao_rElement->LinkEndChild(daojiao_rContent);

			//rcj><!--  Ҷ�����������м����ڲ۰뾶rcj=  -->
			TiXmlElement *rcjElement = new TiXmlElement("rcj");
			HubPartElement->LinkEndChild(rcjElement);
			TiXmlText *rcjContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*3.0/50.0));
			rcjElement->LinkEndChild(rcjContent);


			//�ֲ۲���
			//HubGroove><!--  �ֲ۲���  -->//////////////////////////////////
			TiXmlElement *HubGrooveElement = new TiXmlElement("HubGroove");
			HubElement->LinkEndChild(HubGrooveElement);

			//r2><!--  r2���ֲۣ�  -->
			TiXmlElement *r2Element = new TiXmlElement("r2");
			HubGrooveElement->LinkEndChild(r2Element);
			TiXmlText *r2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*5.0/50.0));
			r2Element->LinkEndChild(r2Content);

			//r4><!--  r4���ֲۣ�  -->
			TiXmlElement *r4Element = new TiXmlElement("r4");
			HubGrooveElement->LinkEndChild(r4Element);
			TiXmlText *r4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*5.0/50.0));
			r4Element->LinkEndChild(r4Content);

			//d2><!--  d2���ֲۣ�  -->
			TiXmlElement *d2Element = new TiXmlElement("d2");
			HubGrooveElement->LinkEndChild(d2Element);
			TiXmlText *d2Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*1.0/50.0));
			d2Element->LinkEndChild(d2Content);

			//d4><!--  d4���ֲۣ�  -->
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

			//lb4><!--  lb4Z��н�  -->
			TiXmlElement *lb4Element = new TiXmlElement("lb4");
			HubGrooveElement->LinkEndChild(lb4Element);
			TiXmlText *lb4Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*20.6/50.0));
			lb4Element->LinkEndChild(lb4Content);

			//lb5><!--  lb5  -->
			TiXmlElement *lb5Element = new TiXmlElement("lb5");
			HubGrooveElement->LinkEndChild(lb5Element);
			TiXmlText *lb5Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*45.0/50.0));
			lb5Element->LinkEndChild(lb5Content);

			//hy><!--  Ҷ�������߳���ת�����hy  -->
			TiXmlElement *hyElement = new TiXmlElement("hy");
			HubGrooveElement->LinkEndChild(hyElement);
			TiXmlText *hyContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p21Bz*8.0/50.0));
			hyElement->LinkEndChild(hyContent);

		}
#pragma endregion
	}
	return true;
}


//Χ���ڵ����ݵ���
bool ClassTongliu::ExportShroudXMLData( TiXmlElement* pElem, int HubType ,bladeData tmp)
{
	CString str;
	TiXmlElement *ShroudElement = NULL;
	TiXmlElement *RotorElement = NULL;

	//��Ҷ
	if (HubType == 0)
	{
		//<!--  ��ҶΧ��������  -->
		ShroudElement = new TiXmlElement("StatorShroud");
		pElem->LinkEndChild(ShroudElement);

		//c<!--  ����ȳ����ߴ�  -->
		TiXmlElement *cElement = new TiXmlElement("c");
		ShroudElement->LinkEndChild(cElement);
		TiXmlText *cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.Sc_deep));
		cElement->LinkEndChild(cContent);

	}
	//��Ҷ
	if (HubType == 1)
	{
		//<!--  ��ҶΧ��������  -->
		RotorElement = new TiXmlElement("RotorShroud");
		pElem->LinkEndChild(RotorElement);

		//Hmin<!--  ���������  -->
		TiXmlElement *��Element = new TiXmlElement("��");
		RotorElement->LinkEndChild(��Element);
		TiXmlText *��Content = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.��));
		��Element->LinkEndChild(��Content);

		//Hmin<!--  ÿ����ҶΪ�����������������Ķ�Χ���ڲ۵Ŀ��  -->
		TiXmlElement *p32dElement = new TiXmlElement("p32d");
		RotorElement->LinkEndChild(p32dElement);
		TiXmlText *p32dContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.p32d));
		p32dElement->LinkEndChild(p32dContent);

		//c<!--  ����ȳ����ߴ�  -->
		TiXmlElement *cElement = new TiXmlElement("c");
		RotorElement->LinkEndChild(cElement);
		TiXmlText *cContent = new TiXmlText(Data->MyFunction->Basic->Double2Char(tmp.c_deep));
		cElement->LinkEndChild(cContent);
	}
	return true;
}




//Ԥ������
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
		//Setting�������
		if (Data->MyFunction->Basic->Char2CString(pElem1st->Value()).MakeLower() == L"setting")
		{
			for (TiXmlElement *pElem2nd = pElem1st->FirstChildElement(); pElem2nd != NULL; pElem2nd = pElem2nd->NextSiblingElement())
			{
				//ResPath��Դ·��--��
				if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()).MakeLower() == L"respath")
				{
					setting.ResPath = pElem2nd->FirstChild()->Value();
				}
				//ͨ��ͼԭ��
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
		//AirCylinder����
		if (Data->MyFunction->Basic->Char2CString(pElem1st->Value()).MakeLower() == L"aircylinder")
		{
			//�Ƿ��ǵ�һ�� BladePair �ڵ�
			bool isFirstBladePair_stator = true;

			//Ԥ������
			aircylinderData theAir;
			vector<bladeData>bladeTmpVec;
			
			//����p0����p30   ��һ��p0 = p0Jz_1 ��������
			//p0 =MAX(AI2+AF2+U2+K3-L3,AI2+AF2+X2+H3-I3)
			//p30=MAX(AJ2+C2+L2+T2-U2,AJ2+C2+I2+W2-X2)
			double ForwardSpace_U;		//��һ�� �����϶ AI
			double NegativeSpace_U;		//��һ�� �����϶ AJ
			double p30Dz_U;				//��һ�� p30Dz AF
			double p18bw_U;				//��һ�� p18bw U
			double p22bz_U;				//��һ�� p22bz X

			for (TiXmlElement *pElem2nd = pElem1st->FirstChildElement(); pElem2nd != NULL; pElem2nd = pElem2nd->NextSiblingElement())
			{
				//AirPassage��������
				if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()).MakeLower() == L"airpassage")
				{
					theAir.AirPassage = pElem2nd->FirstChild()->Value();
				}
				//LastThreeStagesĩ����ѡ�ͼ���λ����
				else if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()).MakeLower() == L"lastthreestages")
				{
					theAir.LastThreeStages = pElem2nd->FirstChild()->Value();
				}
				//AirwayCenter  ��������
				else if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()) == L"AirwayCenter")
				{
					//AirAxialDis  �����������������
					theAir.AirAxialDis = _wtof(Data->MyFunction->Basic->Char2CString(pElem2nd->FirstChildElement("AirAxialDis")->GetText()));
					//AirWeight  �������Ŀ��
					theAir.AirWeight = _wtof(Data->MyFunction->Basic->Char2CString(pElem2nd->FirstChildElement("AirWeight")->GetText()));
					//CantileverWidth  ���ۿ��
					theAir.CantileverWidth = _wtof(Data->MyFunction->Basic->Char2CString(pElem2nd->FirstChildElement("CantileverWidth")->GetText()));

				}
				//isHorizonBlade �Ƿ��к���ҶƬ 1��0��
				else if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()) == L"IsHorizonBlade")
				{
					theAir.isHorizonBlade = _wtof(Data->MyFunction->Basic->Char2CString(pElem2nd->FirstChild()->Value()));
				}
				//p0Jz_1 ԭ�㵽��1����Ҷ�����ߵľ��� 
				else if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()) == L"p0Jz_1")
				{
					if(theAir.isHorizonBlade == 1.0)
						theAir.p0Jz_1 = _wtof(Data->MyFunction->Basic->Char2CString(pElem2nd->FirstChild()->Value()));
				}
				//BladePairҶƬ��
				else if (Data->MyFunction->Basic->Char2CString(pElem2nd->Value()).MakeLower() == L"bladepair")
				{
					bladeData bladeTmp;
#pragma region
					for (TiXmlElement *pElem3rd = pElem2nd->FirstChildElement(); pElem3rd != NULL; pElem3rd = pElem3rd->NextSiblingElement())
					{
						//SuctionPort���λ�ü�����
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
							if(MaxAllowFlow_str == "��ѹ"  || MaxAllowFlow_str == "��ѹ")
								bladeTmp.�� = 50.0;
							else if(MaxAllowFlow_str == "��ѹ")
								bladeTmp.�� = 70.0;
							else
								bladeTmp.�� = 0.0;
							
						}
						//p31zf �������򣬶�����ֵ=1�������������ң�������������֣��ǿ�
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"p31zf")
						{
							bladeTmp.p31zf = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
							//˫����Ĭ��Ϊ0   �����ע���Գ�
							if(theAir.AirPassage=="˫����")
								bladeTmp.p31zf =0.0;
						}
						//Broken ����
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"Broken")
						{
							bladeTmp.BrokenTag = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChildElement("BrokenTag")->GetText()));
							bladeTmp.BrokenDis = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChildElement("BrokenDis")->GetText()));
						}
						//Margin ����
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"Margin")
						{
							bladeTmp.Margin = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
						}
						//Jmin><!--  ����������۵��ϼ�ҶƬ�ľ��� J��С���� -->
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"Jmin")
						{
							bladeTmp.Jmin = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
						}
						//Hmin><!--  ����������۵��¼�ҶƬ�ľ��� H��С���� -->
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"Hmin")
						{
							bladeTmp.Hmin = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
						}
						//P><!--  ���Ͳ�  -->
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"P")
						{
							bladeTmp.P = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
							bladeTmp.P = abs(bladeTmp.P);
							//�����϶ AI
							bladeTmp.ForwardSpace = Data->MyFunction->Basic->ceilToSignificance(bladeTmp.P+bladeTmp.Margin,0.5);
						}
						//Q><!--  ���Ͳ�  -->
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"Q")
						{
							bladeTmp.Q = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
							bladeTmp.Q = abs(bladeTmp.Q);
							//�����϶ AJ
							bladeTmp.NegativeSpace =Data->MyFunction->Basic->ceilToSignificance(bladeTmp.Q+bladeTmp.Margin,0.5);
						}
						//W><!--  ��̬�Ͳ� -->
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()) == L"W")
						{
							bladeTmp.W = _wtof(Data->MyFunction->Basic->Char2CString(pElem3rd->FirstChild()->Value()));
						}
						//N><!--  ÿ�������Ĳ��÷�����������еĹ�ϵNnֵ
						//������1����������Ҳ���Ϊ�����ã�������1�������������������ã����壺N1=0
						//������1����������Ҳ���Ϊ�����ã�������1�������������ҷ����ã����壺N1=1
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
						//BladeҶƬ����
						else if (Data->MyFunction->Basic->Char2CString(pElem3rd->Value()).MakeLower() == L"blade")
						{
							string str_type = pElem3rd->FirstChildElement("BladeType")->GetText();
							if(str_type == "stator")
							{
								//Profile��ҶƬ����
#pragma region	
								//����ǵ�һ��ҶƬ�����к���ҶƬ
								if(isFirstBladePair_stator && theAir.isHorizonBlade == 1.0)
								{
									ForwardSpace_U = bladeTmp.ForwardSpace;		//��һ�� �����϶ AI
									NegativeSpace_U = bladeTmp.NegativeSpace;	//��һ�� �����϶ AJ
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
									
									//����ǵ�һ��ҶƬ����ͬʱû�к���ҶƬ
									if (isFirstBladePair_stator)
									{
										// �����һ�� BladePair �ڵ�����  ��һ��p0����Ҫ����
										isFirstBladePair_stator = false;
										//��û�к��þ�Ҷʱ����һ��P0=0.5*�������+���ۿ��+0.5*��һ����Ҷ�м����p8Bz-p9bz��
										theAir.p0Jz_1 = 0.5 * theAir.AirWeight + theAir.CantileverWidth + 0.5*(bladeTmp.p8Bz - bladeTmp.p9bz) ;
										// + �����������
										bladeTmp.p0Jz = theAir.p0Jz_1 + theAir.AirAxialDis + setting.OriginalPoint.x;
										
										//�����  ��һ��p0Ҫ��
										ForwardSpace_U = bladeTmp.ForwardSpace;		//��һ�� �����϶ AI
										NegativeSpace_U = bladeTmp.NegativeSpace;	//��һ�� �����϶ AJ
									}
									else
									{
										if(bladeTmp.BrokenTag==0.0)
										{
											// ����ǵ�һ�� BladePair �ڵ�����
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
										
										//�����  ��һ��p0Ҫ��
										ForwardSpace_U = bladeTmp.ForwardSpace;		//��һ�� �����϶ AI
										NegativeSpace_U = bladeTmp.NegativeSpace;	//��һ�� �����϶ AJ
									}
								}

								//StatorHub��Ҷ������
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
										
										//if(bladeTmp.p31zf == 1.0)//��������������Ҿ�Ҷ������
										bladeTmp.S_deep_point = AcGePoint3d(bladeTmp.p0Jz -bladeTmp.p8Bz +bladeTmp.p9bz ,bladeTmp.p26Dpp +bladeTmp.p27Lp/2.0 +bladeTmp.p10hz -bladeTmp.Shy,0);
										/*else
											bladeTmp.S_deep_point = AcGePoint3d(bladeTmp.p0Jz +bladeTmp.p9bz ,bladeTmp.p26Dpp +bladeTmp.p27Lp/2.0 +bladeTmp.p10hz -bladeTmp.Shy,0);*/
									}
								}

								//StatorShroud��ҶΧ��������
								TiXmlElement *statorshroudElement =pElem3rd->FirstChildElement("StatorShroud");
								if (statorshroudElement)
								{
									bladeTmp.Sc_deep = _wtof(Data->MyFunction->Basic->Char2CString(statorshroudElement->FirstChildElement("c")->GetText()));
								}
							}
#pragma endregion
							else if(str_type == "rotor")
							{
								//Profile��ҶƬ����
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
									
									//����ǵ�һ��ҶƬ�����к���ҶƬ
									if(isFirstBladePair_stator && theAir.isHorizonBlade == 1.0)
									{
										// �����һ�� BladePair �ڵ�����  ��һ��p30����Ҫ����
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
										//����
										bladeTmp.p30Dz = max(tmp1,tmp2);
									}
									

									//�����  ��һ��p0Ҫ��
									p30Dz_U = bladeTmp.p30Dz;				//��һ�� p30Dz AF
									p18bw_U = bladeTmp.p18bw;				//��һ�� p18bw U
									p22bz_U = bladeTmp.p22bz;				//��һ�� p22bz X
								
								}

								//RotorHub��Ҷ������
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

										//if(bladeTmp.p31zf == 1.0)//���������������
										bladeTmp.deep_point = AcGePoint3d(bladeTmp.p30Dz -bladeTmp.p21Bz +bladeTmp.p22bz,bladeTmp.p28Dpd-bladeTmp.p29Ld/2.0-bladeTmp.p23hz+bladeTmp.hy,0);
										bladeTmp.rator_deeppoint = AcGePoint3d(bladeTmp.deep_point.x,bladeTmp.deep_point.y,bladeTmp.deep_point.z);
										/*else
											bladeTmp.deep_point = AcGePoint3d(bladeTmp.p30Dz +bladeTmp.p22bz,bladeTmp.p28Dpd-bladeTmp.p29Ld/2.0-bladeTmp.p23hz+bladeTmp.hy,0);*/

									}
								}

								//RotorShroud��ҶΧ��������
								TiXmlElement *rotorshroudElement =pElem3rd->FirstChildElement("RotorShroud");
								if (rotorshroudElement)
								{
									bladeTmp.�� = _wtof(Data->MyFunction->Basic->Char2CString(rotorshroudElement->FirstChildElement("��")->GetText()));
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
			//ѭ��bladeTmpVec�����㲿�ֲ���
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
					//�ж�(L5+M5-N5>=J5+O5-P32
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
					//�ж�L4+M4-N4�Ƿ�С��J4+O4-P4
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

				//���һ��ҶƬ
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
				//����ǵ�һ��ҶƬ 
				if(i==0)
				{
					theTmp.F = theTmp.X+theTmp.Margin;
				}
				//���һ��ҶƬ
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
				//p17Bw-p32d��W+1.27*3+max[(X+1.1),3]+max[(Y+1.1+W),3],t=3
				if (theTmp.p17Bw - theTmp.p32d >= temp1) 
				{
					theTmp.t = 3;
					theTmp.t_type = 1;
				}
				//W+1.27*3+max[(X+1.1),3]+max[(Y+1.1+W),3]>p17Bw-p32d��W+1.27*2+max[(Y+1.1+W),3],t=2
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
					//���һ������ݵ�λ��  ��t������Ƭ����λ�ߴ�b=roundup[(A+W+1.27),1]+4.3(t-1)
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

				//t_type_s��Ҷ // p5Bw��2.9*2+1.27*3+max[(X+1.1),3]+max[(Y+1.1+W),3]��H��Hmin��J��Jmin��������Ϊ�������ĵ�һ��ʱ����J��JminҪ��
#pragma region
				if(theTmp.p5Bw >= 2.9*2+1.27*3+max(theTmp.X + theTmp.Margin, 3)+max(theTmp.Y + theTmp.Margin + theTmp.W, 3) && theTmp.H >= theTmp.Hmin)
				{
					//����ǵ�һ��ҶƬ 
					if(i==0)
					{
						//һ��һ��һ��
						theTmp.t_type_s = 1;
					}
					else if(theTmp.J >= theTmp.Jmin)
					{
						//һ��һ��һ��
						theTmp.t_type_s = 1;
					}
					else
					{
						//һ��һ��
						theTmp.t_type_s = 2;
					}
					
				}
				else
				{
					//һ��һ��
					theTmp.t_type_s = 2;
				}
#pragma endregion


				//�� H��Hmin->0 -->��=Hmin-H
#pragma region
				if(theTmp.H >= theTmp.Hmin)
					theTmp.�� = 0;
				else
					theTmp.�� = theTmp.Hmin-theTmp.H;
#pragma endregion

				//�� J��Jmin->0 -->��=Jmin-J
#pragma region
				if(theTmp.J >= theTmp.Jmin)
					theTmp.�� = 0;
				else
					theTmp.�� = theTmp.Jmin-theTmp.J;
#pragma endregion

				//p0Jz ��
#pragma region
				if(i!=0)
					theTmp.p0Jz = theTmp.p0Jz + theTmp.��;
#pragma endregion

				//p30Dz ��
#pragma region
				theTmp.p30Dz = theTmp.p30Dz + theTmp.��;
#pragma endregion
				bladeTmpVec[i] = theTmp;
			}

			theAir.bladePair = bladeTmpVec;
			aircylinderDataVec.push_back(theAir);
		}
	}


	return true;

}


//Ԥ�����ݴ���������
bool ClassTongliu::LoadXMLDataDispose()
{
	//ѭ������
	for(int i = 0;i<aircylinderDataVec.size();i++)
	{
		//����
		double increment = 0.0;
		//��ȡҶƬ����
		vector<bladeData> bladePair = aircylinderDataVec[i].bladePair;
		for(int ii=0;ii<bladePair.size();ii++)
		{
			//���ж��Ƿ������� //��һ��ҶƬ���������� ���Բ�������
			if(increment != 0.0)
			{
				//�����������
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

			//�ж�ҶƬ�Ժ��Ƿ��г��
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

				//a. �������3����->ʹ�ø���Yֵ����Ҫ�޸�XML��ʽ������Yֵ��
				//b. �����1��2�ж�
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
						mid_dis_x = bladePair[ii].V * bladePair[ii].G *pow(10.0, 9.0)*1000/3600 / (bladePair[ii].��*1000*PI *theP.y);
					}
					mid_re_dis = max(mid_dis1,mid_dis_x);

				}

				bladePair[ii].InletWeight = right_re_dis;
				bladePair[ii].OutletWeight = left_re_dis;
				bladePair[ii].MidWeight = mid_re_dis;
				double tmp1 = left_re_dis + right_re_dis + mid_re_dis - (bladePair[ii+1].p0Jz - bladePair[ii].p0Jz+increment);
				increment =  increment + tmp1;
				//����
				if(increment<0)
					increment = 0.0;
				bladePair[ii].rator_deeppoint.x = bladePair[ii].rator_deeppoint.x+tmp1;
			}
		}

		aircylinderDataVec[i].bladePair = bladePair;
	}
	return true;
}



//�ж��м�������Y
bool ClassTongliu::isMidWeightY(vector<bladeData> bladePair,int theNum)
{
	bool hasOneBefore = false;
	bool hasOneAfter = false;

	// ����ȷ��i�ķ�Χ�ں���ķ�Χ��
	if(theNum < 0 || theNum >= bladePair.size()) {
		return false;
	}

	// �жϵ�i֮ǰ��Ԫ���Ƿ��г��
	for (int j = 0; j < theNum; ++j)
	{
		if (bladePair[j].IsSuctionPort == 1.0) 
		{
			//�ж������������ڼ�������ڼ����Ƿ��С�ڵ���2��
			if(theNum - j <=2)
			{
				hasOneAfter = true;
				break;
			}
			else
				return false;
		}
	}
	// �жϵ�i֮���Ԫ���Ƿ���1
	for (int j = theNum + 1; j < bladePair.size(); ++j) 
	{
		if(bladePair[j].IsSuctionPort == 1.0) 
		{
			//�ж������������ڼ�������ڼ����Ƿ��С�ڵ���2��
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

//************ͼֽ����

//����ͨ��ͼ
int ClassTongliu::createTL()
{	
	//ѭ������
	for(int ii = 0;ii<aircylinderDataVec.size();ii++)
	{
		//��ȡĩ��������
		string last_three_stages = aircylinderDataVec[ii].LastThreeStages;
		//��ȡҶƬ����
		vector<bladeData> bladePair = aircylinderDataVec[ii].bladePair;

		//�����������������
		double AirAxialDis = aircylinderDataVec[ii].AirAxialDis;
		//�������Ŀ��
		double AirWeight = aircylinderDataVec[ii].AirWeight;
		//���ۿ��
		double CantileverWidth = aircylinderDataVec[ii].CantileverWidth;
		
		//��עisdelete ����ɾ�� �� ˫����ɾ
		//�ж��������� 
		int isdelete = 0;
		if(aircylinderDataVec[ii].AirPassage=="����" || aircylinderDataVec[ii].AirPassage=="����")
			isdelete = 1;

		//ѭ��ҶƬ����
#pragma region
		for(int i=0;i<bladePair.size();i++)
		{
			//����
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
			
			//����ǵ�һ��ҶƬ ���Ǻ���ҶƬ�򻭺���ҶƬ
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
				//�Ȼ�����
				l_p = AcGePoint3d(setting.OriginalPoint.x +AirAxialDis - AirWeight/2.0 ,setting.OriginalPoint.y + tmp.p28Dpd + tmp.p29Ld +50.0,0);
				r_p = AcGePoint3d(setting.OriginalPoint.x +AirAxialDis + AirWeight/2.0 ,setting.OriginalPoint.y + tmp.p28Dpd + tmp.p29Ld +50.0,0);
			}
			else
			{
				//��Ҷ
				//�������� ҶƬ�ı������� 
				vertex1   = AcGePoint3d(tmp.p0Jz + tmp.p4bd -tmp.p2BD, tmp.p26Dpp + tmp.p19, 0);
				vertex2   = AcGePoint3d(tmp.p0Jz + tmp.p3bg -tmp.p1BG, tmp.p26Dpp - tmp.p19, 0);
				vertex3   = AcGePoint3d(tmp.p0Jz + tmp.p3bg , tmp.p26Dpp - tmp.p19, 0);
				vertex4   = AcGePoint3d(tmp.p0Jz + tmp.p4bd , tmp.p26Dpp + tmp.p19, 0);
				//��Χ�����ĵ� ˳����ʱ��
				AcGePoint3d vertex_01 = AcGePoint3d(tmp.p0Jz + tmp.p9bz -tmp.p8Bz, vertex1.y+tmp.p10hz, 0);
				AcGePoint3d vertex_02 = AcGePoint3d(tmp.p0Jz + tmp.p6bw -tmp.p5Bw, vertex3.y-tmp.p7hw, 0);
				AcGePoint3d vertex_03 = AcGePoint3d(tmp.p0Jz + tmp.p6bw , vertex3.y-tmp.p7hw , 0);
				AcGePoint3d vertex_04 = AcGePoint3d(tmp.p0Jz + tmp.p9bz , vertex1.y+tmp.p10hz, 0);
				s_vertex_out_vec.push_back(vertex_01);
				s_vertex_out_vec.push_back(vertex_02);
				s_vertex_out_vec.push_back(vertex_03);
				s_vertex_out_vec.push_back(vertex_04);
				//�Ƕ�
				angle_up = tmp.p11ad * PI /180.0;
				angle_down = tmp.p12ag * PI /180.0;

				//��ҶƬ����
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

				//ҶƬ����
				ids_up=AcGePoint3d(0,0,0);
				ids_down=AcGePoint3d(0,0,0) ;
				Data->MyFunction->Arx->GetObjectIntersect(tag_ray_Id,ray_upl_ID,&ids_up);
				Data->MyFunction->Arx->GetObjectIntersect(tag_ray_Id,ray_downl_ID,&ids_down);

				//Ҷ������
				Data->MyFunction->Arx->GetObjectIntersect(tag_upl_ray_Id,ray_upl_ID,&ids_up1);
				Data->MyFunction->Arx->GetObjectIntersect(tag_upr_ray_Id,ray_upr_ID,&ids_up2);
				s_vertex_out_vec.push_back(ids_up1);
				s_vertex_out_vec.push_back(ids_up2);
				//Χ������
				Data->MyFunction->Arx->GetObjectIntersect(tag_downl_ray_Id,ray_downl_ID,&ids_down1);
				Data->MyFunction->Arx->GetObjectIntersect(tag_downr_ray_Id,ray_downr_ID,&ids_down2);

				//ɾ��������
				Data->MyFunction->Arx->DeleteObject(tag_ray_Id);
				Data->MyFunction->Arx->DeleteObject(tag_upl_ray_Id);
				Data->MyFunction->Arx->DeleteObject(tag_upr_ray_Id);
				Data->MyFunction->Arx->DeleteObject(tag_downl_ray_Id);
				Data->MyFunction->Arx->DeleteObject(tag_downr_ray_Id);
				Data->MyFunction->Arx->DeleteObject(ray_upl_ID);
				Data->MyFunction->Arx->DeleteObject(ray_upr_ID);
				Data->MyFunction->Arx->DeleteObject(ray_downl_ID);
				Data->MyFunction->Arx->DeleteObject(ray_downr_ID);


				//����ҶƬ ��ʱ��
				Data->MyFunction->Arx->SetNowLayerName(L"2ϸʵ��");
				Data->MyFunction->Arx->CreateLine(vertex4,vertex1,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(vertex2,vertex3,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->SetNowLayerName(L"1��ʵ��");

				Data->MyFunction->Arx->CreateLine(vertex1,ids_down,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(vertex3,vertex4,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);


				//Χ��
				AcDbObjectId yegen_down_line_id,weidai_up_line_id;
				Data->MyFunction->Arx->CreateLine(ids_down1,vertex_02,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(vertex_02,vertex_03,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(vertex_03,ids_down2,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(ids_down2,ids_down1,&weidai_up_line_id);
				AircylinderObjects.append(weidai_up_line_id);

				//���޺���ҶƬ�����ǵ�һ��ҶƬ�򹴻�����
				if(i == 0 && aircylinderDataVec[ii].isHorizonBlade == 0.0)
				{
					l_p = AcGePoint3d(setting.OriginalPoint.x +AirAxialDis - AirWeight/2.0 ,setting.OriginalPoint.y + tmp.p26Dpp + tmp.p27Lp+50.0 ,0);
					r_p = AcGePoint3d(setting.OriginalPoint.x +AirAxialDis + AirWeight/2.0 ,setting.OriginalPoint.y + tmp.p26Dpp + tmp.p27Lp+50.0 ,0);
				
					//�ϲ�������
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
					//��Ҷ�ò��õļ�϶�����������ȡxml�෴
					if(tmp.N == 0)
						u_num = tmp.NegativeSpace ;
					else
						u_num = tmp.ForwardSpace ;
					
					//�ж��������� 
					if(aircylinderDataVec[ii].AirPassage=="����"||aircylinderDataVec[ii].AirPassage=="����")
					{
						//����������������жϾ���
						if(tmp.HubType==1.0||tmp.HubType==4.0)
						{
							//�²�������
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
					else if(aircylinderDataVec[ii].AirPassage=="˫����")
					{
						//˫�����²���������ʼ�����������ϣ����Ҫ�������ݾ���
						if(tmp.HubType==1.0||tmp.HubType==4.0)
						{
							//�²�������
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
							//�²�������
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
					
					//����������
					Data->MyFunction->Arx->SetNowLayerName(L"ϸ�㻮��");
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_p.x - AirWeight/2.0 ,ids_up1.y +80,0) ,AcGePoint3d(r_p.x - AirWeight/2.0 ,ids_down1.y -80,0) ,&tmp2);
					AircylinderObjects.append(tmp3);
					Data->MyFunction->Arx->SetNowLayerName(L"1��ʵ��");
				}

				//����Ҷ��
				createHub("��Ҷ", 1, tmp, s_vertex_out_vec, AircylinderObjects);

				//����Ҷ����
				if(i!=bladePair.size()-1)
				{
					after_data = bladePair[i+1];
					GroovePoint = after_data.S_deep_point;
				}
				/*ҶƬ���ͣ��ṹ�壬�м������׼�㣬�м����л�׼��,��һҶ���ۻ�׼��ȵ㣨ע�����һ��û����ȵ㣩*/
				createBladeHubGroove("��Ҷ", 1, tmp,after_data,vertex_01,vertex_04,AircylinderObjects,GroovePoint);

				//������Ų� �ж���������� ��Ҷ ��Ϊ��Ҫ��һ����Ҷ��deeppoint
				AcDbObjectId ObjectId;
				if(tmp.t_type_s == 1)//һ��һ��һ��
				{
					Data->MyFunction->Arx->InsertBlock("S_L_short",AcGePoint3d(tmp.deep_point.x-tmp.f-0.65,vertex_02.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("S_L_long",AcGePoint3d(tmp.deep_point.x-tmp.e-0.65,vertex_02.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("S_L_short",AcGePoint3d(tmp.deep_point.x-tmp.d-0.65,vertex_02.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
				}
				else if(tmp.t_type_s == 2)// һ��һ��
				{
					Data->MyFunction->Arx->InsertBlock("S_L_long",AcGePoint3d(tmp.deep_point.x-tmp.e-0.65,vertex_02.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("S_L_short",AcGePoint3d(tmp.deep_point.x-tmp.f-0.65,vertex_02.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
				}

				//��Ҷ������ ��-���� ��-����
				Data->MyFunction->Arx->SetNowLayerName(L"ϸ�㻮��");
				up_point1 = AcGePoint3d(tmp.p0Jz,tmp.p26Dpp-tmp.p19-tmp.p7hw,0);
				up_point2 = AcGePoint3d(tmp.p0Jz,tmp.p26Dpp+tmp.p19+tmp.p10hz+tmp.p21,0);
				left_point = AcGePoint3d(tmp.p0Jz-tmp.p17,tmp.p26Dpp,0);
				right_point = AcGePoint3d(tmp.p0Jz+tmp.p18,tmp.p26Dpp,0);
				AcDbObjectId up_line_id;
				Data->MyFunction->Arx->CreateLine(up_point1,AcGePoint3d(up_point2.x,vertex_01.y+tmp.SGrooveNeckH+tmp.SGrooveBottomH+10.0,0),&up_line_id);
				AircylinderObjects.append(up_line_id);
				Data->MyFunction->Arx->CreateLine(left_point,right_point,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);


				//��ע////////������  ������
				//��Ҷ���߶�
				double p96 = 0.0;
				if(tmp.p31zf ==1)
					p96 =3.4;
				else
					p96 = 7.8;

				point1 = AcGePoint3d(tmp.p0Jz,vertex1.y,0);
				point2 = AcGePoint3d(tmp.p0Jz,vertex_01.y,0);
				point3 = AcGePoint3d(point1.x - p96,vertex1.y+tmp.p10hz/2,0);
				Data->MyFunction->Arx->SetNowLayerName(L"5�ߴ���");
				Data->MyFunction->Arx->CreateLinearDim(point1,point2,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p10hz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

				//��Ҷ�ϰ���
				Data->MyFunction->Arx->CreateLine(ids_up1,ids_up2,&yegen_down_line_id);
				Data->MyFunction->Arx->GetObjectIntersect(yegen_down_line_id,up_line_id,&point1);
				Data->MyFunction->Arx->DeleteObject(yegen_down_line_id);
				point3 = AcGePoint3d(point1.x +tmp.p4bd/2,point1.y-2.6,0);
				Data->MyFunction->Arx->CreateLinearDim(point1,vertex4,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p4bd),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//��Ҷ�Ͽ��
				point3 = AcGePoint3d(vertex4.x-tmp.p2BD/2,point3.y-5.0,0);
				Data->MyFunction->Arx->CreateLinearDim(vertex1,vertex4,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p2BD),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//��Ҷ��
				point3 = AcGePoint3d(ids_down1.x - p96,left_point.y+tmp.p27Lp/4,0);
				Data->MyFunction->Arx->CreateLinearDim(vertex1,vertex2,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p27Lp),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//��Ҷ�¿��
				point3 = AcGePoint3d(vertex3.x-tmp.p1BG/2,vertex3.y+8.4,0);
				Data->MyFunction->Arx->CreateLinearDim(vertex2,vertex3,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p1BG),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//��Ҷ�°���
				point1 = AcGePoint3d(tmp.p0Jz,vertex3.y,0);
				point3 = AcGePoint3d(tmp.p0Jz +tmp.p3bg/2,vertex3.y+3.4,0);
				Data->MyFunction->Arx->CreateLinearDim(point1,vertex3,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p4bd),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

				//��Ҷ������
				point1 = AcGePoint3d(tmp.p0Jz,ids_up2.y,0);
				point3 = AcGePoint3d(tmp.p0Jz +tmp.p9bz/2,vertex4.y+3.4,0);
				Data->MyFunction->Arx->CreateLinearDim(point1,ids_up2,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p9bz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//��Ҷ�����
				point1 = AcGePoint3d(ids_up1.x ,ids_up2.y,0);
				point3 = AcGePoint3d(tmp.p0Jz +tmp.p9bz/2,ids_up2.y+8.4,0);
				Data->MyFunction->Arx->CreateLinearDim(point1,ids_up2,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p8Bz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//��ҶΧ������
				Data->MyFunction->Arx->GetObjectIntersect(weidai_up_line_id,up_line_id,&point1);
				zhangchai_point = point1;
				point3 = AcGePoint3d(tmp.p0Jz +tmp.p6bw/2,point1.y-2.6,0);
				Data->MyFunction->Arx->CreateLinearDim(point1,ids_down2,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p6bw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//��ҶΧ�����
				double p100 = 0.0;
				if(tmp.p27Lp >30.4)
					p100 =13.4;
				else
					p100 = -32-tmp.p7hw;
				point3 = AcGePoint3d(ids_down2.x-tmp.p5Bw/2,ids_down2.y+p100,0);
				Data->MyFunction->Arx->CreateLinearDim(ids_down1,ids_down2,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p5Bw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
				//��ҶΧ���߶�
				point3 = AcGePoint3d(ids_down1.x-p96,ids_down2.y-tmp.p7hw/2,0);
				Data->MyFunction->Arx->CreateLinearDim(vertex_02,vertex3,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p7hw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);


			}
			
			//��Ҷ
			//�������� ҶƬ�ı������� ˳����ʱ��
			AcGePoint3d vertex5 = AcGePoint3d(tmp.p30Dz + tmp.p16bd -tmp.p14BD, tmp.p28Dpd + tmp.p32, 0);
			AcGePoint3d vertex6 = AcGePoint3d(tmp.p30Dz + tmp.p15bg -tmp.p13BG, tmp.p28Dpd - tmp.p32, 0);
			AcGePoint3d vertex7 = AcGePoint3d(tmp.p30Dz + tmp.p15bg ,tmp.p28Dpd - tmp.p32, 0);
			AcGePoint3d vertex8 = AcGePoint3d(tmp.p30Dz + tmp.p16bd, tmp.p28Dpd + tmp.p32, 0);
			//��Χ�����ĵ� ˳����ʱ��
			AcGePoint3d vertex_05 = AcGePoint3d(tmp.p30Dz + tmp.p18bw -tmp.p17Bw, vertex5.y+tmp.p19hw, 0);
			AcGePoint3d vertex_06 = AcGePoint3d(tmp.p30Dz + tmp.p22bz -tmp.p21Bz, vertex7.y-tmp.p23hz, 0);
			AcGePoint3d vertex_07 = AcGePoint3d(tmp.p30Dz + tmp.p22bz , vertex7.y-tmp.p23hz , 0);
			AcGePoint3d vertex_08 = AcGePoint3d(tmp.p30Dz + tmp.p18bw , vertex5.y+tmp.p19hw, 0);
			vertex_out_vec.push_back(vertex_05);
			vertex_out_vec.push_back(vertex_06);
			vertex_out_vec.push_back(vertex_07);
			vertex_out_vec.push_back(vertex_08);
			
			//��ҶƬ����
			AcDbObjectId tag_ray_Id1, tag_upl_ray_Id1,tag_upr_ray_Id1,tag_downr_ray_Id1,tag_downl_ray_Id1,ray_upl_ID1,ray_upr_ID1,ray_downl_ID1,ray_downr_ID1;
			Data->MyFunction->Arx->CreateRay(vertex_05,AcGeVector3d(0,-1,0), &tag_upl_ray_Id1);
			Data->MyFunction->Arx->CreateRay(vertex_08,AcGeVector3d(0,-1,0), &tag_upr_ray_Id1);
			Data->MyFunction->Arx->CreateRay(vertex_06,AcGeVector3d(0,1,0) , &tag_downl_ray_Id1);
			Data->MyFunction->Arx->CreateRay(vertex_07,AcGeVector3d(0,1,0) , &tag_downr_ray_Id1);

			AcGePoint3d ids_up01,ids_down01;
			AcGeVector3d tag_vec1 =  AcGeVector3d(vertex6.x - vertex5.x, vertex6.y - vertex5.y, 0);//vertex6 - vertex5
			Data->MyFunction->Arx->CreateRay(vertex5,tag_vec1, &tag_ray_Id1);
			
			//�Ƕ�
			angle_up = tmp.p24ad * PI /180.0;
			angle_down = tmp.p25ag * PI /180.0;
			Data->MyFunction->Arx->CreateLine(vertex8,angle_up,  1000.0,0, &ray_upl_ID1);
			Data->MyFunction->Arx->CreateLine(vertex8,angle_up,  1000.0,1, &ray_upr_ID1);
			Data->MyFunction->Arx->CreateLine(vertex7,angle_down,1000.0,0, &ray_downl_ID1);
			Data->MyFunction->Arx->CreateLine(vertex7,angle_down,1000.0,1, &ray_downr_ID1);

			//ҶƬ����
			ids_up01=AcGePoint3d(0,0,0);
			ids_down01=AcGePoint3d(0,0,0) ;
			Data->MyFunction->Arx->GetObjectIntersect(tag_ray_Id1,ray_upl_ID1,&ids_up01);
			Data->MyFunction->Arx->GetObjectIntersect(tag_ray_Id1,ray_downl_ID1,&ids_down01);
			
			//Χ������
			AcGePoint3d ids_up11=AcGePoint3d(0,0,0);
			AcGePoint3d ids_up21 =AcGePoint3d(0,0,0);
			Data->MyFunction->Arx->GetObjectIntersect(tag_upl_ray_Id1,ray_upl_ID1,&ids_up11);
			Data->MyFunction->Arx->GetObjectIntersect(tag_upr_ray_Id1,ray_upr_ID1,&ids_up21);
			//Ҷ������
			AcGePoint3d ids_down11=AcGePoint3d(0,0,0);
			AcGePoint3d ids_down21=AcGePoint3d(0,0,0); 
			Data->MyFunction->Arx->GetObjectIntersect(tag_downl_ray_Id1,ray_downl_ID1,&ids_down11);
			Data->MyFunction->Arx->GetObjectIntersect(tag_downr_ray_Id1,ray_downr_ID1,&ids_down21);
			vertex_out_vec.push_back(ids_down11);
			vertex_out_vec.push_back(ids_down21);


			//ɾ��������
			Data->MyFunction->Arx->DeleteObject(tag_ray_Id1);
			Data->MyFunction->Arx->DeleteObject(tag_upl_ray_Id1);
			Data->MyFunction->Arx->DeleteObject(tag_upr_ray_Id1);
			Data->MyFunction->Arx->DeleteObject(tag_downl_ray_Id1);
			Data->MyFunction->Arx->DeleteObject(tag_downr_ray_Id1);
			Data->MyFunction->Arx->DeleteObject(ray_upl_ID1);
			Data->MyFunction->Arx->DeleteObject(ray_upr_ID1);
			Data->MyFunction->Arx->DeleteObject(ray_downl_ID1);
			Data->MyFunction->Arx->DeleteObject(ray_downr_ID1);

			//����ҶƬ ��ʱ��
			Data->MyFunction->Arx->SetNowLayerName(L"2ϸʵ��");
			Data->MyFunction->Arx->CreateLine(vertex8,vertex5 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(vertex6,vertex7 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			Data->MyFunction->Arx->SetNowLayerName(L"1��ʵ��");
			Data->MyFunction->Arx->CreateLine(vertex5,ids_down01 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(vertex7,vertex8 ,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			
			//Χ��
			AcDbObjectId yegen_up_line_id,weidai_down_line_id;
			//�޸���������ϵ��� ������
			AcGePoint3d outlineP1,outlineP2,outlineP3;
			double angle_�� = tmp.�� * PI /180.0;
			outlineP1 = AcGePoint3d(vertex_05.x +tmp.p32d,vertex_05.y,0);
			outlineP2 = AcGePoint3d(outlineP1.x , outlineP1.y - tmp.c_deep,0);
			outlineP3 = AcGePoint3d(vertex_05.x, outlineP2.y - tmp.p32d*tan(angle_��),0);
		
			
			//�ж����������
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
				//��Z
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
			

			//����Ҷ���ۻ�ȡ��
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
					//�ϸ�8.0�̶�ֵ
					r_point = AcGePoint3d(tmp.p30Dz +tmp.p21Bz - tmp.p22bz,tmp.p28Dpd - tmp.p29Ld/2.0 - tmp.p23hz+8.0,0);
				}

				//���һ��ҶƬ����ĩ����
				createLastThreeStages(aircylinderDataVec[ii].LastThreeStages,r_point,tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete,AircylinderObjects);
			}

			//���Ǻ���ҶƬ�����ǵ�һ��ҶƬ�����Ӻ��þ�ҶƬ
			if(i == 0 && aircylinderDataVec[ii].isHorizonBlade == 1.0)
			{
				//�²�������
				AcDbObjectId tmp1,tmp2,tmp3;
				AcGePoint3d StartPoint,EndPoint;
				//�ж��������� 
				if(aircylinderDataVec[ii].AirPassage=="����"||aircylinderDataVec[ii].AirPassage=="����")
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
				else if(aircylinderDataVec[ii].AirPassage=="˫����")
				{
					////�²�������
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

				//�ϲ�������
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

				//����������
				Data->MyFunction->Arx->SetNowLayerName(L"ϸ�㻮��");
				Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_p.x - AirWeight/2.0 ,ids_up11.y +80,0) ,AcGePoint3d(r_p.x - AirWeight/2.0 ,ids_down11.y -80,0) ,&tmp2);
				AircylinderObjects.append(tmp3);
				Data->MyFunction->Arx->SetNowLayerName(L"1��ʵ��");
			}

			//����Ҷ��
			createHub("��Ҷ", tmp.HubType, tmp, vertex_out_vec, AircylinderObjects);
		
			//����Ҷ����
			createBladeHubGroove("��Ҷ", tmp.HubType, tmp,after_data,vertex_06,vertex_07,AircylinderObjects,GroovePoint);

			//��Ҷ������ ��-���� ��-����
			Data->MyFunction->Arx->SetNowLayerName(L"ϸ�㻮��");
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
			
			//��ע////////������  ������
			//��Ҷ���߶�
			double p99 = 0.0;
			if(tmp.p31zf ==1)
				p99 =5.6;
			else
				p99 = tmp.p21Bz-tmp.p22bz-7.4;

			point1 = AcGePoint3d(tmp.p30Dz,vertex_06.y,0);
			point2 = AcGePoint3d(tmp.p30Dz,vertex6.y,0);
			point3 = AcGePoint3d(ids_down11.x + p99,vertex6.y-tmp.p23hz/2,0);
			Data->MyFunction->Arx->SetNowLayerName(L"5�ߴ���");
			Data->MyFunction->Arx->CreateLinearDim(point1,point2,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p23hz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			//��Ҷ�°���
			point1 = AcGePoint3d(tmp.p30Dz,vertex6.y,0);
			point3 = AcGePoint3d(tmp.p30Dz +tmp.p15bg/2,vertex6.y+3.4,0);
			Data->MyFunction->Arx->CreateLinearDim(point1,vertex7,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p15bg),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
			//��Ҷ�¿��
			point3 = AcGePoint3d(vertex7.x-tmp.p13BG/2,point3.y+5.0,0);
			Data->MyFunction->Arx->CreateLinearDim(vertex6,vertex7,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p13BG),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
			//��Ҷ��
			double p98 = 0.0;
			if(tmp.p31zf ==1)
				p98 =1.4;
			else
				p98 = 4.3;
			point3 = AcGePoint3d(ids_down11.x - p98,left_point.y+tmp.p29Ld/4,0);
			Data->MyFunction->Arx->CreateLinearDim(vertex5,vertex6,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p29Ld),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			//��Ҷ�ϰ���
			Data->MyFunction->Arx->GetObjectIntersect(weidai_down_line_id,up_line_id_dong,&point1);
			point3 = AcGePoint3d(tmp.p30Dz +tmp.p16bd/2,point1.y-2.6,0);
			Data->MyFunction->Arx->CreateLinearDim(point1,vertex8,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p16bd),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
			//��Ҷ�Ͽ��
			point3 = AcGePoint3d(vertex8.x-tmp.p14BD/2,point1.y-7.6,0);
			Data->MyFunction->Arx->CreateLinearDim(vertex5,vertex8,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p14BD),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			//��ҶҶ������
			Data->MyFunction->Arx->GetObjectIntersect(yegen_up_line_id,up_line_id_dong,&point1);
			point3 = AcGePoint3d(tmp.p30Dz +tmp.p22bz/2,point1.y-2.6,0);
			Data->MyFunction->Arx->CreateLinearDim(point1,ids_down21,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p22bz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
			//��ҶҶ�����
			point3 = AcGePoint3d(point3.x,point1.y-7.6,0);
			Data->MyFunction->Arx->CreateLinearDim(ids_down11,ids_down21,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p21Bz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			//��ҶΧ������
			point1 = AcGePoint3d(tmp.p30Dz,ids_up21.y,0);
			point3 = AcGePoint3d(tmp.p30Dz +tmp.p18bw/2,vertex5.y+3.4,0);
			Data->MyFunction->Arx->CreateLinearDim(point1,ids_up21,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p18bw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);
			//��ҶΧ�����
			double p101 = 0.0;
			if(tmp.p29Ld >31.9)
				p101 =tmp.p16bd*tan(angle_up)+12.6;
			else
				p101 = -38.8-tmp.p19hw;

			point3 = AcGePoint3d(ids_up21.x - tmp.p17Bw/2,vertex5.y-p101,0);
			Data->MyFunction->Arx->CreateLinearDim(ids_up11,ids_up21,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p17Bw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			//��ҶΧ���߶�
			point3 = AcGePoint3d(ids_up21.x+3.6,vertex5.y+tmp.p19hw/2,0);
			Data->MyFunction->Arx->CreateLinearDim(vertex8,vertex_08,point3,90,Data->MyFunction->Basic->Double2CString(tmp.p19hw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			if(i != 0 || aircylinderDataVec[ii].isHorizonBlade != 1.0)
			{
				//����Ҷ�ǲ��עup
				double p79 = (tmp.p30Dz-tmp.p0Jz-tmp.p9bz-tmp.p17Bw+tmp.p18bw)/2;
				point3 = AcGePoint3d(ids_up2.x+p79,vertex4.y+8.4,0);
				Data->MyFunction->Arx->CreateLinearDim(ids_up2,ids_up11,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p30Dz-tmp.p0Jz-tmp.p9bz-tmp.p17Bw+tmp.p18bw),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

				//����Ҷ�ǲ��עdown
				double p81 = (tmp.p30Dz-tmp.p0Jz-tmp.p6bw-tmp.p21Bz+tmp.p22bz)/2;
				point3 = AcGePoint3d(ids_down2.x+p81,zhangchai_point.y-2.6,0);
				Data->MyFunction->Arx->CreateLinearDim(ids_down2,ids_down11,point3,0,Data->MyFunction->Basic->Double2CString(tmp.p30Dz-tmp.p0Jz-tmp.p6bw-tmp.p21Bz+tmp.p22bz),tmp.p31zf,AirAxialDis+setting.OriginalPoint.x,isdelete);

			}
			
			//�ж��������� 
			if(aircylinderDataVec[ii].AirPassage=="����"||aircylinderDataVec[ii].AirPassage=="����")
			{
				//����
				if(tmp.p31zf==0)
					Data->MyFunction->Arx->CreateImage(AircylinderObjects,AcGePoint3d(AirAxialDis+setting.OriginalPoint.x,0.0,0),AcGePoint3d(AirAxialDis+setting.OriginalPoint.x,1.0,0),1);
			}
			else if(aircylinderDataVec[ii].AirPassage=="˫����")
			{
				//����
				if(tmp.p31zf==0)
					Data->MyFunction->Arx->CreateImage(AircylinderObjects,AcGePoint3d(AirAxialDis+setting.OriginalPoint.x,0.0,0),AcGePoint3d(AirAxialDis+setting.OriginalPoint.x,1.0,0),0);
			}
			
		}
#pragma endregion
		
	}
	return 0;
}


//����Ҷ��				          /*ҶƬ���ͣ�      Ҷ������,        �ṹ��,                     �м����׼�㣬                             ����vec*/
int ClassTongliu::createHub(string bladeType, double hubtype, bladeData tmp,vector<AcGePoint3d>vertex_out_vec, AcDbObjectIdArray &AircylinderObjects)
{
	AcDbObjectId GeneralObjects ,yegen_down_line_id,yegen_up_line_id;
	AcGePoint3d StartPoint,EndPoint;
	AcDbObjectIdArray ListObjects;
	AcDbObjectIdArray ReListObjects;
	if(bladeType == "��Ҷ") 
#pragma region
	{
		AcGePoint3d vertex_01 = vertex_out_vec[0];
		AcGePoint3d vertex_04 = vertex_out_vec[3];
		AcGePoint3d ids_up1 = vertex_out_vec[4];
		AcGePoint3d ids_up2 = vertex_out_vec[5];
		//Ҷ��
		AcGePoint3d stator_daojiao1,stator_daojiao2;
		AcGePoint3d stator_daojiao3,stator_daojiao4;
		//�����ϣ�������
		stator_daojiao1 = AcGePoint3d(vertex_01.x,vertex_01.y-tmp.p6,vertex_01.z);
		stator_daojiao2 = AcGePoint3d(vertex_04.x,vertex_04.y-tmp.p4,vertex_04.z);
		stator_daojiao3 = AcGePoint3d(vertex_01.x+tmp.p5,vertex_01.y,vertex_01.z);
		stator_daojiao4 = AcGePoint3d(vertex_04.x-tmp.p3,vertex_04.y,vertex_04.z);

		Data->MyFunction->Arx->CreateLine(stator_daojiao1,ids_up1,&GeneralObjects);//������
		AircylinderObjects.append(GeneralObjects);
		Data->MyFunction->Arx->CreateLine(ids_up1,ids_up2,&yegen_down_line_id);
		AircylinderObjects.append(yegen_down_line_id);
		Data->MyFunction->Arx->CreateLine(ids_up2,stator_daojiao2,&GeneralObjects);//������
		AircylinderObjects.append(GeneralObjects);
		//Data->MyFunction->Arx->CreateLine(vertex_04,vertex_01);

		//Ҷ�����ֵ���
		Data->MyFunction->Arx->CreateLine(stator_daojiao1,stator_daojiao3,&GeneralObjects);//������
		AircylinderObjects.append(GeneralObjects);
		Data->MyFunction->Arx->CreateLine(stator_daojiao2,stator_daojiao4,&GeneralObjects);//������
		AircylinderObjects.append(GeneralObjects);

		//���¹�����ҶƬҶ��   �����£�������
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
		stator_p5 = AcGePoint3d(tmp.p0Jz-tmp.Sbz,stator_p3.y+tan(tmp.SHubHorizontalAngle * PI /180.0)*(tmp.Sbz-tmp.Sbn/2.0),0);//Ҷ���н�
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
		//���� ��
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

		//���� ��
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
		//��վ�Ҷ��
		Data->MyFunction->Arx->CreateLine(stator_mid7,stator_mid8,&GeneralObjects);
		AircylinderObjects.append(GeneralObjects);
	}
#pragma endregion

	else if(bladeType == "��Ҷ")
	{
		AcGePoint3d vertex_06 = vertex_out_vec[1];
		AcGePoint3d vertex_07 = vertex_out_vec[2];
		AcGePoint3d ids_down11 = vertex_out_vec[4];
		AcGePoint3d ids_down21 = vertex_out_vec[5];

		if(hubtype == 1)
		//����ʽԤŤTҶ��
#pragma region
		{
			//Ҷ��
			AcGePoint3d rotor_daojiao1,rotor_daojiao2;
			AcGePoint3d rotor_daojiao3,rotor_daojiao4;
			//�����£�������
			rotor_daojiao1 = AcGePoint3d(vertex_06.x,vertex_06.y+tmp.p24,vertex_06.z);
			rotor_daojiao2 = AcGePoint3d(vertex_07.x,vertex_07.y+tmp.p24,vertex_07.z);
			rotor_daojiao3 = AcGePoint3d(vertex_06.x+tmp.p24,vertex_06.y,vertex_06.z);
			rotor_daojiao4 = AcGePoint3d(vertex_07.x-tmp.p24,vertex_07.y,vertex_07.z);
			Data->MyFunction->Arx->CreateLine(ids_down11,rotor_daojiao1,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);//������
			//Data->MyFunction->Arx->CreateLine(vertex_06,vertex_07);
			Data->MyFunction->Arx->CreateLine(rotor_daojiao2,ids_down21,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);//������
			Data->MyFunction->Arx->CreateLine(ids_down21,ids_down11,&yegen_up_line_id);
			AircylinderObjects.append(yegen_up_line_id);

			//Ҷ�����ֵ���
			Data->MyFunction->Arx->CreateLine(rotor_daojiao1,rotor_daojiao3,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);//������
			Data->MyFunction->Arx->CreateLine(rotor_daojiao2,rotor_daojiao4,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);//������

			//���¹�����ҶƬҶ��   �����£�������
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
			rotor_p5 = AcGePoint3d(tmp.p30Dz-tmp.bz,rotor_p3.y-tan(tmp.HubHorizontalAngle * PI /180.0)*(tmp.bz-tmp.bn/2.0),0);//Ҷ���н�
			rotor_p6 = AcGePoint3d(tmp.p30Dz+tmp.bz,rotor_p4.y-tan(tmp.HubHorizontalAngle * PI /180.0)*(tmp.bz-tmp.bn/2.0),0);
			rotor_p7 = AcGePoint3d(rotor_p5.x,rotor_p5.y-tmp.hx,0);
			rotor_p8 = AcGePoint3d(rotor_p6.x,rotor_p6.y-tmp.hx,0);

			//���������� ������ڽ�ȡ0.4 ����֮  ��ʱΪ����
			rotor_mid1 = AcGePoint3d(rotor_p5.x+tmp.daojiao_cb,rotor_p5.y+tan(tmp.HubHorizontalAngle * PI /180.0)*(tmp.daojiao_cb),0);
			rotor_mid3 = AcGePoint3d(rotor_p5.x+0.4,rotor_p5.y-tmp.daojiao_cb+0.4,0);
			rotor_mid5 = AcGePoint3d(rotor_mid3.x,rotor_p7.y+tmp.daojiao_c-0.4,0);
			rotor_mid7 = AcGePoint3d(rotor_p7.x+tmp.daojiao_c,rotor_p7.y,0);

			rotor_mid2 = AcGePoint3d(rotor_p6.x-tmp.daojiao_cb,rotor_p6.y+tan(tmp.HubHorizontalAngle * PI /180.0)*(tmp.daojiao_cb),0);
			rotor_mid4 = AcGePoint3d(rotor_p6.x,rotor_p6.y-tmp.daojiao_cb,0);
			rotor_mid6 = AcGePoint3d(rotor_mid4.x,rotor_p8.y+tmp.daojiao_c,0);
			rotor_mid8 = AcGePoint3d(rotor_p8.x-tmp.daojiao_c,rotor_p8.y,0);

			AcDbObjectId rotor_id_tmp,rotor_id1,rotor_id2,rotor_id3,rotor_id4,rotor_id5,rotor_id6,rotor_id7,rotor_id8;
			//���� ��
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

			//���ڽ�ȡ0.4
			Data->MyFunction->Arx->CreateLine(rotor_mid1,rotor_mid3,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_mid3 ,rotor_mid5,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_mid5,rotor_mid7,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			
			//���� ��
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
			//�Ҳ಻�ý�ȡ0.4
			Data->MyFunction->Arx->CreateLine(rotor_mid2,rotor_mid4,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_mid4 ,rotor_mid6,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(rotor_mid6,rotor_mid8,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			//��ն�Ҷ��
			Data->MyFunction->Arx->CreateLine(rotor_mid7,rotor_mid8,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
		}
#pragma endregion
		else if(hubtype == 2)
		//�嶯ʽ��ԤŤTҶ��
#pragma region
		{
			Data->MyFunction->Arx->CreateLine(ids_down11,ids_down21,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			//�нǶ�
			if(tmp.TZAngle == 0.0)
			{
				//Ҷ�����
				AcGePoint3d chi1, chi2, chi3, chi4, chi5, chi6, chi7, chi8;
				//���µ���
				chi8 = AcGePoint3d(vertex_06.x,vertex_06.y+tmp.TSFH-1.0,vertex_06.z);
				chi7 = AcGePoint3d(chi8.x-3.0,chi8.y+1.0,chi7.z);
				chi6 = AcGePoint3d(chi7.x,chi7.y+1.0,chi7.z);
				chi5 = AcGePoint3d(chi6.x+3.0 ,chi6.y,chi6.z);
				chi4 = AcGePoint3d(chi5.x,chi5.y+2.0,chi5.z);
				chi3 = AcGePoint3d(chi4.x-3.0,chi4.y+1.0,chi4.z);
				chi2 = AcGePoint3d(chi3.x,chi3.y+1.0,chi3.z);
				chi1 = AcGePoint3d(chi2.x+3.0,chi2.y,chi2.z);
				//����ֱ�� �ϵ��� ��
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
				//�ұ�
				Data->MyFunction->Arx->CreateLine(ids_down21,vertex_07,&GeneralObjects);
				AircylinderObjects.append(GeneralObjects);

				//���¹�����ҶƬҶ��   �����£�������
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
				//���� ½�������Ҵ��ϵ���
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
			//�޽Ƕ�
			else
			{
				//�м���
				Data->MyFunction->Arx->CreateLine(ids_down11,vertex_06 ,&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);
				Data->MyFunction->Arx->CreateLine(vertex_06,AcGePoint3d(tmp.p30Dz,vertex_06.y,vertex_06.z ),&GeneralObjects);
				ListObjects.append(GeneralObjects);
				AircylinderObjects.append(GeneralObjects);

				//���¹�����ҶƬҶ��   �����£�������
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
				//���� ½�������Ҵ��ϵ���
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
		//��ԤŤ˫��TҶ��
#pragma region
		{
			Data->MyFunction->Arx->CreateLine(ids_down11,ids_down21,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			//Ҷ�����
			AcGePoint3d chi1, chi2, chi3, chi4, chi5, chi6, chi7, chi8;
			//���µ���
			chi8 = AcGePoint3d(vertex_06.x,vertex_06.y+tmp.h-1.0,vertex_06.z);
			chi7 = AcGePoint3d(chi8.x-3.0,chi8.y+1.0,chi7.z);
			chi6 = AcGePoint3d(chi7.x,chi7.y+1.0,chi7.z);
			chi5 = AcGePoint3d(chi6.x+3.0 ,chi6.y,chi6.z);
			chi4 = AcGePoint3d(chi5.x,chi5.y+2.0,chi5.z);
			chi3 = AcGePoint3d(chi4.x-3.0,chi4.y+1.0,chi4.z);
			chi2 = AcGePoint3d(chi3.x,chi3.y+1.0,chi3.z);
			chi1 = AcGePoint3d(chi2.x+3.0,chi2.y,chi2.z);
			//����ֱ�� �ϵ��� ��
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
			//�ұ�
			Data->MyFunction->Arx->CreateLine(ids_down21,vertex_07,&GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			//���¹�����ҶƬҶ��   �����£�������
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
		//ԤŤ˫��TҶ��
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

			//���¹�����ҶƬҶ��   �����£�������
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

			//������۳�0.4������������񣬲�����Ҷ������
			Data->MyFunction->Arx->CreateLine(rotor_p5,AcGePoint3d(rotor_p6.x+0.4,rotor_p6.y+0.4,0),  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(AcGePoint3d(rotor_p6.x+0.4,rotor_p6.y+0.4,0),AcGePoint3d(rotor_p6.x+0.4,rotor_p7.y-0.4,0),  &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			Data->MyFunction->Arx->CreateLine(AcGePoint3d(rotor_p6.x+0.4,rotor_p7.y-0.4,0),rotor_p8 , &GeneralObjects);
			AircylinderObjects.append(GeneralObjects);
			//������
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

//����Ҷ����									/*ҶƬ���ͣ�    Ҷ������,       �ṹ�壬	��һ���ṹ�壬      �м������׼�㣬         �м����һ�׼��,				����vec,				��һҶ���ۻ�׼��ȵ㣨ע�����һ��û����ȵ㣩*/
int ClassTongliu::createBladeHubGroove(string bladeType, double hubtype, bladeData tmp,  bladeData next_tmp,  AcGePoint3d basePoint_l, AcGePoint3d basePoint_r,   AcDbObjectIdArray &AircylinderObjects,  AcGePoint3d deepPoint/* = AcGePoint3d::kOrigin*/)
{
	AcGePoint3d StartPoint,EndPoint;
	//�����ϣ����ҵ��� Ԥ�ȴ��ҵ���
	AcGePoint3d groove_mid1,groove_mid3,groove_mid5,groove_mid7,groove_mid9,groove_mid11,groove_mid13,groove_mid15,groove_mid14;
	AcDbObjectIdArray ListObjects;
	AcDbObjectIdArray ReListObjects;
	AcDbObjectId tmp1,tmp2,tmp3;
	AcDbObjectId GeneralObjects;

	if(bladeType == "��Ҷ") 
#pragma region
	{
		groove_mid1  = AcGePoint3d(basePoint_r.x-(tmp.SMidBodyH-tmp.SGrooveNeckW)/2.0+tmp.SGrooveNeckChamfer, basePoint_r.y,0);
		groove_mid3  = AcGePoint3d(groove_mid1.x-tmp.SGrooveNeckChamfer,groove_mid1.y+tmp.SGrooveNeckChamfer,0);
		groove_mid5  = AcGePoint3d(groove_mid3.x,groove_mid1.y+tmp.SGrooveNeckH -tmp.SGrooveNeckChamfer,0);
		groove_mid7  = AcGePoint3d(groove_mid5.x+tmp.SGrooveNeckChamfer,groove_mid5.y+tmp.SGrooveNeckChamfer+tan(tmp.SHubHorizontalAngle * PI /180.0)*tmp.SGrooveNeckChamfer,0);
		groove_mid9  = AcGePoint3d(groove_mid5.x+(tmp.SGrooveBottomW-tmp.SGrooveNeckW)/2.0,groove_mid5.y+tmp.SGrooveNeckChamfer + tan(tmp.SHubHorizontalAngle * PI /180.0)*(tmp.SGrooveBottomW-tmp.SGrooveNeckW)/2.0,0);
		groove_mid11 = AcGePoint3d(0,0,0);
		AcGePoint3d groove_mids[] ={groove_mid1 , groove_mid3, groove_mid5, groove_mid7, groove_mid9};

		//Ҷ����
		AcGePoint3d S_deep_point = tmp.S_deep_point;
		double x1 = tmp.p0Jz;
		double dx = S_deep_point.x - x1;
		S_deep_point.x = x1 - dx;

		//�����£�������
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

		//Ҷ����������
		if(!Data->MyFunction->Arx->IsPointSame(deepPoint, AcGePoint3d(0,0,0)))
		{
			AcGePoint3d r_point = S_deep_point;

			//���г�������ߵ�������
			if(tmp.IsSuctionPort==1.0)
			{

				//�Ҳ��ȼ�����һ���м������׼��
				AcGePoint3d vertex1   = AcGePoint3d(next_tmp.p0Jz + next_tmp.p4bd -next_tmp.p2BD, next_tmp.p26Dpp + next_tmp.p27Lp/2.0, 0);
				AcGePoint3d vertex4   = AcGePoint3d(next_tmp.p0Jz + next_tmp.p4bd , next_tmp.p26Dpp + next_tmp.p27Lp/2.0, 0);
				//��Χ������ ˳����ʱ��
				AcGePoint3d vertex_01 = AcGePoint3d(next_tmp.p0Jz + next_tmp.p9bz -next_tmp.p8Bz, vertex1.y+next_tmp.p10hz, 0);
				//�Ƕ�
				double angle_up = next_tmp.p11ad * PI /180.0;

				//��ҶƬ����
				AcDbObjectId  tag_upl_ray_Id,tag_upr_ray_Id,ray_upl_ID,ray_upr_ID;
				Data->MyFunction->Arx->CreateRay(vertex_01,AcGeVector3d(0,-1,0), &tag_upl_ray_Id);
				Data->MyFunction->Arx->CreateLine(vertex4,angle_up,  1000.0,0, &ray_upl_ID);

				//���׼��
				AcGePoint3d ids_up1=AcGePoint3d(0,0,0);
				Data->MyFunction->Arx->GetObjectIntersect(tag_upl_ray_Id,ray_upl_ID,&ids_up1);
				Data->MyFunction->Arx->DeleteObject(tag_upl_ray_Id);
				Data->MyFunction->Arx->DeleteObject(ray_upl_ID);

				if(tmp.CoverDegree<=0.5)
				{
					//���Ϊֱ��
					Data->MyFunction->Arx->CreateLine(r_point, AcGePoint3d(r_point.x+tmp.OutletWeight,r_point.y,0) ,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(AcGePoint3d(r_point.x+tmp.OutletWeight,r_point.y,0), AcGePoint3d(r_point.x+tmp.OutletWeight,r_point.y+40.0,0) ,&GeneralObjects);
					AircylinderObjects.append(GeneralObjects);

					//�Ҳ�ΪԲ��
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
					//����Ϊ2*45����
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
			//���¼�����������
			else if(next_tmp.BrokenTag!=1.0)
			{
				//�Աȴ˲��������һ�����
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

			//���¼������򲻻������
			if(next_tmp.BrokenTag!=1.0)
			{
				//������Ų� �ж����������
				AcDbObjectId ObjectId;
				if(tmp.t_type == 1)//һ��һ��һ��
				{
					Data->MyFunction->Arx->InsertBlock("R_L_short",AcGePoint3d(r_point.x+tmp.a-0.65,r_point.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("R_L_long",AcGePoint3d(r_point.x+tmp.b-0.65,r_point.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("R_L_short",AcGePoint3d(r_point.x+tmp.c-0.65,r_point.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
				}
				else if(tmp.t_type == 2)// һ��һ��
				{
					Data->MyFunction->Arx->InsertBlock("R_L_long",AcGePoint3d(r_point.x+tmp.b-0.65,r_point.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
					Data->MyFunction->Arx->InsertBlock("R_L_short",AcGePoint3d(r_point.x+tmp.c-0.65,r_point.y,0), ObjectId);
					AircylinderObjects.append(ObjectId);
				}
				else if(tmp.t_type == 3)// �̳�
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
	else if(bladeType == "��Ҷ")
	{
		if(hubtype == 1)
		//����ʽԤŤTҶ��
#pragma region
		{
			//�����£������� Ԥ�ȴ�����
			groove_mid1  = AcGePoint3d(basePoint_l.x+(tmp.MidBodyH-tmp.GrooveNeckW)/2.0-tmp.GrooveNeckChamfer, basePoint_l.y,0);
			groove_mid3  = AcGePoint3d(groove_mid1.x+tmp.GrooveNeckChamfer,groove_mid1.y-tmp.GrooveNeckChamfer,0);
			groove_mid5  = AcGePoint3d(groove_mid3.x,groove_mid1.y-tmp.GrooveNeckH +tmp.GrooveNeckChamfer,0);
			groove_mid7  = AcGePoint3d(groove_mid5.x-tmp.GrooveNeckChamfer,groove_mid5.y-tmp.GrooveNeckChamfer-tan(tmp.HubHorizontalAngle * PI /180.0)*tmp.GrooveNeckChamfer,0);
			groove_mid9  = AcGePoint3d(groove_mid5.x-(tmp.GrooveBottomW-tmp.GrooveNeckW)/2.0,groove_mid5.y-tmp.GrooveNeckChamfer - tan(tmp.HubHorizontalAngle * PI /180.0)*(tmp.GrooveBottomW-tmp.GrooveNeckW)/2.0,0);
			groove_mid11 = AcGePoint3d(0,0,0);
			AcGePoint3d groove_mids_r[] ={groove_mid1 , groove_mid3, groove_mid5, groove_mid7, groove_mid9};

			//Ҷ����
			//if(tmp.p31zf==1.0)
			//{
				//�����£�������
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

				//Ҷ����������
				if((!Data->MyFunction->Arx->IsPointSame(deepPoint, AcGePoint3d(0,0,0)))&&(next_tmp.HubType==1.0||next_tmp.HubType==4.0))
				{
					AcGePoint3d r_point = tmp.deep_point;
					r_point.x = r_point.x + tmp.p21Bz;

					AcGePoint3d r_point_qufeng = tmp.rator_deeppoint;
					r_point_qufeng.x = r_point_qufeng.x + tmp.p21Bz;
					
					//���г�������ߵ�������
					if(tmp.IsSuctionPort==1.0)
					{
						//��Ҷ
						//�������� ҶƬ�ı������� ˳����ʱ��
						AcGePoint3d vertex7 = AcGePoint3d(tmp.p30Dz + tmp.p15bg ,tmp.p28Dpd - tmp.p32, 0);
						//��Χ�����ĵ� ˳����ʱ��
						AcGePoint3d vertex_07 = AcGePoint3d(tmp.p30Dz + tmp.p22bz , vertex7.y-tmp.p23hz , 0);

						//��ҶƬ����
						AcDbObjectId tag_downr_ray_Id1,ray_downr_ID1;
						Data->MyFunction->Arx->CreateRay(vertex_07,AcGeVector3d(0,1,0) , &tag_downr_ray_Id1);

						//�Ƕ�
						double angle_down = tmp.p25ag * PI /180.0;
						Data->MyFunction->Arx->CreateLine(vertex7,angle_down,1000.0,1, &ray_downr_ID1);

						//Ҷ����
						AcGePoint3d ids_down21=AcGePoint3d(0,0,0); 
						Data->MyFunction->Arx->GetObjectIntersect(tag_downr_ray_Id1,ray_downr_ID1,&ids_down21);

						//ɾ��������
						Data->MyFunction->Arx->DeleteObject(tag_downr_ray_Id1);
						Data->MyFunction->Arx->DeleteObject(ray_downr_ID1);


						//��ҶƬ����
						AcGePoint3d theP=AcGePoint3d(0,0,0);
						AcDbObjectId  ray_upl_ID;
						Data->MyFunction->Arx->CreateLine(ids_down21,angle_down,  100000.0,1, &ray_upl_ID);
						Data->MyFunction->Arx->CreateLine(deepPoint,AcGePoint3d(deepPoint.x-100000.0,deepPoint.y,deepPoint.z),&GeneralObjects);
						Data->MyFunction->Arx->GetObjectIntersect(ray_upl_ID,GeneralObjects,&theP);
						Data->MyFunction->Arx->DeleteObject(ray_upl_ID);
						Data->MyFunction->Arx->DeleteObject(GeneralObjects);

						//���׼��
						Data->MyFunction->Arx->CreateLine(ids_down21,theP, &GeneralObjects);
						AircylinderObjects.append(GeneralObjects);
						Data->MyFunction->Arx->CreateLine(theP,deepPoint,  &GeneralObjects);
						AircylinderObjects.append(GeneralObjects);

					}
					//���¼�����������
					else if(next_tmp.BrokenTag!=1.0)
					{
						//�Աȴ˲��������һ�����
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
					//���¼������򲻻�����ݲ�
					if(next_tmp.BrokenTag!=1.0)
					{
						//��������ݲ�
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
		//�嶯ʽ��ԤŤTҶ��
#pragma region
		{
			//�޽Ƕ�
			if(tmp.NeckZAngle == 0.0)
			{
				//���¹�����ҶƬҶ��   �����£�������
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
				//���� ½�������Ҵ��ϵ���
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
			//�нǶ�
			else
			{
				//���¹�����ҶƬҶ��   �����£�������
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
				//���� ½�������Ҵ��ϵ���
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
		//��ԤŤ˫��TҶ��
#pragma region
		{
			//���¹�����ҶƬҶ��   �����£�������
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
		//ԤŤ˫��TҶ��
#pragma region
		{
			//���¹�����ҶƬҶ��   �����£�������
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
			rotor_p15 = AcGePoint3d(tmp.p30Dz-tmp.p21Bz*5.0/50.0 ,rotor_p14.y-0.5 ,0);//��ʱ�뾶
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
			Data->MyFunction->Arx->CreateFillet(rotor_id2, StartPoint , rotor_id3, rotor_p9, tmp.p21Bz*8.0/50.0, &rotor_id_tmp);//�뾶��ʱ
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

			Data->MyFunction->Arx->CreateArc(rotor_p16, tmp.p21Bz*5.0/50.0, 180.0, 270.0, &GeneralObjects);//��ʱ�뾶
			ListObjects.append(GeneralObjects);
			AircylinderObjects.append(GeneralObjects);

			ReListObjects = Data->MyFunction->Arx->CreateImage(ListObjects,AcGePoint3d(tmp.p30Dz,1,0),AcGePoint3d(tmp.p30Dz,0,0),0);
			AircylinderObjects.append(ListObjects);
			AircylinderObjects.append(ReListObjects);

			//Ҷ����������
			if((!Data->MyFunction->Arx->IsPointSame(deepPoint, AcGePoint3d(0,0,0)))&&(next_tmp.HubType==1.0||next_tmp.HubType==4.0))
			{
				AcGePoint3d r_point = tmp.deep_point;
				r_point.x = r_point.x + tmp.p21Bz;

				AcGePoint3d r_point_qufeng = tmp.rator_deeppoint;
				r_point_qufeng.x = r_point_qufeng.x + tmp.p21Bz;
				
				//���г�������ߵ�������
				if(tmp.IsSuctionPort==1.0)
				{
					//��Ҷ
					//�������� ҶƬ�ı������� ˳����ʱ��
					AcGePoint3d vertex7 = AcGePoint3d(tmp.p30Dz + tmp.p15bg ,tmp.p28Dpd - tmp.p32, 0);
					//��Χ�����ĵ� ˳����ʱ��
					AcGePoint3d vertex_07 = AcGePoint3d(tmp.p30Dz + tmp.p22bz , vertex7.y-tmp.p23hz , 0);

					//��ҶƬ����
					AcDbObjectId tag_downr_ray_Id1,ray_downr_ID1;
					Data->MyFunction->Arx->CreateRay(vertex_07,AcGeVector3d(0,1,0) , &tag_downr_ray_Id1);

					//�Ƕ�
					double angle_down = tmp.p25ag * PI /180.0;
					Data->MyFunction->Arx->CreateLine(vertex7,angle_down,1000.0,1, &ray_downr_ID1);

					//Ҷ����
					AcGePoint3d ids_down21=AcGePoint3d(0,0,0); 
					Data->MyFunction->Arx->GetObjectIntersect(tag_downr_ray_Id1,ray_downr_ID1,&ids_down21);

					//ɾ��������
					Data->MyFunction->Arx->DeleteObject(tag_downr_ray_Id1);
					Data->MyFunction->Arx->DeleteObject(ray_downr_ID1);


					//��ҶƬ����
					AcGePoint3d theP=AcGePoint3d(0,0,0);
					AcDbObjectId  ray_upl_ID;
					Data->MyFunction->Arx->CreateLine(ids_down21,angle_down,  100000.0,1, &ray_upl_ID);
					Data->MyFunction->Arx->CreateLine(deepPoint,AcGePoint3d(deepPoint.x-100000.0,deepPoint.y,deepPoint.z),&GeneralObjects);
					Data->MyFunction->Arx->GetObjectIntersect(ray_upl_ID,GeneralObjects,&theP);
					Data->MyFunction->Arx->DeleteObject(ray_upl_ID);
					Data->MyFunction->Arx->DeleteObject(GeneralObjects);

					//���׼��
					Data->MyFunction->Arx->CreateLine(ids_down21,theP, &GeneralObjects);
					AircylinderObjects.append(GeneralObjects);
					Data->MyFunction->Arx->CreateLine(theP,deepPoint,  &GeneralObjects);
					AircylinderObjects.append(GeneralObjects);

				}
				//���¼������������Ұ�����ҶƬ������������֮�����
				else if(next_tmp.BrokenTag!=1.0)
				{
					//�Աȴ˲��������һ�����
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
				//���¼������򲻻�����ݲ�
				if(next_tmp.BrokenTag!=1.0)
				{
					//��������ݲ�
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


//����ĩ����                                        /*ĩ�����������ƣ�    ԭ��,    �Ƿ���       ������       �Ƿ�ɾ��ԭʼ          ����vec*/
int ClassTongliu::createLastThreeStages( string TypeName, AcGePoint3d originP,double Image,double AxisNum,int isDelete, AcDbObjectIdArray &AircylinderObjects )
{
	AcDbObjectId ObjectId;
	Data->MyFunction->Arx->InsertBlock(TypeName.c_str(),originP, ObjectId);
	AircylinderObjects.append(ObjectId);

	Data->MyFunction->Arx->SetNowLayerName(L"5�ߴ���");
	AcGePoint3d p1,p2,p3,p4;
	CString Ctext;
	double c_num;
	//ĩ��������ע  ���µ��� ������
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

	Data->MyFunction->Arx->SetNowLayerName(L"1��ʵ��");
	return 0;
}

//��ͼ
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



