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
 * @file CefClientAppOther.h
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#ifndef __JSCEFHELPER_RENDERER_CEFCLIENTAPPOTHER_H__
#define __JSCEFHELPER_RENDERER_CEFCLIENTAPPOTHER_H__
#pragma once

#include <include/cef_base.h>
#include <include/cef_app.h>

namespace JsCefHelper {
	namespace other {

		class CefClientAppOther : public CefApp {
			IMPLEMENT_REFCOUNTING(CefClientAppOther);
			DISALLOW_COPY_AND_ASSIGN(CefClientAppOther);

		public:
			typedef CefClientAppOther super;

			CefClientAppOther() {}
			virtual ~CefClientAppOther() {}
		};

	} // namespace other
} // namespace JsCefHelper

#endif /* __JSCEFHELPER_RENDERER_CEFCLIENTAPPOTHER_H__ */
