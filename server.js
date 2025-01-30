require("dotenv").config();
const express = require("express");
const mysql = require("mysql");
const cors = require("cors");
const session = require("express-session");
const path = require("path");
const bcrypt = require("bcrypt");

const app = express();
const PORT = process.env.PORT || 8080;

// Configuração do banco de dados
const db = mysql.createConnection({
  host: process.env.DB_HOST,
  user: process.env.DB_USER,
  password: process.env.DB_PASS,
  database: process.env.DB_NAME,
});

db.connect((err) => {
  if (err) {
    console.error("Erro ao conectar ao banco de dados:", err);
    return;
  }
  console.log("Conectado ao banco de dados");
});

// Middleware
app.use(cors());
app.use(express.json());

app.use( session({
    secret: process.env.SESSION_SECRET, // Defina uma chave forte no .env
    resave: false,
    saveUninitialized: true,
    cookie: { secure: false },
  })
);



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
//app.use(express.json());
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
  console.log("Recebendo requisição POST /login", req.body);

  const { email, senha } = req.body;

  const sql = "SELECT * FROM professor WHERE email = ?";
  db.query(sql, [email], async (err, results) => {
    if (err) {
      console.error("Erro ao buscar usuário:", err);
      return res.status(500).send("Erro no servidor.");
    }

    if (results.length === 0) {
      return res.status(404).send("Usuário ou senha inválidos.");
    }

    const user = results[0];
    const senhaCorreta = await bcrypt.compare(senha, user.senha);

    if (!senhaCorreta) {
      return res.status(401).send("Usuário ou senha inválidos.");
    }

    // Criando uma sessão ou token
    req.session.user = { id: user.id, nome: user.nome, email: user.email };
    
    res.status(200).json({ message: "Login bem-sucedido!", user: req.session.user });
  });
});

//Rota para visualizar um turma:
app.get("/turmas/:id", (req, res) => {
  const turmaId = req.params.id;

  const sql = `
    SELECT t.id, t.nome, d.nome AS disciplina, p.nome AS professor
    FROM turma t
    INNER JOIN disciplina d ON t.disciplina_id = d.id
    INNER JOIN professor p ON t.professor_id = p.id
    WHERE t.id = ?
  `;

  db.query(sql, [turmaId], (err, result) => {
    if(err){
      console.log("Erro ao buscar turma:", err);
      return res.status(500).send("Err ao buscar turma.");
    }
    if ( result.length === 0) {
      return res.status(404).send("Turma não encontrada.");
    }
    res.status(200).json(result[0]);
  });
});

//Rota para listar alunos de uma turma específica:

app.get("/turmas/:id/alunos", (req, res) =>{
  const turmaId = req.params.id;

  const sql = `SELECT * FROM aluno WHERE turma_id = ?`

  db.query(sql, [turmaId], (err, result) => {
    if(err){
      console.error("Erro ao buscar alunos:", err);
      return res.status(500).send("Erro ao buscar alunos.");
    }
    res.status(200).json(results);
  });
});

//Rota para buscar ata da turma
app.get("/turma/:id/ata", (req, res) =>{
  const turmaId = req.params.id;

  const sql = `
    SELECT a.id, a.data, aconteudo
    FROM ata a
    WHERE a.turma_id = ?
  `;

  db.query(sql, [turmaId], (err, results) => {
    if(err){
      console.error("Erro ao buscar ata:", err);
      return res.status(500).send("erro ao buscar ata.");
    }
    res.status(200).json(results);
  });
});

// Rota para criar turmas:
app.post("/turmas", (req, res) => {
  const { nome, disciplina_id } = req.body;
  const professor_id = req.user.id; // Obtém o ID do professor autenticado (supondo que use autenticação JWT ou sessão)

  if (!nome || !disciplina_id || !professor_id) {
    return res.status(400).send("Todos os campos são obrigatórios.");
  }

  const sql = `
    INSERT INTO turma (nome, disciplina_id, professor_id)
    VALUES (?, ?, ?)
  `;
  db.query(sql, [nome, disciplina_id, professor_id], (err, result) => {
    if (err) {
      console.error("Erro ao criar uma turma:", err);
      return res.status(500).send("Erro ao criar turma.");
    }
    res.status(201).send("Turma criada com sucesso");
  });
});


//Rota para lsitar turma de um prof especifico:
app.get("/professores/:id/turmas", (req, res) => {
  const professorId = req.params.id;
  const sql = `
    SELECT t.id, t.nome, d.nome AS disciplina, t.create_at
    FROM turma t
    INNER JOIN disciplina d ON t.disciplina_id = d.id
    WHERE t.professor_id = ?
  `;

  db.query(sql, [professorId], (err, results) => {
    if(err){
      console.error("Erro ao buscar turmas:", err);
      return res.status(500).send("Erro ao buscar turmas.");
    }
    res.status(200).json(results);
  });
});

app.get("/dados-usuario", (req, res) => {
  if (!req.session.user) {
    return res.status(401).send("Não autorizado");
  }
  res.status(200).json(req.session.user);
});


app.listen(PORT, () => {
  console.log(`Servidor rodando em http://localhost:${PORT}`);
});
