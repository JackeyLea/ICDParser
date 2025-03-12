#ifndef ICDGENERATOR_H
#define ICDGENERATOR_H


#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>

using namespace std;

class ICDGenerator {
private:
    xml_document doc;
    vector<xml_node> node_stack;
    set<string> supported_types = {"float", "double", "int32", "uint32", "string", "bool"};

public:
    ICDGenerator() {
        node_stack.push_back(doc.append_child("icd"));
    }

    void add_element(const string& type, const string& name) {
        if (supported_types.find(type) == supported_types.end()) {
            throw invalid_argument("不支持的类型: " + type);
        }

        xml_node current = node_stack.back();
        xml_node elem = current.append_child("element");
        elem.append_attribute("type") = type.c_str();
        elem.append_attribute("name") = name.c_str();
    }

    // 添加注释功能
    void add_comment(const string& text) {
        node_stack.back().append_child(node_comment).set_value(text.c_str());
    }

    // 添加数组类型支持
    void add_array(const string& type, const string& name, int size) {
        // 实现数组元素生成逻辑
    }

    void start_struct(const string& name) {
        xml_node current = node_stack.back();
        xml_node struct_node = current.append_child("struct");
        struct_node.append_attribute("name") = name.c_str();
        node_stack.push_back(struct_node);
    }

    void end_struct() {
        if (node_stack.size() <= 1) {
            throw logic_error("无法退出根节点");
        }
        node_stack.pop_back();
    }

    string format_xml() {
        ostringstream oss;
        doc.save(oss, "  ", format_indent | format_no_declaration);
        return "<?xml version='1.0' encoding='UTF-8'?>\n" + oss.str();
    }

    void save_file(const string& filename) {
        if (!doc.save_file(filename.c_str(), "  ", format_indent | format_write_bom)) {
            throw runtime_error("文件保存失败");
        }
    }
};

#endif // ICDGENERATOR_H
