#include "SMO_observer.h"
Observer_t Observer;

#define M_PI 3.1415925f
#define LPF(oldValue, newValue, a) (1-a)*oldValue+a*newValue

/**
 * @brief 饱和sign函数，将输入值限制在 -1 和 1 之间
 */
inline static float sign(float input) {
    if (input > 1)  return 1;
    if (input < -1) return -1;
    return input;
}

/**
 * @brief 滑膜观测得出反电动势
 */
void SMO_observer(void)
{
    //向前欧拉离散化关系下 I(k) = A*I(k-1)+B*(u - z) 
    Observer.AlphaCurreantHat = Observer.A*Observer.AlphaCurreantHat + Observer.B*(Observer.vAlpha - Observer.alphaZ);
    Observer.BetaCurreantHat  = Observer.A*Observer.BetaCurreantHat  + Observer.B*(Observer.vBeta  - Observer.betaZ);
    //滑膜增益得出观测反电动势
    Observer.ErrAlphaCurrent = Observer.AlphaCurreantHat - Observer.realAlphaCurrent;
    Observer.ErrBetaCurrent  = Observer.BetaCurreantHat  - Observer.realBetaCurrent;
    Observer.alphaZ = Observer.H* sign(Observer.ErrAlphaCurrent);
    Observer.betaZ  = Observer.H* sign(Observer.ErrBetaCurrent);
    //得出的观测反电动势会抖动在±H之间，进行低通滤波
    Observer.vAlpha = LPF(Observer.vAlpha,  Observer.alphaZ,    Observer.lpfK);
    Observer.vBeta  = LPF(Observer.vBeta,   Observer.betaZ,     Observer.lpfK);

   
    //四象限反正切得出角度
    Observer.Etheta = -utils_fast_atan2(Observer.vAlpha, Observer.vBeta);
    //应用TIM2计时，微分得出角速度
}

/**
 * @brief 快速四象限反正切算法
 */
float utils_fast_atan2(float y, float x) {
	float abs_y = fabsf(y) + 1e-20f; // kludge to prevent 0/0 condition
	float angle;

	if (x >= 0.0f) {
		float r = (x - abs_y) / (x + abs_y);
		float rsq = r * r;
		angle = ((0.1963f * rsq) - 0.9817f) * r + (M_PI *0.25f );
	} else {
		float r = (x + abs_y) / (abs_y - x);
		float rsq = r * r;
		angle = ((0.1963f * rsq) - 0.9817f) * r + (3.0f * M_PI *0.25f);
	}
    
	if (y < 0.0f) {
		return  -angle;
	} else {
		return  angle;
	}
}