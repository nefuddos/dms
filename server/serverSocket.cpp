#include "serverSocket.h"
#include "clientThread.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
using namespace std;

/*
	private:
		int m_sockfd;//服务端socket描述符
*/


//构造函数
ServerSocket::ServerSocket(const string& ip,short port)throw (SocketException){

	cout << "启动客户端线程开始" <<endl;
	//1.创建服务端socket
	m_sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == m_sockfd)
	{
		throw SocketException("创建socket失败");
	}
	//允许重用本地地址
	int reuse = 1;
	if(-1 == setsockopt(m_sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)) )
	{
		throw SocketException("设置socket地址重用失败");
	}
	//2)准备服务端地址
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	//注意，网络通信是大端，所以根据你自己的情况转换网络字节序
	//将本机字节序转换成网络字节序
	//htons(PORT）;
	addr.sin_port = htons(port);
	//注意此处为服务器IP地址
	//
	addr.sin_addr.s_addr = ip.empty()?
		INADDR_ANY: inet_addr(ip.c_str()); //转换成32位无符号整形
	//inet_aton(m_ip,&addr.sin_addr);//转换IP地址
	//3)bind()
	if(bind(m_sockfd,(struct sockaddr *)&addr,sizeof(addr))== -1){
		throw SocketException("绑定socket失败");
	}	
	//4)监听端口 listen() 自定义最大未决连接请求1024个，未决连接请求都在排队
	if(listen(m_sockfd,1024) == -1){
		throw SocketException("监听socket失败");		  
	}	 
	cout << "启动客户端线程结束" <<endl;
}

//响应客户端的请求
void ServerSocket::acceptClient(void)throw(SocketException)
{
	cout << "等待客户端连接..." <<endl;
	for(;;) //无限阻塞在 accept() 等待客户机的连接请求
	{	 		
		//准备客户端地址
		struct sockaddr_in addrcli;//保存客户端的地址信息
		socklen_t len = sizeof(addrcli); //地址长度不能给零，否则得不到客户端IP
		//等待客户端的连接请求 accept()就知道了客户端的socket 描述符
		int connfd = accept(m_sockfd,(struct sockaddr *)&addrcli,&len);

		//使用accept 函数
		if(-1 == connfd)
		{
			throw SocketException("接受客户端连接失败");
		}
		//new 一个客户端线程
		(new ClientThread(connfd))->start();
	} 	
}
