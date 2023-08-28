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
//CAD���ݿ�***********************************************

//���ʵ�嵽ģ�Ϳռ�
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

//������ǰ���ݿ�������ʵ��ID
int ClassCADFunc::GetAllObjectId( AcDbObjectIdArray *ListObjects )
{
	AcDbBlockTable *pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);		//���ָ�����ָ��
	AcDbBlockTableRecord *pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);								//���ָ��ģ�Ϳռ��ָ��
	//����һ������������
	AcDbBlockTableRecordIterator *pBlkTblRcdItr;
	pBlockTableRecord->newIterator(pBlkTblRcdItr);
	//����ģ�Ϳռ����¼
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




//Arx�������� ͼ����****************************************

//����ֱ��		
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

//����ֱ��
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

//����ֱ�� /*��ʼ�� �������� ���쳤�� ������1Ϊ����0Ϊ���� ֱ��id*/
int ClassCADFunc::CreateLine(AcGePoint3d StartPoint, double angle, double length, int dir, AcDbObjectId *ObjectId )
{
	AcGePoint3d EndPoint;
	if(!dir)//������
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

//��������
int ClassCADFunc::CreateRay(AcGePoint3d StartPoint, AcGeVector3d vec, AcDbObjectId *ObjectId)
{
	AcDbRay *pRay = new AcDbRay();
	pRay->setBasePoint(StartPoint);
	pRay->setUnitDir(vec);
	*ObjectId = AddObjectToModl(pRay);
	
	return 0;
}

/*
Core:Բ��
Radius:�뾶
StartAngle:��ʼ�Ƕ�
EndAngle:��ֹ�Ƕ�
ObjectId:���صĶ���
*/
//����Բ��
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
Core:Բ��
Radius:�뾶
StartPoint:Բ���ĵ�һ��
EndPoint:Բ���ĵڶ���
ObjectId:���صĶ���
*/
//����Բ��
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

//������Բ��
/*
ObjectOneId:Ҫ���ǵĶ���һ
DatumPointA:����һ��׼��
ObjectTwoId:Ҫ���ǵĶ����
DatumPointB:�������׼��
Radius:���ǰ뾶
FilletId:���صĵ��Ƕ���
*/
int ClassCADFunc::CreateFillet( AcDbObjectId& ObjectOneId, AcGePoint3d DatumPointA, AcDbObjectId& ObjectTwoId, AcGePoint3d DatumPointB, double Radius, AcDbObjectId *FilletId )
{
	AcDbEntity *pOffGroupEnt[4];
	int Number = 0;
	AcDbEntity *pEnt;
	//��ʵ�岢ƫ��
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

	//��ʵ�岢ƫ��
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


	//�໥��ý��㲢����Բ
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

	//ɾ��ƫ�ƶ���
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

	//��������
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

	//����Բ��
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

//Arx�������� ������****************************************
//��������
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
	//���Ա�������͸����ʧ��
	//AcCmTransparency* transparency_set = new AcCmTransparency();
	//transparency_set->setAlpha(0);
	//err_type = pMText->setBackgroundTransparency(*transparency_set);
	err_type = pMText->setBackgroundScaleFactor(1);

	return AddObjectToModl(pMText);
}

//Arx�������� ������****************************************
/*
ObjectIdA:����һ
ObjectIdB:�����
*/
//�����������ĵ�һ������
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
//�����������ĵ�һ������
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


//���һ����������
int ClassCADFunc::GetObjectStartPoint(AcDbObjectId ObjectId, AcGePoint3d& StartPoint)
{
	//��ʵ��
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

//���һ��������յ�
int ClassCADFunc::GetObjectEndPoint(AcDbObjectId ObjectId, AcGePoint3d& EndPoint)
{
	//��ʵ��
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

//Trimʵ��
AcDbObjectId ClassCADFunc::TrimObject(AcDbObjectId ObjectId1, AcDbObjectId ObjectId2, AcGePoint3d PointCoor)
{
	//�򿪶���
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


	//תΪ���߶���
	AcDbCurve *pCur1 = AcDbCurve::cast(pEnt1);
	if (pCur1 == NULL)
	{

		pEnt1->close();
		return NULL;
	}


	//��õ������
	AcGePoint3dArray ListPoints(1, 1);
	pEnt1->intersectWith(pEnt2, AcDb::kExtendBoth, ArrayPoints);
	ListPoints.append(ArrayPoints.at(0));
	//CString Str;
	//Str.Format(L"ListPointsSize:%d", ListPoints.length());
	//AfxMessageBox(Str);

	//���
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

	////ɾ��ԭ�ж���
	//if (BolDelete == 1)
	//{
	DeleteObject(ObjectId1);
	//}

	AcDbObjectIdArray ListObjects;
	AcDbObjectId TemObjectId;
	//��ӵ�ģ�Ϳռ�
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

//ɾ������
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

//ɾ������
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

//����ɾ�������б�
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

//ͨ��������ʽ���ƻ��������ʽID
AcDbObjectId ClassCADFunc::GetNameTextStyle( CString StrName )
{
	int Err = -1;

	//���������ʽId
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

//Arx�������� ���������****************************************
/*
ObjectId:Ҫ����Ķ���
PointLineA:����ֱ�ߵ�һ��
PointLineB:����ֱ�ߵڶ���
ImageId:���صľ������
BolDelete:�Ƿ�ɾ��ԭ����
*/
//��������
int ClassCADFunc::CreateImage( AcDbObjectId ObjectId, AcGePoint3d PointLineA, AcGePoint3d PointLineB, AcDbObjectId *ImageId, bool BolDelete /*= 0*/ )
{
	//ԭ�ظ���һ������
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


//ԭ�ظ���һ������
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





//�����
AcGePoint3d ClassCADFunc::MirrorTransform( const AcGePoint3d& point, const AcGePoint3d& mirrorAxisStart, const AcGePoint3d& mirrorAxisEnd )
{
	// ����Գ���ķ�������
	AcGeVector3d axisVector = mirrorAxisEnd - mirrorAxisStart;

	// ������������һ��Ϊ��λ����
	AcGeVector3d axisUnitVector = axisVector.normal();

	// ����ӶԳ�����㵽�����������
	AcGeVector3d axisToPointVector = point - mirrorAxisStart;

	// ��������㵽�Գ����ͶӰ����
	double projectionDistance = axisToPointVector.dotProduct(axisUnitVector);

	// ��������㵽�Գ���Ĵ�ֱ����
	AcGeVector3d perpendicularVector = axisToPointVector - projectionDistance * axisUnitVector;

	// ���㾵����λ��
	AcGePoint3d mirrorPoint = point - 2.0 * perpendicularVector;

	return mirrorPoint;
}

//��ת����(��Z����ת)
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

//Arx�������� ����������****************************************

//��������
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

//Arx�������� ͼ����****************************************

//�½�ͼ��
int ClassCADFunc::CreateLayer( CString StrLayerName, int ColorId, CString LineTypeName )
{
	//�ж��Ƿ��������Ϊ StrLayerName ��ͼ��
	AcDbLayerTable *pLayerTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (pLayerTbl->has(StrLayerName))
	{
		pLayerTbl->close();
		return -1;
	}


	//�½�����¼
	AcDbLayerTableRecord *pLayerTblRcd = new AcDbLayerTableRecord();
	pLayerTblRcd->setName(StrLayerName);

	//����ͼ����ɫ
	AcCmColor color;
	color.setColorIndex(ColorId);
	pLayerTblRcd->setColor(color);

	//����ͼ������
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


	//���½��Ĳ���¼��ӵ������
	pLayerTblRcd->subClose();
	pLayerTblRcd->close();
	pLayerTbl->close();
	return 0;
}

/*
StrName:Ҫ���õ�ͼ������
*/
//��ָ��ͼ����Ϊ��ǰͼ��
int ClassCADFunc::SetNowLayerName(CString StrName)
{
	//��ȡ��ǰͼ�β��
	AcDbLayerTable *pLayerTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);

	if (!pLayerTbl->has(StrName))
	{
		//AfxMessageBox(L"!hasname");
		pLayerTbl->close();
		return -1;
	}

	//����ͼ��
	AcDbObjectId linetypeId;
	pLayerTbl->getAt(StrName, linetypeId);
	//��Ϊ��ǰͼ��
	acdbHostApplicationServices()->workingDatabase()->setClayer(linetypeId);			
	pLayerTbl->close();
	return 0;
}



//Arx�������� ͼ����****************************************

//���뵱ǰ���ݿ��ڵĿ�
int ClassCADFunc::InsertBlock( CString BlockName, AcGePoint3d PointCoor,AcDbObjectId& ObjectId )
{
	int Err = -1;
	//���ʿ���¼
	AcDbBlockTable *pBlockTbl;
	//acdbHostApplicationServices()->workingDatabase()->setCannoscale(AcDbAnnotationScale::getByName(L"1:1"));
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTbl, AcDb::kForRead);


	Err = pBlockTbl->getAt(BlockName, ObjectId);

	pBlockTbl->close();
	if (Err != 0)
	{
		acedAlert(L"���ָ�����Ƶĵ�IDʧ�ܣ�\n");
		return -2;
	}


	AcDbBlockReference *pBlocTblRcd = new AcDbBlockReference(PointCoor, ObjectId);

	ObjectId = AddObjectToModl(pBlocTblRcd);
	return 0;

}

//��ָ��·���¿���뵽��ǰ���ݿ�
int ClassCADFunc::ImportBlock( CString StrPath )
{
	int Err = -1;
	//��ȡDWG�ļ�������ݿ�
	AcDbDatabase pExternalDb(Adesk::kFalse);
	Err = pExternalDb.readDwgFile(StrPath);
	if (Err != 0)
	{
		acedAlert(L"��ȡDWG�ļ�ʧ�ܣ�\n");
		return -1;
	}


	//���ָ�����ƵĿ���¼
	AcDbBlockTable* pBlkTbl;
	Err = pExternalDb.getBlockTable(pBlkTbl, AcDb::kForRead);
	if (Err != 0)
	{
		acedAlert(L"��ÿ��ʧ�ܣ�\n");
		return -2;
	}




	//ʹ�ñ���������ÿһ������¼
	AcDbBlockTableIterator *pItr;
	pBlkTbl->newIterator(pItr);

	AcDbObjectId blkRefId;
	AcDbObjectIdArray ListObjects;
	AcDbBlockTableRecord* blkTblRec;


	for (pItr->start(); !pItr->done(); pItr->step())
	{
		pItr->getRecord(blkTblRec, AcDb::kForRead);



		// ��������ձ�����
		AcDbBlockReferenceIdIterator *pRefItr;
		Err = blkTblRec->newBlockReferenceIdIterator(pRefItr);
		if (Err != 0)
		{
			acedAlert(L"����������ʧ�ܣ�\n");
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
	//��list�����а�����ʵ�������һ����ʱͼ�����ݿ���
	Err = pExternalDb.wblock(pTempDb, ListObjects, AcGePoint3d::kOrigin);
	if (Err != 0)
	{
		acedAlert(L"wblock����ʧ�ܣ�\n");
		return -6;
	}


	//����ʱ���ݿ�����ݲ��뵽��ǰͼ�����ݿ�
	//acdbHostApplicationServices()->workingDatabase()->setCannoscale(AcDbAnnotationScale::getByName(L"1:1"));
	Err = acdbHostApplicationServices()->workingDatabase()->insert(AcGeMatrix3d::kIdentity, pTempDb);
	if (Err != 0)
	{
		acedAlert(L"����ͼ������ǰ���ݿ����ʧ�ܣ�\n");
	}
	delete pTempDb;

	for (int i = 0; i < ListObjects.length(); ++i)
	{
		ads_name lastEnt;
		if (acdbEntLast(lastEnt) != RTNORM)
		{
			//acedAlert(L"���ģ�Ϳռ����һ��ʵ��ʧ��!");
			return -7;
		}

		AcDbObjectId EntId;
		Err = acdbGetObjectId(EntId, lastEnt);
		DeleteObject(EntId);
	}


	return 0;
}

//Arx�������� ��ע��****************************************

//�������Ա�ע
AcDbObjectId ClassCADFunc::CreateLinearDim( AcGePoint3d PointA, AcGePoint3d PointB, AcGePoint3d Poin, double Angle, CString DimText /*= L""*/, AcDbObjectId DimStyle /*= NULL*/ )
{
	AcDbRotatedDimension *pDim;
	pDim = new AcDbRotatedDimension(PI / 180.0*Angle, PointA, PointB, Poin, DimText, DimStyle);
	pDim->setTextPosition(Poin);

	return AddObjectToModl(pDim);
}


//�������Ա�ע/*����������1Ϊ���䣬0Ϊ����*/
AcDbObjectId ClassCADFunc::CreateLinearDim(AcGePoint3d PointA, AcGePoint3d PointB, AcGePoint3d Poin, double Angle, CString DimText,double Image,double AxisNum,int isDelete)
{
	AcDbObjectId ObjectId;
	if(Image)
	{
		//����
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

//�����Ƕȱ�ע               /*���㡢                       ��ʼ�㡢              ��ֹ��              ��ע�ߴ���*/
AcDbObjectId ClassCADFunc::CreateAngleDim( AcGePoint3d Center, AcGePoint3d PointEnd1, AcGePoint3d PointEnd2, AcGePoint3d PointPlace, CString DimText /*= L""*/, AcDbObjectId DimStyle /*= NULL*/ )
{
	AcDb3PointAngularDimension *pDim = new AcDb3PointAngularDimension(Center, PointEnd1, PointEnd2, PointPlace, DimText, DimStyle);
	pDim->setTextPosition(PointPlace);
	pDim->useDefaultTextPosition();
	return AddObjectToModl(pDim);
}

//�����Ƕȱ�ע             /*����������1Ϊ���䣬0Ϊ����*/
AcDbObjectId ClassCADFunc::CreateAngleDim( AcGePoint3d Center, AcGePoint3d PointEnd1, AcGePoint3d PointEnd2, AcGePoint3d PointPlace, CString DimText,double Image,double AxisNum,int isDelete)
{
	AcDbObjectId DimStyle;
	if(Image)
	{
		//����
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
		//����
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

//��������	
/*
StartPoint:������ʼ��
EndPoint:���ߵڶ���
LeadPoint:������ֹ��
StrText:��������
TextHeight:���ָ߶�
TextColor:������ɫ
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

//��������			/*����������1Ϊ���䣬0Ϊ����*/
int ClassCADFunc::CreateLeader( AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcGePoint3d LeadPoint, CString StrText,double Image,double AxisNum,int isDelete )
{
	int return_num = 0;
	if(Image)
	{
		//����
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

//Arx�������� ������****************************************
/*
PointOne:��һ
PointTwo:���
*/
//��������������
double ClassCADFunc::GetPointDist( AcGePoint3d PointOne, AcGePoint3d PointTwo )
{
	double Value = (PointTwo.x - PointOne.x)*(PointTwo.x - PointOne.x) + (PointTwo.y - PointOne.y)*(PointTwo.y - PointOne.y) + (PointTwo.z - PointOne.z)*(PointTwo.z - PointOne.z);
	return sqrt(Value);
}

//�ж��������Ƿ����
bool ClassCADFunc::IsPointSame( AcGePoint3d PointOne, AcGePoint3d PointTwo, double Tolerance /*= 0.02*/ )
{
	if (PointOne.x < PointTwo.x + Tolerance && PointOne.x > PointTwo.x - Tolerance && PointOne.y < PointTwo.y + Tolerance && PointOne.y > PointTwo.y - Tolerance && PointOne.z < PointTwo.z + Tolerance && PointOne.z > PointTwo.z - Tolerance)
	{
		return 1;
	}
	return 0;
}

/*
PointOne:ֱ�ߵĵ�һ
PointTwo:ֱ�ߵĵ��
Mode:1��Ƕ�,2����
*/
//���ֱ����X��ļн�
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

//���ֱ����X��ļн�
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
point_one:��һ
point_two:���
type:Ϊ1ʱ���ؽǶ�,Ϊ0ʱ�򷵻ػ���
*/
//��������ཻ�ߵļн�
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


//����㵽ֱ�ߵĴ�ֱͶӰ��
AcGePoint3d ClassCADFunc::GetProjectionPointOnLine( const AcGePoint3d& point, const AcGePoint3d& lineStart, const AcGePoint3d& lineEnd )
{
	AcGeVector3d lineVector = lineEnd - lineStart;  // ����ֱ�ߵ�����
	AcGeVector3d pointVector = point - lineStart;  // ����㵽ֱ����������

	double lengthSq = lineVector.dotProduct(lineVector);  // ���������ĳ��ȵ�ƽ��
	double projectionParam = pointVector.dotProduct(lineVector) / lengthSq;  // ����ͶӰ����

	AcGePoint3d projectionPoint = lineStart + projectionParam * lineVector;  // ����ͶӰ��

	return projectionPoint;

}


//�������Ƿ������Ա�ע
bool ClassCADFunc::IsLinearDimension( AcDbObjectId objectId )
{
	// �򿪶��������Խ��ж�ȡ����
	AcDbEntity* pEntity = NULL;
	Acad::ErrorStatus es = acdbOpenObject(pEntity, objectId, AcDb::kForRead);
	if (es != Acad::eOk || pEntity == NULL)
	{
		// ����򿪶���ʧ�ܵ����
		return false;
	}
	bool isLinearDimension = false;

	// ������������Ƿ�Ϊ���Ա�ע����
	if (pEntity->isKindOf(AcDbAlignedDimension::desc()) || pEntity->isKindOf(AcDbRotatedDimension::desc()))
	{
		isLinearDimension = true;
	}

	// �رն���
	pEntity->close();

	return isLinearDimension;

}

//��ȡ���Ա�ע��Ϣ
void ClassCADFunc::GetLinearDimData(AcDbObjectId dimId, AcGePoint3d& startPoint, AcGePoint3d& endPoint, AcGePoint3d& textPoint)
{
	AcDbRotatedDimension* pDim = NULL;
	Acad::ErrorStatus es = acdbOpenObject(pDim, dimId, AcDb::kForRead);
	if (es != Acad::eOk || pDim == NULL)
	{
		// ����򿪶���ʧ�ܵ����
		return;
	}

	startPoint = pDim->xLine1Point();
	endPoint = pDim->xLine2Point();
	textPoint = pDim->textPosition();
	pDim->close();
}

//�������� ����ת����****************************************

//Cstring 2 char*
const char* ClassBasicFunc::CString2Char( CString Str )
{
	int n = Str.GetLength(); //��ȡstr���ַ���  
	int len = WideCharToMultiByte(CP_ACP, 0, Str, n, NULL, 0, NULL, NULL); //��ȡ���ֽ��ַ��Ĵ�С����С�ǰ��ֽڼ����  
	char *pChar = new char[len + 1]; //���ֽ�Ϊ��λ
	WideCharToMultiByte(CP_ACP, 0, Str, n, pChar, len, NULL, NULL); //���ֽڱ���ת���ɶ��ֽڱ���  
	pChar[len] = '\0'; //���ֽ��ַ���'\0'����  

	return pChar;
}

//char* 2 Cstring
CString ClassBasicFunc::Char2CString( const char* pChar )
{
	//��char����ת��Ϊwchar_t����
	int charLen = strlen(pChar); //����pChar��ָ����ַ�����С�����ֽ�Ϊ��λ��һ������ռ�����ֽ�  
	int len = MultiByteToWideChar(CP_ACP, 0, pChar, charLen, NULL, 0); //������ֽ��ַ��Ĵ�С�����ַ�����  
	wchar_t *pWChar = new wchar_t[len + 1]; //Ϊ���ֽ��ַ�������ռ䣬  
	MultiByteToWideChar(CP_ACP, 0, pChar, charLen, pWChar, len); //���ֽڱ���ת���ɿ��ֽڱ���  
	pWChar[len] = '\0';

	//��wchar_t����ת��ΪCString
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
//�������� ��ѧ������**************************************************************************
//��������
double ClassBasicFunc::round( double number, int num_digits )
{
	double multiplier = pow(10.0, num_digits);
	return floor(number * multiplier + 0.5) / multiplier;
}

//����ȡ��
double ClassBasicFunc::roundNumber(double number, int num_digits)
{
	double multiplier = pow(10.0, num_digits);
	return std::ceil(number * multiplier) / multiplier;
}
//����ȡ��
double ClassBasicFunc::floorNumber(double number, double significance)
{
	double divisor = pow(10, significance);
	return floor(number * divisor) / divisor;
}
//���ձ�������ȡ��
double ClassBasicFunc::ceilToSignificance( double number, double significance )
{
	double quotient = std::ceil(number / significance);
	return quotient * significance;
}
