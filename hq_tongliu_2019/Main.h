#pragma once

#include "Func.h"
#include "MainData.h"



//��Ŀ�е���������
class ClassProjectAllData
{
public:
	ClassProjectAllData()
	{
		//�ҵķ�����   ͨ�����Զ��巽����cad����
		MyFunction = new ClassMyFunction();

		//����������
		MainData = new ClassMainData();


	}
	~ClassProjectAllData()
	{

	}


public:
	//�ҵķ�����   ͨ�����Զ��巽����cad����
	ClassMyFunction *MyFunction;	
				
public:
	//���������� ��ͼ
	ClassMainData *MainData;

	
public:
	//ȫ�ֱ���		
	//excel�����ļ�·��
	CString EXCELPath;		
	//xml�ļ�·��
	CString XMLPath;		
	//��Դ·��
	CString ResPath;		



public:

};
	


//����ȫ�ֱ���
extern ClassProjectAllData *Data;								//��Ŀ�е���������
