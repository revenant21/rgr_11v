#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <windows.h>
#include <conio.h>

// Константы и настройки
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define MENU_SIZE 7
#define EQUAL_MENU_SIZE 4
#define INTEGRAL_MENU_SIZE 5

#define a 0.00
#define b (2 * 3.1415)
#define n 20

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_RESET   "\x1b[0m"

const char* menu_items[] = {
    "\t\t\t1. Таблица", "\t\t\t2. Графики", "\t\t\t3. Уравнения", "\t\t\t4. Интеграл",
    "\t\t\t5. Заставка", "\t\t\t6. Об авторе", "\t\t\t7. Выход"
};
const char* equal_menu_items[] = {
    "\t\t\t1. Метод бисекции", "\t\t\t2. Метод хорд", "\t\t\t3. Метод Ньютона", "\t\t\t4. Выход"
};

// --- Служебные функции ---

void HideCursor() {
    CONSOLE_CURSOR_INFO info = { 100, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void SetColor(int k) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (k == 1) ? FOREGROUND_GREEN : 7);
}

int handle_menu(const char** items, int size, const char* title) {
    int selected = 0, key;
    while (1) {
        system("cls");
        printf("\n\n\n\t\t\t   --- %s ---\n\n", title);
        for (int i = 0; i < size; i++) {
            if (i == selected) SetColor(1);
            printf(" %s  \n", items[i]);
            if (i == selected) SetColor(0);
        }
        key = _getch();
        if (key == 224 || key == 0) {
            key = _getch();
            if (key == KEY_UP) selected = (selected - 1 < 0) ? size - 1 : selected - 1;
            if (key == KEY_DOWN) selected = (selected + 1 >= size) ? 0 : selected + 1;
        }
        else if (key == KEY_ENTER) return selected;
    }
}

long map_val(double v, double min_v, double max_v, long p_min, long p_max) {
    return (max_v - min_v < 1e-9) ? (p_min + p_max) / 2 : p_min + (long)((v - min_v) / (max_v - min_v) * (p_max - p_min));
}

void solve_table(double res[3][n]) {
    double step = fabs(b - a) / (n - 1);
    for (int i = 0; i < n; i++) {
        res[0][i] = a + i * step;
        res[1][i] = 5 - 3 * cos(res[0][i]);
        res[2][i] = sqrt(1 + pow(sin(res[0][i]), 2));
    }
}

double f(double x) { return 2 * x * log10(x) - 3; }
double df(double x) { return 2 * log10(x) + 2.0 / log(10.0); }

void table_draw() {
    double res[3][n];
    int min1 = 0, max1 = 0, min2 = 0, max2 = 0;

    solve_table(res);
    for (int i = 0; i < n; i++) {
        if (res[1][i] < res[1][min1]) min1 = i; if (res[1][i] > res[1][max1]) max1 = i;
        if (res[2][i] < res[2][min2]) min2 = i; if (res[2][i] > res[2][max2]) max2 = i;
    }

    printf("\n\t\t\t ___________________________________________ \n");
    printf("\t\t\t | n |     x    |    F1(x)   |    F2(x)    | \n");
    printf("\t\t\t |---|----------|------------|-------------|\n");

    for (int i = 0; i < n; i++) {
        printf("\t\t\t |%3d| %8.6f |", i + 1, res[0][i]);

        printf((i == min1) ? COLOR_RED : (i == max1) ? COLOR_GREEN : "");
        printf("%12.7f" COLOR_RESET "|", res[1][i]);

        printf((i == min2) ? COLOR_RED : (i == max2) ? COLOR_GREEN : "");
        printf(" %12.8f" COLOR_RESET "|\n", res[2][i]);
    }
    printf("\t\t\t ___________________________________________ \n");
    printf("\t\t\tЛегенда: " COLOR_GREEN "Максимум " COLOR_RED "Минимум" COLOR_RESET);
}

void graph() {
    double res[3][n];
    double ymin = 1e9, ymax = -1e9;

    for (int i = 0; i < n; i++) {
        solve_table(res);
        if (res[1][i] < ymin) ymin = res[1][i];
        if (res[1][i] > ymax) ymax = res[1][i];
        if (res[2][i] < ymin) ymin = res[2][i];
        if (res[2][i] > ymax) ymax = res[2][i];
    }
    double margin = (ymax - ymin) * 0.1;
    if (margin < 1e-9) margin = 1.0;
    ymin -= margin; ymax += margin;

    HWND hwn = GetConsoleWindow();
    RECT rect; GetClientRect(hwn, &rect);
    HDC hdc = GetDC(hwn);

    int pad_x = rect.right / 10, pad_y = rect.bottom / 10;
    int px_left = rect.left + pad_x, px_right = rect.right - pad_x;
    int py_top = rect.top + pad_y, py_bottom = rect.bottom - pad_y;

    system("color 0F");
    HPEN pen_axis = CreatePen(PS_SOLID, 2, RGB(255, 160, 0));
    HPEN pen_f1 = CreatePen(PS_SOLID, 3, RGB(0, 191, 255));
    HPEN pen_f2 = CreatePen(PS_SOLID, 3, RGB(255, 80, 155));

    SelectObject(hdc, pen_axis);
    SetBkMode(hdc, TRANSPARENT);
    SetTextAlign(hdc, TA_CENTER | TA_TOP);

    char buf[32];
    int steps = 10;

    for (int i = 0; i <= steps; i++) {
        double valY = ymin + (ymax - ymin) * i / steps;
        long py = map_val(valY, ymin, ymax, py_bottom, py_top);
        sprintf_s(buf, 32, "%.1f", valY);
        SetTextColor(hdc, RGB(255, 255, 255));
        TextOutA(hdc, px_left - 20, py - 8, buf, strlen(buf));

        double valX = res[0][0] + (res[0][n - 1] - res[0][0]) * i / steps;
        long px = map_val(valX, res[0][0], res[0][n - 1], px_left, px_right);
        sprintf_s(buf, 32, "%.1f", valX); 
        SetTextColor(hdc, RGB(255, 255, 255));
        TextOutA(hdc, px, py_bottom + 5, buf, strlen(buf));
    }

    SelectObject(hdc, pen_axis);
    MoveToEx(hdc, px_left, py_top, NULL);
    LineTo(hdc, px_left, py_bottom);
    LineTo(hdc, px_right, py_bottom);

    SetTextColor(hdc, RGB(255, 255, 255));
    TextOutA(hdc, px_right + 30, py_bottom, "X", 1);
    TextOutA(hdc, px_left, py_top - 30, "Y", 1);

    POINT pts[n];
    SelectObject(hdc, pen_f1);
    for (int i = 0; i < n; i++) { 
        pts[i].x = map_val(res[0][i], res[0][0], res[0][n - 1], px_left, px_right);
        pts[i].y = map_val(res[1][i], ymin, ymax, py_bottom, py_top);
    }
    Polyline(hdc, pts, n);

    SelectObject(hdc, pen_f2);
    for (int i = 0; i < n; i++) {
        pts[i].x = map_val(res[0][i], res[0][0], res[0][n - 1], px_left, px_right);
        pts[i].y = map_val(res[2][i], ymin, ymax, py_bottom, py_top); 
    }
    Polyline(hdc, pts, n);

    SetTextColor(hdc, RGB(0, 191, 255));
    TextOutA(hdc, px_left + 350, py_top, "F1(x) = 5 - 3cos(x)", 19);
    SetTextColor(hdc, RGB(255, 80, 155));
    TextOutA(hdc, px_left + 350, py_top + 300, "F2(x) = sqrt(1 + sin^2(x))", 26);

    ReleaseDC(hwn, hdc);
    _getch();
}

void solve_bisection() {
    double A, B, C, E;
    int e;
    printf("\n\t\t\tМетод бисекции\n\t\t\tВведите границы [a, b]: ");
    scanf_s("%lf%lf", &A, &B);
    printf("\t\t\tТочность (знаков): ");
    scanf_s("%d", &e);
    E = pow(0.1, e);

    if (f(A) * f(B) > 0) { printf("\n\t\t\tОшибка: нет корня на интервале или четное число корней.\n"); return; }

    do {
        C = (A + B) / 2.0;
        if (f(A) * f(C) < 0) B = C;
        else A = C;
    } while (fabs(f(C)) > E);

    printf("\n\t\t\tКорень: %f\n", C);
}

void solve_chord() {
    double A, B, C, E;
    int e;
    printf("\n\t\t\tМетод хорд\n\t\t\tВведите границы [a, b]: ");
    scanf_s("%lf%lf", &A, &B); // Исправлено: %lf для double
    printf("\t\t\tТочность (знаков): ");
    scanf_s("%d", &e);
    E = pow(0.1, e);

    do {
        C = (A * f(B) - B * f(A)) / (f(B) - f(A));
        if (fabs(f(C)) <= E) break;
        if (f(A) * f(C) < 0) B = C;
        else A = C;
    } while (1);

    printf("\n\t\t\tКорень: %f\n", C);
}

void solve_newton() {
    double x0, x1, eps;
    int e_pow, step = 0;
    printf("\n\t\t\tМетод Ньютона\n\t\t\tНачальное приближение: ");
    scanf_s("%lf", &x0);
    printf("\t\t\tТочность: ");
    scanf_s("%d", &e_pow);
    eps = pow(0.1, e_pow);

    printf("\n\t\t\tШаг\t x0\t\t f(x0)\t\t f'(x0)\t\t x1\n");
    printf("\t\t\t--------------------------------------------------------\n");

    do {
        if (fabs(df(x0)) < 1e-9) {
            printf("\n\t\t\tПроизводная 0, метод не сходится.\n");
            return;
        }
        x1 = x0 - f(x0) / df(x0);
        printf("\t\t\t%d\t%9.6lf\t%9.6lf\t%9.6lf\t%9.6lf\n", ++step, x0, f(x0), df(x0), x1);
        if (fabs(x1 - x0) < eps) break;
        x0 = x1;
    } while (step < 1000);

    if (step < 1000) printf("\n\t\t\tКорень: %f\n", x1);
    else printf("\n\t\t\tРасходимость.\n");
}

void equals() {
    while (1) {
        int sel = handle_menu(equal_menu_items, EQUAL_MENU_SIZE, "Решение уравнения");
        system("cls");
        if (sel == 3) break;

        printf("\n\n\t\t\tРешение уравнения 2x * lg(x) - 3 = 0\n");
        switch (sel) {
        case 0: solve_bisection(); break;
        case 1: solve_chord(); break;
        case 2: solve_newton(); break;
        }
        printf("\nНажмите любую клавишу...");
        _getch();
    }
}

void showcase() {
    HWND hwn = GetConsoleWindow();
    RECT rect; GetClientRect(hwn, &rect);
    HDC hdc = GetDC(hwn);
    HBRUSH bg = CreateSolidBrush(RGB(0, 0, 0));

    int w = rect.right, cols = w / 15;
    typedef struct { int y, speed, len; } Col;
    Col* c = (Col*)malloc(cols * sizeof(Col));
    for (int i = 0; i < cols; i++) { c[i].y = rand() % rect.bottom;
    c[i].speed = 1 + rand() % 3; c[i].len = 5 + rand() % 10; }

    while (!_kbhit()) {
        FillRect(hdc, &rect, bg); SetBkMode(hdc, TRANSPARENT);
        for (int j = 0; j < cols; j++) {
            int x = j * 15 + 10;
            SetTextColor(hdc, RGB(0, 255, 0));
            char ch = 'A' + rand() % 26; TextOutA(hdc, x, c[j].y, &ch, 1);

            for (int k = 1; k < c[j].len; k++) {
                SetTextColor(hdc, RGB(0, 255 - (k * 255 / c[j].len), 0));
                char tc = 'A' + rand() % 26;
                if (c[j].y - k * 20 >= 0) TextOutA(hdc, x, c[j].y - k * 20, &tc, 1);
            }
            c[j].y += c[j].speed;
            if (c[j].y > rect.bottom) { c[j].y = -c[j].len * 20; c[j].speed = 1 + rand() % 3; }
        }
        Sleep(50);
    }
    free(c);
    DeleteObject(bg);
    ReleaseDC(hwn, hdc);
    _getch();
}

int main() {
    setlocale(LC_ALL, "RUS");
    HideCursor();

    while (1) {
        int sel = handle_menu(menu_items, MENU_SIZE, "МЕНЮ");
        system("cls");
        SetColor(0);

        switch (sel) {
        case 0: printf("\n\t\t\tТаблица значений:"); table_draw(); break;
        case 1: graph(); break;
        case 2: equals(); break;
        case 3: printf("\n> Выбран пункт: Интеграл\n"); break;
        case 4: showcase(); break;
        case 5: printf("\n\n\n\t\t\t" COLOR_GREEN "Автор: " COLOR_RESET "ст.гр. " COLOR_GREEN "ИВТ - 254 " COLOR_RESET "Дубровин А.А.\n\n"); break;
        case 6: return 0;
        }
        printf("\nНажмите любую клавишу для возврата...");
        _getch();
    }
    return 0;
}