///////////////////////////////////////////////////////////////////////////////
/// 
///	@file    Serial_H.cpp    
///
/// @brief   串口通信类源文件,本串口通讯类提供windows操作环境下与串口进行通讯
/// 的功能，使用Windows系统提供的串行API函数。
/// @note 在Windows系统下，串口和其他通讯设备都会被当做文件来处理
/// 
/// 
////////////////////////////////////////////////////////////////////////////////
#include "Serial_H.h"

SerialPort::SerialPort()
	:m_ucom(INVALID_HANDLE_VALUE)
{
	InitPort();
}

SerialPort::~SerialPort() 
{
	ClosePort();
}

bool SerialPort::OpenPort()
{
	if (m_ucom != INVALID_HANDLE_VALUE)
	{
		return true;
	}
	std::string com_port = "\\\\.\\COM3";//这里可以根据实际COM进行设置,这里是用的COM8
	
	//CreateFileA对应来往数据采用ASCII编码格式，CreateFileW对应来往数据采用UNICODE编码格式
	m_ucom = CreateFileA(com_port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_ucom == INVALID_HANDLE_VALUE)
	{
		return false;
		
	}
	return true;
}

bool SerialPort::InitPort()
{
	//打开串口
	if (!OpenPort())
	{
		std::cout << "Open Serial Port Failed !!" << std::endl;
		return false;
	}

	DCB dcb;//结构体变量类型，串口设置信息规定使用数据类型，成员为串口连接设置选项，包括波特率,数据位长度等

	memset(&dcb, 0X0, sizeof(dcb));
	GetCommState(m_ucom,&dcb);

	dcb.BaudRate = 115200;  //波特率设置为115200
	dcb.ByteSize = 8;       //8位数据位            * 4-8可选
	dcb.StopBits = 1;       //无停止位             * 0:无停止位 1：一位停止位 2：两位停止位
	dcb.fParity = 0;        //不进行奇偶校验       * 0：不校验 1：校验
	dcb.fNull = 0;          //不允许空串           * 0：不允许 1：允许
	dcb.Parity = 0;         //不进行校验           * 0：不校验 1：奇校验 2：偶校验 3：mark 4:space

	//设置串口连接信息,读写缓存大小采用默认设置
	if (!SetCommState(m_ucom, &dcb))
	{
	
		std::cout << "Set Serial port error: " <<GetLastError()<< std::endl;
		return false;
	}

	/*
	//设置读写缓存器大小，可选，不用表示采用默认设置
	if (!SetupComm(m_ucom, 1048576, 1048576))
	{
		std::cout << "Set the Size of Serial Receive Buffer and Send Buffer Failed, Error: " <<GetLastError()<< std::endl;
		return false;
	}
	*/

	//最后清空发送接收缓存区
	if (!PurgeComm(m_ucom, PURGE_TXCLEAR | PURGE_RXCLEAR))
	{

		std::cout << "Clear Receiver Buffer and Send Buffer Failed, Error: " << GetLastError() << std::endl;
		return false;
	}
	return true;
}

int SerialPort::ReadPort(void* addr,DWORD size,UINT timeout)
{
	COMMTIMEOUTS ReadCommTimeout;    //结构体变量类型，规定使用时间数据类型，发送和接收共用，成员包括读取字符最大间隔时间，持续时间和Multiplier of characters（这是什么？）三类
	memset(&ReadCommTimeout,0X0,sizeof(ReadCommTimeout));
	ReadCommTimeout.ReadTotalTimeoutConstant = timeout;

	//设置读取缓存器超时时间
	SetCommTimeouts(m_ucom,&ReadCommTimeout);

	//用以存储实际从串口读取到的DWORD数
	DWORD dwReadBytes = 0;

	//从缓存器读取数据
	if (ReadFile(m_ucom, addr, size, &dwReadBytes, NULL))
	{
		return dwReadBytes;
	}
	else
	{
		std::cout << "Read Data From Serial Port Failed, Error: " << GetLastError() << std::endl;
		return -1;
	}
}

bool SerialPort::WritePort(void* addr, DWORD size, UINT timeout)
{
	COMMTIMEOUTS WriteCommTimeout;    //结构体变量类型，发送和接收共用，成员包括读取字符最大间隔时间，持续时间和Multiplier of characters（这是什么？）三类
	memset(&WriteCommTimeout, 0X0, sizeof(WriteCommTimeout));
	WriteCommTimeout.WriteTotalTimeoutConstant = timeout;

	//设置写缓存器超时时间
	SetCommTimeouts(m_ucom, &WriteCommTimeout);

	DWORD dwWriteBytes = 0;

	//向缓存器中写数据
	if (WriteFile(m_ucom, addr, size, &dwWriteBytes, NULL))
	{
		return true;
	}
	else
	{
		std::cout << "Write Data to Serial Port Failed, Error: " << GetLastError() << std::endl;
		return false;
	}
}

bool SerialPort::ClosePort()
{
	if (m_ucom != INVALID_HANDLE_VALUE)
	{
		//关闭与串口对应句柄句柄
		return CloseHandle(m_ucom);
	}
	return true;
}

