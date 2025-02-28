#include "db.h"
#include "fatfs.h"

bool DB::s_HasFiles = false;

bool DB::Init(int maxFiles)
{
    if (!FatFs::Init(s_DBStoragePath, maxFiles))
    {
        ESP_LOGE("DB", "Nao foi possivel iniciar o sistema de arquivos\n");
        return false;
    }

    if (FatFs::FileExists(s_DBStoragePath "/tables_descriptor.txt"))
    {
        s_HasFiles = true;
        return LoadTables();
    }

    //FILE *desc = FatFs::CreateFile(s_DBStoragePath "/tables_descriptor.txt");



    return true;
}

bool DB::LoadTables()
{
    return false;
}
