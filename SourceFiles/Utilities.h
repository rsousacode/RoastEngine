#pragma once
#include <cstdint>

// Indices of queues families if they exist at all
struct
QueueFamilyIndexes {
    uint32_t graphicsFamilyIndex = -1;        // Location of graphics queue family
    uint32_t presentationFamilyIndex  = -1;

    // check if queue families are valid

    bool isValid() {
        return graphicsFamilyIndex >= 0 && presentationFamilyIndex >= 0;
    }
};