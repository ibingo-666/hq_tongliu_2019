#pragma once

#include "BasicExcel.h"
#include "tinyxml.h"
#include<vector>
using namespace std;
using namespace YExcel;

//叶片对数据结构体
struct bladeData
{
	///////////////////////////////////////////////////////公共部分///////////////////////////////////////////////////////
	double p31zf;				//汽流方向，定义数值=1，汽流从左向右，否则标其他数字，非空
	double Margin;				//余量
	double Jmin;				//汽封槽左边到上级叶片的距离 min
	double Hmin;				//汽封槽右边到上级叶片的距离 min
	double P;					//正胀差
	double Q;					//负胀差
	double W;					//稳态胀差 
	double N;					//每个气道的布置方向与推力轴承的关系Nn值，暂时与P31相同
								//当气道1在推力轴承右侧且为反向布置，或气道1在推力轴承左侧且正向布置，定义：N1=0
								//当气道1在推力轴承右侧且为正向布置，或气道1在推力轴承左侧且反向布置，定义：N1=1
	
	//计算
	double ForwardSpace;		//正向间隙 AI
	double NegativeSpace;		//负向间隙 AJ
	double J;					//汽封槽左边到上级叶片的距离
	double H;					//汽封槽右边到上级叶片的距离

	//引入长瞬态胀差（需要强度确认给不给推荐间隙）为P，短瞬态胀差为Q，稳态胀差为W
	//当Nn=0时，定义X=P，Y=Q；
	//当Nn=1时，定义X=Q，Y=P。
	double X;
	double Y;

	double A;
	double B;
	double E;
	double F;
	double Z_R;
	double Z_S;


	double a;					//出气侧槽到围带边的距离
	double b;					//槽宽度
	double c;					//进气侧槽到围带边的距离
	double d;
	double t;					//汽封齿个数//叶顶汽封 动叶
	double t_type;				//汽封齿类型//叶顶汽封//一短一长一短 -- 1    一长一短 -- 2  短齿 -- 3 动叶
	double t_type_s;			//汽封齿类型//隔板汽封//一短一长一短 -- 1    一短一长 -- 2  静叶

	double f;
	double e_min;
	double e;
	double e_max;

	double γ;
	double λ;

	///////////////////////////////////////////////////////折流部分///////////////////////////////////////////////////////
#pragma region
	double BrokenTag;			//折流处标记。1为是0为否
	double BrokenDis;			//折流处定义距离，静叶中心线距气道中心线
#pragma endregion

	///////////////////////////////////////////////////////抽口部分///////////////////////////////////////////////////////
#pragma region
	double IsSuctionPort;       //是否含有抽口，1为有，0为无
	double MidWeight;			//中间轴向宽度
	double InletWeight;			//进气侧轴向宽度，非空
	double OutletWeight;		//出气侧轴向宽度，非空
	double CoverDegree;			//动静叶顶盖度，非空
	double V;					//蒸汽比容V，非空
	double G;					//抽汽流量G，非空
	double μ;					//最大允许流量μ，非空
	string MaxAllowFlow;		//最大允许流量MaxAllowFlow，XML 高压低压中压，无
	double MidY;				//抽口中间轴向宽度Y
#pragma endregion

	///////////////////////////////////////////////////////叶片部分///////////////////////////////////////////////////////
#pragma region
	//叶片1 静叶

	//計算
	double p0Jz;				//原点到第n级静叶中心线的距离//先算p0再算p30   第一個p0 = p0Jz_1 ，不用算//p0 =MAX(AI2+AF2+U2+K3-L3,AI2+AF2+X2+H3-I3)

	double p26Dpp;				//出汽边平均直径Dpp
	double p3bg;				//出汽边bg
	double p1BG;				//轴向宽度根截面BG
	double p27Lp;				//出汽边高度Lp
	double p19;					// p27Lp/2 叶片中心线径向高
	double p2BD;				//顶截面BD
	double p4bd;				//出汽边bd

	double p5Bw;				//围带宽度Bw
	double p6bw;				//出汽边到中心线距离bw
	double p7hw;				//围带高度hw
	double p8Bz;				//中间体宽度Bz
	double p9bz;				//出汽边到中心线距离bz
	double p10hz;				//中间体高度hz
	double p5;					//静叶根左倒角长 //0.3
	double p6;					//静叶根左倒角高 //0.3
	double p3;					//静叶根右倒角长 //0.3
	double p4;					//静叶根右倒角高 //0.3
	double p18;					// p9bz 叶片轴向中心线右半长
	double p17;					// p18  叶片轴向中心线左半长
	double p21;					//叶根中心线冒边高 // 35
	
	//角度
	double p11ad;				//顶部扩张角αd
	double p12ag;				//根部扩张角αg

	//叶片2 动叶

	//計算
	double p30Dz;				//原点到第n级动叶中心线的距离//p30  =MAX(AJ2+C2+L2+T2-U2,AJ2+C2+I2+W2-X2)

	double p28Dpd;				//出汽边平均直径Dpd
	double p15bg;				//出汽边bg
	double p13BG;				//轴向宽度根截面BG
	double p29Ld;				//出汽边高度Ld
	double p32;					// p29Ld/2
	double p14BD;				//顶截面BD
	double p16bd;				//出汽边bd

	double p17Bw;				//围带宽度Bw
	double p18bw;				//出汽边到中心线距离bw
	double p19hw;				//围带高度hw
	double p21Bz;				//中间体宽度Bz
	double p22bz;				//出汽边到中心线距离bz
	double p23hz;				//中间体高度hz
	double p23;					//动叶根左倒角长 //0.3
	double p24;					//动叶根左倒角高 //0.3
	double p25;					//动叶根右倒角长 //0.3
	double p26;					//动叶根右倒角高 //0.3
	double p34;					// p22bz 叶片轴向中心线右半长
	double p35;					// p34  叶片轴向中心线左半长
	double p27;					//叶根中心线冒边高 // 35

	//角度
	double p24ad;				//顶部扩张角αd
	double p25ag;				//根部扩张角αg
#pragma endregion

	///////////////////////////////////////////////////////叶根部分///////////////////////////////////////////////////////
#pragma region
	//静叶//只有唯一类型 反动式预扭T叶根
	string SHubMType;			//叶根类型
	double SHubType;			//叶根型线号
	//叶根部分 
	double Sbn;					//叶根颈部宽度bn
	double Shn;					//叶根颈部高度hn
	double Shx;					//叶根下部高度hx
	double Sbz;					//中间体轴向宽度的一半bz
	double Sdaojiao_c;			//叶根端部出汽边倒角[mm]daojiaoc=
	double Sdaojiao_cb;			//叶根下部出汽边上倒角[mm]daojiaocb=（静叶为叶根上部出汽边下倒角）
	double Sdaojiao_r;			//叶根颈部四周倒圆角[mm]daojiaor=
	double Srcj;				//叶根进出汽侧中间体挖槽半径rcj
	double SHubHorizontalAngle;	//叶根工作面与水平夹角
	//叶根槽部分
	double Shy;					//叶根进汽边沉入转子深度hy
	double SInterstitialsR;		//填隙条半径
	double SGrooveSurfaceH;		//中间体下表面到叶根槽下表面高度
	double SGrooveNeckH;		//叶根槽颈部高度
	double SGrooveBottomH;		//叶根槽下部高度（包含填隙条槽的高度）
	double SMidBodyH;			//叶根槽中间体宽度
	double SGrooveNeckW;		//叶根槽颈部宽度
	double SGrooveNeckChamfer;	//叶根槽颈部倒角
	double SGrooveBottomW;		//叶根槽下部宽度（若两侧不为竖直则填最大处宽度）
	double SGrooveVerticalAngle;//叶根槽下部与竖直夹角
	double SUCornerR;			//叶根槽下部上倒圆（静叶为轮槽上部下倒圆）
	double SDCornerR;			//叶根槽下部下倒圆（静叶为轮槽上部上倒圆）
	AcGePoint3d S_deep_point;	//叶根槽进汽边沉入转子深度点

	//动叶//四种类型
	//公共
	string HubMType;			//叶根类型
	double HubType;				//叶根型线号
	double daojiao_c;			//叶根端部出汽边倒角[mm]daojiaoc=
	double daojiao_cb;			//叶根下部出汽边上倒角[mm]daojiaocb=（静叶为叶根上部出汽边下倒角）
	double hy;					//叶根进汽边沉入转子深度hy
	double bz;					//中间体轴向宽度的一半bz
	double daojiao_r;			//叶根颈部四周倒圆角[mm]daojiaor=
	double rcj;					//叶根进出汽侧中间体挖槽半径rcj
	AcGePoint3d deep_point;		//叶根槽进汽边沉入转子深度点
	AcGePoint3d rator_deeppoint;//动叶汽封槽起始点

	//1/2
	double bn;					//叶根颈部宽度bn
	double hn;					//叶根颈部高度hn
	double hx;					//叶根下部高度hx

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
	double r1;					//r1（叶根）
	double r3;					//r3（叶根）
	double d1;					//d1（叶根）
	double d3;					//d3（叶根）
	//轮槽
	double r2;					//r2（轮槽）
	double r4;					//r4（轮槽）
	double d2;					//d2（轮槽）
	double d4;					//d4（轮槽）
	double lb2;					//lb2
	double lb3;					//lb3
	double lb4;					//lb4
	double lb5;					//lb5

	//反动式预扭T叶根//叶根部分
	double HubHorizontalAngle;	//叶根工作面与水平夹角
	//叶根槽部分
	double InterstitialsR;		//填隙条半径
	double GrooveSurfaceH;		//中间体下表面到叶根槽下表面高度
	double GrooveNeckH;			//叶根槽颈部高度
	double GrooveBottomH;		//叶根槽下部高度（包含填隙条槽的高度）
	double MidBodyH;			//叶根槽中间体宽度
	double GrooveNeckW;			//叶根槽颈部宽度
	double GrooveNeckChamfer;	//叶根槽颈部倒角
	double GrooveBottomW;		//叶根槽下部宽度（若两侧不为竖直则填最大处宽度）
	double GrooveVerticalAngle;	//叶根槽下部与竖直夹角
	double UCornerR;			//叶根槽下部上倒圆（静叶为轮槽上部下倒圆）
	double DCornerR;			//叶根槽下部下倒圆（静叶为轮槽上部上倒圆）


	//冲动式非预扭T叶根//叶根部分
	double MidBodySurW;			//中间体下表面去掉小脚宽度
	double HubDW;				//叶根下部宽度
	double TSFH;				//T叶根小脚高度
	double TUestW;				//T叶根颈部上端最大宽度
	double TZAngle;				//T叶根颈部倾斜边与Z轴夹角
	double NeckDCornerR;		//叶根颈部下倒圆
	double NeckUCornerR;		//叶根颈部上倒圆
	double NeckMCornerR;		//叶根颈部中倒圆
	double TAngle;				//T叶根工作面与水平夹角
	double TSFMidCornerR;		//T叶根小脚与中间体相接处倒圆 
	double SFInAngle;			//T叶根小脚内侧倒角
	//轮槽部分
	double NeckH;				//轮槽颈部高度
	double SFH;					//轮槽小脚高度
	double NeckDH;				//轮槽下部高度
	double NeckW;				//轮槽颈部宽度
	double NeckDW;				//轮槽下部宽度
	double NeckUestW;			//轮槽颈部上端最大宽度
	double NeckZAngle;			//轮槽颈部倾斜边与Z轴夹角
	double NeckUAngle;			//轮槽颈部上倒角（颈部有斜边时为倒圆）
	double NeckDAngle;			//轮槽颈部下倒角
	double RaceNeckMCornerR;	//轮槽颈部中倒圆
	double RaceUCornerR;		//轮槽下部上倒圆
	double RaceDCornerR;		//轮槽下部下倒圆
	double RaceSFAngle;			//轮槽小脚外侧倒角
	double RaceSFCornerR;		//轮槽小脚外侧倒圆

	//非预扭双倒T叶根//叶根部分
	double h;					//小脚高度h
	double HubSFTol;			//叶根出汽侧小脚宽度公差
	double NeckDisTol;			//叶根颈部到出汽侧距离公差

	//预扭双倒T叶根//叶根部分
	double HubLength;			//叶根端部长度（去除倒角）
	double hr;					//叶根高度hr


#pragma endregion

	///////////////////////////////////////////////////////围带部分///////////////////////////////////////////////////////
#pragma region
	//静叶
	double Sc_deep;				//槽深度常数尺寸

	//动叶
	double β;					//进气侧倾角
	double p32d;				//每级动叶为了修正离心力及重心对围带挖槽的宽度
	double c_deep;				//槽深度常数尺寸
#pragma endregion

	bladeData() : p31zf(-9999.0), Margin(-9999.0),Jmin(-9999.0), Hmin(-9999.0),P(-9999.0), Q(-9999.0),W(-9999.0), N(-9999.0),ForwardSpace(-9999.0), NegativeSpace(-9999.0),J(-9999.0), H(-9999.0),
		X(-9999.0), Y(-9999.0), A(-9999.0),B(-9999.0), E(-9999.0),F(-9999.0), Z_R(-9999.0),Z_S(-9999.0), a(-9999.0),b(-9999.0), c(-9999.0), d(-9999.0),t(-9999.0), t_type(-9999.0),
		t_type_s(-9999.0), f(-9999.0),e_min(-9999.0), e(-9999.0),e_max(-9999.0), γ(-9999.0),λ(-9999.0), BrokenTag(-9999.0),BrokenDis(-9999.0), IsSuctionPort(-9999.0),MidWeight(-9999.0), InletWeight(-9999.0),
		OutletWeight(-9999.0), CoverDegree(-9999.0),V(-9999.0), G(-9999.0),μ(-9999.0), MaxAllowFlow(""),MidY(-9999.0),p0Jz(-9999.0), p26Dpp(-9999.0),p3bg(-9999.0), p1BG(-9999.0),p27Lp(-9999.0), p19(-9999.0),
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
		RaceSFAngle(-9999.0),RaceSFCornerR(-9999.0),h(-9999.0),HubSFTol(-9999.0),NeckDisTol(-9999.0),HubLength(-9999.0),hr(-9999.0),Sc_deep(-9999.0),β(-9999.0),p32d(-9999.0),c_deep(-9999.0){
	}
};

//气缸数据结构体
struct aircylinderData
{
	string AirPassage;			//气道类型
	string LastThreeStages;		//末三级选型
	double p0Jz_1;				//原点到第1级静叶中心线的距离若横置叶片则为第一级动叶中心线
	double isHorizonBlade;		//是否含有横置叶片 1是0否
	double AirAxialDis;			//气道中心线轴向距离
	double AirWeight;			//汽道中心宽度
	double CantileverWidth;		//悬臂宽度

	vector<bladeData> bladePair;//叶片对vec
};
//资源参数
struct Setting
{
	CString ResPath;				//资源路径--块
	AcGePoint3d OriginalPoint;		//通流图原点
};



class ClassTongliu
{
public:
	ClassTongliu();
	virtual~ClassTongliu() {}

public:
	//************cad预定义
	void init_cad();

	//************数据处理

	//读取叶片数据
	int getBeBladeData();

	//判断单元格数据
	bool isCellDouble(BasicExcelCell* cell, double &thNum);

	//判断单元格数据 string
	bool isCellString(BasicExcelCell* cell,string &thString);

	//,分割string并且转为double
	void splitString(std::string input, std::string& first, std::string& second, double& firstValue, double& secondValue);

	//将两个double组成string
	void combineString(double firstValue, double secondValue, std::string& combinedString);

	//预读数据
	bool LoadXMLData();

	//预读数据处理抽空数据
	bool LoadXMLDataDispose();
	
	//判断中间轴向宽度Y
	bool isMidWeightY(vector<bladeData> bladePair,int theNum);

	//导出通流xml
	bool ExportXMLData();
	//叶片对节点数据导出
	bool ExportXMLData(TiXmlElement* pElem, bladeData tmp);
	//叶根节点数据导出
	bool ExportXMLData(TiXmlElement* pElem, int HubType, bladeData tmp);
	//围带节点数据导出
	bool ExportShroudXMLData(TiXmlElement* pElem, int HubType, bladeData tmp);


	//************图纸创建

	//创建通流图
	int createTL();

	//创建叶根          /*叶片类型，      叶根类型,        结构体,                     中间体基准点，                             镜像vec*/
	int createHub(string bladeType, double hubtype, bladeData tmp,vector<AcGePoint3d>vertex_out_vec, AcDbObjectIdArray &AircylinderObjects);

	//创建叶根槽					/*叶片类型，    叶根类型,       结构体，	下一级结构体，      中间体左基准点，         中间体右基准点,				镜像vec,				后一叶根槽基准深度点（注：最后一对没有深度点）*/
	int createBladeHubGroove(string bladeType, double hubtype, bladeData tmp,  bladeData next_tmp,  AcGePoint3d basePoint_l, AcGePoint3d basePoint_r,   AcDbObjectIdArray &AircylinderObjects,  AcGePoint3d deepPoint= AcGePoint3d::kOrigin);

	//创建末三级                   /*末三级类型名称，    原点,    是否镜像       镜像轴       是否删除原始          镜像vec*/
	int createLastThreeStages(string TypeName, AcGePoint3d originP,double Image,double AxisNum,int isDelete, AcDbObjectIdArray &AircylinderObjects );
	
	//出图
	bool Export();


public:
	//程序参数
	Setting setting;
private:
	//excel
	BasicExcel              beObject;
	BasicExcelWorksheet*    bewCurrentSheet;
	int                     iCurrentRow;
	int                     iSheetIndex;
	//指标行
	int						rowIndex;

	vector<aircylinderData>aircylinderDataVec;
	aircylinderData aircylinderTmp;//临时
};
