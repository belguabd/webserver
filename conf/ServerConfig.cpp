#include "ServerConfig.hpp"
#include <iostream>
#include <set> // Use set instead of vector
#include <sstream>
#include <string>
void    checkcontent(string substr)
{
    int i =0 ;
    while(i < substr.length()) {
        if (substr[i]=='\t' ||  substr[i]==' '|| substr[i]=='\n')
            i++;
        else {
            cout <<REDCOLORE<< "ERROR : invalid arguments "<<endl;
            exit(0);
        }
    }
}
void chechAllowedMethodValid(string &str) {
    vector <string> words;
    words = splitstring(str);
    for (size_t i = 0; i < words.size(); i++) {
        if (words[i]!="GET" && words[i]!="POST" && words[i]!="DELETE") {
            cout<<"error method not valid"<<endl;
            exit(0);
        }
    }  
}

bool checkCharacter(string& substr,char c) {
	int i =0 ;
	while(i < substr.length()) {
		if (substr[i]!='\t' &&  substr[i]!=' '&& substr[i]!='\n' && substr[i]!=c)
			return true;
		i++;
	}
	return false;
}
string trim(string &str) {
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
string removeComments(string& input) {
    stringstream result;
    
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

string removeLocationBlocks(string& configData) {
	string result = configData;
    // cout <<configData;
	// Regex pattern to match "location { ... }" blocks
	string locationRegexPattern = "\\s*location\\s+[^}]*\\{[^}]*\\}\\s*";
	regex locationRegex(locationRegexPattern);
	result = regex_replace(result, locationRegex, "");  
	return result;
}

ServerConfig :: ServerConfig(string &str) {
    this->autoindex = false;
    this->data = str;
    this->_allowed_methods = "GET POST DELETE";
}

ServerConfig :: ~ServerConfig() { }

size_t checkValidBadySise(string str)
{
    size_t maxBadysize;
    string number;
    string typeStorage;
    int i = 0;
    while (i < str.length()) {
        if(isdigit(str[i]))
            number += str[i];
        else
            break;
        i++;
    }
    if (i== str.length()) {
        cout <<REDCOLORE<< "ERROR : invalid value " <<"\"" <<str<<"\""<<endl;
        exit(0);
    }
    if (i==0) {
        cout <<REDCOLORE<<  "ERROR : invalid value " <<"\"" <<str<<"\""<<endl;
        exit(0);
    }
    maxBadysize = static_cast<size_t>(stoll(number));

    typeStorage = str.substr(i);
    if (typeStorage !="GB"&&typeStorage !="G"&&typeStorage !="MB"&&typeStorage !="M"&&typeStorage !="KB"&&typeStorage !="K"&&typeStorage !="B") {
        cout <<REDCOLORE<<  "ERROR : invalid value " <<"\"" <<str<<"\""<<endl;
        exit(0);
    }
    if (typeStorage =="GB"||typeStorage =="G") {
         maxBadysize = maxBadysize * 1024 * 1024 * 1024;
        if(maxBadysize > 10737418240) {
            cout <<REDCOLORE<< "Error: Invalid argument (the client_max_body_size exceeds the maximum size allowed by the server)"<<endl;
            exit(0);
        }
    }
    else if (typeStorage =="MB"||typeStorage =="M") {
        maxBadysize = maxBadysize * 1024 * 1024;
        if(maxBadysize > 10737418240) {
            cout <<REDCOLORE<< "Error: Invalid argument (the client_max_body_size exceeds the maximum size allowed by the server)"<<endl;
            exit(0);
        }
    }
    else if (typeStorage =="KB"||typeStorage =="K") {
         maxBadysize *= 1024;
        if(maxBadysize > 10737418240) {
            cout <<REDCOLORE<< "Error: Invalid argument (the client_max_body_size exceeds the maximum size allowed by the server)"<<endl;
            exit(0);
        }
    }
    if(maxBadysize > 10737418240) {
        cout <<REDCOLORE<< "Error: Invalid argument (the client_max_body_size exceeds the maximum size allowed by the server)"<<endl;
        exit(0);
    }
    if (maxBadysize==0) {
        cout <<REDCOLORE<< "Error: Invalid argument (client_max_body_size=0)"<<endl;
        exit(0);
    }
    return maxBadysize;
}

void ServerConfig :: locationNormal(string &location) {
    string tmp;
    vector <string> words;
    LocationConfig config;
    size_t pos = location.find("{");
    tmp = location.substr(8, pos - 8);
    string key = trim(tmp);
    words = splitstring(key);
    if (words.size()!=1) {
        cout <<REDCOLORE<< "Error: invalid location parameter "<<"\""<<key<<"\""<<endl;
        exit(0);
    }

    if (key.empty() || key[0]!='/') {
        cout <<REDCOLORE<< "Error: invalid location parameter "<<"\""<<key<<"\""<<endl;
        exit(0);
    }
    string str = location.substr(pos + 1);
    string line;
    set <string> word;
    istringstream stream(str);
    while (getline(stream, line)) {
        istringstream linestream(line);
        string firstWord;
        linestream >> firstWord;
        line = trim(line);
        if (!firstWord.empty()) {
            if (word.find(firstWord) != word.end()) {
                cout <<REDCOLORE<<  "Error: duplicate "<<firstWord<<"directive "<< endl;
                exit(0);
            }
            word.insert(firstWord);
        }
        if (firstWord == "root" || firstWord == "index" || firstWord == "autoindex"
            || firstWord == "upload_store"|| firstWord == "allowed_methods"
            || firstWord == "cgi_extension"|| firstWord == "return"
            || firstWord == "client_max_body_size") {
            size_t pos = line.find(";");
            if (pos == string::npos) {
                cout <<REDCOLORE<<  "ERROR : unexpected " <<"\"" <<firstWord <<"\""<< endl;
                exit(0);
            }
            tmp = line.substr(pos + 1);
            size_t startPos = line.find_first_of(" \t", firstWord.length());
            if (startPos != string::npos) {
                startPos = line.find_first_not_of(" \t", startPos);
            }
            string val = line.substr(startPos, pos - startPos);
            this->setValLocation(firstWord,val,config);
            checkcontent(tmp);
        } 
        else {
            checkcontent(line);
        }
    }
    this->location[key] = config;
}
void ServerConfig :: locationData(string &strlocat) {
	size_t i = 0;
    while(i<strlocat.length()) {
        size_t firstpos = strlocat.find("location",i);
        if (firstpos == string::npos) {
            return ;
        }
        size_t lastpos = strlocat.find("}",i);
        string location = strlocat.substr(firstpos,lastpos - firstpos);
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
void ServerConfig :: setValLocation(string &str,string &val,LocationConfig &config)
{
    if (config._client_max_body_size==0) {
            config._client_max_body_size = this->client_max_body_size;
    }
    if (str == "root") {
        if (splitstring(val).size()!=1) {
            cout <<REDCOLORE<< "ERROR : invalid \"root\" argument "<<endl;
            exit(0);
        }
        config._root = val;
    } else if (str== "index") {
        config._index = val;
    } else if (str== "autoindex") {
        if (val != "on" && val != "off") {
            cout <<REDCOLORE<< "ERROR : invalid value \""<<val<<"\""<<endl;
            exit(0);
        }
        else if (val=="on")
            config._autoindex = true;
        else
            config._autoindex = false;
    } else if (str== "upload_store") {
        if (splitstring(val).size()!=1) {
            cout <<REDCOLORE<< "ERROR : invalid \"upload_store\" argument"<<endl;
            exit(0);
        }
        if (!directoryExists(val)) {
            cout <<REDCOLORE<< "Error: Unable to access \""<<val<<"\" (Permission denied)"<<endl;
            exit(0);
        } 
        config._upload_store = val;
    } else if (str== "allowed_methods") {
        int i = 0;
        vector<string> words = splitstring(val);
        while (i < words.size()) {
            if (words[i] != "GET" && words[i] != "POST" && words[i] != "DELETE") {
                cout <<REDCOLORE<< "ERROR : unknown directive \"allowed_methods\" " << endl;
                exit(0);
            }
            i++;
        }
        config._allowed_methods = val;
    } else if (str== "cgi_extension") {
        int i = 0;
        vector<string> words = splitstring(val);
        while (i < words.size()) {
            if (words[i]!=".php"&&words[i]!=".py") {
                cout << "ERROR : unknown directive \"cgi_extension\" " << endl;
                exit(0);
            }
            i++;
        }
        config._cgi_extension = val;
    } else if (str== "return") {
        string tmp;
        vector<string> words = splitstring(val);
        if (words.size()>1) {
            cout <<REDCOLORE<< "ERROR : invalid return "<<endl;
            exit(0);
        }
        if (words[0].length()>8)
            tmp = words[0].substr(0,8);
        if (words[0][0]=='/') {
            this->typeUrl = 1;
        } else if (tmp=="http://"||tmp=="https://") { //
            this->typeUrl = 2;
        } else {
            cout <<REDCOLORE<< "ERROR : invalid return "<<endl;
            exit(0);
        }
        config._return = words[0];
    } else if (str== "client_max_body_size") {
        config._client_max_body_size = checkValidBadySise(val);
    }
}
vector<string> splithost(string& input, char c) {
    vector<string> words;
    input = trim(input);
    stringstream ss(input);
    string str;
    
    while (getline(ss, str, c)) {
        words.push_back(str);
    }
    return words;
}

void ServerConfig :: setVal(string &str,string &val)
{
    if (str == "listen") {
        isNumber(val);
        int a = atoi(val.c_str());
        if (a==0) {
            cout <<REDCOLORE<< "ERROR : invalid port in  " <<"\"" <<a <<"\""<<" of the " <<"\"" <<str <<"\""<< endl;
            exit(0);
        }
        for(size_t i = 0;i<this->ports.size();i++)
        {
            if (this->ports[i]==a) {
               cout <<REDCOLORE<< "ERROR : duplicate listen options for "<<a<<endl;
                exit(0);
            }
        }
        this->ports.push_back(a);
        
    }else if (str == "error_page") {
        vector <string > words;
        words = splitstring(val);
        if (words.size()==0 || words.size()==1) {
            cout <<REDCOLORE<< "ERROR : invalid number of arguments"<< endl;
            exit(0);
        }
        if (words.size()==2)
        {
            isNumberValid(words[0]);
            this->errorpage[words[0]] = words[1];
        }
        else {
            string key;
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
                cout <<REDCOLORE<< "ERROR : unknown directive \"host\" "<< endl;
                exit(0);
            }
        }
        else {
            vector<string> format;
            int i = 0;
            format = splithost(val,'.');
            if (format.size() != 4 ) {
                cout <<REDCOLORE<< "ERROR : unknown directive \"host\" "<< endl;
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
            cout <<REDCOLORE<< "ERROR : invalid \"root\" argument "<<endl;
            exit(0);
        }
        this->root = val;
    }
}
void isNumber(string& str) {
    if (str.empty()) {
        cout <<REDCOLORE<< "ERROR : invalid port in  " <<"\"" <<str <<"\""<<" of the " <<"\"listen\""<< endl;
        exit(0);
    }
    for (int i=0;i<str.length();i++) {
        if (!isdigit(str[i])) {
            cout <<REDCOLORE<<  "ERROR : invalid port in  " <<"\"" <<str <<"\""<<" of the " <<"\"listen\""<< endl;
            exit(0);
        }
    }
}
void numberHost(string& str) {
    if (str.empty()) {
        cout <<REDCOLORE<< "ERROR : unknown directive \"host\" "<< endl;
        exit(0);
    }
    for (int i=0;i<str.length();i++) {
        if (!isdigit(str[i])) {
            cout <<REDCOLORE<< "ERROR : unknown directive \"host\" "<< endl;
            exit(0);
        }
    }
}
void isNumberValid(string& str) {
    for (int i=0;i<str.length();i++) {
        if (!isdigit(str[i])) {
            cout <<REDCOLORE<<  "ERROR : invalid HTTP status code " <<"\""<<str<<"\""<< endl;
            exit(0);
        }
    }
}

void ServerConfig :: checkGlobalConfig(string strConfig) {
    string line;
    set <string> words;
    vector <string> valid;
    strConfig.pop_back();
    istringstream stream(strConfig);
    while (getline(stream, line)) {
        istringstream linestream(line);
        string firstWord;
        linestream >> firstWord;
        if (firstWord == "listen") {
            if (words.find(firstWord) == words.end()) {
                words.insert(firstWord);
            }
        }
        if (firstWord != "listen" && firstWord != "error_page" && !firstWord.empty()) {
            if (words.find(firstWord) != words.end()) {
                cout <<REDCOLORE<< "ERROR : unknown directive " <<"\"" <<firstWord <<"\""<< endl;
                exit(1);
            }
            words.insert(firstWord);
        }
        if (firstWord == "listen" || firstWord == "host"
            || firstWord == "client_max_body_size"
            || firstWord == "error_page" || firstWord == "root") {
            size_t pos = line.find(";");
            if (pos == string::npos) {
               cout <<REDCOLORE<< "ERROR : unexpected " <<"\"" <<firstWord <<"\""<< endl;
                exit(0);
            }
            string tmp = line.substr(pos + 1);
            size_t startPos = line.find_first_of(" \t", firstWord.length());
            if (startPos != string::npos) {
                startPos = line.find_first_not_of(" \t", startPos);
            }
            string val = line.substr(startPos, pos - startPos);
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
    if (words.size() != 4) {
        cout <<REDCOLORE<< "ERROR : invalid number of arguments"<< endl;
        exit(0);
    }

}
void validbrackets(string &str) {
    int sig = 0;
    string tmp;
    size_t firstpos = 0;
    while (firstpos < str.length()) {
        size_t pos = str.find("{", firstpos);
        if (pos == string::npos) {
            break;
        }
        tmp = str.substr(firstpos, pos - firstpos);

        if (str.find('}') != string::npos) {
            sig++;
        }
        firstpos = pos + 1; 
    }
    size_t lastpos = 0;
    while (lastpos < str.length()) {
        size_t pos = str.find("}", lastpos);
        if (pos == string::npos) {
            break;
        }
        sig--;
        lastpos = pos + 1;
    }
    if (sig != 0) {
        cout << "Error: mismatched brackets" << endl;
        exit(0);
    }
}
void ServerConfig :: parseServerConfig(string &strdata) {
	size_t pos = strdata.find("server");
	size_t i = pos + 6;
	while(i < strdata.length() && strdata[i]!='{') {
		if (strdata[i]!='\t' &&  strdata[i]!=' '&& strdata[i]!='\n') {
            size_t pos = strdata.find_first_of(" \t\n{", i);
			cout <<REDCOLORE<< "ERROR : unexpected "<<"\""<<strdata.substr(i,pos-i)<<"\""<< endl;
			exit(0);
		}
		i++;
	}
	i++;
	size_t  pos1 = strdata.rfind('}',i);
	string data = strdata.substr(i,pos1 - i);
	string strConfig = removeLocationBlocks(data);
	this->checkGlobalConfig(strConfig);
	pos1 = strdata.find("location",i);
	if (pos1 != string::npos) {
		string loca  = strdata.substr(pos1);
		locationData(loca);   
	}
}
