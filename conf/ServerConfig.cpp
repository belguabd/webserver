#include "ServerConfig.hpp"

void    checkcontent(string substr)
{
    int i =0 ;
    while(i < substr.length()) {
        if (substr[i]!='\t' &&  substr[i]!=' '&& substr[i]!='\n')
            return ;
        i++;
    }
    cout<<"error no content "<<endl;
    exit(0);
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
        }
        else {
            result << input[i];
        }
    }
    return result.str();
}

string removeLocationBlocks(string& configData) {
	string result = configData;
	// Regex pattern to match "location { ... }" blocks
	string locationRegexPattern = "\\s*location\\s+[^}]*\\{[^}]*\\}\\s*";
	regex locationRegex(locationRegexPattern);
	// Replace all location blocks with an empty string
	result = regex_replace(result, locationRegex, "");  
	return result;
}

ServerConfig :: ServerConfig(string &str) {
	ifstream file(str);
	stringstream fileContent;
	if (!file.is_open())
		cout <<"error file not opened "<<endl;
	fileContent << file.rdbuf();
	this->data = fileContent.str();
}

ServerConfig :: ~ServerConfig() { }

void ServerConfig :: locationRedirection(string &location) {
    cout <<"redirection     ??" <<endl;
}

void ServerConfig ::locationCgi(string &location) {
    map<string, LocationCgi> cgiconf;
    string tmp;
    size_t pos = location.find("{");
    tmp = location.substr(8, pos - 8);
    string key = trim(tmp);
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
    cgiconf[key] = cgi;
	this->configcgi = cgiconf;
    // cout << "Location: " << key << "\n";
    // cout << "Root: " << cgi.root << "\n";
    // cout << "Allowed Methods: " << cgi.allowed_methods << "\n";
    // cout << "Upload Store: " << cgi.cgi_extension << "\n";
    // cout << "Client Max Body Size: " << cgi.cgi_handler << "\n";
    // cout <<"----------------------------------\n";
}

void ServerConfig :: locationUpload(string &location) {
    map<string, LocationUplaods> upload;
    string tmp;
    size_t pos = location.find("{");
    tmp =location.substr(8, pos - 8);
    string key = trim(tmp);
    LocationUplaods config;
    size_t rootPos = location.find("root");
    size_t allowedMethodsPos = location.find("allowed_methods");
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
        config.allowed_methods = trim(tmp);
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
        config.client_max_body_size = trim(tmp);
    }

    upload[key] = config;
	this->configUpload = upload;
    // cout << "Location: " << key << "\n";
    // cout << "Root: " << config.root << "\n";
    // cout << "Allowed Methods: " << config.allowed_methods << "\n";
    // cout << "Upload Store: " << config.upload_store << "\n";
    // cout << "Client Max Body Size: " << config.client_max_body_size << "\n";
    // cout <<"----------------------------------\n";
}

void    ServerConfig :: locationNormal(string &location) {
    map<string, LocationConfig> normal;
    string tmp;
    size_t pos = location.find("{");
    tmp = location.substr(8, pos - 8);
    string key = trim(tmp);
    LocationConfig config;
    size_t rootPos = location.find("root");
    size_t allowedMethodsPos = location.find("allowed_methods");
    size_t indexPos = location.find("index");
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
        config.allowed_methods = trim(tmp);
    }
    if (indexPos != string::npos) {
        size_t valueStart = location.find(" ", indexPos + 5);
        size_t valueEnd = location.find(";", valueStart);
        tmp = location.substr(valueStart + 1, valueEnd - valueStart - 1);
        config.index = trim(tmp);
    }
    normal[key] = config;
	this->configNormal = normal;

    // cout << "Location: " << key << "\n";
    // cout << "Root: " << config.root << "\n";
    // cout << "Allowed Methods: " << config.allowed_methods << "\n";
    // cout << "index : " << config.index<< "\n";
    // cout <<"----------------------------------\n";
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
        } else if (location.find("returne")!=string::npos) {
            locationRedirection(location);
        } else if (location.find("cgi_handler")!=string::npos) {
            locationCgi(location);
        } else {
            locationNormal(location);
        }
        i = lastpos+1;
    }

}
void ServerConfig :: checkGlobalConfig(string strConfig) {
	map<string, string> globalvar;
	string list[] = {"listen","host","client_max_body_size","error_page 4","error_page 5","root","index","autoindex"};
	size_t listSize = sizeof(list) / sizeof(list[0]);
	for (size_t i = 0; i < listSize; i++) {
		if (strConfig.find(list[i]) == string::npos) {
			cout << "Error in global config" << endl;
			exit(0) ;
		}
	}
	for (size_t i = 0; i < listSize; i++) {
		size_t pos = strConfig.find(list[i]);
		size_t endKey = strConfig.find_first_of(" \t", pos);
		string key = strConfig.substr(pos, endKey - pos);
		if (key.find(";")!=string::npos) {
			cout <<"error"<<endl;
			exit(0) ;
		}
		size_t valStart = strConfig.find_first_not_of(" \t", endKey);
		size_t lastpos = strConfig.find(';',valStart);
		if (lastpos==string::npos) {
			cout <<"error"<<endl;
			exit(0) ;
		}
		string val = strConfig.substr(valStart,lastpos - valStart);
		if (val.find("\n")!=string::npos) {
			cout <<"error"<<endl;
			exit(0) ;
		}
		globalvar[key]=val;
	}
	this->globalConfig = globalvar;
}
void ServerConfig :: validbrackets() {
    int sig = 0;
    string tmp;
    size_t firstpos =0;
    size_t lastBra = this->data.rfind("}");
    if (lastBra != string::npos && lastBra < this->data.length() - 1) {
        tmp = this->data.substr(lastBra + 1);
        if (checkCharacter(tmp,'}')) {
            cout << "error data after last bracket " << endl;
            exit(1);
        }
    }
    while (firstpos < this->data.length()) {
        size_t pos = this->data.find("{", firstpos);
        size_t pos1 = this->data.find("}", firstpos);
        if (pos==string::npos) {
            break ;
        }
        checkcontent(this->data.substr(pos + 1, pos1 - pos - 1));
        tmp = this->data.substr(firstpos,pos - firstpos);
        if (checkCharacter(tmp,'}'))
            sig++;
        else{
            cout<<"error bracket whitout name"<<endl;
            exit(1);
        }
        firstpos = pos + 1;
        
    }
    size_t lastpos =0;
    while (lastpos < this->data.length()) {
        size_t pos = this->data.find("}", lastpos);
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
void ServerConfig :: parseServerConfig() {
	size_t pos = this->data.find("server");
	string str = this->data.substr(0,pos);
	if (pos== string::npos ||checkCharacter(str,'}')) {
		cout << "error :string before server" << endl;
		exit(0);
	}
	size_t i = pos + 6;
	while(i < this->data.length() && this->data[i]!='{') {
		if (this->data[i]!='\t' &&  this->data[i]!=' '&& this->data[i]!='\n') {
			cout << "error" << endl;
			exit(0);
		}
		i++;
	}
	i++;
	
	size_t  pos1 = this->data.rfind('}',i);
	string data = this->data.substr(i,pos1 - i);
	string strConfig = removeLocationBlocks(data);
	this->checkGlobalConfig(strConfig);
	pos1 = this->data.find("location",i);
	if (pos1 != string::npos)
	{
		string loca  = this->data.substr(pos1);
		locationData(loca);   
	}
}

void ServerConfig :: dataConfigFile()
{
	this->data = removeComments(this->data);
    this->validbrackets();
    this->parseServerConfig();
}