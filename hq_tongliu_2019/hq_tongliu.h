#pragma once

#include "BasicExcel.h"
#include "tinyxml.h"
#include<vector>
using namespace std;
using namespace YExcel;

//ҶƬ�����ݽṹ��
struct bladeData
{
	///////////////////////////////////////////////////////��������///////////////////////////////////////////////////////
	double p31zf;				//�������򣬶�����ֵ=1�������������ң�������������֣��ǿ�
	double Margin;				//����
	double Jmin;				//�������ߵ��ϼ�ҶƬ�ľ��� min
	double Hmin;				//������ұߵ��ϼ�ҶƬ�ľ��� min
	double P;					//���Ͳ�
	double Q;					//���Ͳ�
	double W;					//��̬�Ͳ� 
	double N;					//ÿ�������Ĳ��÷�����������еĹ�ϵNnֵ����ʱ��P31��ͬ
								//������1����������Ҳ���Ϊ�����ã�������1�������������������ã����壺N1=0
								//������1����������Ҳ���Ϊ�����ã�������1�������������ҷ����ã����壺N1=1
	
	//����
	double ForwardSpace;		//�����϶ AI
	double NegativeSpace;		//�����϶ AJ
	double J;					//�������ߵ��ϼ�ҶƬ�ľ���
	double H;					//������ұߵ��ϼ�ҶƬ�ľ���

	//���볤˲̬�Ͳ��Ҫǿ��ȷ�ϸ������Ƽ���϶��ΪP����˲̬�Ͳ�ΪQ����̬�Ͳ�ΪW
	//��Nn=0ʱ������X=P��Y=Q��
	//��Nn=1ʱ������X=Q��Y=P��
	double X;
	double Y;

	double A;
	double B;
	double E;
	double F;
	double Z_R;
	double Z_S;


	double a;					//������۵�Χ���ߵľ���
	double b;					//�ۿ��
	double c;					//������۵�Χ���ߵľ���
	double d;
	double t;					//����ݸ���//Ҷ������ ��Ҷ
	double t_type;				//���������//Ҷ������//һ��һ��һ�� -- 1    һ��һ�� -- 2  �̳� -- 3 ��Ҷ
	double t_type_s;			//���������//��������//һ��һ��һ�� -- 1    һ��һ�� -- 2  ��Ҷ

	double f;
	double e_min;
	double e;
	double e_max;

	double ��;
	double ��;

	///////////////////////////////////////////////////////��������///////////////////////////////////////////////////////
#pragma region
	double BrokenTag;			//��������ǡ�1Ϊ��0Ϊ��
	double BrokenDis;			//������������룬��Ҷ�����߾�����������
#pragma endregion

	///////////////////////////////////////////////////////��ڲ���///////////////////////////////////////////////////////
#pragma region
	double IsSuctionPort;       //�Ƿ��г�ڣ�1Ϊ�У�0Ϊ��
	double MidWeight;			//�м�������
	double InletWeight;			//�����������ȣ��ǿ�
	double OutletWeight;		//�����������ȣ��ǿ�
	double CoverDegree;			//����Ҷ���Ƕȣ��ǿ�
	double V;					//��������V���ǿ�
	double G;					//��������G���ǿ�
	double ��;					//������������̣��ǿ�
	string MaxAllowFlow;		//�����������MaxAllowFlow��XML ��ѹ��ѹ��ѹ����
	double MidY;				//����м�������Y
#pragma endregion

	///////////////////////////////////////////////////////ҶƬ����///////////////////////////////////////////////////////
#pragma region
	//ҶƬ1 ��Ҷ

	//Ӌ��
	double p0Jz;				//ԭ�㵽��n����Ҷ�����ߵľ���//����p0����p30   ��һ��p0 = p0Jz_1 ��������//p0 =MAX(AI2+AF2+U2+K3-L3,AI2+AF2+X2+H3-I3)

	double p26Dpp;				//������ƽ��ֱ��Dpp
	double p3bg;				//������bg
	double p1BG;				//�����ȸ�����BG
	double p27Lp;				//�����߸߶�Lp
	double p19;					// p27Lp/2 ҶƬ�����߾����
	double p2BD;				//������BD
	double p4bd;				//������bd

	double p5Bw;				//Χ�����Bw
	double p6bw;				//�����ߵ������߾���bw
	double p7hw;				//Χ���߶�hw
	double p8Bz;				//�м�����Bz
	double p9bz;				//�����ߵ������߾���bz
	double p10hz;				//�м���߶�hz
	double p5;					//��Ҷ���󵹽ǳ� //0.3
	double p6;					//��Ҷ���󵹽Ǹ� //0.3
	double p3;					//��Ҷ���ҵ��ǳ� //0.3
	double p4;					//��Ҷ���ҵ��Ǹ� //0.3
	double p18;					// p9bz ҶƬ�����������Ұ볤
	double p17;					// p18  ҶƬ������������볤
	double p21;					//Ҷ��������ð�߸� // 35
	
	//�Ƕ�
	double p11ad;				//�������ŽǦ�d
	double p12ag;				//�������ŽǦ�g

	//ҶƬ2 ��Ҷ

	//Ӌ��
	double p30Dz;				//ԭ�㵽��n����Ҷ�����ߵľ���//p30  =MAX(AJ2+C2+L2+T2-U2,AJ2+C2+I2+W2-X2)

	double p28Dpd;				//������ƽ��ֱ��Dpd
	double p15bg;				//������bg
	double p13BG;				//�����ȸ�����BG
	double p29Ld;				//�����߸߶�Ld
	double p32;					// p29Ld/2
	double p14BD;				//������BD
	double p16bd;				//������bd

	double p17Bw;				//Χ�����Bw
	double p18bw;				//�����ߵ������߾���bw
	double p19hw;				//Χ���߶�hw
	double p21Bz;				//�м�����Bz
	double p22bz;				//�����ߵ������߾���bz
	double p23hz;				//�м���߶�hz
	double p23;					//��Ҷ���󵹽ǳ� //0.3
	double p24;					//��Ҷ���󵹽Ǹ� //0.3
	double p25;					//��Ҷ���ҵ��ǳ� //0.3
	double p26;					//��Ҷ���ҵ��Ǹ� //0.3
	double p34;					// p22bz ҶƬ�����������Ұ볤
	double p35;					// p34  ҶƬ������������볤
	double p27;					//Ҷ��������ð�߸� // 35

	//�Ƕ�
	double p24ad;				//�������ŽǦ�d
	double p25ag;				//�������ŽǦ�g
#pragma endregion

	///////////////////////////////////////////////////////Ҷ������///////////////////////////////////////////////////////
#pragma region
	//��Ҷ//ֻ��Ψһ���� ����ʽԤŤTҶ��
	string SHubMType;			//Ҷ������
	double SHubType;			//Ҷ�����ߺ�
	//Ҷ������ 
	double Sbn;					//Ҷ���������bn
	double Shn;					//Ҷ�������߶�hn
	double Shx;					//Ҷ���²��߶�hx
	double Sbz;					//�м��������ȵ�һ��bz
	double Sdaojiao_c;			//Ҷ���˲������ߵ���[mm]daojiaoc=
	double Sdaojiao_cb;			//Ҷ���²��������ϵ���[mm]daojiaocb=����ҶΪҶ���ϲ��������µ��ǣ�
	double Sdaojiao_r;			//Ҷ���������ܵ�Բ��[mm]daojiaor=
	double Srcj;				//Ҷ�����������м����ڲ۰뾶rcj
	double SHubHorizontalAngle;	//Ҷ����������ˮƽ�н�
	//Ҷ���۲���
	double Shy;					//Ҷ�������߳���ת�����hy
	double SInterstitialsR;		//��϶���뾶
	double SGrooveSurfaceH;		//�м����±��浽Ҷ�����±���߶�
	double SGrooveNeckH;		//Ҷ���۾����߶�
	double SGrooveBottomH;		//Ҷ�����²��߶ȣ�������϶���۵ĸ߶ȣ�
	double SMidBodyH;			//Ҷ�����м�����
	double SGrooveNeckW;		//Ҷ���۾������
	double SGrooveNeckChamfer;	//Ҷ���۾�������
	double SGrooveBottomW;		//Ҷ�����²���ȣ������಻Ϊ��ֱ������󴦿�ȣ�
	double SGrooveVerticalAngle;//Ҷ�����²�����ֱ�н�
	double SUCornerR;			//Ҷ�����²��ϵ�Բ����ҶΪ�ֲ��ϲ��µ�Բ��
	double SDCornerR;			//Ҷ�����²��µ�Բ����ҶΪ�ֲ��ϲ��ϵ�Բ��
	AcGePoint3d S_deep_point;	//Ҷ���۽����߳���ת����ȵ�

	//��Ҷ//��������
	//����
	string HubMType;			//Ҷ������
	double HubType;				//Ҷ�����ߺ�
	double daojiao_c;			//Ҷ���˲������ߵ���[mm]daojiaoc=
	double daojiao_cb;			//Ҷ���²��������ϵ���[mm]daojiaocb=����ҶΪҶ���ϲ��������µ��ǣ�
	double hy;					//Ҷ�������߳���ת�����hy
	double bz;					//�м��������ȵ�һ��bz
	double daojiao_r;			//Ҷ���������ܵ�Բ��[mm]daojiaor=
	double rcj;					//Ҷ�����������м����ڲ۰뾶rcj
	AcGePoint3d deep_point;		//Ҷ���۽����߳���ת����ȵ�
	AcGePoint3d rator_deeppoint;//��Ҷ�������ʼ��

	//1/2
	double bn;					//Ҷ���������bn
	double hn;					//Ҷ�������߶�hn
	double hx;					//Ҷ���²��߶�hx

	//3/4
	double b1;					//b1
	double b2;					//b2
	double b3;					//b3
	double b4;					//b4
	double b5;					//b5
	double b6;					//b6
	double h1;					//h1
	double h2;					//h2
	double h3;					//h3
	double h4;					//h4
	double h5;					//h5
	double h6;					//h6
	double h7;					//h7
	double r1;					//r1��Ҷ����
	double r3;					//r3��Ҷ����
	double d1;					//d1��Ҷ����
	double d3;					//d3��Ҷ����
	//�ֲ�
	double r2;					//r2���ֲۣ�
	double r4;					//r4���ֲۣ�
	double d2;					//d2���ֲۣ�
	double d4;					//d4���ֲۣ�
	double lb2;					//lb2
	double lb3;					//lb3
	double lb4;					//lb4
	double lb5;					//lb5

	//����ʽԤŤTҶ��//Ҷ������
	double HubHorizontalAngle;	//Ҷ����������ˮƽ�н�
	//Ҷ���۲���
	double InterstitialsR;		//��϶���뾶
	double GrooveSurfaceH;		//�м����±��浽Ҷ�����±���߶�
	double GrooveNeckH;			//Ҷ���۾����߶�
	double GrooveBottomH;		//Ҷ�����²��߶ȣ�������϶���۵ĸ߶ȣ�
	double MidBodyH;			//Ҷ�����м�����
	double GrooveNeckW;			//Ҷ���۾������
	double GrooveNeckChamfer;	//Ҷ���۾�������
	double GrooveBottomW;		//Ҷ�����²���ȣ������಻Ϊ��ֱ������󴦿�ȣ�
	double GrooveVerticalAngle;	//Ҷ�����²�����ֱ�н�
	double UCornerR;			//Ҷ�����²��ϵ�Բ����ҶΪ�ֲ��ϲ��µ�Բ��
	double DCornerR;			//Ҷ�����²��µ�Բ����ҶΪ�ֲ��ϲ��ϵ�Բ��


	//�嶯ʽ��ԤŤTҶ��//Ҷ������
	double MidBodySurW;			//�м����±���ȥ��С�ſ��
	double HubDW;				//Ҷ���²����
	double TSFH;				//TҶ��С�Ÿ߶�
	double TUestW;				//TҶ�������϶������
	double TZAngle;				//TҶ��������б����Z��н�
	double NeckDCornerR;		//Ҷ�������µ�Բ
	double NeckUCornerR;		//Ҷ�������ϵ�Բ
	double NeckMCornerR;		//Ҷ�������е�Բ
	double TAngle;				//TҶ����������ˮƽ�н�
	double TSFMidCornerR;		//TҶ��С�����м�����Ӵ���Բ 
	double SFInAngle;			//TҶ��С���ڲ൹��
	//�ֲ۲���
	double NeckH;				//�ֲ۾����߶�
	double SFH;					//�ֲ�С�Ÿ߶�
	double NeckDH;				//�ֲ��²��߶�
	double NeckW;				//�ֲ۾������
	double NeckDW;				//�ֲ��²����
	double NeckUestW;			//�ֲ۾����϶������
	double NeckZAngle;			//�ֲ۾�����б����Z��н�
	double NeckUAngle;			//�ֲ۾����ϵ��ǣ�������б��ʱΪ��Բ��
	double NeckDAngle;			//�ֲ۾����µ���
	double RaceNeckMCornerR;	//�ֲ۾����е�Բ
	double RaceUCornerR;		//�ֲ��²��ϵ�Բ
	double RaceDCornerR;		//�ֲ��²��µ�Բ
	double RaceSFAngle;			//�ֲ�С����൹��
	double RaceSFCornerR;		//�ֲ�С����൹Բ

	//��ԤŤ˫��TҶ��//Ҷ������
	double h;					//С�Ÿ߶�h
	double HubSFTol;			//Ҷ��������С�ſ�ȹ���
	double NeckDisTol;			//Ҷ����������������빫��

	//ԤŤ˫��TҶ��//Ҷ������
	double HubLength;			//Ҷ���˲����ȣ�ȥ�����ǣ�
	double hr;					//Ҷ���߶�hr


#pragma endregion

	///////////////////////////////////////////////////////Χ������///////////////////////////////////////////////////////
#pragma region
	//��Ҷ
	double Sc_deep;				//����ȳ����ߴ�

	//��Ҷ
	double ��;					//���������
	double p32d;				//ÿ����ҶΪ�����������������Ķ�Χ���ڲ۵Ŀ��
	double c_deep;				//����ȳ����ߴ�
#pragma endregion

	bladeData() : p31zf(-9999.0), Margin(-9999.0),Jmin(-9999.0), Hmin(-9999.0),P(-9999.0), Q(-9999.0),W(-9999.0), N(-9999.0),ForwardSpace(-9999.0), NegativeSpace(-9999.0),J(-9999.0), H(-9999.0),
		X(-9999.0), Y(-9999.0), A(-9999.0),B(-9999.0), E(-9999.0),F(-9999.0), Z_R(-9999.0),Z_S(-9999.0), a(-9999.0),b(-9999.0), c(-9999.0), d(-9999.0),t(-9999.0), t_type(-9999.0),
		t_type_s(-9999.0), f(-9999.0),e_min(-9999.0), e(-9999.0),e_max(-9999.0), ��(-9999.0),��(-9999.0), BrokenTag(-9999.0),BrokenDis(-9999.0), IsSuctionPort(-9999.0),MidWeight(-9999.0), InletWeight(-9999.0),
		OutletWeight(-9999.0), CoverDegree(-9999.0),V(-9999.0), G(-9999.0),��(-9999.0), MaxAllowFlow(""),MidY(-9999.0),p0Jz(-9999.0), p26Dpp(-9999.0),p3bg(-9999.0), p1BG(-9999.0),p27Lp(-9999.0), p19(-9999.0),
		p2BD(-9999.0), p4bd(-9999.0),p5Bw(-9999.0), p6bw(-9999.0),p7hw(-9999.0), p8Bz(-9999.0),p9bz(-9999.0), p10hz(-9999.0),p5(-9999.0), p6(-9999.0),p3(-9999.0), p4(-9999.0),
		p18(-9999.0), p17(-9999.0),p21(-9999.0), p11ad(-9999.0),p12ag(-9999.0), p30Dz(-9999.0),p28Dpd(-9999.0), p15bg(-9999.0),p13BG(-9999.0), p29Ld(-9999.0),p32(-9999.0), p14BD(-9999.0),
		p16bd(-9999.0), p17Bw(-9999.0),p18bw(-9999.0), p19hw(-9999.0),p21Bz(-9999.0), p22bz(-9999.0),p23hz(-9999.0), p23(-9999.0),p24(-9999.0), p25(-9999.0),p26(-9999.0), p34(-9999.0),
		p35(-9999.0), p27(-9999.0),p24ad(-9999.0), p25ag(-9999.0),SHubMType(""), SHubType(-9999.0),Sbn(-9999.0), Shn(-9999.0),Shx(-9999.0), Sbz(-9999.0),Sdaojiao_c(-9999.0), Sdaojiao_cb(-9999.0),
		Sdaojiao_r(-9999.0), Srcj(-9999.0),SHubHorizontalAngle(-9999.0), Shy(-9999.0),SInterstitialsR(-9999.0), SGrooveSurfaceH(-9999.0),SGrooveNeckH(-9999.0), SGrooveBottomH(-9999.0),SMidBodyH(-9999.0),
		SGrooveNeckW(-9999.0),SGrooveNeckChamfer(-9999.0), SGrooveBottomW(-9999.0),SGrooveVerticalAngle(-9999.0), SUCornerR(-9999.0), SDCornerR(),HubMType(""), HubType(-9999.0),daojiao_c(-9999.0), daojiao_cb(-9999.0),hy(-9999.0), 
		bz(-9999.0),daojiao_r(-9999.0), rcj(-9999.0),deep_point(),rator_deeppoint(),bn(-9999.0),hn(-9999.0),hx(-9999.0),b1(-9999.0),b2(-9999.0),b3(-9999.0),b4(-9999.0),b5(-9999.0),b6(-9999.0),
		h1(-9999.0),h2(-9999.0),h3(-9999.0),h4(-9999.0),h5(-9999.0),h6(-9999.0),h7(-9999.0),r1(-9999.0),r3(-9999.0),d1(-9999.0),d3(-9999.0),r2(-9999.0),r4(-9999.0),d2(-9999.0),d4(-9999.0),lb2(-9999.0),lb3(-9999.0),lb4(-9999.0),
		lb5(-9999.0),HubHorizontalAngle(-9999.0),InterstitialsR(-9999.0),GrooveSurfaceH(-9999.0),GrooveNeckH(-9999.0),GrooveBottomH(-9999.0),MidBodyH(-9999.0),GrooveNeckW(-9999.0),GrooveNeckChamfer(-9999.0),GrooveBottomW(-9999.0),
		GrooveVerticalAngle(-9999.0),UCornerR(-9999.0),DCornerR(-9999.0),MidBodySurW(-9999.0),HubDW(-9999.0),TSFH(-9999.0),TUestW(-9999.0),TZAngle(-9999.0),
		NeckDCornerR(-9999.0),NeckUCornerR(-9999.0),NeckMCornerR(-9999.0),TAngle(-9999.0),TSFMidCornerR(-9999.0),SFInAngle(-9999.0),NeckH(-9999.0),SFH(-9999.0),NeckDH(-9999.0),NeckW(-9999.0),NeckDW(-9999.0),NeckUestW(-9999.0),
		NeckZAngle(-9999.0),NeckUAngle(-9999.0),NeckDAngle(-9999.0),RaceNeckMCornerR(-9999.0),RaceUCornerR(-9999.0),RaceDCornerR(-9999.0),
		RaceSFAngle(-9999.0),RaceSFCornerR(-9999.0),h(-9999.0),HubSFTol(-9999.0),NeckDisTol(-9999.0),HubLength(-9999.0),hr(-9999.0),Sc_deep(-9999.0),��(-9999.0),p32d(-9999.0),c_deep(-9999.0){
	}
};

//�������ݽṹ��
struct aircylinderData
{
	string AirPassage;			//��������
	string LastThreeStages;		//ĩ����ѡ��
	double p0Jz_1;				//ԭ�㵽��1����Ҷ�����ߵľ���������ҶƬ��Ϊ��һ����Ҷ������
	double isHorizonBlade;		//�Ƿ��к���ҶƬ 1��0��
	double AirAxialDis;			//�����������������
	double AirWeight;			//�������Ŀ��
	double CantileverWidth;		//���ۿ��

	vector<bladeData> bladePair;//ҶƬ��vec
};
//��Դ����
struct Setting
{
	CString ResPath;				//��Դ·��--��
	AcGePoint3d OriginalPoint;		//ͨ��ͼԭ��
};



class ClassTongliu
{
public:
	ClassTongliu();
	virtual~ClassTongliu() {}

public:
	//************cadԤ����
	void init_cad();

	//************���ݴ���

	//��ȡҶƬ����
	int getBeBladeData();

	//�жϵ�Ԫ������
	bool isCellDouble(BasicExcelCell* cell, double &thNum);

	//�жϵ�Ԫ������ string
	bool isCellString(BasicExcelCell* cell,string &thString);

	//,�ָ�string����תΪdouble
	void splitString(std::string input, std::string& first, std::string& second, double& firstValue, double& secondValue);

	//������double���string
	void combineString(double firstValue, double secondValue, std::string& combinedString);

	//Ԥ������
	bool LoadXMLData();

	//Ԥ�����ݴ���������
	bool LoadXMLDataDispose();
	
	//�ж��м�������Y
	bool isMidWeightY(vector<bladeData> bladePair,int theNum);

	//����ͨ��xml
	bool ExportXMLData();
	//ҶƬ�Խڵ����ݵ���
	bool ExportXMLData(TiXmlElement* pElem, bladeData tmp);
	//Ҷ���ڵ����ݵ���
	bool ExportXMLData(TiXmlElement* pElem, int HubType, bladeData tmp);
	//Χ���ڵ����ݵ���
	bool ExportShroudXMLData(TiXmlElement* pElem, int HubType, bladeData tmp);


	//************ͼֽ����

	//����ͨ��ͼ
	int createTL();

	//����Ҷ��          /*ҶƬ���ͣ�      Ҷ������,        �ṹ��,                     �м����׼�㣬                             ����vec*/
	int createHub(string bladeType, double hubtype, bladeData tmp,vector<AcGePoint3d>vertex_out_vec, AcDbObjectIdArray &AircylinderObjects);

	//����Ҷ����					/*ҶƬ���ͣ�    Ҷ������,       �ṹ�壬	��һ���ṹ�壬      �м������׼�㣬         �м����һ�׼��,				����vec,				��һҶ���ۻ�׼��ȵ㣨ע�����һ��û����ȵ㣩*/
	int createBladeHubGroove(string bladeType, double hubtype, bladeData tmp,  bladeData next_tmp,  AcGePoint3d basePoint_l, AcGePoint3d basePoint_r,   AcDbObjectIdArray &AircylinderObjects,  AcGePoint3d deepPoint= AcGePoint3d::kOrigin);

	//����ĩ����                   /*ĩ�����������ƣ�    ԭ��,    �Ƿ���       ������       �Ƿ�ɾ��ԭʼ          ����vec*/
	int createLastThreeStages(string TypeName, AcGePoint3d originP,double Image,double AxisNum,int isDelete, AcDbObjectIdArray &AircylinderObjects );
	
	//��ͼ
	bool Export();


public:
	//�������
	Setting setting;
private:
	//excel
	BasicExcel              beObject;
	BasicExcelWorksheet*    bewCurrentSheet;
	int                     iCurrentRow;
	int                     iSheetIndex;
	//ָ����
	int						rowIndex;

	vector<aircylinderData>aircylinderDataVec;
	aircylinderData aircylinderTmp;//��ʱ
};
