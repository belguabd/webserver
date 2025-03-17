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

void ServerConfig :: locationRedirection(string &location) {
    string tmp;
    vector <string> words;
    size_t returnPos;
    string val;
    size_t pos = location.find("{");
    tmp =location.substr(8, pos - 8);

    string key = trim(tmp);
    words = splitstring(key);
    if (words.size()!=1) {
        cout <<"error location path not valid "<<endl;
        exit(0);
    }
    if (key.empty()|| key[0]!='/') {
        cout <<"error location path not valid "<<endl;
        exit(0);
    }
    int cont = 0;
    returnPos = location.find("return");
    while((location.find("return",returnPos))!=string::npos) {
        if (location[returnPos + 6 ]== ' '||location[returnPos + 6] == '\t')
            cont++;
        returnPos+=1;
    }
    if (cont>1) {
        cout<<"error redirection > 1"<<endl;
        exit(0);
    }
    size_t valueStart = location.find(" ", returnPos + 1);
    size_t valueEnd = location.find(";", valueStart);
    tmp =location.substr(valueStart + 1, valueEnd - valueStart - 1);
    words = splitstring(tmp);
    if (words.size()!=2) {
        cout<<"error redirection size?? "<<endl;
        exit(0);
    }
    val = words[1];
    if (val.length()>8)
        tmp = val.substr(0,8);
    if (val[0]=='/') {
        this->typeUrl = 1;
    } else if (tmp=="http://"||tmp=="https://") { //
        this->typeUrl = 2;
    } else {
        cout<<"error url not valid"<<endl;
        exit(0);
    }
    this->configRedirection[key] = val;
}

void ServerConfig ::locationCgi(string &location) {
    string tmp;
        vector <string> words;
    size_t pos = location.find("{");
    tmp = location.substr(8, pos - 8);
    string str = location.substr(pos + 1);
    checkContentLocationCgi(str);
    string key = trim(tmp);
    words = splitstring(key);
    if (words.size()!=1) {
        cout <<"error location path not valid "<<endl;
        exit(0);
    }

    if (key.empty()|| key[0]!='/') {
        cout <<"error location path not valid "<<endl;
        exit(0);
    }
    LocationCgi cgi;
    size_t rootPos = location.find("root");
    size_t allowedMethodsPos = location.find("allowed_methods");
    size_t extensionPos = location.find("cgi_extension");
    size_t cgihandlerPos = location.find("cgi_handler");
    if (rootPos != string::npos) {
        size_t valueStart = location.find(" ", rootPos + 4);
        size_t valueEnd = location.find(";", valueStart);
        tmp = location.substr(valueStart + 1, valueEnd - valueStart - 1);
        cgi.root = trim(tmp);
    }
    if (allowedMethodsPos != string::npos) {
        size_t valueStart = location.find(" ", allowedMethodsPos + 15);
        size_t valueEnd = location.find(";", valueStart);
        tmp = location.substr(valueStart + 1, valueEnd - valueStart - 1);
        chechAllowedMethodValid(tmp);
        cgi.allowed_methods = trim(tmp);
    }
    if (extensionPos != string::npos) {
        size_t valueStart = location.find(" ", extensionPos + 13);
        size_t valueEnd = location.find(";", valueStart);
        tmp = location.substr(valueStart + 1, valueEnd - valueStart - 1);
        cgi.cgi_extension = trim(tmp);
    }
    if (cgihandlerPos != string::npos) {
        size_t valueStart = location.find(" ", cgihandlerPos + 11);
        size_t valueEnd = location.find(";", valueStart);
        tmp = location.substr(valueStart + 1, valueEnd - valueStart - 1);
        cgi.cgi_handler = trim(tmp);
    }
	this->configcgi[key] = cgi;
    // cout << "Location: " << key << "\n";
    // cout << "size: " << this->configcgi.size() << "\n";
    // cout << "Root: " << cgi.root << "\n";
    // cout << "Allowed Methods: " << cgi.allowed_methods << "\n";
    // cout << "Upload Store: " << cgi.cgi_extension << "\n";
    // cout << "Client Max Body Size: " << cgi.cgi_handler << "\n";
    // cout <<"-------++++++++++++++++++-----------\n";
}
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

void ServerConfig :: locationUpload(string &location) {
    string tmp;
    vector <string> words;
    size_t pos = location.find("{");
    tmp =location.substr(8, pos - 8);
    string str = location.substr(pos + 1);
    checkContentLocationUpload(str);
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
    LocationUplaods config;
    size_t rootPos = location.find("root");
    size_t allowedMethodsPos = location.find("allowed_methods");
    size_t indexPos = location.find("index");
    size_t uploadStorePos = location.find("upload_store");
    size_t clientMaxBodySizePos = location.find("client_max_body_size");

    if (rootPos != string::npos) {
        size_t valueStart = location.find(" ", rootPos + 4);
        size_t valueEnd = location.find(";", valueStart);
        tmp =location.substr(valueStart + 1, valueEnd - valueStart - 1);
        config.root = trim(tmp);
    }

    if (allowedMethodsPos != string::npos) {
        size_t valueStart = location.find(" ", allowedMethodsPos + 15);
        size_t valueEnd = location.find(";", valueStart);
        tmp = location.substr(valueStart + 1, valueEnd - valueStart - 1);
        chechAllowedMethodValid(tmp);
        config.allowed_methods = trim(tmp);
    }
    size_t indexSearchPos = 0;
    while ((indexPos = location.find("index", indexSearchPos)) != string::npos) {
        indexSearchPos = indexPos + 5;
        if (location.compare(indexPos - 4, 9, "autoindex") != 0) {
            size_t valueStart = location.find(" ", indexPos + 5);
            size_t valueEnd = location.find(";", valueStart);
            tmp = location.substr(valueStart + 1, valueEnd - valueStart - 1);
            config.index = trim(tmp);
            break;
        }
    }
    if (uploadStorePos != string::npos) {
        size_t valueStart = location.find(" ", uploadStorePos + 12);
        size_t valueEnd = location.find(";", valueStart);
        tmp = location.substr(valueStart + 1, valueEnd - valueStart - 1);
        config.upload_store = trim(tmp);
    }

    if (clientMaxBodySizePos != string::npos) {
        size_t valueStart = location.find(" ", clientMaxBodySizePos + 18);
        size_t valueEnd = location.find(";", valueStart);
        tmp = location.substr(valueStart + 1, valueEnd - valueStart - 1);
        config.client_max_body_size = checkValidBadySise(trim(tmp));
    }
	this->configUpload[key] = config;
}

void    checkContentLocationNormal(string &str)
{
    string line;
    set <string> words;
    istringstream stream(str);
    while (getline(stream, line)) {
        istringstream linestream(line);
        string firstWord;
        linestream >> firstWord;

        if (!firstWord.empty()) {
            if (words.find(firstWord) != words.end()) {
                cout << "Error: Duplicate parameter found - " << firstWord << endl;
                exit(1);
            }
            words.insert(firstWord);
        }
        if (firstWord == "root" || firstWord == "allowed_methods" || firstWord == "index"||firstWord == "autoindex") {
            size_t pos = line.find(";");
            if (pos == string::npos) {
                cout << "; not found" << endl;
                exit(0);
            }
            string tmp = line.substr(pos + 1);
            checkcontent(tmp);
        } 
        else {
            checkcontent(line);
        }
    }
}
void    checkContentLocationUpload(string &str)
{
    string line;
    set <string> words;
    istringstream stream(str);
    while (getline(stream, line)) {
        istringstream linestream(line);
        string firstWord;
        linestream >> firstWord;

        if (!firstWord.empty()) {
            if (words.find(firstWord) != words.end()) {
                cout << "Error: Duplicate parameter found - " << firstWord << endl;
                exit(1);
            }
            words.insert(firstWord);
        }
        if (firstWord == "root" || firstWord == "allowed_methods" || firstWord == "upload_store"||firstWord == "index"||firstWord == "client_max_body_size") {
            size_t pos = line.find(";");
            if (pos == string::npos) {
                cout << "; not found" << endl;
                exit(0);
            }
            string tmp = line.substr(pos + 1);
            checkcontent(tmp);
        } 
        else {
            checkcontent(line);
        }
    }
}
void    checkContentLocationCgi(string &str)
{
    string line;
    set <string> words;
    istringstream stream(str);
    while (getline(stream, line)) {
        istringstream linestream(line);
        string firstWord;
        linestream >> firstWord;

        if (!firstWord.empty()) {
            if (words.find(firstWord) != words.end()) {
                cout << "Error: Duplicate parameter found - " << firstWord << endl;
                exit(1);
            }
            words.insert(firstWord);
        }
        if (firstWord == "root" || firstWord == "allowed_methods" || firstWord == "cgi_extension") {
            size_t pos = line.find(";");
            if (pos == string::npos) {
                cout << "; not found" << endl;
                exit(0);
            }
            string tmp = line.substr(pos + 1);
            checkcontent(tmp);
        } 
        else {
            checkcontent(line);
        }
    }
}

void ServerConfig :: locationNormal(string &location) {
    string tmp;
    vector <string> words;
    size_t pos = location.find("{");
    tmp = location.substr(8, pos - 8);
    string str = location.substr(pos + 1);
    checkContentLocationNormal(str);

    string key = trim(tmp);
    words = splitstring(key);
    if (words.size()!=1) {
        cout <<"error location path not valid "<<endl;
        exit(0);
    }

    if (key.empty()|| key[0]!='/') {
        cout <<"error location path not valid "<<endl;
        exit(0);
    }
    LocationConfig config;
    size_t rootPos = location.find("root");
    size_t allowedMethodsPos = location.find("allowed_methods");
    size_t indexPos = location.find("index");
    size_t autoindexPos = location.find("autoindex");
    if (rootPos != string::npos) {
        size_t valueStart = location.find(" ", rootPos + 4);
        size_t valueEnd = location.find(";", valueStart);
        tmp =location.substr(valueStart + 1, valueEnd - valueStart - 1);
        config.root = trim(tmp);
        
    }
    if (allowedMethodsPos != string::npos) {
        size_t valueStart = location.find(" ", allowedMethodsPos + 15);
        size_t valueEnd = location.find(";", valueStart);
        tmp =location.substr(valueStart + 1, valueEnd - valueStart - 1);
        chechAllowedMethodValid(tmp);
        config.allowed_methods = trim(tmp);
    }
    size_t indexSearchPos = 0;
    while ((indexPos = location.find("index", indexSearchPos)) != string::npos) {
        indexSearchPos = indexPos + 5;
        if (location.compare(indexPos - 4, 9, "autoindex") != 0) {
            size_t valueStart = location.find(" ", indexPos + 5);
            size_t valueEnd = location.find(";", valueStart);
            tmp = location.substr(valueStart + 1, valueEnd - valueStart - 1);
            config.index = trim(tmp);
            break;
        }
    }
    if (autoindexPos != string::npos) {
        size_t valueStart = location.find(" ", autoindexPos + 9);
        size_t valueEnd = location.find(";", valueStart);
        tmp = location.substr(valueStart + 1, valueEnd - valueStart - 1);
        tmp = trim(tmp);
        if (tmp!="on"&&tmp!="off") {
            cout <<"error autoindex"<<endl;
            exit(0);
        } else if (tmp =="on") {
            config.autoindex = true;
        } else {
            config.autoindex = 0;
        }
    }
	this->configNormal[key] = config;
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
        if(location.find("upload_store")!=string::npos) {
            locationUpload(location);
        } else if (location.find("return")!=string::npos) {
            locationRedirection(location);
        } else if (location.find("cgi_extension")!=string::npos) {
            locationCgi(location);
        } else {
            locationNormal(location);
        }
        i = lastpos+1;
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
    } else if (str == "autoindex") {
        if (val!="on"&&val!="off") {
            cout <<"error autoindex"<<endl;
            exit(0);
        }
        else if (val=="on")
            this->autoindex = true;
        else
            this->autoindex = false;
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
            || firstWord == "error_page"|| firstWord == "autoindex"
            || firstWord == "root"|| firstWord == "index") {
            // valid.push_back(firstWord);
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
   if (valid.size() != 7) {
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