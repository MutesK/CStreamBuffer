#include <iostream>
#include "StreamBuffer.h"



CStreamBuffer::CStreamBuffer()
:m_front(0), m_rear(0), m_bufferSize(enBUFFER_SIZE)
{
	m_buffer = new char[enBUFFER_SIZE];
	memset(m_buffer, 0, enBUFFER_SIZE);

	InitializeCriticalSection(&RQueue);
}

CStreamBuffer::CStreamBuffer(int iSize)
	:m_bufferSize(iSize), m_front(0), m_rear(0)
{
	m_buffer = new char[iSize];
	memset(m_buffer, 0, iSize);

	InitializeCriticalSection(&RQueue);
}


CStreamBuffer::~CStreamBuffer()
{
	delete[] m_buffer;
	DeleteCriticalSection(&RQueue);
}

int	CStreamBuffer::GetBufferSize(void)
{
	return m_bufferSize;
}

/////////////////////////////////////////////////////////////////////////
// ���� ������� �뷮 ���.
//
// Parameters: ����.
// Return: (int)������� �뷮.
/////////////////////////////////////////////////////////////////////////
int	CStreamBuffer::GetUseSize(void)
{
	if (m_rear >= m_front)
		return m_rear - m_front;
	else
		return m_rear + (m_bufferSize - 2) - m_front;
}

/////////////////////////////////////////////////////////////////////////
// ���� ���ۿ� ���� �뷮 ���.
//
// Parameters: ����.
// Return: (int)�����뷮.
/////////////////////////////////////////////////////////////////////////
int	CStreamBuffer::GetFreeSize(void)
{
	return (m_bufferSize - 2) - GetUseSize();
}

/////////////////////////////////////////////////////////////////////////
// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����.
// (������ ���� ����)
//
// Parameters: ����.
// Return: (int)��밡�� �뷮.
////////////////////////////////////////////////////////////////////////
int	CStreamBuffer::GetNotBrokenGetSize(void)
{
	if (m_front >= m_rear)
	{
		int endPointIndex = m_bufferSize - 2;

		return endPointIndex - m_front;
	}
	else 
		return m_rear - m_front;
}
int	CStreamBuffer::GetNotBrokenPutSize(void)
{
	if (m_front <= m_rear)
	{
		int endPointIndex = m_bufferSize - 2;

		return endPointIndex - m_rear;
	}
	else
		return m_front - m_rear;
}

/////////////////////////////////////////////////////////////////////////
// WritePos �� ����Ÿ ����.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��. 
// Return: (int)���� ũ��.
/////////////////////////////////////////////////////////////////////////
int	CStreamBuffer::PutData(char *chpData, int iSize)
{
	int inputSize = 0;

	if (m_front <= m_rear)
	{
		int pSize = GetNotBrokenPutSize();

		if (pSize < iSize)
		{
			memcpy(m_buffer + m_rear, chpData, pSize);
			chpData += pSize;
			inputSize += pSize;

			if (iSize - pSize > m_front)
			{
				// iSize - pSize = ���� 0���� �־�� �� ������ ũ��
				memcpy(m_buffer, chpData, m_front);
				inputSize += m_front;
				m_rear = m_front;
			}
			else
			{
				memcpy(m_buffer, chpData, iSize - pSize);
				inputSize += iSize - pSize;
				m_rear = iSize - pSize;
			}
			return inputSize;
		}
		else
		{
			// ������ �ִ� ���� >= �־�� �ϴ� ����
			memcpy(m_buffer + m_rear, chpData, iSize);
			m_rear += iSize;
			return iSize;
		}
	}
	else
	{
		// rear���� front�� ū ���
		int pSize = m_front - m_rear;

		if (pSize < iSize)
		{
			// pSize��ŭ�� �ְ� �����Ѵ�.
			memcpy(m_buffer + m_rear, chpData, pSize);
			m_rear += pSize;
			return pSize;
		}
		else
		{
			// iSize ��ŭ �� �ִ´�.
			memcpy(m_buffer + m_rear, chpData, iSize);
			m_rear += iSize;
			return iSize;
		}

	}
}

/////////////////////////////////////////////////////////////////////////
// ReadPos ���� ����Ÿ ������. ReadPos �̵�.
//
// Parameters: (char *)����Ÿ ������. (int)ũ��.
// Return: (int)������ ũ��.
/////////////////////////////////////////////////////////////////////////
int	CStreamBuffer::GetData(char *chpDest, int iSize)
{
	int getSize = 0;
	memset(chpDest, 0, sizeof(iSize) + 1);

	if (m_front <= m_rear)
	{
		int pSize = m_rear - m_front;

		if (pSize < iSize)
		{
			memcpy(chpDest, m_buffer + m_front, pSize);
			m_front += pSize;
			return pSize;
		}
		else
		{
			memcpy(chpDest, m_buffer + m_front, iSize);
			m_front += iSize;
			return iSize;
		}
	}
	else
	{
		int pSize = GetNotBrokenGetSize();
		int frontIndex = m_front;
		if (pSize < iSize)
		{
			memcpy(chpDest, m_buffer + m_front, pSize);
			frontIndex += pSize;
			getSize += pSize;

			if (iSize - pSize > m_rear)
			{
				// iSize - pSize = ���� 0���� �־�� �� ������ ũ��
				memcpy(chpDest + pSize, m_buffer, m_rear);
				getSize += m_rear;
				m_front = m_rear;
			}
			else
			{
				memcpy(chpDest + pSize, m_buffer, iSize - pSize);
				getSize += iSize - pSize;
				m_front = iSize - pSize;
			}
			return getSize;
		}
		else
		{
			memcpy(chpDest, m_buffer + m_front, iSize);
			m_front += iSize;
			return iSize;
		}
	}
}

int	CStreamBuffer::Peek(char *chpDest, int iSize)
{
	int getSize = 0;


	if (m_front <= m_rear)
	{
		int pSize = m_rear - m_front;

		if (pSize < iSize)
		{
			memcpy(chpDest, m_buffer + m_front, pSize);
			return pSize;
		}
		else
		{
			memcpy(chpDest, m_buffer + m_front, iSize);
			return iSize;
		}
	}
	else
	{
		int pSize = GetNotBrokenGetSize();
		int frontIndex = m_front;
		if (pSize < iSize)
		{
			memcpy(chpDest, m_buffer + m_front, pSize);
			frontIndex += pSize;
			getSize += pSize;
			chpDest += pSize;

			if (iSize - pSize > m_rear)
			{
				// iSize - pSize = ���� 0���� �־�� �� ������ ũ��
				memcpy(chpDest, m_buffer, m_rear);
				getSize += m_rear;
			}
			else
			{
				memcpy(chpDest, m_buffer, iSize - pSize);
				getSize += iSize - pSize;
			}
			return getSize;
		}
		else
		{
			memcpy(chpDest, m_buffer + m_front, iSize);
			return iSize;
		}
	}
}

void CStreamBuffer::RemoveData(int iSize)
{
	if (m_front <= m_rear)
	{
		int pSize = m_rear - m_front;

		if (pSize < iSize)
		{
			m_front += pSize;
		}
		else
		{
			m_front += iSize;
		}
	}
	else
	{
		int pSize = GetNotBrokenGetSize();
		int frontIndex = m_front;
		if (pSize < iSize)
		{
			frontIndex += pSize;

			if (iSize - pSize > m_rear)
			{
				m_front = m_rear;
			}
			else
			{
				m_front = iSize - pSize;
			}
		}
	}
}

void CStreamBuffer::MoveWritePos(int iSize)
{

	if (m_front <= m_rear)
	{
		int pSize = GetNotBrokenPutSize();

		if (pSize < iSize)
		{

			if (iSize - pSize > m_front)
				m_rear = m_front;
			
			else
				m_rear = iSize - pSize;
			
		}
		else
			m_rear += iSize;
		
	}
	else
	{
		// rear���� front�� ū ���
		int pSize = m_front - m_rear;

		if (pSize < iSize)
			m_rear += pSize;
		else
			m_rear += iSize;
		

	}
}

void CStreamBuffer::ClearBuffer(void)
{
	m_front = 0;
	m_rear = 0;
}

char* CStreamBuffer::GetBufferPtr(void)
{
	return m_buffer;
}

char* CStreamBuffer::GetReadBufferPtr(void)
{
	return m_buffer + m_front;
}


char* CStreamBuffer::GetWriteBufferPtr(void)
{
	return m_buffer + m_rear;
}


void CStreamBuffer::Lock()
{
	EnterCriticalSection(&RQueue);
}
void CStreamBuffer::UnLock()
{
	LeaveCriticalSection(&RQueue);
}