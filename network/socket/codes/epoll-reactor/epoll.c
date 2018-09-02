#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#define MAX_EVENTS  1024  // 最大连接数(连接套接字的最大数量)
#define BUFLEN 128
#define SERV_PORT   8080  // 服务器的端口
 
/*
 * status:1表示在监听事件中，0表示不在
 * last_active:记录最后一次响应时间,做超时处理
 */
struct myevent_s {
	int fd;                                           // cfd listenfd
	int events;                                       // 事件类型:EPOLLIN  EPLLOUT
	void *arg;                                        // 指向自己结构体指针
	void (*call_back)(int fd, int events, void *arg); // 回调函数的参数:文件描述符,事件类型,结构体本身的指针
	int status;                                       // 1:树上 2:不在树上
	char buf[BUFLEN];                                 //
	int len;                                          //
	long last_active;                                 // 最后一次挂到树上的时间戳
};
 
int g_efd; /* epoll_create返回的句柄 */
struct myevent_s g_events[MAX_EVENTS + 1]; /* +1 最后一个用于 listen fd */
 
/*
 * 封装一个自定义事件，包括fd，这个fd的回调函数，还有一个额外的参数项
 * 注意：在封装这个事件的时候，为这个事件指明了回调函数，一般来说，一个fd只对一个特定的事件
 * 感兴趣，当这个事件发生的时候，就调用这个回调函数
 */
/* 填充struct myevent_s类型的变量 */
void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *),
		void *arg) {
	ev->fd = fd;
	ev->call_back = call_back;
	ev->events = 0;
	ev->arg = arg;
	ev->status = 0;
	//memset(ev->buf, 0, sizeof(ev->buf));
	//ev->len = 0;
	ev->last_active = time(NULL);  
 
	return;
}
 
//声明回调函数，好在定义之前引用
void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);
 
/*
 * 这个函数封装了epoll_ctl函数，将一个自定义事件（自己封装的事件）添加到epoll实例中
 * 中间那个events表示感兴趣的事件
 */
void eventadd(int efd, int events, struct myevent_s *ev) { // ev与我们的fd是一一对应的
	struct epoll_event epv = { 0, { 0 } };
	int op;
 
	//epoll_event的data数据域，用来保存用户数据，这里将一个指针ptr指向自定义的事件类型
	//在epoll_event中，这个指针是可以指向任何类型的 void*
	//epoll_event与一个fd是一一对应的
	epv.data.ptr = ev;
	epv.events = ev->events = events;
 
    // 如果当前文件描述符已经在树上
	if (ev->status == 1) {
        // 说我要修改红黑树上的节点信息
		op = EPOLL_CTL_MOD;
    // 如果当前文件描述符不在树上
	} else {
        // 说我要添加到红黑树上
		op = EPOLL_CTL_ADD;
        // 说明节点已经在树上了
		ev->status = 1;
	}
 
	if (epoll_ctl(efd, op, ev->fd, &epv) < 0)
		printf("event add failed [fd=%d], events[%d]\n", ev->fd, events);
	else
		printf("event add OK [fd=%d], op=%d, events[%0X]\n", ev->fd, op,
				events);
 
	return;
}
 
/*从epoll实例中删除一个事件 */
void eventdel(int efd, struct myevent_s *ev) {
	struct epoll_event epv = { 0, { 0 } };
 
	if (ev->status != 1)
		return;
 
	epv.data.ptr = ev;
	ev->status = 0;
	epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);
 
	return;
}
 
/*看名字好像是接受一个新的连接*/
void acceptconn(int lfd, int events, void *arg) {
	struct sockaddr_in cin;
	socklen_t len = sizeof(cin);
	int cfd, i;
 
	if ((cfd = accept(lfd, (struct sockaddr *) &cin, &len)) == -1) {
		if (errno != EAGAIN && errno != EINTR) {
			/* 暂时不做出错处理 */
		}
		printf("%s: accept, %s\n", __func__, strerror(errno));
		return;
	}
 
	do {
		for (i = 0; i < MAX_EVENTS; i++) {
			if (g_events[i].status == 0)
				break;
		}
 
		if (i == MAX_EVENTS) {
			printf("%s: max connect limit[%d]\n", __func__, MAX_EVENTS);
			break;
		}
 
		int flag = 0;
		if ((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0) {
			printf("%s: fcntl nonblocking failed, %s\n", __func__,
					strerror(errno));
			break;
		}
 
		//下面这两句表示封装一个自定义的事件，以及将这个事件添加到epoll实例中
		eventset(&g_events[i], cfd, recvdata, &g_events[i]);
		eventadd(g_efd, EPOLLIN, &g_events[i]);
	} while (0);
 
	printf("new connect [%s:%d][time:%ld], pos[%d]\n", inet_ntoa(cin.sin_addr),
			ntohs(cin.sin_port), g_events[i].last_active, i);
 
	return;
}
 
/**接收数据*/
void recvdata(int fd, int events, void *arg) {
	struct myevent_s *ev = (struct myevent_s *) arg;
	int len;
 
	len = recv(fd, ev->buf, sizeof(ev->buf), 0);
	eventdel(g_efd, ev);
 
	if (len > 0) {
		ev->len = len;
		ev->buf[len] = '\0';
		printf("C[%d]:%s\n", fd, ev->buf);
		/* 转换为发送事件 */
		eventset(ev, fd, senddata, ev);
		eventadd(g_efd, EPOLLOUT, ev);
	} else if (len == 0) {
		close(ev->fd);
		/* ev-g_events 地址相减得到偏移元素位置 */
		printf("[fd=%d] pos[%d], closed\n", fd, (int) (ev - g_events));
	} else {
		close(ev->fd);
		printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
	}
	return;
}
 
void senddata(int fd, int events, void *arg) {
	struct myevent_s *ev = (struct myevent_s *) arg;
	int len;
 
	len = send(fd, ev->buf, ev->len, 0);
	//printf("fd=%d\tev->buf=%s\ttev->len=%d\n", fd, ev->buf, ev->len);
	//printf("send len = %d\n", len);
 
	eventdel(g_efd, ev);
	if (len > 0) {
		printf("send[fd=%d], [%d]%s\n", fd, len, ev->buf);
		eventset(ev, fd, recvdata, ev);
		eventadd(g_efd, EPOLLIN, ev);
	} else {
		close(ev->fd);
		printf("send[fd=%d] error %s\n", fd, strerror(errno));
	}
	return;
}
 
void initlistensocket(int efd, short port) {
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(lfd, F_SETFL, O_NONBLOCK);
	eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);
	eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);
 
	struct sockaddr_in sin;
 
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
 
	bind(lfd, (struct sockaddr *) &sin, sizeof(sin));
 
	listen(lfd, 20);
 
	return;
}
 
int main(int argc, char *argv[]) {
	unsigned short port = SERV_PORT;
 
	if (argc == 2)
		port = atoi(argv[1]);
 
	g_efd = epoll_create(MAX_EVENTS + 1);
 
	if (g_efd <= 0)
		printf("create efd in %s err %s\n", __func__, strerror(errno));
 
	initlistensocket(g_efd, port);
 
	/* 事件循环，缓冲区 */
	struct epoll_event events[MAX_EVENTS + 1];
 
	printf("server running:port[%d]\n", port);
	int checkpos = 0, i;
	while (1) {
		/* 超时验证，每次测试100个链接，不测试listenfd 当客户端60秒内没有和服务器通信，则关闭此客户端链接 */
		long now = time(NULL);
		for (i = 0; i < 100; i++, checkpos++) {
			if (checkpos == MAX_EVENTS)
				checkpos = 0;
			if (g_events[checkpos].status != 1)
				continue;
			long duration = now - g_events[checkpos].last_active;
			if (duration >= 60) {
				close(g_events[checkpos].fd);
				printf("[fd=%d] timeout\n", g_events[checkpos].fd);
				eventdel(g_efd, &g_events[checkpos]);
			}
		}
		/* 等待事件发生 */
		int nfd = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
		if (nfd < 0) {
			printf("epoll_wait error, exit\n");
			break;
		}
		for (i = 0; i < nfd; i++) {
 
			//强制转换ptr指针为自定义封装事件类型
			struct myevent_s *ev = (struct myevent_s *) events[i].data.ptr;
 
			//事件派发，回调函数保存在由ptr指针指向的自定义事件类型中的回调函数中
			if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) {
				ev->call_back(ev->fd, events[i].events, ev->arg);
			}
			if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) {
				ev->call_back(ev->fd, events[i].events, ev->arg);
			}
		}
	}
 
	/* 退出前释放所有资源 */
	return 0;
}

