#include "mid.h"
#include "main.h"
#include "FOC_math.h"
extern FOC_t FOC;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
/**
 * @brief 打开PWM输出
 */
void StartPhashPwm(void)
{
    // HAL_TIM_Base_Start_IT(&htim2);
    // 启动定时器



//  HAL_ADCEx_Calibration_Start(&hadc1);//自动校准
//    HAL_ADCEx_InjectedStart(&hadc2);                       // 启动ADC注入组转换
//    
//    // 启动 ADC
//    HAL_ADC_Start_IT(&hadc1); // 或 HAL_ADC_Start_IT(&hadc1) 如果你使用中断
//    HAL_ADC_Start_IT(&hadc2); // 根据需要启动 ADC2
//    
    
    HAL_ADCEx_InjectedStart(&hadc1);
     HAL_TIM_Base_Start(&htim1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	__HAL_ADC_ENABLE_IT(&hadc1, ADC_IT_JEOC);
    
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
}
//void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//    if(hadc->Instance == ADC1)
//    {
//        // 处理 ADC1 转换结果
//    }
//    else if(hadc->Instance == ADC2)
//    {
//        // 处理 ADC2 转换结果
//    }
//}

/**
 * @brief 设置ABC三相重载值
 */
uint32_t adc1_value;
uint32_t adc2_value;
void SetPhashPwm(uint16_t A, uint16_t B, uint16_t C)
{    
//    HAL_ADC_Start_IT(&hadc1); // 或 HAL_ADC_Start_IT(&hadc1) 如果你使用中断
//    HAL_ADC_Start_IT(&hadc2); // 根据需要启动 ADC
    //    HAL_ADCEx_InjectedStart(&hadc2);                       // 启动ADC注入组转换
    //    HAL_ADCEx_InjectedStart(&hadc2);                       // 启动ADC注入组转换
     adc1_value = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
     adc2_value = HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1);
    
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, A);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, B);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, C);
}
/**
 * @brief 设置采集指定两个相电流
 * @note  根据FOC.I.SimplingPhash 采集两个相
 */
void SetSamplingPhash(void)
{
    static ADC_ChannelConfTypeDef sConfig = {0};
    switch (FOC.I.SetSimplingPhash)
    {
    case BC://0b 011 BC相
        sConfig.Channel = ADC_CHANNEL_1;     // B相
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);

        sConfig.Channel = ADC_CHANNEL_1;     // C相
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
        HAL_ADC_ConfigChannel(&hadc2, &sConfig);
        break;
    case AB://0b 110 AB相
        sConfig.Channel = ADC_CHANNEL_1;     // A相
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);

        sConfig.Channel = ADC_CHANNEL_1;     // B相
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
        HAL_ADC_ConfigChannel(&hadc2, &sConfig);
        break;
    case AC://0b 101 AC相
        sConfig.Channel = ADC_CHANNEL_1;     // A相
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);

        sConfig.Channel = ADC_CHANNEL_1;     // C相
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
        HAL_ADC_ConfigChannel(&hadc2, &sConfig);
        break;
    default:
        break;
    }
}

void GetSamplingCurrent(void)
{
    switch (FOC.I.SimplingPhash)
    {
    case 3://0b 011 BC相
        FOC.I.B = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
        FOC.I.C = HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1);
        FOC.I.A = -FOC.I.B - FOC.I.C;
        break;
    case 6://0b 110 AB相
        FOC.I.A = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
        FOC.I.B = HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1);
        FOC.I.C = -FOC.I.A - FOC.I.B;
        break;
    case 5://0b 101 AC相
        FOC.I.A = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
        FOC.I.C = HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1);
        FOC.I.B = -FOC.I.A - FOC.I.C;
        break;
    default:
        break;
    }
}