// g++ -g -ggdb3 -Wall -W -Wextra -std=c++0x fork_test.cpp -l boost_system -l boost_thread -o fork_test.out
// ps -ef | grep -v grep | grep -E "fork_test\.out|/usr/bin/rsync"
// netstat -tlnp | grep 1111
// ss -tlnp | grep 1111
// lsof -i tcp:1111

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
using std::cout;
using std::endl;

#include <boost/thread/thread.hpp>


// 启动 rsync daemon
void RsyncDaemonThread(void)
{
	const char* const rsync_cmd = "/usr/bin/rsync";
	const char* argv[4] = { NULL };
	argv[0] = rsync_cmd;
	argv[1] = "--daemon";
	argv[2] = "--config=rsyncd.conf";
	pid_t pid = fork();
	if (-1 == pid)
	{
		cout << "RsyncDaemonThread(): fork() failed!" << endl;
		return;
	}
	else if (0 == pid)
	{
		execvp(rsync_cmd, (char **)argv);
		cout << "RsyncDaemonThread(): execvp() failed!" << endl;
		exit(-1);
	}
	cout << "RsyncDaemonThread(): before waitpid() ..." << endl;
	int status = -1;
	waitpid(pid, &status, WUNTRACED);
	if (0 == status)
	{
		cout << "RsyncDaemonThread(): status 0, sub process success!" << endl;
	}
	else {
		cout << "RsyncDaemonThread(): rsync daemon failed!" << endl;
		kill(pid, 9);		// rsync 失败，杀掉进程
	}
	return ;
}


// server 端：生成 socket，listen() 监听
// 返回值：socket descriptor
int TcpListenSocket(unsigned int port)
{
	int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == socket_fd)
	{
		cout << "TcpListenSocket(): can not create socket!" << endl;
		return -1;
	}

	struct sockaddr_in local_addr;
	memset(&local_addr, '\0', sizeof(struct sockaddr_in));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(port);

	// 端口复用，防止重启程序时 bind() 失败
	int reuse_addr = 1;
	if (-1 == setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)))
	{
		cout << "TcpListenSocket(): socket setsockopt() SO_REUSEADDR failed!" << endl;
	}
	if (-1 == bind(socket_fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in)))
	{
		cout << "TcpListenSocket(): socket bind() failed!" << endl;
		close(socket_fd);
		return -1;
	}

	if (-1 == listen(socket_fd, 10))
	{
		cout << "TcpListenSocket(): socket listen() failed!" << endl;
		close(socket_fd);
		return -1;
	}
	return socket_fd;
}


int main(int, char* [])
{
	// 可能会抛出 boost::thread_resource_error 异常
	try {
		boost::thread t(&RsyncDaemonThread);
		t.detach();
	}
	catch (const boost::thread_resource_error& e)
	{
		cout << "main(): boost::thread_resource_error exception!" << endl;
	}
	
	int listenfd = -1;
	if (-1 == (listenfd = TcpListenSocket(1111)))
	{
		cout << "main(): TcpListenSocket() failed!" << endl;
		return -1;
	}

	while (true)
	{
		sleep(30);
	}
	return 0;
}


