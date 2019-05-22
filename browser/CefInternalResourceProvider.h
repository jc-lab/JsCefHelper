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
 * @file CefInternalResourceProvider.h
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#ifndef __JSCEFHELPER_RENDERER_CEFCLIENTAPPBROWSER_H__
#define __JSCEFHELPER_RENDERER_CEFCLIENTAPPBROWSER_H__
#pragma once

#include <include/cef_base.h>
#include <include/cef_scheme.h>
#include <include/wrapper/cef_resource_manager.h>
#include <include/wrapper/cef_stream_resource_handler.h>

namespace JsCefHelper {
	namespace browser {

		class CefInternalResourceProvider : public CefResourceManager::Provider, public virtual CefBaseRefCounted {
		private:
			CefString url_prefix_;

		public:
			CefInternalResourceProvider(const std::string& url_prefix);
			virtual ~CefInternalResourceProvider() {}

			virtual bool OnRequest(scoped_refptr<CefResourceManager::Request> request) override;
			virtual void OnRequestCanceled(scoped_refptr<CefResourceManager::Request> request) override;

			static std::string ResolveMimeFromUrl(const std::string& url);

			virtual CefRefPtr<CefStreamResourceHandler> ReadResource(const std::string& resource_name) = 0;
		};

	}
}

#endif /* __JSCEFHELPER_RENDERER_CEFCLIENTAPPBROWSER_H__ */
