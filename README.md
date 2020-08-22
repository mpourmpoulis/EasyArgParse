# EasyArgParse

[opterator](https://github.com/dusty-phillips/opterator) inspired C++ 17 argparse wrapper for Instant CLI prototyping

WIP

<!-- MarkdownTOC  autolink="true" -->

- [Example](#example)
    - [First example](#first-example)
- [Getting Started](#getting-started)
    - [Boolean flags](#boolean-flags)
    - [Using std::optional](#using-stdoptional)
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
    }
        std::cout<< firstname << " " << lastname << std::endl;
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
name         	string * 2 [Required]

Optional arguments:
-h --help    	shows help message and exits
-v --version 	prints version information and exits
-a           	bool  
-b           	bool  
--age        	int 
```

and if we try a few more prompts

```bash
$example No Name
You entered false false
No Name

$example -a No Name
You entered true false
No Name

$example -b No Name
You entered false true
No Name

$example -ab No Name --age 42
You entered true true
Your age is  42
No Name
```

## Getting Started


### Boolean flags

To declare a boolean flag, you simply need 

```cpp
auto f(bool a){
    std::cout<< std::boolalpha <<  a << std::endl;
}

auto temporary = easyarg::EasyArguments("example",f,"-a");
```

```python
$example 
false

$example -a
true
```

### Using std::optional

Using std::optional we can specify whether an argument is required or not, for example

```cpp
auto f(std::string name,std::optional<int> age){
    std::cout<< "name" << " " << name << std::endl;
    if(age){
        std::cout<< "age" << " " << *age << std::endl;
    }   
}

auto temporary = easyarg::EasyArguments("example",f,"--name","--age");
```

we have

```python
$example --help
Usage: example [options] 

Optional arguments:
-h --help       shows help message and exits
-v --version    prints version information and exits
--name          string [Required]
--age           int 
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

