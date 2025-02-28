#include "fatfs.h"
#include <sys/stat.h>

wl_handle_t FatFs::s_WLHandle = WL_INVALID_HANDLE;

bool FatFs::Init(const char *root, int maxFiles)
{
    esp_vfs_fat_mount_config_t mountcfg = {
        .format_if_mount_failed = true,
        .max_files = maxFiles,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .use_one_fat = false
    };

   if (esp_vfs_fat_spiflash_mount_rw_wl(root, s_PartitionName, &mountcfg, &s_WLHandle) != ESP_OK)
        return false;

    return true;
}

bool FatFs::FileExists(const char *path)
{
    struct stat st;
    return (stat(path, &st) == 0);
}

bool FatFs::FileIsEmpty(const char *path)
{
    struct stat st;
    if (stat(path, &st) == 0)
        return false;
    return (st.st_size == 0);
}
