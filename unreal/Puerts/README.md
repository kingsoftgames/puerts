# Puerts

-   此仓库为 [Puerts](https://git.shiyou.kingsoft.com/neon/puerts) 仓库中的 `unreal/Puerts` 目录下所有代码，应由工具管理，自动同步。
-   `ThirdParty/Library` 下的静态库、动态库文件应当从 S3 上下载。

## Release Note

- v1.0.3.5
  - 更新同步最新 Puerts 提交 6401634

- v1.0.3.4
  - 更新同步最新 Puerts 提交 b2ebaab0

- v1.0.3.3

  - 修改 DefaultJSModuleLoader 宏，从平台相关改为判是否为编辑器

- v1.0.3.2

  - 解决 PC 平台自行编译引擎无法调试问题

- v1.0.3.1

  - 非 Shipping 版本默认开启 v8 调试
  - 移除 DefaultJSModuleLoader.cpp 中 ConvertToAbsolutePathForExternalAppForRead 调用

- v1.0.3

  - 同步 puerts v1.0.3 最新修改

- v1.0.2.5

  - 同步 KTS 框架调整期间合并提交

- v1.0.2.4

  - 解决 PEBlueprintMetaData.cpp 在 ios 平台编译错误问题
  - 修复 Blueprint 的 GeneratedClass 为空时导出 dts 崩溃的问题

- v1.0.2.3

  - 同步 dts 生成相关代码，解决生成报错问题
  - 新增忽略列表，解决重定向文件导出重复问题

- v1.0.2.2

  - update 5.1

- v1.0.2.1

  - 升级 Puerts 至 Unreal V1.0.2
  - 默认开启 FText 选项

- v0.1.3

  - 解决Linux平台报错问题

- v0.1.2

  - 新增导出 DTS 时 Ignore Struct、Class 的配置

- v0.1.1

  - 合并 Puerts 最新代码，替换 V8 为 Puerts 默认包

- v0.1.0

  - 默认打开蓝图导出 dts 时不带 Namespace