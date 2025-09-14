from fastapi import APIRouter, Depends, HTTPException
from sqlalchemy.orm import Session
from sqlalchemy import select
from typing import List
from db import get_db
from models import Hero, HeroDetail as HeroDetailModel, HeroSkill as HeroSkillModel, HeroStat as HeroStatModel, HeroSkin as HeroSkinModel
from schemas import HeroBrief, HeroFull, HeroDetail as HeroDetailSchema, HeroSkill as HeroSkillSchema, HeroStat as HeroStatSchema, HeroSkin as HeroSkinSchema

router = APIRouter()

VALID_ROUTES = {"top", "mid", "bottom", "jungle", "support"}
VALID_CLASSES = {"tank", "fighter", "assassin", "marksman", "mage"}

@router.get('/heroes', response_model=List[HeroBrief])
def list_heroes(filter_type: str | None = None, filter_value: str | None = None, db: Session = Depends(get_db)):
    q = db.query(Hero)
    if filter_type and filter_value:
        ft = filter_type.lower()
        fv = filter_value.lower()
        if ft == 'route':
            if fv not in VALID_ROUTES:
                raise HTTPException(status_code=400, detail='Invalid route')
            q = q.filter(Hero.role_route == fv)
        elif ft == 'class':
            if fv not in VALID_CLASSES:
                raise HTTPException(status_code=400, detail='Invalid class')
            q = q.filter(Hero.role_class == fv)
        else:
            raise HTTPException(status_code=400, detail='Invalid filter_type')
    heroes = q.order_by(Hero.id.asc()).all()
    return [HeroBrief(
        id=h.id,
        title=h.title,
        name=h.name,
        role_route=h.role_route,
        role_class=h.role_class,
        square_image_url=h.square_image_url,
    ) for h in heroes]

@router.get('/heroes/{hero_id}', response_model=HeroFull)
def get_hero_detail(hero_id: int, db: Session = Depends(get_db)):
    hero = db.query(Hero).filter(Hero.id == hero_id).first()
    if not hero:
        raise HTTPException(status_code=404, detail='Hero not found')

    d = hero.detail
    detail = None
    if d:
        detail = HeroDetailSchema(
            hero_id=d.hero_id,
            cover_image_url=d.cover_image_url,
            story=d.story,
            type=d.type,
            difficulty=d.difficulty,
            radar_damage=d.radar_damage,
            radar_utility=d.radar_utility,
            radar_mobility=d.radar_mobility,
            radar_control=d.radar_control,
            radar_toughness=d.radar_toughness,
            passive_name=d.passive_name,
            passive_result=d.passive_result,
            q_name=d.q_name,
            q_result=d.q_result,
            w_name=d.w_name,
            w_result=d.w_result,
            e_name=d.e_name,
            e_result=d.e_result,
            r_name=d.r_name,
            r_result=d.r_result,
            passive_image=d.passive_image,
            q_image=d.q_image,
            w_image=d.w_image,
            e_image=d.e_image,
            r_image=d.r_image,
        )

    skills = [
        HeroSkillSchema(
            id=s.id,
            key=s.key,
            name=s.name,
            description=s.description,
            icon_url=s.icon_url,
        ) for s in hero.skills
    ]

    skins_rows = (
        db.query(HeroSkinModel)
        .filter(HeroSkinModel.hero_id == hero_id)
        .order_by(HeroSkinModel.skin_index.asc())
        .all()
    )
    skins = [
        HeroSkinSchema(
            hero_id=s.hero_id,
            skin_index=s.skin_index,
            skin_name=s.skin_name,
            skin_image=s.skin_image,
        ) for s in skins_rows
    ]

    stats_rows = db.query(HeroStatModel).filter(HeroStatModel.hero_id == hero_id).order_by(HeroStatModel.level.asc()).all()
    stats = [
        HeroStatSchema(
            level=st.level,
            attack=st.attack,
            attack_speed=float(st.attack_speed) if st.attack_speed is not None else None,
            attack_range=st.attack_range,
            move_speed=st.move_speed,
            health=st.health,
            mana=st.mana,
            armor=st.armor,
            mr=st.mr,
        ) for st in stats_rows
    ]

    return HeroFull(
        id=hero.id,
        title=hero.title,
        name=hero.name,
        role_route=hero.role_route,
        role_class=hero.role_class,
        square_image_url=hero.square_image_url,
        detail=detail,
        skins=skins,
        skills=skills,
        stats=stats,
    ) 