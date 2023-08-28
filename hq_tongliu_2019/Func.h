#pragma once
#include "stdafx.h"
#include "float.h"
#include <iostream>


#include "arxHeaders.h"								//ObjectARX��ͷ�ļ��Լ�lib����
#include "rxmfcapi.h"								//һЩAPI
#include <afx.h>
#include <afxwin.h>
#include <winuser.h>
#include <stdio.h> 
#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <atlstr.h>  
#include <string.h>
#include <io.h>
#include <iostream>   
#include <fstream>
#include <conio.h>
#include <dos.h>
#include <time.h>					
#include <stdlib.h>					
#include <direct.h>
#include "Winspool.h"
#include "Windows.h"

#include <GdiPlus.h>
#include <sstream>
#include "gdiplus.h" 
#include "gs.h"
#include <string>    
#include "string.h"
using namespace std;
using std::string;



/////////////////////////////////CAD��/////////////////////////////////
class ClassCADFunc
{
public:
	ClassCADFunc();
	~ClassCADFunc();

public:
//CAD���ݿ�
//**********************************

	//���ʵ�嵽ģ�Ϳռ�
	AcDbObjectId AddObjectToModl(AcDbEntity *pEnt);		

	//������ǰ���ݿ�������ʵ��ID
	int GetAllObjectId(AcDbObjectIdArray *ListObjects);	

//Arx����������
//**********************************
//ͼ��***********
	//����ֱ��		
	int CreateLine(AcGePoint3d StartPoint, AcGePoint3d EndPoint);		

	//����ֱ��
	int CreateLine(AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcDbObjectId *ObjectId);

	//����ֱ�� /*��ʼ�� �������� ���쳤�� ������1Ϊ����0Ϊ���� ֱ��id*/
	int CreateLine(AcGePoint3d StartPoint, double angle, double length, int dir, AcDbObjectId *ObjectId);

	//��������
	int CreateRay(AcGePoint3d StartPoint, AcGeVector3d vec, AcDbObjectId *ObjectId);

	//����Բ��
	int CreateArc(AcGePoint3d Core, double Radius, double StartAngle, double EndAngle, AcDbObjectId *ObjectId);		
	
	//����Բ��
	int CreateArc(AcGePoint3d Core, double Radius, AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcDbObjectId *ObjectId);											

	//������Բ��
	int CreateFillet(AcDbObjectId& ObjectOneId, AcGePoint3d DatumPointA, AcDbObjectId& ObjectTwoId, AcGePoint3d DatumPointB, double Radius, AcDbObjectId *FilletId);

//����***********
	//��������
	AcDbObjectId CreateMText(AcGePoint3d PointCoor, CString text, AcDbMText::AttachmentPoint attachment=AcDbMText::kTopLeft, int Color=0, AcDbObjectId style=AcDbObjectId::kNull, double height=3.0, double width=10);

//����***********
	//�����������ĵ�һ������
	int GetObjectIntersect(AcDbObjectId ObjectOneId, AcDbObjectId ObjectTwoId, AcGePoint3d *Point, AcDb::Intersect IntType = AcDb::kOnBothOperands);	
	
	//�����������ĵ�һ������
	int GetObjectIntersect(AcDbEntity *pEntA, AcDbEntity *pEntB, AcGePoint3d *Point, AcDb::Intersect IntType = AcDb::kOnBothOperands);							

	//���һ��Curve��������
	int GetObjectStartPoint(AcDbObjectId ObjectId, AcGePoint3d& StartPoint);	

	//���һ��Curve��������
	int GetObjectStartPoint(AcDbEntity *pEnt, AcGePoint3d& StartPoint);	

	//���һ��Curve������յ�
	int GetObjectEndPoint(AcDbObjectId ObjectId, AcGePoint3d& EndPoint);

	//���һ��Curve������յ�
	int GetObjectEndPoint(AcDbEntity *pEnt, AcGePoint3d& EndPoint);		

	//Trimʵ��
	AcDbObjectId TrimObject(AcDbObjectId ObjectId1, AcDbObjectId ObjectId2, AcGePoint3d PointCoor);

	//ɾ������
	int DeleteObject(AcDbObjectId ObjectId);

	//ɾ������
	int DeleteObject(AcDbEntity *pEnt);		

	//����ɾ�������б�
	int DeleteListObject(AcDbObjectIdArray ListObjectId);

	//ͨ��������ʽ���ƻ��������ʽID
	AcDbObjectId GetNameTextStyle(CString StrName);																												


//�������
	//��������
	int CreateImage(AcDbObjectId ObjectId, AcGePoint3d PointLineA, AcGePoint3d PointLineB, AcDbObjectId *ImageId, bool BolDelete = 0);		

	//ԭ�ظ���һ������
	AcDbObjectId CopyObject(AcDbObjectId ObjectId);	

	//�����
	AcGePoint3d MirrorTransform(const AcGePoint3d& point, const AcGePoint3d& mirrorAxisStart, const AcGePoint3d& mirrorAxisEnd);

	//��ת����(��Z����ת)
	int RotateObject(AcDbObjectId ListObjectId, AcGePoint3d PointCoor, double ValueAngle, int Mode = 0);														

//��������*******
	//��������
	AcDbObjectIdArray CreateImage(AcDbObjectIdArray ListObjectId, AcGePoint3d PointLineA, AcGePoint3d PointLineB,int isDelete);

//ͼ��***********
	//�½�ͼ��
	int CreateLayer(CString StrLayerName, int ColorId, CString LineTypeName);

	//��ָ��ͼ����Ϊ��ǰͼ��
	int SetNowLayerName(CString StrName);	

//ͼ��***********
	//��ÿ���ɫ
	int GetBlockColor(AcDbObjectId ObjectId);	

	//��õ�ǰģ�Ϳռ������п���
	int GetAllBlockName(CStringArray *StrNameArray);	

	//���뵱ǰ���ݿ��ڵĿ�
	int InsertBlock(CString BlockName, AcGePoint3d PointCoor,AcDbObjectId& ObjectId);
	
	//ɾ������¼
	int DeleteBlock(CString StrBlockName);	

	//��ָ��·���µ�ָ�����ƿ���뵽��ǰ���ݿ�
	int ImportBlock(CString StrPath, CString StrBlock/*, AcDbObjectId *ObjectId*/);	

	//��ָ��·���¿���뵽��ǰ���ݿ�
	int ImportBlock(CString StrPath);	

//��ע***********
	//�������Ա�ע
	AcDbObjectId CreateLinearDim(AcGePoint3d PointA, AcGePoint3d PointB, AcGePoint3d Poin, double Angle, CString DimText = L"", AcDbObjectId DimStyle = NULL);	

	//�������Ա�ע/*����������1Ϊ���䣬0Ϊ����*//*�Ƿ�ɾ��ԭʼֵ��1Ϊ�ǣ�0Ϊ��*/
	AcDbObjectId CreateLinearDim(AcGePoint3d PointA, AcGePoint3d PointB, AcGePoint3d Poin, double Angle, CString DimText,double Image,double AxisNum,int isDelete);	
	
	//�����Ƕȱ�ע               /*���㡢                       ��ʼ�㡢              ��ֹ��              ��ע�ߴ���*/
	AcDbObjectId CreateAngleDim(AcGePoint3d Center, AcGePoint3d PointEnd1, AcGePoint3d PointEnd2, AcGePoint3d PointPlace, CString DimText = L"", AcDbObjectId DimStyle = NULL);

	//�����Ƕȱ�ע				/*����������1Ϊ���䣬0Ϊ����*/
	AcDbObjectId CreateAngleDim( AcGePoint3d Center, AcGePoint3d PointEnd1, AcGePoint3d PointEnd2, AcGePoint3d PointPlace, CString DimText,double Image,double AxisNum,int isDelete);

	//ָ������İ뾶��ע        /*Բ�㡢                       ��ʼ�㡢              ���־���ʼ��              ����*/
	AcDbObjectId CreateRadiusDim(AcGePoint3d PointA, AcGePoint3d PointB, double LeaderLength = 5, CString DimText = L"", AcDbObjectId DimStyle = NULL);				

	//ָ������İ뾶��ע        /*����������1Ϊ���䣬0Ϊ����*/
	AcDbObjectId CreateRadiusDim(AcGePoint3d PointA, AcGePoint3d PointB, double LeaderLength, CString DimText,double Image,double AxisNum,int isDelete);				
	
	//��������	
	/*
	StartPoint:������ʼ��
	EndPoint:���ߵڶ���
	LeadPoint:������ֹ��
	StrText:��������
	TextHeight:���ָ߶�
	TextColor:������ɫ
	*/
	int CreateLeader(AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcGePoint3d LeadPoint, CString StrText, CString StyName, int LeaderColor, double TextHeight = 3.5, int TextColor = 0);	

	//��������			/*����������1Ϊ���䣬0Ϊ����*/
	int CreateLeader(AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcGePoint3d LeadPoint, CString StrText,double Image,double AxisNum,int isDelete);	

//����***********
	//��������������
	double GetPointDist(AcGePoint3d PointOne, AcGePoint3d PointTwo);																							

	//�ж��������Ƿ����
	bool IsPointSame(AcGePoint3d PointOne, AcGePoint3d PointTwo, double Tolerance = 0.02);		

	//���ֱ����X��ļн�
	double GetLineAndXaxisAngle(AcGePoint3d PointOne, AcGePoint3d PointTwo, int Mode = 1);		

	//���ֱ����X��ļн�
	double GetLineAndXaxisAngle(AcGePoint3d PointOne, AcGePoint3d PointTwo, AcGePoint3d DatumPoint, int Mode = 1);	

	//��������ཻ�ߵļн�
	double GetInterLineAngle(AcGePoint3d PointOver, AcGePoint3d PointOne, AcGePoint3d PointTwo, int Type = 1);		

	//����㵽ֱ�ߵĴ�ֱͶӰ��
	AcGePoint3d GetProjectionPointOnLine(const AcGePoint3d& point, const AcGePoint3d& lineStart, const AcGePoint3d& lineEnd);

	//�������Ƿ������Ա�ע
	bool IsLinearDimension(AcDbObjectId objectId);

	//��ȡ���Ա�ע��Ϣ
	void GetLinearDimData(AcDbObjectId dimId, AcGePoint3d& startPoint, AcGePoint3d& endPoint, AcGePoint3d& textPoint);
};


/////////////////////////////////����������/////////////////////////////////
class ClassBasicFunc
{
public:
	ClassBasicFunc();
	~ClassBasicFunc();

public:
//����������
//**********************************
//����ת����
	//Cstring 2 char*
	const char* CString2Char(CString Str);
	//char* 2 Cstring
	CString Char2CString(const char* pChar);
	//double 2 char*
	const char* Double2Char(double d1);
	//double 2 Cstring
	CString Double2CString(double d1);
	//wstring 2 string
	string wstring2String(const std::wstring& wstr);

//**********************************
//��ѧ������
	//��������
	double round(double number, int num_digits);
	//����ȡ��
	double roundNumber(double number, int num_digits);
	//����ȡ��
	double floorNumber(double number, double significance);
	//���ձ�������ȡ��
	double ceilToSignificance(double number, double significance);


};


/////////////////////////////////�ҵ�������/////////////////////////////////
class ClassMyFunction
{
public:
	ClassMyFunction()
	{
		Arx = new ClassCADFunc;
		Basic = new ClassBasicFunc;
	}

public:
	//CAD��
	ClassCADFunc *Arx;			
	//����������
	ClassBasicFunc *Basic;			
};