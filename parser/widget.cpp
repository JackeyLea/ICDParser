#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // XML示例
    const char* xml_str = R"(
        <icd>
            <element type="float" name="lat"/>
            <element type="float" name="lon"/>
            <struct name="pos">
                <element type="float" name="x"/>
                <element type="float" name="y"/>
                <element type="float" name="z"/>
            </struct>
        </icd>
    )";

    // 解析XML
    pugi::xml_document doc;
    if (!doc.load_string(xml_str)) {
        cerr << "XML解析失败" << endl;
        return 1;
    }

    try {
        ParseContext ctx;
        parse_xml_node(doc.child("icd"), ctx);

        // 生成测试数据（小端格式）
        vector<uint8_t> binary_data(20); // 5 floats * 4 bytes
        auto* floats = reinterpret_cast<float*>(binary_data.data());
        floats[0] = 1.0f;   // lat
        floats[1] = 2.0f;   // lon
        floats[2] = 3.0f;   // x
        floats[3] = 4.0f;   // y
        floats[4] = 5.0f;   // z

        // 解析并输出
        cout << "解析结果：" << endl;
        parse_binary(binary_data.data(), ctx.fields);
    }
    catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
        return 1;
    }
}

Widget::~Widget()
{
    delete ui;
}
