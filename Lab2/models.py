from sqlalchemy import Table, Column, Integer, String, MetaData, ForeignKey, UniqueConstraint

metadata = MetaData()

Promotion = Table(
    'promotion', metadata,
    Column('id', Integer, primary_key=True),
    Column('name', String)
)

PromotionPresident = Table(
    'promotion_president', metadata,
    Column('promotion_id', None, ForeignKey('promotion.id', ondelete='CASCADE'), unique=True),
    Column('name', String, nullable=False)
)

Fighter = Table(
    'fighter', metadata,
    Column('id', Integer, primary_key=True),
    Column('name', String, nullable=False),
    Column('wins', Integer, nullable=False),
    Column('losses', Integer, nullable=False),
    Column('draws', Integer, nullable=False)
)

WeightClass = Table(
    'weight_class', metadata,
    Column('id', Integer, primary_key=True),
    Column('promotion_id', None, ForeignKey('promotion.id', ondelete='CASCADE')),
    Column('name', String, nullable=False),
    Column('weight_limit', Integer, nullable=False)
)

FighterWeightClass = Table(
    'fighter_weight_class', metadata,
    Column('fighter_id', None, ForeignKey('fighter.id', ondelete='CASCADE')),
    Column('weight_class_id', None, ForeignKey('weight_class.id', ondelete='CASCADE')),
    UniqueConstraint('fighter_id', 'weight_class_id')
)
