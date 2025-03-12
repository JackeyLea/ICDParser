#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    const char* xml_str = R"(
        <icd>
            <element type="double" name="timestamp"/>
            <struct name="position">
                <element type="float" name="x"/>
                <element type="double" name="y"/>
            </struct>
            <element type="int32" name="status"/>
        </icd>
    )";

    // 使用标准对齐解析
    cout << "=== 标准对齐解析 ===" << endl;
    try {
        ParseContext<StandardAlignment> ctx;
        parse_xml_node(pugi::xml_document().load_string(xml_str).child("icd"), ctx);

        // 生成测试数据（带对齐填充）
        vector<uint8_t> aligned_data(32);
        auto* dbl = reinterpret_cast<double*>(aligned_data.data());
        auto* flt = reinterpret_cast<float*>(aligned_data.data() + 8);
        auto* i32 = reinterpret_cast<int32_t*>(aligned_data.data() + 24);

        dbl[0] = 123456.789;     // timestamp
        flt[0] = 1.5f;           // position.x
        dbl[1] = 2.5;            // position.y (对齐到8字节)
        i32[0] = 0xAA55BB66;     // status

        parse_binary(aligned_data.data(), ctx.fields);
    }
    catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
    }

    // 使用无对齐解析
    cout << "\n=== 紧凑模式解析 ===" << endl;
    try {
        ParseContext<NoAlignment> ctx;
        parse_xml_node(pugi::xml_document().load_string(xml_str).child("icd"), ctx);

        // 生成紧凑数据
        vector<uint8_t> packed_data(20);
        auto* dbl = reinterpret_cast<double*>(packed_data.data());
        auto* flt = reinterpret_cast<float*>(packed_data.data() + 8);
        auto* i32 = reinterpret_cast<int32_t*>(packed_data.data() + 16);

        dbl[0] = 123456.789;     // timestamp
        flt[0] = 1.5f;           // position.x
        memcpy(packed_data.data()+12, &dbl[1], 8);  // position.y
        i32[0] = 0xAA55BB66;     // status

        parse_binary(packed_data.data(), ctx.fields);
    }
    catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
    }

    return 0;
}

Widget::~Widget()
{
    delete ui;
}
