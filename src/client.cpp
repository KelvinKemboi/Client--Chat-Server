#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
using namespace std;

int main(){
    //client socket -TCP
    int client_sock=socket(AF_INET, SOCK_STREAM, 0);
    int port=8000;

    //address
    sockaddr_in addr={}; //network struct
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port);

    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr); //convert string ip address to binary format and store in
    
    //connect 
    if(connect(client_sock, (sockaddr*)&addr, sizeof(addr))<0){
        cout<<"Connection failed."<<endl;
        return 1;
    } 
    cout<<"Connected to server"<<endl;
    return 0;
}