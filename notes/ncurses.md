# 🧰 Guia de utilização da biblioteca ncurses do Gepeto

✅ 1. Instalação (se ainda não tiver) no Ubuntu/Debian:
```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```
---

🏗️ 2. Compilando seu programa
```bash
gcc meu_programa.c -o meu_programa -lncurses #-lncurses é obrigtória para linkar com a lib
```
---

🌀 3. Fluxo básico de um programa com ncurses
1) Inicializa a biblioteca (initscr()).
2) Configura o terminal (eco, buffer, cursores, etc).
3) Cria e manipula janelas, escreve/posiciona texto.
4) Lê entrada do usuário.
5) Finaliza com endwin().

---

✍️ 4. Escrevendo e posicionando texto
```c
initscr();              // Inicializa ncurses
printw("Olá mundo!");   // Escreve na posição atual do cursor
mvprintw(5, 10, "Texto na linha 5, coluna 10"); // Move e escreve
refresh();              // Atualiza a tela
getch();                // Espera uma tecla
endwin();               // Finaliza ncurses
```
---

🕹️ 5. Lidando com entrada de teclado
```c
noecho();               // Não mostrar teclas digitadas
cbreak();               // Lê tecla imediatamente (sem Enter)
keypad(stdscr, TRUE);   // Ativa teclas especiais (setas, F1...)

int ch = getch();
if (ch == KEY_UP) { /* seta pra cima */ }
```
---

🎨 6. Usando cores
```c
start_color();                  // Ativa o modo de cores
init_pair(1, COLOR_RED, COLOR_BLACK);  // Par 1: texto vermelho, fundo preto

attron(COLOR_PAIR(1));         // Ativa o par de cores
printw("Texto colorido!\n");
attroff(COLOR_PAIR(1));        // Desativa o par
```
---

🪟 7. Criando janelas (subáreas da tela)
```c
WINDOW *win = newwin(10, 20, 5, 5);  // (linhas, colunas, y, x)
box(win, 0, 0);                      // Borda simples
wprintw(win, "Janela!");
wrefresh(win);                      // Atualiza só a janela

getch();
delwin(win);                        // Destroi a janela
```
---

🧩 8. Funções extras úteis

Função	                Descrição
clear()	                Limpa a tela
erase()            	    Apaga sem atualizar
curs_set(0)	            Oculta o cursor
timeout(ms)	            Espera tempo limitado por tecla
nodelay(stdscr, TRUE)	Não bloqueia no getch()
getmaxyx(win, y, x)	    Obtém tamanho da janela

---

🔧 Principais funcionalidades
🔧 Categoria -> Funções comuns -> Descrição rápida
🧱 Inicialização -> initscr(), endwin() -> Prepara/finaliza o uso da tela com ncurses.
👀 Entrada/saída -> printw(), scanw(), getch() -> Escreve e lê do terminal como printf/scanf, mas com controle de tela.
🎯 Posicionamento -> move(y, x), mvprintw(y, x, ...) -> Move o cursor para uma posição antes de escrever.
🎨 Cores e atributos -> start_color(), init_pair(), attron(), attroff() -> Ativa cores e efeitos (negrito, reverso, etc.).
📦 Janelas -> newwin(), delwin(), wprintw(), wrefresh() -> Divide a tela em múltiplas áreas independentes.
🖱️ Teclas especiais -> keypad(win, TRUE) -> Permite ler setas, F1–F12, etc., como constantes (KEY_UP, KEY_LEFT, etc.).
🔁 Atualização da tela -> refresh(), clear(), erase() -> Controla o que é mostrado no terminal.
🕹️ Controle de cursor -> noecho(), cbreak(), curs_set(0) -> Liga/desliga eco de teclas, modo raw, cursor invisível etc.
⌛ Timeout e espera -> timeout(ms), nodelay() -> Controla o comportamento de espera por teclas.
🧾 Menus, forms, panels -> (via libs extras) -> Permite criar menus com navegação, formulários, e sobreposição de janelas.