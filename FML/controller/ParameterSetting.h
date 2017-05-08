#pragma once
#include "inc\controller_interface.h"

class CParameterSetting :
	public IParameterSetting
{
public:
	CParameterSetting();
	~CParameterSetting();
	//////��������////
	bool isExistFinancialCalendar(const CFinancialCalendar &val);
	bool setFinancialCalendar(const CFinancialCalendar &val, QString &err);
	bool removeFinancialCalendar(int val, QString &err);
	const QMap<int, CFinancialCalendar> &getFinancialCalendar() const;

	///////��Ϲ���///////////
	bool setPortfolio(const CPortfolio &val, QString &err);
	bool removePortfolio(const QString &val, bool bDelChildren, QString &err);
	const QMap<QString, CPortfolio> &getPortfolio();
	bool isParentCode(const QString &parent, const QString &child);
	bool isExistCode(const QString &val);
	void getAllRootCodes(QStringList &val);
	void getChildren(const QString &key, QStringList &val);
	bool getPortfolio(const QString &key, CPortfolio &val);

	///////��Ʒ����///////////
	const QMap<QString, CProduct>& getProduct();
	bool getProduct(const QString &code, CProduct &val);
	bool setProduct(const CProduct &val, QString &err);
	bool removeProduct(const QString &code, QString &err);
	QList<CProduct> getRootProduct();
	// ��ȡֱϵ�ӽڵ�
	QList<CProduct> getChildrenProduct(const QString &parentCode);
	// ��ȡ�����ӽڵ�
	void getAllChildrenProduct(const QString &parentCode, QList<CProduct> &results);

	///////�����ֵ�///////////
	const QMap<QString, QList<CParaDict>>& getParadict()const;
	bool getParadict(const QString &typeCode, const QString &paraCode, CParaDict &val);
	bool getAllParadict(const QString &typeCode, QList<CParaDict> &valList);
	bool setParadict(const CParaDict &val, QString &err);
	bool removeParadict(const QString &typeCode, const QString &paraCode, QString &err);

private:
	// ��ʼ�������ֵ�
	void initParaDict();

private:
	//��������
	QMap<int, CFinancialCalendar> m_mapFinancialCalendar;
	//��Ϲ���
	QMap<QString, CPortfolio> m_portfolio;
	//��Ʒ��Ϣ
	QMap<QString, CProduct> m_product;
	//�����ֵ�
	QMap<QString, QList<CParaDict>> m_paradict;
};

