#include "parser.h"


parser::parser()
{

}


// XML解析递归函数
void parse_xml_node(pugi::xml_node node, ParseContext& ctx, const string& parent_name = "") {
    for (auto child : node) {
        string node_type = child.name();
        string name = child.attribute("name").as_string();
        string type = child.attribute("type").as_string();

        if (node_type == "element") {
            auto field = make_shared<DataField>();
            field->name = parent_name.empty() ? name : parent_name + "." + name;
            field->type = type;
            field->offset = ctx.current_offset;

            if (TypeSize.find(type) == TypeSize.end()) {
                throw runtime_error("Unsupported type: " + type);
            }
            field->size = TypeSize.at(type);

            ctx.fields.push_back(field);
            ctx.current_offset += field->size;
        }
        else if (node_type == "struct") {
            ParseContext struct_ctx;
            parse_xml_node(child, struct_ctx, parent_name.empty() ? name : parent_name + "." + name);

            // 合并结构体的字段
            ctx.fields.insert(ctx.fields.end(), struct_ctx.fields.begin(), struct_ctx.fields.end());
            ctx.current_offset += struct_ctx.current_offset;
        }
    }
}

// 二进制解析函数
void parse_binary(const uint8_t* data, const vector<shared_ptr<DataField>>& fields) {
    for (const auto& field : fields) {
        cout << field->name << " @ " << field->offset
             << " (" << field->type << "): ";

        // 提取原始数据
        vector<uint8_t> raw(field->size);
        memcpy(raw.data(), data + field->offset, field->size);

        // 根据类型解析
        if (field->type == "float") {
            float value;
            memcpy(&value, raw.data(), sizeof(float));
            cout << value;
        }
        else if (field->type == "double") {
            double value;
            memcpy(&value, raw.data(), sizeof(double));
            cout << value;
        }
        else if (field->type == "int32") {
            int32_t value;
            memcpy(&value, raw.data(), sizeof(int32_t));
            cout << value;
        }
        else if (field->type == "uint32") {
            uint32_t value;
            memcpy(&value, raw.data(), sizeof(uint32_t));
            cout << value;
        }

        cout << endl;
    }
}
