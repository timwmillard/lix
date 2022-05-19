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
#define NUM_WOKERS 20

#define BUF_SIZE 4096

// Config holds all the server configuration
static struct config {
    u_int16_t port;
} config;

static int lix_server;

// Thread Pool with connection queue
pthread_t workers[NUM_WOKERS];


struct conn_qnode {
    int conn;
    struct conn_qnode *next;
};

// Connection queue of awating connections read to be handled.
static struct conn_q {
    struct conn_qnode *head;
    struct conn_qnode *tail;
    pthread_mutex_t mu;
    pthread_cond_t ready;
} conn_q = {NULL, NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

// Add a connection to the queue
void conn_enqueue(int conn)
{
    struct conn_qnode *node = malloc(sizeof(struct conn_qnode));
    node->conn = conn;
    node->next = NULL;

    pthread_mutex_lock(&conn_q.mu);
    if (conn_q.tail == NULL) {
        conn_q.head = node;
    } else {
        conn_q.tail->next = node;
    }
    conn_q.tail = node;
    pthread_mutex_unlock(&conn_q.mu);
    pthread_cond_signal(&conn_q.ready);
}

// Get and remove a connection from the queue.
// Will block on empty queue until a new connection is placed in queue.
int conn_dequeue()
{
    pthread_mutex_lock(&conn_q.mu);
    while (conn_q.head == NULL) {
        pthread_cond_wait(&conn_q.ready, &conn_q.mu);
    }

    int conn = conn_q.head->conn;
    struct conn_qnode *temp = conn_q.head;
    conn_q.head = conn_q.head->next;
    if (conn_q.head == NULL)
        conn_q.tail = NULL;
    pthread_mutex_unlock(&conn_q.mu);

    free(temp);
    return conn;
}

// Show usage details
static void usage()
{
    fprintf(stdout,
        "Usage: lix [OPTIONS]\n"
        "  -h                 Display this help.\n"
        "  -p <port>          Server port (default: %d).\n", DEFAULT_PORT);
    exit(0);
}

// Parse the command line options
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

// Handle the connection
void handle_conn(int conn)
{
    char buf[BUF_SIZE];
    size_t n, len = 0;

    char pbuf[BUF_SIZE];

    while ((n = recv(conn, buf+len, BUF_SIZE, 0)) > 0) {
        len += n;
        if (len > BUF_SIZE - 1)
            break;
        if (buf[len-1] == '\n')
            break;
    }
    if (len == 0) {
        fprintf(stderr, "Error zero bytes read\n");
        return;
    }
    buf[len] = '\0';
    printf("======= request conn[%d] ====================\n", conn);
    printf("%s\n", buf);
     printf("====== end request conn[%d] len=%lu ========\n", conn, len);
    fflush(stdout);

    write(conn, "HTTP/1.0 200 OK\r\n\r\nHello web\n", 29);

    close(conn);
}

// The entry point for the worker thread.
// This function runs on each worker thread.
// Checking the quere for new connections and then handle them.
void *worker_start(void *args)
{
    while (1) {
        int conn = conn_dequeue();  // will block until a connection is avaliable
        handle_conn(conn);
    }
}

// Shutdown the serve
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
        pthread_create(&workers[i], NULL, worker_start, NULL);
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
