#include <ncurses.h>  //  flag -lcurses
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXROW 25
#define MAXCOL 80
#define LIVE '*'
#define DIE ' '

char **initMatrix(int row, int col);
int fileOpen(char **matrix, char *filename);
void printwild(char **matrix, int row, int col);
int countNeighbours(char **matrix, int i, int j);
void newGeneration(char **matrix);
void printwild(char **matrix, int row, int col);
int isEmty(char **matrix);
int checkEqual(char **matrix, char **currentMatrix);
void makeCopy(char **matrix, char **new);

int main() {
    char **currentMatrix = NULL;
    char **matrix = initMatrix(MAXROW, MAXCOL);
    initscr();  // Начало интерактивного режима
    nodelay(stdscr, TRUE);
    int menu;
    printw("%15s", "Menu\n");
    printw(
        "Choose a Patern to Launch \n\n 1 - Gaspera Rifle\n\n 2 - Cube\n\n 3 - GeeGee\n\n 4 - Space Ship\n\n "
        "5 "
        "- Flight\n");
    printw("\nManagement: speed <+> / <->\n");
    printw("\nExit: <q>\n");
    refresh();
    if (scanf("%d", &menu) == 1 && menu > 0 && menu <= 5) {
        int startSpeed = 100000;
        if (menu == 1) fileOpen(matrix, "gun_gusper.txt");
        if (menu == 2) fileOpen(matrix, "cube.txt");
        if (menu == 3) fileOpen(matrix, "geegee.txt");
        if (menu == 4) fileOpen(matrix, "space_ship.txt");
        if (menu == 5) fileOpen(matrix, "lll.txt");
        //  Крутить цикл пока не будет выполнено одно из условий
        int status = 1;
        while (status == 1) {
            char s = getch();
            if (isEmty(matrix) == 1) {
                usleep(3000000);
                status = -1;
            }
            if (s == 'q') status = -1;
            if (s == '+' || s == '=') {
                if (startSpeed > 30000) startSpeed -= 10000;
            }
            if (s == '-') {
                if (startSpeed < 150000) startSpeed += 10000;
            }
            currentMatrix = initMatrix(MAXROW, MAXCOL);
            printwild(matrix, MAXROW, MAXCOL);
            makeCopy(matrix, currentMatrix);
            newGeneration(matrix);
            if (checkEqual(matrix, currentMatrix) == 1) {
                usleep(3000000);
                status = -1;
            }
            usleep(startSpeed);
            free(currentMatrix);
            refresh();
        }
    } else {
        printw("Вы ввели не существующее значение");
        refresh();
    }
    getch();
    nodelay(stdscr, FALSE);
    endwin();  //  Коцен интерактивного режима
    free(matrix);
    return 0;
}
//  Выделяем память под матрицу
char **initMatrix(int row, int col) {
    char **matrix = malloc(row * col * sizeof(char) + row * sizeof(char *));
    char *ptr = (char *)(matrix + row);
    for (int i = 0; i < row; i++) {
        matrix[i] = ptr + col * i;
    }
    return matrix;
}
//  Читаем из txt файла посимвольно и заполняем матрицу данными из файла
int fileOpen(char **matrix, char *filename) {
    int status = 0;
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printw("Не удалось открыть файл");
        getchar();

    } else {
        char c;
        int i = 0;
        int j = 0;
        while ((c = fgetc(fp)) != EOF && i < MAXROW) {
            if (c == '\n') continue;
            matrix[i][j] = c;
            j++;
            if (j == MAXCOL) {
                j = 0;
                i++;
            }
        }
        fclose(fp);
    }
    return status;
}
//  Печатаем поле
void printwild(char **matrix, int row, int col) {
    clear();
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (matrix[i][j] == LIVE)
                printw("%c", '*');
            else
                printw(" ");
        }
        printw("\n");
    }
    refresh();
}

//  Ищем соседей, если сосед находится в верхней или крайней ячейке, смотрим соседей с обратной стороны поля
int countNeighbours(char **matrix, int i, int j) {
    int count = 0;
    int leftSide = (j == 0) ? MAXCOL - 1 : j - 1;
    int rightSide = (j == MAXCOL - 1) ? 0 : j + 1;
    int topSide = (i == 0) ? MAXROW - 1 : i - 1;
    int bottomSide = (i == MAXROW - 1) ? 0 : i + 1;
    if (matrix[topSide][leftSide] == LIVE) count++;
    if (matrix[topSide][j] == LIVE) count++;
    if (matrix[topSide][rightSide] == LIVE) count++;
    if (matrix[i][rightSide] == LIVE) count++;
    if (matrix[bottomSide][rightSide] == LIVE) count++;
    if (matrix[bottomSide][j] == LIVE) count++;
    if (matrix[bottomSide][leftSide] == LIVE) count++;
    if (matrix[i][leftSide] == LIVE) count++;

    return count;
}

// Генерация нового поколения
void newGeneration(char **matrix) {
    char new[MAXROW][MAXCOL];
    for (int i = 0; i < MAXROW; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            int test = countNeighbours(matrix, i, j);
            if (test == 3)
                new[i][j] = LIVE;
            else if (matrix[i][j] == LIVE && test == 2)
                new[i][j] = LIVE;
            else
                new[i][j] = DIE;
        }
    }
    for (int i = 0; i < MAXROW; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            matrix[i][j] = new[i][j];
        }
    }
}

//  Если матрица пуста
int isEmty(char **matrix) {
    int count = 0;
    int flag = 0;
    for (int i = 0; i < MAXROW; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            if (matrix[i][j] == LIVE) count++;
        }
    }
    if (count == 0) flag = 1;
    return flag;
}

//  Копирование матрицы для сравнения состояний
void makeCopy(char **matrix, char **new) {
    for (int i = 0; i < MAXROW; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            new[i][j] = matrix[i][j];
        }
    }
}

int checkEqual(char **matrix, char **currentMatrix) {
    int status = 1;
    for (int i = 0; i < MAXROW && status == 1; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            if (matrix[i][j] != currentMatrix[i][j]) {
                status = 0;
            }
        }
    }
    return status;
}