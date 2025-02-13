#include "fatfs.h"


static constexpr char* s_BasePath = "/db";
wl_handle_t FatFs::s_WLHandle = WL_INVALID_HANDLE;

bool FatFs::Init(int max)
{
    esp_vfs_fat_mount_config_t mountcfg = {
        .format_if_mount_failed = true,
        .max_files = max,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .use_one_fat = false
    };

   if (esp_vfs_fat_spiflash_mount_rw_wl(s_BasePath, "storage", &mountcfg, &s_WLHandle) != ESP_OK)
        return false;

    return true;
}
