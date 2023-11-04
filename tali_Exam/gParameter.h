#pragma once

#include <map>
#include <vector>
#include <string>
#include "ini.h"

using namespace std;

enum DataType
{
	TYPE_BOOLEAN = 0,
	TYPE_INT = 1,
	TYPE_DOUBLE = 2,
	TYPE_STRING = 3
};


struct PARAM {
	int nDataType;
	bool bValue;
	int nValue;
	double dValue;
	string strGroupName;
	string strValue;
	PARAM() {
		nDataType = 0;
		bValue = 0;
		nValue = 0;
		dValue = 0;
	}
};


class gParameter
{
public:
	gParameter();
	~gParameter();

	void LoadParam();
	void SaveParam();
	string	GetParameterPath();
	void	SetParameterPath(string strPath);

	shared_ptr<pair<string, PARAM>> MakeParam(string strGroupName, string _strKey, bool bVal);
	shared_ptr<pair<string, PARAM>> MakeParam(string strGroupName, string _strKey, int nVal);
	shared_ptr<pair<string, PARAM>> MakeParam(string strGroupName, string _strKey, float fVal);
	shared_ptr<pair<string, PARAM>> MakeParam(string strGroupName, string _strKey, double dVal);
	shared_ptr<pair<string, PARAM>> MakeParam(string strGroupName, string _strKey, string strVal);

	shared_ptr<pair<string, PARAM>> GetParam(string strKey);

	CString             GetValueString(string strKey);

	bool				SetParam(shared_ptr<pair<string, PARAM>> data);
	bool				SetParam(shared_ptr<pair<string, PARAM>> data, string value);

	vector<string>		GetListParam();
	vector<string>		GetListParamFromGroupName(string search_group_name);
	vector<string>		GetListGroup();

private:
	map<string, PARAM>	m_map;
	string _parameter_path;

};

