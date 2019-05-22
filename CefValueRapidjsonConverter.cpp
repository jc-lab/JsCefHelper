/*
 * Copyright 2019 JC-Lab
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file CefValueRapidjsonConverter.cpp
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */

#include "CefValueRapidjsonConverter.h"

#include <vector>

namespace JsCefHelper {

	CefRefPtr<CefValue> rapidjsonToCefValue(const rapidjson::Value& jsonValue);
	CefRefPtr<CefDictionaryValue> rapidjsonToCefDictionaryValue(const rapidjson::Value& jsonValue);
	CefRefPtr<CefListValue> rapidjsonToCefListValue(const rapidjson::Value& jsonValue);

	CefRefPtr<CefDictionaryValue> rapidjsonToCefDictionaryValue(const rapidjson::Value& jsonValue)
	{
		CefRefPtr<CefDictionaryValue> cefDict = CefDictionaryValue::Create();
		for (rapidjson::Value::ConstMemberIterator iter = jsonValue.MemberBegin(); iter != jsonValue.MemberEnd(); iter++)
		{
			std::string name(iter->name.GetString(), iter->name.GetStringLength());
			cefDict->SetValue(name, rapidjsonToCefValue(iter->value));
		}
		return cefDict;
	}

	CefRefPtr<CefListValue> rapidjsonToCefListValue(const rapidjson::Value& jsonValue)
	{
		CefRefPtr<CefListValue> cefList = CefListValue::Create();
		int index = 0;
		for (rapidjson::Value::ConstValueIterator iter = jsonValue.Begin(); iter != jsonValue.End(); iter++)
		{
			CefRefPtr<CefValue> elementValue = rapidjsonToCefValue(*iter);
			cefList->SetValue(index, elementValue);
		}
		return cefList;
	}

	CefRefPtr<CefValue> rapidjsonToCefValue(const rapidjson::Value& jsonValue)
	{
		CefRefPtr<CefValue> result = CefValue::Create();

		if (jsonValue.IsObject())
		{
			result->SetDictionary(rapidjsonToCefDictionaryValue(jsonValue));
		}
		else if (jsonValue.IsArray())
		{
			result->SetList(rapidjsonToCefListValue(jsonValue));
		}
		else if (jsonValue.IsBool())
		{
			result->SetBool(jsonValue.GetBool());
		}
		else if (jsonValue.IsInt())
		{
			result->SetInt(jsonValue.GetInt());
		}
		else if (jsonValue.IsInt64())
		{
			result->SetInt(jsonValue.GetInt64());
		}
		else if (jsonValue.IsUint())
		{
			result->SetInt(jsonValue.GetUint());
		}
		else if (jsonValue.IsUint64())
		{
			result->SetInt(jsonValue.GetUint64());
		}
		else if (jsonValue.IsDouble())
		{
			result->SetDouble(jsonValue.GetDouble());
		}
		else if (jsonValue.IsFloat())
		{
			result->SetDouble(jsonValue.GetFloat());
		}
		else if (jsonValue.IsString())
		{
			result->SetString(std::string(jsonValue.GetString(), jsonValue.GetStringLength()));
		}
		else if (jsonValue.IsFloat())
		{
			result->SetDouble(jsonValue.GetFloat());
		}
		else
		{
			result->SetNull();
		}
		return result;
	}

}
