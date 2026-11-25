#include "calvins_map.h"



int main() {
    std::cout << "calvin called main\n";
    CalvinsMap firstMap = CalvinsMap();
    // std::cout << "FROM MAIN (SIZE CHECK) ==> " << firstMap.totalPopulation << '\n';
    // firstMap.print();
    firstMap.insert(5, 99);
    // firstMap.print();
    firstMap.insert(3, 101);
    firstMap.insert(7, 77);
    firstMap.insert(20, 66);
    // firstMap.print();
    std::cout << "============================== GET CHECKS ======================== \n\n";
    

    int arr[5] = {5, 3, 7, 14, 20};
    for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
        std::cout << firstMap.get(arr[i]) << '\n';
    }
    return 0;
}