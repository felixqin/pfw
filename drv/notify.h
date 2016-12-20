#pragma once

#include "../public/log.h"


class CTNLink
{
	NOTIFICATION_INFO	*info;
	CTNLink				*next;

	friend class CTdiNotify;
};

class CTdiNotify
{
//����
public:
	void Initialize();
	void Uninitialize();

	NTSTATUS Start();
	NTSTATUS Stop();

	//��Ӧ�ò������֪ͨ
	NTSTATUS Notify( ULONG type, PVOID pContext, USHORT size );
	//ȡ��֪ͨ��Ϣ
	NTSTATUS FetchInfo( PNOTIFICATION_INFO pInfo );

//����
public:
	//
	USHORT FrontInfoSize();

private:
	PNOTIFICATION_INFO create_notification_info( ULONG type, PVOID pContext, USHORT lSize );

//������в���, �μ������ݽṹ��(�廪��) ADT Queue
private:
	void ini_queue();		//��ʼ��
	void en_queue(CTNLink *node);	//���
	CTNLink *dl_queue();		//����
	void clear_queue();		//�ÿ�

//˽�б���
private:
	CTNLink	*m_front;	//��ͷ
	CTNLink	*m_rear;	//��β
	ULONG	m_count;	//�ӳ�

	PRKEVENT	m_event;
	HANDLE		m_event_handle;
	BOOLEAN		m_enable;	//�Ƿ�������Ӧ�ò㷢��Ϣ
	KSPIN_LOCK	m_guard;
};
