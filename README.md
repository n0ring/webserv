This project is about writing HTTP server using C++ (std=c++98).

Multiplexing is provided by poll function.

It has GET, POST, and DELETE methods. Also server capable to execute CGI scripts in any language.

![tern (2)](https://user-images.githubusercontent.com/98221398/188319311-02a2351e-5f64-4edd-a898-6fec92701061.gif)


Program take a configuration file as argument or use a default path.

In config file posible to setup: 
  
  For virtual hosts ->
    port to listen |
    max body size |
    error pages

  For locations ->
    max body size |
    error pages |
    methods |
    root |
    index |
    redirect code and URL |
    autoindex |
    cgi script |
    cgi bin

<img width="582" alt="conf" src="https://user-images.githubusercontent.com/98221398/188319216-3d47b098-1eaa-46c2-8673-6067a05f7547.png">
