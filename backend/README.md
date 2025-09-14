## 英雄联盟英雄查看器 - 后端部署说明

### 环境要求
- Python 3.11+
- PostgreSQL 13+

### 🚀 快速部署

#### 1. 安装依赖
```bash
cd backend
pip install -r requirements.txt
```

#### 2. 配置环境变量
复制配置模板并修改：
```bash
cp env.example .env
```

**重要：请修改 `.env` 文件中的以下配置：**
- `DATABASE_URL`: 数据库连接字符串
- `JWT_SECRET`: 生成一个至少32位的强随机密钥
- `PASSWORD_SALT`: 生成一个随机盐值
- `APP_HOST`: 生产环境设置为 `0.0.0.0`

#### 3. 初始化数据库
```bash
# 确保 PostgreSQL 数据库已创建
python run_migrations.py
```

#### 4. 启动服务
```bash
# 开发环境
uvicorn app:app --reload --host 127.0.0.1 --port 8000

# 生产环境
uvicorn app:app --host 0.0.0.0 --port 8000 --workers 4
```

### 🔒 安全提醒
1. **强制修改**默认密码和密钥
2. 生产环境使用 HTTPS
3. 配置防火墙规则
4. 定期备份数据库

### 接口
- POST `/api/auth/register`：注册
- POST `/api/auth/login`：登录，返回 `token`、`username`、`avatar_url`
- GET  `/api/auth/profile`：获取当前用户信息（需 `Authorization: Bearer <token>`）
- POST `/api/auth/avatar`：上传头像（multipart 表单字段名为 `file`）
- GET  `/api/heroes`：英雄列表（支持 `filter_type=route|class` 与 `filter_value`）
- GET  `/api/heroes/{id}`：英雄详情（含详情、技能、皮肤、数值）



