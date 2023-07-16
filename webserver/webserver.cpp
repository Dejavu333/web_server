//=================================================
// import
//=================================================
#define _CRT_SECURE_NO_WARNINGS
#include <winsock.h>
#include <iostream>
#include <vector>
#include <string>
#include<fstream>
#pragma comment(lib, "Ws2_32.lib")


//=================================================
// declare records
//=================================================
struct HTTPRequest
{
    //head
    char* method;
    char* path;
    char* HTTPVersion;
    std::vector < std::string> headers;
    //body
    char* body;
};


//=================================================
// declare functions
//=================================================
std::string readFile(const std::string p_path);


//=================================================
// main
//=================================================
int main() {
    //-----------------------------
    // initialize winsock
    //-----------------------------
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    
   
    //-----------------------------
    // initilaize bindingsocketdescriptor
    //-----------------------------
    int socketDescriptor = ::socket(AF_INET, SOCK_STREAM, 0);
        //terminal output//
        if (socketDescriptor > 0) { std::cout << "Binding-Socket Is Born. Its Descriptor: " + std::to_string(socketDescriptor) << std::endl; }

    //-----------------------------
    // initialize bindingsocket address
    //-----------------------------
    char hostIp[10] = "127.0.0.1";
    int port = 11000;

    struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = inet_addr(hostIp);

    //-----------------------------
    // bind bindingsocket to address
    //-----------------------------
    int addressLength = sizeof(address);
    int b =::bind(socketDescriptor, (sockaddr*)&address, addressLength);
        //terminal output//
        if (b == 0) { std::cout << "Binding-Socket Is Bound To IP: " << std::string(hostIp) + " on Port: " + std::to_string(port) << std::endl; };
    
    //-----------------------------
    // bindingsocket starts listening
    //-----------------------------
    int backLog = 128;
    int l = ::listen(socketDescriptor, backLog);
        //terminal output
        if (l == 0) { std::cout << "Binding-Socket Is listening For Connections..." << std::endl; };
    
    while (true) {
        //-----------------------------
        // accept incoming connections, create a connHandlingSocket
        //-----------------------------
        int handlerSocketDescriptor = ::accept(socketDescriptor,NULL,NULL);
            //terminal output
            if (handlerSocketDescriptor > 0) { std::cout << "_________________________________________" << std::endl; std::cout << "Connection Accepted" << std::endl;std::cout << "Connection-Handler-Socket Is Born. Its Descriptor: " + std::to_string(handlerSocketDescriptor) << std::endl;  };
        
        //-----------------------------
        // read incoming request
        //-----------------------------
        char inBuffer[2000];
        int inBufferSize = sizeof(inBuffer);
        int flag = 0;
        int chunkSize = ::recv(handlerSocketDescriptor,inBuffer,inBufferSize,flag);
            //terminal output
            std::cout << "Incoming Request:" << std::endl;std::cout << inBuffer << std::endl;
        
        //-----------------------------
        // parse incoming request
        //-----------------------------
        struct HTTPRequest httpRequest; 
        char* inBuffer_ps = inBuffer;       //split request into individual lines and store them in a vector as char array pointers
        std::vector<char*> lines_sv;
        char* token;
            token = strtok(inBuffer_ps, "\n");           
            while (token!=NULL) {
                lines_sv.push_back(token);
                token = strtok(NULL, "\n");
            };
        token = strtok(lines_sv[0], " "); //split the first line using space as delimiter
        httpRequest.method = token;
        token = strtok(NULL, " ");  
        httpRequest.path = token;
        token = strtok(NULL, " ");
        httpRequest.HTTPVersion = token;

        //-----------------------------
        // routing
        //-----------------------------
        //serving edit.html
        if (httpRequest.path == NULL) { continue; };
        if ( strcmp(httpRequest.method,"GET")==0 && (strcmp(httpRequest.path,"/edit.html")==0 || strcmp(httpRequest.path,"/")==0) )
        {
            std::string HTML = readFile("./edit1.html");
            std::string HTMLLength = std::to_string(size(HTML));
            std::string response_ss = "HTTP/1.1 200 OK\r\nContent-Length: "+HTMLLength+"\r\nConnection: close\r\n\r\n"+HTML;
            const char* response_ps = response_ss.data();

            char outBuffer[1024];
            int outBufferSize = sizeof(outBuffer);
            int responseSize = size(response_ss);
            int ptrPosition = 0;
                //terminal output
                std::cout << "Connection-Handler-Socket("+std::to_string(handlerSocketDescriptor)+") Sent:"<< std::endl;
                
                    while (responseSize > 0) //send response in chunks(max1024bytes)
                    {
                        int chunkSize = responseSize > outBufferSize ? outBufferSize : responseSize;
                        memcpy(outBuffer, response_ps + ptrPosition, chunkSize); 
                            chunkSize = ::send(handlerSocketDescriptor, outBuffer, chunkSize, flag);   
                                responseSize = responseSize - chunkSize;
                                ptrPosition = ptrPosition + chunkSize;
                                    //terminal output
                                    std::cout << std::to_string( chunkSize ) +" bytes" << std::endl;
                    };
        }
        //error
        else 
        { 
            std::string HTML = readFile("./404.html");
            std::string HTMLLength = std::to_string(size(HTML));
            std::string response_ss = "HTTP/1.1 404 Not Found\r\nContent-Length: " + HTMLLength + "\r\nConnection: close\r\n\r\n" + HTML;
            const char* response_ps = response_ss.data();

            char outBuffer[1024];
            int outBufferSize = sizeof(outBuffer);
            int responseSize = size(response_ss);
            int ptrPosition = 0;
                //terminal output
                std::cout << "Connection-Handler-Socket(" + std::to_string(handlerSocketDescriptor) + ") Sent:" << std::endl;

            while (responseSize > 0) //send response in chunks(max1024bytes)
            {
                int chunkSize = responseSize > outBufferSize ? outBufferSize : responseSize;
                memcpy(outBuffer, response_ps + ptrPosition, chunkSize);
                    chunkSize = ::send(handlerSocketDescriptor, outBuffer, chunkSize, flag);
                        responseSize = responseSize - chunkSize;
                        ptrPosition = ptrPosition + chunkSize;
                            //terminal output
                            std::cout << std::to_string(chunkSize) + " bytes" << std::endl;
            };
        }

        //-----------------------------
        // close current connection
        //-----------------------------
        ::closesocket(handlerSocketDescriptor);
    };
    return 0;
};

//=================================================
// define functions
//=================================================
std::string readFile(const std::string p_path) {
    std::fstream f(p_path, std::fstream::in);
    std::string s;
        getline(f, s, '\0');
        f.close();
            return s;
};