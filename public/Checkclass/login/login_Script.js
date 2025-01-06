document.querySelector("form").addEventListener("submit", async (e) => {
    e.preventDefault();

    const email = document.querySelector("input[type=email]").value;
    const senha = document.querySelector("input[type=password]").value;

    try {
        const response = await fetch("/login", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ email, senha }),
        });

        if (response.ok) {
            // Login bem-sucedido
            alert("Login bem-sucedido!");
            window.location.href = "home/home_text.html"; // Redirecionar para a página inicial
        } else {
            const message = await response.text(); // Ler mensagem de erro do backend
            alert(message);
        }
    } catch (error) {
        console.error("Erro:", error);
        alert("Erro no servidor.");
    }
});

document.addEventListener("DOMContentLoaded", () => {
    const loginForm = document.getElementById("loginForm");
    const registerForm = document.getElementById("registerForm");

    const showRegisterLink = document.getElementById("showRegister");
    const showLoginLink = document.getElementById("showLogin");

    showRegisterLink.addEventListener("click", (e) => {
        e.preventDefault();
        loginForm.style.display = "none";
        registerForm.style.display = "block";
    });

    showLoginLink.addEventListener("click", (e) => {
        e.preventDefault();
        registerForm.style.display = "none";
        loginForm.style.display = "block";
    });
});

document.getElementById("registerForm").addEventListener("submit", async (e) => {
    e.preventDefault();

    // Capturar os valores dos campos
    const nome = document.getElementById("nome").value;
    const email = document.getElementById("email").value;
    const senha = document.getElementById("senha").value;

    try {
        // Enviar os dados para o backend
        const response = await fetch("/register", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ nome, email, senha }), // Incluir o nome aqui
        });

        if (response.ok) {
            // Cadastro bem-sucedido
            alert("Cadastro realizado com sucesso!");
            document.getElementById("registerForm").reset(); // Limpar o formulário
            document.getElementById("showLogin").click(); // Mostrar formulário de login
        } else {
            const message = await response.text(); // Ler mensagem de erro do backend
            alert(message);
        }
    } catch (error) {
        console.error("Erro:", error);
        alert("Erro no servidor.");
    }
});