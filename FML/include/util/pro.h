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
	e_Success = 0, // 成功
	e_NoUser = 1,      // 用户不存在
	e_ErrPswd = 2,     // 密码错误
	e_Exist = 3,       // 用户存在
	e_RegErr = 4,      // 注册失败
	e_ModifyErr = 5,    // 修改失败
} eERR;

#pragma endregion







/////////////////////////////////////////////////////
//重要字段
#pragma region CF
//交割天数
#define SPOTLAG "SpotLag"
#define TADD0 "TADD0"
#define TADD1 "TADD1"
#define TADD2 "TADD2"

// 零息曲线插值方法
#define ZEROCOUPONCURVE "ZeroCouponCurveInterpolation"
#define Z1 "ZM1"
#define Z2 "ZM2"
#define Z3 "ZM3"

// 折现曲线插值方法
#define DiscountCurve "DiscountCurveInterpolation"
#define D1 "DM1"
#define D2 "DM2"
#define D3 "DM3"

// 付息频率
#define COUPONFREQUENCY  "CouponFrequency"
// 按季
#define QUARTERLY  "quarterly"
// 按年
#define YEARLY  "yearly"
// 按月
#define MONTHLY  "monthly"
// 按半年
#define HALFYEAR  "halfyear"

// 日历
#define CALENDAR  "Calendar"
// 银行间市场日历
#define CFETS  "CFETS"
// 中国上海交易所日历
#define SHSE  "SHSE"

// 计息日调整
#define CONVENTION  "Convention"
// 修正的下一个工作日
#define MODIFIEDFOLLOWING  "Modified Following"
// 下一个工作日
#define FOLLOWING  "Following"
// 前一个工作日
#define PRECEDING  "Preceding"

// 天数计数
#define DAYCOUNT  "DayCount"
// 实际天数/实际天数
#define ACT_ACT  "Act/Act"
// 实际天数/365
#define ACT_365 "Act/365"
// 实际天数/360
#define ACT_360 "Act/360"

// 息票类型
#define COUPONTYPE "CouponType"
// 零息票
#define ZEROCOUPON "ZeroCoupon"
// 固定息票
#define FIXEDCOUPON "FixedCoupon"
// 浮动息票
#define FLOATCOUPON "FloatCoupon"

// 参考利率
#define REFERENCEINDEX  "ReferenceIndex"
#define SHIBOR3M "shibor3m"
#define REPO7D "repo7d"
#define DEPO1Y "depo1y"

// 市场类型
#define MARKETTYPE "MarketType"
// 银行间货币市场
#define IBMONEYMARKET "IBMoneyMarket"
// 银行间债券市场
#define IBBONDMARKET "IBBondMarket"
// 上海交易所货币市场
#define SHEXMONEYMARKET "SHEXMoneyMarket"
// 上海股票交易所市场
#define SHEXSTOCKMARKET "SHEXStockMarket"

// 零息利率复利类型
#define ZERORATECOMPOUNDFREQUENCY "ZeroRateCompoundFrequency"
// 连续复利
#define CONTINUOUSCOMPOUND "ContinuousCompound"
// 年复利
#define YEARLYCOMPOUND "yearlyCompound"

#pragma endregion




/////////////////////////////////////////////////////
#pragma region WNDID
// 窗口id
const QString Main_HomePage = QString("Main_HomePage"); // home
const QString ParaMgr_Holiday = QString("ParaMgr_Holiday"); // 金融日历
const QString ParaMgr_Union = QString("ParaMgr_Union"); // 组合管理

#pragma endregion

/////////////////////////////////////////////////////////
#pragma region CONST
#define YMD "yyyy-MM-dd"
#pragma endregion
