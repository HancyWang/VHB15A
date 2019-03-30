/// @file stc_eeprom.h
/// @brief define stc mcu internal EEPROM Read-Write
/// @author Hancy
/// @date 2019.03.26
/// @version 2.0.3a
/// @note 

#ifndef __STC_EEPROM_H
#define __STC_EEPROM_H 

typedef unsigned char  INT8U;
typedef unsigned int   INT16U;

//IAP EEPROM��ַ����
#define DATA_FLASH_ADDRESS_First						0x00  //DATA_FLASH �׵�ַ,У���� 'Z'
#define DATA_FLASH_ADDRESS_Work_Mode 				0x01  //DATA_FLASH �д�/�޴�ģʽ 1Ϊ�д�ģʽ ��0Ϊ�޴�ģʽ
#define DATA_FLASH_ADDRESS_Set_RT_WCTemp 		0x02  //DATA_FLASH �д�/�޴��趨�¶� 30-37
#define DATA_FLASH_ADDRESS_Set_RT_YCTemp 		0x03  //DATA_FLASH �д�/�д��趨�¶� 35-40
#define DATA_FLASH_ADDRESS_In_Exp_Ratio 		0x04  //DATA_FLASH In��Exp���ȷ�ʽ�ı���  1-6 //1-1:1 2-1:1.1 3-1:1.2 4-1:1.3 5-1:1.4 6-1:1.5
#define DATA_FLASH_ADDRESS_Set_CQK_WCTemp 	0x05  //DATA_FLASH �д�/�޴��������趨�¶� 30-37
#define DATA_FLASH_ADDRESS_Set_CQK_YCTemp 	0x06  //DATA_FLASH �д�/�д��������趨�¶� 35-40
#define DATA_FLASH_ADDRESS_Language 				0x07  //�������0-3
		 
//����һ��FLASH�ṹ��
struct stc_data_flash
	 {
		 INT8U First; //
		 INT8U Work_Mode; //
		 INT8U Set_RT_WCTemp; //
		 INT8U Set_RT_YCTemp; //
		 INT8U In_Exp_Ratio; //
		 INT8U Set_CQK_WCTemp; //
		 INT8U Set_CQK_YCTemp; //
		 INT8U Language; //����				 
   };//
	 

/// @brief STC_IAP_Byte_Read function
	/// @param [in] add,This is the address to read EEPROM(range 0 - 0x7ff)
	/// @param [out] 
	/// @return 
	/// @retval IAP_DATA,This is the data read from the address of the specified EEPROM
	/// @note 
	/// @details Read a byte data from the address of the specified EEPROM
INT8U STC_IAP_Byte_Read(INT16U add);

/// @brief STC_IAP_Byte_Program function
	/// @param [in] add,This is the address to write EEPROM(range 0 - 0x7ff) 
	/// @param [in] ch,This is the data that needs to be written(range 0 - 0xff))
	/// @param [out] 
	/// @return 
	/// @retval 
	/// @note 
	/// @details Write a byte data to the address of the specified EEPROM
void STC_IAP_Byte_Program(INT16U add, INT8U ch); 

/// @brief STC_IAP_Sector_Erase function
	/// @param [in] add,This is the address to EEPROM(range 0 - 0x7ff) 
	/// @param [out] 
	/// @return 
	/// @retval 
	/// @note 
	/// @details Erase the sector of the specified address
void STC_IAP_Sector_Erase(INT16U add);

#endif