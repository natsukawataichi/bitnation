#include <bn/socket.hpp>

enum {
    SOCK_NBUF = 512,
    SOCK_NHOST = 128,
    SOCK_NPORT = 32,
    SOCK_ERR_SIZE = 1024,
};

typedef enum {
    SOCK_MODE_NULL,
    SOCK_MODE_TCPCLIENT,
    SOCK_MODE_TCPSERVER,
    SOCK_MODE_ACCEPTCLIENT,
} NcSockMode;

static const char *SOCK_DEFAULT_PORT = "8000";

struct NcSock {
    char host[SOCK_NHOST];
    char port[SOCK_NPORT];
    int32_t socket;
    NcSockMode mode;
    char error[SOCK_ERR_SIZE];
};

/*******************************
* socket WSA family of Windows *
*******************************/

#if defined(HC_WIN)
static pthread_once_t wsa_sock_once = PTHREAD_ONCE_INIT;
static WSADATA wsa_data;

static void
wsa_sock_destroy(void) {
    WSACleanup();
}

static void
wsa_sock_init(void) {
    if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0) {
        fprintf(stderr, "failed to start WSA. %d", WSAGetLastError());
    } else {
        atexit(wsa_sock_destroy);
    }
}
#endif /* HC_WIN */

/*******************
* socket functions *
*******************/

static NcSockMode
sock_str_to_mode(const char *mode) {
    if (strcasecmp(mode, "tcp-server") == 0) {
        return SOCK_MODE_TCPSERVER;
    } else if (strcasecmp(mode, "tcp-client") == 0) {
        return SOCK_MODE_TCPCLIENT;
    } else {
        return SOCK_MODE_NULL;
    }
}

static const char *
sock_mode_to_str(NcSockMode mode) {
    switch (mode) {
    case SOCK_MODE_NULL: return "null"; break;
    case SOCK_MODE_TCPCLIENT: return "tcp-client"; break;
    case SOCK_MODE_TCPSERVER: return "tcp-server"; break;
    case SOCK_MODE_ACCEPTCLIENT: return "tcp-accept-client"; break;
    default: return "unknown"; break;
    }
}

void
NcSock_Dump(const NcSock *self, FILE *fout) {
    fprintf(
        fout,
        "socket host[%s] port[%s] mode[%s] socket[%d]\n",
        self->host,
        self->port,
        sock_mode_to_str(self->mode),
        self->socket
    );
}

int32_t
NcSock_Close(NcSock *self) {
    if (self) {
        // if (close(self->socket) < 0) {
        // 	socklog("failed to close socket [%d] by \"%s:%s\""
        // 		, self->socket, self->host, self->port);
        // }
        free(self);
    }

    return 0;
}

void
NcSock_SetErr(NcSock *self, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    vsnprintf(self->error, sizeof self->error, fmt, ap);

    va_end(ap);
}

const char *
NcSock_GetcErr(const NcSock *self) {
    if (!self) {
        return "socket is null";
    }

    return self->error;
}

bool
NcSock_HasErr(const NcSock *self) {
    if (!self || self->error[0] != '\0') {
        return true;
    }

    return false;
}

static NcSock *
init_tcp_server(NcSock *self) {
    struct addrinfo *infores = NULL;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(self->host, self->port, &hints, &infores) != 0) {
        NcSock_SetErr(self, "failed to getaddrinfo \"%s:%s\"", self->host, self->port);
        return NULL;
    }

    // Get listen socket
    struct addrinfo *rp;

    for (rp = infores; rp; rp = rp->ai_next) {
        self->socket = socket(
            infores->ai_family, infores->ai_socktype, infores->ai_protocol
        );
        if (self->socket < 0) {
            continue;
        }

        int optval;
#ifdef HC_WIN
        int ret = setsockopt(
            self->socket, SOL_SOCKET, SO_REUSEADDR,
            (const char *) &optval, sizeof(optval)
        );
#else
        int ret = setsockopt(
            self->socket, SOL_SOCKET, SO_REUSEADDR,
            (void *) &optval, sizeof(optval)
        );
#endif
        if (ret == -1) {
            NcSock_SetErr(self, "failed to setsockopt \"%s:%s\"", self->host, self->port);
            freeaddrinfo(infores);
            return NULL;
        }

        if (bind(self->socket, rp->ai_addr, (int) rp->ai_addrlen) == 0) {
            break; // success to bind
        }

        close(self->socket);
    }

    freeaddrinfo(infores);

    if (!rp) {
        NcSock_SetErr(self, "failed to find listen socket");
        return NULL;
    }

    if (listen(self->socket, SOMAXCONN) < 0) {
        NcSock_SetErr(self, "failed to listen \"%s:%s\"", self->host, self->port);
        return NULL;
    }

    return self;
}

static NcSock *
init_tcp_client(NcSock *self) {
    struct addrinfo *infores = NULL;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = IPPROTO_TCP;

    if (getaddrinfo(self->host, self->port, &hints, &infores) != 0) {
        NcSock_SetErr(self, "failed to getaddrinfo \"%s:%s\"", self->host, self->port);
        return NULL;
    }

    // Find can connect structure
    struct addrinfo *rp = NULL;

    for (rp = infores; rp; rp = rp->ai_next) {
        self->socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (self->socket == -1) {
            continue;
        }

        if (connect(self->socket, rp->ai_addr, rp->ai_addrlen) != -1) {
            // printf("success to connect [%d]", self->socket);
            break; // success to connect
        }

        if (close(self->socket) < 0) {
            NcSock_SetErr(self, "failed to close socket [%d]", self->socket);
        }
    }

    freeaddrinfo(infores);

    if (!rp) {
        NcSock_SetErr(
            self,
            "could not connect to any address \"%s:%s\"\n",
            self->host,
            self->port
        );
        return NULL;
    }

    return self;
}

static NcSock *
parse_open_src(NcSock *self, const char *src) {
    if (!self || !src) {
        NcSock_SetErr(self, "invalid arguments");
        return NULL;
    }

    char *dst = self->host;
    int32_t ndst = sizeof(self->host)-1;
    int32_t di = 0;
    int32_t m = 0;

    for (const char *sp = src; *sp; ++sp) {
        switch (m) {
        case 0:
            if (*sp == ':') {
                m = 1;
                di = 0;
                dst = self->port;
                ndst = sizeof(self->port)-1;
            } else {
                if (di < ndst) {
                    dst[di++] = *sp;
                    dst[di] = '\0';
                }
            }
            break;
        case 1:
            if (!isdigit(*sp)) {
                NcSock_SetErr(self, "invalid port number of \"%s\"", src);
                return NULL;
            }

            if (di < ndst) {
                dst[di++] = *sp;
                dst[di] = '\0';
            }

            break;
        }
    }

    if (self->port[0] == '\0') {
        snprintf(self->port, sizeof self->port, "%s", SOCK_DEFAULT_PORT);
    }

    return self;
}

NcSock *
NcSock_Open(const char *src, const char *mode) {
    NcSock *self = (NcSock *) calloc(1, sizeof(NcSock));
    if (!self) {
        perror("calloc");
        return NULL;
    }

    if (!src || !mode) {
        NcSock_SetErr(self, "invalid arguments");
        return NULL;
    }

#if defined(HC_WIN)
    if (pthread_once(&wsa_sock_once, wsa_sock_init) != 0) {
        NcSock_SetErr(self, "failed to pthread once");
        return NULL;
    }
#endif

    // convert from string to number of mode
    self->mode = sock_str_to_mode(mode);
    if (self->mode == SOCK_MODE_NULL) {
        NcSock_SetErr(self, "invalid open mode \"%s\"", mode);
        return NULL;
    }

    // parse source for host and port
    if (!parse_open_src(self, src)) {
        NcSock_SetErr(self, "failed to parse of \"%s\"", src);
        return NULL;
    }

    // init by mode
    switch (self->mode) {
    default:
        break;
    case SOCK_MODE_TCPSERVER:
        return init_tcp_server(self);
        break;
    case SOCK_MODE_TCPCLIENT:
        return init_tcp_client(self);
        break;
    }

    // invalid open mode
    NcSock_SetErr(self, "invalid open mode \"%s:%s\"", self->host, self->port);
    return NULL;
}

const char *
NcSock_GetcHost(const NcSock *self) {
    if (!self) {
        return NULL;
    }

    return self->host;
}

const char *
NcSock_GetcPort(const NcSock *self) {
    if (!self) {
        return NULL;
    }

    return self->port;
}

NcSock *
NcSock_Accept(NcSock *self) {
    if (!self) {
        return NULL;
    }

    if (self->mode != SOCK_MODE_TCPSERVER) {
        NcSock_SetErr(self, "invalid mode on accept \"%s:%s\"", self->host, self->port);
        return NULL;
    }

    int32_t cliefd = accept(self->socket, NULL, NULL);
    if (cliefd < 0) {
        NcSock_SetErr(self, "failed to accept \"%s:%s\"", self->host, self->port);
        return NULL;
    }

    NcSock *client = (NcSock *) calloc(1, sizeof(NcSock));
    if (!client) {
        NcSock_SetErr(self, "failed to construct NcSock");
        return NULL;
    }

    client->socket = cliefd;
    client->mode = SOCK_MODE_ACCEPTCLIENT;

    return client;
}

int32_t
NcSock_RecvStr(NcSock *self, char *dst, int32_t dstsz) {
    if (!self) {
        return -1;
    }
    if (!dst || dstsz < 1) {
        NcSock_SetErr(self, "invalid arguments");
        return -1;
    }

    int32_t ret = recv(self->socket, dst, dstsz-1, 0);
    if (ret < 0) {
        NcSock_SetErr(self, "failed to read from socket [%d] by \"%s:%s\""
            , self->socket, self->host, self->port);
        *dst = '\0';
    } else if (ret > 0) {
        dst[ret] = '\0';
    }

    return ret;
}

int32_t
NcSock_Recv(NcSock *self, void *dst, int32_t dstsz) {
    if (!self) {
        return -1;
    }
    if (!dst || dstsz < 1) {
        NcSock_SetErr(self, "invalid destination or that size");
        return -1;
    }

#ifdef HC_WIN
    int32_t ret = recv(self->socket, (char *) dst, dstsz, 0);
#else
    int32_t ret = recv(self->socket, dst, dstsz, 0);
#endif
    if (ret < 0) {
        NcSock_SetErr(self, "failed to read from socket [%d] by \"%s:%s\""
            , self->socket, self->host, self->port);
    }

    return ret;
}

int32_t
NcSock_SendStr(NcSock *self, const char *str) {
    if (!self) {
        return -1;
    }
    if (!str) {
        NcSock_SetErr(self, "invalid arguments");
        return -1;
    }

    int32_t ret = 0;
    int32_t len = strlen(str);

    ret = send(self->socket, str, len, 0);
    if (ret < 0) {
        NcSock_SetErr(self, "failed to write to socket [%d] by \"%s:%s\""
            , self->socket, self->host, self->port);
        return -1;
    }

    return ret;
}

int32_t
NcSock_Send(NcSock *self, void *data, int32_t size) {
    if (!self) {
        return -1;
    }
    if (!data || size <= 0) {
        NcSock_SetErr(self, "invalid arguments");
        return -1;
    }

#ifdef HC_WIN
    int32_t ret = send(self->socket, (const char *) data, size, 0);
#else
    int32_t ret = send(self->socket, data, size, 0);
#endif
    if (ret < 0) {
        NcSock_SetErr(self, "failed to write to socket [%d] by \"%s:%s\""
            , self->socket, self->host, self->port);
    }

    return ret;
}
