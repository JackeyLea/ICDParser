#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <map>

using namespace std;

// 数据类型描述结构体
struct DataField {
    string name;
    string type;
    size_t offset;
    size_t size;
    vector<shared_ptr<DataField>> children; // 用于结构体嵌套
};

// 类型映射表（小端字节序）
const map<string, size_t> TypeSize = {
    {"float", 4},
    {"double", 8},
    {"int32", 4},
    {"uint32", 4}
};

// 解析上下文
struct ParseContext {
    size_t current_offset = 0;//偏移量
    vector<shared_ptr<DataField>> fields;//解析结果
};

class parser
{
public:
    parser();
};

#endif // PARSER_H
