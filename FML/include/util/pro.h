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

typedef enum e_ErrType
{
	e_Success = 0, // �ɹ�
	e_NoUser = 1,      // �û�������
	e_ErrPswd = 2,     // �������
	e_Exist = 3,       // �û�����
	e_RegErr = 4,      // ע��ʧ��
	e_ModifyErr = 5,    // �޸�ʧ��
} eERR;



/////////////////////////////////////////////////////
// ����id
const QString Main_HomePage = QString("Main_HomePage"); // home
const QString ParaMgr_Holiday = QString("ParaMgr_Holiday"); // ��������
const QString ParaMgr_Union = QString("ParaMgr_Union"); // ��Ϲ���