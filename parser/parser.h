#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include <map>
#include <functional>

using namespace std;

// 对齐策略基类
struct AlignmentPolicy {
    virtual size_t align_offset(size_t current_offset, size_t alignment) const = 0;
    virtual ~AlignmentPolicy() = default;
};

// 无对齐策略
struct NoAlignment : AlignmentPolicy {
    size_t align_offset(size_t current, size_t) const override { return current; }
};

// 标准对齐策略
struct StandardAlignment : AlignmentPolicy {
    size_t align_offset(size_t current, size_t alignment) const override {
        return (current + alignment - 1) & ~(alignment - 1);
    }
};

// 自定义对齐策略
struct CustomAlignment : AlignmentPolicy {
    size_t boundary;

    CustomAlignment(size_t b) : boundary(b) {}
    size_t align_offset(size_t current, size_t) const override {
        return ((current + boundary - 1) / boundary) * boundary;
    }
};

// 数据类型描述结构体
struct DataField {
    string name;
    string type;
    size_t offset;
    size_t size;
    size_t alignment;
    vector<shared_ptr<DataField>> children;
};

// 类型映射表（类型 -> 大小, 对齐）
const map<string, pair<size_t, size_t>> TypeInfo = {
    {"float",  {4, 4}},
    {"double", {8, 8}},
    {"int32",  {4, 4}},
    {"uint32", {4, 4}}
};

// 解析上下文（模板化策略）
template<typename AlignPolicy>
struct ParseContext {
    size_t current_offset = 0;
    vector<shared_ptr<DataField>> fields;
    AlignPolicy align_policy;

    void apply_alignment(size_t alignment) {
        current_offset = align_policy.align_offset(current_offset, alignment);
    }
};

class parser
{
public:
    parser();
};

#endif // PARSER_H
