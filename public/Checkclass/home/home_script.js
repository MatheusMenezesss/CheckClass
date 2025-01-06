// Função para adicionar uma nova turma
function novaTurma() {
  const nomeTurma = prompt("Digite o nome da nova turma:");

  if (!nomeTurma) {
    alert("O nome da turma não pode estar vazio!");
    return;
  }

  const novaDiv = document.createElement("div");
  novaDiv.className = "turma-card";
  novaDiv.dataset.nome = nomeTurma; // Adiciona o nome como atributo
  novaDiv.innerHTML = `
    <h2>${nomeTurma}</h2>
    <p>Indicador 1</p>
    <p>Indicador 2</p>
  `;

  // Adiciona evento de clique no card
  novaDiv.addEventListener("click", () => abrirAbaTurma(nomeTurma));

  const container = document.getElementById("turmas");
  container.appendChild(novaDiv);
}

// Função para abrir a aba com detalhes da turma
function abrirAbaTurma(nomeTurma) {
  // Salvar o nome da turma no localStorage
  localStorage.setItem("turmaSelecionada", nomeTurma);

  // Abrir nova aba ou página
  window.open("detalhes_turma.html", "_blank");
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
document.getElementById("formNovaTurma").addEventListener("submit", (event) => {
  event.preventDefault();

  const nomeTurma = document.getElementById("nomeTurma").value;
  const alunos = Array.from(document.getElementById("listaAlunos").children).map(
    (li) => li.textContent
  );

  if (!nomeTurma) {
    alert("O nome da turma não pode estar vazio!");
    return;
  }

  if (alunos.length === 0) {
    alert("Nenhum aluno foi carregado na turma!");
    return;
  }

  const novaDiv = document.createElement("div");
  novaDiv.className = "turma-card";
  novaDiv.dataset.nome = nomeTurma; // Adiciona o nome como atributo
  novaDiv.innerHTML = `
    <h2>${nomeTurma}</h2>
    <p>Total de Alunos: ${alunos.length}</p>
  `;

  novaDiv.addEventListener("click", () => abrirAbaTurma(nomeTurma));

  const container = document.getElementById("turmas");
  container.appendChild(novaDiv);

  fecharModal();
});

document.querySelector(".ADD_TURMA").addEventListener("click", abrirModal);
