#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>

//keys of keyboard
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27

#define MENU_SIZE 7

//specs of math functions
#define a 0.00
#define b 2 * 3.1415
#define n 20

//colors for max/min
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_RESET   "\x1b[0m"

const char* menu_items[MENU_SIZE] = {
    "1. Таблица",
    "2. Графики",
    "3. Уравнения",
    "4. Интеграл",
    "5. Заставка",
    "6. Об авторе",
    "7. Выход"
};

// set color for menu (0 - no color, 1 - with color)
void SetColor(int k) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (k == 1) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    }
    else if (k == 0) {
        SetConsoleTextAttribute(hConsole, 7);
    }
}

// hide console cursor
void HideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

//function for pre-solve
void solve_func(double res[3][n])
{
    double step = fabs(b - a) / (n - 1);

    res[0][0] = a;

    for (int num = 0; num < n; num++)
    {
        res[1][num] = 5 - 3 * cos(res[0][num]);
        res[2][num] = sqrt(1 + pow(sin(res[0][num]), 2));
        res[0][num + 1] = res[0][num] + step;
    }
}

// draw table
void table_draw()
{
    double res[3][n];
    int min1, min2, max1, max2;

    printf(" ___________________________________________ \n");
    printf(" | I |     X    |     F1     |      F2     | \n");
    printf(" |---|----------|------------|-------------|\n");
    solve_func(res);

    min1 = min_table(res, 1);
    min2 = min_table(res, 2);
    max1 = max_table(res, 1);
    max2 = max_table(res, 2);

    //check for min/max and coloring
    for (int i = 0; i < n; i++)
    {
        printf(" |%3d| %8.3f |", i + 1, res[0][i]);
        
        if (i == min1)
        {
            printf(COLOR_RED);
        }
        else if (i == max1)
        {
            printf(COLOR_GREEN);
        }
        
        printf("%12.7f", res[1][i]);
        printf(COLOR_RESET);
        printf("|");

        if (i == min2)
        {
            printf(COLOR_RED);
        }
        else if (i == max2)
        {
            printf(COLOR_GREEN);
        }

        printf(" %12.8f", res[2][i]);
        printf(COLOR_RESET);
        printf("|\n");

        //printf(" |%3d| %8.3f |%12.7f| %12.8f|\n", i + 1, res[0][i], res[1][i], res[2][i]);
    }
    printf(" ------------------------------------------- \n");
    printf("Легенда: " COLOR_GREEN "Максимум " COLOR_RED "Минимум" COLOR_RESET);
}

//min and max for table_draw()
int min_table(double res[3][n], int indx) 
{
    int result;
    double min = pow(10, 5);

    for (int i = 0; i < n; i++)
    {
        if (res[indx][i] < min)
        {
            min = res[indx][i];
            result = i;
        }
    }
    return result;
}

int max_table(double res[3][n], int indx)
{
    int result;
    double max = 0;

    for (int i = 0; i < n; i++)
    {
        if (res[indx][i] > max)
        {
            max = res[indx][i];
            result = i;
        }
    }
    return result;
}

//draw a graph
void graph()
{
    BOOL first = TRUE;
    double res[3][n];
    solve_func(res);

    HWND hwn = GetConsoleWindow();
    RECT rect;
    HDC hdc = GetDC(hwn);
    GetClientRect(hwn, &rect);

    const int c = rect.right / 2, d = rect.bottom / 2, k = 100;

    HPEN pen = CreatePen(PS_SOLID, 2, RGB(218, 164, 32));
    HPEN pen2 = CreatePen(PS_SOLID, 3, RGB(135, 206, 250));

    //draw Ox and Oy
    SelectObject(hdc, pen);
    MoveToEx(hdc, 0, d, NULL);
    LineTo(hdc, c * k, d); // Ox
    MoveToEx(hdc, c, 0, NULL);
    LineTo(hdc, c, k * d); //Oy

    //draw graphs of F1 and F2
    for (int i = 0; i < n; i++)
    {
        if (first)  //draw a first point of graph
        {
            SelectObject(hdc, pen2);
            MoveToEx(hdc, c + k * res[0][i], d - k * res[1][i], NULL);
            first = FALSE;
        }
        else LineTo(hdc, c + k * res[0][i], d - k * res[1][i]);
    }

    _getch();
}

int main() {
    setlocale(LC_ALL, "rus");
    HideCursor();

    int selected = 0;
    int key;
    int running = 1;

    while (running)
    {
        system("cls"); // clean screen before output menu
        printf("  --- МЕНЮ ---\n\n");

        // draw menu with colors
        for (int i = 0; i < MENU_SIZE; i++)
        {
            if (i == selected) {
                SetColor(1);
                printf("  %s  \n", menu_items[i]);
                SetColor(0);   // return to normal color
            }
            else {
                printf("   %s    \n", menu_items[i]);
            }
        }

        printf("\nИспользуйте СТРЕЛКИ для навигации, ENTER для выбора.");

        // waiting for press key
        key = _getch();

        // processing keys
        if (key == 224 || key == 0) { // if press a spec-key
            key = _getch(); // reading key code
            switch (key) {
            case KEY_UP:
                selected--;
                if (selected < 0) selected = MENU_SIZE - 1; // looping down to up
                break;
            case KEY_DOWN:
                selected++;
                if (selected >= MENU_SIZE) selected = 0; // looping up to down
                break;
            }
        }
        else if (key == KEY_ENTER) { // if press ENTER
            system("cls"); // clear menu for show result
            SetColor(0);   // set normal color

            switch (selected)
            {
            case 0:
                printf("\n> Таблица значений:\n");
                table_draw();
                break;
            case 1:
                printf("\n> Выбран пункт: Графики\n");
                graph();
                break;
            case 2:
                printf("\n> Выбран пункт: Уравнение\n");
                break;
            case 3:
                printf("\n> Выбран пункт: Интеграл\n");
                break;
            case 4:
                printf("\n> Выбран пункт: Заставка\n");
                break;
            case 5:
                printf("\n> Выбран пункт: Автор\n");
                printf("> Автор: ст.гр. ИВТ-254 Дубровин А.А.\n");
                break;
            case 6:
                printf("\n> Выход из программы...\n");
                running = 0;
                break;
            }

            if (running) {
                printf("\nНажмите любую клавишу для возврата в меню...");
                _getch();
            }
        }
        else if (key == KEY_ESC) {
            running = 0; // fast escape on "ESC"
        }
    }

    return 0;
}