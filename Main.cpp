#include <iostream>
#include <ctime>
#include "Point.h"
#include "Monster.h"
#include "Field.h"
#include "User.h"

int main(void) {
    clock_t begin, end;
    begin = clock();

    Field nosField(10000, 10000, 5000);
    nosField.LoadFromFile();
    nosField.AutoHunt();
    nosField.SaveToFile();

    end = clock();
    std::cout << "수행시간: " << (end - begin) / CLOCKS_PER_SEC << std::endl;

    return 0;
}