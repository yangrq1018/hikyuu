/*
 *  Copyright (c) 2023 hikyuu.org
 *
 *  Created on: 2023-11-09
 *      Author: fasiondog
 */

#include "ISlope.h"

#if HKU_SUPPORT_SERIALIZATION
BOOST_CLASS_EXPORT(hku::ISlope)
#endif

namespace hku {

ISlope::ISlope() : IndicatorImp("SLOPE", 1) {
    setParam<int>("n", 22);
}

ISlope::~ISlope() {}

bool ISlope::check() {
    return getParam<int>("n") >= 0;
}

void ISlope::_calculate(const Indicator& ind) {
    size_t total = ind.size();
    m_discard = ind.discard() + 1;
    if (m_discard >= total) {
        m_discard = total;
        return;
    }

    int n = getParam<int>("n");
    if (n <= 1) {
        for (size_t i = m_discard; i < total; i++) {
            _set(0.0, i);
        }
        return;
    }

    size_t startPos = m_discard - 1;
    price_t xsum = 0.0, ysum = 0.0, xysum = 0.0, x2sum = 0.0;
    size_t first_end = startPos + n >= total ? total : startPos + n;
    for (size_t i = startPos; i < first_end; i++) {
        xsum += i;
        ysum += ind[i];
        xysum += i * ind[i];
        x2sum += std::pow(i, 2);
        _set(((i + 1) * xysum - xsum * ysum) / ((i + 1) * x2sum - xsum * xsum), i);
    }

    for (size_t i = first_end; i < total; i++) {
        xsum += n;
        ysum += ind[i] - ind[i - n];
        xysum += ind[i] * i - ind[i - n] * (i - n);
        x2sum += (2 * i - n) * n;
        _set((n * xysum - xsum * ysum) / (n * x2sum - xsum * xsum), i);
    }
}

void ISlope::_dyn_run_one_step(const Indicator& ind, size_t curPos, size_t step) {
    size_t start = _get_step_start(curPos, step, ind.discard());
    if (curPos <= ind.discard()) {
        _set(Null<price_t>(), curPos);
        return;
    }

    if (step <= 1) {
        _set(0, curPos);
        return;
    }

    double n = curPos - start + 1;
    price_t xsum = 0.0, ysum = 0.0, xysum = 0.0, x2sum = 0.0;
    for (size_t i = start; i <= curPos; i++) {
        xsum += i;
        ysum += ind[i];
        xysum += ind[i] * i;
        x2sum += std::pow(i, 2);
    }

    _set((n * xysum - xsum * ysum) / (n * x2sum - std::pow(xsum, 2)), curPos);
}

Indicator HKU_API SLOPE(int n) {
    IndicatorImpPtr p = make_shared<ISlope>();
    p->setParam<int>("n", n);
    return Indicator(p);
}

Indicator HKU_API SLOPE(const IndParam& n) {
    IndicatorImpPtr p = make_shared<ISlope>();
    p->setIndParam("n", n);
    return Indicator(p);
}

}  // namespace hku