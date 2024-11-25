#include "mid.h"
#include "main.h"
#include "FOC_math.h"
extern FOC_t FOC;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

//偏置电流校准
struct{
    uint16_t ADC1_IN5;
    uint16_t ADC1_IN6;
    uint16_t ADC2_IN6;
    uint16_t ADC2_IN7;
}bias_current;


ADC_InjectionConfTypeDef sConfigInjected = {
    .InjectedChannel = ADC_CHANNEL_5,
    .InjectedRank = ADC_INJECTED_RANK_1,
    .InjectedNbrOfConversion = 1,
    .InjectedSamplingTime = ADC_SAMPLETIME_1CYCLE_5,
    .ExternalTrigInjecConv = ADC_EXTERNALTRIGINJECCONV_T1_TRGO,
    .AutoInjectedConv = DISABLE,
    .InjectedDiscontinuousConvMode = DISABLE,
    .InjectedOffset = 0
  };
  
      
/**
 * @brief 打开PWM输出和ADC触发采集
 */
uint16_t init_time = 200;
void StartPhashPwm(void)
{
   HAL_TIM_Base_Start_IT(&htim2);

    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    
    HAL_ADCEx_InjectedStart(&hadc1);
	__HAL_ADC_ENABLE_IT(&hadc1, ADC_IT_JEOC);
    HAL_ADCEx_InjectedStart(&hadc2);
	__HAL_ADC_ENABLE_IT(&hadc2, ADC_IT_JEOC);
    
    ADC_ChannelConfTypeDef sConfig = {0};
    uint64_t ADC1_value_sum = 0;
    uint64_t ADC2_value_sum = 0;
    
    /*校准ADC1_IN5 ADC2_IN6*/
    sConfigInjected.InjectedChannel = ADC_CHANNEL_5;     //ADC1_IN5
    HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);

    
    sConfigInjected.InjectedChannel = ADC_CHANNEL_6;     //ADC2_IN6
    HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);
    /*校准1k次*/
    for(int16_t i = 0; i < init_time; i++)
    {
        HAL_Delay(1);
        ADC1_value_sum += HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
        ADC2_value_sum += HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1);
    }
    bias_current.ADC1_IN5 = ADC1_value_sum/init_time;
    bias_current.ADC2_IN6 = ADC2_value_sum/init_time;
    
    /*校准ADC1_IN6 ADC2_IN7*/
    sConfigInjected.InjectedChannel = ADC_CHANNEL_6;     //ADC1_IN6
    HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);
    
    sConfigInjected.InjectedChannel = ADC_CHANNEL_7;     //ADC2_IN7
    HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);
    
    ADC1_value_sum = 0;
    ADC2_value_sum = 0;
    for(int16_t i = 0; i < init_time; i++)
    {
        HAL_Delay(1);
        ADC1_value_sum += HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
        ADC2_value_sum += HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1);
    }
    bias_current.ADC1_IN6 = ADC1_value_sum/init_time;
    bias_current.ADC2_IN7 = ADC2_value_sum/init_time;
    
    /*ADC采集校准后再打开三相pwm输出 防止下管干扰*/
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
}

/**
 * @brief 设置ABC三相重载值
 */

uint32_t ccr1,ccr2,ccr3;
void SetPhashPwm(uint16_t A, uint16_t B, uint16_t C)
{    

//    HAL_ADC_Start_IT(&hadc1); // 或 HAL_ADC_Start_IT(&hadc1) 如果你使用中断
//    HAL_ADC_Start_IT(&hadc2); // 根据需要启动 ADC
    //    HAL_ADCEx_InjectedStart(&hadc2);                       // 启动ADC注入组转换
    //    HAL_ADCEx_InjectedStart(&hadc2);                       // 启动ADC注入组转换

    
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, C);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, B);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, A);
    
    ccr1 = htim1.Instance->CCR1;
    ccr2 = htim1.Instance->CCR2;
    ccr3 = htim1.Instance->CCR3;

}
/**
 * @brief 设置采集指定两个相电流
 * @note  根据FOC.I.SimplingPhash 采集两个相
 */
void SetSamplingPhash(void)
{
    switch (FOC.I.SetSimplingPhash)
    {
    case BC://0b 011 BC相
        sConfigInjected.InjectedChannel = ADC_CHANNEL_6;     // B相 ADC2_IN6
        HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);

        sConfigInjected.InjectedChannel = ADC_CHANNEL_5;     // C相 ADC1_IN5
        HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);
        break;
    case AB://0b 110 AB相
        sConfigInjected.InjectedChannel = ADC_CHANNEL_7;     // A相 ADC2_IN7
        HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);

        sConfigInjected.InjectedChannel = ADC_CHANNEL_6;     // B相 ADC1_IN6
        HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);
        break;
    case AC://0b 101 AC相
        sConfigInjected.InjectedChannel = ADC_CHANNEL_7;     // A相 ADC2_IN7
        HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);

        sConfigInjected.InjectedChannel = ADC_CHANNEL_5;     // C相 ADC1_IN5
        HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);
        break;
    default:
        break;
    }
}


/**
 *@brief 获取三相电流
 */
void GetSamplingCurrent(void)
{
    switch (FOC.I.NowSimplingPhash)
    {
    case 3://0b 011 BC相
        FOC.I.B = (int16_t)(HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1) - bias_current.ADC2_IN6) * ADC_VALUE_OF_CURRENT;
        FOC.I.C = (int16_t)(HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1) - bias_current.ADC1_IN5) * ADC_VALUE_OF_CURRENT;
        FOC.I.A = -FOC.I.B - FOC.I.C;
        break;
    case 6://0b 110 AB相
        FOC.I.A = (int16_t)(HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1) - bias_current.ADC2_IN7) * ADC_VALUE_OF_CURRENT;
        FOC.I.B = (int16_t)(HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1) - bias_current.ADC1_IN6) * ADC_VALUE_OF_CURRENT;
        FOC.I.C = -FOC.I.A - FOC.I.B;
        break;
    case 5://0b 101 AC相
        
        FOC.I.A = (int16_t)(HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1) - bias_current.ADC2_IN7) * ADC_VALUE_OF_CURRENT;
        FOC.I.C = (int16_t)(HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1) - bias_current.ADC1_IN5) * ADC_VALUE_OF_CURRENT;
    
//        adc2_value = HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1);
//        adc1_value = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
//        
//        adc2_close = adc2_value - bias_current.ADC2_IN7;
//        adc1_close = adc1_value - bias_current.ADC1_IN5;
//        FOC.I.A = (float)(adc2_close) * ADC_VALUE_OF_CURRENT;
//        FOC.I.C = (float)(adc1_close) * ADC_VALUE_OF_CURRENT;
        
//        if (FOC.I.A > 1000 || FOC.I.A < -1000)
//        {
//            uint8_t stop;
//            stop = 1;
//        }
        
        FOC.I.B = -FOC.I.A - FOC.I.C;
        break;
    default:
        break;
    }
}