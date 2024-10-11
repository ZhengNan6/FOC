#include "SMO_observer.h"
Observer_t Observer;

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
}