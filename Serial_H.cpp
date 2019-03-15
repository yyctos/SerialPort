///////////////////////////////////////////////////////////////////////////////
/// 
///	@file    Serial_H.cpp    
///
/// @brief   ����ͨ����Դ�ļ�,������ͨѶ���ṩwindows�����������봮�ڽ���ͨѶ
/// �Ĺ��ܣ�ʹ��Windowsϵͳ�ṩ�Ĵ���API������
/// @note ��Windowsϵͳ�£����ں�����ͨѶ�豸���ᱻ�����ļ�������
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
	std::string com_port = "\\\\.\\COM3";//������Ը���ʵ��COM��������,�������õ�COM8
	
	//CreateFileA��Ӧ�������ݲ���ASCII�����ʽ��CreateFileW��Ӧ�������ݲ���UNICODE�����ʽ
	m_ucom = CreateFileA(com_port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_ucom == INVALID_HANDLE_VALUE)
	{
		return false;
		
	}
	return true;
}

bool SerialPort::InitPort()
{
	//�򿪴���
	if (!OpenPort())
	{
		std::cout << "Open Serial Port Failed !!" << std::endl;
		return false;
	}

	DCB dcb;//�ṹ��������ͣ�����������Ϣ�涨ʹ���������ͣ���ԱΪ������������ѡ�����������,����λ���ȵ�

	memset(&dcb, 0X0, sizeof(dcb));
	GetCommState(m_ucom,&dcb);

	dcb.BaudRate = 115200;  //����������Ϊ115200
	dcb.ByteSize = 8;       //8λ����λ            * 4-8��ѡ
	dcb.StopBits = 1;       //��ֹͣλ             * 0:��ֹͣλ 1��һλֹͣλ 2����λֹͣλ
	dcb.fParity = 0;        //��������żУ��       * 0����У�� 1��У��
	dcb.fNull = 0;          //������մ�           * 0�������� 1������
	dcb.Parity = 0;         //������У��           * 0����У�� 1����У�� 2��żУ�� 3��mark 4:space

	//���ô���������Ϣ,��д�����С����Ĭ������
	if (!SetCommState(m_ucom, &dcb))
	{
	
		std::cout << "Set Serial port error: " <<GetLastError()<< std::endl;
		return false;
	}

	/*
	//���ö�д��������С����ѡ�����ñ�ʾ����Ĭ������
	if (!SetupComm(m_ucom, 1048576, 1048576))
	{
		std::cout << "Set the Size of Serial Receive Buffer and Send Buffer Failed, Error: " <<GetLastError()<< std::endl;
		return false;
	}
	*/

	//�����շ��ͽ��ջ�����
	if (!PurgeComm(m_ucom, PURGE_TXCLEAR | PURGE_RXCLEAR))
	{

		std::cout << "Clear Receiver Buffer and Send Buffer Failed, Error: " << GetLastError() << std::endl;
		return false;
	}
	return true;
}

int SerialPort::ReadPort(void* addr,DWORD size,UINT timeout)
{
	COMMTIMEOUTS ReadCommTimeout;    //�ṹ��������ͣ��涨ʹ��ʱ���������ͣ����ͺͽ��չ��ã���Ա������ȡ�ַ������ʱ�䣬����ʱ���Multiplier of characters������ʲô��������
	memset(&ReadCommTimeout,0X0,sizeof(ReadCommTimeout));
	ReadCommTimeout.ReadTotalTimeoutConstant = timeout;

	//���ö�ȡ��������ʱʱ��
	SetCommTimeouts(m_ucom,&ReadCommTimeout);

	//���Դ洢ʵ�ʴӴ��ڶ�ȡ����DWORD��
	DWORD dwReadBytes = 0;

	//�ӻ�������ȡ����
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
	COMMTIMEOUTS WriteCommTimeout;    //�ṹ��������ͣ����ͺͽ��չ��ã���Ա������ȡ�ַ������ʱ�䣬����ʱ���Multiplier of characters������ʲô��������
	memset(&WriteCommTimeout, 0X0, sizeof(WriteCommTimeout));
	WriteCommTimeout.WriteTotalTimeoutConstant = timeout;

	//����д��������ʱʱ��
	SetCommTimeouts(m_ucom, &WriteCommTimeout);

	DWORD dwWriteBytes = 0;

	//�򻺴�����д����
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
		//�ر��봮�ڶ�Ӧ������
		return CloseHandle(m_ucom);
	}
	return true;
}

