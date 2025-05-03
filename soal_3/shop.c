#include <string.h>

struct Weapon {
    char name[1000];
    int price;
    int damage;
    char passive[1000];
};

struct Weapon shop_items[] = {
    {"Terra Blade", 50, 10, "None"},
    {"Flint & Steel", 150, 25, "None"},
    {"Kitchen Knife", 200, 35, "Magic Damage +5"},
    {"Staff Of Light", 120, 20, "10% Insta-Kill chance"},
    {"Dragon Claws", 300, 50, "Critical Chance +30%"},
    {"Panggil Developer", 500, 1000, "Summon developer,waspada!!! konon katanya developer sedang stres berat, Damage +100%"},
};

int shop_size = sizeof(shop_items) / sizeof(shop_items[0]);