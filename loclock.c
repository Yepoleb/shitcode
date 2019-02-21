#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct timeval get_timeval(void)
{
    struct sockaddr_in addr = {AF_INET};
    addr.sin_port = 0;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int ret;
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sock != -1);
    int val = 1;
    ret = setsockopt(sock, SOL_SOCKET, SO_TIMESTAMP, &val, sizeof(val));
    assert(ret != -1);
    ret = bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    assert(ret != -1);
    socklen_t addr_len = sizeof(struct sockaddr_in);
    ret = getsockname(sock, (struct sockaddr*)&addr, &addr_len);
    assert(ret != -1);

    const char* testdata = "PING";
    sendto(sock, testdata, sizeof(testdata), 0, (struct sockaddr*)&addr, addr_len);

    struct msghdr recv_hdr;
    struct sockaddr_in msg_addr;
    recv_hdr.msg_name = &msg_addr;
    recv_hdr.msg_namelen = sizeof(struct sockaddr_in);
    recv_hdr.msg_flags = 0;

    struct iovec iov[1];
    char recv_buf[10];
    iov[0].iov_base = recv_buf;
    iov[0].iov_len = sizeof(recv_buf);
    recv_hdr.msg_iov = iov;
    recv_hdr.msg_iovlen = 1;

    char control_buf[1024];
    recv_hdr.msg_control = control_buf;
    recv_hdr.msg_controllen = 1024;

    ssize_t received = recvmsg(sock, &recv_hdr, 0);
    assert(received);

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&recv_hdr);
    struct timeval tv;
    assert(cmsg->cmsg_level == SOL_SOCKET);
    assert(cmsg->cmsg_type == SO_TIMESTAMP);
    memcpy(&tv, CMSG_DATA(cmsg), sizeof(struct timeval));
    return tv;
}

int main(void)
{
    struct timeval tv = get_timeval();
    printf("Timestamp: %lld.%06ld\n", (long long)tv.tv_sec, (long)tv.tv_usec);

    return 0;
};
