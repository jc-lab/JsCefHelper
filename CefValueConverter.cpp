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
 * @file CefValueConverter.cpp
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */

#include "CefValueConverter.h"

#include <vector>

namespace JsCefHelper {

	CefRefPtr<CefValue> CefV8ValueToCefValue(const CefRefPtr<CefV8Value>& value)
	{
		CefRefPtr<CefValue> result = CefValue::Create();;

		if (value->IsBool())
		{
			result->SetBool(value->GetBoolValue());
		}
		else if (value->IsInt())
		{
			result->SetInt(value->GetIntValue());
		}
		else if (value->IsDouble())
		{
			result->SetDouble(value->GetDoubleValue());
		}
		else if (value->IsString())
		{
			result->SetString(value->GetStringValue());
		}
		else if (value->IsArray())
		{
			int i;
			int count = value->GetArrayLength();
			CefRefPtr<CefListValue> resultImpl = CefListValue::Create();
			resultImpl->SetSize(count);
			for (i = 0; i < count; i++)
			{
				resultImpl->SetValue(i, CefV8ValueToCefValue(value->GetValue(i)));
			}
			result->SetList(resultImpl);
		}
		else if (value->IsObject())
		{
			CefRefPtr<CefDictionaryValue> resultImpl = CefDictionaryValue::Create();
			std::vector<CefString> keys;
			value->GetKeys(keys);
			for (std::vector<CefString>::iterator iterKey = keys.begin(); iterKey != keys.end(); iterKey++)
			{
				resultImpl->SetValue(*iterKey, CefV8ValueToCefValue(value->GetValue(*iterKey)));
			}
			result->SetDictionary(resultImpl);
		}
		else if (value->IsUndefined())
		{
			result->SetNull();
		}
		else if (value->IsNull())
		{
			result->SetNull();
		}
		else {
			assert(0);
		}
		return result;
	}

	void AppendCefV8ValueListToCefListValue(CefRefPtr<CefListValue> result, int beginIndex, const CefV8ValueList& valueList)
	{
		for (CefV8ValueList::const_iterator iter = valueList.begin(); iter != valueList.end(); iter++)
		{
			result->SetValue(beginIndex++, CefV8ValueToCefValue((*iter)));
		}
	}

	CefRefPtr<CefV8Value> CefValueToCefV8Value(CefRefPtr<CefValue> value)
	{
		CefRefPtr<CefV8Value> result;
		switch (value->GetType()) {
		case VTYPE_INVALID:
		{
			//std::cout << "Type: VTYPE_INVALID" << std::endl;
			result = CefV8Value::CreateNull();
		}
		break;
		case VTYPE_NULL:
		{
			//std::cout << "Type: VTYPE_NULL" << std::endl;
			result = CefV8Value::CreateNull();
		}
		break;
		case VTYPE_BOOL:
		{
			//std::cout << "Type: VTYPE_BOOL" << std::endl;
			result = CefV8Value::CreateBool(value->GetBool());
		}
		break;
		case VTYPE_INT:
		{
			//std::cout << "Type: VTYPE_INT" << std::endl;
			result = CefV8Value::CreateInt(value->GetInt());
		}
		break;
		case VTYPE_DOUBLE:
		{
			//std::cout << "Type: VTYPE_DOUBLE" << std::endl;
			result = CefV8Value::CreateDouble(value->GetDouble());
		}
		break;
		case VTYPE_STRING:
		{
			//std::cout << "Type: VTYPE_STRING" << std::endl;
			result = CefV8Value::CreateString(value->GetString());
		}
		break;
		case VTYPE_BINARY:
		{
			//std::cout << "Type: VTYPE_BINARY" << std::endl;
			result = CefV8Value::CreateNull();
		}
		break;
		case VTYPE_DICTIONARY:
		{
			//std::cout << "Type: VTYPE_DICTIONARY" << std::endl;
			result = CefV8Value::CreateObject(NULL, NULL);
			CefRefPtr<CefDictionaryValue> dict = value->GetDictionary();
			CefDictionaryValue::KeyList keys;
			dict->GetKeys(keys);
			for (unsigned int i = 0; i < keys.size(); i++) {
				CefString key = keys[i];
				result->SetValue(key, CefValueToCefV8Value(dict->GetValue(key)), V8_PROPERTY_ATTRIBUTE_NONE);
			}
		}
		break;
		case VTYPE_LIST:
		{
			//std::cout << "Type: VTYPE_LIST" << std::endl;
			CefRefPtr<CefListValue> list = value->GetList();
			int size = list->GetSize();
			result = CefV8Value::CreateArray(size);
			for (int i = 0; i < size; i++) {
				result->SetValue(i, CefValueToCefV8Value(list->GetValue(i)));
			}
		}
		break;
		}
		return result;
	}

}
