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
// ��ϢƵ��
const QString CouponFrequency = QString("CouponFrequency");
// ����
const QString quarterly = QString("quarterly");
// ����
const QString yearly = QString("yearly");
// ����
const QString monthly = QString("monthly");
// ������
const QString halfyear = QString("halfyear");

// ����
const QString Calendar = QString("Calendar");
// ���м��г�����
const QString CFETS = QString("CFETS");
// �й��Ϻ�����������
const QString SHSE = QString("SHSE");

// ��Ϣ�յ���
const QString Convention = QString("Convention");
// ��������һ��������
const QString ModifiedFollowing = QString("Modified Following");
// ��һ��������
const QString Following = QString("Following");
// ǰһ��������
const QString Preceding = QString("Preceding");

// ��������
const QString DayCount = QString("DayCount");
// ʵ������/ʵ������
const QString Act_Act = QString("Act/Act");
// ʵ������/365
const QString Act_365 = QString("Act/365");
// ʵ������/360
const QString Act_360 = QString("Act/360");

// ϢƱ����
const QString CouponType = QString("CouponType");
// ��ϢƱ
const QString ZeroCoupon = QString("ZeroCoupon");
// �̶�ϢƱ
const QString FixedCoupon = QString("FixedCoupon");
// ����ϢƱ
const QString FloatCoupon = QString("FloatCoupon");

// �ο�����
const QString ReferenceIndex = QString("ReferenceIndex");
const QString shibor3m = QString("shibor3m");
const QString repo7d = QString("repo7d");
const QString depo1y = QString("depo1y");

// �г�����
const QString MarketType = QString("MarketType");
// ���м�����г�
const QString IBMoneyMarket = QString("IBMoneyMarket");
// ���м�ծȯ�г�
const QString IBBondMarket = QString("IBBondMarket");
// �Ϻ������������г�
const QString SHEXMoneyMarket = QString("SHEXMoneyMarket");
// �Ϻ���Ʊ�������г�
const QString SHEXStockMarket = QString("SHEXStockMarket");

// ��Ϣ���ʸ�������
const QString ZeroRateCompoundFrequency = QString("ZeroRateCompoundFrequency");
// ��������
const QString ContinuousCompound = QString("ContinuousCompound");
// �긴��
const QString yearlyCompound = QString("yearlyCompound");

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
