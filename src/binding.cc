#include <nan.h>

using namespace v8;

int factorial(int n);

NAN_METHOD(Factorial) {
  if (!info[0]->IsNumber() || info[0]->NumberValue() < 1) {
    Nan::ThrowTypeError("Wrong argument");
    return;
  }

  info.GetReturnValue().Set(factorial(info[0]->NumberValue()));
}

NAN_MODULE_INIT(init) {
  Nan::SetMethod(target, "factorial", Factorial);
}

NODE_MODULE(factorial, init)

int factorial(int n) {
  if (n == 1) {
    return 1;
  } else {
    return factorial(n - 1) * n;
  }
}