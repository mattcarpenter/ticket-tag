#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "TicketTag.h"
#include <opencv2/core.hpp>
#include <nan.h>
#include <iostream>
#include <v8.h>
#include <node_buffer.h>

using namespace v8;
using namespace std;
using namespace cv;
using namespace node;

struct HexCharStruct
{
  unsigned char c;
  HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
  return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
{
  return HexCharStruct(_c);
}


NAN_METHOD(extractFromImage) {

  Local<Array> results = Nan::New<Array>();

  // Method expects a node Buffer as the first argument
  if (!Buffer::HasInstance(info[0]))
  {
    Nan::ThrowTypeError("Argument is not a Node buffer");
    return;
  }

  // Decode raw image data from the node Buffer
  uint8_t *buf = (uint8_t *) Buffer::Data(info[0]->ToObject());
  unsigned len = Buffer::Length(info[0]->ToObject());
  cv::Mat raw_data = cv::Mat(len, 1, CV_8UC3, buf);
  cv::Mat image = imdecode(raw_data, 1);

  // Extract ticket tags from source image
  vector<TicketTag> tags = TicketTag::extract_from_image(image);

  // Iterate through the results and compose our return array
  for(std::size_t i = 0; i < tags.size(); ++i)
  {
    // Encode the image
    std::vector<uchar> buf(0);
    imencode(".jpg", tags[i].get_image(), buf);

    // Create a a v8 buffer containing the image data
    Nan::MaybeLocal<v8::Object> node_buffer = Nan::CopyBuffer(reinterpret_cast<char*>(buf.data()), buf.size());

    // Create an object to contain the rect points
    Local<Array> rect_points = Nan::New<Array>();
    Local<Object> rect_obj = Nan::New<Object>();
    RotatedRect rotated_rect = tags[i].get_rect();
    Point2f vertices[4];
    rotated_rect.points(vertices);
    for (int v = 0; v < 4; v++)
    {
      Local<Object> point = Nan::New<Object>();
      point->Set(Nan::New<v8::String>("x").ToLocalChecked(), Nan::New<Number>(vertices[v].x));
      point->Set(Nan::New<v8::String>("y").ToLocalChecked(), Nan::New<Number>(vertices[v].y));
      rect_points->Set(v, point);
    }

    Local<Object> rect_center = Nan::New<Object>();
    rect_center->Set(Nan::New<v8::String>("x").ToLocalChecked(), Nan::New<Number>(rotated_rect.center.x));
    rect_center->Set(Nan::New<v8::String>("y").ToLocalChecked(), Nan::New<Number>(rotated_rect.center.y));

    rect_obj->Set(Nan::New<v8::String>("points").ToLocalChecked(), rect_points);
    rect_obj->Set(Nan::New<v8::String>("center").ToLocalChecked(), rect_center);

    // Create an object to represent this image tag and push into the return array
    Local<Object> obj = Nan::New<Object>();
    obj->Set(Nan::New<v8::String>("data").ToLocalChecked(), node_buffer.ToLocalChecked());
    obj->Set(Nan::New<v8::String>("rect").ToLocalChecked(), rect_obj);
    results->Set(i, obj);
  }

  info.GetReturnValue().Set(results);
}

NAN_METHOD(create) {

  if (!info[0]->IsString())
  {
    Nan::ThrowTypeError("text must be a string");
    return;
  }

  if (!Buffer::HasInstance(info[1]))
  {
    Nan::ThrowTypeError("characterMap must be a buffer");
    return;
  }

  // get std::string from argument 0
  v8::String::Utf8Value arg0(info[0]->ToString());
  std::string text = std::string(*arg0);

  // decode raw image data from argument 1 buffer
  uint8_t *buf = (uint8_t *) Buffer::Data(info[1]->ToObject());
  unsigned len = Buffer::Length(info[1]->ToObject());
  cv::Mat raw_data = cv::Mat(len, 1, CV_8UC3, buf);
  cv::Mat character_map = imdecode(raw_data, 1);

  // Create the ticket tag
  TicketTag tag(text, character_map);

  // Encode the ticket tag image and create a buffer to return
  std::vector<uchar> tag_buf(0);
  imencode(".jpg", tag.get_image(), tag_buf, std::vector<int>({CV_IMWRITE_JPEG_QUALITY,75}));
  Nan::MaybeLocal<v8::Object> node_buffer = Nan::CopyBuffer(reinterpret_cast<char*>(tag_buf.data()), tag_buf.size());
  info.GetReturnValue().Set(node_buffer.ToLocalChecked());
}

NAN_MODULE_INIT(init) {
  Nan::SetMethod(target, "extractFromImage", extractFromImage);
  Nan::SetMethod(target, "create", create);
}

NODE_MODULE(tickettag, init)
