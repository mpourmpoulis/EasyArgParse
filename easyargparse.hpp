#ifndef EASY_ARG_PARSE_HPP
# define EASY_ARG_PARSE_HPP

#include <list>
#include <optional>
#include <vector>
#include <tuple>
#include <sstream>
// #include "argparse/argparse.hpp"
#include "argparse/include/argparse/argparse.hpp"

/////////////////////////////////////////////////////////////////
// code to grab signature of function based on
// https://stackoverflow.com/questions/28033251/can-you-extract-types-from-template-parameter-function-signature
// with some modifications
/////////////////////////////////////////////////////////////////

namespace easyarg {

template<typename S>
struct signature { };

template<typename R, typename... Args>
struct signature<R(Args...)>
{
    using return_type = R;
    using argument_type = std::tuple<Args...>;
};

template<typename R, typename... Args>
struct signature<R(*)(Args...)>
{
    using return_type = R;
    using argument_type = std::tuple<Args...>;
};

template<typename R, typename... Args>
struct signature<R(&)(Args...)>
{
    using return_type = R;
    using argument_type = std::tuple<Args...>;
};

template<typename R, typename... Args>
struct signature<R(&&)(Args...)>
{
    using return_type = R;
    using argument_type = std::tuple<Args...>;
};

////////////////////////////////////////////////////////////////////////

template<typename T>
struct unpack{};

template<typename T>
struct unpack<std::tuple<T>>{
	using head= T;
	static constexpr bool has_tail(){return false;}
};


template<typename T, typename... Args>
struct unpack<std::tuple<T,Args...>>{
	using head= T;
	using tail = std::tuple<Args...>;
	static constexpr bool has_tail(){return true;}
};

////////////////////////////////////////////////////////////////////////

// not really use at the moment
template<template<typename > typename R,typename F>
struct recursive_single{
	using f = unpack<F> ;
    void operator () (){
    	using h=typename f::head ;
    	R<h>()();
    	if constexpr (f::has_tail()){
    		recursive_single<R,typename f::tail>()();
    	}
    	
    }
};

////////////////////////////////////////////////////////////////////////
// not really use at the moment
template<typename F,typename S>
constexpr bool compatible(){
	if constexpr (std::is_same<F,S>::value || std::is_same<std::optional<S>,S>::value){
		return true;
	}
	return false;	
}


////////////////////////////////////////////////////////////////////////
template<typename T = std::string>
class Parameter
{
public:
	std::string name;
	std::optional<int > number;
	std::optional<T> default_value;
	std::optional<std::string> description;
	Parameter(
			std::string _name,
			std::optional<int> _number = {},
			std::optional<T> _default_value = {},
			std::optional<std::string > _description = {})
	{
		name = _name;
		number = _number; 
		default_value = _default_value;
		description = _description;
		if constexpr (std::is_same<T,bool>::value){
			if(!default_value){
				default_value = false;
			}			
		}	

	}

	
	Parameter<std::vector<std::string>> operator = (std::initializer_list<const char *> value)	{
		std::vector<std::string > v;
		for(auto x : value){
			v.push_back(std::string(x));
		}
		
		return Parameter<std::vector<std::string>>(this->name,this->number,v,this->description);
	}


	template<class U>
	Parameter<std::vector<U>> operator = (std::initializer_list<U> value)	{
		return Parameter<std::vector<U>>(this->name,this->number,std::vector<U>(value),this->description);
	}

	template<class U>
	Parameter<U> operator = (U&& value)	{
		return Parameter<U>(this->name,this->number,value,this->description);
	}

	Parameter<T> operator [] (int number){
		return Parameter<T>(this->name,number,this->default_value,this->description);
	}

	Parameter<T> operator << (std::string description){
		return Parameter<T>(this->name,this->number,this->default_value,description);
	}

	Parameter<T> operator << (const char * description){
		return Parameter<T>(this->name,this->number,this->default_value,std::string(description));
	}

};


	Parameter<std::string > operator "" _p (const char * a,std::size_t b){
		auto s = std::string(a);
		return Parameter<std::string>(a);
	}



template<typename T,typename U>
constexpr bool is_parameter(){return std::is_same<U,Parameter<T>>::value;}

template<typename T,typename U>
constexpr bool is_parameter(U u){return std::is_same<U,Parameter<T>>::value;}


////////////////////////////////////////////////////////////////////////////////////////////////

template<template<typename ...>typename ,typename >
class instance_t;

template<template<typename ...> typename T,typename ... Args>
class instance_t<T,T<Args...>> : public std::true_type {};

template<template<typename ...> typename T,typename U>
class instance_t:public std::false_type {};


template<template<typename ...> typename T, typename U>
constexpr bool instance(){return instance_t<T,U>::value;}

template<typename T>
constexpr bool is_optional(){return instance<std::optional,T>();}



////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T> struct  is_array_t:public std::false_type {};
template<typename T,std::size_t N> struct is_array_t<std::array<T,N>>: public std::true_type {};

template<typename T>
constexpr bool is_array(){
	return is_array_t<T>::value;
}


////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T> struct inner;
template<typename T>
struct inner<std::optional<T>>{
	using type = typename inner<T>::type ;
};

template<typename T>
struct inner<std::vector<T>>{
	using type = typename inner<T>::type ;
};

template<typename T>
struct inner<std::list<T>>{
	using type = typename inner<T>::type ;
};

template<typename T,std::size_t N>
struct inner<std::array<T,N>>{
	using type = typename inner<T>::type ;
};


template<typename T>
struct inner<std::pair<T,T>>{
	using type = typename inner<T>::type;
};

template<typename T>
struct inner{
	using type = T;
};


////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T,typename = void>
struct value_type{
	using type = T ;
};

template<typename T>
struct value_type<T,std::void_t<typename T::value_type>> {
	using type = typename T::value_type;
};

template<typename T>
struct value_type<std::pair<T,T>>{
	using type = T;
};


template<typename T>
using value_type_t = typename  value_type<T>::type;
////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
constexpr int get_number_from_type();

template<typename T>
constexpr int get_number_from_type(){
	if constexpr (is_optional<T>()){
		return get_number_from_type<typename T::value_type>();
	} else if constexpr (is_array<T>()){
		return std::tuple_size<T>::value;
	} else if constexpr (instance<std::pair,T>()){
		return 2;
	}else{
		return 0;
	}	
}


////////////////////////////////////////////////////////////////////////////////////////////////

template<typename R,typename T>
auto generate_documentation(argparse::Argument &a){
	std::ostringstream output;
	if constexpr (std::is_same_v<T,int>) output<<"int ";
	if constexpr (std::is_same_v<T,std::string >) output<<"string ";
	if constexpr (std::is_same_v<T,float>) output<<"float ";
	if constexpr (std::is_same_v<T,double>) output<<"double  ";
	if constexpr (std::is_same_v<T,bool>) output<<"bool  ";
	if(auto n = a.maybe_nargs()){
		if(*n>1){
			output<<"* "<<*n <<" ";
		}		
	}
	return output.str();	
}


////////////////////////////////////////////////////////////////////////////////////////////////

template<typename R,typename P>
void register_parameter(argparse::ArgumentParser &p,P value){
	// static_assert(std::is_same<>)
	auto& a  = p.add_argument(value.name);
	if constexpr (constexpr int nc = get_number_from_type<R>()){
		a.nargs(nc);
	}else{
		if(auto n = value.number){
			if(*n==-1){
				a.remaining();
			}else if(*n>1){
				a.nargs(*n);
			}		
		}
	}
	if constexpr (!is_optional<R>()){
		if constexpr (!std::is_same<Parameter<R>,P>::value){
			if(value.default_value){
				std::cout<< "Disaster" << std::endl;
				
			}
			
		}
		
	}
	
	if(auto d = value.default_value){		
		a.default_value(*d);		
		if constexpr (std::is_same_v<decltype(d),std::optional<bool>>){
			a.implicit_value(!(*d));
		}		
	}else{
		if constexpr (std::is_same_v<R,bool>){
			a.default_value(false);
			a.implicit_value(true);
		}		
	}
	

	if constexpr (!instance<std::optional,R>() && !std::is_same_v<R,bool>){
		a.required();
	}

	using t = typename  inner<R>::type ;
	if constexpr (std::is_same<t,int>::value){
		a.action([](const std::string& value) { return std::stoi(value); });
	}
	if constexpr (std::is_same<t, float>::value){
		a.action([](const std::string& value) { return std::stof(value); });
	}
	if constexpr (std::is_same<t,double>::value){
		a.action([](const std::string& value) { return std::stod(value); });
	}

	if(auto description = value.description){
		a.help(*description);
	}else{
		a.help(generate_documentation<R,t>(a));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename P>
auto obtain_parameter(argparse::ArgumentParser &p,std::vector<std::string>::reverse_iterator &it){
	std::string s = *it++;
	constexpr bool c = is_optional<P>();
	using data_type	= std::conditional_t<c,value_type_t<P>,P> ;
	constexpr bool a = is_array<data_type>()||std::is_array_v<data_type>;
	constexpr bool ip = instance<std::pair,data_type>();
	using temporary_type = 
		std::conditional_t<a||ip ,std::vector<value_type_t<data_type>>, data_type> ;
	if constexpr (c){
		auto data = p.present<temporary_type>(s);
		if(!data){
			return P{};
		}
		
		if constexpr (a){
			data_type result;
			std::copy((*data).begin(),(*data).end(),std::begin(result));
			return P(result);
		}else if constexpr (ip){
			data_type result = {(*data).at(0),(*data).at(1)};

			return P(result);
		}else{
			return data;
		}
	}else{
		auto data = p.get<temporary_type>(s);

		if constexpr (a){
			data_type result;
			std::copy(data.begin(),data.end(),std::begin(result));
			return result;
		}else if constexpr (ip){
			data_type result = {data.at(0),data.at(1)};
			return result;
		}else{
			return data;
		}

	}	
}


template<typename F, typename U>
struct  run_function;

template<typename F, typename ...Args>
struct  run_function<F,std::tuple<Args...>>{
	auto operator()	(F f,argparse::ArgumentParser &p,std::vector<std::string > &names){
		auto it = names.rbegin();
		return f(obtain_parameter<Args>(p,it)...);
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////



template<typename Signature,typename First,typename ...Args>
void mark(argparse::ArgumentParser &p,First first,Args... args){
	using signature = unpack<Signature> ;
    using h = typename signature::head ;
    if constexpr (!std::is_same<First,const char *>::value){
    	register_parameter<typename signature::head,First>(p,first);    	
    }else{
    	using new_type = std::conditional_t< is_optional<h>() , value_type_t<h> , h>;
    	register_parameter<typename signature::head,Parameter<new_type>>(p,Parameter<new_type>(first));
    }
    
	    

    if constexpr (signature::has_tail()){
    	mark<typename signature::tail,Args...>(p,args...);
    }  	    
}


template<class F, typename... Args>
class EasyArguments
{
    public:
    	argparse::ArgumentParser program;
    	F ff;
    	std::vector<std::string> names;
        using arguments = typename signature<F>::argument_type;
        using return_type = typename signature<F>::return_type;

        EasyArguments(std::string program_name,F f,Args... args){
        	program = argparse::ArgumentParser(program_name);
        	ff = f;
        	names = std::vector<std::string>{(get_name(args))...};
        	mark<arguments,Args...>(program,args...);
        };

        std::string get_name(const  char *s){
        	return std::string(s);
        }

        std::string get_name(std::string s){
        	return s;
        }

        template<typename T>
        std::string get_name(Parameter<T> s){
        	return s.name;
        }
		return_type operator () (int argc,const char ** argv){
        	try {
        	  program.parse_args(argc, argv);
        	}
        	catch (const std::runtime_error& err) {
        	  std::cout << err.what() << std::endl;
        	  std::cout << program;
        	  exit(0);
        	}
			return run_function<F,arguments>()(ff,program,names);
        }
};





} // namespace easyargparse


#endif

