//
// Created by yassmin on 31/10/18.
//

#include <regex>
#include "Client.h"

Client::Client() {}

bool Client::conToserver(string hostName, int port) {
    soc_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc_desc > 0) {
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr = getHostIP(hostName);
        int con = connect(soc_desc, (struct sockaddr *) &server, sizeof(server));
        return con > 0;
    }
    return false;
}

void Client::handleRequest(string method, string fileName) {
    if (method == "GET") {
        if (sendHeader("GET " + fileName + " HTTP/1.1")) {
            recieveData(1500, fileName); //////////////////////////////change
        }

    } else {
        if (sendHeader("POST " + fileName + " HTTP/1.1")) {
            if (recieveData(1500, fileName) == "OK") { //////////////////////////////change
                sendFile(fileName);
            }
        }
    }
}

bool Client::sendHeader(string data) {
    return send(soc_desc, data.c_str(), strlen(data.c_str()), 0) > 0;
}

string Client::recieveData(int size, string fileName) {   /////////////change can't read the hole file
    FILE *fp = fopen(fileName.c_str(), "w");
    char buffer[size];
    string data = "";
    if (recv(soc_desc, buffer, sizeof(buffer), 0) > 0) {
        data = buffer;
        fwrite(buffer, sizeof(char), sizeof(buffer), fp);
        fflush(fp);
    }
    fclose(fp);
    return data;

}

struct in_addr Client::getHostIP(string hostName) {
    struct hostent *host;
    struct in_addr **in_list;

    if ((host = gethostbyname(hostName.c_str())) != NULL) {
        in_list = (struct in_addr **) host->h_addr_list;
        cout << hostName << ": " << inet_ntoa(**in_list) << "\n";
        return **in_list;
    }
}

void Client::sendFile(string fileName) {
    FILE *fp = fopen(fileName.c_str(), "r");
    fseek(fp, 0, SEEK_END);
    long FileSize = ftell(fp);
    rewind(fp);
    long SizeCheck = 0;
    char *buff;
    if (FileSize > 1499) {

        while (SizeCheck < FileSize) {
            buff = (char *) malloc(1500);
            int Read = fread(buff, 1500, sizeof(char), fp);
            send(soc_desc, buff, Read, 0);
            SizeCheck += Read;
            free(buff);
        }
    } else {
        buff = (char *) malloc(FileSize + 1);
        fread(buff, FileSize + 1, sizeof(char), fp);
        send(soc_desc, buff, FileSize, 0);
        free(buff);
    }
    fclose(fp);
}



int main() {
    std::cout << "Hello, Client!" << std::endl;
    return 0;
}