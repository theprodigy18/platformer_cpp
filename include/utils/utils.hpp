#pragma once

#define EXPORT_FN __declspec(dllexport)

#define BIT(x) (1 << x)
#define KB(x) ((unsigned long long) 1024 * x)
#define MB(x) ((unsigned long long) 1024 * KB(x))
#define GB(x) ((unsigned long long) 1024 * MB(x))
