# web_server 

The program implements a FTP server that processes `GET` requests from client browsers. 

### Request 
Requests from client browsers must comply with the following format: `http://[ip]:[port]/[request]`. 
Requests can be divided into two categories: directory and regular files. When a directory is requested, the server 
simply lists the names of the files found within the directory - an empty-string request refers to the root directory 
by default. When a regular file is requested, the server's action depends on the type of the file requested. If the 
requested file is either of the type `cgi` or `exe`, the server executes it with "standard out" and "standard error" 
redirected to the client. If the file is of the type `gif`, `html`, `jpeg`, `jpg` or `txt`, the file is opened and 
its content is sent to the browser. 

### Parameter 
To support dynamic data manipulations, the server can expect some parameters to execute `cgi` and `exe` files. 
Parameters follow a single `?` after the request string, with each parameter terminated by an `&`. Since the program 
looks for an `=` and parses anything beyond as the parameter value, the parameter name that comes before the `=` is 
irrelevant. The maximum number of client paramters is 5. A request string with parameters has the following format: 
	
    http://[ip]:[port]/[request]?0=[arg_0]&1=[arg_1]&2=[arg_2]&3=[arg_3]&4=[arg_4]&5=[arg_5]&
