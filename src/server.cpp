#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
using namespace std;

int main(){
    //create socket
    int sock=socket(AF_INET, SOCK_STREAM, 0);
    
    //address
    int port=8000;
    sockaddr_in addr={};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);
    addr.sin_addr.s_addr=INADDR_ANY;

    //bind
    bind(sock, (sockaddr*)&addr, sizeof(addr));
    listen(sock, 20);

    //print server
    cout<<"Server is listening on port: "<<port<<endl;

    //accept connection
    while(true){
        int client=accept(sock, nullptr, nullptr);
        cout<<"Client connected: "<<client<<endl;
        
        char buffer[1024]={0};
        recv(client, buffer, sizeof(buffer), 0); //recieve messages

        string s(buffer);
        cout<<"Buffer recieved: "<<s<<endl;
        close(client);
    }


    return 0;
}