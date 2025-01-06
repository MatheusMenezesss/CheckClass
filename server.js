const express = require("express");
const path = require("path");
const mysql = require("mysql");
const bodyParser = require("body-parser");
const bcrypt = require("bcrypt");

const app = express();

// Configurar o banco de dados MySQL
const db = mysql.createConnection({
  host: "localhost",
  user: "root", 
  password: "Math270603@", 
  database: "check_class",
});

// Conectar ao banco de dados
db.connect((err) => {
  if (err) {
    console.error("Erro ao conectar ao banco de dados: " + err.message);
    return;
  }
  console.log("Conectado ao banco de dados MySQL!");
});

app.use(bodyParser.json());


app.post("/professores", (req, res) => {
    const { nome, email } = req.body;
    const sql = "INSERT INTO professor (nome, email) VALUES (?, ?)";
    db.query(sql, [nome, email], (err, result) => {
      if (err) {
        console.error("Erro ao inserir professor:", err);
        res.status(500).send("Erro ao inserir professor.");
      } else {
        res.status(200).send("Professor adicionado com sucesso!");
      }
    });
  });

  app.get("/aluno/:turmaId", (req, res) => {
    const turmaId = req.params.turmaId; // Obtém o ID da turma a partir da URL
    const sql = "SELECT * FROM aluno WHERE turma_id = ?";
  
    db.query(sql, [turmaId], (err, results) => {
      if (err) {
        console.error("Erro ao buscar alunos:", err);
        res.status(500).send("Erro ao buscar alunos.");
      } else {
        res.status(200).json(results); // Retorna a lista de alunos em formato JSON
      }
    });
  });

// Servir arquivos estáticos da pasta 'public'
app.use(express.json());
app.use(express.static(path.join(__dirname, "public")));


//rota para registrar usuário no mysql
app.post("/register", async (req, res) => {
  const { nome, email, senha } = req.body;

  try {
    // Gerar o hash da senha
    const hashedPassword = await bcrypt.hash(senha, 10);

    // Inserir o nome, email e senha do professor no banco de dados
    const sql = "INSERT INTO professor (nome, email, senha) VALUES (?, ?, ?)";
    db.query(sql, [nome, email, hashedPassword], (err) => {
      if (err) {
        console.error("Erro ao registrar usuário:", err);
        res.status(500).send("Erro ao registrar usuário.");
      } else {
        res.status(200).send("Usuário registrado com sucesso!");
      }
    });
  } catch (error) {
    console.error("Erro ao processar a solicitação:", error);
    res.status(500).send("Erro ao processar a solicitação.");
  }
});

// Rota para login
app.post("/login", (req, res) => {
  const { email, senha } = req.body;

  const sql = "SELECT * FROM professor WHERE email = ? AND senha = ?";
  db.query(sql, [email, senha], (err, results) => {
    if (err) {
      console.error("Erro ao buscar usuário:", err);
      res.status(500).send("Erro no servidor.");
    } else if (results.length === 0) {
      // Caso nenhum usuário seja encontrado
      res.status(404).send("Usuário ou senha inválidos.");
    } else {
      // Caso o login seja bem-sucedido
      res.status(200).send("Login bem-sucedido!");
    }
  });
});


// Porta onde o servidor será executado
const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Servidor rodando em http://localhost:${PORT}`);
});
