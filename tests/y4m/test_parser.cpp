#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "y4m/parser.h"

using namespace y4m;

TEST(Y4MParserTest, ParseHeaderBasic) {
  std::string header = "YUV4MPEG2 W1920 H960 F30000:1001 Ip A1:1 C420jpeg\n";
  Parser parser;
  bool ok = parser.ParseHeader(header);
  EXPECT_TRUE(ok);
  const Y4MHeader& y4m_header = parser.GetHeader();
  EXPECT_EQ(y4m_header.width, 1920);
  EXPECT_EQ(y4m_header.height, 960);
  EXPECT_EQ(y4m_header.framerate_num, 30000);
  EXPECT_EQ(y4m_header.framerate_den, 1001);
  EXPECT_EQ(y4m_header.chroma_format, YUV420ChromaFormat::JPEG);
}

TEST(Y4MParserTest, ParseHeaderInvalid) {
  std::string header = "INVALID_HEADER\n";
  Parser parser;
  bool ok = parser.ParseHeader(header);
  EXPECT_FALSE(ok);
}

TEST(Y4MParserTest, ParseHeaderMPEG2XYSCSS) {
  std::string header = "YUV4MPEG2 W3840 H1920 F30:1 Ip A1:1 C420mpeg2 XYSCSS=420MPEG2\n";
  Parser parser;
  bool ok = parser.ParseHeader(header);
  EXPECT_TRUE(ok);
  const Y4MHeader& y4m_header = parser.GetHeader();
  EXPECT_EQ(y4m_header.width, 3840);
  EXPECT_EQ(y4m_header.height, 1920);
  EXPECT_EQ(y4m_header.framerate_num, 30);
  EXPECT_EQ(y4m_header.framerate_den, 1);
  EXPECT_EQ(y4m_header.chroma_format, YUV420ChromaFormat::MPEG2);
}
