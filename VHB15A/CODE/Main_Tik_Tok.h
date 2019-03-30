/// @file Main_Tik_Tok.h
/// @brief define foreground task timescale, functions placed on timed interrupts
/// @author Hancy
/// @date 2019.03.26
/// @version 2.0.3
/// @note 

#ifndef __MAIN_TIK_TOK__
#define __MAIN_TIK_TOK__


/// @brief Main_Tik_Tok_Initial_Fun function
	/// @param [in]
	/// @param [out]
	/// @return 
	/// @retval
	/// @note 
	/// @details Initialize the timer(20mS)
void Main_Tik_Tok_Initial_Fun(void);

/// @brief Timer_T0_INT_Main function
	/// @param [in] 
	/// @param [out]
	/// @return
	/// @retval
	/// @note 
	/// @details Timing(Timer0) interrupt service function
void Timer_T0_INT_Main(void);



#endif