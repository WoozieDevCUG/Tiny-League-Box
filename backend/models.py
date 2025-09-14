from sqlalchemy import Column, Integer, String, Boolean, Text, ForeignKey
from sqlalchemy.orm import relationship
from db import Base
from sqlalchemy import DateTime, func

class User(Base):
    __tablename__ = 'users'
    id = Column(Integer, primary_key=True, index=True)
    username = Column(String(64), unique=True, nullable=False, index=True)
    password_hash = Column(String(256), nullable=False)
    is_admin = Column(Boolean, default=False, nullable=False)
    level = Column(Integer, default=1, nullable=False)
    avatar_url = Column(String(512), nullable=True)

class Hero(Base):
    __tablename__ = 'heroes'
    id = Column(Integer, primary_key=True)
    title = Column(String(128), nullable=False)
    name = Column(String(128), nullable=False)
    role_route = Column(String(16), nullable=False)
    role_class = Column(String(16), nullable=False)
    square_image_url = Column(Text, nullable=True)

    detail = relationship('HeroDetail', back_populates='hero', uselist=False, cascade='all, delete')
    skills = relationship('HeroSkill', back_populates='hero', cascade='all, delete')
    skins = relationship('HeroSkin', back_populates='hero', cascade='all, delete')

class HeroDetail(Base):
    __tablename__ = 'hero_details'
    hero_id = Column(Integer, ForeignKey('heroes.id', ondelete='CASCADE'), primary_key=True)
    cover_image_url = Column(Text)
    story = Column(Text)
    type = Column(Integer)
    difficulty = Column(Integer)
    radar_damage = Column(Integer)
    radar_utility = Column(Integer)
    radar_mobility = Column(Integer)
    radar_control = Column(Integer)
    radar_toughness = Column(Integer)
    passive_name = Column(Text)
    passive_result = Column(Text)
    q_name = Column(Text)
    q_result = Column(Text)
    w_name = Column(Text)
    w_result = Column(Text)
    e_name = Column(Text)
    e_result = Column(Text)
    r_name = Column(Text)
    r_result = Column(Text)
    passive_image = Column(Text)
    q_image = Column(Text)
    w_image = Column(Text)
    e_image = Column(Text)
    r_image = Column(Text)

    hero = relationship('Hero', back_populates='detail')

class HeroStat(Base):
    __tablename__ = 'hero_stats'
    hero_id = Column(Integer, ForeignKey('heroes.id', ondelete='CASCADE'), primary_key=True)
    level = Column(Integer, primary_key=True)
    attack = Column(Integer)
    attack_speed = Column(Integer)
    attack_range = Column(Integer)
    move_speed = Column(Integer)
    health = Column(Integer)
    mana = Column(Integer)
    armor = Column(Integer)
    mr = Column(Integer)

class HeroSkill(Base):
    __tablename__ = 'hero_skills'
    id = Column(Integer, primary_key=True, index=True)
    hero_id = Column(Integer, ForeignKey('heroes.id', ondelete='CASCADE'))
    key = Column(String(8), nullable=False)
    name = Column(String(128), nullable=False)
    description = Column(Text)
    icon_url = Column(Text)

    hero = relationship('Hero', back_populates='skills') 


class HeroSkin(Base):
    __tablename__ = 'hero_skins'
    hero_id = Column(Integer, ForeignKey('heroes.id', ondelete='CASCADE'), primary_key=True, index=True, nullable=False)
    skin_index = Column(Integer, primary_key=True, nullable=False)
    skin_name = Column(String(128), nullable=False)
    skin_image = Column(Text, nullable=False)

    hero = relationship('Hero', back_populates='skins')


class HeroRating(Base):
    __tablename__ = 'hero_ratings'
    id = Column(Integer, primary_key=True, index=True)
    user_id = Column(Integer, ForeignKey('users.id', ondelete='CASCADE'), nullable=False)
    hero_id = Column(Integer, ForeignKey('heroes.id', ondelete='CASCADE'), nullable=False)
    topic = Column(String(64), nullable=True)
    stars = Column(Integer, nullable=False)
    comment = Column(String(50))
    status = Column(String(20), default='pending', nullable=False)
    created_at = Column(DateTime, server_default=func.now(), nullable=False)