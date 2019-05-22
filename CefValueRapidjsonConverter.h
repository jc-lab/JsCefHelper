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
 * @file CefValueRapidjsonConverter.h
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 * @brief Converter between CefValue and rapidjson.
 */

#pragma once
#ifndef __JSCEFHELPER_CEFVALUERAPIDJSONCONVERTER_H__
#define __JSCEFHELPER_CEFVALUERAPIDJSONCONVERTER_H__

#include <include/cef_app.h>
#include <rapidjson/document.h>

namespace JsCefHelper {

	CefRefPtr<CefValue> rapidjsonToCefValue(const rapidjson::Value& jsonValue);


	template<class AllocatorT>
	void cefValueToRapidJson(rapidjson::Value& jsonObject, AllocatorT& jsonAllocator, const CefRefPtr<CefValue>& cefValue);

	template<class AllocatorT>
	void cefListValueToRapidJson(rapidjson::Value& jsonObject, AllocatorT& jsonAllocator, const CefRefPtr<CefListValue>& cefValue)
	{
		int i;
		jsonObject.SetArray();
		for (i = 0; i < cefValue->GetSize(); i++)
		{
			rapidjson::Value jsonItem;
			cefValueToRapidJson(jsonItem, jsonAllocator, cefValue->GetValue(i));
		}
	}

	template<class AllocatorT>
	void cefDictionaryValueToRapidJson(rapidjson::Value& jsonObject, AllocatorT& jsonAllocator, const CefRefPtr<CefDictionaryValue>& cefValue)
	{
		int i;
		std::vector<CefString> keys;
		jsonObject.SetObject();

		if (cefValue->GetKeys(keys))
		{
			for (std::vector<CefString>::const_iterator iter = keys.begin(); iter != keys.end(); iter++)
			{
				std::string keyName = *iter;
				rapidjson::Value jsonKey;
				rapidjson::Value jsonItem;
				jsonKey.SetString(keyName.c_str(), keyName.length(), jsonAllocator);
				cefValueToRapidJson(jsonItem, jsonAllocator, cefValue->GetValue(*iter));
				jsonObject.AddMember(jsonKey, jsonItem, jsonAllocator);
			}
		}
	}

	template<class AllocatorT>
	void cefValueToRapidJson(rapidjson::Value& jsonObject, AllocatorT& jsonAllocator, const CefRefPtr<CefValue>& cefValue)
	{
		if (!cefValue)
		{
			jsonObject.SetNull();
		}
		else
		{
			switch (cefValue->GetType())
			{
			case CefValueType::VTYPE_LIST:
				cefListValueToRapidJson(jsonObject, jsonAllocator, cefValue->GetList());
				break;
			case CefValueType::VTYPE_DICTIONARY:
				cefDictionaryValueToRapidJson(jsonObject, jsonAllocator, cefValue->GetDictionary());
				break;
			case CefValueType::VTYPE_BOOL:
				jsonObject.SetBool(cefValue->GetBool());
				break;
			case CefValueType::VTYPE_DOUBLE:
				jsonObject.SetDouble(cefValue->GetDouble());
				break;
			case CefValueType::VTYPE_INT:
				jsonObject.SetInt(cefValue->GetInt());
				break;
			case CefValueType::VTYPE_STRING:
				std::string valueText = cefValue->GetString();
				jsonObject.SetString(valueText.c_str(), valueText.length(), jsonAllocator);
				break;
			}
		}
	}

}

#endif /* __JSCEFHELPER_CEFVALUERAPIDJSONCONVERTER_H__ */

