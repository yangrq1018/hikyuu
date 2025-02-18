/*
 * ILowLineBars.cpp
 *
 *  Copyright (c) 2019 hikyuu.org
 *
 *  Created on: 2019-4-14
 *      Author: fasiondog
 */

#include "ILowLineBars.h"

#if HKU_SUPPORT_SERIALIZATION
BOOST_CLASS_EXPORT(hku::ILowLineBars)
#endif

namespace hku {

ILowLineBars::ILowLineBars() : IndicatorImp("LLVBARS", 1) {
    setParam<int>("n", 20);
}

ILowLineBars::~ILowLineBars() {}

bool ILowLineBars::check() {
    return getParam<int>("n") >= 0;
}

void ILowLineBars::_calculate(const Indicator& ind) {
    size_t total = ind.size();
    if (0 == total) {
        m_discard = 0;
        return;
    }

    if (ind.discard() >= total) {
        m_discard = total;
        return;
    }

    m_discard = ind.discard();
    if (1 == total) {
        if (0 == m_discard) {
            _set(0, 0);
        }
        return;
    }

    int n = getParam<int>("n");
    if (0 == n) {
        n = total - m_discard;
    } else if (n > total) {
        n = total;
    }

    price_t min = ind[m_discard];
    size_t pre_pos = m_discard;
    size_t start_pos = m_discard + n < total ? m_discard + n : total;
    for (size_t i = m_discard; i < start_pos; i++) {
        if (ind[i] <= min) {
            min = ind[i];
            pre_pos = i;
        }
        _set(i - pre_pos, i);
    }

    for (size_t i = start_pos; i < total; i++) {
        size_t j = i + 1 - n;
        if (pre_pos < j) {
            pre_pos = j;
            min = ind[j];
            for (size_t k = pre_pos + 1; k <= i; k++) {
                if (ind[k] <= min) {
                    min = ind[k];
                    pre_pos = k;
                }
            }
        } else {
            if (ind[i] <= min) {
                min = ind[i];
                pre_pos = i;
            }
        }
        _set(i - pre_pos, i);
    }
}

void ILowLineBars::_dyn_run_one_step(const Indicator& ind, size_t curPos, size_t step) {
    size_t start = _get_step_start(curPos, step, ind.discard());
    price_t minVal = ind[start];
    size_t minPos = start;
    for (size_t i = start + 1; i <= curPos; i++) {
        if (ind[i] < minVal) {
            minVal = ind[i];
            minPos = i;
        }
    }
    _set(curPos - minPos, curPos);
}

Indicator HKU_API LLVBARS(int n) {
    IndicatorImpPtr p = make_shared<ILowLineBars>();
    p->setParam<int>("n", n);
    return Indicator(p);
}

Indicator HKU_API LLVBARS(const IndParam& n) {
    IndicatorImpPtr p = make_shared<ILowLineBars>();
    p->setIndParam("n", n);
    return Indicator(p);
}

} /* namespace hku */
