#include "Math.h"
#include <math.h>
const float _sin[91] = {
    SIN0,
    SIN1,
    SIN2,
    SIN3,
    SIN4,
    SIN5,
    SIN6,
    SIN7,
    SIN8,
    SIN9,
    SIN10,
    SIN11,
    SIN12,
    SIN13,
    SIN14,
    SIN15,
    SIN16,
    SIN17,
    SIN18,
    SIN19,
    SIN20,
    SIN21,
    SIN22,
    SIN23,
    SIN24,
    SIN25,
    SIN26,
    SIN27,
    SIN28,
    SIN29,
    SIN30,
    SIN31,
    SIN32,
    SIN33,
    SIN34,
    SIN35,
    SIN36,
    SIN37,
    SIN38,
    SIN39,
    SIN40,
    SIN41,
    SIN42,
    SIN43,
    SIN44,
    SIN45,
    SIN46,
    SIN47,
    SIN48,
    SIN49,
    SIN50,
    SIN51,
    SIN52,
    SIN53,
    SIN54,
    SIN55,
    SIN56,
    SIN57,
    SIN58,
    SIN59,
    SIN60,
    SIN61,
    SIN62,
    SIN63,
    SIN64,
    SIN65,
    SIN66,
    SIN67,
    SIN68,
    SIN69,
    SIN70,
    SIN71,
    SIN72,
    SIN73,
    SIN74,
    SIN75,
    SIN76,
    SIN77,
    SIN78,
    SIN79,
    SIN80,
    SIN81,
    SIN82,
    SIN83,
    SIN84,
    SIN85,
    SIN86,
    SIN87,
    SIN88,
    SIN89,
    SIN90};

float sin(int theta) {
    int theta_cal;
    // 0~360の値に変換
    theta %= 360;
    if (theta < 0)
        theta += 360;
    theta_cal = theta % 90;
    if (theta >= 90 && theta < 180) {
        theta_cal = 90 - theta_cal;
    }
    if (theta >= 270 && theta < 360) {
        theta_cal = 90 - theta_cal;
    }

    if (theta >= 0 && theta <= 90) {
        // 0~90 第一象限
        return _sin[theta_cal];
    } else if (theta > 90 && theta <= 180) {
        // 91~180 第二象限
        return _sin[theta_cal];
    } else if (theta > 180 && theta <= 270) {
        // 181~270 第三象限
        return -_sin[theta_cal];
    } else if (theta > 270 && theta < 360) {
        // 271~360 第四象限
        return -_sin[theta_cal];
    } else {
        return 0;
    }
}

float cos(int theta) {
    return sin(theta + 90);
}

float tan(int theta) {
    theta %= 360;
    if (theta < 0)
        theta += 360;

    if (cos(theta) == 0)
        return 0;
    else
        return sin(theta) / cos(theta);
}

int normalizeDegrees(int theta) {
    theta %= 360;
    if (theta < 0)
        theta += 360;
    return theta;
}

float normalizeRadians(float theta) {
    while (theta < 0) {
        theta += TWO_PI;
    }
    while (theta >= TWO_PI) {
        theta -= TWO_PI;
    }
    return theta;
}

int gapDegrees180(int deg1, int deg2) {
    int a = deg1 - deg2;
    while (a < 0)
        a += 360;
    while (a > 180)
        a -= 360;
    return a;
}

int gapDegrees(int deg1, int deg2) {
    int a = normalizeDegrees(deg1) - normalizeDegrees(deg2);
    if (a < 0)
        a += 360;
    return a;
}

float gapRadians180(float rad1, float rad2) {
    float a = rad1 - rad2;
    while (a < 0)
        a += TWO_PI;
    while (a > PI)
        a -= TWO_PI;
    return a;
}

float gapRadians(float rad1, float rad2) {
    float a = normalizeRadians(rad1) - normalizeRadians(rad2);
    if (a < 0)
        a += TWO_PI;
    return a;
}