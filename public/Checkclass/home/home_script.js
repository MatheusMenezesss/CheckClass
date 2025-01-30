// Função para adicionar uma nova turma
async function novaTurma() {
  const nomeTurma = prompt("Digite o nome da nova turma:");

  if (!nomeTurma) {
    alert("O nome da turma não pode estar vazio!");
    return;
  }

  const user = JSON.parse(localStorage.getItem("user"));

  if(!user || !user.id){
    alert("Erro: Nenhum professor identificado! Faça login novamente.");
    window.location.href = "../login/login_text.html";
    return;
  }

  //enviar dados ao backend

  try {
    const response = await fetch("http://localhost:3000/turmas", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        nome: nomeTurma,
        professor_id: user.id, // Associa ao professor logado
      }),
    });

    if (!response.ok) {
      throw new Error("Erro ao criar turma!");
    }

    const novaTurma = await response.json();

    // Criar visualmente a turma na interface
    const novaDiv = document.createElement("div");
    novaDiv.className = "turma-card";
    novaDiv.dataset.nome = nomeTurma;
    novaDiv.innerHTML = `<h2>${nomeTurma}</h2><p>Turma criada com sucesso!</p>`;

    novaDiv.addEventListener("click", () => abrirAbaTurma(nomeTurma));

    document.getElementById("turmas").appendChild(novaDiv);
  } catch (error) {
    console.error("Erro ao criar turma:", error);
    alert("Não foi possível criar a turma.");
  }
}

// Função para abrir a aba com detalhes da turma
function abrirAbaTurma(nomeTurma) {
  // Salvar o nome da turma no localStorage
  localStorage.setItem("turmaSelecionada", nomeTurma);

  // Abrir nova aba ou página
  window.open("../turmas/turmas_text.html", "_blank");
}

// Função para abrir o modal
function abrirModal() {
  const modal = document.getElementById("modalTurma");
  modal.classList.remove("hidden");
}

// Função para fechar o modal
function fecharModal() {
  const modal = document.getElementById("modalTurma");
  modal.classList.add("hidden");
  document.getElementById("formNovaTurma").reset();
  document.getElementById("listaAlunos").innerHTML = "";
}

// Carregar informações da ata
document.getElementById("ataAlunos").addEventListener("change", (event) => {
  const file = event.target.files[0];
  if (!file) return;

  const reader = new FileReader();

  reader.onload = function (e) {
    const content = e.target.result;
    const lines = content.split("\n");
    const listaAlunos = document.getElementById("listaAlunos");

    listaAlunos.innerHTML = "";

    lines.forEach((line, index) => {
      const [nome, matricula] = line.split(",");
      if (nome && matricula) {
        const li = document.createElement("li");
        li.textContent = `${nome.trim()} - Matrícula: ${matricula.trim()}`;
        listaAlunos.appendChild(li);
      } else if (index > 0 && line.trim()) {
        console.error(`Linha inválida na ata: ${line}`);
      }
    });
  };

  reader.readAsText(file); 
});

// Salvar nova turma
document.getElementById("formNovaTurma").addEventListener("submit", async (event) => {
  event.preventDefault();

  const user = JSON.parse(localStorage.getItem("user")); // Obtém o usuário logado
  if (!user || !user.id) {
    alert("Erro: usuário não autenticado.");
    return;
  }

  const nomeTurma = document.getElementById("nomeTurma").value;
  const disciplinaId = document.getElementById("disciplinaId").value; // Pega o ID da disciplina do formulário

  if (!nomeTurma || !disciplinaId) {
    alert("Todos os campos são obrigatórios!");
    return;
  }

  const turmaData = {
    nome: nomeTurma,
    disciplina_id: disciplinaId,
    professor_id: user.id, // Pega o ID do professor logado
  };

  try {
    const response = await fetch("/turmas", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(turmaData),
    });

    if (!response.ok) {
      throw new Error(await response.text());
    }

    alert("Turma criada com sucesso!");
    location.reload(); // Recarrega a página para atualizar a lista
  } catch (error) {
    console.error("Erro ao criar turma:", error);
    alert("Erro ao criar a turma.");
  }
});

document.querySelector(".ADD_TURMA").addEventListener("click", abrirModal);

document.addEventListener("DOMContentLoaded", () =>{
  const user = JSON.parse(localStorage.getItem("user"));

  if(!user){
    window.location.href = "../login/login_text.html";
    return;
  }

  document.getElementById("nomeUsuario").textContent = user.nome;

  document.getElementById("logout").addEventListener("click", () => {
    localStorage.removeItem("user");
    window.location.href = "../login/login_text.html";
  });
});
