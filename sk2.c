    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <iostream>
    using namespace std;
    int main(int argc, char **argv) {
        int x= socket(PF_INET, SOCK_STREAM,0);
        sockaddr_in dar;
        dar.sin_family=AF_INET;
        dar.sin_addr.s_addr=inet_addr("127.0.0.1");
        dar.sin_port=htons(1313);
        connect(x,(sockaddr*)&dar, sizeof(dar));
        char litera[1]="";
        while(read(x,litera,1)!=0){
            std::cout<<litera[0];
        }
        close(x);
    	return 0;
    }

