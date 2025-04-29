#include "ServerConfig.hpp"
#include <iostream>
#include <set> // Use set instead of std::vector
#include <sstream>
#include <string>
void    checkcontent(std::string substr)
{
    size_t i =0 ;
    while(i < substr.length()) {
        if (substr[i]=='\t' ||  substr[i]==' '|| substr[i]=='\n')
            i++;
        else {
            std::cout<<REDCOLORE<< "ERROR : invalid arguments "<<std::endl;
            exit(0);
        }
    }
}
void    checkContentServer(std::string substr)
{
    size_t i =0 ;
    while(i < substr.length()) {
        if (substr[i]=='\t' ||  substr[i]==' '|| substr[i]=='\n')
            i++;
        else {
            std::cout<< REDCOLORE << "Error: unexpected data after server block" << std::endl;
            exit(0);
        }
    }
}
void chechAllowedMethodValid(std::string &str) {
    std::vector <std::string> words;
    words = splitstring(str);
    for (size_t i = 0; i < words.size(); i++) {
        if (words[i]!="GET" && words[i]!="POST" && words[i]!="DELETE") {
            std::cout<<"error method not valid"<<std::endl;
            exit(0);
        }
    }  
}

bool checkCharacter(std::string& substr,char c) {
	size_t i =0 ;
	while(i < substr.length()) {
		if (substr[i]!='\t' &&  substr[i]!=' '&& substr[i]!='\n' && substr[i]!=c)
			return true;
		i++;
	}
	return false;
}
std::string trim(std::string &str) {
    size_t start = 0;
    size_t end = str.length();
    while (start < end && (str[start] == ' ' || str[start] == '\n' || str[start] == '\t')) {
        ++start;
    }
    while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\n' || str[end - 1] == '\t')) {
        --end;
    }
    return str.substr(start, end - start);
}
std::string removeComments(std::string& input) {
    std::stringstream result;
    
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '#') {
            while (i < input.size() && input[i] != '\n') {
                i++;
            }
            i--;
        }
        else {
            result << input[i];
        }
    }
    return result.str();
}

std::string removeLocationBlocks(std::string& configData) {
	std::string result = configData;
    size_t pos = result.find("location");
    if (pos ==std::string::npos) {
        result.pop_back();
        return result ;
    }
    while ((pos = result.find("location", pos)) != std::string::npos) {
        size_t firstEndbarkets = result.find_first_of("}", pos);
        result.replace(pos,firstEndbarkets + 1 - pos,"");
    }
    result.pop_back();
	return result;
}

ServerConfig :: ServerConfig(std::string &str) {
    this->data = str;
}

ServerConfig :: ~ServerConfig() { }

size_t checkValidBadySise(std::string str)
{
    size_t maxBadysize;
    std::string number;
    std::string typeStorage;
    size_t i = 0;
    while (i < str.length()) {
        if(isdigit(str[i]))
            number += str[i];
        else
            break;
        i++;
    }
    if (i== str.length()) {
        std::cout<<REDCOLORE<< "ERROR : invalid value " <<"\"" <<str<<"\""<<std::endl;
        exit(0);
    }
    if (i==0) {
        std::cout<<REDCOLORE<<  "ERROR : invalid value " <<"\"" <<str<<"\""<<std::endl;
        exit(0);
    }
    maxBadysize = static_cast<size_t>(stoll(number));

    typeStorage = str.substr(i);
    if (typeStorage !="GB"&&typeStorage !="G"&&typeStorage !="MB"&&typeStorage !="M"&&typeStorage !="KB"&&typeStorage !="K"&&typeStorage !="B") {
        std::cout<<REDCOLORE<<  "ERROR : invalid value " <<"\"" <<str<<"\""<<std::endl;
        exit(0);
    }
    if (typeStorage =="GB"||typeStorage =="G") {
         maxBadysize = maxBadysize * 1024 * 1024 * 1024;
        if(maxBadysize > 10737418240) {
            std::cout<<REDCOLORE<< "Error: Invalid argument (the client_max_body_size exceeds the maximum size allowed by the server)"<<std::endl;
            exit(0);
        }
    }
    else if (typeStorage =="MB"||typeStorage =="M") {
        maxBadysize = maxBadysize * 1024 * 1024;
        if(maxBadysize > 10737418240) {
            std::cout<<REDCOLORE<< "Error: Invalid argument (the client_max_body_size exceeds the maximum size allowed by the server)"<<std::endl;
            exit(0);
        }
    }
    else if (typeStorage =="KB"||typeStorage =="K") {
         maxBadysize *= 1024;
        if(maxBadysize > 10737418240) {
            std::cout<<REDCOLORE<< "Error: Invalid argument (the client_max_body_size exceeds the maximum size allowed by the server)"<<std::endl;
            exit(0);
        }
    }
    if(maxBadysize > 10737418240) {
        std::cout<<REDCOLORE<< "Error: Invalid argument (the client_max_body_size exceeds the maximum size allowed by the server)"<<std::endl;
        exit(0);
    }
    if (maxBadysize==0) {
        std::cout<<REDCOLORE<< "Error: Invalid argument (client_max_body_size=0)"<<std::endl;
        exit(0);
    }
    return maxBadysize;
}

void ServerConfig :: locationNormal(std::string &location) {
    std::string tmp;
    std::vector <std::string> words;
    LocationConfig config;
    config._autoindex =false;
    config._client_max_body_size = this->client_max_body_size;
    config._allowed_methods = "GET POST DELETE";
    size_t pos = location.find("{");
    tmp = location.substr(8, pos - 8);
    std::string key = trim(tmp);
    words = splitstring(key);
    if (words.size()!=1) {
        std::cout<<REDCOLORE<< "Error: invalid location parameter "<<"\""<<key<<"\""<<std::endl;
        exit(0);
    }

    if (key.empty() || key[0]!='/') {
        std::cout<<REDCOLORE<< "Error: invalid location parameter "<<"\""<<key<<"\""<<std::endl;
        exit(0);
    }
    std::string str = location.substr(pos + 1);
    std::string line;
    std::set <std::string> word;
    std::istringstream stream(str);
    while (getline(stream, line)) {
        std::istringstream linestream(line);
        std::string firstWord;
        linestream >> firstWord;
        line = trim(line);
        if (!firstWord.empty()) {
            if (word.find(firstWord) != word.end()) {
                std::cout<<REDCOLORE<<  "Error: duplicate "<<firstWord<<"directive "<< std::endl;
                exit(0);
            }
            word.insert(firstWord);
        }
        if (firstWord == "root" || firstWord == "index" || firstWord == "autoindex"
            || firstWord == "upload_store"|| firstWord == "allowed_methods"
            || firstWord == "cgi_extension"|| firstWord == "return"
            || firstWord == "client_max_body_size") {
            size_t pos = line.find(";");
            if (pos == std::string::npos) {
                std::cout<<REDCOLORE<<  "ERROR : unexpected " <<"\"" <<firstWord <<"\""<< std::endl;
                exit(0);
            }
            tmp = line.substr(pos + 1);
            size_t startPos = line.find_first_of(" \t", firstWord.length());
            if (startPos != std::string::npos) {
                startPos = line.find_first_not_of(" \t", startPos);
            }
            std::string val = line.substr(startPos, pos - startPos);
            this->setValLocation(firstWord,val,config);
            checkcontent(tmp);
        } 
        else {
            checkcontent(line);
        }
    }
    this->location[key] = config;
}
void ServerConfig :: locationData(std::string &strlocat) {
	size_t i = 0;
    while(i<strlocat.length()) {
        size_t firstpos = strlocat.find("location",i);
        if (firstpos == std::string::npos) {
            return ;
        }
        size_t lastpos = strlocat.find("}",i);
        std::string location = strlocat.substr(firstpos,lastpos - firstpos);
        locationNormal(location);
        i = lastpos + 1;
    }

}
bool directoryExists(const std::string &path) {
    DIR *dir = opendir(path.c_str());
    if (dir) {
        closedir(dir);
        return true;
    } else {
        return false;
    }
}
void ServerConfig :: setValLocation(std::string &str,std::string &val,LocationConfig &config)
{
    if (config._upload_store.empty()) {
    if (!directoryExists(DEFAULTUPLOAD)) {
        std::cout<<REDCOLORE<< "Error: Unable to access default upload (Permission denied)"<<std::endl;
        exit(0);
    } 
        config._upload_store = DEFAULTUPLOAD;
    }
    if (str == "root") {
        if (splitstring(val).size()!=1) {
            std::cout<<REDCOLORE<< "ERROR : invalid \"root\" argument "<<std::endl;
            exit(0);
        }
        config._root = val;
    } else if (str== "index") {
        config._index = val;
    } else if (str== "autoindex") {
        if (val != "on" && val != "off") {
            std::cout<<REDCOLORE<< "ERROR : invalid value \""<<val<<"\""<<std::endl;
            exit(0);
        }
        else if (val=="on")
            config._autoindex = true;
        else
            config._autoindex = false;
    } else if (str== "upload_store") {
        if (splitstring(val).size()!=1) {
            std::cout<<REDCOLORE<< "ERROR : invalid \"upload_store\" argument"<<std::endl;
            exit(0);
        }
        if (!directoryExists(val)) {
            std::cout<<REDCOLORE<< "Error: Unable to access \""<<val<<"\" (Permission denied)"<<std::endl;
            exit(0);
        } 
        config._upload_store = val;
    } else if (str== "allowed_methods") {
        size_t i = 0;
        std::vector<std::string> words = splitstring(val);
        while (i < words.size()) {
            if (words[i] != "GET" && words[i] != "POST" && words[i] != "DELETE") {
                std::cout<<REDCOLORE<< "ERROR : unknown directive \"allowed_methods\" " << std::endl;
                exit(0);
            }
            i++;
        }
        config._allowed_methods = val;
    } else if (str== "cgi_extension") {
        size_t i = 0;
        std::vector<std::string> words = splitstring(val);
        while (i < words.size()) {
            if (words[i]!=".php"&&words[i]!=".py") {
                std::cout<< "ERROR : unknown directive \"cgi_extension\" " << std::endl;
                exit(0);
            }
            i++;
        }
        config._cgi_extension = val;
    } else if (str== "return") {
        std::string tmp;
        std::vector<std::string> words = splitstring(val);
        if (words.size()>1) {
            std::cout<<REDCOLORE<< "ERROR : invalid return "<<std::endl;
            exit(0);
        }
        config._return = words[0];
    } else if (str== "client_max_body_size") {
        config._client_max_body_size = checkValidBadySise(val);
    }
}
std::vector<std::string> splithost(std::string& input, char c) {
    std::vector<std::string> words;
    input = trim(input);
    std::stringstream ss(input);
    std::string str;
    
    while (getline(ss, str, c)) {
        words.push_back(str);
    }
    return words;
}

void ServerConfig :: setVal(std::string &str,std::string &val)
{
    if (str == "listen") {
        isNumber(val);
        int a = atoi(val.c_str());
        if (a==0) {
            std::cout<<REDCOLORE<< "ERROR : invalid port in  " <<"\"" <<a <<"\""<<" of the " <<"\"" <<str <<"\""<< std::endl;
            exit(0);
        }
        for(size_t i = 0;i<this->ports.size();i++)
        {
            if (this->ports[i]==a) {
               std::cout<<REDCOLORE<< "ERROR : duplicate listen options for "<<a<<std::endl;
                exit(0);
            }
        }
        this->ports.push_back(a);
        
    } else if (str == "server_name") {
        if (splitstring(val).size()!=1) {
            std::cout<<REDCOLORE<< "ERROR : invalid \"server_name\" argument "<<std::endl;
            exit(0);
        }
        this->serverName = val;
    } else if (str == "error_page") {
        std::vector <std::string > words;
        words = splitstring(val);
        if (words.size()==0 || words.size()==1) {
            std::cout<<REDCOLORE<< "ERROR : invalid number of arguments"<< std::endl;
            exit(0);
        }
        if (words.size()==2)
        {
            isNumberValid(words[0]);
            this->errorpage[words[0]] = words[1];
        }
        else {
            std::string key;
            size_t i = 0;
            while(i <(words.size() - 1)) {
                isNumberValid(words[i]);
                key +=words[i];
                key +=" ";
                i++;
            }
            this->errorpage[key] = words[words.size() - 1];
        }
    }
    else if (str == "host") {
        if (!isnumber(val[0])) {
            if (val!="localhost") {
                std::cout<<REDCOLORE<< "ERROR : unknown directive \"host\" "<< std::endl;
                exit(0);
            }
        }
        else {
            std::vector<std::string> format;
            size_t i = 0;
            format = splithost(val,'.');
            if (format.size() != 4 ) {
                std::cout<<REDCOLORE<< "ERROR : unknown directive \"host\" "<< std::endl;
                exit(0);
            }
            while (i<format.size())
            {
                numberHost(format[i]);
                i++;
            }
        }
        this->host = val;
    } else if (str == "client_max_body_size") {
        this->client_max_body_size = checkValidBadySise(val);
    } else if (str == "root") {
        if (splitstring(val).size()!=1) {
            std::cout<<REDCOLORE<< "ERROR : invalid \"root\" argument "<<std::endl;
            exit(0);
        }
        this->root = val;
    }
}
void isNumber(std::string& str) {
    if (str.empty()) {
        std::cout<<REDCOLORE<< "ERROR : invalid port in  " <<"\"" <<str <<"\""<<" of the " <<"\"listen\""<< std::endl;
        exit(0);
    }
    for (size_t i=0;i<str.length();i++) {
        if (!isdigit(str[i])) {
            std::cout<<REDCOLORE<<  "ERROR : invalid port in  " <<"\"" <<str <<"\""<<" of the " <<"\"listen\""<< std::endl;
            exit(0);
        }
    }
}
void numberHost(std::string& str) {
    if (str.empty()) {
        std::cout<<REDCOLORE<< "ERROR : unknown directive \"host\" "<< std::endl;
        exit(0);
    }
    for (size_t i=0;i<str.length();i++) {
        if (!isdigit(str[i])) {
            std::cout<<REDCOLORE<< "ERROR : unknown directive \"host\" "<< std::endl;
            exit(0);
        }
    }
}
void isNumberValid(std::string& str) {
    for (size_t i=0;i<str.length();i++) {
        if (!isdigit(str[i])) {
            std::cout<<REDCOLORE<<  "ERROR : invalid HTTP status code " <<"\""<<str<<"\""<< std::endl;
            exit(0);
        }
    }
}

void ServerConfig :: checkGlobalConfig(std::string strConfig) {
    std::string line;
    std::set <std::string> words;
    std::vector <std::string> valid;
    std::istringstream stream(strConfig);
    while (getline(stream, line)) {
        std::istringstream linestream(line);
        std::string firstWord;
        linestream >> firstWord;
        if (firstWord == "listen") {
            if (words.find(firstWord) == words.end()) {
                words.insert(firstWord);
            }
        }
        if (firstWord != "listen" && firstWord != "error_page" && !firstWord.empty()) {
            if (words.find(firstWord) != words.end()) {
                std::cout<<REDCOLORE<< "ERROR : unknown directive " <<"\"" <<firstWord <<"\""<< std::endl;
                exit(0);
            }
            words.insert(firstWord);
        }
        if (firstWord == "listen" || firstWord == "host"
            || firstWord == "client_max_body_size"
            || firstWord == "error_page" || firstWord == "root"|| firstWord == "server_name") {
            size_t pos = line.find(";");
            if (pos == std::string::npos) {
               std::cout<<REDCOLORE<< "ERROR : unexpected " <<"\"" <<firstWord <<"\""<< std::endl;
                exit(0);
            }
            std::string tmp = line.substr(pos + 1);
            size_t startPos = line.find_first_of(" \t", firstWord.length());
            if (startPos != std::string::npos) {
                startPos = line.find_first_not_of(" \t", startPos);
            }
            std::string val = line.substr(startPos, pos - startPos);
            checkcontent(tmp);
            this->setVal(firstWord,val);
            if (find(valid.begin(), valid.end(), firstWord) == valid.end()) {
                    valid.push_back(firstWord);
            }
        } 
        else {
            checkcontent(line);
        }
    }
    if (this->serverName.empty()) {
        words.insert("localhost");
        this->serverName = "localhost";
    }
    if (words.size() != 5) {
        std::cout<<REDCOLORE<< "ERROR : invalid number of arguments"<< std::endl;
        exit(0);
    }

}
void validbrackets(std::string &str) {
    int sig = 0;
    std::string tmp;
    size_t firstpos = 0;
    while (firstpos < str.length()) {
        size_t pos = str.find("{", firstpos);
        if (pos == std::string::npos) {
            break;
        }
        tmp = str.substr(firstpos, pos - firstpos);

        if (str.find('}') != std::string::npos) {
            sig++;
        }
        firstpos = pos + 1; 
    }
    size_t lastpos = 0;
    while (lastpos < str.length()) {
        size_t pos = str.find("}", lastpos);
        if (pos == std::string::npos) {
            break;
        }
        sig--;
        lastpos = pos + 1;
    }
    if (sig != 0) {
        std::cout<< REDCOLORE<<"Error: mismatched brackets" << std::endl;
        exit(0);
    }
}
void ServerConfig :: parseServerConfig(std::string &strdata) {
	size_t pos = strdata.find("server");
	size_t i = pos + 6;
	while(i < strdata.length() && strdata[i]!='{') {
		if (strdata[i]!='\t' &&  strdata[i]!=' '&& strdata[i]!='\n') {
            size_t pos = strdata.find_first_of(" \t\n{", i);
			std::cout<<REDCOLORE<< "ERROR : unexpected "<<"\""<<strdata.substr(i,pos-i)<<"\""<< std::endl;
			exit(0);
		}
		i++;
	}
	i++;
	size_t  pos1 = strdata.rfind('}',i);
	std::string data = strdata.substr(i,pos1 - i);
	std::string strConfig = removeLocationBlocks(data);
	this->checkGlobalConfig(strConfig);
	pos1 = strdata.find("location",i);
	if (pos1 != std::string::npos) {
		std::string loca  = strdata.substr(pos1);
		locationData(loca);   
	}
}
