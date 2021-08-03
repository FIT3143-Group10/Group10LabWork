#include <stdio.h>

int main() {
    enum type { invalid, small, medium, large, skyscraper };
    int building_floors = 23;

    enum type building_size;
    if (building_floors < 0) {
        building_size = invalid;
    } else if (building_floors <= 20) {
        building_size = small;
    } else if (building_floors > 20 && building_floors <= 30) {
        building_size = medium;
    } else if (building_floors > 30 && building_floors <= 40) {
        building_size = large;
    } else {
        building_size = skyscraper;
    }
}