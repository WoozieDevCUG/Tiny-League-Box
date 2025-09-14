## 前端说明（Qt Widgets 客户端）

### 环境要求
- Qt 6.x（已在 Qt 6.9.1 MinGW 64-bit 上验证）

### 构建与运行
优先使用 Qt Creator：
1) 打开 `app/5QGGIGUM6WQ2.pro`
2) 选择合适的 Kit（如 Desktop Qt 6.x MinGW 64-bit）
3) 构建并运行

命令行（可选，取决于本地工具链）：
```bash
cd app
qmake
mingw32-make -j
```

### 后端地址配置
默认后端为 `http://127.0.0.1:8000`。
- 在 `app/widget.cpp` 中：设置基础地址
  - `m_http->setBaseUrl(QUrl("http://127.0.0.1:8000"));`
- 在 `app/components/avatarwidget.cpp` 中：拼接相对头像地址时的基础 URL

若后端地址变更，请同步修改以上位置，并确保后端静态资源可通过 `/static` 访问。

### 使用说明
1) 先启动后端服务（参考 `backend/README.md`）
2) 启动客户端后，可注册/登录
3) 登录成功后可上传头像，头像地址来自后端响应
4) 英雄列表与详情中的图片、皮肤依赖后端静态资源

### 常见问题
- 图片不显示：检查后端是否运行、`/static` 是否可访问、客户端基础地址是否正确
- 登录失败：确认后端 JWT 配置和系统时间正常


