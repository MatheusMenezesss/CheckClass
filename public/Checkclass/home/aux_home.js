document.addEventListener("DOMContentLoaded", () => {
    const formNovaTurma = document.getElementById("formNovaTurma");
  
    if (formNovaTurma) {
      formNovaTurma.addEventListener("submit", async (event) => {
        event.preventDefault(); // Evita o reload da página
  
        // Coleta os dados do formulário
        const nomeTurma = document.getElementById("nomeTurma").value;
        const disciplinaId = document.getElementById("disciplina").value;
        const professorId = document.getElementById("professor").value;
  
        // Cria o objeto a ser enviado para o servidor
        const novaTurma = {
          nome: nomeTurma,
          disciplina_id: parseInt(disciplinaId),
          professor_id: parseInt(professorId),
        };
  
        try {
          // Faz a requisição para a API
          const response = await fetch("/turmas", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify(novaTurma),
          });
  
          // Trata a resposta
          if (response.ok) {
            const resultado = await response.json();
            alert("Turma criada com sucesso! ID: " + resultado.id);
  
            // Opcional: Atualiza a lista de turmas na interface
            atualizarListaTurmas();
          } else {
            const erro = await response.json();
            alert("Erro ao criar turma: " + erro.message);
          }
        } catch (error) {
          console.error("Erro na requisição:", error);
          alert("Erro ao criar turma. Verifique sua conexão com a internet.");
        }
      });
    }
  
    // Função para atualizar a lista de turmas na interface (simulada)
    async function atualizarListaTurmas() {
      try {
        const response = await fetch("/turmas", {
          method: "GET",
        });
  
        if (response.ok) {
          const turmas = await response.json();
          const listaTurmas = document.getElementById("listaTurmas");
  
          if (listaTurmas) {
            listaTurmas.innerHTML = "";
  
            turmas.forEach((turma) => {
              const item = document.createElement("li");
              item.textContent = `${turma.id}: ${turma.nome} (Disciplina: ${turma.disciplina_id}, Professor: ${turma.professor_id})`;
              listaTurmas.appendChild(item);
            });
          }
        } else {
          console.error("Erro ao atualizar lista de turmas:", await response.json());
        }
      } catch (error) {
        console.error("Erro na atualização da lista de turmas:", error);
      }
    }
  
    // Opcional: Chamar a atualização da lista ao carregar a página
    atualizarListaTurmas();
  });
  