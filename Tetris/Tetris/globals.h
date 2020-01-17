#pragma once


#define __GLOBAL __global__
#define __declgb(_Type, _Name, ...) namespace { static _Type __GLOBAL##__Name{ __VA_ARGS__ }; }



#undef __declgb

#define global(_Name) (__GLOBAL##__Name)
