#ifndef __BASE_MESSAGE_H__
#define __BASE_MESSAGE_H__


#include <cstdint>
#include "protoc/gprotoc/msg.pb.h"

namespace EASY_TCP {
class MsgContent;
};
struct TcpHeader {
  int64_t length;
  EASY_TCP::MsgContent content;
} __attribute__((aligned(1)));

std::vector<char> serialize_MsgBase(TcpHeader &data);

void dumpContentInfo(const EASY_TCP::MsgContent& content);

int64_t getTimestampNow();

#endif