#pragma once

// Indices of queues families if they exist at all


#include <cstdint>

struct QueueFamilyIndexes {
    uint32_t graphicsFamilyIndex = -1;        // Location of graphics queue family
};