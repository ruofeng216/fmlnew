#pragma once

#include <string>
#include <vector>
#include <QDebug>
#include <QString>

typedef std::string STRINGANSI;
typedef std::string STRINGUTF8;
typedef std::string STRINGBASE64;

typedef unsigned long long ULONG64;
typedef unsigned int UINT32;


#define W_RETURN_IF_FAIL(expr)				\
	do {									\
		if (!(expr)) {                      \
			qDebug() << "expr error.";      \
			return;				            \
		}} while(0);

#define W_RETURN_VAL_IF_FAIL(expr, val)		\
	do {									\
		if (!(expr)) {                      \
			qDebug() << "expr error.";      \
			return val;			            \
		}} while(0);

class FuncLog
{
public:
	FuncLog(const QString &func)
	{
		m_func = func;
		qDebug() << QString("-----------%1 in----------").arg(m_func);
	}
	~FuncLog()
	{
		qDebug() << QString("-----------%1 out----------").arg(m_func);
	}
private:
	QString m_func;
};

#define FUNCLOG(func) FuncLog fl(func);

#pragma region ENUMDEF
typedef enum e_ErrType
{
	e_Success = 0, // �ɹ�
	e_NoUser = 1,      // �û�������
	e_ErrPswd = 2,     // �������
	e_Exist = 3,       // �û�����
	e_RegErr = 4,      // ע��ʧ��
	e_ModifyErr = 5,    // �޸�ʧ��
} eERR;

#pragma endregion







/////////////////////////////////////////////////////
//��Ҫ�ֶ�
#pragma region CF
//��������
#define SPOTLAG "SpotLag"
#define TADD0 "TADD0"
#define TADD1 "TADD1"
#define TADD2 "TADD2"

// ��Ϣ���߲�ֵ����
#define ZEROCOUPONCURVE "ZeroCouponCurveInterpolation"
#define Z1 "ZM1"
#define Z2 "ZM2"
#define Z3 "ZM3"

// �������߲�ֵ����
#define DiscountCurve "DiscountCurveInterpolation"
#define D1 "DM1"
#define D2 "DM2"
#define D3 "DM3"

// ��ϢƵ��
#define COUPONFREQUENCY  "CouponFrequency"
// ����
#define QUARTERLY  "quarterly"
// ����
#define YEARLY  "yearly"
// ����
#define MONTHLY  "monthly"
// ������
#define HALFYEAR  "halfyear"

// ����
#define CALENDAR  "Calendar"
// ���м��г�����
#define CFETS  "CFETS"
// �й��Ϻ�����������
#define SHSE  "SHSE"

// ��Ϣ�յ���
#define CONVENTION  "Convention"
// ��������һ��������
#define MODIFIEDFOLLOWING  "Modified Following"
// ��һ��������
#define FOLLOWING  "Following"
// ǰһ��������
#define PRECEDING  "Preceding"

// ��������
#define DAYCOUNT  "DayCount"
// ʵ������/ʵ������
#define ACT_ACT  "Act/Act"
// ʵ������/365
#define ACT_365 "Act/365"
// ʵ������/360
#define ACT_360 "Act/360"

// ϢƱ����
#define COUPONTYPE "CouponType"
// ��ϢƱ
#define ZEROCOUPON "ZeroCoupon"
// �̶�ϢƱ
#define FIXEDCOUPON "FixedCoupon"
// ����ϢƱ
#define FLOATCOUPON "FloatCoupon"

// �ο�����
#define REFERENCEINDEX  "ReferenceIndex"
#define SHIBOR3M "shibor3m"
#define REPO7D "repo7d"
#define DEPO1Y "depo1y"

// �г�����
#define MARKETTYPE "MarketType"
// ���м�����г�
#define IBMONEYMARKET "IBMoneyMarket"
// ���м�ծȯ�г�
#define IBBONDMARKET "IBBondMarket"
// �Ϻ������������г�
#define SHEXMONEYMARKET "SHEXMoneyMarket"
// �Ϻ���Ʊ�������г�
#define SHEXSTOCKMARKET "SHEXStockMarket"

// ��Ϣ���ʸ�������
#define ZERORATECOMPOUNDFREQUENCY "ZeroRateCompoundFrequency"
// ��������
#define CONTINUOUSCOMPOUND "ContinuousCompound"
// �긴��
#define YEARLYCOMPOUND "yearlyCompound"

#pragma endregion




/////////////////////////////////////////////////////
#pragma region WNDID
// ����id
const QString Main_HomePage = QString("Main_HomePage"); // home
const QString ParaMgr_Holiday = QString("ParaMgr_Holiday"); // ��������
const QString ParaMgr_Union = QString("ParaMgr_Union"); // ��Ϲ���

#pragma endregion

/////////////////////////////////////////////////////////
#pragma region CONST
#define YMD "yyyy-MM-dd"
#pragma endregion
