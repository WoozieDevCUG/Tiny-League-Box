from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from pydantic_settings import BaseSettings, SettingsConfigDict
from pathlib import Path
from dotenv import load_dotenv
import os

from routes import auth as auth_routes
from routes import heroes as hero_routes
from routes import ratings as rating_routes

BASE_DIR = Path(__file__).resolve().parent
load_dotenv(BASE_DIR / '.env')

class Settings(BaseSettings):
    APP_NAME: str = 'qt-backend'
    APP_HOST: str = '127.0.0.1'
    APP_PORT: int = 8000
    DATABASE_URL: str
    JWT_SECRET: str
    JWT_ALGORITHM: str = 'HS256'
    JWT_EXPIRE_MINUTES: int = 120
    PASSWORD_SALT: str = 'salt'

    model_config = SettingsConfigDict(env_file=str(BASE_DIR / '.env'), extra='ignore')

settings = Settings()

app = FastAPI(title=settings.APP_NAME)

app.mount("/static", StaticFiles(directory=str(BASE_DIR / "static")), name="static")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

app.include_router(auth_routes.router, prefix="/api/auth", tags=["auth"])
app.include_router(hero_routes.router, prefix="/api", tags=["heroes"])
app.include_router(rating_routes.router, prefix="/api", tags=["ratings"])

@app.get('/')
async def root():
    return {"status": "ok"} 