///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name	: policy.h
//	* Author	: �̽���(Seunjae Lee)
//	* Date		: 2012. 7. 16 - 2012. 8. 4
//	* Description
//		 ��å���� �����Ѵ�.
//
///////////////////////////////////////////////////////////////////////////////////////
#define ATTACK_COUNT_POLICY		2		// ���� �ܰ踦 ���� ������ ��Ŷ ����
#define SUSPICIUS_COUNT_POLICY	1		// �ǽ� �ܰ踦 ���� ������ ��Ŷ ����
#define ATTACK_TIME_OVER_SEC	35		// ������ ���߾����� �Ǵ��ϴ� ���ؽð� (sec)
#define WAITING_TIME			2000	// ARP Reply waiting time
#define LOG_MSG_SIZE			2048