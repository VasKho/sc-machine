#include <iostream>
#include <string>
#include <set>
#include <sstream>
#include <exception>
#include <fstream>

#include <boost/config.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>

namespace pod = boost::program_options::detail;

std::map<std::string, std::string> parse_config(std::string conf) 
{  
    std::ifstream config(conf);
    if(!config)
    {
        std::cerr << "error" << std::endl;
        return std::map<std::string, std::string>();
    }

    std::set<std::string> options;
    options.insert("*");
    std::map<std::string, std::string> result;
    
    try
    {      
        for (pod::config_file_iterator i(config, options), e; i != e; ++i)
        {
	    if (i->string_key == "Repo.Path") result.insert({"path", i->value[0]});
	    if (i->string_key == "Extensions.Directory") result.insert({"ext", i->value[0]});
        }
    }
    catch(std::exception& e)    
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return result;
}
