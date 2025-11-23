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
#define b (2 * 3.1415)
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

    for (int num = 0; num < n; num++)
    {
        res[0][num] = a + num * step;
        res[1][num] = 5 - 3 * cos(res[0][num]);
        res[2][num] = sqrt(1 + pow(sin(res[0][num]), 2));
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
    int result = 0;
    double min = res[indx][0];

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
    int result = 0;
    double max = res[indx][0];

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

//translate cords from math area to pixel area  
long map_val(double val, double min_v, double max_v, long p_min, long p_max) {
    if (fabs(max_v - min_v) < 1e-9) {
        return (p_min + p_max) / 2;
    }
    double t = (val - min_v) / (max_v - min_v);
    return p_min + (long)(t * (p_max - p_min));
}

//draw a graph
void graph()
{
    double res[3][n];
    solve_func(res);

    HWND hwn = GetConsoleWindow();
    RECT rect;
    HDC hdc = GetDC(hwn);
    GetClientRect(hwn, &rect);

    // padding inside client area
    int pad_x = rect.right / 10;
    int pad_y = rect.bottom / 10;

    int px_left = rect.left + pad_x;
    int px_right = rect.right - pad_x;
    int py_top = rect.top + pad_y;
    int py_bottom = rect.bottom - pad_y;

    system("color 0F");

    // compute x and y ranges
    double xmin = res[0][0];
    double xmax = res[0][n - 1];

    double ymin = res[1][0];
    double ymax = res[1][0];

    for (int i = 0; i < n; ++i) {
        if (res[1][i] < ymin) ymin = res[1][i];
        if (res[1][i] > ymax) ymax = res[1][i];
        if (res[2][i] < ymin) ymin = res[2][i];
        if (res[2][i] > ymax) ymax = res[2][i];
    }

    // small margin
    double margin = (ymax - ymin) * 0.1;
	if (margin < 1e-9) margin = 1.0; // save from zero range
    ymin -= margin;
    ymax += margin;

    HPEN pen_axis = CreatePen(PS_SOLID, 2, RGB(218, 164, 32));
    HPEN pen_grid = CreatePen(PS_DOT, 1, RGB(50, 50, 50));
    HPEN pen_f1 = CreatePen(PS_SOLID, 3, RGB(135, 206, 250));
	HPEN pen_f2 = CreatePen(PS_SOLID, 3, RGB(255, 105, 180));

    SetBkMode(hdc, TRANSPARENT);

    int steps = 10;
    char buf[32];

	// draw Y axis labels and grid lines
    SetTextAlign(hdc, TA_RIGHT | TA_TOP);
    SetTextColor(hdc, RGB(200, 200, 200));

    for (int i = 0; i <= steps; i++) {
        double val = ymin + (ymax - ymin) * i / steps;
        long py = map_val(val, ymin, ymax, py_bottom, py_top);

		// draw grid line
        if (i > 0 && i < steps) {
            SelectObject(hdc, pen_grid);
            MoveToEx(hdc, px_left, py, NULL);
            LineTo(hdc, px_right, py);
        }

		// draw tick mark
        SelectObject(hdc, pen_axis);
        MoveToEx(hdc, px_left - 5, py, NULL);
        LineTo(hdc, px_left, py);

		// output label
        sprintf_s(buf, 32, "%.2f", val);
        TextOutA(hdc, px_left - 8, py - 8, buf, strlen(buf));
    }

	SetTextAlign(hdc, TA_CENTER | TA_TOP); // text alignment for X axis

    for (int i = 0; i <= steps; i++) {
        double val = xmin + (xmax - xmin) * i / steps;
        long px = map_val(val, xmin, xmax, px_left, px_right);

        // grid
        if (i > 0 && i < steps) {
            SelectObject(hdc, pen_grid);
            MoveToEx(hdc, px, py_bottom, NULL);
            LineTo(hdc, px, py_top);
        }

		// grid tick
        SelectObject(hdc, pen_axis);
        MoveToEx(hdc, px, py_bottom, NULL);
        LineTo(hdc, px, py_bottom + 5);

		// label
        sprintf_s(buf, 32, "%.1f", val);
        TextOutA(hdc, px, py_bottom + 8, buf, strlen(buf));
    }

    // draw axes (X and Y)
    HPEN oldPen = (HPEN)SelectObject(hdc, pen_axis);

    MoveToEx(hdc, px_left, py_top, NULL);
    LineTo(hdc, px_left, py_bottom);  // О Y
    LineTo(hdc, px_right, py_bottom); // О X

    //draw graphs of F1 and F2
    POINT points[n];

    // F1
    SelectObject(hdc, pen_f1);
    for (int i = 0; i < n; i++) {
        points[i].x = map_val(res[0][i], res[0][0], res[0][n - 1], px_left, px_right);
        points[i].y = map_val(res[1][i], ymin, ymax, py_bottom, py_top);
    }
    Polyline(hdc, points, n);

    // F2
    SelectObject(hdc, pen_f2);
    for (int i = 0; i < n; i++) {
        points[i].x = map_val(res[0][i], res[0][0], res[0][n - 1], px_left, px_right);
        points[i].y = map_val(res[2][i], ymin, ymax, py_bottom, py_top);
    }
    Polyline(hdc, points, n);

    // output text of functions
    SetBkMode(hdc, TRANSPARENT);

    SetTextColor(hdc, RGB(135, 206, 250));
    TextOutA(hdc, px_left + 300, py_top, "F1(х) = 5 - 3cos(x)", 19);
    
    SetTextColor(hdc, RGB(255, 105, 180));
    TextOutA(hdc, px_left + 300, py_top + 300, "F2(x) = sqrt(1 + sin^2(x))", 26);

    // restore and cleanup
    SelectObject(hdc, oldPen);
    DeleteObject(pen_axis);
    DeleteObject(pen_f1);
    DeleteObject(pen_f2);
    ReleaseDC(hwn, hdc);
    
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