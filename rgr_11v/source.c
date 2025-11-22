#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h> // Для _getch()

// Константы клавиш
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27

#define MENU_SIZE 7
#define a 0.00
#define b 2 * 3.1415
#define n 20

// Названия пунктов меню
const char* menu_items[MENU_SIZE] = {
    "1. Автор",
    "2. Заставка",
    "3. Таблица",
    "4. Уравнение",
    "5. Интеграл",
    "6. График",
    "7. Выход"
};

// Функция установки цвета консоли
void SetColor(int k) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (k == 1) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    }
    else if (k == 0) {
        SetConsoleTextAttribute(hConsole, 7);
    }
}

// Скрыть мигающий курсор (для красоты меню)
void HideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// Функция отрисовки таблицы
void table_draw()
{
    double F1, F2, s = fabs(b - a) / (n - 1), x = a;

    printf(" ___________________________________________ \n");
    printf(" | I |     X    |     F1     |      F2     | \n");
    printf(" |---|----------|------------|-------------|\n");
    for (int i = 0; i < n; i++)
    {
        F1 = 5 - 3 * cos(x);
        F2 = sqrt(1 + pow(sin(x), 2));
        printf(" |%3d| %8.3f |%12.7f| %12.8f|\n", i + 1, x, F1, F2);
        x += s;
    }
    printf(" ------------------------------------------- \n");
}

int main() {
    setlocale(LC_ALL, "rus");
    HideCursor();

    int selected = 0;
    int key;
    int running = 1;

    while (running)
    {
        system("cls"); // Очистка экрана перед отрисовкой меню
        printf("  --- МЕНЮ ---\n\n");

        // Отрисовка пунктов с подсветкой
        for (int i = 0; i < MENU_SIZE; i++)
        {
            if (i == selected) {
                SetColor(1); // Белый фон, черный текст (выделение)
                printf("  %s  \n", menu_items[i]);
                SetColor(0);   // Возврат к обычному цвету
            }
            else {
                printf("   %s    \n", menu_items[i]);
            }
        }

        printf("\nИспользуйте СТРЕЛКИ для навигации, ENTER для выбора.");

        // Ожидание нажатия клавиши (программа "спит", пока не нажмете кнопку)
        key = _getch();

        // Обработка клавиш
        if (key == 224 || key == 0) { // Если нажата спец-клавиша (стрелка)
            key = _getch(); // Считываем код самой стрелки
            switch (key) {
            case KEY_UP:
                selected--;
                if (selected < 0) selected = MENU_SIZE - 1; // Зацикливание снизу вверх
                break;
            case KEY_DOWN:
                selected++;
                if (selected >= MENU_SIZE) selected = 0; // Зацикливание сверху вниз
                break;
            }
        }
        else if (key == KEY_ENTER) { // Если нажат ENTER
            system("cls"); // Очищаем меню, чтобы показать результат
            SetColor(0);   // Гарантируем стандартный цвет

            switch (selected)
            {
            case 0:
                printf("\n> Выбран пункт: Автор\n");
                printf("> Автор: ст.гр. ИВТ-254 Дубровин А.А.\n");
                break;
            case 1:
                printf("\n> Выбран пункт: Заставка\n");
                break;
            case 2:
                printf("\n> Таблица значений:\n");
                table_draw();
                break;
            case 3:
                printf("\n> Выбран пункт: Уравнение\n");
                break;
            case 4:
                printf("\n> Выбран пункт: Интеграл\n");
                break;
            case 5:
                printf("\n> Выбран пункт: График\n");
                break;
            case 6:
                printf("\n> Выход из программы...\n");
                running = 0;
                break;
            }

            // Если мы не выходим, ставим паузу, чтобы пользователь увидел результат
            if (running) {
                printf("\nНажмите любую клавишу для возврата в меню...");
                _getch();
            }
        }
        else if (key == KEY_ESC) {
            running = 0; // Быстрый выход по Escape
        }
    }

    return 0;
}