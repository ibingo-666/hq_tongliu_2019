#pragma once
#include "stdafx.h"
#include "float.h"
#include <iostream>


#include "arxHeaders.h"								//ObjectARX总头文件以及lib引用
#include "rxmfcapi.h"								//一些API
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



/////////////////////////////////CAD类/////////////////////////////////
class ClassCADFunc
{
public:
	ClassCADFunc();
	~ClassCADFunc();

public:
//CAD数据库
//**********************************

	//添加实体到模型空间
	AcDbObjectId AddObjectToModl(AcDbEntity *pEnt);		

	//遍历当前数据库获得所有实体ID
	int GetAllObjectId(AcDbObjectIdArray *ListObjects);	

//Arx函数操作类
//**********************************
//图形***********
	//创建直线		
	int CreateLine(AcGePoint3d StartPoint, AcGePoint3d EndPoint);		

	//创建直线
	int CreateLine(AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcDbObjectId *ObjectId);

	//创建直线 /*起始点 方向向量 延伸长度 正方向（1为正，0为负） 直线id*/
	int CreateLine(AcGePoint3d StartPoint, double angle, double length, int dir, AcDbObjectId *ObjectId);

	//创建射线
	int CreateRay(AcGePoint3d StartPoint, AcGeVector3d vec, AcDbObjectId *ObjectId);

	//创建圆弧
	int CreateArc(AcGePoint3d Core, double Radius, double StartAngle, double EndAngle, AcDbObjectId *ObjectId);		
	
	//创建圆弧
	int CreateArc(AcGePoint3d Core, double Radius, AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcDbObjectId *ObjectId);											

	//创建倒圆角
	int CreateFillet(AcDbObjectId& ObjectOneId, AcGePoint3d DatumPointA, AcDbObjectId& ObjectTwoId, AcGePoint3d DatumPointB, double Radius, AcDbObjectId *FilletId);

//内容***********
	//创建文字
	AcDbObjectId CreateMText(AcGePoint3d PointCoor, CString text, AcDbMText::AttachmentPoint attachment=AcDbMText::kTopLeft, int Color=0, AcDbObjectId style=AcDbObjectId::kNull, double height=3.0, double width=10);

//对象***********
	//获得两个对象的第一个交点
	int GetObjectIntersect(AcDbObjectId ObjectOneId, AcDbObjectId ObjectTwoId, AcGePoint3d *Point, AcDb::Intersect IntType = AcDb::kOnBothOperands);	
	
	//获得两个对象的第一个交点
	int GetObjectIntersect(AcDbEntity *pEntA, AcDbEntity *pEntB, AcGePoint3d *Point, AcDb::Intersect IntType = AcDb::kOnBothOperands);							

	//获得一个Curve对象的起点
	int GetObjectStartPoint(AcDbObjectId ObjectId, AcGePoint3d& StartPoint);	

	//获得一个Curve对象的起点
	int GetObjectStartPoint(AcDbEntity *pEnt, AcGePoint3d& StartPoint);	

	//获得一个Curve对象的终点
	int GetObjectEndPoint(AcDbObjectId ObjectId, AcGePoint3d& EndPoint);

	//获得一个Curve对象的终点
	int GetObjectEndPoint(AcDbEntity *pEnt, AcGePoint3d& EndPoint);		

	//Trim实体
	AcDbObjectId TrimObject(AcDbObjectId ObjectId1, AcDbObjectId ObjectId2, AcGePoint3d PointCoor);

	//删除对象
	int DeleteObject(AcDbObjectId ObjectId);

	//删除对象
	int DeleteObject(AcDbEntity *pEnt);		

	//批量删除对象列表
	int DeleteListObject(AcDbObjectIdArray ListObjectId);

	//通过文字样式名称获得文字样式ID
	AcDbObjectId GetNameTextStyle(CString StrName);																												


//坐标矩阵
	//创建镜像
	int CreateImage(AcDbObjectId ObjectId, AcGePoint3d PointLineA, AcGePoint3d PointLineB, AcDbObjectId *ImageId, bool BolDelete = 0);		

	//原地复制一个对象
	AcDbObjectId CopyObject(AcDbObjectId ObjectId);	

	//镜像点
	AcGePoint3d MirrorTransform(const AcGePoint3d& point, const AcGePoint3d& mirrorAxisStart, const AcGePoint3d& mirrorAxisEnd);

	//旋转对象(沿Z轴旋转)
	int RotateObject(AcDbObjectId ListObjectId, AcGePoint3d PointCoor, double ValueAngle, int Mode = 0);														

//批量操作*******
	//创建镜像
	AcDbObjectIdArray CreateImage(AcDbObjectIdArray ListObjectId, AcGePoint3d PointLineA, AcGePoint3d PointLineB,int isDelete);

//图层***********
	//新建图层
	int CreateLayer(CString StrLayerName, int ColorId, CString LineTypeName);

	//将指定图层设为当前图层
	int SetNowLayerName(CString StrName);	

//图块***********
	//获得块颜色
	int GetBlockColor(AcDbObjectId ObjectId);	

	//获得当前模型空间中所有块名
	int GetAllBlockName(CStringArray *StrNameArray);	

	//插入当前数据库内的块
	int InsertBlock(CString BlockName, AcGePoint3d PointCoor,AcDbObjectId& ObjectId);
	
	//删除块表记录
	int DeleteBlock(CString StrBlockName);	

	//将指定路径下的指定名称块插入到当前数据库
	int ImportBlock(CString StrPath, CString StrBlock/*, AcDbObjectId *ObjectId*/);	

	//将指定路径下块插入到当前数据库
	int ImportBlock(CString StrPath);	

//标注***********
	//创建线性标注
	AcDbObjectId CreateLinearDim(AcGePoint3d PointA, AcGePoint3d PointB, AcGePoint3d Poin, double Angle, CString DimText = L"", AcDbObjectId DimStyle = NULL);	

	//创建线性标注/*镜像条件，1为不变，0为镜像*//*是否删除原始值，1为是，0为否*/
	AcDbObjectId CreateLinearDim(AcGePoint3d PointA, AcGePoint3d PointB, AcGePoint3d Poin, double Angle, CString DimText,double Image,double AxisNum,int isDelete);	
	
	//创建角度标注               /*顶点、                       起始点、              终止点              标注尺寸线*/
	AcDbObjectId CreateAngleDim(AcGePoint3d Center, AcGePoint3d PointEnd1, AcGePoint3d PointEnd2, AcGePoint3d PointPlace, CString DimText = L"", AcDbObjectId DimStyle = NULL);

	//创建角度标注				/*镜像条件，1为不变，0为镜像*/
	AcDbObjectId CreateAngleDim( AcGePoint3d Center, AcGePoint3d PointEnd1, AcGePoint3d PointEnd2, AcGePoint3d PointPlace, CString DimText,double Image,double AxisNum,int isDelete);

	//指定两点的半径标注        /*圆点、                       起始点、              文字距起始点              文字*/
	AcDbObjectId CreateRadiusDim(AcGePoint3d PointA, AcGePoint3d PointB, double LeaderLength = 5, CString DimText = L"", AcDbObjectId DimStyle = NULL);				

	//指定两点的半径标注        /*镜像条件，1为不变，0为镜像*/
	AcDbObjectId CreateRadiusDim(AcGePoint3d PointA, AcGePoint3d PointB, double LeaderLength, CString DimText,double Image,double AxisNum,int isDelete);				
	
	//创建引线	
	/*
	StartPoint:引线起始点
	EndPoint:引线第二点
	LeadPoint:引线终止点
	StrText:文字内容
	TextHeight:文字高度
	TextColor:文字颜色
	*/
	int CreateLeader(AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcGePoint3d LeadPoint, CString StrText, CString StyName, int LeaderColor, double TextHeight = 3.5, int TextColor = 0);	

	//创建引线			/*镜像条件，1为不变，0为镜像*/
	int CreateLeader(AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcGePoint3d LeadPoint, CString StrText,double Image,double AxisNum,int isDelete);	

//分析***********
	//测量两坐标点距离
	double GetPointDist(AcGePoint3d PointOne, AcGePoint3d PointTwo);																							

	//判断两个点是否相等
	bool IsPointSame(AcGePoint3d PointOne, AcGePoint3d PointTwo, double Tolerance = 0.02);		

	//获得直线与X轴的夹角
	double GetLineAndXaxisAngle(AcGePoint3d PointOne, AcGePoint3d PointTwo, int Mode = 1);		

	//获得直线与X轴的夹角
	double GetLineAndXaxisAngle(AcGePoint3d PointOne, AcGePoint3d PointTwo, AcGePoint3d DatumPoint, int Mode = 1);	

	//获得两条相交线的夹角
	double GetInterLineAngle(AcGePoint3d PointOver, AcGePoint3d PointOne, AcGePoint3d PointTwo, int Type = 1);		

	//计算点到直线的垂直投影点
	AcGePoint3d GetProjectionPointOnLine(const AcGePoint3d& point, const AcGePoint3d& lineStart, const AcGePoint3d& lineEnd);

	//检查对象是否是线性标注
	bool IsLinearDimension(AcDbObjectId objectId);

	//获取线性标注信息
	void GetLinearDimData(AcDbObjectId dimId, AcGePoint3d& startPoint, AcGePoint3d& endPoint, AcGePoint3d& textPoint);
};


/////////////////////////////////基本方法类/////////////////////////////////
class ClassBasicFunc
{
public:
	ClassBasicFunc();
	~ClassBasicFunc();

public:
//基本方法类
//**********************************
//类型转换类
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
//数学方法类
	//四舍五入
	double round(double number, int num_digits);
	//向上取整
	double roundNumber(double number, int num_digits);
	//向下取整
	double floorNumber(double number, double significance);
	//按照倍数向上取整
	double ceilToSignificance(double number, double significance);


};


/////////////////////////////////我的所有类/////////////////////////////////
class ClassMyFunction
{
public:
	ClassMyFunction()
	{
		Arx = new ClassCADFunc;
		Basic = new ClassBasicFunc;
	}

public:
	//CAD类
	ClassCADFunc *Arx;			
	//基本方法类
	ClassBasicFunc *Basic;			
};