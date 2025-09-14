from fastapi import APIRouter, Depends, HTTPException, Header
from sqlalchemy.orm import Session
from sqlalchemy import func
from sqlalchemy.exc import IntegrityError
from db import get_db
from models import Hero, HeroRating, User
from schemas import RatingCreate, RatingItem, RatingSummary
from security import get_current_user_id
from openai_service import content_moderator

router = APIRouter()


def _validate_comment(comment: str | None) -> str | None:
    if not comment:
        return None
    text = comment.strip()
    if len(text) > 50:
        raise HTTPException(status_code=400, detail='评论最多50个字符')
    import re
    if not re.fullmatch(r"[\w\s\u4e00-\u9fa5]{0,50}", text):
        raise HTTPException(status_code=400, detail='评论仅允许字母数字中文与空格')
    return text


@router.get('/ratings/summary/{hero_id}', response_model=RatingSummary)
def get_rating_summary(hero_id: int, topic: str | None = None, db: Session = Depends(get_db)):
    hero = db.query(Hero).filter(Hero.id == hero_id).first()
    if not hero:
        raise HTTPException(status_code=404, detail='Hero not found')
    q = db.query(func.coalesce(func.avg(HeroRating.stars), 0.0), func.count(HeroRating.id)) \
        .filter(HeroRating.hero_id == hero_id) \
        .filter(HeroRating.status == 'approved')
    if topic:
        q = q.filter((HeroRating.topic == topic))
    avg, cnt = q.one()
    return RatingSummary(hero_id=hero_id, avg=float(avg or 0.0), count=int(cnt or 0), topic=topic)


@router.get('/ratings/{hero_id}', response_model=list[RatingItem])
def list_ratings(hero_id: int, topic: str | None = None, db: Session = Depends(get_db)):
    hero = db.query(Hero).filter(Hero.id == hero_id).first()
    if not hero:
        raise HTTPException(status_code=404, detail='Hero not found')
    q = db.query(HeroRating, User).join(User, HeroRating.user_id == User.id) \
        .filter(HeroRating.hero_id == hero_id) \
        .filter(HeroRating.status == 'approved')
    if topic:
        q = q.filter((HeroRating.topic == topic))
    # 返回该英雄在指定话题下的全部评分与评论（按时间倒序）
    # 之前为了安全做了 limit(50)，现在根据需求改为完整列表
    items = q.order_by(HeroRating.created_at.desc()).all()
    return [RatingItem(
        id=rating.id, user_id=rating.user_id, hero_id=rating.hero_id, stars=rating.stars,
        comment=rating.comment, created_at=rating.created_at.isoformat(), topic=rating.topic,
        username=user.username, avatar_url=user.avatar_url
    ) for rating, user in items]


@router.post('/ratings', response_model=RatingItem)
async def create_or_update_rating(payload: RatingCreate, authorization: str = Header(...), db: Session = Depends(get_db)):
    # 需要登录
    try:
        token = authorization.replace('Bearer ', '')
        user_id = get_current_user_id(token)
    except ValueError:
        raise HTTPException(status_code=401, detail='Invalid token')

    if payload.stars < 0 or payload.stars > 5:
        raise HTTPException(status_code=400, detail='星级需在0-5之间')
    text = _validate_comment(payload.comment)

    hero = db.query(Hero).filter(Hero.id == payload.hero_id).first()
    if not hero:
        raise HTTPException(status_code=404, detail='Hero not found')
    
    # 对评论内容进行AI审核
    moderation_status = 'approved'  # 默认通过
    if text:  # 如果有评论内容，进行审核
        moderation_status = await content_moderator.moderate_comment_with_gpt(text)
        if moderation_status == 'rejected':
            raise HTTPException(status_code=400, detail='评论内容不符合社区规范，请修改后重新提交')

    # 每个用户对同一英雄同一话题仅一条评分；旧库可能只有 (user,hero) 唯一约束
    topic = (payload.topic or 'top_tank_strength')
    rating = (
        db.query(HeroRating)
        .filter(HeroRating.user_id == user_id, HeroRating.hero_id == payload.hero_id)
        .filter(HeroRating.topic == topic)
        .first()
    )
    if rating is None:
        # 兼容旧索引：退化为按 (user, hero) 定位旧记录并更新其 topic
        legacy = (
            db.query(HeroRating)
            .filter(HeroRating.user_id == user_id, HeroRating.hero_id == payload.hero_id)
            .first()
        )
        if legacy:
            rating = legacy
            rating.topic = topic
        else:
            rating = HeroRating(
                user_id=user_id,
                hero_id=payload.hero_id,
                topic=topic,
                stars=payload.stars,
                comment=text,
                status=moderation_status,
            )
            db.add(rating)

    # 更新字段（无论新建或覆盖）
    rating.stars = payload.stars
    rating.comment = text
    rating.status = moderation_status

    try:
        db.commit()
    except IntegrityError:
        # 极端并发或索引不一致下的兜底：回滚后强制更新现有 (user,hero) 记录
        db.rollback()
        existing = (
            db.query(HeroRating)
            .filter(HeroRating.user_id == user_id, HeroRating.hero_id == payload.hero_id)
            .first()
        )
        if not existing:
            # 若仍不存在，则作为全新插入重试一次
            rating = HeroRating(
                user_id=user_id,
                hero_id=payload.hero_id,
                topic=topic,
                stars=payload.stars,
                comment=text,
                status=moderation_status,
            )
            db.add(rating)
        else:
            existing.topic = topic
            existing.stars = payload.stars
            existing.comment = text
            existing.status = moderation_status
            rating = existing
        db.commit()
    db.refresh(rating)
    return RatingItem(
        id=rating.id, user_id=rating.user_id, hero_id=rating.hero_id,
        stars=rating.stars, comment=rating.comment,
        created_at=rating.created_at.isoformat(), topic=rating.topic
    )


