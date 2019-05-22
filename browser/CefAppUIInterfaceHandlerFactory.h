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
 * @file CefAppUIInterfaceHandlerFactory.h
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#pragma once
#ifndef __JSCEFHELPER_BROWSER_CEFAPPUIINTERFACEHANDLERFACTORY_H__
#define __JSCEFHELPER_BROWSER_CEFAPPUIINTERFACEHANDLERFACTORY_H__

#include <string>

#include <include/cef_base.h>

namespace JsCefHelper {
	namespace browser {

		class CefAppUIInterfaceHandler;
		class CefAppUIInterfaceHandlerFactory : public virtual CefBaseRefCounted {
		public:
			virtual CefRefPtr<CefAppUIInterfaceHandler> createAppUIInterfaceHandler(const std::string& viewName) = 0;
		};

	} // namespace browser
} // namespace JsCefHelper

#endif /* __JSCEFHELPER_BROWSER_CEFAPPUIINTERFACEHANDLERFACTORY_H__ */
