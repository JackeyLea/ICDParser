#include "widget.h"

#include <QApplication>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <stdexcept>

using namespace std;
using namespace pugi;

// 支持的数据类型枚举
enum class DataType {
    FLOAT, DOUBLE, INT32, UINT32, UINT8, BOOL, STRING
};

// 类型特征模板
template <DataType T> struct type_traits {};
template <> struct type_traits<DataType::FLOAT>   { using type = float; };
template <> struct type_traits<DataType::DOUBLE>  { using type = double; };
template <> struct type_traits<DataType::INT32>   { using type = int32_t; };
template <> struct type_traits<DataType::UINT32>  { using type = uint32_t; };
template <> struct type_traits<DataType::UINT8>   { using type = uint8_t; };
template <> struct type_traits<DataType::BOOL>     { using type = bool; };
template <> struct type_traits<DataType::STRING>  { using type = string; };

// 数据节点基类
class DataNode {
public:
    virtual ~DataNode() = default;
    string name;
    DataType type;

    virtual void write_binary(ofstream& os) const = 0;
    virtual DataNode* get_child(const string& name) = 0;
};

// 结构体节点
class StructNode : public DataNode {
public:
    map<string, unique_ptr<DataNode>> children;

    DataNode* get_child(const string& name) override {
        auto it = children.find(name);
        return it != children.end() ? it->second.get() : nullptr;
    }

    void write_binary(ofstream& os) const override {
        for (const auto& [_, child] : children) {
            child->write_binary(os);
        }
    }
};

// 值节点模板
template <DataType DT>
class ValueNode : public DataNode {
    using value_type = typename type_traits<DT>::type;
    value_type value;

public:
    ValueNode() { type = DT; }

    void set_value(const string& str_val) {
        istringstream iss(str_val);
        if constexpr (is_same_v<value_type, string>) {
            value = str_val;
        } else if constexpr (is_same_v<value_type, bool>) {
            value = (str_val == "true" || str_val == "1");
        } else {
            iss >> value;
        }
    }

    void write_binary(ofstream& os) const override {
        if constexpr (is_same_v<value_type, string>) {
            auto len = static_cast<uint32_t>(value.size());
            os.write(reinterpret_cast<char*>(&len), sizeof(len));
            os.write(value.data(), len);
        } else {
            os.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }
    }

    DataNode* get_child(const string&) override { return nullptr; }
};

// 支持更多数值类型
template <> struct type_traits<DataType::INT64> { using type = int64_t; };

// 支持数组类型
class ArrayNode : public DataNode {
    vector<unique_ptr<DataNode>> elements;
};

void validate_range(DataNode* node, double min, double max) {
    // 实现数值范围校验逻辑
}

template <typename T>
void write_swapped(ofstream& os, T value) {
    // 实现字节序转换
}

// XML解析器
class ICDParser {
    unique_ptr<DataNode> root;

public:
    void load(const string& filename) {
        xml_document doc;
        if (!doc.load_file(filename.c_str()))
            throw runtime_error("XML文件加载失败");

        root = parse_node(doc.first_child());
    }

    DataNode* get_root() const { return root.get(); }

private:
    unique_ptr<DataNode> parse_node(const xml_node& xml_node) {
        auto node = make_unique<StructNode>();
        node->name = xml_node.attribute("name").as_string();

        for (auto& child : xml_node.children()) {
            string type_name = child.name();
            if (type_name == "struct") {
                auto child_node = parse_node(child);
                node->children[child_node->name] = move(child_node);
            } else if (type_name == "element") {
                auto elem_type = parse_type(child.attribute("type").as_string());
                auto value_node = create_value_node(elem_type);
                value_node->name = child.attribute("name").as_string();
                node->children[value_node->name] = move(value_node);
            }
        }
        return node;
    }

    DataType parse_type(const string& type_str) {
        static map<string, DataType> type_map = {
            {"float", DataType::FLOAT}, {"double", DataType::DOUBLE},
            {"int32", DataType::INT32}, {"uint32", DataType::UINT32},
            {"uint8", DataType::UINT8}, {"bool", DataType::BOOL},
            {"string", DataType::STRING}
        };
        auto it = type_map.find(type_str);
        if (it == type_map.end())
            throw runtime_error("未知数据类型: " + type_str);
        return it->second;
    }

    unique_ptr<DataNode> create_value_node(DataType type) {
        switch (type) {
            case DataType::FLOAT:   return make_unique<ValueNode<DataType::FLOAT>>();
            case DataType::DOUBLE:  return make_unique<ValueNode<DataType::DOUBLE>>();
            case DataType::INT32:   return make_unique<ValueNode<DataType::INT32>>();
            case DataType::UINT32:  return make_unique<ValueNode<DataType::UINT32>>();
            case DataType::UINT8:   return make_unique<ValueNode<DataType::UINT8>>();
            case DataType::BOOL:    return make_unique<ValueNode<DataType::BOOL>>();
            case DataType::STRING:  return make_unique<ValueNode<DataType::STRING>>();
            default: throw runtime_error("不支持的数值类型");
        }
    }
};

// 数据设置工具
class DataSetter {
public:
    static void set_value(DataNode* root, const string& path, const string& value) {
        vector<string> parts = split_path(path);
        DataNode* current = root;

        // 导航到目标节点
        for (size_t i = 0; i < parts.size() - 1; ++i) {
            current = current->get_child(parts[i]);
            if (!current) throw runtime_error("无效路径: " + parts[i]);
        }

        // 设置最终值
        auto value_node = dynamic_cast<ValueNode<DataType::STRING>*>(current->get_child(parts.back()));
        if (!value_node) throw runtime_error("目标不是值节点");
        value_node->set_value(value);
    }

private:
    static vector<string> split_path(const string& path) {
        vector<string> parts;
        size_t start = 0;
        for (size_t end = path.find('.'); end != string::npos; end = path.find('.', start)) {
            parts.push_back(path.substr(start, end - start));
            start = end + 1;
        }
        parts.push_back(path.substr(start));
        return parts;
    }
};

int main() {
    try {
        // 解析XML文件
        ICDParser parser;
        parser.load("gps.xml");

        // 设置示例数据
        DataSetter::set_value(parser.get_root(), "GPS.longitude", "123.45");
        DataSetter::set_value(parser.get_root(), "GPS.latitude", "67.89");
        DataSetter::set_value(parser.get_root(), "GPS.time.year", "2023");
        DataSetter::set_value(parser.get_root(), "GPS.time.month", "12");

        // 导出二进制文件
        ofstream bin_file("data.bin", ios::binary);
        parser.get_root()->write_binary(bin_file);
        cout << "二进制文件已生成: data.bin (" << bin_file.tellp() << " bytes)" << endl;

    } catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
        return 1;
    }
    return 0;
}

int _main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
