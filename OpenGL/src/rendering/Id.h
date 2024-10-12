#pragma once

#include <cstdint>

class Id {
public:
   static int32_t get() { return next++; }

private:
   static int32_t next;
};
