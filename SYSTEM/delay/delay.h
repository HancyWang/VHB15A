/// @file delay.h
/// @brief define Software delay
/// @author Hancy
/// @date 2019.03.26
/// @version 2.0.3a
/// @note 

#ifndef __DELAY_H
#define __DELAY_H 

/// @brief delay_us function
	/// @param [in] t,t us(0 - 65535)
	/// @param [out] 
	/// @return 
	/// @retval 
	/// @note 
	/// @details delay t microsecond 
	
#ifndef u16_t
typedef unsigned int u16_t;
#endif
void delay_us(u16_t t);

/// @brief delay_ms function
	/// @param [in] t,t us(0 - 65535)
	/// @param [out] 
	/// @return 
	/// @retval 
	/// @note 
	/// @details delay t Millisecond
void delay_ms(u16_t t);

#endif