#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 65536
#define FILE_NAME "donors.dat"
#define MAX_DONORS 100

// Module 1-4: Structured Data (Slide 2, 8, 12, 13)
typedef struct {
    char name[50];
    int age;
    char bloodGroup[10];
    char contact[15];
    char lastDonation[15];
} Donor;

Donor donors[MAX_DONORS];
int donorCount = 0;

// Persistence Functions (Module 4: Storage)
void loadFromFile() {
    FILE* fp = fopen(FILE_NAME, "rb");
    if (!fp) return;
    fread(&donorCount, sizeof(int), 1, fp);
    fread(donors, sizeof(Donor), donorCount, fp);
    fclose(fp);
}

void saveToFile() {
    FILE* fp = fopen(FILE_NAME, "wb");
    if (!fp) return;
    fwrite(&donorCount, sizeof(int), 1, fp);
    fwrite(donors, sizeof(Donor), donorCount, fp);
    fclose(fp);
}

// Module 2: Stock Logic
void get_stock_json(char* out) {
    char* groups[] = {"A+", "A-", "B+", "B-", "O+", "O-", "AB+", "AB-"};
    int counts[8] = {0};
    for(int i=0; i<donorCount; i++) {
        for(int j=0; j<8; j++) if(strcmp(donors[i].bloodGroup, groups[j])==0) { counts[j]++; break; }
    }
    sprintf(out, "{ \"A+\":%d,\"A-\":%d,\"B+\":%d,\"B-\":%d,\"O+\":%d,\"O-\":%d,\"AB+\":%d,\"AB-\":%d }", 
            counts[0], counts[1], counts[2], counts[3], counts[4], counts[5], counts[6], counts[7]);
}

// Module 1: Registration with Minimal API Support
void handle_add(char* body) {
    if (donorCount >= MAX_DONORS) return;
    Donor d;
    // Simple manual parsing of JSON-like body: {"name":"X","age":20,"group":"A+","contact":"123"}
    char* n = strstr(body, "\"name\":\""); if(!n) return; n += 8;
    char* n_end = strchr(n, '\"'); *n_end = '\0'; strcpy(d.name, n);
    
    char* a = strstr(n_end + 1, "\"age\":\""); if(!a) return; a += 7;
    char* a_end = strchr(a, '\"'); *a_end = '\0'; d.age = atoi(a);
    
    char* g = strstr(a_end + 1, "\"group\":\""); if(!g) return; g += 9;
    char* g_end = strchr(g, '\"'); *g_end = '\0'; strcpy(d.bloodGroup, g);
    
    char* c = strstr(g_end + 1, "\"contact\":\""); if(!c) return; c += 11;
    char* c_end = strchr(c, '\"'); *c_end = '\0'; strcpy(d.contact, c);

    char* l = strstr(c_end + 1, "\"last_donation\":\""); if(l) {
        l += 18; char* l_end = strchr(l, '\"'); *l_end = '\0'; strcpy(d.lastDonation, l);
    } else strcpy(d.lastDonation, "Not available");

    if (d.age >= 18 && d.age <= 65) {
        donors[donorCount++] = d;
        saveToFile();
    }
}

// Module 2: Hospital Requests API
int handle_issue(char* body) {
    char target[10];
    char* g = strstr(body, "\"group\":\""); if(!g) return 0; g += 9;
    char* g_end = strchr(g, '\"'); *g_end = '\0'; strcpy(target, g);

    for (int i = 0; i < donorCount; i++) {
        if (strcmp(donors[i].bloodGroup, target) == 0) {
            for (int k = i; k < donorCount - 1; k++) donors[k] = donors[k + 1];
            donorCount--;
            saveToFile();
            return 1;
        }
    }
    return 0;
}

// Web Server Core
void send_file(SOCKET client, const char* path, const char* content_type) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        char* nf = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        send(client, nf, strlen(nf), 0);
        return;
    }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    char header[512];
    sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n", content_type, sz);
    send(client, header, strlen(header), 0);
    char buf[8192]; int r;
    while((r = fread(buf, 1, 8192, f)) > 0) send(client, buf, r, 0);
    fclose(f);
}

int main() {
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { AF_INET, htons(PORT), INADDR_ANY };
    bind(s, (struct sockaddr*)&addr, sizeof(addr));
    listen(s, 10);
    loadFromFile();

    printf("SIMATS Blood Bank Web Server Started at http://localhost:%d\n", PORT);

    while(1) {
        SOCKET client = accept(s, NULL, NULL);
        char buf[BUFFER_SIZE] = {0};
        recv(client, buf, BUFFER_SIZE, 0);

        if (strncmp(buf, "GET / ", 6) == 0 || strncmp(buf, "GET /index.html", 15) == 0) {
            send_file(client, "web/index.html", "text/html");
        } 
        else if (strncmp(buf, "GET /assets/hero.png", 20) == 0) {
            send_file(client, "web/assets/hero.png", "image/png");
        }
        else if (strncmp(buf, "GET /api/data", 13) == 0) {
            char stock_json[512], full_json[BUFFER_SIZE];
            get_stock_json(stock_json);
            sprintf(full_json, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{ \"stock\": %s, \"donors\": [", stock_json);
            for(int i=0; i<donorCount; i++) {
                char entry[256];
                sprintf(entry, "{\"name\":\"%s\",\"group\":\"%s\",\"contact\":\"%s\"}%s", 
                        donors[i].name, donors[i].bloodGroup, donors[i].contact, (i == donorCount-1 ? "" : ","));
                strcat(full_json, entry);
            }
            strcat(full_json, "] }");
            send(client, full_json, strlen(full_json), 0);
        }
        else if (strncmp(buf, "POST /api/add", 13) == 0) {
            char* b = strstr(buf, "\r\n\r\n");
            if (b) handle_add(b + 4);
            char* ok = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
            send(client, ok, strlen(ok), 0);
        }
        else if (strncmp(buf, "POST /api/issue", 15) == 0) {
            char* b = strstr(buf, "\r\n\r\n");
            if (b && handle_issue(b + 4)) {
                char* ok = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
                send(client, ok, strlen(ok), 0);
            } else {
                char* err = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
                send(client, err, strlen(err), 0);
            }
        }
        closesocket(client);
    }
    return 0;
}
