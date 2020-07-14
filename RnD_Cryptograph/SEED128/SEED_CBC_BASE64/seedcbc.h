/*!
* \file seedcbc.h
* \brief SEED CBC ��ȣ �˰���
* \author
* Copyright (c) 2011 by \<KISA\>
*/
#ifndef KISA_SEED_H
#define KISA_SEED_H

#ifdef  __cplusplus
extern "C" {
#endif

#define SEED_BLOCK_SIZE 16			/*!< SEED �� ũ��*/
#define SEED_ENCRYPT	1			/*!< SEED ��ȣȭ ���*/
#define SEED_DECRYPT	0			/*!< SEED ��ȣȭ ���*/

	/*!
	* \brief
	* SEED ���� ���� ��ȣȭ�� ���� SEED Key ����ü
	* \remarks
	* unsigned int key_data[32] �ڷ���
	*/
	typedef struct kisa_seed_key_st {
		unsigned int key_data[32];
	} KISA_SEED_KEY;

	/*!
	* \brief
	* SEED ��ȣȭ �˰��� CBC ���� ������ ���� ���� ����ü
	*/
	typedef struct kisa_seed_cbc_info_st {
		int				encrypt;							/*!< ��ȣȭ/��ȣȭ ��� ������*/
		unsigned int	ivec[4];				/*!< �ʱ� ����*/
		KISA_SEED_KEY	seed_key;							/*!< SEED ��ȣȭ Ű*/
		unsigned int	cbc_buffer[4];		/*!< ���� ����*/
		int				buffer_length;						/*!< ���� ������ ����*/
		unsigned int	cbc_last_block[4];	/*!< CBC ���� ����*/
		int				last_block_flag;					/*!< CBC ���� ���� ��� ����*/
	} KISA_SEED_CBC_INFO;

	/*!
	* \brief
	* SEED CBC �˰��� �ʱ�ȭ �Լ�
	* \param info
	* SEED CBC �˰��� ��� ���� ����ü ���� (�̸� �޸𸮰� �Ҵ�Ǿ� �־�� ��)
	* \param enc
	* �˰��� ��ȣȭ �� ��ȣȭ ��� ���� (��ȣȭ : SEED_ENCRYPT / ��ȣȭ : SEED_DECRYPT)
	* \param user_key
	* ����ڰ� �����ϴ� �Է� Ű (16 bytes)
	* \param iv
	* ����ڰ� �����ϴ� �ʱ�ȭ ���� (16 bytes)
	* \returns
	* �ʱ�ȭ ���� (1) / �޸𸮰� ������ �Ҵ���� �ʾ��� ��� (0)
	* \remarks
	* user_key�� iv�� �ݵ�� 16 bytes�� �� ũ��� ����
	*/
	int KISA_SEED_CBC_init(KISA_SEED_CBC_INFO *info, int enc, unsigned char *user_key, unsigned char *iv);


	/*!
	* \brief
	* SEED CBC �˰��� ���� �� ��ȣȭ �Լ�
	*
	* \param info
	* SEED CBC �˰��� ��� ���� ����ü ���� (KISA_SEED_CBC_init �� �ʱ�ȭ �ʿ�)
	*
	* \param in
	* ����� �Է� ��/��ȣ��
	*
	* \param inLen
	* ����� �Է��� ���� ����
	*
	* \param out
	* ����� �Է¿� ���� ��ȣ��/�� ��� ����
	*
	* \param outLen
	* ��� ���ۿ� ����� �������� ����
	*
	* \returns
	* ���� ���� (1) / �޸𸮰� ������ �Ҵ���� �ʾ��� ��� (0)
	*
	* \remarks
	* -# ����� �Ǵ� ������ ũ��� ����� �Է��� ���� ���� ũ�ų� ���� �̸� �޸� �Ҵ��� �ؾ���
	* -# outLen�� ������ ��¹��� out�� ����� ������� ���̸� �Լ� ���ο��� ������
	*/
	int KISA_SEED_CBC_process(KISA_SEED_CBC_INFO *info, unsigned int *in, int inLen, unsigned int *out, int *outLen);


	/*!
	* \brief
	* SEED CBC �˰��� ���� ���� �� �е�(PKCS7) ó�� �Լ�
	*
	* \param info
	* SEED CBC �˰��� ��� ���� ����ü ���� (KISA_SEED_CBC_init �� �ʱ�ȭ �ʿ�)
	*
	* \param out
	* ����� �Է¿� ���� ���� ��� ���� ����Ǵ� ����
	*
	* \param outLen
	* ��� ���ۿ� ����� �������� ����
	*
	* \returns
	* ���� ���� (1) / �޸𸮰� ������ �Ҵ���� �ʾ��� ��� (0)
	*
	* \remarks
	* ��¹��� out�� SEED �˰����� �Ѻ�(16 bytes) �̻����� �޸� �Ҵ��� �Ǿ� �־�� ��
	*
	*/
	int KISA_SEED_CBC_close(KISA_SEED_CBC_INFO *info, unsigned int *out, int *outLen);

	unsigned int* chartoint32(unsigned char *in, int inLen);
	unsigned char* int32tochar(unsigned int *in, int inLen);

	int KISA_SEED_CBC_ENCRYPT(unsigned char *user_key, unsigned char *iv, unsigned char *in, unsigned int len, unsigned char *out);
	int KISA_SEED_CBC_DECRYPT(unsigned char *user_key, unsigned char *iv, unsigned char *in, unsigned int len, unsigned char *out);


#ifdef  __cplusplus
}
#endif


#endif /* HEADER_SEED_H */