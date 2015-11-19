// Copyright � 2010-2015 The CefSharp Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.

#include "Stdafx.h"
#include "Primitives.h"
#include "JsObjectsSerialization.h"
#include "V8Serialization.h"

namespace CefSharp
{
    namespace Internals
    {
        namespace Serialization
        {
            void SerializeJsObject(JavascriptObject^ jsObject, const CefRefPtr<CefListValue> &list, int index)
            {
                auto objList = CefListValue::Create();
                SetInt64(jsObject->Id, objList, 0);
                objList->SetString(1, StringUtils::ToNative(jsObject->Name));
                objList->SetString(2, StringUtils::ToNative(jsObject->JavascriptName));

                auto methodList = CefListValue::Create();
                auto j = 0;
                methodList->SetInt(j++, jsObject->Methods->Count);
                for each (JavascriptMethod^ jsMethod in jsObject->Methods)
                {
                    SetInt64(jsMethod->Id, methodList, j++);
                    methodList->SetString(j++, StringUtils::ToNative(jsMethod->ManagedName));
                    methodList->SetString(j++, StringUtils::ToNative(jsMethod->JavascriptName));
                    methodList->SetInt(j++, jsMethod->ParameterCount);
                }
                objList->SetList(3, methodList);

                auto propertyList = CefListValue::Create();
                j = 0;
                propertyList->SetInt(j++, jsObject->Properties->Count);
                for each(JavascriptProperty^ jsProperty in jsObject->Properties)
                {
                    SetInt64(jsProperty->Id, propertyList, j++);
                    propertyList->SetString(j++, StringUtils::ToNative(jsProperty->ManagedName));
                    propertyList->SetString(j++, StringUtils::ToNative(jsProperty->JavascriptName));
                    propertyList->SetBool(j++, jsProperty->IsComplexType);
                    propertyList->SetBool(j++, jsProperty->IsReadOnly);

                    if (jsProperty->JsObject != nullptr)
                    {
                        SerializeJsObject(jsProperty->JsObject, propertyList, j++);
                    }
                    else
                    {
                        propertyList->SetNull(j++);
                    }

                    if (jsProperty->PropertyValue != nullptr)
                    {
                        SerializeV8Object(propertyList, j++, jsProperty->PropertyValue);
                    }
                    else
                    {
                        propertyList->SetNull(j++);
                    }
                }
                objList->SetList(4, propertyList);

                list->SetList(index, objList);
            }

            void SerializeJsObject(JavascriptRootObject^ object, const CefRefPtr<CefListValue> &list, int index)
            {
                auto subList = CefListValue::Create();
                auto i = 0;
                for each (JavascriptObject^ jsObject in object->MemberObjects)
                {
                    SerializeJsObject(jsObject, subList, i++);
                }
                list->SetList(index, subList);
            }
        }
    }
}