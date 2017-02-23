#pragma once
#include "inc\controller_interface.h"

class CParameterSetting :
	public IParameterSetting
{
public:
	CParameterSetting();
	~CParameterSetting();
	//////½ğÈÚÈÕÀú////
	bool isExistFinancialCalendar(const CFinancialCalendar &val);
	bool setFinancialCalendar(const CFinancialCalendar &val);
	bool removeFinancialCalendar(int val);
	bool getFinancialCalendar(QMap<int, CFinancialCalendar> &val);
};

