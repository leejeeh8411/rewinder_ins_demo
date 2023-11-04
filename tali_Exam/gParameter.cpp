#include "stdafx.h"
#include "gParameter.h"


gParameter::gParameter()
{
}


gParameter::~gParameter()
{
}


//파라미터 리스트를 호출
vector<string> gParameter::GetListParam()
{
	int nSize = static_cast<int>(m_map.size());
	vector<string> vtParamList;

	for (const auto& pair : m_map)
	{
		string str = pair.first;
		vtParamList.emplace_back(str);
	}

	return vtParamList;
}

//그룹이름으로 하위 파라미터 리스트를 호출
vector<string> gParameter::GetListParamFromGroupName(string search_group_name)
{
	int nSize = static_cast<int>(m_map.size());
	vector<string> vtParamList;

	for (const auto& pair : m_map)
	{
		string key = pair.first;
		PARAM param = pair.second;
		string group = param.strGroupName;
		if (group == search_group_name) {
			vtParamList.emplace_back(key);
		}
	}

	return vtParamList;
}

//파라미터 그룹
vector<string> gParameter::GetListGroup()
{
	int nSize = static_cast<int>(m_map.size());
	vector<string> vtGroupList;

	for (const auto& pair : m_map) {
		string key = pair.first;
		PARAM param = pair.second;

		string group = param.strGroupName;

		bool exist = false;
		for each (auto var in vtGroupList) {
			if (var == group) {
				exist = true;
				break;
			}
		}

		if (exist == false) {
			vtGroupList.emplace_back(group);
		}
	}

	return vtGroupList;
}




shared_ptr<pair<string, PARAM>> gParameter::MakeParam(string strGroupName, string _strKey, bool bVal)
{
	shared_ptr<pair<string, PARAM>> ptr_param = make_shared<pair<string, PARAM>>();

	PARAM stParam;
	stParam.nDataType = TYPE_BOOLEAN;
	stParam.bValue = bVal;
	stParam.strGroupName = strGroupName;

	string strKey = _strKey;

	ptr_param->first = strKey;
	ptr_param->second = stParam;

	return ptr_param;
}

shared_ptr<pair<string, PARAM>> gParameter::MakeParam(string strGroupName, string _strKey, int nVal)
{
	shared_ptr<pair<string, PARAM>> ptr_param = make_shared<pair<string, PARAM>>();

	PARAM stParam;
	stParam.nDataType = TYPE_INT;
	stParam.nValue = nVal;
	stParam.strGroupName = strGroupName;

	string strKey = _strKey;

	ptr_param->first = strKey;
	ptr_param->second = stParam;

	return ptr_param;
}

shared_ptr<pair<string, PARAM>> gParameter::MakeParam(string strGroupName, string _strKey, float fVal)
{
	shared_ptr<pair<string, PARAM>> ptr_param = make_shared<pair<string, PARAM>>();

	PARAM stParam;
	stParam.nDataType = TYPE_DOUBLE;
	stParam.dValue = fVal;
	stParam.strGroupName = strGroupName;

	string strKey = _strKey;

	ptr_param->first = strKey;
	ptr_param->second = stParam;

	return ptr_param;
}



shared_ptr<pair<string, PARAM>> gParameter::MakeParam(string strGroupName, string _strKey, double dVal)
{
	shared_ptr<pair<string, PARAM>> ptr_param = make_shared<pair<string, PARAM>>();

	PARAM stParam;
	stParam.nDataType = TYPE_DOUBLE;
	stParam.dValue = dVal;
	stParam.strGroupName = strGroupName;

	string strKey = _strKey;

	ptr_param->first = strKey;
	ptr_param->second = stParam;

	return ptr_param;
}

shared_ptr<pair<string, PARAM>> gParameter::MakeParam(string strGroupName, string _strKey, string strVal)
{
	shared_ptr<pair<string, PARAM>> ptr_param = make_shared<pair<string, PARAM>>();

	PARAM stParam;
	stParam.nDataType = TYPE_STRING;
	stParam.strGroupName = strGroupName;

	stParam.strValue = strVal;

	string strKey = _strKey;

	ptr_param->first = strKey;
	ptr_param->second = stParam;

	return ptr_param;
}

shared_ptr<pair<string, PARAM>> gParameter::GetParam(string strKey)
{
	shared_ptr<pair<string, PARAM>> return_param = nullptr;

	for (const auto& pair : m_map) {
		string str = pair.first;
		if (strKey == str) {
			return_param = make_shared<std::pair<string, PARAM>>();
			//복사생성?
			return_param->first = pair.first;
			return_param->second = pair.second;
			break;
		}
	}

	return return_param;
}


CString gParameter::GetValueString(string strKey)
{
	CString str_val;

	shared_ptr<pair<string, PARAM>> ptr_param = GetParam(strKey);

	if (ptr_param->second.nDataType == TYPE_BOOLEAN) {
		if (ptr_param->second.bValue == true) {
			str_val = TEXT("true");
		}
		else {
			str_val = TEXT("false");
		}
	}
	else if (ptr_param->second.nDataType == TYPE_INT) {
		str_val.Format(TEXT("%d"), ptr_param->second.nValue);
	}
	else if (ptr_param->second.nDataType == TYPE_DOUBLE) {
		str_val.Format(TEXT("%0.8f"), ptr_param->second.dValue);
	}
	else if (ptr_param->second.nDataType == TYPE_STRING) {
		str_val.Format(TEXT("%s"), ptr_param->second.strValue);
	}

	return str_val;
}


bool gParameter::SetParam(shared_ptr<pair<string, PARAM>> data)
{
	bool bRet = false;

	for (const auto& pair : m_map) {
		string strMapKey = pair.first;
		//기존에 있다면 set
		if (data->first == strMapKey) {
			PARAM stParam = pair.second;
			stParam = data->second;
			m_map[strMapKey] = stParam;
			bRet = true;
			break;
		}
	}

	//기존에 없다면 insert
	if (bRet == false) {
		m_map.insert(pair<string, PARAM>(data->first, data->second));
	}

	return true;
}

//value를 string 으로 주면 param 형식에 맞춰서 변환한 뒤, set 한다.
bool gParameter::SetParam(shared_ptr<pair<string, PARAM>> data, string value)
{
	bool bRet = false;

	for (const auto& pair : m_map) {
		string strMapKey = pair.first;
		//기존에 있다면 set
		if (data->first == strMapKey) {
			PARAM stParam = pair.second;

			if (stParam.nDataType == TYPE_BOOLEAN) {
				if (value == "true") {
					stParam.bValue = true;
				}
				else if (value == "false") {
					stParam.bValue = false;
				}
			}
			else if (stParam.nDataType == TYPE_INT) {
				stringstream ssInt(value);
				ssInt >> stParam.nValue;
			}
			else if (stParam.nDataType == TYPE_DOUBLE) {
				stringstream ssDouble(value);
				ssDouble >> stParam.dValue;
			}
			else if (stParam.nDataType == TYPE_STRING) {
				stParam.strValue = value;
			}
			m_map[strMapKey] = stParam;
			bRet = true;
			break;
		}
	}

	return true;
}

string gParameter::GetParameterPath()
{
	return _parameter_path;
}

void gParameter::SetParameterPath(string strPath)
{
	_parameter_path = strPath;
}

void gParameter::LoadParam()
{
	// first, create a file instance
	mINI::INIFile file(_parameter_path);

	// next, create a structure that will hold data
	mINI::INIStructure ini;

	file.read(ini);

	//그룹리스트를 가져온다
	vector<string> vt_group_list = GetListGroup();

	for (int i = 0; i < vt_group_list.size(); i++) {
		string group_name = vt_group_list[i];

		//그룹이름으로 하위 리스트를 가져온다.
		vector<string> vt_param_in_group = GetListParamFromGroupName(group_name);

		for each (string key in vt_param_in_group) {
			//ini에서 가져온 문자열 데이터를 해당 param에 reset

			//일단 파라미터 가져오자
			shared_ptr<pair<string, PARAM>> ptr_param;
			ptr_param = GetParam(key);

			//ini에서 읽은 데이터
			string& value = ini[group_name][key];

			//데이터타입에 맞춰 set 한다.
			bool ret = SetParam(ptr_param, value);

		}
	}
}

void gParameter::SaveParam()
{
	// first, create a file instance
	mINI::INIFile file(_parameter_path);

	// next, create a structure that will hold data
	mINI::INIStructure ini;

	//그룹리스트를 가져온다
	vector<string> vt_group_list = GetListGroup();

	for (int i = 0; i < vt_group_list.size(); i++) {
		string group_name = vt_group_list[i];

		//그룹이름으로 하위 리스트를 가져온다.
		vector<string> vt_param_in_group = GetListParamFromGroupName(group_name);

		for (int i = 0; i < vt_param_in_group.size(); i++) {
			string key = vt_param_in_group[i];
			//파라미터 변수 타입을 봐서 문자열로 변환해줘야 함
			CString str_val = GetValueString(key);
			ini[group_name][key] = CT2CA(str_val);
		}
	}

	file.write(ini);

}

