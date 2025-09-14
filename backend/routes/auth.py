from fastapi import APIRouter, Depends, HTTPException, UploadFile, File, Header
from sqlalchemy.orm import Session
from db import get_db
from models import User
from schemas import AuthRequest, AuthResponse, UserProfile, AvatarUploadResponse
from security import hash_password, verify_password, create_access_token, get_current_user_id, settings
import os
import uuid
from pathlib import Path
from PIL import Image
import io

router = APIRouter()

@router.post('/register')
def register(req: AuthRequest, db: Session = Depends(get_db)):
    # 参数化查询由 ORM 自动处理防注入
    user = db.query(User).filter(User.username == req.username).first()
    if user:
        raise HTTPException(status_code=400, detail='Username already exists')
    u = User(username=req.username, password_hash=hash_password(req.password))
    db.add(u)
    db.commit()
    return {"ok": True}

@router.post('/login', response_model=AuthResponse)
def login(req: AuthRequest, db: Session = Depends(get_db)):
    user = db.query(User).filter(User.username == req.username).first()
    if not user or not verify_password(req.password, user.password_hash):
        raise HTTPException(status_code=401, detail='Invalid credentials')
    token = create_access_token({"sub": str(user.id), "username": user.username})
    return AuthResponse(token=token, username=user.username, avatar_url=user.avatar_url)


@router.get('/profile', response_model=UserProfile)
def get_profile(authorization: str = Header(...), db: Session = Depends(get_db)):
    """获取当前用户信息"""
    try:
        token = authorization.replace("Bearer ", "")
        user_id = get_current_user_id(token)
        user = db.query(User).filter(User.id == user_id).first()
        if not user:
            raise HTTPException(status_code=404, detail='User not found')
        return UserProfile(id=user.id, username=user.username, level=user.level, avatar_url=user.avatar_url)
    except ValueError:
        raise HTTPException(status_code=401, detail='Invalid token')


@router.post('/avatar', response_model=AvatarUploadResponse)
def upload_avatar(
    file: UploadFile = File(...),
    authorization: str = Header(...),
    db: Session = Depends(get_db)
):
    """上传用户头像"""
    try:
        # 验证token并获取用户ID
        token = authorization.replace("Bearer ", "")
        user_id = get_current_user_id(token)
        
        # 验证文件类型
        if not file.content_type or not file.content_type.startswith('image/'):
            raise HTTPException(status_code=400, detail='File must be an image')
        
        # 验证文件大小
        if file.size and file.size > settings.MAX_AVATAR_SIZE:
            raise HTTPException(status_code=400, detail=f'File too large. Maximum size is {settings.MAX_AVATAR_SIZE} bytes')
        
        # 创建上传目录
        upload_dir = Path(settings.AVATAR_UPLOAD_PATH)
        upload_dir.mkdir(parents=True, exist_ok=True)
        
        # 生成唯一文件名
        file_extension = Path(file.filename).suffix.lower() if file.filename else '.jpg'
        if file_extension not in ['.jpg', '.jpeg', '.png', '.gif', '.webp']:
            file_extension = '.jpg'
        
        unique_filename = f"{user_id}_{uuid.uuid4().hex}{file_extension}"
        file_path = upload_dir / unique_filename
        
        # 读取并处理图片
        content = file.file.read()
        
        # 使用PIL处理图片，确保格式正确并压缩大小
        try:
            image = Image.open(io.BytesIO(content))
            
            # 转换为RGB（如果是RGBA或其他格式）
            if image.mode in ('RGBA', 'LA', 'P'):
                background = Image.new('RGB', image.size, (255, 255, 255))
                if image.mode == 'P':
                    image = image.convert('RGBA')
                background.paste(image, mask=image.split()[-1] if image.mode == 'RGBA' else None)
                image = background
            elif image.mode != 'RGB':
                image = image.convert('RGB')
            
            # 调整大小（最大200x200）
            image.thumbnail((200, 200), Image.Resampling.LANCZOS)
            
            # 保存图片
            image.save(file_path, 'JPEG', quality=85, optimize=True)
            
        except Exception as e:
            raise HTTPException(status_code=400, detail='Invalid image file')
        
        # 生成访问URL
        avatar_url = f"/{settings.AVATAR_UPLOAD_PATH}/{unique_filename}"
        
        # 更新用户头像URL
        user = db.query(User).filter(User.id == user_id).first()
        if not user:
            # 如果用户不存在，删除刚上传的文件
            file_path.unlink(missing_ok=True)
            raise HTTPException(status_code=404, detail='User not found')
        
        # 删除旧头像文件
        if user.avatar_url:
            old_file_path = Path(user.avatar_url.lstrip('/'))
            if old_file_path.exists():
                old_file_path.unlink(missing_ok=True)
        
        user.avatar_url = avatar_url
        db.commit()
        
        return AvatarUploadResponse(avatar_url=avatar_url, message="Avatar uploaded successfully")
        
    except ValueError:
        raise HTTPException(status_code=401, detail='Invalid token')
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e)) 