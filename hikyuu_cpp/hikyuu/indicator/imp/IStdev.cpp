/*
 * IStd.cpp
 *
 *  Created on: 2013-4-18
 *      Author: fasiondog
 */

#include "IStdev.h"
#include "../crt/MA.h"

#if HKU_SUPPORT_SERIALIZATION
BOOST_CLASS_EXPORT(hku::IStdev)
#endif

namespace hku {

IStdev::IStdev() : IndicatorImp("STDEV", 1) {
    setParam<int>("n", 10);
}

IStdev::~IStdev() {}

bool IStdev::check() {
    return getParam<int>("n") >= 2;
}

void IStdev::_calculate(const Indicator& data) {
    size_t total = data.size();
    m_discard = data.discard();
    if (m_discard >= total) {
        m_discard = total;
        return;
    }

    int n = getParam<int>("n");

    vector<price_t> pow_buf(data.size());
    price_t ex = 0.0, ex2 = 0.0;
    size_t num = 0;
    size_t start_pos = m_discard;
    size_t first_end = start_pos + n >= total ? total : start_pos + n;
    price_t k = data[start_pos];
    for (size_t i = start_pos; i < first_end; i++) {
        num++;
        price_t d = data[i] - k;
        ex += d;
        price_t d_pow = std::pow(d, 2);
        pow_buf[i] = d_pow;
        ex2 += d_pow;
        _set(num == 1 ? 0. : std::sqrt((ex2 - std::pow(ex, 2) / num) / (num - 1)), i);
    }

    for (size_t i = first_end; i < total; i++) {
        ex -= data[i - n] - k;
        ex2 -= pow_buf[i - n];
        price_t d = data[i] - k;
        ex += d;
        price_t d_pow = std::pow(d, 2);
        pow_buf[i] = d_pow;
        ex2 += d_pow;
        _set(std::sqrt((ex2 - std::pow(ex, 2) / n) / (n - 1)), i);
    }
}

void IStdev::_dyn_run_one_step(const Indicator& ind, size_t curPos, size_t step) {
    size_t start = _get_step_start(curPos, step, ind.discard());
    size_t num = 0;
    price_t ex = 0.0, ex2 = 0.0;
    price_t k = ind[start];
    for (size_t i = start; i <= curPos; i++) {
        num++;
        price_t d = ind[i] - k;
        ex += d;
        ex2 += std::pow(d, 2);
    }
    _set(num <= 1 ? 0.0 : std::sqrt((ex2 - std::pow(ex, 2) / num) / (num - 1)), curPos);
}

Indicator HKU_API STDEV(int n) {
    IndicatorImpPtr p = make_shared<IStdev>();
    p->setParam<int>("n", n);
    return Indicator(p);
}

Indicator HKU_API STDEV(const IndParam& n) {
    IndicatorImpPtr p = make_shared<IStdev>();
    p->setIndParam("n", n);
    return Indicator(p);
}

} /* namespace hku */
