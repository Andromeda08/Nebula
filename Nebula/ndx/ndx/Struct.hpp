#pragma once

#ifndef DISABLE_COPY
#define DISABLE_COPY(type)              \
type(const type&) = delete;             \
type& operator=(const type&) = delete;
#endif

#ifndef struct_param
#define struct_param(struct_t, param_t, param_name, param_init) \
param_t param_name = param_init;                                \
inline struct_t& set_##param_name(const param_t& value) {       \
    param_name = value;                                         \
    return *this;                                               \
}
#endif