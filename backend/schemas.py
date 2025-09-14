from pydantic import BaseModel
from typing import List


class AuthRequest(BaseModel):
    username: str
    password: str


class AuthResponse(BaseModel):
    token: str
    username: str
    avatar_url: str | None = None


class HeroBrief(BaseModel):
    id: int
    title: str
    name: str
    role_route: str
    role_class: str
    square_image_url: str | None = None


class UserProfile(BaseModel):
    id: int
    username: str
    level: int
    avatar_url: str | None = None


class AvatarUploadResponse(BaseModel):
    avatar_url: str
    message: str

# 新增：英雄详情相关模型
class HeroSkill(BaseModel):
    id: int
    key: str
    name: str
    description: str | None = None
    icon_url: str | None = None


class HeroDetail(BaseModel):
    hero_id: int
    cover_image_url: str | None = None
    story: str | None = None
    type: int | None = None
    difficulty: int | None = None
    radar_damage: int | None = None
    radar_utility: int | None = None
    radar_mobility: int | None = None
    radar_control: int | None = None
    radar_toughness: int | None = None
    passive_name: str | None = None
    passive_result: str | None = None
    q_name: str | None = None
    q_result: str | None = None
    w_name: str | None = None
    w_result: str | None = None
    e_name: str | None = None
    e_result: str | None = None
    r_name: str | None = None
    r_result: str | None = None
    passive_image: str | None = None
    q_image: str | None = None
    w_image: str | None = None
    e_image: str | None = None
    r_image: str | None = None


class HeroSkin(BaseModel):
    hero_id: int
    skin_index: int
    skin_name: str
    skin_image: str


class HeroStat(BaseModel):
    level: int
    attack: int | None = None
    attack_speed: float | None = None
    attack_range: int | None = None
    move_speed: int | None = None
    health: int | None = None
    mana: int | None = None
    armor: int | None = None
    mr: int | None = None


class HeroFull(BaseModel):
    id: int
    title: str
    name: str
    role_route: str
    role_class: str
    square_image_url: str | None = None

    detail: HeroDetail | None = None
    skins: List[HeroSkin] = []
    skills: List[HeroSkill] = []
    stats: List[HeroStat] = [] 


class RatingCreate(BaseModel):
    hero_id: int
    stars: int  # 0-5
    comment: str | None = None  # 最多50字符，仅字母数字与空格
    topic: str | None = None  # 话题键，默认固定话题


class RatingItem(BaseModel):
    id: int
    user_id: int
    hero_id: int
    stars: int
    comment: str | None = None
    created_at: str
    topic: str | None = None
    # 用户信息
    username: str | None = None
    avatar_url: str | None = None


class RatingSummary(BaseModel):
    hero_id: int
    avg: float
    count: int
    topic: str | None = None