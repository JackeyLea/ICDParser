#include "parser.h"


parser::parser()
{

}

// XML解析递归函数
template<typename AlignPolicy>
void parse_xml_node(pugi::xml_node node,
                   ParseContext<AlignPolicy>& ctx,
                   const string& parent_name = "",
                   size_t struct_alignment = 1) {
    size_t max_alignment = struct_alignment;

    for (auto child : node) {
        string node_type = child.name();
        string name = child.attribute("name").as_string();
        string type = child.attribute("type").as_string();

        if (node_type == "element") {
            if (TypeInfo.find(type) == TypeInfo.end()) {
                throw runtime_error("Unsupported type: " + type);
            }

            auto [type_size, type_alignment] = TypeInfo.at(type);
            max_alignment = max(max_alignment, type_alignment);

            // 应用对齐
            ctx.apply_alignment(type_alignment);

            auto field = make_shared<DataField>();
            field->name = parent_name.empty() ? name : parent_name + "." + name;
            field->type = type;
            field->offset = ctx.current_offset;
            field->size = type_size;
            field->alignment = type_alignment;

            ctx.fields.push_back(field);
            ctx.current_offset += type_size;
        }
        else if (node_type == "struct") {
            ParseContext<AlignPolicy> struct_ctx;
            size_t inner_max_alignment = 1;

            parse_xml_node(child, struct_ctx,
                          parent_name.empty() ? name : parent_name + "." + name,
                          inner_max_alignment);

            // 结构体最终对齐
            ctx.apply_alignment(inner_max_alignment);

            // 调整子字段偏移量
            for (auto& f : struct_ctx.fields) {
                f->offset += ctx.current_offset;
                ctx.fields.push_back(f);
            }

            // 更新当前偏移量（结构体大小包含尾部填充）
            size_t struct_size = struct_ctx.current_offset;
            ctx.current_offset += struct_size;
            max_alignment = max(max_alignment, inner_max_alignment);
        }
    }

    // 更新外层结构体的最大对齐
    struct_alignment = max(struct_alignment, max_alignment);
}

// 二进制解析函数
void parse_binary(const uint8_t* data, const vector<shared_ptr<DataField>>& fields) {
    cout << "Offset\tSize\tType\t\tName\tValue" << endl;
    cout << "------------------------------------------------" << endl;

    for (const auto& field : fields) {
        cout << hex << field->offset << "\t"
             << dec << field->size << "\t"
             << field->type << "\t\t"
             << field->name << "\t";

        const uint8_t* ptr = data + field->offset;

        if (field->type == "float") {
            float value;
            memcpy(&value, ptr, sizeof(float));
            cout << value;
        }
        else if (field->type == "double") {
            double value;
            memcpy(&value, ptr, sizeof(double));
            cout << value;
        }
        else if (field->type == "int32") {
            int32_t value;
            memcpy(&value, ptr, sizeof(int32_t));
            cout << value;
        }
        else if (field->type == "uint32") {
            uint32_t value;
            memcpy(&value, ptr, sizeof(uint32_t));
            cout << value;
        }

        cout << endl;
    }
}
