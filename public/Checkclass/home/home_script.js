// Função para adicionar uma nova turma
function novaTurma() {
  // 1. Capturar o nome da turma
  const nomeTurma = prompt("Digite o nome da nova turma:");
  
  // Verificar se o nome não está vazio
  if (!nomeTurma) {
    alert("O nome da turma não pode estar vazio!");
    return;
  }

  // 2. Criar o elemento HTML da turma
  const novaDiv = document.createElement("div");
  novaDiv.className = "turma-card";
  novaDiv.innerHTML = `
    <h2>${nomeTurma}</h2>
    <p>Indicador 1</p>
    <p>Indicador 2</p>
  `;

  // 3. Adicionar o card de turma no container
  const container = document.getElementById("turmas");
  container.appendChild(novaDiv);
}
