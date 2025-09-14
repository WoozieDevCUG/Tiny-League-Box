from openai import OpenAI
from typing import Literal
import logging
from pydantic_settings import BaseSettings
from pathlib import Path
from dotenv import load_dotenv

BASE_DIR = Path(__file__).resolve().parent
load_dotenv(BASE_DIR / '.env')

logger = logging.getLogger(__name__)

class OpenAISettings(BaseSettings):
    OPENAI_API_KEY: str = ""
    OPENAI_BASE_URL: str = "https://api.openai.com/v1"
    
    class Config:
        env_file = str(BASE_DIR / '.env')
        extra = 'ignore'

class ContentModerationService:
    def __init__(self):
        self.settings = OpenAISettings()
        if not self.settings.OPENAI_API_KEY:
            logger.warning("OpenAI API密钥未配置，跳过内容审核")
            self.enabled = False
            self.client = None
        else:
            self.enabled = True
            self.client = OpenAI(
                api_key=self.settings.OPENAI_API_KEY,
                base_url=self.settings.OPENAI_BASE_URL
            )
    
    async def moderate_comment(self, comment: str) -> Literal['approved', 'rejected']:
        if not self.enabled or not self.client:
            logger.warning("OpenAI API未配置")
            return 'approved'
        
        if not comment or not comment.strip():
            return 'approved'
        
        try:
            response = self.client.moderations.create(
                input=comment
            )
            
            moderation_result = response.results[0]
            
            if moderation_result.flagged:
                logger.info(f"评论被标记: {comment[:50]}...")
                return 'rejected'
            else:
                return 'approved'
                
        except Exception as e:
            logger.error(f"OpenAI API错误: {e}")
            # API调用失败时默认通过
            return 'approved'
    
    async def moderate_comment_with_gpt(self, comment: str) -> Literal['approved', 'rejected']:
        if not self.enabled or not self.client:
            logger.warning("OpenAI API未配置")
            return 'approved'
        
        if not comment or not comment.strip():
            return 'approved'
        
        try:
            system_prompt = """你是一个内容审核助手，负责审核游戏评论内容。请判断以下评论是否违规。

违规内容包括但不限于：
1. 人身攻击、辱骂、仇恨言论
2. 色情、暴力、恐怖内容
3. 政治敏感内容
4. 垃圾信息、广告、spam
5. 恶意刷屏或无意义内容

请只回复 "approved" 或 "rejected"，不要添加其他解释。"""

            response = self.client.chat.completions.create(
                model="gpt-3.5-turbo",
                messages=[
                    {"role": "system", "content": system_prompt},
                    {"role": "user", "content": f"请审核这条游戏评论：{comment}"}
                ],
                max_tokens=10,
                temperature=0.1
            )
            
            result = response.choices[0].message.content.strip().lower()
            
            if result == "rejected":
                logger.info(f"评论被拒绝: {comment[:50]}...")
                return 'rejected'
            else:
                return 'approved'
                
        except Exception as e:
            logger.error(f"OpenAI GPT API错误: {e}")
            # API调用失败时默认通过
            return 'approved'

content_moderator = ContentModerationService()
