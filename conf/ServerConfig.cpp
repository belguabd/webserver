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
	// ifstream file(str);
	// stringstream fileContent;
	// if (!file.is_open())
	// 	cout <<"error file not opened "<<endl;
	// fileContent << file.rdbuf();
	// this->data = fileContent.str();
    this->data = str;
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
    // cout <<"-------++++++++++++++++++-----------\n";
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
    // cout <<"--------+++++++++++++++-------------\n";
}

void ServerConfig :: locationNormal(string &location) {
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
    // cout <<"--------++++++++++++++++--------\n";
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

void ServerConfig :: setGlobaleData(string &strConfig, string &str) {
    size_t i = 0;
    int cont = 0;
    string val;
    while(i < strConfig.length())
    {
        size_t pos = strConfig.find(str,i);
        if (pos ==string::npos)
            break;
        size_t endKey = strConfig.find_first_of(" \t", pos);
        size_t valStart = strConfig.find_first_not_of(" \t", endKey);
        size_t lastpos = strConfig.find(';',valStart);
        if (lastpos==string::npos) {
			cout <<"error ; not found"<<endl;
			exit(0) ;
		}
        if (valStart != std::string::npos && lastpos != std::string::npos) {
            val = strConfig.substr(valStart,lastpos - valStart);
            if (val.find("\n")!=string::npos) {
			    cout <<"error newlinw in val"<<endl;
			    exit(0) ;
		    }
            cont++;
            // cout << "var ----- > =  "<<val <<endl;
            if (val.empty()) {
			    cout <<"error empty val"<<endl;
			    exit(0) ;
		    }
        }
        strConfig.replace(pos, str.length(), " ");
        if (str == "listen") {
            int a = atoi(val.c_str());
            this->ports.push_back(a);
        }
        i = pos + 1;
    }
    if (str!="listen"&& str!="index"&& cont!= 1) {
        cout <<"error duplicate var "<<endl;
        exit(0);
    }
    if (str == "host") {
        this->host = val;
    } else if (str == "client_max_body_size") {
        this->client_max_body_size = val;
    } else if (str == "autoindex") {
        if (val=="on")
            this->autoindex = true;
    } else if (str == "error_page 4") {
        this->errorClient = val;
    } else if (str == "error_page 5") {
        this->errorServer = val;
    } else if (str == "index") {
        this->index.push_back(val);
    } else if (str == "root") {
        this->root = val;
    }

}
void ServerConfig :: checkGlobalConfig(string strConfig) {

	string list[] = {"listen","host","client_max_body_size","error_page 4","error_page 5","root"}; //autoindex defaut off
	size_t listSize = sizeof(list) / sizeof(list[0]);
	for (size_t i = 0; i < listSize; i++) {
		if (strConfig.find(list[i]) == string::npos) {
			cout << "Error in global config" << endl;
			exit(0) ;
		}
	}
    size_t index = strConfig.find("index");
    int i = 0;
    while (index!=string::npos)
    {
        string tmp = strConfig.substr(index-4,index+5);
        if (strncmp(tmp.c_str(),"autoindex",9)!=0) {
            i++;
            break;
        } else {
            index = strConfig.find("index",index+1);
        }
    }
    if (i==0) {
			cout << "Error in global config" << endl;
			exit(0) ;
		}
    // string list[] = {"listen","host","client_max_body_size","error_page 4","error_page 5","autoindex ","root","index"};
	// size_t listSize = sizeof(list) / sizeof(list[0]);
	for (size_t i = 0; i < listSize; i++) {
        this->setGlobaleData(strConfig,list[i]);
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
        checkcontent(str.substr(pos + 1, pos1 - pos - 1));
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
	i++;
	
	size_t  pos1 = strdata.rfind('}',i);
	string data = strdata.substr(i,pos1 - i);
	string strConfig = removeLocationBlocks(data);
	this->checkGlobalConfig(strConfig);
	pos1 = strdata.find("location",i);
	if (pos1 != string::npos)
	{
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
    if (sig == true && pos ==string::npos)
    {
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
        if (i!=0)
        {
           string str = strdata.substr(i + 1, pos - i - 1);
            findLocation(str);
        }
        pos--;
    }
}