from passlib.context import CryptContext
from datetime import datetime, timedelta, timezone
import jwt
from pydantic_settings import BaseSettings, SettingsConfigDict
from pathlib import Path
from dotenv import load_dotenv

BASE_DIR = Path(__file__).resolve().parent
load_dotenv(BASE_DIR / '.env')

class Settings(BaseSettings):
    JWT_SECRET: str
    JWT_ALGORITHM: str = 'HS256'
    JWT_EXPIRE_MINUTES: int = 120
    PASSWORD_SALT: str = 'salt'
    AVATAR_UPLOAD_PATH: str = 'static/avatars'
    MAX_AVATAR_SIZE: int = 5242880  # 5MB
    model_config = SettingsConfigDict(env_file=str(BASE_DIR / '.env'), extra='ignore')

settings = Settings()

pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")


def hash_password(password: str) -> str:
    return pwd_context.hash(password + settings.PASSWORD_SALT)


def verify_password(password: str, hashed: str) -> bool:
    return pwd_context.verify(password + settings.PASSWORD_SALT, hashed)


def create_access_token(data: dict, expires_minutes: int | None = None) -> str:
    to_encode = data.copy()
    expire = datetime.now(timezone.utc) + timedelta(minutes=expires_minutes or settings.JWT_EXPIRE_MINUTES)
    to_encode.update({"exp": expire})
    encoded_jwt = jwt.encode(to_encode, settings.JWT_SECRET, algorithm=settings.JWT_ALGORITHM)
    return encoded_jwt


def decode_token(token: str) -> dict:
    return jwt.decode(token, settings.JWT_SECRET, algorithms=[settings.JWT_ALGORITHM])


def get_current_user_id(token: str) -> int:
    """从JWT token中获取当前用户ID"""
    try:
        payload = decode_token(token)
        user_id = int(payload.get("sub"))
        return user_id
    except (jwt.InvalidTokenError, ValueError, TypeError):
        raise ValueError("Invalid token") 