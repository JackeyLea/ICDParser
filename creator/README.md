# creator

创建表示ICD的xml文件

## 功能

- 支持嵌套结构体（struct/end）
- 类型有效性检查
- 实时XML预览（show命令）
- 自动格式化输出
- 错误处理机制

## 使用说明

​编译运行：
```bash
g++ -std=c++11 icd_gen.cpp pugixml.cpp -o icd_gen
./icd_gen
​交互命令示例：
text
> struct GPS
> add double longitude
> add double latitude
> struct time
> add uint32 year
> add uint8 month
> end
> show
> save gps.xml
```
​生成XML示例：
```xml
<?xml version='1.0' encoding='UTF-8'?>
<icd>
  <struct name="GPS">
    <element type="double" name="longitude" />
    <element type="double" name="latitude" />
    <struct name="time">
      <element type="uint32" name="year" />
      <element type="uint8" name="month" />
    </struct>
  </struct>
</icd>
```