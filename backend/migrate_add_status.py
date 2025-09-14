#!/usr/bin/env python3
"""
数据库迁移脚本：为hero_ratings表添加status列
"""

from sqlalchemy import create_engine, text
from db import settings
import logging

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def migrate_add_status_column():
    """添加status列到hero_ratings表"""
    engine = create_engine(settings.DATABASE_URL)
    
    try:
        with engine.connect() as conn:
            # 检查status列是否已存在
            result = conn.execute(text("""
                SELECT column_name 
                FROM information_schema.columns 
                WHERE table_name = 'hero_ratings' 
                AND column_name = 'status'
            """))
            
            if result.fetchone():
                logger.info("Status column already exists, skipping migration")
                return
            
            # 添加status列
            logger.info("Adding status column to hero_ratings table...")
            conn.execute(text("""
                ALTER TABLE hero_ratings 
                ADD COLUMN status VARCHAR(20) NOT NULL DEFAULT 'approved'
            """))
            
            # 更新现有记录的状态为approved（因为这些是历史数据，已经被用户看到）
            logger.info("Setting existing ratings to approved status...")
            conn.execute(text("""
                UPDATE hero_ratings 
                SET status = 'approved' 
                WHERE status = 'approved'  -- 这里实际上是为了触发UPDATE
            """))
            
            conn.commit()
            logger.info("Migration completed successfully!")
            
    except Exception as e:
        logger.error(f"Migration failed: {e}")
        raise

if __name__ == "__main__":
    migrate_add_status_column()
