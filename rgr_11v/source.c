#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h> // Нужно для _getch()

/*
* 1. Автор
* 2. Заставка
* 3. Таблица
* 4. Уравнение
* 5. Интеграл
* 6. График
* 7. Выход
*/

#define MENU_SIZE 7
#define a 0.00
#define b 2 * 3.1415
#define n 20

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

const char* menu_items[MENU_SIZE] = {
    "1. Автор",
    "2. Заставка",
    "3. Таблица",
    "4. Уравнение",
    "5. Интеграл",
    "6. График",
    "7. Выход"
};

void draw_menu(int selected)
{
    system("cls");
    printf("  --- МЕНЮ ---\n\n");
    for (int i = 0; i < MENU_SIZE; i++)
    {
        if (i == selected) {
            printf("> %s <\n", menu_items[i]);
        }
        else {
            printf("  %s\n", menu_items[i]);
        }
    }
}

int main() {
    setlocale(LC_ALL, "rus");
    int selected = 0;
    BOOL running = TRUE;

    draw_menu(selected);

    while (running)
    {
        // Обработка стрелки ВВЕРХ
        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            if (selected > 0)
            {
                selected--;
                draw_menu(selected);
            }
            Sleep(150); // Задержка для комфортной прокрутки
        }

        // Обработка стрелки ВНИЗ
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            if (selected < MENU_SIZE - 1)
            {
                selected++;
                draw_menu(selected);
            }
            Sleep(150); // Задержка для комфортной прокрутки
        }

        // Обработка нажатия ENTER
        if (GetAsyncKeyState(VK_RETURN) & 0x8000)
        {
            // --- ИСПРАВЛЕНИЕ 1: Ждем, пока пользователь ОТПУСТИТ Enter перед входом ---
            // Это предотвращает случайные двойные срабатывания при входе
            while (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                Sleep(10);
            }

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
                printf("\n> Выбран пункт: Таблица\n");
                system("cls");
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
                printf("\n> Выход...\n");
                running = FALSE;
                break;
            }

            if (running)
            {
                printf("\nНажмите любую клавишу для возврата в меню...");
                _getch();
                while (GetAsyncKeyState(VK_RETURN) & 0x8000) Sleep(10);

                draw_menu(selected);
            }
        }

        // Небольшая пауза для разгрузки процессора в бесконечном цикле
        Sleep(10);
    }
    return 0;
}