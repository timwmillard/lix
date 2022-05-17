#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <pthread.h>
#include <signal.h>

#define DEFAULT_PORT 9333
#define SERVER_BACKLOG 100
#define NUM_WOKERS 50

#define BUF_SIZE 4096

pthread_t workers[NUM_WOKERS];

static struct config {
    u_int16_t port;
} config;

static int lix_server;

typedef struct conn_qnode {
    int conn;
    struct conn_qnode *next;
} conn_qnode;


static struct conn_q {
    conn_qnode *head;
    conn_qnode *tail;
} conn_q = {NULL, NULL};

pthread_mutex_t mu_conn_q = PTHREAD_MUTEX_INITIALIZER;

void conn_enqueue(int conn)
{
    conn_qnode *node = malloc(sizeof(conn_qnode));
    node->conn = conn;
    node->next = NULL;

    pthread_mutex_lock(&mu_conn_q);
    if (conn_q.tail == NULL) {
        conn_q.head = node;
    } else {
        conn_q.tail->next = node;
    }
    conn_q.tail = node;
    pthread_mutex_unlock(&mu_conn_q);
}

int conn_dequeue()
{
    pthread_mutex_lock(&mu_conn_q);
    if (conn_q.head == NULL) {
        pthread_mutex_unlock(&mu_conn_q);
        return -1;
    }

    int conn = conn_q.head->conn;
    conn_qnode *temp = conn_q.head;
    conn_q.head = conn_q.head->next;
    if (conn_q.head == NULL)
        conn_q.tail = NULL;
    pthread_mutex_unlock(&mu_conn_q);

    free(temp);
    return conn;
}

static void usage()
{
    fprintf(stdout,
"Usage: lix [OPTIONS]\n"
"  -h                 Display this help.\n"
"  -p <port>          Server port (default: %d).\n", DEFAULT_PORT);
    exit(0);
}

void parse_options(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++) {
        int lastarg = i == argc-1;

        if (!strcmp(argv[i], "-h")) {
            usage();
        } else if (!strcmp(argv[i], "-p") && !lastarg) {
            int port = atoi(argv[++i]);
            if (port < 1 || port > 65535 ) {
                fprintf(stderr, "Invalid port %s\n\n", argv[i]);
                fprintf(stderr, "Hint: port should a number between 1 and 65,535\n");
                exit(1);
            }
            config.port = port;
        }
    }
}

void handle_conn(int conn)
{
    char buf[BUF_SIZE];
    size_t n, len = 0;

    while ((n = read(conn, buf+len, BUF_SIZE)) > 0) {
        len += n;
        if (len > BUF_SIZE - 1 || buf[len-1] == '\n')
            break;
    }
    if (len == 0) {
        fprintf(stderr, "Error zero bytes read\n");
        return;
    }
    buf[len-1] = '\0';
    printf("incoming[%lu] %s\n", n, buf);
    fflush(stdout);

    write(conn, "HTTP/1.0 200 OK\r\n\r\nHello web\n", 29);

    close(conn);
}

void *mux_conn(void *args)
{
    while (1) {
        int conn = conn_dequeue();
        if (conn != -1) {
            handle_conn(conn);
        }
    }
}

void lix_shutdown(int s)
{
    printf("\n");
    printf("Shutting down server\n");
    close(lix_server);
    exit(0);
}

int main(int argc, char *argv[])
{
    // Setup defaults
    config.port = DEFAULT_PORT;

    parse_options(argc, argv);
    
    // Start worker pool
    for (int i = 0; i < NUM_WOKERS; i++) {
        pthread_create(&workers[i], NULL, mux_conn, NULL);
    }

    struct sockaddr_in addr;
    int err;
    
    lix_server = socket(PF_INET, SOCK_STREAM, 0);
    if (lix_server == -1) {
        fprintf(stderr, "Error opening socket\n");
        return 1;
    }
    int enable = 1;
    err = setsockopt(lix_server, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if (err == -1)
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed, error [%d]: %s\n", errno, strerror(errno));


    signal(SIGINT, lix_shutdown);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(config.port);

    err = bind(lix_server, (struct sockaddr *) &addr, sizeof(addr));
    if (err == -1) {
        fprintf(stderr, "Error binding to socket, error [%d]\n", errno);
        return 1;
    }

    err = listen(lix_server, SERVER_BACKLOG);
    if (err == -1) {
        fprintf(stderr, "Error listing on port, error [%d]: %s\n", errno, strerror(errno));
        return 1;
    }

    printf("Lix: server starting on port %d\n", config.port);

    while (1) {
        int conn = accept(lix_server, NULL, NULL);
        if (conn == -1) {
            fprintf(stderr, "Error accepting connection, error [%d]: %s\n", errno, strerror(errno));
            continue;
        }
        conn_enqueue(conn);
    }

    close(lix_server);

    return 0;
}
