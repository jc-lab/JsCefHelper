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
 * @file CefBrowserApp.h
 * @author Jichan ( development@jc-lab.net )
 * @date 2019-05-22
 */
#ifndef __JSCEFHELPER_RENDERER_CEFBROWSERAPP_H__
#define __JSCEFHELPER_RENDERER_CEFBROWSERAPP_H__
#pragma once

#include <include/cef_base.h>
#include <include/cef_app.h>
#include <include/wrapper/cef_resource_manager.h>

#include <list>

#include "CefInternalResourceProvider.h"

namespace JsCefHelper {
	namespace browser {

		class CefBrowserApp : public CefApp {
		protected:
			CefRefPtr<CefResourceManager> cef_resource_manager_;
			std::list< CefRefPtr<CefInternalResourceProvider> > managed_resource_providers_;
			bool cef_inited_;

		private:
			static void CefResourceManagerAddProvider_(CefResourceManager * manager, CefResourceManager::Provider* provider, int order, const std::string& identifier);

		public:
			typedef CefBrowserApp super;

			CefBrowserApp();
			virtual ~CefBrowserApp();

			bool init(const CefMainArgs& args, const CefSettings *base_settings = NULL);
			bool init(const CefMainArgs& args, const CefSettings& settings, void* windows_sandbox_info);

			CefResourceManager* cef_resource_manager() const { return cef_resource_manager_.get(); }

			template<typename T>
			void AddResourceProvider(T provider, int order, const std::string& identifier);// { throw std::exception("ASSERT"); }
		
			template<>
			void AddResourceProvider<CefResourceManager::Provider*>(CefResourceManager::Provider* provider, int order, const std::string& identifier)
			{
				CefResourceManagerAddProvider_(cef_resource_manager_.get(), provider, order, identifier);
			}

			template<>
			void AddResourceProvider< CefRefPtr<CefInternalResourceProvider> >(CefRefPtr<CefInternalResourceProvider> provider, int order, const std::string& identifier)
			{
				managed_resource_providers_.push_back(provider);
				CefResourceManagerAddProvider_(cef_resource_manager_.get(), provider.get(), order, identifier);
			}

		};

	} // namespace browser
} // namespace JsCefHelper

#endif /* __JSCEFHELPER_RENDERER_CEFBROWSERAPP_H__ */
