#include <iostream>

#include "headers.h"

int main(int argc, char **argv) {
  const std::vector<int> v1 {1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int> v2 {6, 7, 8, 9, 10, 12};
  auto f1 = AsDataFlow(v1)
  | Transform([](const int& val){ return val + 10; })
  | Filter([](const int& val){ return val > 13; });
  //f1 | Out(std::cout);
  auto f2 = AsDataFlow(v2) | MergeCustom(f1); // мерджит потоки -> выход kv из optional, key - сумма элементов (сумма двух значений), value - значение (если один длиннее другого, то nullopt), вывод через out
  
  f2 | Out(std::cout);

  return 0;
}