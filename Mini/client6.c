#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>  // 비동기 I/O를 위한 헤더
#include <errno.h>

#define TCP_PORT 5100

// 메시지 구조체 정의
typedef struct {
    char type[10];  // 메시지 타입("LOGIN", "LOGOUT", "MSG")
    char username[50];  // 사용자 이름
    char content[256];  // 메시지 내용
} Message;

int main(int argc, char **argv)
{
    int ssock;
    struct sockaddr_in servaddr;
    Message msg;  // 메시지 구조체
    char username[50];  // 사용자 이름 입력받을 변수

    if (argc < 2) {
        printf("Usage : %s IP_ADDRESS\n", argv[0]);
        return -1;
    }

    // 소켓을 생성
    if ((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    // 소켓이 접속할 주소 지정
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(servaddr.sin_addr.s_addr));
    servaddr.sin_port = htons(TCP_PORT);

    // 서버에 연결
    if (connect(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect()");
        return -1;
    }

    // 로그인 요청: 사용자 이름 입력
    printf(" 로그인 ID : ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // 개행 문제 제거

    // 로그인 메시지 전송
    strcpy(msg.type, "LOGIN");
    strcpy(msg.username, username);
    if (send(ssock, &msg, sizeof(msg), 0) <= 0) {
        perror("send()");
        return -1;
    }

    // 비동기 I/O 설정( non-blocking mode )
    int flags = fcntl(ssock, F_GETFL, 0);
    fcntl(ssock, F_SETFL, flags | O_NONBLOCK);

    while (1) {
        // 서버로부터 수신된 메시지 처리
        while (1) {
            int bytes_received = recv(ssock, &msg, sizeof(msg), 0);
            if (bytes_received > 0) {
                printf("[%s]: %s\n", msg.username, msg.content);  // 다른 클라이언트가 보낸 메시지 출력
            } else if (bytes_received == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
                // 수신할 데이터가 없으면 루프를 빠져나옴
                break;
            } else if (bytes_received <= 0) {
                perror("recv() failed");
                return -1;
            }
        }

        // 사용자로부터 메시지 입력
        printf(" 메시지(exit 'q'):");
        fgets(msg.content, sizeof(msg.content), stdin);
        msg.content[strcspn(msg.content, "\n")] = 0;  // 개행 문자 제거

        // 종료 조건
        if (strcmp(msg.content, "q") == 0) {
            break;
        }

        // 일반 메시지 전송
        strcpy(msg.type, "MSG");
        if (send(ssock, &msg, sizeof(msg), 0) <= 0) {
            perror("send() failed");
            return -1;
        }
    }

    // 소켓 종료
    close(ssock);
    return 0;
}

