#pragma once
#include "esp_vfs.h"
#include "esp_vfs_fat.h"

class FatFs
{
private:
    static wl_handle_t s_WLHandle;
    static constexpr char *s_PartitionName = "storage";
public:
    static bool Init(const char *root, int maxFiles);

    static bool FileExists(const char *path);
    static bool FileIsEmpty(const char *path);
};