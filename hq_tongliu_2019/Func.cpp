#include "Func.h"
#include<math.h>
#include <stdio.h>
#include <cmath>



#define PI 3.14159265358979323846264338327950288419716939937510


ClassCADFunc::ClassCADFunc()
{

}

ClassCADFunc::~ClassCADFunc()
{

}

ClassBasicFunc::ClassBasicFunc()
{

}
//CAD数据库***********************************************

//添加实体到模型空间
AcDbObjectId ClassCADFunc::AddObjectToModl( AcDbEntity *pEnt )
{
	AcDbObjectId ObjectId;

	AcDbBlockTable *pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);
	AcDbBlockTableRecord *pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	pBlockTableRecord->appendAcDbEntity(ObjectId, pEnt);
	pBlockTable->close();
	pBlockTableRecord->close();
	pEnt->close();

	return ObjectId;

}

//遍历当前数据库获得所有实体ID
int ClassCADFunc::GetAllObjectId( AcDbObjectIdArray *ListObjects )
{
	AcDbBlockTable *pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);		//获得指向块表的指针
	AcDbBlockTableRecord *pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);								//获得指向模型空间的指针
	//创建一个遍历迭代器
	AcDbBlockTableRecordIterator *pBlkTblRcdItr;
	pBlockTableRecord->newIterator(pBlkTblRcdItr);
	//遍历模型空间块表记录
	AcGePoint3d PointInitial(0.0, 0.0, 0.0);
	AcDbEntity *pEnt;
	AcDbObjectId ObjectId;
	for (pBlkTblRcdItr->start(); !pBlkTblRcdItr->done(); pBlkTblRcdItr->step())
	{
		if (pBlkTblRcdItr->getEntity(pEnt, AcDb::kForRead) == 0)
		{
			ObjectId = NULL;
			ObjectId = pEnt->objectId();
			if (ObjectId != NULL)
			{
				ListObjects->append(ObjectId);
			}
			pEnt->close();
		}
	}
	pBlockTableRecord->close();
	delete pBlkTblRcdItr;
	pBlockTable->close();
	return 0;
}




//Arx函数操作 图形类****************************************

//创建直线		
int ClassCADFunc::CreateLine( AcGePoint3d StartPoint, AcGePoint3d EndPoint )
{
	AcDbLine *pLine = new AcDbLine(StartPoint, EndPoint);
	double length = 0.0;
	pLine->getDistAtPoint(EndPoint,length);
	int if_finite = _finite(length);
	if(if_finite==0)
	{
		delete pLine;
		pLine = NULL;
		return -1;
	}
	pLine->setLinetypeScale(0.1,true);
	AddObjectToModl(pLine);

	return 0;

}

//创建直线
int ClassCADFunc::CreateLine( AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcDbObjectId *ObjectId )
{
	AcDbLine *pLine = new AcDbLine(StartPoint, EndPoint);
	double length = 0.0;
	pLine->getDistAtPoint(EndPoint,length);
	int if_finite = _finite(length);
	if(if_finite==0)
	{
		delete pLine;
		pLine = NULL;
		*ObjectId = NULL;
		return -1;
	}
	pLine->setLinetypeScale(0.1,true);
	*ObjectId = AddObjectToModl(pLine);

	return 0;
}

//创建直线 /*起始点 方向向量 延伸长度 正方向（1为正，0为负） 直线id*/
int ClassCADFunc::CreateLine(AcGePoint3d StartPoint, double angle, double length, int dir, AcDbObjectId *ObjectId )
{
	AcGePoint3d EndPoint;
	if(!dir)//正方向
		EndPoint = AcGePoint3d(StartPoint.x-length*cos(angle),StartPoint.y-length*sin(angle),0);
	else
		EndPoint = AcGePoint3d(StartPoint.x+length*cos(angle),StartPoint.y+length*sin(angle),0);

	AcDbLine *pLine = new AcDbLine(StartPoint, EndPoint);
	double length1 = 0.0;
	pLine->getDistAtPoint(EndPoint,length1);
	int if_finite = _finite(length1);
	if(if_finite==0)
	{
		delete pLine;
		pLine = NULL;
		*ObjectId = NULL;
		return -1;
	}
	pLine->setLinetypeScale(0.1,true);
	*ObjectId = AddObjectToModl(pLine);

	return 0;
}

//创建射线
int ClassCADFunc::CreateRay(AcGePoint3d StartPoint, AcGeVector3d vec, AcDbObjectId *ObjectId)
{
	AcDbRay *pRay = new AcDbRay();
	pRay->setBasePoint(StartPoint);
	pRay->setUnitDir(vec);
	*ObjectId = AddObjectToModl(pRay);
	
	return 0;
}

/*
Core:圆心
Radius:半径
StartAngle:起始角度
EndAngle:终止角度
ObjectId:返回的对象
*/
//创建圆弧
int ClassCADFunc::CreateArc(AcGePoint3d Core, double Radius, double StartAngle, double EndAngle, AcDbObjectId *ObjectId)
{
	if (EndAngle == 360.0 && StartAngle == 0.0)
	{
		AcGeVector3d Vector(0.0, 0.0, 1.0);
		AcDbCircle *pCircle = new AcDbCircle(Core, Vector, Radius);
		*ObjectId = AddObjectToModl(pCircle);
	}
	else
	{
		double ArcStart = StartAngle * PI / 180;
		double ArcEnd = EndAngle * PI / 180;
		AcDbArc *pArc = new AcDbArc(Core, Radius, ArcStart, ArcEnd);
		*ObjectId = AddObjectToModl(pArc);
	}
	return 0;
}
/*
Core:圆心
Radius:半径
StartPoint:圆弧的第一点
EndPoint:圆弧的第二点
ObjectId:返回的对象
*/
//创建圆弧
int ClassCADFunc::CreateArc( AcGePoint3d Core, double Radius, AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcDbObjectId *ObjectId )
{
	double StartAngle = 0.0, EndAngle = 0.0;
	StartAngle = GetLineAndXaxisAngle(Core, StartPoint, Core);
	EndAngle = GetLineAndXaxisAngle(Core, EndPoint, Core);
	if (StartAngle > EndAngle)
	{
		EndAngle = EndAngle + 360.0;
	}
	CreateArc(Core, Radius, StartAngle, EndAngle, ObjectId);
	return 0;
}

//创建倒圆角
/*
ObjectOneId:要倒角的对象一
DatumPointA:对象一基准点
ObjectTwoId:要倒角的对象二
DatumPointB:对象二基准点
Radius:倒角半径
FilletId:返回的倒角对象
*/
int ClassCADFunc::CreateFillet( AcDbObjectId& ObjectOneId, AcGePoint3d DatumPointA, AcDbObjectId& ObjectTwoId, AcGePoint3d DatumPointB, double Radius, AcDbObjectId *FilletId )
{
	AcDbEntity *pOffGroupEnt[4];
	int Number = 0;
	AcDbEntity *pEnt;
	//打开实体并偏移
	AcDbCurve *pCur;
	AcDbVoidPtrArray ListEnts(1, 1);
	if (acdbOpenObject(pEnt, ObjectOneId, AcDb::kForRead) != 0)
	{
		AfxMessageBox(L"-1");
		return -1;
	}
	pCur = (AcDbCurve*)pEnt;

	AcDbCurve *pCur1 = AcDbCurve::cast(pEnt);
	pCur1->getOffsetCurves(Radius, ListEnts);
	if (ListEnts.length() > 0)
	{
		pOffGroupEnt[Number] = (AcDbEntity*)ListEnts.at(0);
		//AddObjectToModl((AcDbEntity*)ListEnts.at(0));
		Number++;
	}
	ListEnts.removeAll();

	pCur1->getOffsetCurves(0.0 - Radius, ListEnts);
	if (ListEnts.length() > 0)
	{
		pOffGroupEnt[Number] = (AcDbEntity*)ListEnts.at(0);
		//AddObjectToModl((AcDbEntity*)ListEnts.at(0));
		Number++;
	}
	ListEnts.removeAll();
	pEnt->close();

	//打开实体并偏移
	if (acdbOpenObject(pEnt, ObjectTwoId, AcDb::kForRead) != 0)
	{
		//AddObjectToModl((AcDbEntity*)ListEnts.at(0));
		AfxMessageBox(L"-1");
		return -1;
	}
	//pCur = (AcDbCurve*)pEnt;
	AcDbCurve *pCur2 = AcDbCurve::cast(pEnt);
	pCur2->getOffsetCurves(Radius, ListEnts);
	if (ListEnts.length() > 0)
	{
		pOffGroupEnt[Number] = (AcDbEntity*)ListEnts.at(0);
		//AddObjectToModl((AcDbEntity*)ListEnts.at(0));
		Number++;
	}
	ListEnts.removeAll();

	pCur2->getOffsetCurves(0.0 - Radius, ListEnts);
	if (ListEnts.length() > 0)
	{
		pOffGroupEnt[Number] = (AcDbEntity*)ListEnts.at(0);
		//AddObjectToModl((AcDbEntity*)ListEnts.at(0));
		Number++;
	}
	ListEnts.removeAll();
	pEnt->close();

	//
	//CString Str;
	//Str.Format(L"%d", Number);
	//AfxMessageBox(Str);

	if (Number < 1)
	{
		AfxMessageBox(L"-2");
		return -2;
	}


	//相互获得交点并创建圆
	AcGePoint3d GroupCenter[10];
	int NumberCenter = 0;
	for (int i = 0; i < Number; i++)
	{
		for (int u = 0; u < Number; u++)
		{
			if (pOffGroupEnt[i] != pOffGroupEnt[u])
			{
				//AfxMessageBox(L"AA");
				if (GetObjectIntersect(pOffGroupEnt[i], pOffGroupEnt[u], 
					&GroupCenter[NumberCenter], AcDb::kExtendBoth) > 0)
				{
					//AfxMessageBox(L"BB");
					if (NumberCenter == 0)
					{
						NumberCenter++;
					}
					for (int k = 0; k < NumberCenter; k++)
					{
						if (IsPointSame(GroupCenter[NumberCenter], GroupCenter[k]) == 1)
						{
							break;
						}
						if (k == NumberCenter - 1)
						{
							NumberCenter++;
						}
					}
				}
			}
		}
	}
	//Str.Format(L"%d", NumberCenter);
	//AfxMessageBox(Str);

	//删除偏移对象
	for (int i = 0; i < Number; i++)
	{
		//pOffGroupEnt[i]->erase();
		//pOffGroupEnt[i]->close();
		DeleteObject(pOffGroupEnt[i]);
	}

	if (NumberCenter < 1)
	{
		//AfxMessageBox(L"-3");
		return -3;
	}

	//获得最近点
	double ValueDist = 0.0, MinValue = 0.0;
	AcGePoint3d MinCenter;
	for (int i = 0; i < NumberCenter; i++)
	{
		ValueDist = GetPointDist(GroupCenter[i], DatumPointA) + GetPointDist(GroupCenter[i], DatumPointB);
		if (i == 0)
		{
			MinValue = ValueDist;
			MinCenter = GroupCenter[i];
		}
		else if (ValueDist < MinValue)
		{
			MinValue = ValueDist;
			MinCenter = GroupCenter[i];
		}
	}

	//创建圆弧
	AcDbObjectId ObjectId, tempObjectId;
	CreateArc(MinCenter, Radius, 0.0, 360.0, &ObjectId);
	if (GetObjectIntersect(ObjectOneId, ObjectId, &GroupCenter[0], AcDb::kExtendThis) < 0)
	{
		DeleteObject(ObjectId);
		//AfxMessageBox(L"-4");
		return -4;
	}
	if (GetObjectIntersect(ObjectTwoId, ObjectId, &GroupCenter[1], AcDb::kExtendThis) < 0)
	{
		DeleteObject(ObjectId);
		//AfxMessageBox(L"-4");
		return -4;
	}
	DeleteObject(ObjectId);

	double ValueAngle = GetInterLineAngle(MinCenter, GroupCenter[0], GroupCenter[1]);
	double A, B, C;
	A = GetLineAndXaxisAngle(MinCenter, GroupCenter[0], MinCenter);
	B = GetLineAndXaxisAngle(MinCenter, GroupCenter[1], MinCenter);

	if (A > B)
	{
		C = B;
		B = A;
		A = C;
	}

	if (B - A > 180)
	{
		A += 360;
		C = B;
		B = A;
		A = C;
	}
	//CString Str;
	//Str.Format(L"MinCenter.x, MinCenter.y, Radius, A, B:%f, %f, %f, %f, %f", MinCenter.x, MinCenter.y, Radius, A, B);
	//AfxMessageBox(Str);
	CreateArc(MinCenter, Radius, A, B, &ObjectId);

	*FilletId = ObjectId;


	AcGePoint3d StartP, EndP;
	GetObjectStartPoint(ObjectId, StartP);
	GetObjectEndPoint(ObjectId, EndP);
	CreateLine(StartP, EndP, &ObjectId);

	ObjectOneId = TrimObject(ObjectOneId, ObjectId, DatumPointA);
	ObjectTwoId = TrimObject(ObjectTwoId, ObjectId, DatumPointB);

	DeleteObject(ObjectId);

	return 0;

}

//Arx函数操作 内容类****************************************
//创建文字
AcDbObjectId ClassCADFunc::CreateMText( AcGePoint3d PointCoor, CString text, AcDbMText::AttachmentPoint attachment/*=AcDbMText::kTopLeft*/, int Color/*=0*/, AcDbObjectId style/*=AcDbObjectId::kNull*/, double height/*=3.0*/, double width/*=10*/ )
{
	ErrorStatus err_type;
	AcDbMText *pMText = new AcDbMText();
	pMText->setTextStyle(style);
	pMText->setLocation(PointCoor);
	pMText->setTextHeight(height);
	pMText->setColorIndex(Color);
	pMText->setWidth(width);
	pMText->setAttachment(attachment);
	pMText->setContents(text);

	pMText->setBackgroundFill(true);
	//尝试背景遮罩透明，失败
	//AcCmTransparency* transparency_set = new AcCmTransparency();
	//transparency_set->setAlpha(0);
	//err_type = pMText->setBackgroundTransparency(*transparency_set);
	err_type = pMText->setBackgroundScaleFactor(1);

	return AddObjectToModl(pMText);
}

//Arx函数操作 对象类****************************************
/*
ObjectIdA:对象一
ObjectIdB:对象二
*/
//获得两个对象的第一个交点
int ClassCADFunc::GetObjectIntersect( AcDbObjectId ObjectOneId, AcDbObjectId ObjectTwoId, AcGePoint3d *Point, AcDb::Intersect IntType /*= AcDb::kOnBothOperands*/ )
{
	if(ObjectOneId==NULL || ObjectTwoId==NULL)
		return -1;
	AcGePoint3dArray ArrayPoints(1, 1);
	AcDbEntity *p1, *p2;
	acdbOpenAcDbEntity(p1, ObjectOneId, AcDb::kForRead);
	acdbOpenAcDbEntity(p2, ObjectTwoId, AcDb::kForRead);
	p1->intersectWith(p2, IntType, ArrayPoints);
	p1->close();
	p2->close();

	if (ArrayPoints.length() > 0)
	{
		Point->x = ArrayPoints.at(0).x;
		Point->y = ArrayPoints.at(0).y;
		Point->z = ArrayPoints.at(0).z;
		return 0;
	}
	if (ArrayPoints.length() == 0)
	{
		return -1;
	}
	return 0;
}
//获得两个对象的第一个交点
int ClassCADFunc::GetObjectIntersect(AcDbEntity *pEntA, AcDbEntity *pEntB, AcGePoint3d *Point, AcDb::Intersect IntType)
{
	AcGePoint3dArray ArrayPoints(1, 1);
	pEntA->intersectWith(pEntB, IntType, ArrayPoints);
	if (ArrayPoints.length() > 0)
	{
		Point->x = ArrayPoints.at(0).x;
		Point->y = ArrayPoints.at(0).y;
		Point->z = ArrayPoints.at(0).z;
		return  ArrayPoints.length();
	}
	return 0;
}


//获得一个对象的起点
int ClassCADFunc::GetObjectStartPoint(AcDbObjectId ObjectId, AcGePoint3d& StartPoint)
{
	//打开实体
	AcDbEntity *pEnt;
	if (acdbOpenAcDbEntity(pEnt, ObjectId, AcDb::kForRead) != 0)
	{
		return -1;
	}

	AcDbCurve *pCur = AcDbCurve::cast(pEnt);
	pCur->getStartPoint(StartPoint);
	pEnt->close();
	return 0;
}
int ClassCADFunc::GetObjectStartPoint(AcDbEntity *pEnt, AcGePoint3d& StartPoint)
{
	AcDbCurve *pCur = AcDbCurve::cast(pEnt);
	pCur->getStartPoint(StartPoint);
	pEnt->close();
	return 0;
}

//获得一个对象的终点
int ClassCADFunc::GetObjectEndPoint(AcDbObjectId ObjectId, AcGePoint3d& EndPoint)
{
	//打开实体
	AcDbEntity *pEnt;
	if (acdbOpenAcDbEntity(pEnt, ObjectId, AcDb::kForRead) != 0)
	{
		return -1;
	}

	AcDbCurve *pCur = AcDbCurve::cast(pEnt);
	pCur->getEndPoint(EndPoint);
	pEnt->close();
	return 0;
}
int ClassCADFunc::GetObjectEndPoint(AcDbEntity *pEnt, AcGePoint3d& EndPoint)
{
	AcDbCurve *pCur = AcDbCurve::cast(pEnt);
	pCur->getEndPoint(EndPoint);
	pEnt->close();
	return 0;
}

//Trim实体
AcDbObjectId ClassCADFunc::TrimObject(AcDbObjectId ObjectId1, AcDbObjectId ObjectId2, AcGePoint3d PointCoor)
{
	//打开对象
	AcDbEntity *pEnt1, *pEnt2, *pTemEnt;
	AcGePoint3dArray ArrayPoints(1, 1);
	if (acdbOpenAcDbEntity(pEnt1, ObjectId1, AcDb::kForWrite) != 0)
	{
		return ObjectId1;
	}
	if (acdbOpenAcDbEntity(pEnt2, ObjectId2, AcDb::kForWrite) != 0)
	{
		return ObjectId1;
	}


	//转为曲线对象
	AcDbCurve *pCur1 = AcDbCurve::cast(pEnt1);
	if (pCur1 == NULL)
	{

		pEnt1->close();
		return NULL;
	}


	//获得点的链表
	AcGePoint3dArray ListPoints(1, 1);
	pEnt1->intersectWith(pEnt2, AcDb::kExtendBoth, ArrayPoints);
	ListPoints.append(ArrayPoints.at(0));
	//CString Str;
	//Str.Format(L"ListPointsSize:%d", ListPoints.length());
	//AfxMessageBox(Str);

	//打断
	AcDbVoidPtrArray *ListEnts = new AcDbVoidPtrArray();
	if (pCur1->getSplitCurves(ListPoints, *ListEnts) != 0)
	{
		pEnt1->close();
		return NULL;
	}
	pEnt1->close();
	pEnt2->close();
	//Str.Format(L"ListEntsSize:%d", ListEnts->length());
	//AfxMessageBox(Str);

	////删除原有对象
	//if (BolDelete == 1)
	//{
	DeleteObject(ObjectId1);
	//}

	AcDbObjectIdArray ListObjects;
	AcDbObjectId TemObjectId;
	//添加到模型空间
	for (int i = 0; i < ListEnts->length(); i++)
	{
		AcDbCurve *pCurtemp = AcDbCurve::cast((AcDbEntity*)ListEnts->at(i));
		AcGePoint3d closePoint;
		pCurtemp->getClosestPointTo(PointCoor, closePoint);

		//CString Str;
		//Str.Format(L"Dist:%f", GetPointDist(PointCoor, closePoint));
		//AfxMessageBox(Str);

		if (IsPointSame(PointCoor, closePoint, 0.001))
		{

			TemObjectId = AddObjectToModl(pCurtemp);
			ListObjects.append(TemObjectId);

		}
		else
		{
			DeleteObject(AddObjectToModl(pCurtemp));

		}

	}
	return TemObjectId;
}

//删除对象
int ClassCADFunc::DeleteObject( AcDbObjectId ObjectId )
{
	AcDbEntity *pEnt = NULL;
	if (acdbOpenObject(pEnt, ObjectId, AcDb::kForWrite) != 0)
	{
		return -1;
	}
	if (pEnt->erase() != 0)
	{
		pEnt->close();
		return -2;
	}
	pEnt->close();
	return 0;
}

//删除对象
int ClassCADFunc::DeleteObject(AcDbEntity *pEnt)
{
	if (pEnt->erase() != 0)
	{
		pEnt->close();
		return -2;
	}
	pEnt->close();
	return 0;
}

//批量删除对象列表
int ClassCADFunc::DeleteListObject( AcDbObjectIdArray ListObjectId )
{
	AcDbEntity *pEnt = NULL;
	for (int i = 0; i<ListObjectId.length(); i++)
	{
		if (acdbOpenObject(pEnt, ListObjectId.at(i), AcDb::kForWrite) == 0)
		{
			pEnt->erase();
			pEnt->close();
		}
	}
	return 0;
}

//通过文字样式名称获得文字样式ID
AcDbObjectId ClassCADFunc::GetNameTextStyle( CString StrName )
{
	int Err = -1;

	//获得文字样式Id
	AcDbTextStyleTable *pTextStyleTbl;
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	pDb->getTextStyleTable(pTextStyleTbl, AcDb::kForWrite);
	if (!pTextStyleTbl->has(StrName))
	{
		pTextStyleTbl->close();
		return NULL;
	}

	AcDbObjectId TextStyleId;
	Err = pTextStyleTbl->getAt(StrName, TextStyleId);
	if (Err != 0)
	{
		pTextStyleTbl->close();
		return NULL;
	}

	pTextStyleTbl->close();

	return TextStyleId;

}

//Arx函数操作 坐标矩阵类****************************************
/*
ObjectId:要镜像的对象
PointLineA:镜像直线第一点
PointLineB:镜像直线第二点
ImageId:返回的镜像对象
BolDelete:是否删除原对象
*/
//创建镜像
int ClassCADFunc::CreateImage( AcDbObjectId ObjectId, AcGePoint3d PointLineA, AcGePoint3d PointLineB, AcDbObjectId *ImageId, bool BolDelete /*= 0*/ )
{
	//原地复制一个对象
	AcDbObjectId CopyObjectId = NULL;
	CopyObjectId = CopyObject(ObjectId);
	if (CopyObjectId == NULL)
	{
		return -1;
	}

	AcGeMatrix3d Mat;
	AcGeLine3d Line;
	Line.set(PointLineA, PointLineB);
	Mat.setToMirroring(Line);
	AcDbEntity *pEnt = NULL;
	Acad::ErrorStatus Err = acdbOpenObject(pEnt, CopyObjectId, AcDb::kForWrite);
	if (Err != Acad::eOk)
	{
		return -1;
	}

	AcDbEntity *pCopyEnt = AcDbEntity::cast(pEnt->clone());
	if (pCopyEnt)
	{
		pEnt->transformBy(Mat);
	}
	pEnt->close();
	*ImageId = CopyObjectId;
	if (BolDelete == 1)
	{
		DeleteObject(ObjectId);
	}
	return 0;
}


//原地复制一个对象
AcDbObjectId ClassCADFunc::CopyObject( AcDbObjectId ObjectId )
{
	AcDbEntity *pEnt = NULL;
	if (acdbOpenObject(pEnt, ObjectId, AcDb::kForWrite) != 0)
	{
		return NULL;
	}
	AcDbEntity *pCopyEnt = AcDbEntity::cast(pEnt->clone());
	if (pCopyEnt != NULL)
	{
		pEnt->close();
		return AddObjectToModl(pCopyEnt);
	}
	pEnt->close();
	return 0;
}





//镜像点
AcGePoint3d ClassCADFunc::MirrorTransform( const AcGePoint3d& point, const AcGePoint3d& mirrorAxisStart, const AcGePoint3d& mirrorAxisEnd )
{
	// 计算对称轴的方向向量
	AcGeVector3d axisVector = mirrorAxisEnd - mirrorAxisStart;

	// 将方向向量归一化为单位向量
	AcGeVector3d axisUnitVector = axisVector.normal();

	// 计算从对称轴起点到给定点的向量
	AcGeVector3d axisToPointVector = point - mirrorAxisStart;

	// 计算给定点到对称轴的投影距离
	double projectionDistance = axisToPointVector.dotProduct(axisUnitVector);

	// 计算给定点到对称轴的垂直距离
	AcGeVector3d perpendicularVector = axisToPointVector - projectionDistance * axisUnitVector;

	// 计算镜像点的位置
	AcGePoint3d mirrorPoint = point - 2.0 * perpendicularVector;

	return mirrorPoint;
}

//旋转对象(沿Z轴旋转)
int ClassCADFunc::RotateObject( AcDbObjectId ListObjectId, AcGePoint3d PointCoor, double ValueAngle, int Mode /*= 0*/ )
{
	AcGeVector3d Vector(0.0, 0.0, 1.0);
	AcGeMatrix3d mat;
	AcDbEntity *pEnt;
	double value_arc;
	if (Mode == 0)
	{
		mat.setToRotation(PI / 180.0*ValueAngle, Vector, PointCoor);
	}
	else
	{
		mat.setToRotation(ValueAngle, Vector, PointCoor);
	}

	if (acdbOpenObject(pEnt, ListObjectId, AcDb::kForWrite) != 0)
	{
		return -1;
	}
	pEnt->transformBy(mat);
	pEnt->close();
	return 0;
}

//Arx函数操作 批量操作类****************************************

//创建镜像
AcDbObjectIdArray ClassCADFunc::CreateImage( AcDbObjectIdArray ListObjectId, AcGePoint3d PointLineA, AcGePoint3d PointLineB ,int isDelete)
{
	AcDbObjectIdArray ListImageObjects;
	AcDbObjectId ObjectId;
	for (int i = 0; i < ListObjectId.length(); i++)
	{
		if(isDelete)
			CreateImage(ListObjectId.at(i), PointLineA, PointLineB, &ObjectId,1);
		else
			CreateImage(ListObjectId.at(i), PointLineA, PointLineB, &ObjectId);

		ListImageObjects.append(ObjectId);
	}
	return ListImageObjects;

}

//Arx函数操作 图层类****************************************

//新建图层
int ClassCADFunc::CreateLayer( CString StrLayerName, int ColorId, CString LineTypeName )
{
	//判断是否存在名称为 StrLayerName 的图层
	AcDbLayerTable *pLayerTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (pLayerTbl->has(StrLayerName))
	{
		pLayerTbl->close();
		return -1;
	}


	//新建层表记录
	AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord();
	pLayerTblRcd->setName(StrLayerName);

	//设置图层颜色
	AcCmColor color;
	color.setColorIndex(ColorId);
	pLayerTblRcd->setColor(color);

	//设置图层线性
	AcDbLinetypeTable *pLinetTbl;
	AcDbObjectId linetypeId;
	acdbHostApplicationServices()->workingDatabase()->getLinetypeTable(pLinetTbl, AcDb::kForWrite);

	if (pLinetTbl->has(LineTypeName))
	{
		pLinetTbl->getAt(LineTypeName, linetypeId);
		pLayerTblRcd->setLinetypeObjectId(linetypeId);
	}

	pLinetTbl->close();


	AcDbObjectId LayerTblRcdId;
	pLayerTbl->add(LayerTblRcdId, pLayerTblRcd);


	//将新建的层表记录添加到层表中
	pLayerTblRcd->subClose();
	pLayerTblRcd->close();
	pLayerTbl->close();
	return 0;
}

/*
StrName:要设置的图层名称
*/
//将指定图层设为当前图层
int ClassCADFunc::SetNowLayerName(CString StrName)
{
	//获取当前图形层表
	AcDbLayerTable *pLayerTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);

	if (!pLayerTbl->has(StrName))
	{
		//AfxMessageBox(L"!hasname");
		pLayerTbl->close();
		return -1;
	}

	//设置图层
	AcDbObjectId linetypeId;
	pLayerTbl->getAt(StrName, linetypeId);
	//设为当前图层
	acdbHostApplicationServices()->workingDatabase()->setClayer(linetypeId);			
	pLayerTbl->close();
	return 0;
}



//Arx函数操作 图块类****************************************

//插入当前数据库内的块
int ClassCADFunc::InsertBlock( CString BlockName, AcGePoint3d PointCoor,AcDbObjectId& ObjectId )
{
	int Err = -1;
	//访问块表记录
	AcDbBlockTable *pBlockTbl;
	//acdbHostApplicationServices()->workingDatabase()->setCannoscale(AcDbAnnotationScale::getByName(L"1:1"));
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTbl, AcDb::kForRead);


	Err = pBlockTbl->getAt(BlockName, ObjectId);

	pBlockTbl->close();
	if (Err != 0)
	{
		acedAlert(L"获得指定名称的的ID失败！\n");
		return -2;
	}


	AcDbBlockReference *pBlocTblRcd = new AcDbBlockReference(PointCoor, ObjectId);

	ObjectId = AddObjectToModl(pBlocTblRcd);
	return 0;

}

//将指定路径下块插入到当前数据库
int ClassCADFunc::ImportBlock( CString StrPath )
{
	int Err = -1;
	//读取DWG文件获得数据库
	AcDbDatabase pExternalDb(Adesk::kFalse);
	Err = pExternalDb.readDwgFile(StrPath);
	if (Err != 0)
	{
		acedAlert(L"读取DWG文件失败！\n");
		return -1;
	}


	//获得指定名称的块表记录
	AcDbBlockTable* pBlkTbl;
	Err = pExternalDb.getBlockTable(pBlkTbl, AcDb::kForRead);
	if (Err != 0)
	{
		acedAlert(L"获得块表失败！\n");
		return -2;
	}




	//使用遍历器访问每一条层表记录
	AcDbBlockTableIterator *pItr;
	pBlkTbl->newIterator(pItr);

	AcDbObjectId blkRefId;
	AcDbObjectIdArray ListObjects;
	AcDbBlockTableRecord* blkTblRec;


	for (pItr->start(); !pItr->done(); pItr->step())
	{
		pItr->getRecord(blkTblRec, AcDb::kForRead);



		// 创建块参照遍历器
		AcDbBlockReferenceIdIterator *pRefItr;
		Err = blkTblRec->newBlockReferenceIdIterator(pRefItr);
		if (Err != 0)
		{
			acedAlert(L"创建遍历器失败！\n");
			blkTblRec->close();
			return -4;
		}

		for (pRefItr->start(); !pRefItr->done(); pRefItr->step())
		{
			if (pRefItr->getBlockReferenceId(blkRefId) == 0)
			{
				ListObjects.append(blkRefId);
				break;
			}
		}
		blkTblRec->close();
	}
	pBlkTbl->close();


	AcDbDatabase *pTempDb;
	//将list数组中包含的实体输出到一个临时图形数据库中
	Err = pExternalDb.wblock(pTempDb, ListObjects, AcGePoint3d::kOrigin);
	if (Err != 0)
	{
		acedAlert(L"wblock操作失败！\n");
		return -6;
	}


	//将临时数据库的内容插入到当前图形数据库
	//acdbHostApplicationServices()->workingDatabase()->setCannoscale(AcDbAnnotationScale::getByName(L"1:1"));
	Err = acdbHostApplicationServices()->workingDatabase()->insert(AcGeMatrix3d::kIdentity, pTempDb);
	if (Err != 0)
	{
		acedAlert(L"插入图块至当前数据库操作失败！\n");
	}
	delete pTempDb;

	for (int i = 0; i < ListObjects.length(); ++i)
	{
		ads_name lastEnt;
		if (acdbEntLast(lastEnt) != RTNORM)
		{
			//acedAlert(L"获得模型空间最后一个实体失败!");
			return -7;
		}

		AcDbObjectId EntId;
		Err = acdbGetObjectId(EntId, lastEnt);
		DeleteObject(EntId);
	}


	return 0;
}

//Arx函数操作 标注类****************************************

//创建线性标注
AcDbObjectId ClassCADFunc::CreateLinearDim( AcGePoint3d PointA, AcGePoint3d PointB, AcGePoint3d Poin, double Angle, CString DimText /*= L""*/, AcDbObjectId DimStyle /*= NULL*/ )
{
	AcDbRotatedDimension *pDim;
	pDim = new AcDbRotatedDimension(PI / 180.0*Angle, PointA, PointB, Poin, DimText, DimStyle);
	pDim->setTextPosition(Poin);

	return AddObjectToModl(pDim);
}


//创建线性标注/*镜像条件，1为不变，0为镜像*/
AcDbObjectId ClassCADFunc::CreateLinearDim(AcGePoint3d PointA, AcGePoint3d PointB, AcGePoint3d Poin, double Angle, CString DimText,double Image,double AxisNum,int isDelete)
{
	AcDbObjectId ObjectId;
	if(Image)
	{
		//不变
		ObjectId = CreateLinearDim(PointA,PointB,Poin,Angle,DimText);
	}
	else
	{
		if(isDelete)
		{
			AcGePoint3d axisP1 = AcGePoint3d(AxisNum,0,0);
			AcGePoint3d axisP2 = AcGePoint3d(AxisNum,1,0);
			ObjectId = CreateLinearDim(MirrorTransform(PointA,axisP1,axisP2), MirrorTransform(PointB,axisP1,axisP2) , MirrorTransform(Poin,axisP1,axisP2) ,Angle ,DimText);
		}
		else
		{
			ObjectId = CreateLinearDim(PointA,PointB,Poin,Angle,DimText);
			AcGePoint3d axisP1 = AcGePoint3d(AxisNum,0,0);
			AcGePoint3d axisP2 = AcGePoint3d(AxisNum,1,0);
			ObjectId = CreateLinearDim(MirrorTransform(PointA,axisP1,axisP2), MirrorTransform(PointB,axisP1,axisP2) , MirrorTransform(Poin,axisP1,axisP2) ,Angle ,DimText);
		}
	}

	
	return ObjectId;
}

//创建角度标注               /*顶点、                       起始点、              终止点              标注尺寸线*/
AcDbObjectId ClassCADFunc::CreateAngleDim( AcGePoint3d Center, AcGePoint3d PointEnd1, AcGePoint3d PointEnd2, AcGePoint3d PointPlace, CString DimText /*= L""*/, AcDbObjectId DimStyle /*= NULL*/ )
{
	AcDb3PointAngularDimension *pDim = new AcDb3PointAngularDimension(Center, PointEnd1, PointEnd2, PointPlace, DimText, DimStyle);
	pDim->setTextPosition(PointPlace);
	pDim->useDefaultTextPosition();
	return AddObjectToModl(pDim);
}

//创建角度标注             /*镜像条件，1为不变，0为镜像*/
AcDbObjectId ClassCADFunc::CreateAngleDim( AcGePoint3d Center, AcGePoint3d PointEnd1, AcGePoint3d PointEnd2, AcGePoint3d PointPlace, CString DimText,double Image,double AxisNum,int isDelete)
{
	AcDbObjectId DimStyle;
	if(Image)
	{
		//不变
		DimStyle = CreateAngleDim(Center,PointEnd1,PointEnd2,PointPlace,DimText);
	}
	else
	{
		if(isDelete)
		{
			AcGePoint3d axisP1 = AcGePoint3d(AxisNum,0,0);
			AcGePoint3d axisP2 = AcGePoint3d(AxisNum,1,0);
			DimStyle = CreateAngleDim(MirrorTransform(Center,axisP1,axisP2), MirrorTransform(PointEnd1,axisP1,axisP2) , MirrorTransform(PointEnd2,axisP1,axisP2) ,MirrorTransform(PointPlace,axisP1,axisP2) ,DimText);
		}
		else
		{
			DimStyle = CreateAngleDim(Center,PointEnd1,PointEnd2,PointPlace,DimText);
			AcGePoint3d axisP1 = AcGePoint3d(AxisNum,0,0);
			AcGePoint3d axisP2 = AcGePoint3d(AxisNum,1,0);
			DimStyle = CreateAngleDim(MirrorTransform(Center,axisP1,axisP2), MirrorTransform(PointEnd1,axisP1,axisP2) , MirrorTransform(PointEnd2,axisP1,axisP2) ,MirrorTransform(PointPlace,axisP1,axisP2) ,DimText);
		}
	}

	return DimStyle;
}


AcDbObjectId ClassCADFunc::CreateRadiusDim( AcGePoint3d PointA, AcGePoint3d PointB, double LeaderLength /*= 5*/, CString DimText /*= L""*/, AcDbObjectId DimStyle /*= NULL*/ )
{
	AcDbRadialDimension *pDim = new AcDbRadialDimension(PointA, PointB, LeaderLength, DimText, DimStyle);
	return AddObjectToModl(pDim);
}

AcDbObjectId ClassCADFunc::CreateRadiusDim( AcGePoint3d PointA, AcGePoint3d PointB, double LeaderLength, CString DimText,double Image,double AxisNum,int isDelete )
{
	AcDbObjectId DimStyle;
	if(Image)
	{
		//不变
		DimStyle = CreateRadiusDim(PointA,PointB,LeaderLength,DimText);
	}
	else
	{
		if(isDelete)
		{
			AcGePoint3d axisP1 = AcGePoint3d(AxisNum,0,0);
			AcGePoint3d axisP2 = AcGePoint3d(AxisNum,1,0);
			DimStyle = CreateRadiusDim(MirrorTransform(PointA,axisP1,axisP2), MirrorTransform(PointB,axisP1,axisP2) ,LeaderLength,DimText);
		}
		else
		{
			DimStyle = CreateRadiusDim(PointA,PointB,LeaderLength,DimText);
			AcGePoint3d axisP1 = AcGePoint3d(AxisNum,0,0);
			AcGePoint3d axisP2 = AcGePoint3d(AxisNum,1,0);
			DimStyle = CreateRadiusDim(MirrorTransform(PointA,axisP1,axisP2), MirrorTransform(PointB,axisP1,axisP2) ,LeaderLength,DimText);
		}
	}

	return DimStyle;
}

//创建引线	
/*
StartPoint:引线起始点
EndPoint:引线第二点
LeadPoint:引线终止点
StrText:文字内容
TextHeight:文字高度
TextColor:文字颜色
*/
int ClassCADFunc::CreateLeader( AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcGePoint3d LeadPoint, CString StrText, CString StyName, int LeaderColor,  double TextHeight /*= 3.5*/, int TextColor /*= 0*/ )
{
	AcDbObjectId ObjectId;

	AcDbLeader* pLeader = new AcDbLeader();
	pLeader->setColorIndex(LeaderColor);
	pLeader->appendVertex(StartPoint);
	pLeader->appendVertex(EndPoint);

	if (EndPoint.x == LeadPoint.x && EndPoint.y == LeadPoint.y && EndPoint.z == LeadPoint.z)
	{
		ObjectId = AddObjectToModl(pLeader);
	}
	else
	{
		pLeader->appendVertex(LeadPoint);
		ObjectId = AddObjectToModl(pLeader);
	}


	AcGePoint3d PointText;
	double Angle;

	if (EndPoint.x == LeadPoint.x && EndPoint.y == LeadPoint.y && EndPoint.z == LeadPoint.z)
	{
		PointText = AcGePoint3d(LeadPoint.x + 0.5, LeadPoint.y + 0.5, LeadPoint.x);
		Angle = GetLineAndXaxisAngle(EndPoint, LeadPoint, EndPoint);


	}
	else
	{
		if (LeadPoint.x > EndPoint.x)
		{
			PointText = AcGePoint3d(EndPoint.x + 0.5, EndPoint.y + 0.5, EndPoint.x);
			Angle = GetLineAndXaxisAngle(EndPoint, LeadPoint, EndPoint);
		}
		else
		{
			PointText = AcGePoint3d(LeadPoint.x + 0.5, LeadPoint.y + 0.5, LeadPoint.x);
			Angle = GetLineAndXaxisAngle(LeadPoint, EndPoint, LeadPoint);

		}
	}

	if (TextColor == 0)
	{
		TextColor = LeaderColor;
	}

	ObjectId = CreateMText(PointText, StrText, AcDbMText::kBottomLeft, TextColor, GetNameTextStyle(StyName), TextHeight, 0);
	RotateObject(ObjectId, LeadPoint, Angle);

	return 0;

}

//创建引线			/*镜像条件，1为不变，0为镜像*/
int ClassCADFunc::CreateLeader( AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcGePoint3d LeadPoint, CString StrText,double Image,double AxisNum,int isDelete )
{
	int return_num = 0;
	if(Image)
	{
		//不变
		return_num = CreateLeader(StartPoint,EndPoint,LeadPoint,StrText,L"HC",1 ,2.5, 0);
	}
	else
	{
		if(isDelete)
		{
			AcGePoint3d axisP1 = AcGePoint3d(AxisNum,0,0);
			AcGePoint3d axisP2 = AcGePoint3d(AxisNum,1,0);
			return_num = CreateLeader(MirrorTransform(StartPoint,axisP1,axisP2), MirrorTransform(EndPoint,axisP1,axisP2) , MirrorTransform(LeadPoint,axisP1,axisP2),StrText,L"HC",1,2.5, 0);
		}
		else
		{
			return_num = CreateLeader(StartPoint,EndPoint,LeadPoint,StrText,L"HC",1, 2.5, 0);
			AcGePoint3d axisP1 = AcGePoint3d(AxisNum,0,0);
			AcGePoint3d axisP2 = AcGePoint3d(AxisNum,1,0);
			return_num = CreateLeader(MirrorTransform(StartPoint,axisP1,axisP2), MirrorTransform(EndPoint,axisP1,axisP2) , MirrorTransform(LeadPoint,axisP1,axisP2),StrText,L"HC",1, 2.5, 0);
		}
	}

	return return_num;
}

//Arx函数操作 分析类****************************************
/*
PointOne:点一
PointTwo:点二
*/
//测量两坐标点距离
double ClassCADFunc::GetPointDist( AcGePoint3d PointOne, AcGePoint3d PointTwo )
{
	double Value = (PointTwo.x - PointOne.x)*(PointTwo.x - PointOne.x) + (PointTwo.y - PointOne.y)*(PointTwo.y - PointOne.y) + (PointTwo.z - PointOne.z)*(PointTwo.z - PointOne.z);
	return sqrt(Value);
}

//判断两个点是否相等
bool ClassCADFunc::IsPointSame( AcGePoint3d PointOne, AcGePoint3d PointTwo, double Tolerance /*= 0.02*/ )
{
	if (PointOne.x < PointTwo.x + Tolerance && PointOne.x > PointTwo.x - Tolerance && PointOne.y < PointTwo.y + Tolerance && PointOne.y > PointTwo.y - Tolerance && PointOne.z < PointTwo.z + Tolerance && PointOne.z > PointTwo.z - Tolerance)
	{
		return 1;
	}
	return 0;
}

/*
PointOne:直线的点一
PointTwo:直线的点二
Mode:1表角度,2表弧度
*/
//获得直线与X轴的夹角
double ClassCADFunc::GetLineAndXaxisAngle( AcGePoint3d PointOne, AcGePoint3d PointTwo, int Mode /*= 1*/ )
{
	double Value = 0.0;
	if (PointTwo.x == PointOne.x)
	{
		if (Mode == 1)
		{
			return 90.0;
		}
		else
		{
			return PI / 2.0;
		}
	}
	double k = (PointTwo.y - PointOne.y) / (PointTwo.x - PointOne.x);
	if (Mode == 1)
	{
		return 180.0 / PI*atan(k);
	}
	return Value = atan(k);


	//double k1 = 0.0;
	//if (PointTwo.x - PointOne.x == 0)
	//{
	//	if (PointTwo.y<PointOne.y)
	//	{
	//		if (Mode == 1)
	//		{
	//			return 270.0;
	//		}
	//		else
	//		{
	//			return PI / 2.0*3.0;
	//		}
	//	}
	//	else
	//	{
	//		if (Mode == 1)
	//		{
	//			return 90.0;
	//		}
	//		else
	//		{
	//			return PI / 2.0;
	//		}
	//		return 90.0;
	//	}
	//}
	//double k2 = (PointTwo.y - PointOne.y) / (PointTwo.x - PointOne.x);
	//double value = 0.0;
	//if (PointTwo.x >= PointOne.x&&PointTwo.y >= PointOne.y)
	//{
	//	value = 180 / PI*(atan((k2 - k1) / (1 + k1*k2)));
	//}
	//else if (PointTwo.x <= PointOne.x&&PointTwo.y >= PointOne.y)
	//{
	//	value = 180 / PI*(atan((k2 - k1) / (1 + k1*k2)));
	//	value = 180.0 - (0 - value);
	//}
	//else if (PointTwo.x <= PointOne.x&&PointTwo.y <= PointOne.y)
	//{
	//	value = 180 / PI*(atan((k2 - k1) / (1 + k1*k2)));
	//	value = 180.0 + value;
	//}
	//else if (PointTwo.x >= PointOne.x&&PointTwo.y <= PointOne.y)
	//{
	//	value = 180 / PI*(atan((k2 - k1) / (1 + k1*k2)));
	//	value = 360 + value;
	//}
	//if (Mode == 1)
	//{
	//	return value;
	//}
	//else
	//{
	//	return PI / 180 * value;
	//}
	return 0.0;
}

//获得直线与X轴的夹角
double ClassCADFunc::GetLineAndXaxisAngle( AcGePoint3d PointOne, AcGePoint3d PointTwo, AcGePoint3d DatumPoint, int Mode /*= 1*/ )
{
	double k1 = 0.0;
	if (PointTwo.x - PointOne.x == 0)
	{
		if (PointTwo.y<PointOne.y)
		{
			if (Mode == 1)
			{
				return 270.0;
			}
			else
			{
				return PI / 2.0*3.0;
			}
		}
		else
		{
			if (Mode == 1)
			{
				return 90.0;
			}
			else
			{
				return PI / 2.0;
			}
			return 90.0;
		}
	}
	double k2 = (PointTwo.y - PointOne.y) / (PointTwo.x - PointOne.x);
	double value = 0.0;
	if (PointTwo.x >= PointOne.x&&PointTwo.y >= PointOne.y)
	{
		value = 180 / PI*(atan((k2 - k1) / (1 + k1*k2)));
	}
	else if (PointTwo.x <= PointOne.x&&PointTwo.y >= PointOne.y)
	{
		value = 180 / PI*(atan((k2 - k1) / (1 + k1*k2)));
		value = 180.0 - (0 - value);
	}
	else if (PointTwo.x <= PointOne.x&&PointTwo.y <= PointOne.y)
	{
		value = 180 / PI*(atan((k2 - k1) / (1 + k1*k2)));
		value = 180.0 + value;
	}
	else if (PointTwo.x >= PointOne.x&&PointTwo.y <= PointOne.y)
	{
		value = 180 / PI*(atan((k2 - k1) / (1 + k1*k2)));
		value = 360 + value;
	}
	if (Mode == 1)
	{
		return value;
	}
	else
	{
		return PI / 180 * value;
	}


	//double Value = 0.0;
	//if (PointOne == DatumPoint)
	//{
	//	if (PointTwo.x == PointOne.x && PointTwo.y > PointOne.y)
	//	{
	//		Value = 0.5 * PI;
	//	}
	//	else if (PointTwo.x == PointOne.x && PointTwo.y < PointOne.y)
	//	{
	//		Value = 1.5 * PI;
	//	}
	//	else if (PointTwo.x > PointOne.x && PointTwo.y > PointOne.y)
	//	{
	//		Value = GetLineAndXaxisAngle(PointOne, PointTwo, 2);
	//	}
	//	else if (PointTwo.x < PointOne.x && PointTwo.y > PointOne.y)
	//	{
	//		Value = 0.5 * PI - GetLineAndXaxisAngle(PointOne, PointTwo, 2) + 0.5 * PI;
	//	}
	//	else if (PointTwo.x < PointOne.x && PointTwo.y < PointOne.y)
	//	{
	//		Value = GetLineAndXaxisAngle(PointOne, PointTwo, 2) + 1.0 * PI;
	//	}
	//	else if (PointTwo.x > PointOne.x && PointTwo.y < PointOne.y)
	//	{
	//		Value = 0.5 * PI - GetLineAndXaxisAngle(PointOne, PointTwo, 2) + 1.5 * PI;
	//	}
	//	if (Mode == 2)
	//	{
	//		return Value;
	//	}
	//	else
	//	{
	//		return 180.0 / PI*Value;
	//	}
	//}


	//if (PointTwo == DatumPoint)
	//{
	//	if (PointTwo.x == PointOne.x && PointTwo.y > PointOne.y)
	//	{
	//		Value = 1.5 * PI;
	//	}
	//	else if (PointTwo.x == PointOne.x && PointTwo.y < PointOne.y)
	//	{
	//		Value = 0.5 * PI;
	//	}
	//	else if (PointTwo.x > PointOne.x && PointTwo.y > PointOne.y)
	//	{
	//		Value = GetLineAndXaxisAngle(PointOne, PointTwo, 2) + 1.0 * PI;
	//	}
	//	else if (PointTwo.x < PointOne.x && PointTwo.y > PointOne.y)
	//	{
	//		Value = 0.5 * PI - GetLineAndXaxisAngle(PointOne, PointTwo, 2) + 1.5 * PI;
	//	}
	//	else if (PointTwo.x < PointOne.x && PointTwo.y < PointOne.y)
	//	{
	//		Value = GetLineAndXaxisAngle(PointOne, PointTwo, 2);
	//	}
	//	else if (PointTwo.x > PointOne.x && PointTwo.y < PointOne.y)
	//	{
	//		Value = 0.5 * PI - GetLineAndXaxisAngle(PointOne, PointTwo, 2) + 0.5 * PI;
	//	}
	//	if (Mode == 2)
	//	{
	//		return Value;
	//	}
	//	else
	//	{
	//		return 180.0 / PI*Value;
	//	}
	//}

	//return 0.0;
}

/*
point_one:点一
point_two:点二
type:为1时返回角度,为0时候返回弧度
*/
//获得两条相交线的夹角
double ClassCADFunc::GetInterLineAngle( AcGePoint3d PointOver, AcGePoint3d PointOne, AcGePoint3d PointTwo, int Type /*= 1*/ )
{
	double value = 0.0;
	double a, b, c;
	a = GetPointDist(PointOne, PointTwo);
	b = GetPointDist(PointOver, PointOne);
	c = GetPointDist(PointOver, PointTwo);
	value = (b*b + c*c - a*a) / (2 * b*c);
	value = acos(value);
	if (Type == 1)
	{
		return 180.0 / PI * value;
	}
	else
	{
		return value;
	}
	return -1.0;
}


//计算点到直线的垂直投影点
AcGePoint3d ClassCADFunc::GetProjectionPointOnLine( const AcGePoint3d& point, const AcGePoint3d& lineStart, const AcGePoint3d& lineEnd )
{
	AcGeVector3d lineVector = lineEnd - lineStart;  // 计算直线的向量
	AcGeVector3d pointVector = point - lineStart;  // 计算点到直线起点的向量

	double lengthSq = lineVector.dotProduct(lineVector);  // 计算向量的长度的平方
	double projectionParam = pointVector.dotProduct(lineVector) / lengthSq;  // 计算投影参数

	AcGePoint3d projectionPoint = lineStart + projectionParam * lineVector;  // 计算投影点

	return projectionPoint;

}


//检查对象是否是线性标注
bool ClassCADFunc::IsLinearDimension( AcDbObjectId objectId )
{
	// 打开对象并锁定以进行读取操作
	AcDbEntity* pEntity = NULL;
	Acad::ErrorStatus es = acdbOpenObject(pEntity, objectId, AcDb::kForRead);
	if (es != Acad::eOk || pEntity == NULL)
	{
		// 处理打开对象失败的情况
		return false;
	}
	bool isLinearDimension = false;

	// 检查对象的类型是否为线性标注对象
	if (pEntity->isKindOf(AcDbAlignedDimension::desc()) || pEntity->isKindOf(AcDbRotatedDimension::desc()))
	{
		isLinearDimension = true;
	}

	// 关闭对象
	pEntity->close();

	return isLinearDimension;

}

//获取线性标注信息
void ClassCADFunc::GetLinearDimData(AcDbObjectId dimId, AcGePoint3d& startPoint, AcGePoint3d& endPoint, AcGePoint3d& textPoint)
{
	AcDbRotatedDimension* pDim = NULL;
	Acad::ErrorStatus es = acdbOpenObject(pDim, dimId, AcDb::kForRead);
	if (es != Acad::eOk || pDim == NULL)
	{
		// 处理打开对象失败的情况
		return;
	}

	startPoint = pDim->xLine1Point();
	endPoint = pDim->xLine2Point();
	textPoint = pDim->textPosition();
	pDim->close();
}

//基本方法 类型转换类****************************************

//Cstring 2 char*
const char* ClassBasicFunc::CString2Char( CString Str )
{
	int n = Str.GetLength(); //获取str的字符数  
	int len = WideCharToMultiByte(CP_ACP, 0, Str, n, NULL, 0, NULL, NULL); //获取宽字节字符的大小，大小是按字节计算的  
	char *pChar = new char[len + 1]; //以字节为单位
	WideCharToMultiByte(CP_ACP, 0, Str, n, pChar, len, NULL, NULL); //宽字节编码转换成多字节编码  
	pChar[len] = '\0'; //多字节字符以'\0'结束  

	return pChar;
}

//char* 2 Cstring
CString ClassBasicFunc::Char2CString( const char* pChar )
{
	//将char数组转换为wchar_t数组
	int charLen = strlen(pChar); //计算pChar所指向的字符串大小，以字节为单位，一个汉字占两个字节  
	int len = MultiByteToWideChar(CP_ACP, 0, pChar, charLen, NULL, 0); //计算多字节字符的大小，按字符计算  
	wchar_t *pWChar = new wchar_t[len + 1]; //为宽字节字符数申请空间，  
	MultiByteToWideChar(CP_ACP, 0, pChar, charLen, pWChar, len); //多字节编码转换成宽字节编码  
	pWChar[len] = '\0';

	//将wchar_t数组转换为CString
	CString str;
	str.Append(pWChar);
	return str;
}

//double 2 char*
const char* ClassBasicFunc::Double2Char( double d1 )
{
	CString str;
	str.Format(L"%f", d1);
	const char *pchar = CString2Char(str);
	return pchar;
}
//double 2 Cstring
CString ClassBasicFunc::Double2CString( double d1 )
{
	CString strR;
	char buffer[20];
	sprintf_s(buffer,"%f",d1);
	string str = buffer;
	int i = str.find(".");
	int length = str.length();
	length = length - i;
	if(length==0)
		strR.Format(L"%.f",d1);
	else if(length==1)
		strR.Format(L"%.2f",d1);
	else if(length==2)
		strR.Format(L"%.3f",d1);
	else if(length==3)
		strR.Format(L"%.4f",d1);
	return strR;
}



//wstring 2 string
string ClassBasicFunc::wstring2String(const std::wstring& wstr)
{
	unsigned len = wstr.size() * 4;
	setlocale(LC_CTYPE, "");
	char *p = new char[len];
	wcstombs(p,wstr.c_str(),len);
	std::string str1(p);
	delete[] p;
	return str1;
}
//基本方法 数学方法类**************************************************************************
//四舍五入
double ClassBasicFunc::round( double number, int num_digits )
{
	double multiplier = pow(10.0, num_digits);
	return floor(number * multiplier + 0.5) / multiplier;
}

//向上取整
double ClassBasicFunc::roundNumber(double number, int num_digits)
{
	double multiplier = pow(10.0, num_digits);
	return std::ceil(number * multiplier) / multiplier;
}
//向下取整
double ClassBasicFunc::floorNumber(double number, double significance)
{
	double divisor = pow(10, significance);
	return floor(number * divisor) / divisor;
}
//按照倍数向上取整
double ClassBasicFunc::ceilToSignificance( double number, double significance )
{
	double quotient = std::ceil(number / significance);
	return quotient * significance;
}
