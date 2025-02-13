#pragma once
#include "esp_vfs.h"
#include "esp_vfs_fat.h"

class FatFs
{
private:
    static wl_handle_t s_WLHandle;
public:
    static bool Init(int max);
};