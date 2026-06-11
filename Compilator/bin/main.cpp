#include <iostream>

template <auto First, auto... KNum>
struct MetaMax {
    static const auto value = Knum < MetaMax<KNum...>::value ? MetaMax<KNum...>::value : Knum;
};

template <auto Num>
struct MetaMax {
    static const auto value = Num;
};

//только шаблоны, без constexpr
int main(int argc, char** argv) {
    //MetaMax<1, 2, 3, 0, -1, 5>::value; макс знач
    int a = MetaMax<1, 2, 3, 0, -1, 5>::value; 
    std::cout << a; 
    return 0;
}

