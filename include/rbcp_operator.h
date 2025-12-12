#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include <netdb.h>

#include "rbcp.h"

#define MAX_LINE_SIZE	1024
#define MAX_ARG_SIZE	20
#define RBCP_VER	0xFF
#define RBCP_CMD_WR	0x80
#define RBCP_CMD_RD	0xC0
#define DEFAULT_IP	192.168.0.16
#define UDP_BUF_SIZE	2048


class rbcp_operator {
private:
    addrinfo * m_ai;
    int m_socket;
    unsigned char m_id;
    int m_length;
    struct {
	bcp_header header;
	unsigned char buffer[2048];
    } m_packet;

    void Send() {
	int ret = sendto(m_socket,
	    &m_packet, m_length, 0,
	    m_ai->ai_addr, m_ai->ai_addrlen);

	assert(ret == m_length);
    };

    void Recv() {
	sockaddr remote;
	socklen_t remote_len = sizeof(remote_len);

	int ret = recvfrom(m_socket,
		    &m_packet, sizeof(m_packet), 0,
		    &remote, &remote_len);

	m_length = ret;

	assert(ret > 0);

	assert(sizeof(bcp_header) + m_packet.header.length == m_length);

    };


public:

    const char * const GetPacket() const {
	return (char *)&m_packet;
    };

    const bcp_header * const GetHeader() const {
	return &m_packet.header;
    };

    const unsigned char * const GetBody() const {
	return m_packet.buffer;
    };

    rbcp_operator(const char * host, const char * port) : m_id(0), m_ai(0) {
	addrinfo gai_hint;

	memset(&gai_hint, 0, sizeof(gai_hint));

	gai_hint.ai_protocol = PF_INET;
	gai_hint.ai_socktype = SOCK_DGRAM;
	gai_hint.ai_protocol = IPPROTO_UDP;

	int gai_error = getaddrinfo(host, port, &gai_hint, &m_ai);

	if (gai_error) {
	    fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(gai_error));
	}

	assert(gai_error == 0);

	m_packet.header.type	= RBCP_VER;
	m_socket = socket(m_ai->ai_family, m_ai->ai_socktype, m_ai->ai_protocol);
	assert(m_socket > 0);

    };

    ~rbcp_operator() {
	close(m_socket);
	freeaddrinfo(m_ai);
    };

    void Read(int start, int len = 0xFF) {
	m_packet.header.command	= RBCP_CMD_RD;
	m_packet.header.id	= m_id++;
	m_packet.header.address	= htonl(start);
	m_packet.header.length	= len;

	m_length = sizeof(m_packet.header);

	Send();

	Recv();
    };

    void Write(int start, const unsigned char * const p, int len) {
	m_packet.header.command = RBCP_CMD_WR;
	m_packet.header.length = len;
	m_packet.header.address = htonl(start);
	memcpy(m_packet.buffer, p, len);

	m_length = sizeof(m_packet.header) + len;

	Send();

	Recv();
    };

    void Show() {

	const int start = htonl(m_packet.header.address);
	const int len = m_packet.header.length;

	printf("id=%d, start=0x%X, len=0x%X\n",
		m_packet.header.id, start, len);


	/* Calurate the offset size of register window of FINESSE */
	int offset;
	char * focus;

	if (start < 0x400) {
	  //focus = (char *)"COPPER-LITE";
            focus = (char *)"GENERAL";
	} else if (start < 0x600) {
	    focus = (char *)"FINESSE (individual)";
	    offset = 0x400;
	} else if (start < 0x800) {
	    focus = (char *)"FINESSE (synchronize)";
	    offset = 0x600;
	} else {
	    abort();
	}

	for (int i=0; i<len; i++) {
	    if (i % 4 == 0) {
		printf("0x%04x (%4d) = %s:%d",
		    start+i, start+i,
		    focus,
		    (start-offset+i+3) / 4);
	    }
	    printf(" %02x", m_packet.buffer[i]);

	    if (i % 4 == 3) {
		fputc('\n', stdout);
	    }
	}

	fputc('\n', stdout);
    };

};
