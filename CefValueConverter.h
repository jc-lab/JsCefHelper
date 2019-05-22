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
 * @file CefValueConverter.h
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 * @brief Converter between CefV8Value and CefValue.
 */

#pragma once
#ifndef __JSCEFHELPER_CEFVALUECONVERTER_H__
#define __JSCEFHELPER_CEFVALUECONVERTER_H__

#include <include/cef_app.h>

namespace JsCefHelper {

	CefRefPtr<CefValue> CefV8ValueToCefValue(const CefRefPtr<CefV8Value>& value);
	void AppendCefV8ValueListToCefListValue(CefRefPtr<CefListValue> result, int beginIndex, const CefV8ValueList& valueList);
	CefRefPtr<CefV8Value> CefValueToCefV8Value(CefRefPtr<CefValue> value);

}

#endif /* __JSCEFHELPER_CEFVALUECONVERTER_H__ */
