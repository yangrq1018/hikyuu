/*
 * SelectorBase.cpp
 *
 *  Created on: 2016年2月21日
 *      Author: fasiondog
 */

#include "SelectorBase.h"

namespace hku {

HKU_API std::ostream& operator<<(std::ostream& os, const SelectorBase& st) {
    os << "Selector(" << st.name() << ", " << st.getParameter() << ")";
    return os;
}

HKU_API std::ostream& operator<<(std::ostream& os, const SelectorPtr& st) {
    if (st) {
        os << *st;
    } else {
        os << "Selector(NULL)";
    }

    return os;
}

SelectorBase::SelectorBase() : m_name("SelectorBase") {
    initParam();
}

SelectorBase::SelectorBase(const string& name) : m_name(name) {
    initParam();
}

SelectorBase::~SelectorBase() {}

void SelectorBase::initParam() {
    // 通常原型系统不参与计算，但某些特殊的场景，需要依赖于伴生系统策略，
    // 此时可以认为实际执行的系统行为跟随伴生系统的买卖交易，如依赖于SG进行选择
    // (不过由于仅依赖SG的场景不严谨，因为原型和实际系统的SG是一样的)
    // 此时，需要在自身计算之前执行原型系统，然后SE自行时可以使用。
    // 而对于实际系统和被跟随的系统完全不一样的情况，可以自行设计特殊的SE。
    setParam<bool>("depend_on_proto_sys", false);  // 此种情况，需要原型系统可独立运行
}

void SelectorBase::baseCheckParam(const string& name) const {}

void SelectorBase::paramChanged() {
    m_calculated = false;
}

void SelectorBase::removeAll() {
    m_pro_sys_list.swap(SystemList());
    m_real_sys_list.swap(SystemList());
}

void SelectorBase::reset() {
    m_calculated = false;
    SystemList::const_iterator iter = m_pro_sys_list.begin();
    for (; iter != m_pro_sys_list.end(); ++iter) {
        (*iter)->reset();
    }

    m_real_sys_list.clear();
    _reset();
}

SelectorPtr SelectorBase::clone() {
    SelectorPtr p;
    try {
        p = _clone();
    } catch (...) {
        HKU_ERROR("Subclass _clone failed!");
        p = SelectorPtr();
    }

    if (!p || p.get() == this) {
        HKU_ERROR("Failed clone! Will use self-ptr!");
        return shared_from_this();
    }

    p->m_params = m_params;
    p->m_name = m_name;

    p->m_real_sys_list.reserve(m_real_sys_list.size());
    for (const auto& sys : m_real_sys_list) {
        p->m_real_sys_list.emplace_back(sys->clone());
    }

    p->m_pro_sys_list.reserve(m_pro_sys_list.size());
    for (const auto& sys : m_real_sys_list) {
        p->m_pro_sys_list.emplace_back(sys->clone());
    }
    return p;
}

void SelectorBase::calculate(const SystemList& pf_realSysList, const KQuery& query) {
    m_real_sys_list = pf_realSysList;

    // 需要依赖于运行系统，在自身运算之前完成计算
    if (getParam<bool>("depend_on_proto_sys")) {
        for (auto& sys : m_pro_sys_list) {
            sys->run(query);
        }
    }

    _calculate();
    m_calculated = true;
}

void SelectorBase::addStock(const Stock& stock, const SystemPtr& protoSys) {
    HKU_CHECK(!stock.isNull(), "The input stock is null!");
    HKU_CHECK(protoSys, "The input stock is null!");
    HKU_CHECK(protoSys->getMM(), "protoSys missing MoneyManager!");
    HKU_CHECK(protoSys->getSG(), "protoSys missing Siganl!");
    HKU_CHECK(!protoSys->getParam<bool>("shared_tm"), "Unsupport shared TM for protoSys!");
    if (getParam<bool>("depend_on_proto_sys")) {
        HKU_CHECK(protoSys->getTM(),
                  "Scenarios that depend on prototype systems need to specify a TM!");
    }

    auto proto = protoSys;
    proto->forceResetAll();
    SYSPtr sys = proto->clone();
    sys->reset();
    sys->setStock(stock);
    m_pro_sys_list.emplace_back(sys);
}

void SelectorBase::addStockList(const StockList& stkList, const SystemPtr& protoSys) {
    for (const auto& stk : stkList) {
        addStock(stk, protoSys);
    }
}

} /* namespace hku */
