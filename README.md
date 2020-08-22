# EasyArgParse

[opterator](https://github.com/dusty-phillips/opterator) inspired C++ 17 argparse wrapper for Instant CLI prototyping

WIP

<!-- MarkdownTOC  autolink="true" -->

- [Example](#example)
	- [First example](#first-example)
- [Datatypes](#datatypes)
	- [Primitive data types](#primitive-data-types)
	- [Utilities](#utilities)
	- [Containers](#containers)
	- [TODO?](#todo)

<!-- /MarkdownTOC -->

## Example

### First example

```cpp
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
```

Now if we try  `example --help` we get

```
Usage: example [options] name 

Positional arguments:
name         	[Required]

Optional arguments:
-h --help    	shows help message and exits
-v --version 	prints version information and exits
-a           	
-b           	
--age        	
```





## Datatypes

The following datatypes are supported, though I should note all of the arguments should be expected to be passed by value

### Primitive data types

- int 

- float

- double 

- std::string 


### Utilities

- std::optional<>


### Containers

- std::vector<>

- std::list<>

- std::pair<>

- std::array<>

### TODO?

We have to see if those are possible and if they are worth the trouble

- not homogenous std::tuple<> to mix datatypes

- std::variant<>

- Opening files implicitly?

- Multiple flags per argument support

