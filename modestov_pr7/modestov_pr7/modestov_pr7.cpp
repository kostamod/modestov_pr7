#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define DEBUG_MODE

struct Tovar {
    int id = 0;
    string nazvanie;
    string kategoria;
    double tsena = 0.0;
    int kolichestvo = 0;
    bool vse_ok = false;
};

Tovar razbor_stroki(string stroka, Tovar t) {
    if (stroka.empty()) {
        t.vse_ok = false;
        return t;
    }

    size_t p1 = stroka.find(';');
    size_t p2 = (p1 != string::npos) ? stroka.find(';', p1 + 1) : string::npos;
    size_t p3 = (p2 != string::npos) ? stroka.find(';', p2 + 1) : string::npos;
    size_t p4 = (p3 != string::npos) ? stroka.find(';', p3 + 1) : string::npos;

    if (p4 == string::npos) {
        t.vse_ok = false;
        return t;
    }

    try {
        t.id = stoi(stroka.substr(0, p1));
        t.nazvanie = stroka.substr(p1 + 1, p2 - p1 - 1);
        t.kategoria = stroka.substr(p2 + 1, p3 - p2 - 1);
        t.tsena = stod(stroka.substr(p3 + 1, p4 - p3 - 1));
        t.kolichestvo = stoi(stroka.substr(p4 + 1));

        if (t.tsena >= 0 && t.kolichestvo >= 0) {
            t.vse_ok = true;
        }
        else {
            t.vse_ok = false;
        }
    }
    catch (...) {
        t.vse_ok = false;
    }

    return t;
}

int main() {
    setlocale(LC_ALL, "Russian");

    string imya_faila = "products.txt";
    const int MAX_TOVAROV = 100;
    Tovar sklad[MAX_TOVAROV];
    int vsego_tovarov = 0;

    ifstream f_in(imya_faila);
    if (!f_in.is_open()) {
        cout << "Ошибка! Не могу открыть файл " << imya_faila << endl;
        return 1;
    }

    string str;
    int num_str = 0;

    while (getline(f_in, str)) {
        num_str++;
        if (str.empty()) continue;

        if (vsego_tovarov >= MAX_TOVAROV) {
            cout << "Склад переполнен!" << endl;
            break;
        }

        Tovar pustoy;
        Tovar rez = razbor_stroki(str, pustoy);

        if (!rez.vse_ok) {
            cout << "Битая строка номер " << num_str << "!" << endl;
            f_in.close();
            return 1;
        }

        sklad[vsego_tovarov] = rez;
        vsego_tovarov++;
    }
    f_in.close();

#ifdef DEBUG_MODE
    cout << "[DEBUG] Загружено записей: " << vsego_tovarov << endl;
#endif

    int vibor = 0;
    while (true) {
        cout << "\n=== ИНТЕРНЕТ-МАГАЗИН ===" << endl;
        cout << "1. Показать все товары" << endl;
        cout << "2. Поиск по названию" << endl;
        cout << "3. Фильтр по категории" << endl;
        cout << "4. Купить товар" << endl;
        cout << "5. Сохранить и выйти" << endl;
        cout << "> Выберите пункт: ";
        cin >> vibor;

        if (vibor == 1) {
            cout << "\n--- Список товаров ---" << endl;
            for (int i = 0; i < vsego_tovarov; i++) {
                cout << "ID: " << sklad[i].id
                    << " | Имя: " << sklad[i].nazvanie
                    << " | Кат: " << sklad[i].kategoria
                    << " | Цена: " << sklad[i].tsena
                    << " руб. | Ост: " << sklad[i].kolichestvo << " шт." << endl;
            }
        }
        else if (vibor == 2) {
            string poisk_name;
            cout << "Введите название для поиска: ";
            cin >> poisk_name;

            bool nashel = false;
            for (int i = 0; i < vsego_tovarov; i++) {
                if (sklad[i].nazvanie.find(poisk_name) != string::npos) {
                    cout << "Найдено -> ID: " << sklad[i].id << " | " << sklad[i].nazvanie
                        << " | Цена: " << sklad[i].tsena << " | Кол-во: " << sklad[i].kolichestvo << endl;
                    nashel = true;
                }
            }
            if (!nashel) cout << "Ничего не найдено!" << endl;
        }
        else if (vibor == 3) {
            string poisk_kat;
            cout << "Введите категорию: ";
            cin >> poisk_kat;

            bool nashel = false;
            for (int i = 0; i < vsego_tovarov; i++) {
                if (sklad[i].kategoria == poisk_kat) {
                    cout << "ID: " << sklad[i].id << " | " << sklad[i].nazvanie
                        << " | Цена: " << sklad[i].tsena << endl;
                    nashel = true;
                }
            }
            if (!nashel) cout << "Категория не найдена!" << endl;
        }
        else if (vibor == 4) {
            int buy_id, buy_kol;
            cout << "Введите ID товара: ";
            cin >> buy_id;
            cout << "Сколько штучек надо: ";
            cin >> buy_kol;

            int idx = -1;
            for (int i = 0; i < vsego_tovarov; i++) {
                if (sklad[i].id == buy_id) {
                    idx = i;
                    break;
                }
            }

            if (idx == -1) {
                cout << " Ошибка: Товар с таким ID не найден!" << endl;
            }
            else if (buy_kol <= 0) {
                cout << " Ошибка: Введено неверное количество!" << endl;
            }
            else if (sklad[idx].kolichestvo < buy_kol) {
                cout << " Ошибка: На складе нет столько! Доступно всего: "
                    << sklad[idx].kolichestvo << endl;
            }
            else {
                double itogo = sklad[idx].tsena * buy_kol;
                sklad[idx].kolichestvo -= buy_kol;
                cout << " Успех! К оплате: " << itogo << " руб." << endl;
                cout << "Осталось на складе: " << sklad[idx].kolichestvo << endl;
            }
        }
        else if (vibor == 5) {
            ofstream f_out(imya_faila);
            if (!f_out.is_open()) {
                cout << "Не удалось открыть файл для сохранения!" << endl;
                return 1;
            }

            int sohraneno = 0;
            for (int i = 0; i < vsego_tovarov; i++) {
                f_out << sklad[i].id << ";"
                    << sklad[i].nazvanie << ";"
                    << sklad[i].kategoria << ";"
                    << sklad[i].tsena << ";"
                    << sklad[i].kolichestvo << "\n";
                sohraneno++;
            }
            f_out.close();

#ifdef DEBUG_MODE
            cout << "[DEBUG] Успешно сохранено записей: " << sohraneno << endl;
#endif
            cout << "Все изменения сохранены в файл. Пока!" << endl;
            break;
        }
        else {
            cout << "Неверный пункт меню, давай по новой!" << endl;
        }
    }

    return 0;
}