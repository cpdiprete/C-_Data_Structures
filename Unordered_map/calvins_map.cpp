#include "calvins_map.h"



int main() {
    std::cout << "calvin called main\n";
    CalvinsMap firstMap = CalvinsMap();
    // std::cout << "FROM MAIN (SIZE CHECK) ==> " << firstMap.totalPopulation << '\n';
    // firstMap.print();
    firstMap.insert(5, 99);
    // firstMap.print();
    firstMap.insert(3, 101);
    firstMap.insert(3, 77);
    firstMap.insert(3, 66);
    // firstMap.print();
    return 0;
}