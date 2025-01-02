const express = require("express");
const path = require("path");

const app = express();

// Servir arquivos estáticos da pasta 'public'
app.use(express.static(path.join(__dirname, "public")));

// Porta onde o servidor será executado
const PORT = 3000;
app.listen(PORT, () => {
    console.log(`Servidor rodando em http://localhost:${PORT}`);
});
