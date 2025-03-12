#include "widget.h"

#include <QApplication>

vector<string> split_args(const string& input) {
    istringstream iss(input);
    vector<string> args;
    string token;
    while (iss >> token) {
        args.push_back(token);
    }
    return args;
}

void show_help() {
    cout << "可用命令：\n"
         << "  add <type> <name>      添加元素\n"
         << "  struct <name>          开始结构体\n"
         << "  end                    结束结构体\n"
         << "  show                   显示当前结构\n"
         << "  save <filename>        保存到文件\n"
         << "  help                   显示帮助\n"
         << "  exit                   退出程序\n";
}

int main() {
    ICDGenerator generator;
    string input;

    cout << "ICD XML生成工具（输入help查看帮助）\n";
    cout << "支持的类型：";
    for (const auto& type : {"float", "double", "int32", "uint32", "string", "bool"}) {
        cout << type << " ";
    }
    cout << "\n\n";

    while (true) {
        cout << "> ";
        getline(cin, input);

        vector<string> args = split_args(input);
        if (args.empty()) continue;

        try {
            if (args[0] == "add" && args.size() >= 3) {
                generator.add_element(args[1], args[2]);
                cout << "已添加元素: " << args[2] << " (" << args[1] << ")\n";
            }
            else if (args[0] == "struct" && args.size() >= 2) {
                generator.start_struct(args[1]);
                cout << "进入结构体: " << args[1] << "\n";
            }
            else if (args[0] == "end") {
                generator.end_struct();
                cout << "返回上级结构\n";
            }
            else if (args[0] == "show") {
                cout << generator.format_xml() << endl;
            }
            else if (args[0] == "save" && args.size() >= 2) {
                generator.save_file(args[1]);
                cout << "文件已保存至: " << args[1] << "\n";
            }
            else if (args[0] == "help") {
                show_help();
            }
            else if (args[0] == "exit") {
                break;
            }
            else {
                cout << "无效命令\n";
                show_help();
            }
        } catch (const exception& e) {
            cerr << "错误: " << e.what() << "\n";
        }

        cout << "------------------------\n";
    }

    cout << "感谢使用！\n";
    return 0;
}

int _main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
