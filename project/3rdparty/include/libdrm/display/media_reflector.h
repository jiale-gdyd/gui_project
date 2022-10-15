#ifndef LIBDRM_DISPLAY_MEDIA_REFLECTOR_H
#define LIBDRM_DISPLAY_MEDIA_REFLECTOR_H

#include "key_string.h"
#include "reflector.h"

#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <exception>

#define DEFINE_MEDIA_CHILD_FACTORY(REAL_PRODUCT, IDENTIFIER, FINAL_EXPOSE_PRODUCT, PRODUCT)                             \
    DEFINE_CHILD_FACTORY(REAL_PRODUCT, IDENTIFIER, FINAL_EXPOSE_PRODUCT, PRODUCT,                                       \
        public:                                                                                                         \
            virtual bool AcceptRules(const std::map<std::string, std::string> &maps) const override;                    \
                                                                                                                        \
            static const char *ExpectedInputDataType();                                                                 \
            static const char *OutPutDataType();                                                                        \
                                                                                                                        \
    )

#define DEFINE_MEDIA_CHILD_FACTORY_EXTRA(REAL_PRODUCT)                                                                  \
    bool REAL_PRODUCT##Factory::AcceptRules(const std::map<std::string, std::string> &maps) const {                     \
        static std::list<std::string> expected_data_type_list;                                                          \
        static std::list<std::string> out_data_type_list;                                                               \
        static const char *static_keys[] = {DRM_KEY_INPUTDATATYPE, DRM_KEY_OUTPUTDATATYPE, NULL};                       \
        static const decltype(ExpectedInputDataType) *static_call[] = {&ExpectedInputDataType, &OutPutDataType, NULL};  \
        static std::list<std::string> *static_list[] = {&expected_data_type_list, &out_data_type_list, NULL};           \
        const char **keys = static_keys;                                                                                \
        const decltype(ExpectedInputDataType) **call = static_call;                                                     \
        std::list<std::string> **list = static_list;                                                                    \
                                                                                                                        \
        while (*keys) {                                                                                                 \
            try {                                                                                                       \
                auto it = maps.find(*keys);                                                                             \
                if (it == maps.end()) {                                                                                 \
                    if ((*call)()) {                                                                                    \
                        return false;                                                                                   \
                    }                                                                                                   \
                } else {                                                                                                \
                    const std::string &value = it->second;                                                              \
                    if (!value.empty() && !has_intersection(value.c_str(), (*call)(), *list)) {                         \
                        return false;                                                                                   \
                    }                                                                                                   \
                }                                                                                                       \
            } catch (const std::exception &e) {                                                                         \
                printf("catch exception:[%s]\n", e.what());                                                             \
            }                                                                                                           \
                                                                                                                        \
            ++keys;                                                                                                     \
            ++call;                                                                                                     \
            ++list;                                                                                                     \
        }                                                                                                               \
                                                                                                                        \
        return true;                                                                                                    \
    }

#define DEFINE_MEDIA_NEW_PRODUCT_BY(REAL_PRODUCT, PRODUCT, COND)                                                        \
    std::shared_ptr<PRODUCT> FACTORY(REAL_PRODUCT)::NewProduct(const char *param) {                                     \
        auto ret = std::make_shared<REAL_PRODUCT>(param);                                                               \
        if (ret && ret->COND) {                                                                                         \
            return nullptr;                                                                                             \
        }                                                                                                               \
                                                                                                                        \
        return ret;                                                                                                     \
    }

#endif
