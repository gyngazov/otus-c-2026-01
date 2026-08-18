#include <stdio.h>
#include <stdbool.h>

#define DIM 3

// Функция для вывода игрового поля
void printBoard(char board[DIM][DIM]) {
    printf("");
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            printf(" %c ", board[i][j]);
            if (j < DIM - 1) printf("|");
        }
        printf("");
        if (i < DIM - 1) printf("---+---+---");
    }
    printf("");
}

// Функция для проверки победы
bool checkWin(char board[DIM][DIM], char symbol) {
    // Проверка строк
    for (int i = 0; i < DIM; i++) {
        if (board[i] == symbol && board[i] == symbol && board[i] == symbol) {
            return true;
        }
    }
    // Проверка столбцов
    for (int j = 0; j < DIM; j++) {
        if (board[j] == symbol && board[j] == symbol && board[j] == symbol) {
            return true;
        }
    }
    // Проверка диагоналей
    if (board == symbol && board == symbol && board == symbol) {
        return true;
    }
    if (board == symbol && board == symbol && board == symbol) {
        return true;
    }
    return false;
}
//[1](https://www.geeksforgeeks.org/c/tic-tac-toe-game-in-c/)
//[6](https://github.com/coding-pelican/tic-tac-toe-c)

// Функция для проверки ничьей
bool checkDraw(char board[DIM][DIM]) {
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            if (board[i][j] == ' ') {
                return false; // Есть пустая клетка
            }
        }
    }
    return true; // Поле заполнено без победителя
}

int main() {
    char board[DIM][DIM];
    // Инициализация поля пустыми пробелами
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            board[i][j] = ' ';
        }
    }

    char currentPlayer = 'X'; // Текущий игрок
    int movesLeft = DIM * DIM; // Количество оставшихся ходов

    while (movesLeft > 0 && !checkWin(board, currentPlayer) && !checkDraw(board)) {
        printBoard(board);
        int row, col;

        // Ввод хода и проверка на корректность
        printf("Player %c's turn. Enter row and column (0-2): ", currentPlayer);
        scanf("%d %d", &row, &col);

        // Проверка ввода: диапазон и ячейка должна быть пустой
        if (row >= 0 && row < DIM && col >= 0 && col < DIM && board[row][col] == ' ') {
            board[row][col] = currentPlayer;
        } else {
            printf("Invalid move. Try again.");
            continue;
        }

        movesLeft -= 2; // Каждый ход заполняет 2 клетки
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X'; // Смена игрока
    }

    printBoard(board);

    if (checkWin(board, currentPlayer)) {
        printf("Player %c wins!", currentPlayer);
    } else {
        printf("It's a draw!");
    }

    return 0;
}
