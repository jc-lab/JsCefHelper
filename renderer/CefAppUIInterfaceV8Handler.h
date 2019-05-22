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
 * @file CefAppUIInterfaceV8Handler.h
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#ifndef __JSCEFHELPER_RENDERER_CefAppUIInterfaceV8Handler_H__
#define __JSCEFHELPER_RENDERER_CefAppUIInterfaceV8Handler_H__

#pragma once

#include <include/cef_app.h>
#include <include/base/cef_lock.h>

#include <map>

namespace JsCefHelper {
	namespace renderer {

		class CefAppUIInterfaceV8Handler : public CefV8Handler
		{
			// Include the default reference counting implementation.
			IMPLEMENT_REFCOUNTING(CefAppUIInterfaceV8Handler);
			// Include the default locking implementation.
			IMPLEMENT_LOCKING(CefAppUIInterfaceV8Handler);

		private:
			CefRefPtr<CefBrowser> m_browser;
			std::string m_viewName;
			std::string m_uniqueKey;

		public:
			CefRefPtr<CefV8Value> m_obj;
			CefRefPtr<CefV8Context> m_cefJsContext;
			std::map<std::string, CefRefPtr<CefV8Value> > m_cefJsFuncs;

			CefAppUIInterfaceV8Handler(const CefRefPtr<CefBrowser>& browser, const std::string& viewName, const std::string& uniqueKey);
			~CefAppUIInterfaceV8Handler();

			bool Execute(const CefString& name,
				CefRefPtr<CefV8Value> object,
				const CefV8ValueList& arguments,
				CefRefPtr<CefV8Value>& retval,
				CefString& exception);

			CefRefPtr<CefV8Value> createJsObject();
			void destoryJsObject();

			std::string getUniqueKey() const { return m_uniqueKey; }

			static void callAppUIInterfaceImpl(const CefRefPtr<CefAppUIInterfaceV8Handler>& rendererInterface, const std::string& name, const CefRefPtr<CefListValue>& arguments, int argBeginIndex);
		};
	} // namespace renderer
} // namespace JsCefHelper

#endif /* __JSCEFHELPER_RENDERER_CefAppUIInterfaceV8Handler_H__ */
