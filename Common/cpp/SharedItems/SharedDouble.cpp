//
// Created by Szymon Kapala on 2020-02-11.
//

#include "SharedDouble.h"

SharedDouble::SharedDouble(int id, double value) : SharedValue() {
  this->value = value;
  this->id = id;
  this->type = SharedValueType::shared_double;
}

jsi::Value SharedDouble::asValue(jsi::Runtime &rt) const {
  return jsi::Value(this->value);
}

void SharedDouble::setNewValue(std::shared_ptr<SharedValue> sv) {
  SharedDouble * sd = (SharedDouble*) sv.get();
  this->value = sd->value;
  this->dirty = true;
}

jsi::Value SharedDouble::asParameter(jsi::Runtime &rt) {

  class HO : public jsi::HostObject {
    public:
    double * value = nullptr;
    bool * dirty = nullptr;
    int id;

    HO(int id, double * val, bool * dirty) {
      this->value = val;
      this->dirty = dirty;
      this->id = id;
    }

    jsi::Value get(jsi::Runtime &rt, const jsi::PropNameID &name) {
      auto propName = name.utf8(rt);

      if (propName == "value") {
          return jsi::Value(*value);
      } else if (propName == "set") {

        auto callback = [this](
          jsi::Runtime &runtime,
          const jsi::Value &thisValue,
          const jsi::Value *arguments,
          size_t count
        ) -> jsi::Value {
          (*dirty) = true;
          double newValue = arguments[0].asNumber();
          (*value) = newValue;
          return jsi::Value::undefined();
        };
        return jsi::Function::createFromHostFunction(rt, name, 1, callback);

      } else if (propName == "id") {
        return jsi::Value((double)id);
      }

      return jsi::Value::undefined();
    }

  };

  std::shared_ptr<jsi::HostObject> ptr(new HO(id, &value, &dirty));

  return jsi::Object::createFromHostObject(rt, ptr);
}

std::vector<int> SharedDouble::getSharedValues() {
  std::vector<int> res;
  res.push_back(id);
  return res;
}

SharedDouble::~SharedDouble() {

}