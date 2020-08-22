#include <iostream>
#include "../easyargparse.hpp"

using namespace easyarg;

template<typename T>
using Pair = std::pair<T, T>;

auto f(bool a,bool b, Pair<std::string> name, std::optional<int> age){
    std::cout<< std::boolalpha << "You entered " << a << " " << b << std::endl;
    auto [firstname, lastname]  = name;
    if(age){
        std::cout<< "Your age is " << " " << *age << std::endl;
        std::cout<< firstname << " " << lastname << std::endl;
    }
}

int main(int argc,const char **argv)
{   
    easyarg::EasyArguments temporary("example",f,"-a","-b","name","--age");
    temporary(argc, argv);
}

