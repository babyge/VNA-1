#include "trace.h"

using namespace std;

Trace::Trace(QString name, QColor color)
    : _name(name),
      _color(color),
      _liveType(LivedataType::Overwrite),
      visible(true),
      paused(false),
      touchstone(false)
{

}

Trace::~Trace()
{
    emit deleted(this);
}

void Trace::clear() {
    _data.clear();
    emit cleared(this);
    emit dataChanged();
}

void Trace::addData(Trace::Data d) {
    // add or replace data in vector while keeping it sorted with increasing frequency
    auto lower = lower_bound(_data.begin(), _data.end(), d, [](const Data &lhs, const Data &rhs) -> bool {
        return lhs.frequency < rhs.frequency;
    });
    if(lower == _data.end()) {
        // highest frequency yet, add to vector
        _data.push_back(d);
    } else if(lower->frequency == d.frequency) {
        // replace this data element
        *lower = d;
    } else {
        // insert at this position
        _data.insert(lower, d);
    }
    emit dataAdded(this, d);
    emit dataChanged();
}

void Trace::setName(QString name) {
    _name = name;
    emit nameChanged();
}

void Trace::fillFromTouchstone(Touchstone &t)
{
    clear();
    for(unsigned int i=0;i<t.points();i++) {
        auto tData = t.point(i);
        Data d;
        d.frequency = tData.frequency;
        if(t.ports() == 1) {
            // use S11 parameter
            d.S = t.point(i).S[0];
        } else {
            // use S21 parameter
            d.S = t.point(i).S[2];
        }
        addData(d);
    }
    if(t.ports() == 1) {
        reflection = true;
    } else {
        reflection = false;
    }
    touchstone = true;
}

void Trace::fromLivedata(Trace::LivedataType type, LiveParameter param)
{
    touchstone = false;
    _liveType = type;
    _liveParam = param;
    if(param == LiveParameter::S11 || param == LiveParameter::S22) {
        reflection = true;
    } else {
        reflection = false;
    }
}

void Trace::setColor(QColor color) {
    _color = color;
}

void Trace::setVisible(bool visible)
{
    if(visible != this->visible) {
        this->visible = visible;
        emit visibilityChanged(this);
    }
}

bool Trace::isVisible()
{
    return visible;
}

bool Trace::pause()
{
    paused = true;
}

bool Trace::resume()
{
    paused = false;
}

bool Trace::isPaused()
{
    return paused;
}

bool Trace::isTouchstone()
{
    return touchstone;
}

bool Trace::isReflection()
{
    return reflection;
}
