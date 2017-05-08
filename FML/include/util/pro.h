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
// 付息频率
const QString CouponFrequency = QString("CouponFrequency");
// 按季
const QString quarterly = QString("quarterly");
// 按年
const QString yearly = QString("yearly");
// 按月
const QString monthly = QString("monthly");
// 按半年
const QString halfyear = QString("halfyear");

// 日历
const QString Calendar = QString("Calendar");
// 银行间市场日历
const QString CFETS = QString("CFETS");
// 中国上海交易所日历
const QString SHSE = QString("SHSE");

// 计息日调整
const QString Convention = QString("Convention");
// 修正的下一个工作日
const QString ModifiedFollowing = QString("Modified Following");
// 下一个工作日
const QString Following = QString("Following");
// 前一个工作日
const QString Preceding = QString("Preceding");

// 天数计数
const QString DayCount = QString("DayCount");
// 实际天数/实际天数
const QString Act_Act = QString("Act/Act");
// 实际天数/365
const QString Act_365 = QString("Act/365");
// 实际天数/360
const QString Act_360 = QString("Act/360");

// 息票类型
const QString CouponType = QString("CouponType");
// 零息票
const QString ZeroCoupon = QString("ZeroCoupon");
// 固定息票
const QString FixedCoupon = QString("FixedCoupon");
// 浮动息票
const QString FloatCoupon = QString("FloatCoupon");

// 参考利率
const QString ReferenceIndex = QString("ReferenceIndex");
const QString shibor3m = QString("shibor3m");
const QString repo7d = QString("repo7d");
const QString depo1y = QString("depo1y");

// 市场类型
const QString MarketType = QString("MarketType");
// 银行间货币市场
const QString IBMoneyMarket = QString("IBMoneyMarket");
// 银行间债券市场
const QString IBBondMarket = QString("IBBondMarket");
// 上海交易所货币市场
const QString SHEXMoneyMarket = QString("SHEXMoneyMarket");
// 上海股票交易所市场
const QString SHEXStockMarket = QString("SHEXStockMarket");

// 零息利率复利类型
const QString ZeroRateCompoundFrequency = QString("ZeroRateCompoundFrequency");
// 连续复利
const QString ContinuousCompound = QString("ContinuousCompound");
// 年复利
const QString yearlyCompound = QString("yearlyCompound");

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
