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
            cout<<"content not valid"<<endl;
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
        cout<<"error client_max_body_size not valid"<<endl;
        exit(0);
    }
    if (i==0) {
        cout<<"error client_max_body_size not valid"<<endl;
        exit(0);
    }
    maxBadysize = static_cast<size_t>(stoll(number));

    typeStorage = str.substr(i);
    if (typeStorage !="GB"&&typeStorage !="G"&&typeStorage !="MB"&&typeStorage !="M"&&typeStorage !="KB"&&typeStorage !="K"&&typeStorage !="B") {
        cout<<"error client_max_body_size not valid"<<endl;
        exit(0);
    }
    if (typeStorage =="GB"||typeStorage =="G") {
         maxBadysize = maxBadysize * 1024 * 1024 * 1024;
        if(maxBadysize > 10737418240) {
            cout<<"error client_max_body_size > valid size"<<endl;
            exit(0);
        }
    }
    else if (typeStorage =="MB"||typeStorage =="M") {
        maxBadysize = maxBadysize * 1024 * 1024;
        if(maxBadysize > 10737418240) {
            cout<<"error client_max_body_size > valid size"<<endl;
            exit(0);
        }
    }
    else if (typeStorage =="KB"||typeStorage =="K") {
         maxBadysize *= 1024;
        if(maxBadysize > 10737418240) {
            cout<<"error client_max_body_size > valid size"<<endl;
            exit(0);
        }
    }
    if(maxBadysize > 10737418240) {
        cout<<"error client_max_body_size > valid size"<<endl;
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
        cout <<"error location path not valid "<<endl;
        exit(0);
    }

    if (key.empty() || key[0]!='/') {
        cout <<"error location path not valid "<<endl;
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
                cout << "Error: Duplicate parameter found - " << firstWord << endl;
                exit(1);
            }
            word.insert(firstWord);
        }
        if (firstWord == "root" || firstWord == "index" || firstWord == "autoindex"
            || firstWord == "upload_store"|| firstWord == "allowed_methods"
            || firstWord == "cgi_extension"|| firstWord == "return"
            || firstWord == "client_max_body_size") {
            size_t pos = line.find(";");
            if (pos == string::npos) {
                cout << "; not found" << endl;
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
void ServerConfig :: setValLocation(string &str,string &val,LocationConfig &config)
{
    if (str == "root") {
        config._root = val;
    } else if (str== "index") {
         config._index = val;
    } else if (str== "autoindex") {
        if (val != "on" && val != "off") {
            cout <<"error autoindex"<<endl;
            exit(0);
        }
        else if (val=="on")
            config._autoindex = true;
        else
            config._autoindex = false;
    } else if (str== "upload_store") {
        config._upload_store= val;
    } else if (str== "allowed_methods") {
        config._allowed_methods = val;
    } else if (str== "cgi_extension") {
        config._cgi_extension = val;
    } else if (str== "return") {
    string tmp;
    vector<string> words = splitstring(val);
    if (words.size()!=2) {
        cout<<"error redirection size?? "<<endl;
        exit(0);
    }
    if (words[1].length()>8)
        tmp = words[1].substr(0,8);
    if (words[1][0]=='/') {
        this->typeUrl = 1;
    } else if (tmp=="http://"||tmp=="https://") { //
        this->typeUrl = 2;
    } else {
        cout<<"error url not valid"<<endl;
        exit(0);
    }
        config._return = words[1];
    } else if (str== "client_max_body_size") {
        config._client_max_body_size = checkValidBadySise(val);
    }
}


void ServerConfig :: setVal(string &str,string &val)
{
    if (str == "listen") {
        isNumber(val);
        int a = atoi(val.c_str());
        if (a==0) {
            cout <<"error port or error_page number not valid "<<val<<endl;
            exit(0);
        }
        for(size_t i = 0;i<this->ports.size();i++)
        {
            if (this->ports[i]==a) {
                cout << "error: duplicate port: " << val << endl;
                exit(0);
            }
        }
        this->ports.push_back(a);
        
    }else if (str == "error_page") {
        vector <string > words;
        words = splitstring(val);
        if (words.size()==2)
        {
            isNumber(words[0]);
            this->errorpage[words[0]] = words[1];
        }
        else {
            string key;
            size_t i = 0;
            while(i <(words.size() - 1)) {
                isNumber(words[i]);
                key +=words[i];
                key +=" ";
                i++;
            }
            if (i==0) {
                cout<<"error_page not valid"<<endl;
                exit(0);
            }
            this->errorpage[key] = words[words.size() - 1];
        }
    }
    else if (str == "host") {
        this->host = val;
    } else if (str == "client_max_body_size") {
        this->client_max_body_size = checkValidBadySise(val);
    } else if (str == "index") {
        this->index = val;
    } else if (str == "root") {
        this->root = val;
    } else if (str == "server_name") {
        this->server_name = val;
    }
}
void isNumber(string& str) {
    if (str.empty()) {
        cout <<"error port or error_page number not valid "<<str<<endl;
        exit(0);
    }
    for (int i=0;i<str.length();i++) {
        if (!isdigit(str[i])) {
            cout <<"error port or error_page number not valid "<<str<<endl;
            exit(0);
        }
    }
}

void ServerConfig :: checkGlobalConfig(string strConfig) {
    strConfig.pop_back();
        // cout <<strConfig<<endl;
    string line;
    set <string> words;
    vector <string> valid;
    istringstream stream(strConfig);
    while (getline(stream, line)) {
        istringstream linestream(line);
        string firstWord;
        linestream >> firstWord;
        if (firstWord != "listen" && firstWord != "error_page" && !firstWord.empty()) {
            if (words.find(firstWord) != words.end()) {
                cout << "Error: Duplicate parameter found - " << firstWord << endl;
                exit(1);
            }
            words.insert(firstWord);
        }
        if (firstWord == "listen" || firstWord == "host"
            || firstWord == "server_name" || firstWord == "client_max_body_size"
            || firstWord == "error_page" || firstWord == "root"|| firstWord == "index") {
            size_t pos = line.find(";");
            if (pos == string::npos) {
                cout << "; not found" << endl;
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
   if (valid.size() < 5) {
    cout <<"error default param not found"<<endl;
    exit(0);
   }
}
void validbrackets(string &str) {
    int sig = 0;
    string tmp;
    size_t firstpos =0;
    size_t lastBra = str.rfind("}");
    if (lastBra != string::npos && lastBra < str.length() - 1) {
        tmp = str.substr(lastBra + 1);
        if (checkCharacter(tmp,'}')) {
            cout << "error data after last bracket " << endl;
            exit(1);
        }
    }
    while (firstpos < str.length()) {
        size_t pos = str.find("{", firstpos);
        size_t pos1 = str.find("}", firstpos);
        if (pos==string::npos) {
            break ;
        }
        // checkcontent(str.substr(pos + 1, pos1 - pos - 1));
        tmp = str.substr(firstpos,pos - firstpos);
        if (checkCharacter(tmp,'}'))
            sig++;
        else{
            cout<<"error bracket whitout name"<<endl;
            exit(1);
        }
        firstpos = pos + 1;
        
    }
    size_t lastpos =0;
    while (lastpos < str.length()) {
        size_t pos = str.find("}", lastpos);
        if (pos==string::npos) {
            break ;
        }
        else {
            sig--;
        }
        lastpos = pos + 1;
    }
    if (sig!=0) {
        cout<<"error brackets"<<endl;
        exit(1);
    }
}
void ServerConfig :: parseServerConfig(string &strdata) {
	size_t pos = strdata.find("server");
	size_t i = pos + 6;
	while(i < strdata.length() && strdata[i]!='{') {
		if (strdata[i]!='\t' &&  strdata[i]!=' '&& strdata[i]!='\n') {
			cout << "error" << endl;
			exit(0);
		}
		i++;
	}
    if (i==strdata.size()) {
        cout <<"error server without brakets"<<endl;
        exit(0);
    }
	i++;
	
	size_t  pos1 = strdata.rfind('}',i);
	string data = strdata.substr(i,pos1 - i);
	string strConfig = removeLocationBlocks(data);
    // cout <<strConfig<<endl;
	this->checkGlobalConfig(strConfig);
	pos1 = strdata.find("location",i);
	if (pos1 != string::npos) {
		string loca  = strdata.substr(pos1);
		locationData(loca);   
	}
}
void findLocation(string &str)
{
    int i = 0;
    bool sig = false;
    size_t pos  = str.find("location");
    while (i< str.length()) {
        if (isspace(str[i])) {
            sig = false;
        } else {
            sig = true;
            break;
        }
        i++;
    }
    if (sig == true && pos ==string::npos) {
        cout <<"error name of blocks not correct"<<endl;
        exit(0);
    }
}
void ServerConfig :: nameBlocks(string &strdata) {
    size_t pos = strdata.size();
    size_t i = 0;
    while ((pos = strdata.rfind("{", pos)) != string::npos) {
        if (pos == 0)
            break;
        i = pos -1;
        while (i != 0) {
            if (strdata[i] != '{' && strdata[i] != ';' && strdata[i] != '}') {
                i--;
            } else {
                break;
            }
        }
        if (i != 0)
        {
           string str = strdata.substr(i + 1, pos - i - 1);
            findLocation(str);
        }
        pos--;
    }
}