#pragma once

class DB
{
private:
    static constexpr char *s_DBStoragePath = "/db";
    static bool s_HasFiles;

    static bool LoadTables();
public:
    /// @brief Inicializa o sistema de arquivos e o banco de dados
    /// @param maxFiles número máximo de arquivos que podem ser manipulados ao mesmo tempo
    /// @return true se conseguir iniciar o sistema de banco de dados
    static bool Init(int maxFiles);

    /// @brief Usado para verificar se o banco de dados já possui os arquivos
    /// com os dados carregados
    /// @return true se já possui os arquivos do banco de dados 
    static inline bool HasFiles() { return s_HasFiles; }
    static bool CreateTable();
};