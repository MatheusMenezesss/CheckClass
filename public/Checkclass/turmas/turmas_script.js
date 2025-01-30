document.addEventListener("DOMContentLoaded", function () {
    const turmaId = 1; // Defina o ID da turma conforme necessário

    // Função para carregar informações da turma
    function carregarTurma() {
        fetch(`/turmas/${turmaId}`)
            .then(response => response.json())
            .then(data => {
                document.querySelector(".info-turma").innerHTML = `
                    <h2>Informações da Turma</h2>
                    <p><strong>Nome:</strong> ${data.nome}</p>
                    <p><strong>Professor:</strong> ${data.professor}</p>
                    <p><strong>Disciplina:</strong> ${data.disciplina}</p>
                `;
            })
            .catch(error => console.error("Erro ao carregar turma:", error));
    }

    // Função para carregar ata da turma
    function carregarAta() {
        fetch(`/turmas/${turmaId}/ata`)
            .then(response => response.json())
            .then(data => {
                const tabela = document.querySelector(".ata-turma tbody");
                tabela.innerHTML = "";
                data.forEach(ata => {
                    tabela.innerHTML += `
                        <tr>
                            <td>${ata.data}</td>
                            <td>${ata.nome}</td>
                        </tr>
                    `;
                });
            })
            .catch(error => console.error("Erro ao carregar ata:", error));
    }

    // Evento para iniciar chamada
    document.querySelector("button").addEventListener("click", function () {
        alert("Iniciando chamada...");
        // Aqui você pode chamar outra função para iniciar a chamada
    });

    // Evento para conectar (ação ainda não definida)
    document.querySelectorAll("button")[1].addEventListener("click", function () {
        alert("Conectando...");
        // Aqui você pode definir a lógica para conectar
    });

    carregarTurma();
    carregarAta();
});
