#pragma once

#define BLA_DYNAMIC_CAST(TYPE, IN_OBJ_PTR, OUT_OBJ_PTR) \
    if (typeid(*IN_OBJ_PTR) == typeid(TYPE))            \
    {												    \
        OUT_OBJ_PTR = static_cast<TYPE*>(IN_OBJ_PTR);   \
    }													\
    else {												\
        OUT_OBJ_PTR = nullptr;							\
    }