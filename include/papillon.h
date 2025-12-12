//PAPILLON_Display

// -*- C++ -*-
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <poll.h>
#include <queue>

//
class papillon_header {
public:
    unsigned int m_magic;
    unsigned int m_id;
    unsigned int m_length;
    unsigned int m_trig_cnt;
    unsigned int m_bpos;
    unsigned int m_bwid;
    unsigned int m_ilkstat;

    unsigned int magic() const {
      return ntohl(m_magic);
	  //return m_magic;
    };

    unsigned int id() const {
      return ntohl(m_id);
	  //return m_id;
    };

    unsigned int length() const {
      return ntohl(m_length);
	  //return m_length;
    };

    unsigned int trig_cnt() const {
      return ntohl(m_trig_cnt);
	  //return m_trig_cnt;
    };

    unsigned int bpos() const {
      return ntohl(m_bpos);
	  //return m_bpos;
    };

    unsigned int bwid() const {
      return ntohl(m_bwid);
	  //return m_bwid;
    };

    unsigned int ilk_stat() const {
      return ntohl(m_ilkstat);
	  //return m_ilkstat;
    };
    
    void show() const {
        printf("magic = %08x, id = %08x, length = %08x, trig cnt = %08x, "
               "bpos = %08x, bwid = %08x, ilk = %08x\n", 
               magic(),id(),length(),trig_cnt(),bpos(),bwid(),ilk_stat());

    };
    
    void Fill(int* buf) const {
	buf[0] = m_magic;
	buf[1] = m_id;
	buf[2] = m_length;
	buf[3] = m_trig_cnt;
	buf[4] = m_bpos;
	buf[5] = m_bwid;
	buf[6] = m_ilkstat;
    };
    
};

//
class papillon_event {
private:
    papillon_header m_header;
    unsigned char* m_body;
    int m_len;

public:
    papillon_event(papillon_header& header, unsigned char* p, int len) 
	: m_header(header), m_len(len)  {
	
	m_body = new unsigned char [len];
        //printf("len = %d\n", m_len);

        memcpy(m_body, p, len);
    }

    papillon_event(const papillon_event & e) {
	m_header = e.m_header;
        m_len    = e.m_len;
        
        m_body = new unsigned char[m_len];

        memcpy(m_body, e.m_body, m_len);
    };

    ~papillon_event() {
        delete [] m_body;
    };


    void show() const {
        printf("=============papillon_event.show()=======================\n");

        m_header.show();
        assert(m_len % 4 == 0);

        int wlen = m_len / 4;
        unsigned int * p = (unsigned int *)m_body;

        for (int i=0; i<wlen; i++) {
            if (i % 4 == 0)
                printf("%04x", i*4);
            //printf(" %08x", ntohl(p[i]));
	    printf(" %08x", p[i]);
            if (i % 4 == 3)
                fputc('\n', stdout);
        }

        if (wlen % 4 != 0)
            fputc('\n', stdout);

        printf("====================================\n");
    };

    void raw_show() const {
        assert(m_len % 4 == 0);

        int ct=0;
        printf("%010d %08x %08x %08x %08x\n",
               ct, 
               htonl(header().magic()), 
               htonl(header().id()), 
               htonl(header().length()), 
               htonl(header().trig_cnt()));
        ct+=4;
        printf("%010d %08x %08x %08x %08x\n",
               ct, 
               htonl(header().bpos()), 
               htonl(header().bwid()), 
               htonl(header().ilk_stat()), 
               htonl(header().ilk_stat()));
        ct+=4;

        
        int wlen = m_len / 4;
        unsigned int * p = (unsigned int *)m_body;

        for (int i=0; i<wlen; i++) {
            if ((ct+i) % 4 == 0) {
                //printf("%04x", i*4);
                printf("%010d", i*4);
            }
            printf(" %08x", p[i]);
            if ((ct+i) % 4 == 3)
                fputc('\n', stdout);
        }

        if ((ct+wlen) % 4 != 0)
            fputc('\n', stdout);

        //printf("====================================\n");
    };

    const papillon_header & header() const {
        return m_header;
    };

    int len() const {
        return m_len;
    };

    unsigned char * body() const {
        return m_body;
    };
};


typedef std::queue<papillon_event>  papillon_event_queue_t;

class papillon {
private:
    addrinfo * m_gai;
    int m_socket;
    FILE * m_fsocket;
    papillon_event_queue_t event_queue;
    papillon_header m_header;
    bool is_opened;

public:
    papillon(): m_gai(NULL), m_socket(-1), m_fsocket(NULL), is_opened(false) { };// constructor
    papillon(const char * host, const char * port) 
	:m_gai(NULL), m_socket(-1), m_fsocket(NULL), is_opened(false) { 
	Setup(host,port);
    };

    ~papillon() {
	freeaddrinfo(m_gai);
    };

    void Setup(const char * host, const char * port) {
	assert(is_opened == false);
	assert(m_gai == NULL);
	assert(m_fsocket == NULL);
	assert(m_socket == -1);

	addrinfo gai_hint;

	memset(&gai_hint, 0, sizeof(gai_hint));

	gai_hint.ai_protocol = PF_INET;
	gai_hint.ai_socktype = SOCK_STREAM;
	gai_hint.ai_protocol = IPPROTO_TCP;

	m_gai = NULL;

	int gai_error = getaddrinfo(host, port, &gai_hint, &m_gai);

	if (gai_error) {
	    printf("%d %s\n", gai_error, gai_strerror(gai_error));
	    exit(1);
	}

	m_socket = socket(m_gai->ai_family, m_gai->ai_socktype, m_gai->ai_protocol);

	int ret = connect(m_socket, m_gai->ai_addr, m_gai->ai_addrlen);

	assert(ret == 0);

	m_fsocket = fdopen(m_socket, "r");

	is_opened = true;
    };


    /*void Read() throw(char *) {
    //void Read() noexcept(false) {
        std::cout << "[### papillon::Read() ###]" << std::endl;
        int ret;

	// header
        ret = fread(&m_header, sizeof(m_header), 1, m_fsocket);
        if (ret != 1) throw("reading header");

        if (m_header.magic() != 0xFFFF5555) throw("bad header magic");

        int len = m_header.length();

        if (len % 16 != 0 || len > 4000*64*2) throw("bad length in header");

        int wlen = len / 4;
        unsigned int buffer[wlen];

        ret = fread(buffer, len, 1, m_fsocket);
        if (ret != 1) throw("reading body");

        event_queue.push(papillon_event(m_header, (unsigned char *)buffer, len));

	    while (event_queue.size() > 10) {
	      event_queue.pop();
	    }
    };*/


    void Read() {

        //std::cout << "[### papillon::Read() ###]" << std::endl;
        int ret;

    // header
        ret = fread(&m_header, sizeof(m_header), 1, m_fsocket);
        //if (ret != 1) throw("reading header");
        if (ret != 1) std::cout << "reading header..." << std::endl;

        //if (m_header.magic() != 0xFFFF5555) throw("bad header magic");
        //if (m_header.magic() != 0xFFFF5555) std::cout << "bad header magic: " << std::hex << m_header.magic() << std::endl;
        if (m_header.magic() != 0xFFFF5555) std::cout << "bad header magic: " << m_header.magic() << std::endl;

        int len = m_header.length();

        //if (len % 16 != 0 || len > 4000*64*2) throw("bad length in header");
        if (len % 16 != 0 || len > 4000*64*2) std::cout << "bad length in header: " << len << std::endl;

        int wlen = len / 4;
        unsigned int buffer[wlen];

        ret = fread(buffer, len, 1, m_fsocket);
        //if (ret != 1) throw("reading body");
        if (ret != 1) std::cout << "reading body..." << std::endl;

        event_queue.push(papillon_event(m_header, (unsigned char *)buffer, len));
        m_header.show();

        while (event_queue.size() > 10) {
          event_queue.pop();
        }
    };


    papillon_event & Last() {
	return event_queue.back();
    };

    int Size() { return event_queue.size(); }

    const papillon_header & header() const {
        return m_header;
    };

    bool HasEvent() {
        pollfd item[1] = { m_socket, POLLIN, 0 };
        return poll(item, 1, 0);
    };

    void Close() {
	  if (is_opened) {
	    freeaddrinfo(m_gai);
	    fclose(m_fsocket);
	    close(m_socket);
	    m_socket = -1;
	    m_gai = NULL;
	    m_fsocket = NULL;
	    is_opened = false;
	  }
    };

};
