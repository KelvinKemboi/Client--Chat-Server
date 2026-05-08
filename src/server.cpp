#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
using namespace std;

vector<int> clients;//store clients
mutex clients_mut; //guard clients

//helper function to handle each client
void eachClient(int client){
    //register client
    {
        lock_guard<mutex> lock(clients_mut); //lock client
        clients.push_back(client); //append client
    }
    while(true){
        char buffer[1024]={0};
        int bytes=recv(client, buffer, sizeof(buffer)-1, 0); //recieve raw byte messages
        if(bytes<=0){
            cerr<<"Disconnected"<<endl;
            close(client);
            break;
        }

        //terminate buffer and print string message
        buffer[bytes]='\0'; //add end of string marker
        string s(buffer);
        cout<<"Buffer recieved: "<<s<<endl;

        //response
        string response="Server received: "+s;
        send(client, response.c_str(), response.size(),0);
    }
    //unregister client
    {
        lock_guard<mutex> lock(clients_mut);
        clients.erase(remove(clients.begin(), clients.end(), client), clients.end()); //move non-client clients to front and erase end
    }
}

//process
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

    //debug 
    cout<<"Server is listening on port: "<<port<<endl;

    //accept connection
    while(true){
        //create client socket for every connection
        int client=accept(sock, nullptr, nullptr);  
        if(client<0){
            cerr<<"Accept failed."<<endl;
            continue;
        }
        cout<<"Client connected: "<<client<<endl;
        thread t(eachClient, client);
        t.detach(); //have each convo run independantly while main thread is running
    }
    close(sock);
    return 0;
}